#ifndef A_LANG_3AC_HPP
#define A_LANG_3AC_HPP

#include <assert.h>
#include <list>
#include <map>
#include <set>
#include <string.h>
#include "symbol_table.hpp"
#include "types.hpp"

namespace a_lang{

class TypeAnalysis;
class Procedure;
class IRProgram;
class ControlFlowGraph;
class ASTNode;

class Label{
public:
	Label(std::string nameIn){
		this->name = nameIn;
	}
	std::string toString(){
		return this->name;
	}
	std::string getName(){
		return name;
	}
private:
	std::string name;
};

enum Register{
	A, B, C, D, DI, SI
};

class RegUtils{
public:
	static std::string rootStr(Register reg){
		switch(reg){
			case A: return "a";
			case B: return "b";
			case C: return "c";
			case D: return "d";
			case DI: return "di";
			case SI: return "si";
		}
	}

	static std::string reg64(Register reg){
		switch(reg){
			case A: return "%rax";
			case B: return "%rbx";
			case C: return "%rcx";
			case D: return "%rdx";
			case DI: return "%rdi";
			case SI: return "%rsi";
		}
		throw new InternalError("no such register");
	}

	static std::string reg8(Register reg){
		switch(reg){
			case A: return "%al";
			case B: return "%bl";
			case C: return "%cl";
			case D: return "%dl";
			case DI: return "%dil";
			case SI: return "%sil";
		}
		throw new InternalError("no such register");
	}
};

class Opd{
public:
	Opd(size_t widthIn) : myWidth(widthIn), myIsFunction(false){}
	virtual std::string valString() = 0;
	virtual std::string locString() = 0;
	virtual size_t getWidth(){ return myWidth; }
	virtual void genLoadAddr(std::ostream& out, Register reg) = 0;
	virtual void genStoreAddr(std::ostream& out, Register reg) = 0;
	virtual void genLoadVal(std::ostream& out, Register reg) = 0;
	virtual void genStoreVal(std::ostream& out, Register reg) = 0;
	static size_t width(const DataType * type){
		if (const BasicType * basic = type->asBasic()){
			return basic->getSize();
		}
		if (type->asFn()){
			return 8;
		}
		return type->getSize();
	}
	virtual std::string getMovOp(){
		switch(myWidth){
			case 1: return "movb ";
			case 8: return "movq ";
		}

		throw new InternalError("Bad mov width");
	}
	std::string getReg(Register reg){
		switch(myWidth){
			case 1: return RegUtils::reg8(reg);
			case 8: return RegUtils::reg64(reg);
		}
		throw new InternalError("Bad getReg width");
	}
	bool isFunction(){ return myIsFunction; }
	void setIsFunction(bool isFnIn){ myIsFunction = isFnIn; }
	virtual std::string getMemoryLoc() = 0;
private:
	size_t myWidth;
	bool myIsFunction;
};

class SymOpd : public Opd{
public:
	virtual std::string valString() override{
		return "[" + mySym->getName() + "]";
	}
	virtual std::string locString() override{
		return mySym->getName();
	}
	virtual std::string getName(){
		return mySym->getName();
	}
	const SemSymbol * getSym(){ return mySym; }
	virtual void genLoadVal(std::ostream& out, Register reg) override;
	virtual void genStoreVal(std::ostream& out, Register reg) override;
	virtual void genLoadAddr(std::ostream& out, Register reg) override;
	virtual void genStoreAddr(std::ostream& out, Register reg) override{
		throw new InternalError("Cannot change the addr of a symOpd");
	}
	virtual void setMemoryLoc(std::string loc){
		myLoc = loc;
	}
	virtual std::string getMemoryLoc() override{
		return myLoc;
	}
private:
	//Private Constructor
	SymOpd(SemSymbol * sym, size_t width)
	: Opd(width), mySym(sym) {}
	SemSymbol * mySym;
	friend class Procedure;
	friend class IRProgram;
	std::string myLoc;
};

class LitOpd : public Opd{
public:
	LitOpd(std::string valIn, size_t width)
	: Opd(width), val(valIn){ }
	static LitOpd * buildInt(int val){
		/*
		if (val < 256){
			return new LitOpd(std::to_string(val), 1);
		} else {
		*/
			return new LitOpd(std::to_string(val), 8);
		/*
		}
		*/
	}
	static LitOpd * buildBool(bool val){
		if (val){ return new LitOpd("1", 1); }
		else { return new LitOpd("0", 1); }
	}

	virtual std::string valString() override{
		return val;
	}
	virtual std::string locString() override{
		throw InternalError("Tried to get location of a constant");
	}
	virtual void genLoadVal(std::ostream& out, Register reg) override;
	virtual void genStoreVal(std::ostream& out, Register reg) override{
		throw new InternalError("Cannot change value of a literal");
	}
	virtual void genLoadAddr(std::ostream& out, Register reg) override{
		throw new InternalError("Cannot get addr of a literal");
	}
	virtual void genStoreAddr(std::ostream& out, Register reg) override{
		throw new InternalError("Cannot set the addr of a literal");
	}

	virtual std::string getMemoryLoc() override{
		throw InternalError("Tried to get location of a constant");
	}
private:
	std::string val;
};

class AuxOpd : public Opd{
public:
	AuxOpd(std::string nameIn, size_t width)
	: Opd(width), name(nameIn) { }
	virtual std::string valString() override{
		return "[" + getName() + "]";
	}
	virtual std::string locString() override{
		return getName();
	}
	std::string getName(){
		return name;
	}
	virtual void genLoadVal(std::ostream& out, Register reg) override;
	virtual void genStoreVal(std::ostream& out, Register reg) override;
	virtual void genLoadAddr(std::ostream& out, Register reg) override;
	virtual void genStoreAddr(std::ostream& out, Register reg) override{
		throw new InternalError("Cannot change the addr of a auxOpd");
	}

	virtual void setMemoryLoc(std::string loc){
		myLoc = loc;
	}
	virtual std::string getMemoryLoc() override{
		return myLoc;
	}

private:
	std::string name;
	std::string myLoc = "UNINIT";
};

class AddrOpd : public Opd{
public:
	AddrOpd(std::string nameIn, size_t width)
	: Opd(width), name(nameIn) { }
	virtual std::string valString() override{
		return "[[" + getName() + "]]";
	}
	virtual std::string locString() override{
		return "[" + getName() + "]";
	}
	virtual void genLoadAddr(std::ostream& out, Register reg) override;
	virtual void genStoreAddr(std::ostream& out, Register reg) override;
	virtual void genLoadVal(std::ostream& out, Register reg) override;
	virtual void genStoreVal(std::ostream& out, Register reg) override;

	virtual void setMemoryLoc(std::string loc){
		myLoc = loc;
	}
	virtual std::string getMemoryLoc() override{
		return myLoc;
	}
	virtual std::string getName(){
		return name;
	}
private:
	std::string val;
	std::string name;
	std::string myLoc;
};

enum BinOp {
	ADD64, SUB64, DIV64, MULT64, EQ64, NEQ64, LT64, GT64, LTE64, GTE64,
	OR64,   AND64,
	ADD8,  SUB8,  DIV8,  MULT8,  EQ8,  NEQ8,  LT8,  GT8,  LTE8,  GTE8,
	OR8,   AND8
};
enum UnaryOp{
	NEG64, NEG8, NOT64, NOT8
};

std::string binOpToX64(BinOp opr);
Register indexToReg(size_t index);

class Quad{
public:
	Quad();
	void addLabel(Label * label);
	Label * getLabel(){ return labels.front(); }
	void clearLabels(){ labels.clear(); }
	virtual std::string repr() = 0;
	std::string commentStr();
	virtual std::string toString(bool verbose=false);
	void setComment(std::string commentIn);
	virtual void codegenX64(std::ostream& out) = 0;
	void codegenLabels(std::ostream& out);
private:
	std::string myComment;
	std::list<Label *> labels;
};

class BinOpQuad : public Quad{
public:
	BinOpQuad(Opd * dstIn, BinOp oprIn, Opd * src1In, Opd * src2In);
	std::string repr() override;
	static std::string oprString(BinOp opr);
	void codegenX64(std::ostream& out) override;
	Opd * getDst(){ return dst; }
	Opd * getSrc1(){ return src1; }
	Opd * getSrc2(){ return src2; }
	BinOp getOp(){ return opr; }
private:
	Opd * dst;
	BinOp opr;
	Opd * src1;
	Opd * src2;
};

class UnaryOpQuad : public Quad {
public:
	UnaryOpQuad(Opd * dstIn, UnaryOp opIn, Opd * srcIn);
	std::string repr() override ;
	void codegenX64(std::ostream& out) override;
	Opd * getDst(){ return dst; }
	Opd * getSrc(){ return src; }
	UnaryOp getOp(){ return op; }
private:
	Opd * dst;
	UnaryOp op;
	Opd * src;
};

class AssignQuad : public Quad{
public:
	AssignQuad(Opd * dstIn, Opd * srcIn);
	std::string repr() override;
	void codegenX64(std::ostream& out) override;
	Opd * getDst(){ return dst; }
	Opd * getSrc(){ return src; }
private:
	Opd * dst;
	Opd * src;
};



class LocQuad : public Quad {
public:
	LocQuad(Opd * srcIn, Opd * tgtIn, bool srcLocIn, bool tgtLocIn)
	: src(srcIn), tgt(tgtIn), srcIsLoc(srcLocIn), tgtIsLoc(tgtLocIn){ }
	std::string repr() override;
	void codegenX64(std::ostream& out) override;
private:
	Opd * src;
	Opd * tgt;
	bool srcIsLoc;
	bool tgtIsLoc;
};

class GotoQuad : public Quad {
public:
	GotoQuad(Label * tgtIn);
	std::string repr() override;
	void codegenX64(std::ostream& out) override;
	Label * getTarget(){ return tgt; }
private:
	Label * tgt;
};

class IfzQuad : public Quad {
public:
	IfzQuad(Opd * cndIn, Label * tgtIn);
	std::string repr() override;
	Label * getTarget(){ return tgt; }
	Opd * getCnd(){ return cnd; }
	void codegenX64(std::ostream& out) override;
private:
	Opd * cnd;
	Label * tgt;
};

class NopQuad : public Quad {
public:
	NopQuad();
	std::string repr() override;
	void codegenX64(std::ostream& out) override;
};

class WriteQuad : public Quad {
public:
	WriteQuad(Opd * src, const DataType * type);
	std::string repr() override;
	Opd * getSrc(){ return mySrc; }
	const DataType * getType(){ return mySrcType; }
	void codegenX64(std::ostream& out) override;
private:
	Opd * mySrc;
	const DataType * mySrcType;
};

class ReadQuad : public Quad {
public:
	ReadQuad(Opd* dst, const DataType * dstType);
	std::string repr() override;
	Opd * getDst(){ return myDst; }
	const DataType * getType(){ return myDstType; }
	void codegenX64(std::ostream& out) override;
private:
	Opd * myDst;
	const DataType * myDstType;
};

class CallQuad : public Quad{
public:
	CallQuad(SemSymbol * calleeIn);
	std::string repr() override;
	void codegenX64(std::ostream& out) override;
private:
	Opd * calleeOpd;
	SemSymbol * sym;
};

class EnterQuad : public Quad{
public:
	EnterQuad(Procedure * proc);
	virtual std::string repr() override;
	void codegenX64(std::ostream& out) override;
private:
	Procedure * myProc;
};

class LeaveQuad : public Quad{
public:
	LeaveQuad(Procedure * proc);
	virtual std::string repr() override;
	void codegenX64(std::ostream& out) override;
private:
	Procedure * myProc;
};

class SetArgQuad : public Quad{
public:
	SetArgQuad(size_t indexIn, Opd * opdIn, const DataType * typeIn);
	std::string repr() override;
	void codegenX64(std::ostream& out) override;
	Opd * getSrc(){ return opd; }
	size_t getIndex(){ return index; }
	const DataType * getType(){ return type; }
private:
	size_t index;
	Opd * opd;
	const DataType * type;
};

class GetArgQuad : public Quad{
public:
	GetArgQuad(size_t indexIn, Opd * opdIn);
	std::string repr() override;
	void codegenX64(std::ostream& out) override;
	Opd * getDst(){ return opd; }
private:
	size_t index;
	Opd * opd;
};

class SetRetQuad : public Quad{
public:
	SetRetQuad(Opd * opdIn);
	std::string repr() override;
	Opd * getSrc(){ return opd; }
	void codegenX64(std::ostream& out) override;
private:
	Opd * opd;
};

class GetRetQuad : public Quad{
public:
	GetRetQuad(Opd * opdIn);
	std::string repr() override;
	Opd * getDst(){ return opd; }
	void codegenX64(std::ostream& out) override;
private:
	Opd * opd;
};

class Procedure{
public:
	Procedure(IRProgram * prog, std::string name);
	void addQuad(Quad * quad);
	Quad * popQuad();
	IRProgram * getProg();
	std::list<SymOpd *> getFormals() { return formals; }
	SymOpd * getFormal(size_t idx){
		auto itr = formals.begin();
		for (size_t i = 0; i < idx; i++, itr++);
		return *itr;
	}
	a_lang::Label * makeLabel();

	void gatherLocal(SemSymbol * sym);
	void gatherFormal(SemSymbol * sym);
	SymOpd * getSymOpd(SemSymbol * sym);
	AuxOpd * makeTmp(size_t width);
	AddrOpd * makeAddrOpd(size_t width);

	std::string toString(bool verbose=false);
	std::string getName();

	a_lang::Label * getLeaveLabel();

	void toX64(std::ostream& out);
	size_t arSize() const;
	size_t numTemps() const;
	int getAllocBytes() const { return allocBytes; }

	std::list<Quad *> * getQuads(){ return bodyQuads; }
	EnterQuad * getEnter(){ return enter; }
	LeaveQuad * getLeave(){ return leave; }
	void replaceQuad(Quad * oldQuad, Quad * newQuad);
private:
	void allocLocals();

	EnterQuad * enter;
	LeaveQuad * leave;
	Label * leaveLabel;

	IRProgram * myProg;
	std::map<SemSymbol *, SymOpd *> locals;
	std::list<AuxOpd *> temps;
	std::list<SymOpd *> formals;
	std::list<AddrOpd *> addrOpds;
	std::list<Quad *> * bodyQuads;
	std::string myName;
	size_t maxTmp;
	int allocBytes;
};

class IRProgram{
public:
	IRProgram(TypeAnalysis * taIn) : ta(taIn){
		procs = new std::list<Procedure *>();
		init = new Procedure(this, "<init>");
		TypeList * argsType = 
			TypeList::produce(new std::list<TypeNode *>());
		auto t = FnType::produce(argsType, BasicType::BOOL());
		randSym = new FnSymbol("randBool", t);
	}
	Procedure * makeProc(std::string name);
	std::list<Procedure *> * getProcs();
	Label * makeLabel();
	Opd * makeString(std::string val);
	void gatherGlobal(SemSymbol * sym);
	SymOpd * getGlobal(SemSymbol * sym);
	size_t opWidth(ASTNode * node);
	const DataType * nodeType(ASTNode * node);
	std::set<Opd *> globalSyms();
	std::string toString(bool verbose=false);

	void toX64(std::ostream& out);
	Procedure * getInitProc(){ return init; }
	SemSymbol * getRandSym(){ return randSym; }
private:
	TypeAnalysis * ta;
	size_t max_label = 0;
	size_t str_idx = 0;
	std::list<Procedure *> * procs;
	Procedure * init;
	SemSymbol * randSym;
	HashMap<LitOpd *, std::string> strings;
	std::map<SemSymbol *, SymOpd *> globals;

	void datagenX64(std::ostream& out);
	void allocGlobals();
};

}

#endif

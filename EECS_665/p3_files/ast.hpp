#ifndef A_LANG_AST_HPP
#define A_LANG_AST_HPP

#include <ostream>
#include <list>
#include "tokens.hpp"
#include <cassert>


/* You'll probably want to add a bunch of ASTNode subclasses */

namespace a_lang{

/* You may find it useful to forward declare AST subclasses
   here so that you can use a class before it's full definition
*/
class DeclNode;
class TypeNode;
class StmtNode;
class IDNode;

/** 
* \class ASTNode
* Base class for all other AST Node types
**/
class ASTNode{
public:
	ASTNode(const Position * p) : myPos(p){ }
	virtual void unparse(std::ostream& out, int indent) = 0;
	const Position * pos() { return myPos; }
	std::string posStr() { return pos()->span(); }
protected:
	const Position * myPos = nullptr;
};

/** 
* \class ProgramNode
* Class that contains the entire abstract syntax tree for a program.
* Note the list of declarations encompasses all global declarations
* which includes (obviously) all global variables and struct declarations
* and (perhaps less obviously), all function declarations
**/
class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn) ;
	void unparse(std::ostream& out, int indent) override;
private:
	std::list<DeclNode * > * myGlobals;
};

class StmtNode : public ASTNode{
public:
	StmtNode(const Position * p) : ASTNode(p){ }
	void unparse(std::ostream& out, int indent) override = 0;
};



/** \class DeclNode
* Superclass for declarations (i.e. nodes that can be used to 
* declare a struct, function, variable, etc).  This base class will 
**/
class DeclNode : public StmtNode{
public:
	DeclNode(const Position * p) : StmtNode(p) { }
	void unparse(std::ostream& out, int indent) override = 0;
};




/**  \class ExpNode
* Superclass for expression nodes (i.e. nodes that can be used as
* part of an expression).  Nodes that are part of an expression
* should inherit from this abstract superclass.
**/
class ExpNode : public ASTNode{
protected:
	ExpNode(const Position * p) : ASTNode(p){ }

};


/**  \class TypeNode
* Superclass of nodes that indicate a data type. For example, in 
* the declaration "int a", the int part is the type node (a is an IDNode
* and the whole thing is a DeclNode).
**/
class TypeNode : public ASTNode{
protected:
	TypeNode(const Position * p) : ASTNode(p){
	}
public:
	virtual void unparse(std::ostream& out, int indent) = 0;
};

class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(const Position * p) : TypeNode(p){ }
	void unparse(std::ostream& out, int indent);
};

class VoidTypeNode : public TypeNode {
public:
	VoidTypeNode(const Position * p) : TypeNode(p) { }
	void unparse(std::ostream& out, int indent);
};

class ImmutableTypeNode : public TypeNode {
public:
	ImmutableTypeNode(const Position * p, TypeNode * type) : TypeNode(p), myType(type) { }
	void unparse(std::ostream& out, int indent);

private:
	TypeNode * myType;
};

class ClassTypeNode : public TypeNode {
public:
	ClassTypeNode(const Position * p, DeclNode * Decl) : TypeNode(p), myDecl(Decl) {}
	void unparse(std::ostream& out, int indent);
private:
	DeclNode * myDecl;
};

class ClassDefnNode : public DeclNode {
public:
	ClassDefnNode(const Position * p, IDNode * ID, std::list<DeclNode *> * DeclList) : DeclNode(p), myID(ID), myDeclList(DeclList) {}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myID;
	std::list<DeclNode *> * myDeclList;
};

class BinaryExpNode : public ExpNode {
public:
	BinaryExpNode(const Position * p): ExpNode(p){ }
	void unparse(std::ostream& out, int indent) override = 0;

};

class MinusNode : public BinaryExpNode {
public:
	MinusNode(const Position * p, ExpNode * left, ExpNode * right): BinaryExpNode(p), myLeft(left), myRight(right){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class PlusNode : public BinaryExpNode {
public:
	PlusNode(const Position * p, ExpNode * left, ExpNode * right): BinaryExpNode(p), myLeft(left), myRight(right){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class MultNode : public BinaryExpNode {
public:
	MultNode(const Position * p, ExpNode * left, ExpNode * right): BinaryExpNode(p), myLeft(left), myRight(right){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class DivNode : public BinaryExpNode {
public:
	DivNode(const Position * p, ExpNode * left, ExpNode * right): BinaryExpNode(p), myLeft(left), myRight(right){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class AndNode : public BinaryExpNode {
public:
	AndNode(const Position * p, ExpNode * left, ExpNode * right): BinaryExpNode(p), myLeft(left), myRight(right){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class OrNode : public BinaryExpNode {
public:
	OrNode(const Position * p, ExpNode * left, ExpNode * right): BinaryExpNode(p), myLeft(left), myRight(right){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class EqualsNode : public BinaryExpNode {
public:
	EqualsNode(const Position * p, ExpNode * left, ExpNode * right): BinaryExpNode(p), myLeft(left), myRight(right){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class NotEqualsNode : public BinaryExpNode {
public:
	NotEqualsNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p), myLeft(left), myRight(right) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class GreaterNode : public BinaryExpNode {
public:
	GreaterNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p), myLeft(left), myRight(right) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class GreaterEqNode : public BinaryExpNode {
public:
	GreaterEqNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p), myLeft(left), myRight(right) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class LessNode : public BinaryExpNode {
public:
	LessNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p), myLeft(left), myRight(right) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class LessEqNode : public BinaryExpNode {
public:
	LessEqNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p), myLeft(left), myRight(right) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLeft;
	ExpNode * myRight;
};

class UnaryExpNode : public ExpNode {
public:
	UnaryExpNode(const Position * p) : ExpNode(p) {}
	void unparse(std::ostream& out, int indent) override = 0;
};

class NegNode : public UnaryExpNode {
public:
	NegNode(const Position * p, ExpNode * exp) : UnaryExpNode(p), myExp(exp) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
};

class NotNode : public UnaryExpNode {
public:
	NotNode(const Position * p, ExpNode * exp) : UnaryExpNode(p), myExp(exp) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
};


class IntLitNode : public ExpNode {
public:
	IntLitNode(const Position * p, IntLitToken * token) : ExpNode(p), myInt(token->num()) {
		myInt = token->num();
	}
	void unparse(std::ostream& out, int indent);
private:
	int myInt;
};

class StrLitNode : public ExpNode {
public:
	StrLitNode(const Position * p, StrToken * token) : ExpNode(p), myStr(token->str()) {
		myStr = token->str();
	}
	void unparse(std::ostream& out, int indent);
private:
	std::string myStr;
};

/** A memory location. LocNodes subclass ExpNode
 * because they can be used as part of an expression. 
**/
class LocNode : public ExpNode{
public:
	LocNode(const Position * p)
	: ExpNode(p) {}
	void unparse(std::ostream& out, int indent) = 0;
};

/** An identifier. Note that IDNodes subclass
 * LocNode because they are a type of memory location. 
**/
class IDNode : public LocNode{
public:
	IDNode(const Position * p, std::string nameIn) 
	: LocNode(p), name(nameIn){ }
	void unparse(std::ostream& out, int indent);
private:
	/** The name of the identifier **/
	std::string name;
};

class MemberFieldExpNode : LocNode {
public:
	MemberFieldExpNode(const Position * p) : LocNode(p) { }
};
/** A variable declaration.
**/
class VarDeclNode : public DeclNode{
public:
	VarDeclNode(const Position * p, IDNode * inID, TypeNode * inType) 
	: DeclNode(p), myID(inID), myType(inType){
		assert (myType != nullptr);
		assert (myID != nullptr);
	}
	VarDeclNode(const Position * p, IDNode * inID, TypeNode * inType, ExpNode * exp) : DeclNode(p), myID(inID), myType(inType), myExp(exp){
		assert (myType != nullptr);
		assert (myID != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myID;
	TypeNode * myType;
	ExpNode * myExp;
};

class FormalDeclNode : public VarDeclNode {
public:
	FormalDeclNode(const Position * p, IDNode * inID, TypeNode * inType) : VarDeclNode(p, inID, inType) {
		myID = inID;
		myType = inType;
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myID;
	TypeNode * myType;
};

class FnDeclNode : public DeclNode {
public:
	FnDeclNode(const Position * p, IDNode * id, std::list<a_lang::FormalDeclNode *> * maybeFormals, TypeNode * type, std::list<a_lang::StmtNode *> * stmtList)
	: DeclNode(p), myID(id), myMaybeFormals(maybeFormals), myType(type), myStmtList(stmtList) {}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myID;
	std::list<a_lang::FormalDeclNode *> * myMaybeFormals;
	TypeNode * myType;
	std::list<a_lang::StmtNode *> * myStmtList;
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(const Position * p) : TypeNode(p){ }
	void unparse(std::ostream& out, int indent);
};

class AssignStmtNode : public StmtNode {
public:
	AssignStmtNode(const Position * p, LocNode * loc, ExpNode * exp) : StmtNode(p), myLoc(loc), myExp(exp) {}
	void unparse(std::ostream& out, int indent);
private:
	LocNode * myLoc;
	ExpNode * myExp;
};

class PostDecNode : public StmtNode {
public:
	PostDecNode(const Position * p, LocNode * loc) : StmtNode(p), myLoc(loc){}
	void unparse(std::ostream& out, int indent);
private:
	LocNode * myLoc;
};

class PostIncNode : public StmtNode {
public:
	PostIncNode(const Position * p, LocNode * loc) : StmtNode(p), myLoc(loc){}
	void unparse(std::ostream& out, int indent);
private:
	LocNode * myLoc;
};

class ToConsoleNode : public StmtNode {
public:
	ToConsoleNode(const Position * p, ExpNode * exp) : StmtNode(p), myExp(exp) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
};

class FromConsoleNode : public StmtNode {
public:
	FromConsoleNode(const Position * p, LocNode * loc) : StmtNode(p), myLoc(loc) {}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myLoc;
};

class WhileStmtNode : public StmtNode {
public:
	WhileStmtNode(const Position * p, ExpNode * exp, std::list<StmtNode *> * stmtList) : StmtNode(p), myExp(exp), myStmtList(stmtList){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
	std::list<StmtNode *> * myStmtList;
};

class IfStmtNode : public StmtNode {
public:
	IfStmtNode(const Position * p, ExpNode * exp, std::list<StmtNode *> * stmtList): StmtNode(p), myExp(exp), myStmtList(stmtList){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
	std::list<StmtNode *> * myStmtList;
};

class IfElseStmtNode : public StmtNode {
public:
	IfElseStmtNode(const Position * p, ExpNode * exp, std::list<StmtNode *> * stmtList1, std::list<StmtNode *> * stmtList2) : StmtNode(p), myExp(exp), myStmtList1(stmtList1), myStmtList2(stmtList2){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
	std::list<StmtNode *> * myStmtList1;
	std::list<StmtNode *> * myStmtList2;
};

class MaybeStmtNode : public StmtNode {
public:
	MaybeStmtNode(const Position * p, ExpNode * exp1, ExpNode * exp2, ExpNode * exp3): StmtNode(p), myExp1(exp1), myExp2(exp2), myExp3(exp3){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp1;
	ExpNode * myExp2;
	ExpNode * myExp3;
};

class ReturnStmtNode : public StmtNode {
public:
	ReturnStmtNode(const Position * p) : StmtNode(p){}
	ReturnStmtNode(const Position * p, ExpNode * exp) : StmtNode(p), myExp(exp){}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
};

class TrueNode : public ExpNode {
public:
	TrueNode(const Position * p) : ExpNode(p){}
	void unparse(std::ostream& out, int indent);
};

class FalseNode : public ExpNode {
public:
	FalseNode(const Position * p) : ExpNode(p){}
	void unparse(std::ostream& out, int indent);
};

class EhNode : public ExpNode {
public:
	EhNode(const Position * p) : ExpNode(p){}
	void unparse(std::ostream& out, int indent);
};

class CallExpNode : public ExpNode {
public:
	CallExpNode(const Position * p, LocNode * loc) : ExpNode(p), myLoc(loc){ }
	CallExpNode(const Position * p, LocNode * loc, std::list<a_lang::ExpNode *> * list) : ExpNode(p), myLoc(loc), myList(list){}
	void unparse(std::ostream& out, int indent);
private:
	LocNode * myLoc;
	std::list<a_lang::ExpNode *> * myList;
};

class CallStmtNode : public StmtNode {
public:
	CallStmtNode(const Position * p, CallExpNode * callExp) : StmtNode(p), myCallExp(callExp){}
	void unparse(std::ostream& out, int indent);
private:
	CallExpNode * myCallExp;
};

class RefTypeNode : public TypeNode {
public:
	RefTypeNode(const Position * p, IDNode * ID) : TypeNode(p), myID(ID) {}
	RefTypeNode(const Position * p, TypeNode * type) : TypeNode(p), myType(type) {}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myID;
	TypeNode * myType;
};

} //End namespace a_lang

#endif

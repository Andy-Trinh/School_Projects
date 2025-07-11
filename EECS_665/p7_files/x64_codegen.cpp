#include <ostream>
#include "3ac.hpp"

namespace a_lang{

void IRProgram::allocGlobals(){
	//Choose a label for each global
	for (auto pair : globals) {
		SymOpd * opd = pair.second;
		std::string s = "gbl_" + opd->getSym()->getName();
		opd->setMemoryLoc(s);
	}

}

void IRProgram::datagenX64(std::ostream& out){
	out << ".data\n";
	for (auto pair : globals) {
		SymOpd * opd = pair.second;
		out << opd->getMemoryLoc()
			<< ": " 
			<< (opd->getWidth() == 8 ? ".byte" : ".quad")
			<< " 0\n";
	}
	
	for (auto pair : strings) {
		out << pair.first->getMemoryLoc()
			<< ": .asciz "
			<< pair.second
			<< "\n.align 8\n";
	}

}

void IRProgram::toX64(std::ostream& out){
	allocGlobals();
	datagenX64(out);
	// Iterate over each procedure and codegen it
	out << "\n.globl main\n.text\n\n";
	for (Procedure * proc : *procs) {
		proc->toX64(out);
		out << "\n";
	}
}

void Procedure::allocLocals(){
	//Allocate space for locals
	// Iterate over each procedure and codegen it
	int formals_size = formals.size();
	int locals_size = locals.size();
	int temps_size = temps.size();
	int offset;

	int i = 1;
	for (SymOpd * opd : formals) {
		offset = (i <= 6) ? (-16 - (8 * i)) : (8 * (formals_size - i));
		opd->setMemoryLoc(std::to_string(offset) + "(%rbp)");
		i++;
	}

	int n = formals_size < 7 ? formals_size : 6;

	i = 1;
	for (auto pair : locals) {
		offset = -16 - (8 * n) - (8 * i);
		pair.second->setMemoryLoc(std::to_string(offset) + "(%rbp)");
		i++;
	}

	i = 1;
	for (AuxOpd * opd : temps) {
		offset = -16 - (8 * formals_size) - (8 * locals_size) - (8 * i);
		opd->setMemoryLoc(std::to_string(offset) + "(%rbp)");
		i++;
	}

	int align = ((8 * formals_size) + (8 * locals_size) + (8 * temps_size)) % 16;
	allocBytes = align + (8 * n) + (8 * locals_size) + (8 * temps_size);
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();

	enter->codegenLabels(out);
	enter->codegenX64(out);
	out << "#Fn body " << myName << "\n";
	for (auto quad : *bodyQuads){
		quad->codegenLabels(out);
		out << "#" << quad->toString() << "\n";
		quad->codegenX64(out);
	}
	out << "#Fn epilogue " << myName << "\n";
	leave->codegenLabels(out);
	leave->codegenX64(out);
}

void Quad::codegenLabels(std::ostream& out){
	if (labels.empty()){ return; }

	size_t numLabels = labels.size();
	size_t labelIdx = 0;
	for ( Label * label : labels){
		out << label->getName() << ": ";
		if (labelIdx != numLabels - 1){ out << "\n"; }
		labelIdx++;
	}
}

void BinOpQuad::codegenX64(std::ostream& out){
	src1->genLoadVal(out, A);
	src2->genLoadVal(out, B);

	if (opr == MULT64 || opr == DIV64) {
		out << "xorq %rdx, %rdx\n";
		out << binOpToX64(opr) << " %rbx\n";
	} else if (opr == EQ64 || opr == NEQ64 || opr == GT64 || opr == GTE64 || opr == LT64 || opr == LTE64) {
		out << "cmpq %rbx, %rax\n" 
			<< binOpToX64(opr) << " " << "%al" << "\n";
	} else {
		out << binOpToX64(opr) << " %rbx, %rax\n";
	}
	
	dst->genStoreVal(out, A);
}

void UnaryOpQuad::codegenX64(std::ostream& out){
	src->genLoadVal(out, A);
	
	if (op == NOT64) {
		out <<"cmpq $0, %rax\n"
			<< "setz %al\n";
	} else if (op == NEG64) {
		out << "negq %rax\n";
	}

	dst->genStoreVal(out, A);
}

void AssignQuad::codegenX64(std::ostream& out){
	src->genLoadVal(out, A);
	dst->genStoreVal(out, A);
}

void ReadQuad::codegenX64(std::ostream& out){
	if (myDstType->isInt()) {
		out << "callq getInt\n";
	} else if (myDstType->isBool()) {
		out << "callq getBool\n";
	}

	myDst->genStoreVal(out, A);
}

void WriteQuad::codegenX64(std::ostream& out){
	mySrc->genLoadVal(out, DI);

	if (mySrcType->isInt()) {
		out << "callq printInt\n";
	} else if (mySrcType->isBool()) {
		out << "callq printBool\n";
	} else {
		out << "callq printString\n";
	}
}

void GotoQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->getName() << "\n";
}

void IfzQuad::codegenX64(std::ostream& out){
	out << "movq $0, %rax\n";
	cnd->genLoadVal(out, A);
	out << "cmp $0, %rax\n";
	out << "je " << tgt->toString() << "\n";
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

void CallQuad::codegenX64(std::ostream& out){
	out << "call fun_" << sym->getName() << "\n";
	int args = sym->getDataType()->asFn()->getFormalTypes()->getSize();

	if((args / 8) > 6) {
		int addback = 8 * (args - 6);
		out << "addq $" << addback << ", %rsp\n";
	}
}

void EnterQuad::codegenX64(std::ostream& out){
	out << "pushq %rbp\n"
		<< "movq %rsp, %rbp\n"
		<< "addq $16, %rbp\n" 
		<< "subq $" << myProc->getAllocBytes() << ", %rsp\n";
}

void LeaveQuad::codegenX64(std::ostream& out){
	out << "addq $" << myProc->getAllocBytes() << ", %rsp\n"
		<< "popq %rbp\n"
		<< "ret\n";
}

void SetArgQuad::codegenX64(std::ostream& out){
	if(index <= 6) {
		opd->genLoadVal(out, indexToReg(index));
	} else {
		opd->genLoadVal(out, A);
		out << "pushq %rax\n";
	}
}

void GetArgQuad::codegenX64(std::ostream& out){
	if(index <= 6) {
		opd->genStoreVal(out, indexToReg(index));
	}
}

void SetRetQuad::codegenX64(std::ostream& out){
	opd->genLoadVal(out, A);
}

void GetRetQuad::codegenX64(std::ostream& out){
	opd->genStoreVal(out, A);
}

void LocQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void SymOpd::genLoadVal(std::ostream& out, Register reg){
	out << this->getMovOp() << this->getMemoryLoc() << ", " << this->getReg(reg) << "\n";
}

void SymOpd::genStoreVal(std::ostream& out, Register reg){
	out << this->getMovOp() << this->getReg(reg) << ", " << this->getMemoryLoc() << "\n";	
}

void SymOpd::genLoadAddr(std::ostream& out, Register reg) {
	out << "movq %eax, $0\n"
		<< "leaq %eax, " << this->valString() << "\n";
}

void AuxOpd::genLoadVal(std::ostream& out, Register reg){
	out << this->getMovOp() << this->getMemoryLoc() << ", " << this->getReg(reg) << "\n";
}

void AuxOpd::genStoreVal(std::ostream& out, Register reg){
	out << this->getMovOp() << this->getReg(reg) << ", " << this->getMemoryLoc() << "\n";
}
void AuxOpd::genLoadAddr(std::ostream& out, Register reg){
	out << "movq %eax, $0\n"
		<< "leaq %eax, " << this->valString() << "\n";
}

void AddrOpd::genStoreVal(std::ostream& out, Register reg){
	out << this->getMovOp() << this->getReg(reg) << ", " << this->getMemoryLoc() << "\n";
}

void AddrOpd::genLoadVal(std::ostream& out, Register reg){
	out << this->getMovOp() << this->getReg(reg) << ", " << this->getMemoryLoc() << "\n";
}

void AddrOpd::genStoreAddr(std::ostream& out, Register reg){
	out << "movq %eax, $0\n"
		<< "leaq %eax, " << this->valString() << "\n";
}

void AddrOpd::genLoadAddr(std::ostream & out, Register reg){
	out << "movq %eax, " << this->locString() << "\n";

}

void LitOpd::genLoadVal(std::ostream & out, Register reg){
	out << getMovOp() << " $" << val << ", " << getReg(reg) << "\n";
}

std::string binOpToX64(BinOp opr) {
	switch(opr){
	case ADD64: return "addq";
	case SUB64: return "subq";
	case DIV64: return "idivq";
	case MULT64: return "imulq";
	case OR64: return "orq";
	case AND64: return "andq";
	case EQ64: return "sete";
	case NEQ64: return "negq";
	case LT64: return "setl";
	case GT64: return "setg";
	case LTE64: return "setle";
	case GTE64: return "setge";

	case ADD8: return "addb";  
	case SUB8: return "subb";  
	case DIV8: return "idivb";  
	case MULT8: return "imulb";  
	case OR8: return "orb";  
	case AND8: return "andb";  
	case EQ8: return "sete";  
	case NEQ8: return "negb";  
	case LT8: return "setl";  
	case GT8: return "setg";  
	case LTE8: return "setle";  
	case GTE8: return "setge";  
	} 
	throw new InternalError("Invalid op");
}

Register indexToReg(size_t index) {
	switch (index) {
		case 1: return DI;
		case 2: return SI;
		case 3: return D;
		case 4: return C;
		case 5: return A;
		case 6: return B;
		default: break;
	}
	throw new InternalError("Index out of range of target registers");
}
}

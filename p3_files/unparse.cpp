#include "ast.hpp"

namespace a_lang{

/*
doIndent is declared static, which means that it can 
only be called in this file (its symbol is not exported).
*/
static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

/*
In this code, the intention is that functions are grouped 
into files by purpose, rather than by class.
If you're used to having all of the functions of a class 
defined in the same file, this style may be a bit disorienting,
though it is legal. Thus, we can have
ProgramNode::unparse, which is the unparse method of ProgramNodes
defined in the same file as DeclNode::unparse, the unparse method
of DeclNodes.
*/


void ProgramNode::unparse(std::ostream& out, int indent){
	/* Oh, hey it's a for-each loop in C++!
	   The loop iterates over each element in a collection
	   without that gross i++ nonsense. 
	 */
	for (auto global : *myGlobals){
		/* The auto keyword tells the compiler
		   to (try to) figure out what the
		   type of a variable should be from 
		   context. here, since we're iterating
		   over a list of DeclNode *s, it's 
		   pretty clear that global is of 
		   type DeclNode *.
		*/
		global->unparse(out, indent);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	this->myID->unparse(out, 0);
	out << " : ";
	this->myType->unparse(out, 0);
	if (myExp != nullptr) {
		out << " = ";
		this->myExp->unparse(out, 0);
	}
	out << ";\n";

}

void IDNode::unparse(std::ostream& out, int indent){
	out << this->name;
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	out << "int";
}

void BoolTypeNode::unparse(std::ostream& out, int indent){
	out << "bool";
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
	out << "void";
}

void ImmutableTypeNode::unparse(std::ostream& out, int indent) {
	out << "immutable ";
	this->myType->unparse(out, 0);
}


void MinusNode::unparse(std::ostream& out, int indent){
	this->myLeft->unparse(out, 0);
	out << " - ";
	this->myRight->unparse(out, 0);
	
}

void PlusNode::unparse(std::ostream& out, int indent){
	this->myLeft->unparse(out, 0);
	out << " + ";
	this->myRight->unparse(out, 0);
	
}

void MultNode::unparse(std::ostream& out, int indent){
	this->myLeft->unparse(out, 0);
	out << " * ";
	this->myRight->unparse(out, 0);
	
}

void DivNode::unparse(std::ostream& out, int indent){
	this->myLeft->unparse(out, 0);
	out << " / ";
	this->myRight->unparse(out, 0);
	
}

void AndNode::unparse(std::ostream& out, int indent) {
	this->myLeft->unparse(out, 0);
	out << " and ";
	this->myRight->unparse(out, 0);
}

void OrNode::unparse(std::ostream& out, int indent) {
	this->myLeft->unparse(out, 0);
	out << " or ";
	this->myRight->unparse(out, 0);
}

void EqualsNode::unparse(std::ostream& out, int indent) {
	this->myLeft->unparse(out, 0);
	out << " == ";
	this->myRight->unparse(out, 0);
}

void NotEqualsNode::unparse(std::ostream& out, int indent) {
    this->myLeft->unparse(out, 0);
    out << " != ";
    this->myRight->unparse(out, 0);
}

void GreaterNode::unparse(std::ostream& out, int indent) {
    this->myLeft->unparse(out, 0);
    out << " >= ";
    this->myRight->unparse(out, 0);
}

void GreaterEqNode::unparse(std::ostream& out, int indent) {
    this->myLeft->unparse(out, 0);
    out << " >= ";
    this->myRight->unparse(out, 0);
}

void LessNode::unparse(std::ostream& out, int indent) {
    this->myLeft->unparse(out, 0);
    out << " < ";
    this->myRight->unparse(out, 0);
}

void LessEqNode::unparse(std::ostream& out, int indent) {
    this->myLeft->unparse(out, 0);
    out << " <= ";
    this->myRight->unparse(out, 0);
}

void NegNode::unparse(std::ostream& out, int indent){
	out << " -";
	this->myExp->unparse(out, 0);
	
}

void NotNode::unparse(std::ostream& out, int indent){
	out << " !";
	this->myExp->unparse(out, 0);
	
}

void ClassDefnNode::unparse(std::ostream& out, int indent){
	this->myID->unparse(out, 0);
	out <<" : custom {\n";
	for (auto decl : *myDeclList) {
		doIndent(out, indent + 1);
		decl->unparse(out, 0);
	}
	out << "};\n";
}

void ClassTypeNode::unparse(std::ostream& out, int indent){
	out <<"ClassType";
}

void IntLitNode::unparse(std::ostream& out, int indent) {
	out << this->myInt;
}

void StrLitNode::unparse(std::ostream& out, int indent) {
	out << this->myStr;
}

void FormalDeclNode::unparse(std::ostream& out, int indent){
	this->myID->unparse(out, 0);
	out << " : ";
	this->myType->unparse(out, 0);
}

void AssignStmtNode::unparse(std::ostream& out, int indent){
	this->myLoc->unparse(out, 0);
	out << " = ";
	this->myExp->unparse(out, 0);
	out << ";\n";
}

void FnDeclNode::unparse(std::ostream& out, int indent) {
	this->myID->unparse(out, 0);
	out << " : (";
	bool firstFormal = true;
	for (auto formals : *myMaybeFormals) {
		if (firstFormal) { firstFormal = false; }
		else { out << ", "; }
		formals->unparse(out, 0);
	}
	
	out << ") -> ";
	
	this->myType->unparse(out, 0);
	out << " {\n";
	for (auto stmt : *myStmtList) {
		doIndent(out, 1);
		stmt->unparse(out, 1);
	}
	
	out << "}\n";
}

void PostDecNode::unparse(std::ostream& out, int indent) {
	this->myLoc->unparse(out, 2);
	out << "--;\n";
}

void PostIncNode::unparse(std::ostream& out, int indent) {
	this->myLoc->unparse(out, 2);
	out << "++;\n";
}

void ToConsoleNode::unparse(std::ostream& out, int indent) {
	out << "toconsole ";
	this->myExp->unparse(out, 2);
	out << ";\n";
}

void FromConsoleNode::unparse(std::ostream& out, int indent) {
	out << "fromconsole ";
	this->myLoc->unparse(out, 2);
	out << ";\n";
}

void WhileStmtNode::unparse(std::ostream& out, int indent) {
	
	out << "while (";
	this->myExp->unparse(out, 2);
	out << "){\n";
	for (auto stmt : *myStmtList) {
		doIndent(out, 1);
		stmt->unparse(out, 5);
	}
	doIndent(out, 1);
	out << "}\n";
}

void IfStmtNode::unparse(std::ostream& out, int indent) {
	out << "if (";
	this->myExp->unparse(out, 2);
	out << "){\n";
	for (auto stmt : *myStmtList) {
		doIndent(out, 1);
		stmt->unparse(out, 1);
	}
	doIndent(out, 1);
	out << "}\n";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent) {
	out << "if (";
	this->myExp->unparse(out, 2);
	out << "){\n";
	for (auto stmt : *myStmtList1) {
		doIndent(out, 1);
		stmt->unparse(out, 1);
	}
	doIndent(out, 1);
	out << "} else {\n";
	for (auto stmt : *myStmtList2) {
		doIndent(out, 1);
		stmt->unparse(out, 1);
	}
	doIndent(out, 1);
	out << "}\n";
}

void MaybeStmtNode::unparse(std::ostream& out, int indent) {
	out << "maybe ";
	this->myExp1->unparse(out, 2);
	out << " means ";
	this->myExp2->unparse(out, 2);
	out << " otherwise ";
	this->myExp3->unparse(out, 2);
	out << ";\n";
}

void ReturnStmtNode::unparse(std::ostream& out, int indent) {
	
	if (myExp != nullptr) {
		out << "return ";
		this->myExp->unparse(out, 0);
	} else { out << "return";}
	out << ";\n";
}

void TrueNode::unparse(std::ostream& out, int indent) {
	out << "true";
}

void FalseNode::unparse(std::ostream& out, int indent) {
	out << "false";
}

void EhNode::unparse(std::ostream& out, int indent) {
	out << "eh?";
}

void CallStmtNode::unparse(std::ostream& out, int indent) {
	this->myCallExp->unparse(out, 0);
}

void CallExpNode::unparse(std::ostream& out, int indent) {
	this->myLoc->unparse(out, 0);
	out << "(";
	if (myList != nullptr) {
		for (auto list : *myList) {
			list->unparse(out, 1);
		}
	}
	out << ");\n";
	
}

void RefTypeNode::unparse(std::ostream& out, int indent) {
	if (myID != nullptr) {
		this->myID->unparse(out, 0);
	} else if (myType != nullptr) {
		out << "ref ";
		this->myType->unparse(out, 0);
	} else {
		out << "ref";
	}
	out << ";\n";
	
}

} // End namespace a_lang

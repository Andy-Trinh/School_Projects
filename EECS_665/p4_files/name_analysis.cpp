#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "errName.hpp"
#include <iostream>
namespace a_lang{

//TODO here is a subset of the nodes needed to do nameAnalysis, 
// you should add the rest to allow for a complete treatment
// of any AST

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	throw new ToDoError("This function should have"
		"been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab){
	bool res = true;
	symTab->enterScope();
	for (auto global : *myGlobals){
		
		res = global->nameAnalysis(symTab) && res;

	}
	return res;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){
	
	std::string varName = ID()->getName();
	//const DataType * dataType = getTypeNode()->getType();

	bool validName = !symTab->conflictName(varName);
	if (!validName){ NameErr::multiDecl(ID()->pos()); }
	
	if (!validName){ 
		return false; 
	} else {
		symTab->insert(new varSymbol(varName));
		SemSymbol * sym = symTab->find(varName);
		this->myID->attachSymbol(sym);
		return true;
	}
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	std::string fnName = this->ID()->getName();
	
	bool validRet = myRetType->nameAnalysis(symTab);

	ScopeTable * atFnScope = symTab->getCurrentScope();
	ScopeTable * inFnScope = symTab->enterScope();

	bool validName = true;
	if (atFnScope->conflictName(fnName)){
		NameErr::multiDecl(ID()->pos()); 
		return false;
	}

	bool validFormals = true;
	auto formalTypeNodes = list<TypeNode *>();
	for (auto formal : *(this->myFormals)){
		validFormals = formal->nameAnalysis(symTab) && validFormals;
		TypeNode * formalTypeNode = formal->getTypeNode();
		formalTypeNodes.push_back(formalTypeNode);
	}

	bool validBody = true;
	

	symTab->leaveScope();

	if (!(validRet && validFormals && validName && validBody)){ 
		return false; 
	} else {
		symTab->insert(new fnSymbol(fnName));
		SemSymbol * sym = symTab->find(fnName);
		this->myID->attachSymbol(sym);
		return true;
	}
}

bool BinaryExpNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool IDNode::nameAnalysis(SymbolTable* symTab){
	std::string myName = this->getName();
	SemSymbol * sym = symTab->find(myName);
	this->attachSymbol(sym);
	return true;
}

void IDNode::attachSymbol(SemSymbol * symbolIn){
	this->mySymbol = symbolIn;
}

bool MemberFieldExpNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool AssignStmtNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool CallStmtNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool WhileStmtNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool CallExpNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool NegNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool NotNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool PostIncStmtNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool PostDecStmtNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool TypeNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool TrueNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool FalseNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool EhNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool IntTypeNode::nameAnalysis(SymbolTable* symTab){
	// Name analysis may never even recurse down to IntTypeNode,
	// but if it does, just return true to indicate that 
	// name analysis has not failed, and add nothing to the symbol table
	return true;
}

bool IntLitNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool StrLitNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

}

#include "symbol_table.hpp"
namespace a_lang {

ScopeTable::ScopeTable(){
	symbols = new HashMap<std::string, SemSymbol *>();
}

SymbolTable::SymbolTable(){
	//TODO: implement the list of hashtables approach
	// to building a symbol table:
	// Upon entry to a scope a new scope table will be 
	// entered into the front of the chain and upon exit the 
	// latest scope table will be removed from the front of 
	// the chain.
	scopeTableChain = new std::list<ScopeTable *>();
}


ScopeTable * SymbolTable::enterScope(){
	ScopeTable * newScope = new ScopeTable();
	scopeTableChain->push_front(newScope);
	return newScope;
}

void SymbolTable::leaveScope(){
	scopeTableChain->pop_front();
}

ScopeTable * SymbolTable::getCurrentScope(){
	return scopeTableChain->front();
}

bool SymbolTable::insert(SemSymbol * symbol){
	return scopeTableChain->front()->insert(symbol);
}

SemSymbol * SymbolTable::find(std::string varName){
	for (ScopeTable * scope : *scopeTableChain){
		SemSymbol * sym = scope->lookup(varName);
		if (sym != nullptr) { return sym; }
	}
	return nullptr;
}

bool ScopeTable::insert(SemSymbol * symbol){
	std::string symName = symbol->getName();
	bool alreadyInScope = (this->lookup(symName) != NULL);

	if (alreadyInScope){
		return false;
	}

	this->symbols->insert(std::make_pair(symName, symbol));
	return true;
}

bool SymbolTable::conflictName(std::string varName){
	bool hasConflict = getCurrentScope()->conflictName(varName);
	return hasConflict;
}

bool ScopeTable::conflictName(std::string varName){
	SemSymbol * found = lookup(varName);
	if (found != nullptr){
		return true;
	}
	return false;
}

SemSymbol * ScopeTable::lookup(std::string name){
	auto found = symbols->find(name);
	if (found == symbols->end()){
		return NULL;
	}
	return found->second;
}

std::string ScopeTable::toString(){
	std::string result = "";
	for (auto entry : *symbols){
		result += entry.second->toString();
		result += "\n";
	}
	return result;
}

std::string SemSymbol::toString(){
	std::string result = this->getName();
	return result;
}

}

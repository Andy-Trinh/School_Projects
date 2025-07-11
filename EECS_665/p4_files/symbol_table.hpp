#ifndef A_LANG_SYMBOL_TABLE_HPP
#define A_LANG_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>
#include "types.hpp"


//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using namespace std;

namespace a_lang {

enum SymbolKind {
	VAR, FN
};

//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols 
// exist for the lifetime of a scope in the 
// symbol table. 
class SemSymbol {
	//TODO add the fields that 
	// each semantic symbol should track
	// (i.e. the kind of the symbol (either a variable or function)
	// and functions to get/set those fields
public:
	SemSymbol(std::string name) : myName(name){}
	std::string getName() { return myName; }
	virtual std::string toString();
	virtual SymbolKind getKind() const = 0;
	std::string getSymbolType() { 
			switch(getKind()){
			case VAR:
				return "var";
			case FN:
				return "fn";
			default:
				return "Invalid Symbol";
		}
	}
private:
	std::string myName;

};

class varSymbol : public SemSymbol {
public:
	varSymbol(std::string name) : SemSymbol(name) { }
	virtual SymbolKind getKind() const override { return VAR; } 
};

class fnSymbol : public SemSymbol{
public:
	fnSymbol(std::string name) : SemSymbol(name){ }
	virtual SymbolKind getKind() const { return FN; }
	SymbolKind getKind(){ return FN; } 
};

//A single scope. The symbol table is broken down into a 
// chain of scope tables, and each scope table holds 
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
	public:
		ScopeTable();
		//TODO: add functions for looking up symbols
		// and/or returning information to indicate
		// that the symbol does not exist within the
		// current scope.
		SemSymbol * lookup(std::string name);
		bool insert(SemSymbol * symbol);
		bool conflictName(std::string name);
		std::string toString();
		
	private:
		HashMap<std::string, SemSymbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc. 
		ScopeTable * enterScope();
		void leaveScope();
		ScopeTable * getCurrentScope();
		bool insert(SemSymbol * symbol);
		SemSymbol * find(std::string varName);
		bool conflictName(std::string name);
	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif

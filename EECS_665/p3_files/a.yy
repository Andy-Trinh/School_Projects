%skeleton "lalr1.cc"
%require "3.0"
%debug
%defines
%define api.namespace{a_lang}
%define api.parser.class {Parser}
%define api.value.type variant
%define parse.error verbose
%output "parser.cc"
%token-table

%code requires{
	#include <list>
	#include "tokens.hpp"
	#include "ast.hpp"
	namespace a_lang {
		class Scanner;
	}

//The following definition is required when 
// we don't use the %locations directive (which we won't)
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

//End "requires" code
}

%parse-param { a_lang::Scanner &scanner }
%parse-param { a_lang::ProgramNode** root }
%code{
   // C std code for utility functions
   #include <iostream>
   #include <cstdlib>
   #include <fstream>

   // Our code for interoperation between scanner/parser
   #include "scanner.hpp"
   #include "ast.hpp"
   #include "tokens.hpp"

  //Request tokens from our scanner member, not 
  // from a global function
  #undef yylex
  #define yylex scanner.yylex
}

//%define parse.assert

/* Terminals 
 *  No need to touch these, but do note the translation type
 *  of each node. Most are just Token pointers, 
 *  and thus have no fields (other than line and column).
 *  Some terminals, like ID, have the translation type IDToken *,
 *  meaning the translation also has a name field. 
*/
%token                     END   0 "end file"
%token	<a_lang::Token *>       AND
%token	<a_lang::Token *>       ASSIGN
%token	<a_lang::Token *>       ARROW
%token	<a_lang::Token *>       BOOL
%token	<a_lang::Token *>       COLON
%token	<a_lang::Token *>       COMMA
%token	<a_lang::Token *>       CUSTOM
%token	<a_lang::Token *>       DASH
%token	<a_lang::Token *>       ELSE
%token	<a_lang::Token *>       EH
%token	<a_lang::Token *>       EQUALS
%token	<a_lang::Token *>       FALSE
%token	<a_lang::Token *>       FROMCONSOLE
%token	<a_lang::Token *>       GREATER
%token	<a_lang::Token *>       GREATEREQ
%token	<a_lang::IDToken *>     ID
%token	<a_lang::Token *>       IF
%token	<a_lang::Token *>       INT
%token	<a_lang::IntLitToken *> INTLITERAL
%token	<a_lang::Token *>       IMMUTABLE
%token	<a_lang::Token *>       LCURLY
%token	<a_lang::Token *>       LESS
%token	<a_lang::Token *>       LESSEQ
%token	<a_lang::Token *>       LPAREN
%token	<a_lang::Token *>       MAYBE
%token	<a_lang::Token *>       MEANS
%token	<a_lang::Token *>       NOT
%token	<a_lang::Token *>       NOTEQUALS
%token	<a_lang::Token *>       OR
%token	<a_lang::Token *>       OTHERWISE
%token	<a_lang::Token *>       CROSS
%token	<a_lang::Token *>       POSTDEC
%token	<a_lang::Token *>       POSTINC
%token	<a_lang::Token *>       RETURN
%token	<a_lang::Token *>       RCURLY
%token	<a_lang::Token *>       REF
%token	<a_lang::Token *>       RPAREN
%token	<a_lang::Token *>       SEMICOL
%token	<a_lang::Token *>       SLASH
%token	<a_lang::Token *>       STAR
%token	<a_lang::StrToken *>    STRINGLITERAL
%token	<a_lang::Token *>       TOCONSOLE
%token	<a_lang::Token *>       TRUE
%token	<a_lang::Token *>       VOID
%token	<a_lang::Token *>       WHILE

/* Nonterminals
*  The specifier in angle brackets
*  indicates the type of the translation attribute.
*  TODO: You will need to add more attributes for other nonterminals
*  to this list as you add productions to the grammar
*  below (along with indicating the appropriate translation
*  attribute type).
*/
/*       (attribute type)    (nonterminal)    */
%type <a_lang::ProgramNode *> program
%type <std::list<a_lang::DeclNode *> *> globals
%type <a_lang::DeclNode *> decl
%type <a_lang::VarDeclNode *> varDecl
%type <a_lang::TypeNode *> type
%type <a_lang::TypeNode *> datatype
%type <a_lang::TypeNode *> primType
%type <a_lang::LocNode *> loc
%type <a_lang::IDNode *> name
%type <a_lang::ExpNode *> exp
%type <a_lang::ExpNode *> term
%type <a_lang::CallExpNode *> callExp
%type <a_lang::ClassDefnNode *> classTypeDecl
%type <std::list<a_lang::DeclNode *> *> classBody
%type <a_lang::FnDeclNode *> fnDecl
%type <std::list<a_lang::FormalDeclNode *> *> formalList
%type <std::list<a_lang::FormalDeclNode *> *> maybeFormals
%type <a_lang::FormalDeclNode *> formalDecl
%type <std::list<a_lang::StmtNode *> *> stmtList
%type <a_lang::StmtNode *> stmt
%type <a_lang::StmtNode *> blockStmt
%type <std::list<a_lang::ExpNode *> *> actualList

%right ASSIGN
%left OR
%left AND
%nonassoc LESS GREATER LESSEQ GREATEREQ EQUALS NOTEQUALS
%left DASH CROSS
%left STAR SLASH
%left NOT 

%%


program		: globals
		  {
		  $$ = new ProgramNode($1);
		  *root = $$;
		  }

globals		: globals decl
		  {
		  $$ = $1;
		  DeclNode * declNode = $2;
		  $$->push_back(declNode);
		  }
		| /* epsilon */
		  {
		  $$ = new std::list<DeclNode *>();
		  }

decl		: varDecl SEMICOL
		  {
		  $$ = $1;
		  }
		| classTypeDecl
		  {
			$$ = $1;
		  }
		| fnDecl
		  {
			$$ = $1;
		  }

varDecl		: name COLON type
		  {
		  const Position * p;
		  p = new Position($1->pos(), $2->pos());
		  $$ = new VarDeclNode(p, $1, $3);
		  }
		| name COLON type ASSIGN exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $3->pos());
		  	$$ = new VarDeclNode(p, $1, $3, $5);
		  }

type		: IMMUTABLE datatype
		  {
			$$ = new ImmutableTypeNode($1->pos(), $2);
		  }
		| datatype
		  {
		  $$ = $1;
		  }

datatype	: REF primType
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new RefTypeNode(p, $2);
		  }
		| primType
		  {
		  $$ = $1;
		  }
		| REF name
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new RefTypeNode(p, $2);
		  }
		| name
		  {
			$$ = new RefTypeNode($1->pos(), $1);
		  }

primType	: INT 
		  {
		  $$ = new IntTypeNode($1->pos());
		  }
		| BOOL
		  {
		  $$ = new BoolTypeNode($1->pos());
		  }
		| VOID
		  {
			$$ = new VoidTypeNode($1->pos());
		  }

classTypeDecl 	: name COLON CUSTOM LCURLY classBody RCURLY SEMICOL 
		  {
			const Position * p;
		  	p = new Position($1->pos(), $7->pos());
			$$ = new ClassDefnNode(p, $1, $5);
		  }

classBody 	: classBody varDecl SEMICOL
		  {
		  //TODO
			$$ = $1;
			VarDeclNode * varDeclNode = $2;
			$$->push_back(varDeclNode);
		  }
		| classBody fnDecl
		  {
		  //TODO
		  $$ = $1;
		  FnDeclNode * fnDeclNode = $2;
		  $$->push_back(fnDeclNode);
		  }
		| /* epsilon */
		  {
			$$ = new std::list<DeclNode *>();
		  }

fnDecl 		: name COLON LPAREN maybeFormals RPAREN ARROW type LCURLY stmtList RCURLY
		  {
			const Position * p;
			p = new Position($1->pos(), $10->pos());
			$$ = new FnDeclNode(p, $1, $4, $7, $9);
		  }

maybeFormals	: /* epsilon */
		  {
			$$ = new std::list<FormalDeclNode *>();
		  }
		| formalList
		  {
			$$ = $1;
		  }

formalList	: formalDecl
		  {
			$$ = new std::list<FormalDeclNode *>();
			FormalDeclNode * formalDecl = $1;
			$$->push_back(formalDecl);
		  }
		| formalList COMMA formalDecl
		  {
			$$ = $1;
			FormalDeclNode * formalDecl = $3;
			$$->push_back(formalDecl);
		  }

formalDecl	: name COLON type
		  {
			const Position * p;
			p = new Position($1->pos(), $2->pos());
			$$ = new FormalDeclNode(p, $1, $3);
		  }

stmtList	: /* epsilon */
		  {
			$$ = new std::list<StmtNode *>();
		  }
		| stmtList stmt SEMICOL
		  {
			$$ = $1;
			StmtNode * stmt = $2;
			$$->push_back(stmt);
		  }
		| stmtList blockStmt
		  {
			$$ = $1;
			StmtNode * blockStmt = $2;
			$$->push_back(blockStmt);
		  }

blockStmt	: WHILE LPAREN exp RPAREN LCURLY stmtList RCURLY
		  {
			const Position * p;
			p = new Position($1->pos(), $7->pos());
			$$ = new WhileStmtNode(p, $3, $6);
		  }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY
		  {
			const Position * p;
			p = new Position($1->pos(), $7->pos());
			$$ = new IfStmtNode(p, $3, $6);
		  }
		| IF LPAREN exp RPAREN LCURLY stmtList RCURLY ELSE LCURLY stmtList RCURLY
		  {
			const Position * p;
			p = new Position($1->pos(), $11->pos());
			$$ = new IfElseStmtNode(p, $3, $6, $10);
		  }

stmt		: varDecl
		  {
			$$ = $1;
		  }
		| loc ASSIGN exp
		  {
			const Position * p;
			p = new Position($1->pos(), $2->pos());
			$$ = new AssignStmtNode(p, $1, $3);
		  }
		| callExp
		  {
			$$ = new CallStmtNode($1->pos(), $1);
		  }
		| loc POSTDEC
		  {
			const Position * p;
			p = new Position($1->pos(), $2->pos());
			$$ = new PostDecNode(p, $1);
		  }
		| loc POSTINC
		  {
			const Position * p;
			p = new Position($1->pos(), $2->pos());
			$$ = new PostIncNode(p, $1);
		  }
		| TOCONSOLE exp
		  {
			const Position * p;
			p = new Position($1->pos(), $2->pos());
			$$ = new ToConsoleNode(p, $2);
		  }
		| FROMCONSOLE loc
		  {
			const Position * p;
			p = new Position($1->pos(), $2->pos());
			$$ = new FromConsoleNode(p, $2);
		  }
		| MAYBE exp MEANS exp OTHERWISE exp
		  {
			const Position * p;
			p = new Position($1->pos(), $6->pos());
			$$ = new MaybeStmtNode(p, $2, $4, $6);
		  }
		| RETURN exp
		  {
			const Position * p;
			p = new Position($1->pos(), $2->pos());
			$$ = new ReturnStmtNode(p, $2);
		  }
		| RETURN
		  {
			$$ = new ReturnStmtNode($1->pos());
		  }


exp		: exp DASH exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new MinusNode($1->pos(),$1, $3);
		  }
		| exp CROSS exp
		  {	
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new PlusNode($1->pos(),$1, $3);		
		  }
		| exp STAR exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new MultNode($1->pos(),$1, $3);
		  }
		| exp SLASH exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new DivNode($1->pos(),$1, $3);
		  }
		| exp AND exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new AndNode($1->pos(),$1, $3);
		  }
		| exp OR exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new OrNode($1->pos(),$1, $3);
		  }
		| exp EQUALS exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new EqualsNode($1->pos(),$1, $3);
		  }
		| exp NOTEQUALS exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new NotEqualsNode($1->pos(),$1, $3);
		  }
		| exp GREATER exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new GreaterNode($1->pos(),$1, $3);
		  }
		| exp GREATEREQ exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new GreaterEqNode($1->pos(),$1, $3);
		  }
		| exp LESS exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new LessNode($1->pos(),$1, $3);
		  }
		| exp LESSEQ exp
		  {
			const Position * p;
		  	p = new Position($1->pos(), $2->pos());
			$$ = new LessEqNode($1->pos(),$1, $3);
		  }
		| NOT exp
		  {
			$$ = new NotNode($2->pos(), $2);
		  }
		| DASH term
		  {
			$$ = new NegNode($2->pos(), $2);
		  }
		| term
		  {
			$$ = $1;
		  }

callExp		: loc LPAREN RPAREN
		  {
			std::cout << "callExp";
			const Position * p;
		  	p = new Position($1->pos(), $3->pos());
			$$ = new CallExpNode(p, $1);
		  }
		| loc LPAREN actualList RPAREN
		  {
			const Position * p;
		  	p = new Position($1->pos(), $4->pos());
			$$ = new CallExpNode(p, $1, $3);
		  }

actualList	: exp
		  {
			$$ = new std::list<ExpNode *>();
			ExpNode * exp = $1;
			$$->push_back($1);
		  }
		| actualList COMMA exp
		  {
			$$ = $1;
			ExpNode * exp = $3;
			$$->push_back(exp);
		  }

term 		: loc
		  {
			$$ = $1;
		  }
		| INTLITERAL
		  {
			$$ = new IntLitNode($1->pos(), $1);
		  }
		| STRINGLITERAL
		  {
			$$ = new StrLitNode($1->pos(), $1);
		  }
		| TRUE
		  {
			$$ = new TrueNode($1->pos());
		  }
		| FALSE
		  {
			$$ = new FalseNode($1->pos());
		  }
		| EH
		  {
			$$ = new EhNode($1->pos());
		  }
		| LPAREN exp RPAREN
		  {
			$$ = $2;
		  }
		| callExp 
		  {
			$$ = $1;
		  }

loc		: name
		  {
		  $$ = $1;
		  }
		| loc ARROW name
		  {
			$$ = $1;
		  }

name		: ID
		  {
                  const Position * pos = $1->pos();
	 	  $$ = new IDNode(pos, $1->value());
		  }
	
%%

void a_lang::Parser::error(const std::string& msg){
	std::cout << msg << std::endl;
	std::cerr << "syntax error" << std::endl;
}

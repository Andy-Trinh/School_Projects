%{
#include <string>
#include <limits.h>

/* Get our custom yyFlexScanner subclass */
#include "scanner.hpp"
#include "tokens.hpp"
#undef  YY_DECL
#define YY_DECL int a_lang::Scanner::yylex( a_lang::Parser::semantic_type * const lval )

/* define yyterminate as this instead of NULL */
#define yyterminate() return( TokenKind::END )

/* Exclude unistd.h for Visual Studio compatability. */
#define YY_NO_UNISTD_H

/* typedef to make the reference to token kinds shorter */
using TokenKind = a_lang::Parser::token;

%}

%option debug
%option nodefault
%option yyclass="a_lang::Scanner"
%option noyywrap
%option c++

DIGIT [0-9]
LETTER [a-zA-Z]
STRING  \"([^\\"\n]|\\.)*\"

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}



"+"		{
		// Keep track of the "kind" of token we matched. In this case,
		// it's a CROSS (aka the plus symbol). The full list of token
		// kinds is declared around line 197 of grammar.hh. The testing
		// function Scanner::outputTokens also indicates what each token
		// represents (declared in scanner.cpp line 9)
		int tokenKind = TokenKind::CROSS;

		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);

		//Instatiate a token. There are 5 classes of tokens defined based on 
		// - a literal string (StringLitToken)
		// - a literal integer (IntLitToken)
		// - a literal short (ShortLitToken)
		// - an identifier (IDToken)
		// - a base Token (any other type)
		// The token object is a handy place to keep information about the
		// token (i.e. token metadata) such as the line and column on which
		// the token character started (which can be used later in debug 
		// messages, etc). 
		Token * token = new Token(pos, tokenKind);

		//Supply the scanner with the lastest token.
		// (When Flex processes a .l file in C++ mode, it creates 
		// a class called Scanner, which is a subclass of yyFlexScanner)
		// By emplacing the token value, the scanner's client program
		// can retrieve it and use it for parsing or printing.
		this->yylval->emplace<a_lang::Token *>(token);

		//Since we consumed 1 character, increase the column number by 1
		colNum++;

		//Let the scanner's client program know that a PLUS token
		// has been matched
		return tokenKind;
		}

"-" {
		//Essentially the same thing as CROSS, but without the verbose
		// comments so it doesn't look so scary.
		int tokenKind = TokenKind::DASH;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
		}

"*" {
		int tokenKind = TokenKind::STAR;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"/" {
		int tokenKind = TokenKind::SLASH;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"=" {
		int tokenKind = TokenKind::ASSIGN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

":" {
		int tokenKind = TokenKind::COLON;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"," {
		int tokenKind = TokenKind::COMMA;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"==" {
		int tokenKind = TokenKind::EQUALS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += 2;
		return tokenKind;
}

">" {
		int tokenKind = TokenKind::GREATER;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

">=" {
		int tokenKind = TokenKind::GREATEREQ;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+2);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += 2;
		return tokenKind;
}

"<" {
		int tokenKind = TokenKind::LESS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"<=" {
		int tokenKind = TokenKind::LESSEQ;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+2);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += 2;
		return tokenKind;
}

"!=" {
		int tokenKind = TokenKind::NOTEQUALS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+2);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += 2;
		return tokenKind;
}


"!" {
		int tokenKind = TokenKind::NOT;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"&" {
		int tokenKind = TokenKind::REF;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"++" {
		int tokenKind = TokenKind::POSTINC;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += 2;
		return tokenKind;
}

"--" {
		int tokenKind = TokenKind::POSTDEC;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += 2;
		return tokenKind;
}

"{" {
		int tokenKind = TokenKind::LCURLY;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"}" {
		int tokenKind = TokenKind::RCURLY;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"(" {
		int tokenKind = TokenKind::LPAREN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

")" {
		int tokenKind = TokenKind::RPAREN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

";" {
		int tokenKind = TokenKind::SEMICOL;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum++;
		return tokenKind;
}

"->" {
		int tokenKind = TokenKind::ARROW;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"and" {
		int tokenKind = TokenKind::AND;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"bool" {
		int tokenKind = TokenKind::BOOL;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"custom" {
		int tokenKind = TokenKind::CUSTOM;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"else" {
		int tokenKind = TokenKind::ELSE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"eh?" {
		int tokenKind = TokenKind::EH;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"true" {
		int tokenKind = TokenKind::TRUE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"false" {
		int tokenKind = TokenKind::FALSE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"toconsole" {
		int tokenKind = TokenKind::TOCONSOLE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"fromconsole" {
		int tokenKind = TokenKind::FROMCONSOLE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"if" {
		int tokenKind = TokenKind::IF;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"immutable" {
		int tokenKind = TokenKind::IMMUTABLE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"int" {
		int tokenKind = TokenKind::INT;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"or" {
		int tokenKind = TokenKind::OR;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"otherwise" {
		int tokenKind = TokenKind::OTHERWISE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"means" {
		int tokenKind = TokenKind::MEANS;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"return" {
		int tokenKind = TokenKind::RETURN;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"void" {
		int tokenKind = TokenKind::VOID;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

"while" {
		int tokenKind = TokenKind::WHILE;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		this->yylval->emplace<a_lang::Token *>(new Token(pos, tokenKind));
		colNum += strlen(yytext);
		return tokenKind;
}

\"([^\n\"]|\\[nt'\"?\\])*\" {
		std::string strVal(yytext + 1, strlen(yytext) - 2);
		int tokenKind = TokenKind::STRINGLITERAL;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum + strlen(yytext));
        this->yylval->emplace<a_lang::Token *>(new StrToken(pos, yytext));
        colNum += strlen(yytext);
        return tokenKind;
}


{DIGIT}+	{
		//TODO: The following computation of the integer
		// value does not check for overflow. It should do so.

		char *endptr;
    	long intVal = strtol(yytext, &endptr, 10);

		if ((intVal > INT_MAX || intVal < INT_MIN)) {
			// Handle integer overflow
            std::string msg = "Integer literal overflow";
            Position * pos = new Position(lineNum, colNum, lineNum, colNum+strlen(yytext));
            Report::fatal(pos, msg);
			break;
		}

		//Note that the IntLitToken does not take a TokenKind argument
		// since it's implicitly known that the token represents an
		// INTLITERAL		

		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));
		Token * tok = new IntLitToken(pos, intVal);
		yylval->emplace<a_lang::Token *>(tok);

		//TODO: update the colNum count according to the number of
		// characters consumed. 
		colNum += strlen(yytext);


		//As before, alert the client program that an INTLITERAL 
		// token was matched
		return TokenKind::INTLITERAL;
		}

({LETTER}+)({LETTER}|{DIGIT}+) {
		int tokenKind = TokenKind::ID;

		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+strlen(yytext));

		
		this->yylval->emplace<a_lang::Token *>(new IDToken(pos, yytext));
		
		colNum += strlen(yytext);

		return tokenKind;
}

\n		{
		//Update the line number, reset the columnNumber
		lineNum++;
		colNum = 1;
		}

[ \t\r]+    {
                colNum += strlen(yytext);
            }

"#"[^\n]* {
		colNum += strlen(yytext);
}

\"([^\n"\\]|\\[nt'"\\])* {
		std::string msg = "Unterminated string literal detected";
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+ strlen(yytext));
		Report::fatal(pos,msg);
		colNum += strlen(yytext);
}

\"([^\n\"\\]|\\[nt'\"?\\])*\\[^\nnt'\"?\\]([^\n\"])*\" {
		std::string msg = "String literal with bad escape sequence detected";
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+ strlen(yytext));
		Report::fatal(pos,msg);
		colNum += strlen(yytext);
}

\"([^\n\"\\]|\\[nt'\"?\\])*(\\[^\nnt'\"?\\])?([^\n\"\\]|\\[nt'\"?\\])*\\? {
		std::string msg = "Unterminated string literal with bad escape sequence detected";
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+ strlen(yytext));
		Report::fatal(pos,msg);
		colNum += strlen(yytext);
}


.   {
		//TODO: Add the rest of the rules above this point so that legal characters
		// are properly accounted for. 
		std::string msg = "Illegal character ";
		msg += yytext;
		Position * pos = new Position(
			lineNum, colNum, lineNum, colNum+1);
		Report::fatal(pos,msg);
		colNum += strlen(yytext);
    }
%%

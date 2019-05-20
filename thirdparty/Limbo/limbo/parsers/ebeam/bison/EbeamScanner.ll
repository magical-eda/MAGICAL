/* $Id: scanner.ll 44 2008-10-23 09:03:19Z tb $ -*- mode: c++ -*- */
/** \file scanner.ll Define the example Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "EbeamScanner.h"

/* import the parser's token type into a local typedef */
typedef EbeamParser::Parser::token token;
typedef EbeamParser::Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "ExampleFlexLexer" */
%option prefix="EbeamParser"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

 /*** BEGIN EXAMPLE - Change the example lexer rules below ***/

"END" {
	return token::KWD_END;
}
"UNITS" {
	return token::KWD_UNITS;
}
"DATABASE" {
	return token::KWD_DATABASE;
}
"DISTANCE" {
	return token::KWD_DISTANCE;
}
"MICRONS" {
	return token::KWD_MICRONS;
}
"OFFSET" {
	return token::KWD_OFFSET;
}
"WIDTH" {
	return token::KWD_WIDTH;
}
"STEP" {
	return token::KWD_STEP;
}
"LAYERID" {
	return token::KWD_LAYERID;
}
"LAYER" {
	return token::KWD_LAYER;
}
"MACRO" {
	return token::KWD_MACRO;
}
"EBEAMBOUNDARY" {
	return token::KWD_EBEAMBOUNDARY;
}
"CONFLICTSITE" {
	return token::KWD_CONFLICTSITE;
}
"SITE" {
	return token::KWD_SITE;
}
"LIBRARY" {
	return token::KWD_LIBRARY;
}

[\+\-]?[0-9]+ {
    yylval->integerVal = atoi(yytext);
    return token::INTEGER;
}

[\+\-]?[0-9]+"."[0-9]* {
    yylval->doubleVal = atof(yytext);
    return token::DOUBLE;
}

[A-Za-z][A-Za-z0-9_,.\-]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}

\"([^"])*\" {
    yylval->quoteVal = new std::string(yytext+1, yyleng-2);
    return token::QUOTE;
}

 /* gobble up comments */
"#"([^\n])*\n {
    yylloc->step();
}

 /* gobble up white-spaces */
[ \t\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
\n {
    yylloc->lines(yyleng); yylloc->step();
    /* return token::EOL; */
}

 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the example lexer rules above ***/

%% /*** Additional Code ***/

namespace EbeamParser {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : EbeamParserFlexLexer(in, out)
{
}

Scanner::~Scanner()
{
}

void Scanner::set_debug(bool b)
{
    yy_flex_debug = b;
}

}

/* This implementation of ExampleFlexLexer::yylex() is required to fill the
 * vtable of the class ExampleFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int EbeamParserFlexLexer::yylex()
{
    std::cerr << "in EbeamParserFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int EbeamParserFlexLexer::yywrap()
{
    return 1;
}

/* $Id: scanner.ll 44 2008-10-23 09:03:19Z tb $ -*- mode: c++ -*- */
/** \file scanner.ll Define the example Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "DefScanner.h"

/* import the parser's token type into a local typedef */
typedef DefParser::Parser::token token;
typedef DefParser::Parser::token_type token_type;

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
%option prefix="DefParser"

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

"VERSION" {
	return token::KWD_VERSION;
}
"DIVIDERCHAR" {
	return token::KWD_DIVIDERCHAR;
}
"BUSBITCHARS" {
	return token::KWD_BUSBITCHARS;
}
"DESIGN" {
	return token::KWD_DESIGN;
}
"UNITS" {
	return token::KWD_UNITS;
}
"DISTANCE" {
	return token::KWD_DISTANCE;
}
"MICRONS" {
	return token::KWD_MICRONS;
}
"DIEAREA" {
	return token::KWD_DIEAREA;
}
"ROW" {
	return token::KWD_ROW;
}
"DO" {
	return token::KWD_DO;
}
"BY" {
	return token::KWD_BY;
}
"STEP" {
	return token::KWD_STEP;
}
"COMPONENTS" {
	return token::KWD_COMPONENTS;
}
"PINS" {
	return token::KWD_PINS;
}
"NETS" {
	return token::KWD_NETS;
}
"END" {
	return token::KWD_END;
}
"NET" {
	return token::KWD_NET;
}
"DIRECTION" {
	return token::KWD_DIRECTION;
}
"LAYER" {
	return token::KWD_LAYER;
}
"PROPERTYDEFINITIONS" {
	return token::KWD_PROPERTYDEFINITIONS;
}
"COMPONENTPIN" {
	return token::KWD_COMPONENTPIN;
}
"TRACKS" {
	return token::KWD_TRACKS;
}
"GCELLGRID" {
	return token::KWD_GCELLGRID;
}
"VIAS" {
	return token::KWD_VIAS;
}
"VIA" {
	return token::KWD_VIA;
}
"VIARULE" {
	return token::KWD_VIARULE;
}
"CUTSIZE" {
	return token::KWD_CUTSIZE;
}
"LAYERS" {
	return token::KWD_LAYERS;
}
"CUTSPACING" {
	return token::KWD_CUTSPACING;
}
"ENCLOSURE" {
	return token::KWD_ENCLOSURE;
}
"ROWCOL" {
	return token::KWD_ROWCOL;
}
"USE" {
	return token::KWD_USE;
}
"SPECIALNETS" {
	return token::KWD_SPECIALNETS;
}
"SHAPE" {
	return token::KWD_SHAPE;
}
"RECT" {
	return token::KWD_RECT;
}
"SOURCE" {
	return token::KWD_SOURCE;
}
"NONDEFAULTRULES" {
	return token::KWD_NONDEFAULTRULES;
}
"NONDEFAULTRULE" {
	return token::KWD_NONDEFAULTRULE;
}
"HARDSPACING" {
	return token::KWD_HARDSPACING;
}
"WIDTH" {
	return token::KWD_WIDTH;
}
"SPACING" {
	return token::KWD_SPACING;
}
"REGIONS" {
	return token::KWD_REGIONS;
}
"REGION" {
	return token::KWD_REGION;
}
"TYPE" {
	return token::KWD_TYPE;
}
"FENCE" {
	return token::KWD_FENCE;
}
"GROUPS" {
	return token::KWD_GROUPS;
}
"GROUP" {
	return token::KWD_GROUP;
}
"BLOCKAGES" {
    return token::KWD_BLOCKAGES;
}
"PLACEMENT" {
    return token::KWD_PLACEMENT;
}
"ROUTING" {
    return token::KWD_ROUTING;
}

[\+\-]?[0-9]+ {
    yylval->integerVal = atoi(yytext);
    return token::INTEGER;
}

[\+\-]?[0-9]+"."[0-9]* {
    yylval->doubleVal = atof(yytext);
    return token::DOUBLE;
}

[0-9]*\'[A-Za-z][0-9]+ {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::BINARY;
}

[A-Za-z_]([A-Za-z0-9_,.\-\[\]\/\*]|([\\][\(\)]))* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}

\"([^"])*\" {
    yylval->quoteVal = new std::string(yytext+1, yyleng-2);
    return token::QUOTE;
}

 /* gobble up comments */
"#"([^\n])* {
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

namespace DefParser {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : DefParserFlexLexer(in, out)
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

int DefParserFlexLexer::yylex()
{
    std::cerr << "in DefParserFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int DefParserFlexLexer::yywrap()
{
    return 1;
}

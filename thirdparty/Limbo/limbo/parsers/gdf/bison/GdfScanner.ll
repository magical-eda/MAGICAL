/* $Id: scanner.ll 44 2008-10-23 09:03:19Z tb $ -*- mode: c++ -*- */
/** \file scanner.ll Define the example Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "GdfScanner.h"

/* import the parser's token type into a local typedef */
typedef GdfParser::Parser::token token;
typedef GdfParser::Parser::token_type token_type;

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
%option prefix="GdfParser"

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

(?i:GDIF) {return token::KWD_GDIF;}
(?i:GDIFVERSION) {return token::KWD_GDIFVERSION;}
(?i:COMMENT) {return token::KWD_COMMENT;}
(?i:CELL) {return token::KWD_CELL;}
(?i:PORT) {return token::KWD_PORT;}
(?i:TYPE) {return token::KWD_TYPE;}
(?i:LAYER) {return token::KWD_LAYER;}
(?i:PT) {return token::KWD_PT;}
(?i:PATH) {return token::KWD_PATH;}
(?i:IN) {return token::KWD_IN;}
(?i:OUT) {return token::KWD_OUT;}
(?i:INOUT) {return token::KWD_INOUT;}
(?i:POWER) {return token::KWD_POWER;}
(?i:GROUND) {return token::KWD_GROUND;}
(?i:NEW) {return token::KWD_NEW;}
(?i:WIDTH) {return token::KWD_WIDTH;}
(?i:HEIGHT) {return token::KWD_HEIGHT;}
(?i:HORIZONTAL) {return token::KWD_HORIZONTAL;}
(?i:VERTICAL) {return token::KWD_VERTICAL;}
(?i:INSTANCE) {return token::KWD_INSTANCE;}
(?i:CELLREF) {return token::KWD_CELLREF;}
(?i:ORIENT) {return token::KWD_ORIENT;}
(?i:TEXT) {return token::KWD_TEXT;}
(?i:NUMBER_OF_LAYERS) {return token::KWD_NUMBER_OF_LAYERS;}
(?i:WIRE_SPACINGS) {return token::KWD_WIRE_SPACINGS;}
(?i:VIA_SPACINGS) {return token::KWD_VIA_SPACINGS;}
(?i:WIRE_WIDTHS) {return token::KWD_WIRE_WIDTHS;}
(?i:VIA_WIDTHS) {return token::KWD_VIA_WIDTHS;}
(?i:VERTICAL_WIRE_COSTS) {return token::KWD_VERTICAL_WIRE_COSTS;}
(?i:HORIZONTAL_WIRE_COSTS) {return token::KWD_HORIZONTAL_WIRE_COSTS;}
(?i:VIA_COSTS) {return token::KWD_VIA_COSTS;}
(?i:NET) {return token::KWD_NET;}
(?i:PORTREF) {return token::KWD_PORTREF;}
(?i:INSTREF) {return token::KWD_INSTREF;}
(?i:SEGMENT) {return token::KWD_SEGMENT;}
(?i:VIA) {return token::KWD_VIA;}

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

[A-Za-z\[][A-Za-z0-9_,.\$\-\[\]]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}

\"([^"])*\" {
    yylval->quoteVal = new std::string(yytext+1, yyleng-2);
    return token::QUOTE;
}

 /* gobble up comments */
"("[ \t\r]*(?i:COMMENT)[ \t\r]*([^\)])*")" {
    yylloc->step();
}

 /* gobble up white-spaces */
[ \t\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
\n {
    yylloc->lines(yyleng); yylloc->step();
     //return token::EOL; 
}

 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the example lexer rules above ***/

%% /*** Additional Code ***/

namespace GdfParser {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : GdfParserFlexLexer(in, out)
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

int GdfParserFlexLexer::yylex()
{
    std::cerr << "in GdfParserFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int GdfParserFlexLexer::yywrap()
{
    return 1;
}

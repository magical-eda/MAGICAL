/* $Id: scanner.ll 44 2008-10-23 09:03:19Z tb $ -*- mode: c++ -*- */
/** \file scanner.ll Define the example Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "BookshelfScanner.h"

/* import the parser's token type into a local typedef */
typedef BookshelfParser::Parser::token token;
typedef BookshelfParser::Parser::token_type token_type;

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
%option prefix="BookshelfParser"

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

(?i:NumNets) {return token::KWD_NUMNETS;}
(?i:NumPins) {return token::KWD_NUMPINS;}
(?i:NumNodes) {return token::KWD_NUMNODES;}
(?i:NumTerminals) {return token::KWD_NUMTERMINALS;}
(?i:NumRows) {return token::KWD_NUMROWS;}
(?i:CoreRow) {return token::KWD_COREROW;}
(?i:Horizontal) {return token::KWD_HORIZONTAL;}
(?i:Vertical) {return token::KWD_VERTICAL;}
(?i:Coordinate) {return token::KWD_COORDINATE;}
(?i:Height) {return token::KWD_HEIGHT;}
(?i:Sitewidth) {return token::KWD_SITEWIDTH;}
(?i:Sitespacing) {return token::KWD_SITESPACING;}
(?i:Siteorient) {return token::KWD_SITEORIENT;}
(?i:Sitesymmetry) {return token::KWD_SITESYMMETRY;}
(?i:SubrowOrigin) {return token::KWD_SUBROWORIGIN;}
(?i:NumSites) {return token::KWD_NUMSITES;}
(?i:End) {return token::KWD_END;}
(?i:terminal) {return token::KWD_TERMINAL;}
(?i:UCLA) {return token::KWD_UCLA;}
(?i:NetDegree) {return token::KWD_NETDEGREE;}
(?i:scl) {return token::KWD_SCL;}
(?i:nodes) {return token::KWD_NODES;}
(?i:nets) {return token::KWD_NETS;}
(?i:pl) {return token::KWD_PL;}
(?i:wts) {return token::KWD_WTS;}
(?i:aux) {return token::KWD_AUX;}
(?i:[\/]?FIXED) {return token::KWD_FIXED;}
(?i:[\/]?FIXED_NI) {return token::KWD_FIXED_NI;}
(?i:[\/]?PLACED) {return token::KWD_PLACED;}
(?i:[\/]?UNPLACED) {return token::KWD_UNPLACED;}
(?i:O) {return token::KWD_O;}
(?i:I) {return token::KWD_I;}
(?i:B) {return token::KWD_B;}
(?i:N) {return token::KWD_N;}
(?i:S) {return token::KWD_S;}
(?i:W) {return token::KWD_W;}
(?i:E) {return token::KWD_E;}
(?i:FN) {return token::KWD_FN;}
(?i:FS) {return token::KWD_FS;}
(?i:FW) {return token::KWD_FW;}
(?i:FE) {return token::KWD_FE;}


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

[A-Za-z][A-Za-z0-9_,.\$\-\[\]]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}

\"([^"])*\" {
    yylval->quoteVal = new std::string(yytext+1, yyleng-2);
    return token::QUOTE;
}

[A-Za-z0-9_,.\$\-\[\]\/\(\)]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::PATH;
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
     return token::EOL; 
}

 /* pass all other characters up to bison */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the example lexer rules above ***/

%% /*** Additional Code ***/

namespace BookshelfParser {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : BookshelfParserFlexLexer(in, out)
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

int BookshelfParserFlexLexer::yylex()
{
    std::cerr << "in BookshelfParserFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int BookshelfParserFlexLexer::yywrap()
{
    return 1;
}

/* $Id: parser.yy 48 2009-09-05 08:07:10Z tb $ -*- mode: c++ -*- */
/** \file parser.yy Contains the example Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

/*#include "expression.h"*/

%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="GdfParser"

/* set the parser's class identifier */
%define "parser_class_name" "Parser"

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }

/* verbose error messages */
%error-verbose

 /*** BEGIN EXAMPLE - Change the example grammar's tokens below ***/

%union {
    int  			integerVal;
    double 			doubleVal;
    char            charVal;
    double          numberVal;
    std::string*		stringVal;
	std::string*		quoteVal;
	std::string*		binaryVal;

	class NumberArray* numberArrayVal;
	class StringArray* stringArrayVal;
}

%token			END	     0	"end of file"
/*%token			EOL		"end of line" */
%token <integerVal> 	INTEGER		"integer"
%token <doubleVal> 	DOUBLE		"double"
%token <stringVal> 	STRING		"string"
%token <quoteVal> 	QUOTE		"quoted chars"
%token <stringVal> 	PATH		"path"
%token <binaryVal> 	BINARY		"binary numbers"
%token          KWD_GDIF         "GDIF"
%token          KWD_GDIFVERSION         "GDIFVERSION"
%token          KWD_COMMENT         "COMMENT"
%token          KWD_CELL         "CELL"
%token          KWD_PORT         "PORT"
%token          KWD_TYPE         "TYPE"
%token          KWD_LAYER         "LAYER"
%token          KWD_PT         "PT"
%token          KWD_PATH         "PATH"
%token          KWD_IN         "IN"
%token          KWD_OUT         "OUT"
%token          KWD_INOUT         "INOUT"
%token          KWD_POWER         "POWER"
%token          KWD_GROUND         "GROUND"
%token          KWD_NEW         "NEW"
%token          KWD_WIDTH         "WIDTH"
%token          KWD_HEIGHT         "HEIGHT"
%token          KWD_HORIZONTAL         "HORIZONTAL"
%token          KWD_VERTICAL         "VERTICAL"
%token          KWD_INSTANCE         "INSTANCE"
%token          KWD_CELLREF         "CELLREF"
%token          KWD_ORIENT         "ORIENT"
%token          KWD_TEXT         "TEXT"
%token          KWD_NUMBER_OF_LAYERS         "NUMBER_OF_LAYERS"
%token          KWD_WIRE_SPACINGS         "WIRE_SPACINGS"
%token          KWD_VIA_SPACINGS         "VIA_SPACINGS"
%token          KWD_WIRE_WIDTHS         "WIRE_WIDTHS"
%token          KWD_VIA_WIDTHS         "VIA_WIDTHS"
%token          KWD_VERTICAL_WIRE_COSTS         "VERTICAL_WIRE_COSTS"
%token          KWD_HORIZONTAL_WIRE_COSTS         "HORIZONTAL_WIRE_COSTS"
%token          KWD_VIA_COSTS         "VIA_COSTS"
%token          KWD_NET         "NET"
%token          KWD_PORTREF         "PORTREF"
%token          KWD_INSTREF         "INSTREF"
%token          KWD_SEGMENT         "SEGMENT"
%token          KWD_VIA         "VIA"

%type <numberArrayVal> number_array 
/* %type <stringArrayVal> string_array */
%type <numberVal>  NUMBER      

%destructor { delete $$; } STRING QUOTE BINARY 
%destructor { delete $$; } number_array /*string_array*/ 

 /*** END EXAMPLE - Change the example grammar's tokens above ***/

%{

#include "GdfDriver.h"
#include "GdfScanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the example grammar rules below ***/

NUMBER : INTEGER {$$ = $1;}
       | DOUBLE {$$ = $1;}
       ;

number_array : NUMBER {
				$$ = new NumberArray(1, $1);
			  }
			  | number_array NUMBER {
				$1->push_back($2);
				$$ = $1;
			  }
/*
string_array : STRING {
				$$ = new StringArray(1, *$1);
                delete $1;
			  }
			  | string_array STRING {
				$1->push_back(*$2);
                delete $2;
				$$ = $1;
			  }
              ;
*/

port_entry : '(' KWD_PORT ':' STRING '(' KWD_TYPE KWD_IN ')' '(' KWD_LAYER STRING ')' '(' KWD_PT NUMBER NUMBER ')' ')' {
           driver.cellPortCbk(*$4, CellPort::IN, *$11, $15, $16);
           delete $4;
           delete $11;
           }
           | '(' KWD_PORT ':' STRING '(' KWD_TYPE KWD_OUT ')' '(' KWD_LAYER STRING ')' '(' KWD_PT NUMBER NUMBER ')' ')'{
           driver.cellPortCbk(*$4, CellPort::OUT, *$11, $15, $16);
           delete $4;
           delete $11;
           }
           | '(' KWD_PORT ':' STRING '(' KWD_TYPE KWD_INOUT ')' '(' KWD_LAYER STRING ')' '(' KWD_PT NUMBER NUMBER ')' ')'{
           driver.cellPortCbk(*$4, CellPort::INOUT, *$11, $15, $16);
           delete $4;
           delete $11;
           }
           | '(' KWD_PORT ':' STRING '(' KWD_TYPE KWD_POWER ')' '(' KWD_LAYER STRING ')' '(' KWD_PT NUMBER NUMBER ')' ')'{
           driver.cellPortCbk(*$4, CellPort::POWER, *$11, $15, $16);
           delete $4;
           delete $11;
           }
           | '(' KWD_PORT ':' STRING '(' KWD_TYPE KWD_GROUND ')' '(' KWD_LAYER STRING ')' '(' KWD_PT NUMBER NUMBER ')' ')'{
           driver.cellPortCbk(*$4, CellPort::GROUND, *$11, $15, $16);
           delete $4;
           delete $11;
           }
           ; 

instance_entry : '(' KWD_INSTANCE ':' STRING '(' KWD_CELLREF STRING ')' '(' KWD_PT NUMBER NUMBER ')' '(' KWD_ORIENT INTEGER ')' ')' {
               driver.cellInstanceCbk(*$4, *$7, $11, $12, $16);
               delete $4;
               delete $7;
               }
               ;

text_entry : '(' KWD_TEXT ':' KWD_NUMBER_OF_LAYERS QUOTE ')' {
           driver.textCbk(Text::NUMBER_OF_LAYERS, "NUMBER_OF_LAYERS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' KWD_WIRE_SPACINGS QUOTE ')' {
           driver.textCbk(Text::WIRE_SPACINGS, "WIRE_SPACINGS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' KWD_VIA_SPACINGS QUOTE ')' {
           driver.textCbk(Text::VIA_SPACINGS, "VIA_SPACINGS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' KWD_WIRE_WIDTHS QUOTE ')' {
           driver.textCbk(Text::WIRE_WIDTHS, "WIRE_WIDTHS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' KWD_VIA_WIDTHS QUOTE ')' {
           driver.textCbk(Text::VIA_WIDTHS, "VIA_WIDTHS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' KWD_VERTICAL_WIRE_COSTS QUOTE ')' {
           driver.textCbk(Text::VERTICAL_WIRE_COSTS, "VERTICAL_WIRE_COSTS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' KWD_HORIZONTAL_WIRE_COSTS QUOTE ')' {
           driver.textCbk(Text::HORIZONTAL_WIRE_COSTS, "HORIZONTAL_WIRE_COSTS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' KWD_VIA_COSTS QUOTE ')' {
           driver.textCbk(Text::VIA_COSTS, "VIA_COSTS", *$5);
           delete $5;
           }
           | '(' KWD_TEXT ':' STRING QUOTE ')' {
           driver.textCbk(Text::STRING, *$4, *$5);
           delete $4;
           delete $5;
           }
           ;

pathobj_entry : '(' KWD_SEGMENT ')' '(' KWD_LAYER STRING ')' '(' KWD_WIDTH NUMBER ')'  '(' KWD_PT NUMBER NUMBER ')'  '(' KWD_PT NUMBER NUMBER ')' {
              driver.pathObjCbk(PathObj::SEGMENT, "", *$6, $10, $14, $15, $19, $20);
              delete $6;
              }
              | '(' KWD_VIA ')' '(' KWD_LAYER STRING ')' '(' KWD_WIDTH NUMBER ')'  '(' KWD_PT NUMBER NUMBER ')'  '(' KWD_PT NUMBER NUMBER ')' {
              driver.pathObjCbk(PathObj::VIA, "", *$6, $10, $14, $15, $19, $20);
              delete $6;
              }
              | '(' KWD_NEW ')' '(' KWD_LAYER STRING ')' '(' KWD_WIDTH NUMBER ')'  '(' KWD_PT NUMBER NUMBER ')'  '(' KWD_PT NUMBER NUMBER ')' {
              driver.pathObjCbk(PathObj::STRING, "new", *$6, $10, $14, $15, $19, $20);
              delete $6;
              };
pathobj_entres : pathobj_entry 
               | pathobj_entres pathobj_entry
               ;
path_entry : '(' KWD_PATH ':' STRING {
           driver.pathCbk(*$4);
           delete $4;
           } 
           pathobj_entres ')' 
           ;

net_port_entry : '(' KWD_PORTREF STRING '(' KWD_INSTREF STRING ')'  ')'/* connect to instance  */ {
               driver.netPortCbk(*$3, *$6);
               delete $3;
               delete $6;
               }
               | '(' KWD_PORTREF STRING  ')'/* connect to PI/PO */ {
               driver.netPortCbk(*$3);
               delete $3;
               }
               ; 
net_port_entries : net_port_entry
             | net_port_entries net_port_entry
             ;

net_entry : '(' KWD_NET ':' STRING {
          driver.netCbk(*$4);
          delete $4;
          } net_port_entries  ')'
          ; 

gdif_version_entry  : '(' KWD_GDIFVERSION INTEGER INTEGER INTEGER ')'
                    ; 

cell_addon : port_entry
           | path_entry
           | instance_entry
           | text_entry
           | net_entry
           ; 

cell_addons : cell_addon 
            | cell_addons cell_addon 
            ;

cell_entry : '(' KWD_CELL ':' STRING cell_addons ')' {
           driver.cellCbk(*$4);
           delete $4;
           }
              ;
cell_entries : cell_entry 
             | cell_entries cell_entry
             ;

 /*** grammar for top file ***/
start : '(' KWD_GDIF gdif_version_entry cell_entries ')'
	  ;

 /*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void GdfParser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}

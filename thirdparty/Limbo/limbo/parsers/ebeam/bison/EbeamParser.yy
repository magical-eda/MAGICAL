/* $Id: parser.yy 48 2009-09-05 08:07:10Z tb $ -*- mode: c++ -*- */
/** \file parser.yy Contains the example Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#include "EbeamDataBase.h"

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
%name-prefix="EbeamParser"

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
    std::string*		stringVal;
	std::string*		quoteVal;

	class IntegerArray* integerArrayVal;
	class StringArray* stringArrayVal;

/*    class CalcNode*		calcnode; */
}

%token			END	     0	"end of file"
/* %token			EOL		"end of line" */
%token <integerVal> 	INTEGER		"integer"
%token <doubleVal> 	DOUBLE		"double"
%token <stringVal> 	STRING		"string"
%token <quoteVal> 	QUOTE		"quoted chars"
%token			KWD_END		"END"
%token			KWD_UNITS		"UNITS"
%token			KWD_DATABASE		"DATABASE"
%token			KWD_DISTANCE	"DISTANCE"
%token			KWD_MICRONS		"MICRONS"
%token			KWD_OFFSET		"OFFSET"
%token			KWD_WIDTH		"WIDTH"
%token			KWD_STEP		"STEP"
%token			KWD_LAYERID		"LAYERID"
%token			KWD_LAYER		"LAYER"
%token			KWD_MACRO		"MACRO"
%token		KWD_EBEAMBOUNDARY	"EBEAMBOUNDARY"
%token		KWD_CONFLICTSITE	"CONFLICTSITE"
%token			KWD_SITE		"SITE"
%token			KWD_LIBRARY		"LIBRARY"

%type <integerArrayVal> integer_array 
%type <stringArrayVal> string_array
%type <doubleVal> number 

/*
%type <integerVal>	block_other block_row block_comp block_pin block_net 
%type <integerVal>	expression 
*/

%destructor { delete $$; } STRING QUOTE
%destructor { delete $$; } integer_array string_array 
/*
%destructor { delete $$; } constant variable
%destructor { delete $$; } atomexpr powexpr unaryexpr mulexpr addexpr expr
*/

 /*** END EXAMPLE - Change the example grammar's tokens above ***/

%{

#include "EbeamDriver.h"
#include "EbeamScanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the example grammar rules below ***/

/* the definition of DOUBLE needs the decimal point, but sometimes a floating point can also be an INTEGER  */
number : INTEGER {
		$$ = $1;
	   }
	   | DOUBLE {
		$$ = $1;
	   }
integer_array : INTEGER {
				$$ = new IntegerArray(1, $1);
			  }
			  | integer_array INTEGER {
				$1->push_back($2);
				$$ = $1;
			  }
string_array : STRING {
				$$ = new StringArray(1, *$1); delete $1;
			  }
			  | string_array STRING {
				$1->push_back(*$2); delete $2;
				$$ = $1;
			  }

 /*** grammar for units ***/
block_units : KWD_UNITS 
		   KWD_DATABASE KWD_MICRONS INTEGER ';'
		   KWD_END KWD_UNITS {
			driver.units_cbk($4);
		   }
			
 /*** grammar for EbeamBoundary ***/
entry_offset : KWD_OFFSET number ';' {
			 driver.ebeam_cbk_offset($2);
			 }
entry_width : KWD_WIDTH number ';' {
			 driver.ebeam_cbk_width($2);
			}
entry_step : KWD_STEP number ';' {
			 driver.ebeam_cbk_step($2);
		   }
entry_layer : KWD_LAYERID integer_array ';' {
				driver.ebeam_cbk_layerid(*$2);
                delete $2;
			}
			| KWD_LAYER string_array ';' {
				driver.ebeam_cbk_layer(*$2);
			}
block_ebeam : KWD_EBEAMBOUNDARY 
			entry_offset 
			entry_width 
			entry_step 
			entry_layer 
			KWD_END KWD_EBEAMBOUNDARY {
				driver.ebeam_cbk();
			}

 /*** grammar for macros ***/
begin_macro : KWD_MACRO STRING {
				driver.macro_cbk_name(*$2);
                delete $2;
			}
end_macro : KWD_END STRING {
			driver.macro_cbk(*$2);
            delete $2;
		  }
single_confsite : KWD_CONFLICTSITE STRING
			   KWD_LAYERID INTEGER ';'
			   KWD_SITE integer_array ';'
			   KWD_END STRING {
				driver.macro_cbk_confsite_name(*$2);
				driver.macro_cbk_confsite_layerid($4);
				driver.macro_cbk_confsite_site(*$7);
				driver.macro_cbk_confsite(*$10);
                delete $2; delete $7; delete $10;
			   }
			   | KWD_CONFLICTSITE STRING
			   KWD_LAYER STRING ';'
			   KWD_SITE integer_array ';'
			   KWD_END STRING {
				driver.macro_cbk_confsite_name(*$2);
				driver.macro_cbk_confsite_layer(*$4);
				driver.macro_cbk_confsite_site(*$7);
				driver.macro_cbk_confsite(*$10);
                delete $2; delete $4; delete $7; delete $10;
			   }
multiple_confsites : single_confsite 
				   | multiple_confsites single_confsite 
				   ;
block_macro : begin_macro 
			multiple_confsites 
			end_macro
			;

 /*** grammar for top file ***/
start : 
	  | start block_units
	  | start block_ebeam 
	  | start block_macro
	  | start KWD_END KWD_LIBRARY
	  ;

 /*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void EbeamParser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}

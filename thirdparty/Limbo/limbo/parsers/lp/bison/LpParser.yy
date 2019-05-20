/* $Id: parser.yy 48 2009-09-05 08:07:10Z tb $ -*- mode: c++ -*- */
/** \file parser.yy Contains the example Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#include <limbo/parsers/lp/bison/LpDataBase.h>

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
%name-prefix="LpParser"

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
    int64_t 		integerVal;
    double 			doubleVal;
    char            charVal; 
    std::string*		stringVal;
	std::string*		quoteVal;

	Term* termVal;
    IntegerArray* integerArrayVal;
	StringArray* stringArrayVal;
    TermArray* termArrayVal; 
}

%token			END	     0	"end of file"
/* %token			EOL		"end of line" */
%token <integerVal> 	INTEGER		"integer"
%token <doubleVal> 	DOUBLE		"double"
%token <stringVal> 	STRING		"string"
%token			KWD_TO			"TO"
%token			KWD_END			"END"
%token			KWD_MINIMIZE	"MINIMIZE"
%token			KWD_MAXIMIZE	"MAXIMIZE"
%token			KWD_SUBJECT		"SUBJECT"
%token			KWD_BOUNDS		"BOUNDS"
%token			KWD_GENERALS	"GENERALS"
%token			KWD_BINARY		"BINARY"
%token <charVal>	KWD_COMPARE	"COMPARE"
%token <integerVal>	KWD_OP		"OP"

%type <doubleVal> number
%type <stringArrayVal> string_array
%type <termVal> term;
%type <termArrayVal> multiple_terms; 
/*
%type <integerVal>	block_other block_row block_comp block_pin block_net 
%type <integerVal>	expression 
*/

%destructor { delete $$; } STRING 
%destructor { delete $$; } string_array term multiple_terms
/*
%destructor { delete $$; } constant variable
%destructor { delete $$; } atomexpr powexpr unaryexpr mulexpr addexpr expr
*/

 /*** END EXAMPLE - Change the example grammar's tokens above ***/

%{

#include <limbo/parsers/lp/bison/LpDriver.h>
#include <limbo/parsers/lp/bison/LpScanner.h>

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the example grammar rules below ***/

number : INTEGER {
       $$ = $1;
       }
       | DOUBLE {
       $$ = $1; 
       }

string_array : STRING {
				$$ = new StringArray(1, *$1);
			  }
			  | string_array STRING {
				$1->push_back(*$2);
				$$ = $1;
			  }

term : STRING {
		$$ = new Term (1, *$1);
	 }
	 | number STRING {
		$$ = new Term ($1, *$2);
	 }
	 ;

 /*** grammar for obj ***/
multiple_terms : term {
               $$ = new TermArray(1, *$1); 
			   }
			| KWD_OP term {
               $$ = new TermArray(1, Term($1*$2->coef, $2->var)); 
			}
		  | multiple_terms KWD_OP term {
          $1->push_back(Term($2*$3->coef, $3->var)); 
          $$ = $1; 
		  }
		  ;

block_obj : KWD_MINIMIZE multiple_terms {
          driver.obj_cbk(true, *$2); 
          }
          | KWD_MAXIMIZE multiple_terms {
          driver.obj_cbk(false, *$2); 
          }
		  ;

 /*** grammar for constraints ***/
single_constraint : multiple_terms KWD_COMPARE number {
                  driver.constraint_cbk(*$1, $2, $3);
				 } 
                 | STRING ':' multiple_terms KWD_COMPARE number {
                  driver.constraint_cbk(*$1, *$3, $4, $5);
				 } 
multiple_constraints : single_constraint
					| multiple_constraints single_constraint
					;
block_constraints : KWD_SUBJECT KWD_TO multiple_constraints
				  ;
				 
 /*** grammar for bounds ***/
single_bound : STRING KWD_COMPARE number {
				driver.bound_cbk(*$1, $2, $3);
			 }
			 | number KWD_COMPARE STRING {
				driver.bound_cbk($1, $2, *$3);
			 }
			 | number KWD_COMPARE STRING KWD_COMPARE number {
                driver.bound_cbk($1, $2, *$3, $4, $5);
			 }
			 ;
multiple_bounds : single_bound
					| multiple_bounds single_bound
					;
block_bounds : KWD_BOUNDS 
			 | KWD_BOUNDS multiple_bounds
				  ;

 /*** grammar for integer ***/
block_generals : KWD_GENERALS
			   | KWD_GENERALS string_array {
				driver.generals_cbk(*$2);
			   }
			  ;

 /*** grammar for binary ***/
block_binary : KWD_BINARY 
			 | KWD_BINARY string_array {
				driver.binary_cbk(*$2);
			 }
			  ;

 /*** grammar for top lp ***/
block_lp : block_obj
		 block_constraints
		 block_bounds
		 | block_lp block_generals
		 | block_lp block_binary
		 ;

 /*** grammar for top file ***/
start : block_lp
	  KWD_END 
	  ;

 /*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void LpParser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}

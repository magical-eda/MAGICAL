/* $Id: parser.yy 48 2009-09-05 08:07:10Z tb $ -*- mode: c++ -*- */
/** \file parser.yy Contains the example Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

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
%name-prefix="VerilogParser"

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
    int integerVal;
    std::string* stringVal;
	class StringArray* stringArrayVal;
    class GeneralNameArray* generalNameArrayVal; 
    struct {
        long value;
        long bits;
    } mask;
    struct Range* rangeVal;
/*    struct Range {
        int low;
        int high;
    } rangeVal;
    */
}

%token ALWAYS 
%token EQUAL
%token POSEDGE
%token NEGEDGE
%token OR
%token BEG
%token END 0 "end of file"
%token AND
%token IF
%token ELSE
%token INIT
%token AT
%token NAME
%token NUM
%token MODULE
%token ENDMODULE
%token INPUT
%token OUTPUT
%token INOUT
%token REG
%token WIRE
%token INTEGER
%token ASSIGN
%token TIME
%token BIT_MASK;
%token OCT_MASK;
%token DEC_MASK;
%token HEX_MASK;

%left OR
%left AND

%type<stringVal> NAME
%type<mask> BIT_MASK OCT_MASK DEC_MASK HEX_MASK
%type<integerVal> NUM 
%type<rangeVal> range
/*%type <stringArrayVal> name_array*/
%type <generalNameArrayVal> general_name_array

%destructor {delete $$;} NAME 
%destructor {delete $$;} range
/*%destructor {delete $$;} name_array*/
%destructor {delete $$;} general_name_array

 /*** END EXAMPLE - Change the example grammar's tokens above ***/

%{

#include "VerilogDriver.h"
#include "VerilogScanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the example grammar rules below ***/

range: '[' NUM ':' NUM ']' {$$ = new Range ($2, $4);}
     | '[' NUM ']' {$$ = new Range (std::numeric_limits<int>::min(), $2);}

/*
name_array: NAME {
          $$ = new StringArray(1, *$1);
          delete $1;
          }
          | name_array ',' NAME {
            $1->push_back(std::string());
            $1->back().swap(*$3);
            delete $3;
            $$ = $1;
          }
          ;
*/

/* general name array may have range after name */
general_name_array: NAME {
          $$ = new GeneralNameArray(1, GeneralName(*$1));
          delete $1;
        }
        | NAME range {
          $$ = new GeneralNameArray(1, GeneralName(*$1, $2->low, $2->high));
          delete $1;
          delete $2; 
        }
        | general_name_array ',' NAME {
            $1->push_back(GeneralName(*$3));
            delete $3;
            $$ = $1;
        }
        | general_name_array ',' NAME range {
            $1->push_back(GeneralName(*$3, $4->low, $4->high));
            delete $3;
            delete $4; 
            $$ = $1;
        }

param1: NAME {delete $1;}
      | NAME range {delete $1; delete $2;} 
      ;

/* wire_pin_cbk will be called before module_instance_cbk */
param2: '.' NAME '(' NAME ')' {driver.wire_pin_cbk(*$4, *$2); delete $2; delete $4;}
      | '.' NAME '(' NAME range ')' {driver.wire_pin_cbk(*$4, *$2, *$5); delete $2; delete $4; delete $5;}
      | '.' NAME '(' ')' {delete $2;} /* allow floating pin */
      ;

param3: INPUT general_name_array {driver.pin_declare_cbk(*$2, kINPUT); delete $2;}
      | INPUT range general_name_array {driver.pin_declare_cbk(*$3, kINPUT, *$2); delete $2; delete $3;} 
      | INPUT REG range general_name_array {driver.pin_declare_cbk(*$4, kINPUT|kREG, *$3); delete $3; delete $4;}
      | INPUT REG general_name_array {driver.pin_declare_cbk(*$3, kINPUT|kREG); delete $3;}
      | OUTPUT general_name_array {driver.pin_declare_cbk(*$2, kOUTPUT); delete $2;}
      | OUTPUT range general_name_array {driver.pin_declare_cbk(*$3, kOUTPUT, *$2); delete $2; delete $3;}
      | OUTPUT REG range general_name_array {driver.pin_declare_cbk(*$4, kOUTPUT|kREG, *$3); delete $3; delete $4;}
      | OUTPUT REG general_name_array {driver.pin_declare_cbk(*$3, kOUTPUT|kREG); delete $3;}
      | INOUT general_name_array {driver.pin_declare_cbk(*$2, kINPUT|kOUTPUT); delete $2;}
      | INOUT range general_name_array {driver.pin_declare_cbk(*$3, kINPUT|kOUTPUT, *$2); delete $2; delete $3;}
      | INOUT REG range general_name_array {driver.pin_declare_cbk(*$4, kINPUT|kOUTPUT|kREG, *$3); delete $3; delete $4;}
      | INOUT REG general_name_array {driver.pin_declare_cbk(*$3, kINPUT|kOUTPUT|kREG); delete $3;}
      ;

param4: REG general_name_array {delete $2;}
      | REG range general_name_array {delete $2; delete $3;}
      ;

param5: WIRE general_name_array {driver.wire_declare_cbk(*$2); delete $2;}
      | WIRE range general_name_array {driver.wire_declare_cbk(*$3, *$2); delete $2; delete $3;}
      ;


module_param: param1 
            | param3
      ;

module_params: /* empty */
             | module_param 
             | module_params ',' module_param 
             ;

module_declare: MODULE NAME '(' module_params ')' ';' {delete $2;}
              ;

variable_declare: param3 ';'
                | param4 ';'
                | param5 ';'
                ;

/* do not support param1 yet */
instance_params: /* empty */
               | param2 
               | instance_params ',' param2 
               ;

module_instance: NAME NAME '(' instance_params ')' ';' {driver.module_instance_cbk(*$1, *$2); delete $1; delete $2;}
               | NAME NAME '[' NUM ']' '(' instance_params ')' ';' {
               /* append NUM to instance name */
               char buf[256];
               sprintf(buf, "[%d]", $4);
               $2->append(buf);
               driver.module_instance_cbk(*$1, *$2); 
               delete $1; 
               delete $2;
               }
               ;

assignment: ASSIGN NAME '=' NAME ';' {driver.assignment_cbk(*$2, Range(), *$4, Range()); delete $2; delete $4;}
          | ASSIGN NAME range '=' NAME ';' {driver.assignment_cbk(*$2, *$3, *$5, Range()); delete $2; delete $3; delete $5;}
          | ASSIGN NAME '=' NAME range ';' {driver.assignment_cbk(*$2, Range(), *$4, *$5); delete $2; delete $4; delete $5;}
          | ASSIGN NAME range '=' NAME range ';' {driver.assignment_cbk(*$2, *$3, *$5, *$6); delete $2; delete $3; delete $5; delete $6;}

module_content: /* empty */
              | module_content variable_declare
              | module_content module_instance 
              | module_content assignment 
              ;


single_module: module_declare module_content ENDMODULE 
             ;

start : /* empty */
     | start single_module
     ;

 /*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void VerilogParser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}

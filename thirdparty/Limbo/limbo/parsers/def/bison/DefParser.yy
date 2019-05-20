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
%name-prefix="DefParser"

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
	std::string*		binaryVal;

/*	class IntegerArray* integerArrayVal;*/
	class StringArray* stringArrayVal;
}

%token			END	     0	"end of file"
/* %token			EOL		"end of line" */
%token <integerVal> 	INTEGER		"integer"
%token <doubleVal> 	DOUBLE		"double"
%token <stringVal> 	STRING		"string"
%token <quoteVal> 	QUOTE		"quoted chars"
%token <binaryVal> 	BINARY		"binary numbers"
/*%type <integerArrayVal> integer_array */
%type <stringArrayVal> string_array
%token			KWD_VERSION		"VERSION"
%token			KWD_DIVIDERCHAR	"DIVIDERCHAR"
%token			KWD_BUSBITCHARS	"BUSBITCHARS"
%token			KWD_DESIGN		"DESIGN"
%token			KWD_UNITS		"UNITS"
%token			KWD_DISTANCE	"DISTANCE"
%token			KWD_MICRONS		"MICRONS"
%token			KWD_DIEAREA		"DIEAREA"
%token			KWD_ROW			"ROW"
%token			KWD_DO			"DO"
%token			KWD_BY			"BY"
%token			KWD_COMPONENTS	"COMPONENTS"
%token			KWD_PINS		"PINS"
%token			KWD_NETS		"NETS"
%token			KWD_NET			"NET"
%token			KWD_STEP		"STEP"
%token			KWD_END			"END"
%token			KWD_DIRECTION	"DIRECTION"
%token			KWD_LAYER		"LAYER"
%token			KWD_PROPERTYDEFINITIONS		"PROPERTYDEFINITIONS"
%token			KWD_COMPONENTPIN		"COMPONENTPIN"
%token			KWD_TRACKS		"TRACKS"
%token			KWD_GCELLGRID		"GCELLGRID"
%token			KWD_VIAS		"VIAS"
%token			KWD_VIA		"VIA"
%token			KWD_VIARULE		"VIARULE"
%token			KWD_CUTSIZE		"CUTSIZE"
%token			KWD_LAYERS		"LAYERS"
%token			KWD_ROWCOL		"ROWCOL"
%token			KWD_ENCLOSURE		"ENCLOSURE"
%token			KWD_CUTSPACING		"CUTSPACING"
%token			KWD_USE		"USE"
%token			KWD_SPECIALNETS		"SPECIALNETS"
%token			KWD_SHAPE		"SHAPE"
%token			KWD_RECT		"RECT"
%token			KWD_SOURCE		"SOURCE"
%token			KWD_NONDEFAULTRULES		"NONDEFAULTRULES"
%token			KWD_NONDEFAULTRULE		"NONDEFAULTRULE"
%token			KWD_HARDSPACING		"HARDSPACING"
%token			KWD_WIDTH		"WIDTH"
%token			KWD_SPACING		"SPACING"
%token			KWD_REGIONS		"REGIONS"
%token			KWD_REGION		"REGION"
%token			KWD_TYPE		"TYPE"
%token			KWD_FENCE		"FENCE"
%token			KWD_GROUPS		"GROUPS"
%token			KWD_GROUP		"GROUP"
%token			KWD_BLOCKAGES		"BLOCKAGES"
%token			KWD_PLACEMENT		"PLACEMENT"
%token			KWD_ROUTING		"ROUTING"

/*
%type <integerVal>	block_other block_row block_comp block_pin block_net 
%type <integerVal>	expression 
*/

%destructor { delete $$; } STRING QUOTE BINARY
%destructor { delete $$; } /*integer_array*/ string_array 
/*
%destructor { delete $$; } constant variable
%destructor { delete $$; } atomexpr powexpr unaryexpr mulexpr addexpr expr
*/

 /*** END EXAMPLE - Change the example grammar's tokens above ***/

%{

#include "DefDriver.h"
#include "DefScanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the example grammar rules below ***/

/*
integer_array : INTEGER {
				$$ = new IntegerArray(1, $1);
			  }
			  | integer_array INTEGER {
				$1->push_back($2);
				$$ = $1;
			  }
*/
string_array : STRING {
				$$ = new StringArray(1, *$1);
                delete $1;
			  }
			  | string_array STRING {
				$1->push_back(*$2);
                delete $2;
				$$ = $1;
			  }

block_other : KWD_VERSION DOUBLE ';' {
				driver.version_cbk($2);
			}
			| KWD_DIVIDERCHAR QUOTE ';' {
				driver.dividerchar_cbk(*$2);
                delete $2;
			}
			| KWD_BUSBITCHARS QUOTE ';' {
				driver.busbitchars_cbk(*$2);
                delete $2;
			}
			
 /*** grammar for rows ***/
single_row : KWD_ROW STRING STRING INTEGER INTEGER STRING KWD_DO INTEGER KWD_BY INTEGER KWD_STEP INTEGER INTEGER ';' {
				driver.row_cbk(*$2, *$3, $4, $5, *$6, $8, $10, $12, $13);
                delete $2;
                delete $3;
                delete $6;
			 }

block_rows : single_row 
		  | block_rows single_row 
		  ;

 /*** grammar for tracks ***/
single_tracks : KWD_TRACKS STRING INTEGER KWD_DO INTEGER KWD_STEP INTEGER KWD_LAYER string_array ';' {
				driver.track_cbk(*$2, $3, $5, $7, *$9);
                delete $2;
                delete $9;
			 }

block_tracks : single_tracks 
		  | block_tracks single_tracks 
		  ;

 /*** grammar for gcellgrid ***/
single_gcellgrid : KWD_GCELLGRID STRING INTEGER KWD_DO INTEGER KWD_STEP INTEGER ';' {
				driver.gcellgrid_cbk(*$2, $3, $5, $7);
                delete $2;
			 }

block_gcellgrid : single_gcellgrid 
		  | block_gcellgrid single_gcellgrid 
		  ;

 /*** grammar for vias ***/
begin_vias : KWD_VIAS INTEGER ';'
		   ;

end_vias : KWD_END KWD_VIAS 
		 ;

via_addon : /* empty */
		  | via_addon '+' KWD_VIARULE STRING {delete $4;}
		  | via_addon '+' KWD_CUTSIZE INTEGER INTEGER
		  | via_addon '+' KWD_LAYERS string_array {delete $4;}
		  | via_addon '+' KWD_CUTSPACING INTEGER INTEGER 
		  | via_addon '+' KWD_ENCLOSURE INTEGER INTEGER INTEGER INTEGER 
		  | via_addon '+' KWD_RECT STRING '(' INTEGER INTEGER ')' '(' INTEGER INTEGER ')' {delete $4;}
		  | via_addon '+' KWD_ROWCOL INTEGER INTEGER 
		  ;

single_via : '-' STRING via_addon ';' {delete $2;}
		   ;

multiple_vias : single_via 
			 | multiple_vias single_via
			 ;

block_vias : begin_vias multiple_vias end_vias
		   | begin_vias end_vias 
		   ;

/*** grammar for nondefault rules ***/
begin_nondefaultrules : KWD_NONDEFAULTRULES INTEGER ';'
                       ;
end_nondefaultrules : KWD_END KWD_NONDEFAULTRULES
                    ;

nondefaultrule_addon : /* empty */
           | nondefaultrule_addon '+' KWD_HARDSPACING
           | nondefaultrule_addon '+' KWD_LAYER STRING KWD_WIDTH INTEGER KWD_SPACING INTEGER {delete $4;}
           | nondefaultrule_addon '+' KWD_VIA STRING {delete $4;}
           ;

single_nondefaultrule : '-' STRING nondefaultrule_addon ';' {delete $2;}
                      ;

multiple_nondefaultrules : single_nondefaultrule
                         | multiple_nondefaultrules single_nondefaultrule
                         ;

block_nondefaultrules : begin_nondefaultrules multiple_nondefaultrules end_nondefaultrules
                      | begin_nondefaultrules end_nondefaultrules
                      ; 

/*** grammar for regions ***/
begin_regions : KWD_REGIONS INTEGER ';'
              ;
end_regions : KWD_END KWD_REGIONS
            ;

region_addon : /* empty */
              | region_addon '+' KWD_TYPE KWD_FENCE
              ;

region_points : '(' INTEGER INTEGER ')'
              | region_points '(' INTEGER INTEGER ')'
             ;

single_region : '-' STRING region_points region_addon ';' {delete $2;}
              ;

multiple_regions : single_region
                 | multiple_regions single_region
                 ;

block_regions : begin_regions multiple_regions end_regions
              | begin_regions end_regions
              ;

 /*** grammar for components ***/
begin_components : KWD_COMPONENTS INTEGER ';' {
					driver.component_cbk_size($2);
				 }

end_components : KWD_END KWD_COMPONENTS 
			   ;

component_addon : /* empty */
				| component_addon '+' STRING '(' INTEGER INTEGER ')' STRING {
					driver.component_cbk_position(*$3, $5, $6, *$8);
                    delete $3;
                    delete $8;
				}
				| component_addon '+' STRING '(' DOUBLE DOUBLE ')' STRING { /*it may be double in some benchmarks*/
					driver.component_cbk_position(*$3, $5, $6, *$8);
                    delete $3; 
                    delete $8;
				}
				| component_addon '+' KWD_SOURCE STRING {
					driver.component_cbk_source(*$4);
                    delete $4;
				}
				| component_addon '+' STRING {
					driver.component_cbk_position(*$3);
                    delete $3;
				}
				;

single_component : '-' STRING STRING component_addon ';' {
				driver.component_cbk(*$2, *$3);
                delete $2;
                delete $3;
			}

multiple_components : single_component 
				  | multiple_components single_component
			  ;

block_components : begin_components 
				 multiple_components 
				 end_components
				 | begin_components
				 end_components
				 ;
				 
 /*** grammar for pins ***/
begin_pins : KWD_PINS INTEGER ';' {
					driver.pin_cbk_size($2);
				}

end_pins : KWD_END KWD_PINS
		 ;

pin_addon : /* empty */
		  | pin_addon '+' KWD_NET STRING {
			driver.pin_cbk_net(*$4);
            delete $4;
		  }
		  | pin_addon '+' KWD_DIRECTION STRING {
			driver.pin_cbk_direction(*$4);
            delete $4;
		  }
		  | pin_addon '+' STRING '(' INTEGER INTEGER ')' STRING {
			driver.pin_cbk_position(*$3, $5, $6, *$8);
            delete $3;
            delete $8;
		  }
		  | pin_addon '+' KWD_LAYER STRING '(' INTEGER INTEGER ')' '(' INTEGER INTEGER ')' {
			driver.pin_cbk_bbox(*$4, $6, $7, $10, $11);
            delete $4;
		  }
		  | pin_addon '+' KWD_USE STRING {
			driver.pin_cbk_use(*$4);
            delete $4;
		  }
		  ;

single_pin : '-' STRING pin_addon ';' {
			driver.pin_cbk(*$2);
            delete $2;
		   }

multiple_pins : single_pin 
			  | multiple_pins single_pin 
			  ;

block_pins : begin_pins 
		   multiple_pins 
		   end_pins
		   | begin_pins 
		   end_pins 
		   ;

 /*** grammar for blockages ***/
begin_blockages : KWD_BLOCKAGES INTEGER ';' {driver.blockage_cbk_size($2);}
                ;
end_blockages : KWD_END KWD_BLOCKAGES
              ;
single_blockage : '-' KWD_PLACEMENT KWD_RECT '(' INTEGER INTEGER ')' '(' INTEGER INTEGER ')' ';' {driver.blockage_cbk_placement($5, $6, $9, $10);}
                | '-' KWD_ROUTING KWD_RECT '(' INTEGER INTEGER ')' '(' INTEGER INTEGER ')' ';' {driver.blockage_cbk_routing($5, $6, $9, $10);}
                | '-' KWD_LAYER STRING KWD_RECT '(' INTEGER INTEGER ')' '(' INTEGER INTEGER ')' ';' {delete $3;}
                ;
multiple_blockages : single_blockage
                   | multiple_blockages single_blockage
                   ;
block_blockages : begin_blockages multiple_blockages end_blockages
                | begin_blockages end_blockages
                ;

 /*** grammar for special nets ***/
 /*** so far we do not use special nets, so I simply pass the grammar ***/
begin_specialnets : KWD_SPECIALNETS INTEGER ';'
				  ;
end_specialnets : KWD_END KWD_SPECIALNETS
				;
specialnets_metal_layer : STRING STRING INTEGER {delete $1; delete $2;}
						;
specialnets_metal_shape : '+' KWD_SHAPE STRING '(' INTEGER INTEGER ')' '(' INTEGER '*' ')' {delete $3;}
						| '+' KWD_SHAPE STRING '(' INTEGER INTEGER ')' '(' '*' INTEGER ')' {delete $3;}
						| '+' KWD_SHAPE STRING '(' INTEGER INTEGER ')' STRING {delete $3; delete $8;}
						;
specialnets_metal_array : specialnets_metal_layer specialnets_metal_shape 
						| specialnets_metal_array specialnets_metal_layer specialnets_metal_shape 
						;
specialnets_addon : /* empty */
				  | specialnets_addon '+' specialnets_metal_array
				  | specialnets_addon '+' KWD_USE STRING {delete $4;}
                  | specialnets_addon '+' KWD_RECT STRING '(' INTEGER INTEGER ')' '(' INTEGER INTEGER ')' {delete $4;}
				  ;
single_specialnet : '-' BINARY specialnets_addon ';' {delete $2;}
				  | '-' STRING specialnets_addon ';' {delete $2;}
				   ;
multiple_specialnets : single_specialnet
					 | multiple_specialnets single_specialnet
					 ;
block_specialnets : begin_specialnets multiple_specialnets end_specialnets
				  | begin_specialnets end_specialnets
				  ;

 /*** grammar for nets ***/
begin_nets : KWD_NETS INTEGER ';' {
					driver.net_cbk_size($2);
				}

end_nets : KWD_END KWD_NETS
		 ;

node_pin_pair : '(' STRING STRING ')' {
		   /** be careful, this callback will be invoked before net_cbk_name **/
				driver.net_cbk_pin(*$2, *$3); 
                delete $2;
                delete $3;
			  }

node_pin_pairs : node_pin_pair 
			   | node_pin_pairs node_pin_pair
			   ;

net_addon : /* empty */
          | net_addon '+' KWD_USE STRING {delete $4;}
          | net_addon '+' KWD_NONDEFAULTRULE STRING {delete $4;}
          ;

single_net : '-' STRING node_pin_pairs net_addon ';' {
				driver.net_cbk_name(*$2);
                delete $2;
		   } 
		   | '-' BINARY node_pin_pairs net_addon ';' {
				driver.net_cbk_name(*$2);
                delete $2;
		   } 
           ;

multiple_nets : single_net 
			  | multiple_nets single_net
			  ;

block_nets : begin_nets
		   multiple_nets 
		   end_nets
		   | begin_nets 
		   end_nets 
		   ;

 /*** grammar for property definitions ***/
 /*** additional block, usually useless for placement ***/
 /*** so no callbacks are created for it ***/
single_propterty : KWD_COMPONENTPIN STRING STRING ';' {delete $2; delete $3;}
				 | KWD_DESIGN STRING STRING DOUBLE ';' {delete $2; delete $3;}
                 | KWD_NET STRING STRING ';' {delete $2; delete $3;}
				 ;

multiple_property : single_propterty 
				  | multiple_property single_propterty
				  ;

block_propertydefinitions : KWD_PROPERTYDEFINITIONS 
						  multiple_property
						  KWD_END KWD_PROPERTYDEFINITIONS
						  | KWD_PROPERTYDEFINITIONS
						  KWD_END KWD_PROPERTYDEFINITIONS
						  ;

/*** grammar for groups ***/
begin_groups : KWD_GROUPS INTEGER ';'
             ;
end_groups : KWD_END KWD_GROUPS
           ;

group_addon : /* empty */
            | group_addon '+' KWD_REGION STRING {delete $4;}
            ;

single_group : '-' STRING string_array group_addon ';' {delete $2; delete $3;}
             ;

multiple_groups : single_group 
                | multiple_groups single_group
                ;

block_groups : begin_groups multiple_groups end_groups
             | begin_groups end_groups
             ;

 /*** grammar for top design ***/

begin_design : KWD_DESIGN STRING ';' {
				driver.design_cbk(*$2);
                delete $2;
			 }

end_design : KWD_END KWD_DESIGN
		   ;

block_unit : KWD_UNITS KWD_DISTANCE KWD_MICRONS INTEGER ';' {
				driver.unit_cbk($4);
		   }

block_diearea : KWD_DIEAREA '(' INTEGER INTEGER ')' '(' INTEGER INTEGER ')' ';' {
				driver.diearea_cbk($3, $4, $7, $8);
			  }

block_option : block_unit
			 | block_propertydefinitions
			 | block_diearea
			 | block_rows 
			 | block_tracks
			 | block_gcellgrid
			 | block_vias
             | block_nondefaultrules
             | block_regions
			 | block_components
			 | block_pins
             | block_blockages
			 | block_specialnets
			 | block_nets 
             | block_groups
			 ;

block_design : block_option
			 | block_design block_option
			 ;

 /*** grammar for top file ***/
start : 
	  | start block_other
	  | start begin_design block_design end_design
	  ;

 /*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void DefParser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}

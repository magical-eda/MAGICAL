/* $Id: parser.yy 48 2009-09-05 08:07:10Z tb $ -*- mode: c++ -*- */
/** \file parser.yy Contains the example Bison parser source */

%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>
#include <string.h>
#include "LefDataBase.h"

%}

/*
%code requires {
#include "lefiDefs.hpp"
}
*/

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start lef_file

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="LefParser"

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
	/*char*           string;*/
    std::string*		stringVal;
	std::string*		qstringVal;
	std::string*		binaryVal;

	struct lefPoint* pt; 
}

%token K_DEFINE "DEFINE"
%token K_DEFINEB "DEFINEB"
%token K_DEFINES "DEFINES"
%token K_MESSAGE "MESSAGE"
%token K_CREATEFILE "CREATEFILE"
%token K_OPENFILE "OPENFILE"
%token K_CLOSEFILE "CLOSEFILE"
%token K_WARNING "WARNING"
%token K_ERROR "ERROR"
%token K_FATALERROR "FATALERROR"
%token K_ABOVE "ABOVE"
%token K_ABUT "ABUT"
%token K_ABUTMENT "ABUTMENT"
%token K_ACCURRENTDENSITY "ACCURRENTDENSITY"
%token K_ACTIVE "ACTIVE"
%token K_ADJACENTCUTS "ADJACENTCUTS"
%token K_ANALOG "ANALOG"
%token K_AND "AND"
%token K_ANTENNAAREAFACTOR "ANTENNAAREAFACTOR"
%token K_ANTENNAAREADIFFREDUCEPWL "ANTENNAAREADIFFREDUCEPWL"
%token K_ANTENNAAREAMINUSDIFF "ANTENNAAREAMINUSDIFF"
%token K_ANTENNAAREARATIO "ANTENNAAREARATIO"
%token K_ANTENNACELL "ANTENNACELL"
%token K_ANTENNACUMAREARATIO "ANTENNACUMAREARATIO"
%token K_ANTENNACUMDIFFAREARATIO "ANTENNACUMDIFFAREARATIO"
%token K_ANTENNACUMDIFFSIDEAREARATIO "ANTENNACUMDIFFSIDEAREARATIO"
%token K_ANTENNACUMROUTINGPLUSCUT "ANTENNACUMROUTINGPLUSCUT"
%token K_ANTENNACUMSIDEAREARATIO "ANTENNACUMSIDEAREARATIO"
%token K_ANTENNADIFFAREA "ANTENNADIFFAREA"
%token K_ANTENNADIFFAREARATIO "ANTENNADIFFAREARATIO"
%token K_ANTENNADIFFSIDEAREARATIO "ANTENNADIFFSIDEAREARATIO"
%token K_ANTENNAGATEAREA "ANTENNAGATEAREA"
%token K_ANTENNAGATEPLUSDIFF "ANTENNAGATEPLUSDIFF"
%token K_ANTENNAINOUTDIFFAREA "ANTENNAINOUTDIFFAREA"
%token K_ANTENNAINPUTGATEAREA "ANTENNAINPUTGATEAREA"
%token K_ANTENNALENGTHFACTOR "ANTENNALENGTHFACTOR"
%token K_ANTENNAMAXAREACAR "ANTENNAMAXAREACAR"
%token K_ANTENNAMAXCUTCAR "ANTENNAMAXCUTCAR"
%token K_ANTENNAMAXSIDEAREACAR "ANTENNAMAXSIDEAREACAR"
%token K_ANTENNAMETALAREA "ANTENNAMETALAREA"
%token K_ANTENNAMETALLENGTH "ANTENNAMETALLENGTH"
%token K_ANTENNAMODEL "ANTENNAMODEL"
%token K_ANTENNAOUTPUTDIFFAREA "ANTENNAOUTPUTDIFFAREA"
%token K_ANTENNAPARTIALCUTAREA "ANTENNAPARTIALCUTAREA"
%token K_ANTENNAPARTIALMETALAREA "ANTENNAPARTIALMETALAREA"
%token K_ANTENNAPARTIALMETALSIDEAREA "ANTENNAPARTIALMETALSIDEAREA"
%token K_ANTENNASIDEAREARATIO "ANTENNASIDEAREARATIO"
%token K_ANTENNASIZE "ANTENNASIZE"
%token K_ANTENNASIDEAREAFACTOR "ANTENNASIDEAREAFACTOR"
%token K_ANYEDGE "ANYEDGE"
%token K_AREA "AREA"
%token K_AREAIO "AREAIO"
%token K_ARRAY "ARRAY"
%token K_ARRAYCUTS "ARRAYCUTS"
%token K_ARRAYSPACING "ARRAYSPACING"
%token K_AVERAGE "AVERAGE"
%token K_BELOW "BELOW"
%token K_BEGINEXT "BEGINEXT"
%token K_BLACKBOX "BLACKBOX"
%token K_BLOCK "BLOCK"
%token K_BOTTOMLEFT "BOTTOMLEFT"
%token K_BOTTOMRIGHT "BOTTOMRIGHT"
%token K_BUMP "BUMP"
%token K_BUSBITCHARS "BUSBITCHARS"
%token K_BUFFER "BUFFER"
%token K_BY "BY"
%token K_CANNOTOCCUPY "CANNOTOCCUPY"
%token K_CANPLACE "CANPLACE"
%token K_CAPACITANCE "CAPACITANCE"
%token K_CAPMULTIPLIER "CAPMULTIPLIER"
%token K_CENTERTOCENTER "CENTERTOCENTER"
%token K_CLASS "CLASS"
%token K_CLEARANCEMEASURE "CLEARANCEMEASURE"
%token K_CLOCK "CLOCK"
%token K_CLOCKTYPE "CLOCKTYPE"
%token K_COLUMNMAJOR "COLUMNMAJOR"
%token K_CURRENTDEN "CURRENTDEN"
%token K_COMPONENTPIN "COMPONENTPIN"
%token K_CORE "CORE"
%token K_CORNER "CORNER"
%token K_CORRECTIONFACTOR "CORRECTIONFACTOR"
%token K_CORRECTIONTABLE "CORRECTIONTABLE"
%token K_COVER "COVER"
%token K_CPERSQDIST "CPERSQDIST"
%token K_CURRENT "CURRENT"
%token K_CURRENTSOURCE "CURRENTSOURCE"
%token K_CUT "CUT"
%token K_CUTAREA "CUTAREA"
%token K_CUTSIZE "CUTSIZE"
%token K_CUTSPACING "CUTSPACING"
%token K_DATA "DATA"
%token K_DATABASE "DATABASE"
%token K_DCCURRENTDENSITY "DCCURRENTDENSITY"
%token K_DEFAULT "DEFAULT"
%token K_DEFAULTCAP "DEFAULTCAP"
%token K_DELAY "DELAY"
%token K_DENSITY "DENSITY"
%token K_DENSITYCHECKSTEP "DENSITYCHECKSTEP"
%token K_DENSITYCHECKWINDOW "DENSITYCHECKWINDOW"
%token K_DESIGNRULEWIDTH "DESIGNRULEWIDTH"
%token K_DIAG45 "DIAG45"
%token K_DIAG135 "DIAG135"
%token K_DIAGMINEDGELENGTH "DIAGMINEDGELENGTH"
%token K_DIAGSPACING "DIAGSPACING"
%token K_DIAGPITCH "DIAGPITCH"
%token K_DIAGWIDTH "DIAGWIDTH"
%token K_DIELECTRIC "DIELECTRIC"
%token K_DIFFUSEONLY "DIFFUSEONLY"
%token K_DIRECTION "DIRECTION"
%token K_DIVIDERCHAR "DIVIDERCHAR"
%token K_DO "DO"
%token K_E "E"
%token K_EDGECAPACITANCE "EDGECAPACITANCE"
%token K_EDGERATE "EDGERATE"
%token K_EDGERATESCALEFACTOR "EDGERATESCALEFACTOR"
%token K_EDGERATETHRESHOLD1 "EDGERATETHRESHOLD1"
%token K_EDGERATETHRESHOLD2 "EDGERATETHRESHOLD2"
%token K_EEQ "EEQ"
%token K_ELSE "ELSE"
%token K_ENCLOSURE "ENCLOSURE"
%token K_END "END"
%token K_ENDEXT "ENDEXT"
%token K_ENDCAP "ENDCAP"
%token K_ENDOFLINE "ENDOFLINE"
%token K_ENDOFNOTCHWIDTH "ENDOFNOTCHWIDTH"
%token K_EUCLIDEAN "EUCLIDEAN"
%token K_EXCEPTEXTRACUT "EXCEPTEXTRACUT"
%token K_EXCEPTSAMEPGNET "EXCEPTSAMEPGNET"
%token K_EXCEPTPGNET "EXCEPTPGNET"
%token K_EXTENSION "EXTENSION"
%token K_FALL "FALL"
%token K_FALLCS "FALLCS"
%token K_FALLRS "FALLRS"
%token K_FALLSATCUR "FALLSATCUR"
%token K_FALLSATT1 "FALLSATT1"
%token K_FALLSLEWLIMIT "FALLSLEWLIMIT"
%token K_FALLT0 "FALLT0"
%token K_FALLTHRESH "FALLTHRESH"
%token K_FALLVOLTAGETHRESHOLD "FALLVOLTAGETHRESHOLD"
%token K_FALSE "FALSE"
%token K_FE "FE"
%token K_FEEDTHRU "FEEDTHRU"
%token K_FILLACTIVESPACING "FILLACTIVESPACING"
%token K_FIXED "FIXED"
%token K_FLIP "FLIP"
%token K_FLOORPLAN "FLOORPLAN"
%token K_FN "FN"
%token K_FOREIGN "FOREIGN"
%token K_FREQUENCY "FREQUENCY"
%token K_FROMABOVE "FROMABOVE"
%token K_FROMBELOW "FROMBELOW"
%token K_FROMPIN "FROMPIN"
%token K_FUNCTION "FUNCTION"
%token K_FS "FS"
%token K_FW "FW"
%token K_GCELLGRID "GCELLGRID"
%token K_GENERATE "GENERATE"
%token K_GENERATED "GENERATED"
%token K_GENERATOR "GENERATOR"
%token K_GROUND "GROUND"
%token K_GROUNDSENSITIVITY "GROUNDSENSITIVITY"
%token K_HARDSPACING "HARDSPACING"
%token K_HEIGHT "HEIGHT"
%token K_HISTORY "HISTORY"
%token K_HOLD "HOLD"
%token K_HORIZONTAL "HORIZONTAL"
%token K_IF "IF"
%token K_IMPLANT "IMPLANT"
%token K_INFLUENCE "INFLUENCE"
%token K_INOUT "INOUT"
%token K_INOUTPINANTENNASIZE "INOUTPINANTENNASIZE"
%token K_INPUT "INPUT"
%token K_INPUTPINANTENNASIZE "INPUTPINANTENNASIZE"
%token K_INPUTNOISEMARGIN "INPUTNOISEMARGIN"
%token K_INSIDECORNER "INSIDECORNER"
%token K_INTEGER "INTEGER"
%token K_INTRINSIC "INTRINSIC"
%token K_INVERT "INVERT"
%token K_INVERTER "INVERTER"
%token K_IRDROP "IRDROP"
%token K_ITERATE "ITERATE"
%token K_IV_TABLES "IV_TABLES"
%token K_LAYER "LAYER"
%token K_LAYERS "LAYERS"
%token K_LEAKAGE "LEAKAGE"
%token K_LENGTH "LENGTH"
%token K_LENGTHSUM "LENGTHSUM"
%token K_LENGTHTHRESHOLD "LENGTHTHRESHOLD"
%token K_LEQ "LEQ"
%token K_LIBRARY "LIBRARY"
%token K_LONGARRAY "LONGARRAY"
%token K_MACRO "MACRO"
%token K_MANUFACTURINGGRID "MANUFACTURINGGRID"
%token K_MASTERSLICE "MASTERSLICE"
%token K_MATCH "MATCH"
%token K_MAXADJACENTSLOTSPACING "MAXADJACENTSLOTSPACING"
%token K_MAXCOAXIALSLOTSPACING "MAXCOAXIALSLOTSPACING"
%token K_MAXDELAY "MAXDELAY"
%token K_MAXEDGES "MAXEDGES"
%token K_MAXEDGESLOTSPACING "MAXEDGESLOTSPACING"
%token K_MAXLOAD "MAXLOAD"
%token K_MAXIMUMDENSITY "MAXIMUMDENSITY"
%token K_MAXVIASTACK "MAXVIASTACK"
%token K_MAXWIDTH "MAXWIDTH"
%token K_MAXXY "MAXXY"
%token K_MEGAHERTZ "MEGAHERTZ"
%token K_METALOVERHANG "METALOVERHANG"
%token K_MICRONS "MICRONS"
%token K_MILLIAMPS "MILLIAMPS"
%token K_MILLIWATTS "MILLIWATTS"
%token K_MINCUTS "MINCUTS"
%token K_MINENCLOSEDAREA "MINENCLOSEDAREA"
%token K_MINFEATURE "MINFEATURE"
%token K_MINIMUMCUT "MINIMUMCUT"
%token K_MINIMUMDENSITY "MINIMUMDENSITY"
%token K_MINPINS "MINPINS"
%token K_MINSIZE "MINSIZE"
%token K_MINSTEP "MINSTEP"
%token K_MINWIDTH "MINWIDTH"
%token K_MPWH "MPWH"
%token K_MPWL "MPWL"
%token K_MUSTJOIN "MUSTJOIN"
%token K_MX "MX"
%token K_MY "MY"
%token K_MXR90 "MXR90"
%token K_MYR90 "MYR90"
%token K_N "N"
%token K_NAMEMAPSTRING "NAMEMAPSTRING"
%token K_NAMESCASESENSITIVE "NAMESCASESENSITIVE"
%token K_NANOSECONDS "NANOSECONDS"
%token K_NEGEDGE "NEGEDGE"
%token K_NETEXPR "NETEXPR"
%token K_NETS "NETS"
%token K_NEW "NEW"
%token K_NONDEFAULTRULE "NONDEFAULTRULE"
%token K_NONE "NONE"
%token K_NONINVERT "NONINVERT"
%token K_NONUNATE "NONUNATE"
%token K_NOISETABLE "NOISETABLE"
%token K_NOTCHLENGTH "NOTCHLENGTH"
%token K_NOTCHSPACING "NOTCHSPACING"
%token K_NOWIREEXTENSIONATPIN "NOWIREEXTENSIONATPIN"
%token K_OBS "OBS"
%token K_OFF "OFF"
%token K_OFFSET "OFFSET"
%token K_OHMS "OHMS"
%token K_ON "ON"
%token K_OR "OR"
%token K_ORIENT "ORIENT"
%token K_ORIENTATION "ORIENTATION"
%token K_ORIGIN "ORIGIN"
%token K_ORTHOGONAL "ORTHOGONAL"
%token K_OUTPUT "OUTPUT"
%token K_OUTPUTPINANTENNASIZE "OUTPUTPINANTENNASIZE"
%token K_OUTPUTNOISEMARGIN "OUTPUTNOISEMARGIN"
%token K_OUTPUTRESISTANCE "OUTPUTRESISTANCE"
%token K_OUTSIDECORNER "OUTSIDECORNER"
%token K_OVERHANG "OVERHANG"
%token K_OVERLAP "OVERLAP"
%token K_OVERLAPS "OVERLAPS"
%token K_OXIDE1 "OXIDE1"
%token K_OXIDE2 "OXIDE2"
%token K_OXIDE3 "OXIDE3"
%token K_OXIDE4 "OXIDE4"
%token K_PAD "PAD"
%token K_PARALLELEDGE "PARALLELEDGE"
%token K_PARALLELOVERLAP "PARALLELOVERLAP"
%token K_PARALLELRUNLENGTH "PARALLELRUNLENGTH"
%token K_PATH "PATH"
%token K_PATTERN "PATTERN"
%token K_PEAK "PEAK"
%token K_PERIOD "PERIOD"
%token K_PGONLY "PGONLY"
%token K_PICOFARADS "PICOFARADS"
%token K_PIN "PIN"
%token K_PITCH "PITCH"
%token K_PLACED "PLACED"
%token K_POLYGON "POLYGON"
%token K_PORT "PORT"
%token K_POSEDGE "POSEDGE"
%token K_POST "POST"
%token K_POWER "POWER"
%token K_PRE "PRE"
%token K_PREFERENCLOSURE "PREFERENCLOSURE"
%token K_PRL "PRL"
%token K_PROPERTY "PROPERTY"
%token K_PROPERTYDEFINITIONS "PROPERTYDEFINITIONS"
%token K_PROTRUSIONWIDTH "PROTRUSIONWIDTH"
%token K_PULLDOWNRES "PULLDOWNRES"
%token K_PWL "PWL"
%token K_R0 "R0"
%token K_R90 "R90"
%token K_R180 "R180"
%token K_R270 "R270"
%token K_RANGE "RANGE"
%token K_REAL "REAL"
%token K_RECOVERY "RECOVERY"
%token K_RECT "RECT"
%token K_RESISTANCE "RESISTANCE"
%token K_RESISTIVE "RESISTIVE"
%token K_RING "RING"
%token K_RISE "RISE"
%token K_RISECS "RISECS"
%token K_RISERS "RISERS"
%token K_RISESATCUR "RISESATCUR"
%token K_RISESATT1 "RISESATT1"
%token K_RISESLEWLIMIT "RISESLEWLIMIT"
%token K_RISET0 "RISET0"
%token K_RISETHRESH "RISETHRESH"
%token K_RISEVOLTAGETHRESHOLD "RISEVOLTAGETHRESHOLD"
%token K_RMS "RMS"
%token K_ROUTING "ROUTING"
%token K_ROWABUTSPACING "ROWABUTSPACING"
%token K_ROWCOL "ROWCOL"
%token K_ROWMAJOR "ROWMAJOR"
%token K_ROWMINSPACING "ROWMINSPACING"
%token K_ROWPATTERN "ROWPATTERN"
%token K_RPERSQ "RPERSQ"
%token K_S "S"
%token K_SAMENET "SAMENET"
%token K_SCANUSE "SCANUSE"
%token K_SDFCOND "SDFCOND"
%token K_SDFCONDEND "SDFCONDEND"
%token K_SDFCONDSTART "SDFCONDSTART"
%token K_SETUP "SETUP"
%token K_SHAPE "SHAPE"
%token K_SHRINKAGE "SHRINKAGE"
%token K_SIGNAL "SIGNAL"
%token K_SITE "SITE"
%token K_SIZE "SIZE"
%token K_SKEW "SKEW"
%token K_SLOTLENGTH "SLOTLENGTH"
%token K_SLOTWIDTH "SLOTWIDTH"
%token K_SLOTWIRELENGTH "SLOTWIRELENGTH"
%token K_SLOTWIREWIDTH "SLOTWIREWIDTH"
%token K_SPLITWIREWIDTH "SPLITWIREWIDTH"
%token K_SOFT "SOFT"
%token K_SOURCE "SOURCE"
%token K_SPACER "SPACER"
%token K_SPACING "SPACING"
%token K_SPACINGTABLE "SPACINGTABLE"
%token K_SPECIALNETS "SPECIALNETS"
%token K_STABLE "STABLE"
%token K_STACK "STACK"
%token K_START "START"
%token K_STEP "STEP"
%token K_STOP "STOP"
%token K_STRING "STRING"
%token K_STRUCTURE "STRUCTURE"
%token K_SUPPLYSENSITIVITY "SUPPLYSENSITIVITY"
%token K_SYMMETRY "SYMMETRY"
%token K_TABLE "TABLE"
%token K_TABLEAXIS "TABLEAXIS"
%token K_TABLEDIMENSION "TABLEDIMENSION"
%token K_TABLEENTRIES "TABLEENTRIES"
%token K_TAPERRULE "TAPERRULE"
%token K_THEN "THEN"
%token K_THICKNESS "THICKNESS"
%token K_TIEHIGH "TIEHIGH"
%token K_TIELOW "TIELOW"
%token K_TIEOFFR "TIEOFFR"
%token K_TIME "TIME"
%token K_TIMING "TIMING"
%token K_TO "TO"
%token K_TOPIN "TOPIN"
%token K_TOPLEFT "TOPLEFT"
%token K_TOPOFSTACKONLY "TOPOFSTACKONLY"
%token K_TOPRIGHT "TOPRIGHT"
%token K_TRACKS "TRACKS"
%token K_TRANSITIONTIME "TRANSITIONTIME"
%token K_TRISTATE "TRISTATE"
%token K_TRUE "TRUE"
%token K_TWOEDGES "TWOEDGES"
%token K_TWOWIDTHS "TWOWIDTHS"
%token K_TYPE "TYPE"
%token K_UNATENESS "UNATENESS"
%token K_UNITS "UNITS"
%token K_UNIVERSALNOISEMARGIN "UNIVERSALNOISEMARGIN"
%token K_USE "USE"
%token K_USELENGTHTHRESHOLD "USELENGTHTHRESHOLD"
%token K_USEMINSPACING "USEMINSPACING"
%token K_USER "USER"
%token K_USEVIA "USEVIA"
%token K_USEVIARULE "USEVIARULE"
%token K_VARIABLE "VARIABLE"
%token K_VERSION "VERSION"
%token K_VERTICAL "VERTICAL"
%token K_VHI "VHI"
%token K_VIA "VIA"
%token K_VIARULE "VIARULE"
%token K_VICTIMLENGTH "VICTIMLENGTH"
%token K_VICTIMNOISE "VICTIMNOISE"
%token K_VIRTUAL "VIRTUAL"
%token K_VLO "VLO"
%token K_VOLTAGE "VOLTAGE"
%token K_VOLTS "VOLTS"
%token K_W "W"
%token K_WELLTAP "WELLTAP"
%token K_WIDTH "WIDTH"
%token K_WITHIN "WITHIN"
%token K_WIRECAP "WIRECAP"
%token K_WIREEXTENSION "WIREEXTENSION"
%token K_X "X"
%token K_Y "Y"
%token K_EQ K_NE K_LE K_LT K_GE K_GT K_NOT

%token			END	     0	"end of file"
/* %token			EOL		"end of line" */
%token <integerVal> 	INTEGER		"integer"
%token <doubleVal> 	DOUBLE		"double"
%token <stringVal> 	STRING		"string"
%token <qstringVal> 	QSTRING		"qstring"
%token <binaryVal> 	BINARY		"binary numbers"

%type <doubleVal>   NUMBER "number"
%type <stringVal> GSTRING      "generalized string"
/*%type <stringVal> start_macro end_macro*/
/*%type <stringVal> start_layer*/
%type <stringVal> macro_pin_use
%type <stringVal> macro_scan_use
%type <stringVal> pin_shape
%type <stringVal> pad_type core_type endcap_type class_type site_class
%type <stringVal> start_foreign spacing_type clearance_type
%type <pt> pt 
%type <pt> macro_origin
%type <stringVal> layer_type layer_direction
%type <stringVal> electrical_direction
%type <integerVal> orientation
%type <doubleVal> expression
%type <integerVal> b_expr
%type <stringVal>  s_expr
%type <integerVal> relop spacing_value
%type <stringVal> opt_layer_name risefall unateness delay_or_transition
%type <stringVal> two_pin_trigger from_pin_trigger to_pin_trigger
%type <stringVal> one_pin_trigger req_layer_name
%type <stringVal> layer_table_type layer_enclosure_type_opt layer_minstep_type
%type <doubleVal>   layer_sp_TwoWidthsPRL

%destructor { delete $$; } STRING QSTRING BINARY 
%destructor { delete $$; } pt
%destructor { delete $$; } GSTRING 

%nonassoc IF
%left K_AND
%left K_OR
%left K_LE K_EQ K_LT K_NE K_GE K_GT
%nonassoc LNOT
%left '-' '+'
%left '*' '/'
%nonassoc UMINUS

 /*** END EXAMPLE - Change the example grammar's tokens above ***/

%{

#include "LefDriver.h"
#include "LefScanner.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN EXAMPLE - Change the example grammar rules below ***/

NUMBER : INTEGER {$$=$1;} 
	   | DOUBLE {$$=$1;}

GSTRING : STRING {$$ = $1;}
| K_DEFINE {$$ = new std::string ("DEFINE");} 
| K_DEFINEB {$$ = new std::string ("DEFINEB");} 
| K_DEFINES {$$ = new std::string ("DEFINES");} 
| K_MESSAGE {$$ = new std::string ("MESSAGE");} 
| K_CREATEFILE {$$ = new std::string ("CREATEFILE");} 
| K_OPENFILE {$$ = new std::string ("OPENFILE");} 
| K_CLOSEFILE {$$ = new std::string ("CLOSEFILE");} 
| K_WARNING {$$ = new std::string ("WARNING");} 
| K_ERROR {$$ = new std::string ("ERROR");} 
| K_FATALERROR {$$ = new std::string ("FATALERROR");} 
| K_ABOVE {$$ = new std::string ("ABOVE");} 
| K_ABUT {$$ = new std::string ("ABUT");} 
| K_ABUTMENT {$$ = new std::string ("ABUTMENT");} 
| K_ACCURRENTDENSITY {$$ = new std::string ("ACCURRENTDENSITY");} 
| K_ACTIVE {$$ = new std::string ("ACTIVE");} 
| K_ADJACENTCUTS {$$ = new std::string ("ADJACENTCUTS");} 
| K_ANALOG {$$ = new std::string ("ANALOG");} 
| K_AND {$$ = new std::string ("AND");} 
| K_ANTENNAAREAFACTOR {$$ = new std::string ("ANTENNAAREAFACTOR");} 
| K_ANTENNAAREADIFFREDUCEPWL {$$ = new std::string ("ANTENNAAREADIFFREDUCEPWL");} 
| K_ANTENNAAREAMINUSDIFF {$$ = new std::string ("ANTENNAAREAMINUSDIFF");} 
| K_ANTENNAAREARATIO {$$ = new std::string ("ANTENNAAREARATIO");} 
| K_ANTENNACELL {$$ = new std::string ("ANTENNACELL");} 
| K_ANTENNACUMAREARATIO {$$ = new std::string ("ANTENNACUMAREARATIO");} 
| K_ANTENNACUMDIFFAREARATIO {$$ = new std::string ("ANTENNACUMDIFFAREARATIO");} 
| K_ANTENNACUMDIFFSIDEAREARATIO {$$ = new std::string ("ANTENNACUMDIFFSIDEAREARATIO");} 
| K_ANTENNACUMROUTINGPLUSCUT {$$ = new std::string ("ANTENNACUMROUTINGPLUSCUT");} 
| K_ANTENNACUMSIDEAREARATIO {$$ = new std::string ("ANTENNACUMSIDEAREARATIO");} 
| K_ANTENNADIFFAREA {$$ = new std::string ("ANTENNADIFFAREA");} 
| K_ANTENNADIFFAREARATIO {$$ = new std::string ("ANTENNADIFFAREARATIO");} 
| K_ANTENNADIFFSIDEAREARATIO {$$ = new std::string ("ANTENNADIFFSIDEAREARATIO");} 
| K_ANTENNAGATEAREA {$$ = new std::string ("ANTENNAGATEAREA");} 
| K_ANTENNAGATEPLUSDIFF {$$ = new std::string ("ANTENNAGATEPLUSDIFF");} 
| K_ANTENNAINOUTDIFFAREA {$$ = new std::string ("ANTENNAINOUTDIFFAREA");} 
| K_ANTENNAINPUTGATEAREA {$$ = new std::string ("ANTENNAINPUTGATEAREA");} 
| K_ANTENNALENGTHFACTOR {$$ = new std::string ("ANTENNALENGTHFACTOR");} 
| K_ANTENNAMAXAREACAR {$$ = new std::string ("ANTENNAMAXAREACAR");} 
| K_ANTENNAMAXCUTCAR {$$ = new std::string ("ANTENNAMAXCUTCAR");} 
| K_ANTENNAMAXSIDEAREACAR {$$ = new std::string ("ANTENNAMAXSIDEAREACAR");} 
| K_ANTENNAMETALAREA {$$ = new std::string ("ANTENNAMETALAREA");} 
| K_ANTENNAMETALLENGTH {$$ = new std::string ("ANTENNAMETALLENGTH");} 
| K_ANTENNAMODEL {$$ = new std::string ("ANTENNAMODEL");} 
| K_ANTENNAOUTPUTDIFFAREA {$$ = new std::string ("ANTENNAOUTPUTDIFFAREA");} 
| K_ANTENNAPARTIALCUTAREA {$$ = new std::string ("ANTENNAPARTIALCUTAREA");} 
| K_ANTENNAPARTIALMETALAREA {$$ = new std::string ("ANTENNAPARTIALMETALAREA");} 
| K_ANTENNAPARTIALMETALSIDEAREA {$$ = new std::string ("ANTENNAPARTIALMETALSIDEAREA");} 
| K_ANTENNASIDEAREARATIO {$$ = new std::string ("ANTENNASIDEAREARATIO");} 
| K_ANTENNASIZE {$$ = new std::string ("ANTENNASIZE");} 
| K_ANTENNASIDEAREAFACTOR {$$ = new std::string ("ANTENNASIDEAREAFACTOR");} 
| K_ANYEDGE {$$ = new std::string ("ANYEDGE");} 
| K_AREA {$$ = new std::string ("AREA");} 
| K_AREAIO {$$ = new std::string ("AREAIO");} 
| K_ARRAY {$$ = new std::string ("ARRAY");} 
| K_ARRAYCUTS {$$ = new std::string ("ARRAYCUTS");} 
| K_ARRAYSPACING {$$ = new std::string ("ARRAYSPACING");} 
| K_AVERAGE {$$ = new std::string ("AVERAGE");} 
| K_BELOW {$$ = new std::string ("BELOW");} 
| K_BEGINEXT {$$ = new std::string ("BEGINEXT");} 
| K_BLACKBOX {$$ = new std::string ("BLACKBOX");} 
| K_BLOCK {$$ = new std::string ("BLOCK");} 
| K_BOTTOMLEFT {$$ = new std::string ("BOTTOMLEFT");} 
| K_BOTTOMRIGHT {$$ = new std::string ("BOTTOMRIGHT");} 
| K_BUMP {$$ = new std::string ("BUMP");} 
| K_BUSBITCHARS {$$ = new std::string ("BUSBITCHARS");} 
| K_BUFFER {$$ = new std::string ("BUFFER");} 
| K_BY {$$ = new std::string ("BY");} 
| K_CANNOTOCCUPY {$$ = new std::string ("CANNOTOCCUPY");} 
| K_CANPLACE {$$ = new std::string ("CANPLACE");} 
| K_CAPACITANCE {$$ = new std::string ("CAPACITANCE");} 
| K_CAPMULTIPLIER {$$ = new std::string ("CAPMULTIPLIER");} 
| K_CENTERTOCENTER {$$ = new std::string ("CENTERTOCENTER");} 
| K_CLASS {$$ = new std::string ("CLASS");} 
| K_CLEARANCEMEASURE {$$ = new std::string ("CLEARANCEMEASURE");} 
| K_CLOCK {$$ = new std::string ("CLOCK");} 
| K_CLOCKTYPE {$$ = new std::string ("CLOCKTYPE");} 
| K_COLUMNMAJOR {$$ = new std::string ("COLUMNMAJOR");} 
| K_CURRENTDEN {$$ = new std::string ("CURRENTDEN");} 
| K_COMPONENTPIN {$$ = new std::string ("COMPONENTPIN");} 
| K_CORE {$$ = new std::string ("CORE");} 
| K_CORNER {$$ = new std::string ("CORNER");} 
| K_CORRECTIONFACTOR {$$ = new std::string ("CORRECTIONFACTOR");} 
| K_CORRECTIONTABLE {$$ = new std::string ("CORRECTIONTABLE");} 
| K_COVER {$$ = new std::string ("COVER");} 
| K_CPERSQDIST {$$ = new std::string ("CPERSQDIST");} 
| K_CURRENT {$$ = new std::string ("CURRENT");} 
| K_CURRENTSOURCE {$$ = new std::string ("CURRENTSOURCE");} 
| K_CUT {$$ = new std::string ("CUT");} 
| K_CUTAREA {$$ = new std::string ("CUTAREA");} 
| K_CUTSIZE {$$ = new std::string ("CUTSIZE");} 
| K_CUTSPACING {$$ = new std::string ("CUTSPACING");} 
| K_DATA {$$ = new std::string ("DATA");} 
| K_DATABASE {$$ = new std::string ("DATABASE");} 
| K_DCCURRENTDENSITY {$$ = new std::string ("DCCURRENTDENSITY");} 
| K_DEFAULT {$$ = new std::string ("DEFAULT");} 
| K_DEFAULTCAP {$$ = new std::string ("DEFAULTCAP");} 
| K_DELAY {$$ = new std::string ("DELAY");} 
| K_DENSITY {$$ = new std::string ("DENSITY");} 
| K_DENSITYCHECKSTEP {$$ = new std::string ("DENSITYCHECKSTEP");} 
| K_DENSITYCHECKWINDOW {$$ = new std::string ("DENSITYCHECKWINDOW");} 
| K_DESIGNRULEWIDTH {$$ = new std::string ("DESIGNRULEWIDTH");} 
| K_DIAG45 {$$ = new std::string ("DIAG45");} 
| K_DIAG135 {$$ = new std::string ("DIAG135");} 
| K_DIAGMINEDGELENGTH {$$ = new std::string ("DIAGMINEDGELENGTH");} 
| K_DIAGSPACING {$$ = new std::string ("DIAGSPACING");} 
| K_DIAGPITCH {$$ = new std::string ("DIAGPITCH");} 
| K_DIAGWIDTH {$$ = new std::string ("DIAGWIDTH");} 
| K_DIELECTRIC {$$ = new std::string ("DIELECTRIC");} 
| K_DIFFUSEONLY {$$ = new std::string ("DIFFUSEONLY");} 
| K_DIRECTION {$$ = new std::string ("DIRECTION");} 
| K_DIVIDERCHAR {$$ = new std::string ("DIVIDERCHAR");} 
| K_DO {$$ = new std::string ("DO");} 
| K_E {$$ = new std::string ("E");} 
| K_EDGECAPACITANCE {$$ = new std::string ("EDGECAPACITANCE");} 
| K_EDGERATE {$$ = new std::string ("EDGERATE");} 
| K_EDGERATESCALEFACTOR {$$ = new std::string ("EDGERATESCALEFACTOR");} 
| K_EDGERATETHRESHOLD1 {$$ = new std::string ("EDGERATETHRESHOLD1");} 
| K_EDGERATETHRESHOLD2 {$$ = new std::string ("EDGERATETHRESHOLD2");} 
| K_EEQ {$$ = new std::string ("EEQ");} 
| K_ELSE {$$ = new std::string ("ELSE");} 
| K_ENCLOSURE {$$ = new std::string ("ENCLOSURE");} 
| K_END {$$ = new std::string ("END");} 
| K_ENDEXT {$$ = new std::string ("ENDEXT");} 
| K_ENDCAP {$$ = new std::string ("ENDCAP");} 
| K_ENDOFLINE {$$ = new std::string ("ENDOFLINE");} 
| K_ENDOFNOTCHWIDTH {$$ = new std::string ("ENDOFNOTCHWIDTH");} 
| K_EUCLIDEAN {$$ = new std::string ("EUCLIDEAN");} 
| K_EXCEPTEXTRACUT {$$ = new std::string ("EXCEPTEXTRACUT");} 
| K_EXCEPTSAMEPGNET {$$ = new std::string ("EXCEPTSAMEPGNET");} 
| K_EXCEPTPGNET {$$ = new std::string ("EXCEPTPGNET");} 
| K_EXTENSION {$$ = new std::string ("EXTENSION");} 
| K_FALL {$$ = new std::string ("FALL");} 
| K_FALLCS {$$ = new std::string ("FALLCS");} 
| K_FALLRS {$$ = new std::string ("FALLRS");} 
| K_FALLSATCUR {$$ = new std::string ("FALLSATCUR");} 
| K_FALLSATT1 {$$ = new std::string ("FALLSATT1");} 
| K_FALLSLEWLIMIT {$$ = new std::string ("FALLSLEWLIMIT");} 
| K_FALLT0 {$$ = new std::string ("FALLT0");} 
| K_FALLTHRESH {$$ = new std::string ("FALLTHRESH");} 
| K_FALLVOLTAGETHRESHOLD {$$ = new std::string ("FALLVOLTAGETHRESHOLD");} 
| K_FALSE {$$ = new std::string ("FALSE");} 
| K_FE {$$ = new std::string ("FE");} 
| K_FEEDTHRU {$$ = new std::string ("FEEDTHRU");} 
| K_FILLACTIVESPACING {$$ = new std::string ("FILLACTIVESPACING");} 
| K_FIXED {$$ = new std::string ("FIXED");} 
| K_FLIP {$$ = new std::string ("FLIP");} 
| K_FLOORPLAN {$$ = new std::string ("FLOORPLAN");} 
| K_FN {$$ = new std::string ("FN");} 
| K_FOREIGN {$$ = new std::string ("FOREIGN");} 
| K_FREQUENCY {$$ = new std::string ("FREQUENCY");} 
| K_FROMABOVE {$$ = new std::string ("FROMABOVE");} 
| K_FROMBELOW {$$ = new std::string ("FROMBELOW");} 
| K_FROMPIN {$$ = new std::string ("FROMPIN");} 
| K_FUNCTION {$$ = new std::string ("FUNCTION");} 
| K_FS {$$ = new std::string ("FS");} 
| K_FW {$$ = new std::string ("FW");} 
| K_GCELLGRID {$$ = new std::string ("GCELLGRID");} 
| K_GENERATE {$$ = new std::string ("GENERATE");} 
| K_GENERATED {$$ = new std::string ("GENERATED");} 
| K_GENERATOR {$$ = new std::string ("GENERATOR");} 
| K_GROUND {$$ = new std::string ("GROUND");} 
| K_GROUNDSENSITIVITY {$$ = new std::string ("GROUNDSENSITIVITY");} 
| K_HARDSPACING {$$ = new std::string ("HARDSPACING");} 
| K_HEIGHT {$$ = new std::string ("HEIGHT");} 
| K_HISTORY {$$ = new std::string ("HISTORY");} 
| K_HOLD {$$ = new std::string ("HOLD");} 
| K_HORIZONTAL {$$ = new std::string ("HORIZONTAL");} 
| K_IF {$$ = new std::string ("IF");} 
| K_IMPLANT {$$ = new std::string ("IMPLANT");} 
| K_INFLUENCE {$$ = new std::string ("INFLUENCE");} 
| K_INOUT {$$ = new std::string ("INOUT");} 
| K_INOUTPINANTENNASIZE {$$ = new std::string ("INOUTPINANTENNASIZE");} 
| K_INPUT {$$ = new std::string ("INPUT");} 
| K_INPUTPINANTENNASIZE {$$ = new std::string ("INPUTPINANTENNASIZE");} 
| K_INPUTNOISEMARGIN {$$ = new std::string ("INPUTNOISEMARGIN");} 
| K_INSIDECORNER {$$ = new std::string ("INSIDECORNER");} 
| K_INTEGER {$$ = new std::string ("INTEGER");} 
| K_INTRINSIC {$$ = new std::string ("INTRINSIC");} 
| K_INVERT {$$ = new std::string ("INVERT");} 
| K_INVERTER {$$ = new std::string ("INVERTER");} 
| K_IRDROP {$$ = new std::string ("IRDROP");} 
| K_ITERATE {$$ = new std::string ("ITERATE");} 
| K_IV_TABLES {$$ = new std::string ("IV_TABLES");} 
| K_LAYER {$$ = new std::string ("LAYER");} 
| K_LAYERS {$$ = new std::string ("LAYERS");} 
| K_LEAKAGE {$$ = new std::string ("LEAKAGE");} 
| K_LENGTH {$$ = new std::string ("LENGTH");} 
| K_LENGTHSUM {$$ = new std::string ("LENGTHSUM");} 
| K_LENGTHTHRESHOLD {$$ = new std::string ("LENGTHTHRESHOLD");} 
| K_LEQ {$$ = new std::string ("LEQ");} 
| K_LIBRARY {$$ = new std::string ("LIBRARY");} 
| K_LONGARRAY {$$ = new std::string ("LONGARRAY");} 
| K_MACRO {$$ = new std::string ("MACRO");} 
| K_MANUFACTURINGGRID {$$ = new std::string ("MANUFACTURINGGRID");} 
| K_MASTERSLICE {$$ = new std::string ("MASTERSLICE");} 
| K_MATCH {$$ = new std::string ("MATCH");} 
| K_MAXADJACENTSLOTSPACING {$$ = new std::string ("MAXADJACENTSLOTSPACING");} 
| K_MAXCOAXIALSLOTSPACING {$$ = new std::string ("MAXCOAXIALSLOTSPACING");} 
| K_MAXDELAY {$$ = new std::string ("MAXDELAY");} 
| K_MAXEDGES {$$ = new std::string ("MAXEDGES");} 
| K_MAXEDGESLOTSPACING {$$ = new std::string ("MAXEDGESLOTSPACING");} 
| K_MAXLOAD {$$ = new std::string ("MAXLOAD");} 
| K_MAXIMUMDENSITY {$$ = new std::string ("MAXIMUMDENSITY");} 
| K_MAXVIASTACK {$$ = new std::string ("MAXVIASTACK");} 
| K_MAXWIDTH {$$ = new std::string ("MAXWIDTH");} 
| K_MAXXY {$$ = new std::string ("MAXXY");} 
| K_MEGAHERTZ {$$ = new std::string ("MEGAHERTZ");} 
| K_METALOVERHANG {$$ = new std::string ("METALOVERHANG");} 
| K_MICRONS {$$ = new std::string ("MICRONS");} 
| K_MILLIAMPS {$$ = new std::string ("MILLIAMPS");} 
| K_MILLIWATTS {$$ = new std::string ("MILLIWATTS");} 
| K_MINCUTS {$$ = new std::string ("MINCUTS");} 
| K_MINENCLOSEDAREA {$$ = new std::string ("MINENCLOSEDAREA");} 
| K_MINFEATURE {$$ = new std::string ("MINFEATURE");} 
| K_MINIMUMCUT {$$ = new std::string ("MINIMUMCUT");} 
| K_MINIMUMDENSITY {$$ = new std::string ("MINIMUMDENSITY");} 
| K_MINPINS {$$ = new std::string ("MINPINS");} 
| K_MINSIZE {$$ = new std::string ("MINSIZE");} 
| K_MINSTEP {$$ = new std::string ("MINSTEP");} 
| K_MINWIDTH {$$ = new std::string ("MINWIDTH");} 
| K_MPWH {$$ = new std::string ("MPWH");} 
| K_MPWL {$$ = new std::string ("MPWL");} 
| K_MUSTJOIN {$$ = new std::string ("MUSTJOIN");} 
| K_MX {$$ = new std::string ("MX");} 
| K_MY {$$ = new std::string ("MY");} 
| K_MXR90 {$$ = new std::string ("MXR90");} 
| K_MYR90 {$$ = new std::string ("MYR90");} 
| K_N {$$ = new std::string ("N");} 
| K_NAMEMAPSTRING {$$ = new std::string ("NAMEMAPSTRING");} 
| K_NAMESCASESENSITIVE {$$ = new std::string ("NAMESCASESENSITIVE");} 
| K_NANOSECONDS {$$ = new std::string ("NANOSECONDS");} 
| K_NEGEDGE {$$ = new std::string ("NEGEDGE");} 
| K_NETEXPR {$$ = new std::string ("NETEXPR");} 
| K_NETS {$$ = new std::string ("NETS");} 
| K_NEW {$$ = new std::string ("NEW");} 
| K_NONDEFAULTRULE {$$ = new std::string ("NONDEFAULTRULE");} 
| K_NONE {$$ = new std::string ("NONE");} 
| K_NONINVERT {$$ = new std::string ("NONINVERT");} 
| K_NONUNATE {$$ = new std::string ("NONUNATE");} 
| K_NOISETABLE {$$ = new std::string ("NOISETABLE");} 
| K_NOTCHLENGTH {$$ = new std::string ("NOTCHLENGTH");} 
| K_NOTCHSPACING {$$ = new std::string ("NOTCHSPACING");} 
| K_NOWIREEXTENSIONATPIN {$$ = new std::string ("NOWIREEXTENSIONATPIN");} 
| K_OBS {$$ = new std::string ("OBS");} 
| K_OFF {$$ = new std::string ("OFF");} 
| K_OFFSET {$$ = new std::string ("OFFSET");} 
| K_OHMS {$$ = new std::string ("OHMS");} 
| K_ON {$$ = new std::string ("ON");} 
| K_OR {$$ = new std::string ("OR");} 
| K_ORIENT {$$ = new std::string ("ORIENT");} 
| K_ORIENTATION {$$ = new std::string ("ORIENTATION");} 
| K_ORIGIN {$$ = new std::string ("ORIGIN");} 
| K_ORTHOGONAL {$$ = new std::string ("ORTHOGONAL");} 
| K_OUTPUT {$$ = new std::string ("OUTPUT");} 
| K_OUTPUTPINANTENNASIZE {$$ = new std::string ("OUTPUTPINANTENNASIZE");} 
| K_OUTPUTNOISEMARGIN {$$ = new std::string ("OUTPUTNOISEMARGIN");} 
| K_OUTPUTRESISTANCE {$$ = new std::string ("OUTPUTRESISTANCE");} 
| K_OUTSIDECORNER {$$ = new std::string ("OUTSIDECORNER");} 
| K_OVERHANG {$$ = new std::string ("OVERHANG");} 
| K_OVERLAP {$$ = new std::string ("OVERLAP");} 
| K_OVERLAPS {$$ = new std::string ("OVERLAPS");} 
| K_OXIDE1 {$$ = new std::string ("OXIDE1");} 
| K_OXIDE2 {$$ = new std::string ("OXIDE2");} 
| K_OXIDE3 {$$ = new std::string ("OXIDE3");} 
| K_OXIDE4 {$$ = new std::string ("OXIDE4");} 
| K_PAD {$$ = new std::string ("PAD");} 
| K_PARALLELEDGE {$$ = new std::string ("PARALLELEDGE");} 
| K_PARALLELOVERLAP {$$ = new std::string ("PARALLELOVERLAP");} 
| K_PARALLELRUNLENGTH {$$ = new std::string ("PARALLELRUNLENGTH");} 
| K_PATH {$$ = new std::string ("PATH");} 
| K_PATTERN {$$ = new std::string ("PATTERN");} 
| K_PEAK {$$ = new std::string ("PEAK");} 
| K_PERIOD {$$ = new std::string ("PERIOD");} 
| K_PGONLY {$$ = new std::string ("PGONLY");} 
| K_PICOFARADS {$$ = new std::string ("PICOFARADS");} 
| K_PIN {$$ = new std::string ("PIN");} 
| K_PITCH {$$ = new std::string ("PITCH");} 
| K_PLACED {$$ = new std::string ("PLACED");} 
| K_POLYGON {$$ = new std::string ("POLYGON");} 
| K_PORT {$$ = new std::string ("PORT");} 
| K_POSEDGE {$$ = new std::string ("POSEDGE");} 
| K_POST {$$ = new std::string ("POST");} 
| K_POWER {$$ = new std::string ("POWER");} 
| K_PRE {$$ = new std::string ("PRE");} 
| K_PREFERENCLOSURE {$$ = new std::string ("PREFERENCLOSURE");} 
| K_PRL {$$ = new std::string ("PRL");} 
| K_PROPERTY {$$ = new std::string ("PROPERTY");} 
| K_PROPERTYDEFINITIONS {$$ = new std::string ("PROPERTYDEFINITIONS");} 
| K_PROTRUSIONWIDTH {$$ = new std::string ("PROTRUSIONWIDTH");} 
| K_PULLDOWNRES {$$ = new std::string ("PULLDOWNRES");} 
| K_PWL {$$ = new std::string ("PWL");} 
| K_R0 {$$ = new std::string ("R0");} 
| K_R90 {$$ = new std::string ("R90");} 
| K_R180 {$$ = new std::string ("R180");} 
| K_R270 {$$ = new std::string ("R270");} 
| K_RANGE {$$ = new std::string ("RANGE");} 
| K_REAL {$$ = new std::string ("REAL");} 
| K_RECOVERY {$$ = new std::string ("RECOVERY");} 
| K_RECT {$$ = new std::string ("RECT");} 
| K_RESISTANCE {$$ = new std::string ("RESISTANCE");} 
| K_RESISTIVE {$$ = new std::string ("RESISTIVE");} 
| K_RING {$$ = new std::string ("RING");} 
| K_RISE {$$ = new std::string ("RISE");} 
| K_RISECS {$$ = new std::string ("RISECS");} 
| K_RISERS {$$ = new std::string ("RISERS");} 
| K_RISESATCUR {$$ = new std::string ("RISESATCUR");} 
| K_RISESATT1 {$$ = new std::string ("RISESATT1");} 
| K_RISESLEWLIMIT {$$ = new std::string ("RISESLEWLIMIT");} 
| K_RISET0 {$$ = new std::string ("RISET0");} 
| K_RISETHRESH {$$ = new std::string ("RISETHRESH");} 
| K_RISEVOLTAGETHRESHOLD {$$ = new std::string ("RISEVOLTAGETHRESHOLD");} 
| K_RMS {$$ = new std::string ("RMS");} 
| K_ROUTING {$$ = new std::string ("ROUTING");} 
| K_ROWABUTSPACING {$$ = new std::string ("ROWABUTSPACING");} 
| K_ROWCOL {$$ = new std::string ("ROWCOL");} 
| K_ROWMAJOR {$$ = new std::string ("ROWMAJOR");} 
| K_ROWMINSPACING {$$ = new std::string ("ROWMINSPACING");} 
| K_ROWPATTERN {$$ = new std::string ("ROWPATTERN");} 
| K_RPERSQ {$$ = new std::string ("RPERSQ");} 
| K_S {$$ = new std::string ("S");} 
| K_SAMENET {$$ = new std::string ("SAMENET");} 
| K_SCANUSE {$$ = new std::string ("SCANUSE");} 
| K_SDFCOND {$$ = new std::string ("SDFCOND");} 
| K_SDFCONDEND {$$ = new std::string ("SDFCONDEND");} 
| K_SDFCONDSTART {$$ = new std::string ("SDFCONDSTART");} 
| K_SETUP {$$ = new std::string ("SETUP");} 
| K_SHAPE {$$ = new std::string ("SHAPE");} 
| K_SHRINKAGE {$$ = new std::string ("SHRINKAGE");} 
| K_SIGNAL {$$ = new std::string ("SIGNAL");} 
| K_SITE {$$ = new std::string ("SITE");} 
| K_SIZE {$$ = new std::string ("SIZE");} 
| K_SKEW {$$ = new std::string ("SKEW");} 
| K_SLOTLENGTH {$$ = new std::string ("SLOTLENGTH");} 
| K_SLOTWIDTH {$$ = new std::string ("SLOTWIDTH");} 
| K_SLOTWIRELENGTH {$$ = new std::string ("SLOTWIRELENGTH");} 
| K_SLOTWIREWIDTH {$$ = new std::string ("SLOTWIREWIDTH");} 
| K_SPLITWIREWIDTH {$$ = new std::string ("SPLITWIREWIDTH");} 
| K_SOFT {$$ = new std::string ("SOFT");} 
| K_SOURCE {$$ = new std::string ("SOURCE");} 
| K_SPACER {$$ = new std::string ("SPACER");} 
| K_SPACING {$$ = new std::string ("SPACING");} 
| K_SPACINGTABLE {$$ = new std::string ("SPACINGTABLE");} 
| K_SPECIALNETS {$$ = new std::string ("SPECIALNETS");} 
| K_STABLE {$$ = new std::string ("STABLE");} 
| K_STACK {$$ = new std::string ("STACK");} 
| K_START {$$ = new std::string ("START");} 
| K_STEP {$$ = new std::string ("STEP");} 
| K_STOP {$$ = new std::string ("STOP");} 
| K_STRING {$$ = new std::string ("STRING");} 
| K_STRUCTURE {$$ = new std::string ("STRUCTURE");} 
| K_SUPPLYSENSITIVITY {$$ = new std::string ("SUPPLYSENSITIVITY");} 
| K_SYMMETRY {$$ = new std::string ("SYMMETRY");} 
| K_TABLE {$$ = new std::string ("TABLE");} 
| K_TABLEAXIS {$$ = new std::string ("TABLEAXIS");} 
| K_TABLEDIMENSION {$$ = new std::string ("TABLEDIMENSION");} 
| K_TABLEENTRIES {$$ = new std::string ("TABLEENTRIES");} 
| K_TAPERRULE {$$ = new std::string ("TAPERRULE");} 
| K_THEN {$$ = new std::string ("THEN");} 
| K_THICKNESS {$$ = new std::string ("THICKNESS");} 
| K_TIEHIGH {$$ = new std::string ("TIEHIGH");} 
| K_TIELOW {$$ = new std::string ("TIELOW");} 
| K_TIEOFFR {$$ = new std::string ("TIEOFFR");} 
| K_TIME {$$ = new std::string ("TIME");} 
| K_TIMING {$$ = new std::string ("TIMING");} 
| K_TO {$$ = new std::string ("TO");} 
| K_TOPIN {$$ = new std::string ("TOPIN");} 
| K_TOPLEFT {$$ = new std::string ("TOPLEFT");} 
| K_TOPOFSTACKONLY {$$ = new std::string ("TOPOFSTACKONLY");} 
| K_TOPRIGHT {$$ = new std::string ("TOPRIGHT");} 
| K_TRACKS {$$ = new std::string ("TRACKS");} 
| K_TRANSITIONTIME {$$ = new std::string ("TRANSITIONTIME");} 
| K_TRISTATE {$$ = new std::string ("TRISTATE");} 
| K_TRUE {$$ = new std::string ("TRUE");} 
| K_TWOEDGES {$$ = new std::string ("TWOEDGES");} 
| K_TWOWIDTHS {$$ = new std::string ("TWOWIDTHS");} 
| K_TYPE {$$ = new std::string ("TYPE");} 
| K_UNATENESS {$$ = new std::string ("UNATENESS");} 
| K_UNITS {$$ = new std::string ("UNITS");} 
| K_UNIVERSALNOISEMARGIN {$$ = new std::string ("UNIVERSALNOISEMARGIN");} 
| K_USE {$$ = new std::string ("USE");} 
| K_USELENGTHTHRESHOLD {$$ = new std::string ("USELENGTHTHRESHOLD");} 
| K_USEMINSPACING {$$ = new std::string ("USEMINSPACING");} 
| K_USER {$$ = new std::string ("USER");} 
| K_USEVIA {$$ = new std::string ("USEVIA");} 
| K_USEVIARULE {$$ = new std::string ("USEVIARULE");} 
| K_VARIABLE {$$ = new std::string ("VARIABLE");} 
| K_VERSION {$$ = new std::string ("VERSION");} 
| K_VERTICAL {$$ = new std::string ("VERTICAL");} 
| K_VHI {$$ = new std::string ("VHI");} 
| K_VIA {$$ = new std::string ("VIA");} 
| K_VIARULE {$$ = new std::string ("VIARULE");} 
| K_VICTIMLENGTH {$$ = new std::string ("VICTIMLENGTH");} 
| K_VICTIMNOISE {$$ = new std::string ("VICTIMNOISE");} 
| K_VIRTUAL {$$ = new std::string ("VIRTUAL");} 
| K_VLO {$$ = new std::string ("VLO");} 
| K_VOLTAGE {$$ = new std::string ("VOLTAGE");} 
| K_VOLTS {$$ = new std::string ("VOLTS");} 
| K_W {$$ = new std::string ("W");} 
| K_WELLTAP {$$ = new std::string ("WELLTAP");} 
| K_WIDTH {$$ = new std::string ("WIDTH");} 
| K_WITHIN {$$ = new std::string ("WITHIN");} 
| K_WIRECAP {$$ = new std::string ("WIRECAP");} 
| K_WIREEXTENSION {$$ = new std::string ("WIREEXTENSION");} 
| K_X {$$ = new std::string ("X");} 
| K_Y {$$ = new std::string ("Y");} 
| K_EQ {$$ = new std::string ("EQ");} 
| K_NE {$$ = new std::string ("NE");} 
| K_LE {$$ = new std::string ("LE");} 
| K_LT {$$ = new std::string ("LT");} 
| K_GE {$$ = new std::string ("GE");} 
| K_GT {$$ = new std::string ("GT");} 
| K_NOT {$$ = new std::string ("NOT");}

lef_file: rules extension_opt  end_library
      {
        // 11/16/2001 - Wanda da Rosa - pcr 408334
        // Return 1 if there are errors
        if (driver.lef_errors)
           return 1;
        if (!driver.hasVer && driver.versionNum < 5.6)
           driver.lefWarning(2001, "VERSION is a required statement on LEF file with version 5.5 and earlier.\nWithout VERSION defined, the LEF file is technically illegal.\nRefer the LEF/DEF 5.5 and earlier Language Reference manual on how to defined this statement.");
        //only pre 5.6, 5.6 it is obsolete
        if (!driver.hasNameCase && driver.versionNum < 5.6)
           driver.lefWarning(2002, "NAMESCASESENSITIVE is a required statement on LEF file with version 5.5 and earlier.\nWithout NAMESCASESENSITIVE defined, the LEF file is technically illegal.\nRefer the LEF/DEF 5.5 and earlier Language Referece manual on how to define this statement.");
        if (!driver.hasBusBit && driver.versionNum < 5.6)
           driver.lefWarning(2003, "BUSBITCHARS is a required statement on LEF file with version 5.5 and earlier.\nWithout BUSBITCHARS defined, the LEF file is technically illegal.\nRefer the LEF/DEF 5.5 and earlier Language Referece manual on how to define this statement.");
        if (!driver.hasDivChar && driver.versionNum < 5.6)
           driver.lefWarning(2004, "DIVIDERCHAR is a required statementon LEF file with version 5.5 and earlier.\nWithout DIVIDECHAR defined, the LEF file is technically illegal.\nRefer the LEF/DEF 5.5 and earlier Language Referece manual on how to define this statement.");

       driver.resetVars();
      }

version: K_VERSION { driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING ';'
      { 
         driver.versionNum = driver.convert_name2num((*$3).c_str());
         if (driver.versionNum > 5.7) {
            char temp[120];
            sprintf(temp,
               "Lef parser 5.7 does not support lef file with version %s. Parser stops executions.", (*$3).c_str());
            driver.lefError(1503, temp);
            //return 1;
         }
/*
         driver.versionNum = $3;         Save the version number for future use */
		 driver.lefrVersionStrCbk(*$3);
		 driver.lefrVersionCbk(driver.versionNum);
         if (driver.versionNum > 5.3 && driver.versionNum < 5.4) {
            driver.ignoreVersion = 1;
         }
         driver.use5_3 = driver.use5_4 = 0;
         driver.lef_errors = 0;
         if (driver.hasVer)     /* More than 1 lef file within the open file */
            driver.resetVars();
         driver.hasVer = 1;
         if (driver.versionNum < 5.6) {
            driver.doneLib = 0;
            driver.lefNamesCaseSensitive = driver.lefReaderCaseSensitive;
         } else {
            driver.doneLib = 1;
            driver.lefNamesCaseSensitive = 1;
         }
         delete $3;
      }
	  | K_VERSION { driver.lefDumbMode = 1; driver.lefNoNum = 1;} DOUBLE ';'
      { 
         driver.versionNum = $3;
         if (driver.versionNum > 5.7) {
            char temp[120];
            sprintf(temp,
               "Lef parser 5.7 does not support lef file with version %f. Parser stops executions.", $3);
            driver.lefError(1503, temp);
            //return 1;
         }
/*
         driver.versionNum = $3;         Save the version number for future use */
		 char tmp[20];
		 snprintf(tmp, sizeof(tmp), "%g", $3);
		 driver.lefrVersionStrCbk(tmp);
		 driver.lefrVersionCbk(driver.versionNum);
         if (driver.versionNum > 5.3 && driver.versionNum < 5.4) {
            driver.ignoreVersion = 1;
         }
         driver.use5_3 = driver.use5_4 = 0;
         driver.lef_errors = 0;
         if (driver.hasVer)     /* More than 1 lef file within the open file */
            driver.resetVars();
         driver.hasVer = 1;
         if (driver.versionNum < 5.6) {
            driver.doneLib = 0;
            driver.lefNamesCaseSensitive = driver.lefReaderCaseSensitive;
         } else {
            driver.doneLib = 1;
            driver.lefNamesCaseSensitive = 1;
         }
      }


dividerchar: K_DIVIDERCHAR QSTRING ';'
      {
		  if (!(*$2).empty()) 
		  {
			  driver.lefrDividerCharCbk(*$2);
		  }
		  else 
		  {
			  driver.lefrDividerCharCbk("/");
			  driver.lefWarning(2005, "DIVIDERCHAR has an invalid null value. Value is set to default /");
		  }
		  driver.hasDivChar = 1;
          delete $2;
      }

busbitchars: K_BUSBITCHARS QSTRING ';'
   {
		if (!(*$2).empty()) 
	   {
		   driver.lefrBusBitCharsCbk(*$2);
	   } 
	  else 
	  {
		  driver.lefrBusBitCharsCbk("[]");
		 driver.lefWarning(2006, "BUSBITCHAR has an invalid null value. Value is set to default []");
	  }
	driver.hasBusBit = 1;
    delete $2;
  }

rules: /* empty */
	| rules rule
	| error 
	    { }

end_library: /* empty */
      {
        if (driver.versionNum >= 5.6) {
           driver.doneLib = 1;
           driver.ge56done = 1;
        }
      }
      | K_END K_LIBRARY
      {
        driver.doneLib = 1;
        driver.ge56done = 1;
		driver.lefrLibraryEndCbk();
        // 11/16/2001 - Wanda da Rosa - pcr 408334
        // Return 1 if there are errors
/*
        if (driver.lef_errors)
           return 1;
        if (!driver.hasVer)
           driver.lefWarning(2001, "VERSION is a required statement.");
        if (!driver.hasNameCase)
           driver.lefWarning(2002, "NAMESCASESENSITIVE is a required statement.");
        if (!driver.hasBusBit && driver.versionNum < 5.6)
           driver.lefWarning(2003, "BUSBITCHARS is a required statement.");
        if (!driver.hasDivChar && driver.versionNum < 5.6)
           driver.lefWarning(2004, "DIVIDERCHAR is a required statement.");
        driver.hasVer = 0;
        driver.hasNameCase = 0;
        driver.hasBusBit = 0;
        driver.hasDivChar = 0;
        driver.hasManufactur = 0;
        driver.hasMinfeature = 0;
        driver.hasSite = 0;
*/
      }

rule:  version | busbitchars | case_sensitivity | units_section
    | layer_rule | via | viarule | viarule_generate | dividerchar
    | wireextension | msg_statement
    | spacing_rule | dielectric | minfeature | irdrop | site | macro | array
    | def_statement | nondefault_rule | prop_def_section
    | universalnoisemargin | edgeratethreshold1
    | edgeratescalefactor | edgeratethreshold2
    | noisetable | correctiontable | input_antenna
    | output_antenna | inout_antenna
    | antenna_input | antenna_inout | antenna_output | manufacturing
    | useminspacing | clearancemeasure | maxstack_via
    | create_file_statement
    	;

case_sensitivity: K_NAMESCASESENSITIVE K_ON ';'
	  {
            if (driver.versionNum < 5.6) {
              driver.lefNamesCaseSensitive = TRUE;
			  driver.lefrCaseSensitiveCbk(driver.lefNamesCaseSensitive);
              driver.hasNameCase = 1;
            } 
			else if (driver.caseSensitiveWarnings++ < driver.lefrCaseSensitiveWarnings)
				driver.lefWarning(2007, "NAMESCASESENSITIVE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
	  }
      | K_NAMESCASESENSITIVE K_OFF ';'
	  {
            if (driver.versionNum < 5.6) 
			{
			driver.lefNamesCaseSensitive = FALSE;
			driver.lefrCaseSensitiveCbk(driver.lefNamesCaseSensitive);
			driver.hasNameCase = 1;
            } 
			else 
			{
                if (driver.caseSensitiveWarnings++ < driver.lefrCaseSensitiveWarnings) 
				{
                  driver.lefError(1504, "NAMESCASESENSITIVE statement is set with OFF.\nStarting version 5.6, NAMESCASENSITIVE is obsolete,\nif it is defined, it has to have the ON value.\nParser stops executions.");
              }
            }
	  }

wireextension: K_NOWIREEXTENSIONATPIN K_ON ';'
    {
      if (driver.versionNum < 5.6) 
	  {
	  driver.lefrNoWireExtensionCbk("ON");
      } 
	  else if (driver.noWireExtensionWarnings++ < driver.lefrNoWireExtensionWarnings)
	  {
	  driver.lefWarning(2008, "NOWIREEXTENSIONATPIN statement is obsolete in version 5.6 or later.\nThe NOWIREEXTENSIONATPIN statement will be ignored.");
	  }
    }
  | K_NOWIREEXTENSIONATPIN K_OFF ';'
    {
      if (driver.versionNum < 5.6) 
	  {
	  driver.lefrNoWireExtensionCbk("OFF");
      } 
	  else if (driver.noWireExtensionWarnings++ < driver.lefrNoWireExtensionWarnings)
	  {
	  driver.lefWarning(2008, "NOWIREEXTENSIONATPIN statement is obsolete in version 5.6 or later.\nThe NOWIREEXTENSIONATPIN statement will be ignored.");
	  }
    }

manufacturing: K_MANUFACTURINGGRID NUMBER ';'
    {
	driver.lefrManufacturingCbk($2);
	  driver.hasManufactur = 1;
    }

useminspacing: K_USEMINSPACING spacing_type spacing_value ';'
  {
    if ((strcmp((*$2).c_str(), "PIN") == 0) && (driver.versionNum >= 5.6)) 
	{
         if (driver.useMinSpacingWarnings++ < driver.lefrUseMinSpacingWarnings)
            driver.lefWarning(2009, "USEMINSPACING PIN statement is obsolete in version 5.6 or later.\n The USEMINSPACING PIN statement will be ignored.");
    } 
	else 
	{
		driver.lefrUseMinSpacing.lefiUseMinSpacing::set((*$2).c_str(), $3);
		driver.lefrUseMinSpacingCbk(driver.lefrUseMinSpacing);
    }
    delete $2;
  }

clearancemeasure: K_CLEARANCEMEASURE clearance_type ';'
    {driver.lefrClearanceMeasureCbk(*$2); delete $2;}

clearance_type:
  K_MAXXY   {$$ = new std::string ("MAXXY");}
  | K_EUCLIDEAN   {$$ = new std::string ("EUCLIDEAN");}

spacing_type:
  K_OBS     {$$ = new std::string ("OBS");}
  | K_PIN   {$$ = new std::string ("PIN");}

spacing_value:
  K_ON      {$$ = 1;}
  | K_OFF   {$$ = 0;}

units_section: start_units units_rules K_END K_UNITS
    { driver.lefrUnitsCbk(driver.lefrUnits);}

start_units: K_UNITS
    {
      driver.lefrUnits.lefiUnits::clear();
      if (driver.hasManufactur) 
	  {
        if (driver.unitsWarnings++ < driver.lefrUnitsWarnings) 
		{driver.lefError(1505, "MANUFACTURINGGRID statement was defined before UNITS.\nRefer the LEF Language Reference manual for the order of LEF statements.");}
      }
      if (driver.hasMinfeature) {
        if (driver.unitsWarnings++ < driver.lefrUnitsWarnings) 
		{
          driver.lefError(1712, "MINFEATURE statement was defined before UNITS.\nRefer the LEF Language Reference manual for the order of LEF statements.");
        }
      }
      if (driver.versionNum < 5.6) {
        if (driver.hasSite) {/*SITE is defined before UNIT and is illegal in pre 5.6*/
          driver.lefError(1713, "SITE statement was defined before UNITS.\nRefer the LEF Language Reference manual for the order of LEF statements.");
        }
      }
    }

units_rules: /* empty */
  | units_rules units_rule
  ;

units_rule: K_TIME K_NANOSECONDS NUMBER ';'
    { driver.lefrUnits.lefiUnits::setTime($3); }
  | K_CAPACITANCE K_PICOFARADS NUMBER ';'
    { driver.lefrUnits.lefiUnits::setCapacitance($3); }
  | K_RESISTANCE K_OHMS NUMBER ';'
    { driver.lefrUnits.lefiUnits::setResistance($3); }
  | K_POWER K_MILLIWATTS NUMBER ';'
    { driver.lefrUnits.lefiUnits::setPower($3); }
  | K_CURRENT K_MILLIAMPS NUMBER ';'
    { driver.lefrUnits.lefiUnits::setCurrent($3); }
  | K_VOLTAGE K_VOLTS NUMBER ';'
    { driver.lefrUnits.lefiUnits::setVoltage($3); }
  | K_DATABASE K_MICRONS NUMBER ';'
    { 
      if(driver.validNum((int)$3)) {
            driver.lefrUnits.lefiUnits::setDatabase("MICRONS", $3);
      }
    }
  | K_FREQUENCY K_MEGAHERTZ NUMBER ';'
    { driver.lefrUnits.lefiUnits::setFrequency($3); }

layer_rule: start_layer 
    layer_options end_layer
    { driver.lefrLayerCbk(driver.lefrLayer);}

start_layer: K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1; } GSTRING 
    { 
      if (driver.lefrHasMaxVS) {   /* 5.5 */
          if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
            driver.lefError(1506, "A MAXVIASTACK statement is defined before the LAYER statement.\nRefer to the LEF Language Reference manual for the order of LEF statements.");
          }
      }
	  driver.lefrLayer.lefiLayer::setName((*$3).c_str());
      driver.useLenThr = 0;
      driver.layerCut = 0;
      driver.layerMastOver = 0;
      driver.layerRout = 0;
      driver.layerDir = 0;
      driver.lefrHasLayer = 1;
      driver.layerName = *$3;
      driver.hasType = 0;
      driver.hasPitch = 0;
      driver.hasWidth = 0;
      driver.hasDirection = 0;
      driver.hasParallel = 0;
      driver.hasInfluence = 0;
      driver.hasTwoWidths = 0;
      driver.lefrHasSpacingTbl = 0;
      driver.lefrHasSpacing = 0;
      delete $3;
    }

end_layer: K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1; } GSTRING
    { 
      if (driver.layerName != *$3) {
          if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
             driver.outMsg = (char*)lefMalloc(10000);
             sprintf (driver.outMsg,
                "END LAYER name %s is different from the LAYER name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.layerName.c_str());
             driver.lefError(1507, driver.outMsg);
             lefFree(driver.outMsg);
          } 
      } 
      if (!driver.lefrRelaxMode) {
        if (driver.hasType == 0) {
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1508, "TYPE statement is a required statement in a LAYER and it is not defined.");
            }
        }
        if ((driver.layerRout == 1) && (driver.hasPitch == 0)) {
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1509, "PITCH statement is a required statement in a LAYER with type ROUTING and it is not defined.");
          }
        }
        if ((driver.layerRout == 1) && (driver.hasWidth == 0)) {
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1510, "WIDTH statement is a required statement in a LAYER with type ROUTING and it is not defined.");
          }
        }
        if ((driver.layerRout == 1) && (driver.hasDirection == 0)) {
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg, "The DIRECTION statement which is required in a LAYER with TYPE ROUTING is not defined in LAYER %s.\nUpdate your lef file and add the DIRECTION statement for layer %s.", (*$3).c_str(), (*$3).c_str());
              driver.lefError(1511, driver.outMsg);
              lefFree(driver.outMsg);
            }
        }
      }
      delete $3;
    }

layer_options:
  /* empty */
    { }
  | layer_options layer_option    /* Use left recursions */
/*  WANDA
  | layer_option layer_options
*/
    { }

layer_option:
  K_ARRAYSPACING                   /* 5.7 */
    {
       /* let setArraySpacingCutSpacing to set the data */
    }
    layer_arraySpacing_long
    layer_arraySpacing_width
    K_CUTSPACING NUMBER K_ARRAYCUTS NUMBER K_SPACING NUMBER
    {
         driver.lefrLayer.lefiLayer::setArraySpacingCut($6);
         driver.lefrLayer.lefiLayer::addArraySpacingArray((int)$8, $10);
         driver.arrayCutsVal = (int)$8;  /* set the value */
         driver.arrayCutsWar = 0;
    }
    layer_arraySpacing_arraycuts ';'
    {
      if (driver.versionNum < 5.7) {
         driver.outMsg = (char*)lefMalloc(10000);
         sprintf(driver.outMsg,
           "ARRAYSPACING is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
         driver.lefError(1685, driver.outMsg);
         lefFree(driver.outMsg);
      }
    }
  | K_TYPE layer_type ';'
    {
         driver.lefrLayer.lefiLayer::setType((*$2).c_str());
      driver.hasType = 1;
      delete $2;
    }
  | K_PITCH NUMBER ';'
    { 
      driver.lefrLayer.lefiLayer::setPitch($2);
      driver.hasPitch = 1;  
    }
  | K_PITCH NUMBER NUMBER ';'
    { 
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setPitchXY($2, $3);
      driver.hasPitch = 1;  
    }
  | K_DIAGPITCH NUMBER ';'
    { 
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setDiagPitch($2);
    }
  | K_DIAGPITCH NUMBER NUMBER ';'
    { 
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setDiagPitchXY($2, $3);
    }
  | K_OFFSET NUMBER ';'
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setOffset($2);
    }
  | K_OFFSET NUMBER NUMBER ';'
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setOffsetXY($2, $3);
    }
  | K_DIAGWIDTH NUMBER ';'
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setDiagWidth($2);
    }
  | K_DIAGSPACING NUMBER ';'
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setDiagSpacing($2);
    }
  | K_WIDTH NUMBER ';'    // CUT & ROUTING
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setWidth($2);
      driver.hasWidth = 1;  
    }
  | K_AREA NUMBER ';'
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setArea($2);
    }
  | K_SPACING NUMBER
    {
      driver.hasSpCenter = 0;       /* reset to 0, only once per spacing is allowed */
      driver.hasSpSamenet = 0;
      driver.hasSpParallel = 0;
      driver.hasSpLayer = 0;
      driver.layerCutSpacing = $2;  /* for error message purpose */
      // 11/22/99 - Wanda da Rosa, PCR 283762
      //            Issue an error is this is defined in masterslice
      if (driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1512, "It is illegal to define a SPACING statement in LAYER with TYPE MASTERSLICE or OVERLAP. Parser stops executions.");
            }
         }
      }
      // 5.5 either SPACING or SPACINGTABLE, not both for routing layer only
      if (driver.layerRout) {
        if (driver.lefrHasSpacingTbl && driver.versionNum < 5.7) {
           if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
              if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                driver.lefWarning(2010, "It is illegal to have both SPACING rules & SPACINGTABLE rules within a ROUTING layer");
              }
           }
        }
        if (/*driver.lefrLayerCbk*/ 1)
           driver.lefrLayer.lefiLayer::setSpacingMin($2);
        driver.lefrHasSpacing = 1;
      } else { 
        if (/*driver.lefrLayerCbk*/ 1)
           driver.lefrLayer.lefiLayer::setSpacingMin($2);
      }
    }
    layer_spacing_opts
    layer_spacing_cut_routing ';'
  | K_SPACINGTABLE K_ORTHOGONAL K_WITHIN NUMBER K_SPACING NUMBER   /* 5.7 */
    {
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::setSpacingTableOrtho();
      if (/*driver.lefrLayerCbk*/ 1) /* due to converting to C, else, convertor produce */
         driver.lefrLayer.lefiLayer::addSpacingTableOrthoWithin($4, $6);/*bad code*/
    }
    layer_spacingtable_opts ';'
    {
      if (driver.versionNum < 5.7) {
         driver.outMsg = (char*)lefMalloc(10000);
         sprintf(driver.outMsg,
           "SPACINGTABLE ORTHOGONAL is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
         driver.lefError(1694, driver.outMsg);
         lefFree(driver.outMsg);
      }
    }
  | K_DIRECTION layer_direction ';'
    {
      driver.layerDir = 1;
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1513, "DIRECTION statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setDirection(*$2);
      driver.hasDirection = 1;  
      delete $2;
    }
  | K_RESISTANCE K_RPERSQ NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1514, "RESISTANCE statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setResistance($3);
    }
  | K_RESISTANCE K_RPERSQ K_PWL '(' res_points ')' ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1515, "RESISTANCE statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
    }
  | K_CAPACITANCE K_CPERSQDIST NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1516, "CAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setCapacitance($3);
    }
  | K_CAPACITANCE K_CPERSQDIST K_PWL '(' cap_points ')' ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1517, "CAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
    }
  | K_HEIGHT NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1518, "HEIGHT statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setHeight($2);
    }
  | K_WIREEXTENSION NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1519, "WIREEXTENSION statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setWireExtension($2);
    }
  | K_THICKNESS NUMBER ';'
    {
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1520, "THICKNESS statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setThickness($2);
    }
  | K_SHRINKAGE NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1521, "SHRINKAGE statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setShrinkage($2);
    }
  | K_CAPMULTIPLIER NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1522, "CAPMULTIPLIER statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setCapMultiplier($2);
    }
  | K_EDGECAPACITANCE NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1523, "EDGECAPACITANCE statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setEdgeCap($2);
    }
/* 3/24/2000 - Wanda da Rosa.  Same call for 5.4 but more options.
              Syntax is handle 1 page down.
  | K_ANTENNAAREAFACTOR NUMBER ';'
    {
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1524, "ANTENNAAREAFACTOR statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              CHKERR();
            }
      }
      if (driver.lefrLayerCbk) driver.lefrLayer.lefiLayer::setAntennaArea($2);
    }
*/
  | K_ANTENNALENGTHFACTOR NUMBER ';'
    { /* 5.3 syntax */
      driver.use5_3 = 1;
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1525, "ANTENNALENGTHFACTOR statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      } else if (driver.versionNum >= 5.4) {
         if (driver.use5_4) {
            if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
               if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                  driver.outMsg = (char*)lefMalloc(10000);
                  sprintf (driver.outMsg,
                    "ANTENNALENGTHFACTOR statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNALENGTHFACTOR syntax, which is illegal.", driver.versionNum);
                  driver.lefError(1526, driver.outMsg);
                  lefFree(driver.outMsg);
                  /*CHKERR();*/
               }
            }
         }
      }

      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaLength($2);
    }
  | K_CURRENTDEN NUMBER ';'
    {
      if (driver.versionNum < 5.2) {
         if (!driver.layerRout) {
            if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
               if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                 driver.lefError(1526, "CURRENTDEN statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
                 /*CHKERR();*/
               }
            }
         }
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setCurrentDensity($2);
      } else {
         if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
            driver.lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            /*CHKERR();*/
         }
      }
    }
  | K_CURRENTDEN K_PWL '(' current_density_pwl_list ')' ';'
    { 
      if (driver.versionNum < 5.2) {
         if (!driver.layerRout) {
            if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
               if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                 driver.lefError(1526, "CURRENTDEN statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
                 /*CHKERR();*/
               }
            }
         }
      } else {
         if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
            driver.lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            /*CHKERR();*/
         }
      }
    }
  | K_CURRENTDEN '(' NUMBER NUMBER ')' ';'
    {
      if (driver.versionNum < 5.2) {
         if (!driver.layerRout) {
            if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
               if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                 driver.lefError(1526, "CURRENTDEN statement can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
                 /*CHKERR();*/
               }
            }
         }
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setCurrentPoint($3, $4);
      } else {
         if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
            driver.lefWarning(2079, "CURRENTDEN statement is obsolete in version 5.2 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.2 or later.");
            /*CHKERR();*/
         }
      }
    }
  | K_PROPERTY { driver.lefDumbMode = 10000000; driver.lefRealNum = 1; driver.lefInProp = 1; } layer_prop_list ';'
    {
      driver.lefDumbMode = 0;
      driver.lefRealNum = 0;
      driver.lefInProp = 0;
    }
  | K_ACCURRENTDENSITY layer_table_type
    {
      if (driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1527, "ACCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addAccurrentDensity((*$2).c_str());
      delete $2;
    }
    layer_frequency
  | K_ACCURRENTDENSITY layer_table_type NUMBER ';'
    {
      if (driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1527, "ACCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) {
           driver.lefrLayer.lefiLayer::addAccurrentDensity((*$2).c_str());
           driver.lefrLayer.lefiLayer::setAcOneEntry($3);
           delete $2;
      }
    }
  | K_DCCURRENTDENSITY K_AVERAGE NUMBER ';'
    {
      if (driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1528, "DCCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::addDccurrentDensity("AVERAGE");
         driver.lefrLayer.lefiLayer::setDcOneEntry($3);
      }
    }
  | K_DCCURRENTDENSITY K_AVERAGE K_CUTAREA NUMBER
    {
      if (driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1528, "DCCURRENTDENSITY statement can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (!driver.layerCut) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1529, "CUTAREA statement can only be defined in LAYER with type CUT. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::addDccurrentDensity("AVERAGE");
         driver.lefrLayer.lefiLayer::addNumber($4);
      }
    }
    number_list ';'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addDcCutarea(); }
    dc_layer_table
  | K_DCCURRENTDENSITY K_AVERAGE K_WIDTH NUMBER
    {
      if (driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1528, "DCCURRENTDENSITY can't be defined in LAYER with TYPE MASTERSLICE or OVERLAP. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1530, "WIDTH statement can only be defined in LAYER with type ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::addDccurrentDensity("AVERAGE");
         driver.lefrLayer.lefiLayer::addNumber($4);
      }
    }
    number_list ';'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addDcWidth(); }
    dc_layer_table

/* 3/23/2000 - 5.4 syntax.  Wanda da Rosa */
  | K_ANTENNAAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNAAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1531, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNAAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNAAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1532, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (!driver.layerRout && !driver.layerCut && driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1533, "ANTENNAAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaAreaRatio($2);
    }
  | K_ANTENNADIFFAREARATIO
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNADIFFAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1532, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNADIFFAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNADIFFAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1533, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (!driver.layerRout && !driver.layerCut && driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1534, "ANTENNADIFFAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      driver.antennaType = lefiAntennaDAR; 
    }
    layer_antenna_pwl ';'
  | K_ANTENNACUMAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNACUMAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1535, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNACUMAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1536, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (!driver.layerRout && !driver.layerCut && driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1537, "ANTENNACUMAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaCumAreaRatio($2);
    }
  | K_ANTENNACUMDIFFAREARATIO
    {  /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNACUMDIFFAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1538, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNACUMDIFFAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMDIFFAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1539, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (!driver.layerRout && !driver.layerCut && driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1540, "ANTENNACUMDIFFAREARATIO statement can only be defined in LAYER with TYPE ROUTING or CUT. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      driver.antennaType = lefiAntennaCDAR;
    }
    layer_antenna_pwl ';'
  | K_ANTENNAAREAFACTOR NUMBER
    { /* both 5.3  & 5.4 syntax */
      if (!driver.layerRout && !driver.layerCut && driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1541, "ANTENNAAREAFACTOR can only be defined in LAYER with TYPE ROUTING or CUT. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      /* this does not need to check, since syntax is in both 5.3 & 5.4 */
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaAreaFactor($2);
      driver.antennaType = lefiAntennaAF;
    }
    layer_antenna_duo ';'
  | K_ANTENNASIDEAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1542, "ANTENNASIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNASIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1543, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNASIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNASIDEAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1544, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaSideAreaRatio($2);
    }
  | K_ANTENNADIFFSIDEAREARATIO
    {  /* 5.4 syntax */
      driver.use5_4 = 1;
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1545, "ANTENNADIFFSIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNADIFFSIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1546, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNADIFFSIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNADIFFSIDEAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1547, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      driver.antennaType = lefiAntennaDSAR;
    }
    layer_antenna_pwl ';'
  | K_ANTENNACUMSIDEAREARATIO NUMBER ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1548, "ANTENNACUMSIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNACUMSIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1549, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNACUMSIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMSIDEAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1550, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaCumSideAreaRatio($2);
    }
  | K_ANTENNACUMDIFFSIDEAREARATIO
    {  /* 5.4 syntax */
      driver.use5_4 = 1;
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1551, "ANTENNACUMDIFFSIDEAREARATIO can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNACUMDIFFSIDEAREARATIO statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1552, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNACUMDIFFSIDEAREARATIO statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNACUMDIFFSIDEAREARATIO syntax, which is illegal.", driver.versionNum);
               driver.lefError(1553, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      driver.antennaType = lefiAntennaCDSAR;
    }
    layer_antenna_pwl ';'
  | K_ANTENNASIDEAREAFACTOR NUMBER
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (!driver.layerRout && (driver.layerCut || driver.layerMastOver)) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1554, "ANTENNASIDEAREAFACTOR can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNASIDEAREAFACTOR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1555, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNASIDEAREAFACTOR statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNASIDEAREAFACTOR syntax, which is illegal.", driver.versionNum);
               driver.lefError(1556, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaSideAreaFactor($2);
      driver.antennaType = lefiAntennaSAF;
    }
    layer_antenna_duo ';'
  | K_ANTENNAMODEL /* 5.5 */
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (!driver.layerRout && !driver.layerCut && driver.layerMastOver) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1557, "ANTENNAMODEL can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNAMODEL statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1558, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.use5_3) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "ANTENNAMODEL statement is a version 5.4 or earlier syntax.\nYour lef file with version %g, has both old and new ANTENNAMODEL syntax, which is illegal.", driver.versionNum);
               driver.lefError(1559, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      driver.antennaType = lefiAntennaO;
    }
    layer_oxide ';'
  | K_ANTENNACUMROUTINGPLUSCUT ';'        /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
         driver.outMsg = (char*)lefMalloc(10000);
         sprintf(driver.outMsg,
           "ANTENNACUMROUTINGPLUSCUT is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
         driver.lefError(1686, driver.outMsg);
         lefFree(driver.outMsg);
         /*CHKERR();*/
      } else {
         if (!driver.layerRout && !driver.layerCut) {
            if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
               if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                  driver.lefError(1560, "ANTENNACUMROUTINGPLUSCUT can only be defined in LAYER with type ROUTING or CUT. Parser stops executions.");
                  /*CHKERR();*/
               }
            }
         }
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaCumRoutingPlusCut();
      }
    }
  | K_ANTENNAGATEPLUSDIFF NUMBER ';'      /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
         driver.outMsg = (char*)lefMalloc(10000);
         sprintf(driver.outMsg,
           "ANTENNAGATEPLUSDIFF is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
         driver.lefError(1687, driver.outMsg);
         lefFree(driver.outMsg);
         /*CHKERR();*/
      } else {
         if (!driver.layerRout && !driver.layerCut) {
            if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
               if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                  driver.lefError(1561, "ANTENNAGATEPLUSDIFF can only be defined in LAYER with type ROUTING or CUT. Parser stops executions.");
                  /*CHKERR();*/
               }
            }
         }
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaGatePlusDiff($2);
      }
    }
  | K_ANTENNAAREAMINUSDIFF NUMBER ';'     /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
         driver.outMsg = (char*)lefMalloc(10000);
         sprintf(driver.outMsg,
           "ANTENNAAREAMINUSDIFF is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
         driver.lefError(1688, driver.outMsg);
         lefFree(driver.outMsg);
         /*CHKERR();*/
      } else {
         if (!driver.layerRout && !driver.layerCut) {
            if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
               if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                  driver.lefError(1562, "ANTENNAAREAMINUSDIFF can only be defined in LAYER with type ROUTING or CUT. Parser stops executions.");
                  /*CHKERR();*/
               }
            }
         }
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setAntennaAreaMinusDiff($2);
      }
    }
  | K_ANTENNAAREADIFFREDUCEPWL '(' pt pt            /* 5.7 */
    {
      if (!driver.layerRout && !driver.layerCut) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1563, "ANTENNAAREADIFFREDUCEPWL can only be defined in LAYER with type ROUTING or CUT. Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) { /* require min 2 points, set the 1st 2 */
         driver.lefrAntennaPWLPtr=(lefiAntennaPWL*)lefMalloc(sizeof(lefiAntennaPWL));
         driver.lefrAntennaPWLPtr->lefiAntennaPWL::Init();
         driver.lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($3->x, $3->y);
         driver.lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($4->x, $4->y);
      }
      delete $3; delete $4;
    } 
    layer_diffusion_ratios ')' ';'
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setAntennaPWL(lefiAntennaADR, driver.lefrAntennaPWLPtr);
    }
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "ANTENNAAREADIFFREDUCEPWL is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1689, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      }
    }
  | K_SLOTWIREWIDTH NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotWireWidth($2);
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
               driver.lefWarning(2011, "SLOTWIREWIDTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "SLOTWIREWIDTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1564, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotWireWidth($2);
    }
  | K_SLOTWIRELENGTH NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotWireLength($2);
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
               driver.lefWarning(2012, "SLOTWIRELENGTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "SLOTWIRELENGTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1565, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotWireLength($2);
    }
  | K_SLOTWIDTH NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotWidth($2);
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
               driver.lefWarning(2013, "SLOTWIDTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "SLOTWIDTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1566, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotWidth($2);
    }
  | K_SLOTLENGTH NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotLength($2);
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
               driver.lefWarning(2014, "SLOTLENGTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "SLOTLENGTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1567, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSlotLength($2);
    }
  | K_MAXADJACENTSLOTSPACING NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaxAdjacentSlotSpacing($2);
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
               driver.lefWarning(2015, "MAXADJACENTSLOTSPACING statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MAXADJACENTSLOTSPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1568, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaxAdjacentSlotSpacing($2);
    }
  | K_MAXCOAXIALSLOTSPACING NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaxCoaxialSlotSpacing($2);
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
                driver.lefWarning(2016, "MAXCOAXIALSLOTSPACING statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MAXCOAXIALSLOTSPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1569, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaxCoaxialSlotSpacing($2);
    }
  | K_MAXEDGESLOTSPACING NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaxEdgeSlotSpacing($2);
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
               driver.lefWarning(2017, "MAXEDGESLOTSPACING statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MAXEDGESLOTSPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1570, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else
         if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaxEdgeSlotSpacing($2);
    }
  | K_SPLITWIREWIDTH NUMBER ';'
    { /* 5.4 syntax */
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum >= 5.7) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings)
               driver.lefWarning(2018, "SPLITWIREWIDTH statement is obsolete in version 5.7 or later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.7 or later.");
         }
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "SPLITWIREWIDTH statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1571, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setSplitWireWidth($2);
    }
  | K_MINIMUMDENSITY NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MINIMUMDENSITY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1572, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMinimumDensity($2);
    }
  | K_MAXIMUMDENSITY NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MAXIMUMDENSITY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1573, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaximumDensity($2);
    }
  | K_DENSITYCHECKWINDOW NUMBER NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "DENSITYCHECKWINDOW statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1574, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setDensityCheckWindow($2, $3);
    }
  | K_DENSITYCHECKSTEP NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "DENSITYCHECKSTEP statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1575, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setDensityCheckStep($2);
    }
  | K_FILLACTIVESPACING NUMBER ';'
    { /* 5.4 syntax, pcr 394389 */
      if (driver.ignoreVersion) {
         /* do nothing */
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "FILLACTIVESPACING statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1576, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setFillActiveSpacing($2);
    }
  | K_MAXWIDTH NUMBER ';'              /* 5.5 */
    {
      // 5.5 MAXWIDTH, is for routing layer only
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.lefError(1577, "MAXWIDTH statement can only be defined in LAYER with TYPE ROUTING.  Parser stops executions.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MAXWIDTH statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1578, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMaxwidth($2);
    }
  | K_MINWIDTH NUMBER ';'              /* 5.5 */
    {
      // 5.5 MINWIDTH, is for routing layer only
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1579, "MINWIDTH statement can only be defined in LAYER with TYPE ROUTING.  Parser stops executions.");
              /*CHKERR();*/
            }
         }
      }
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MINWIDTH statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1580, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setMinwidth($2);
    }
  | K_MINENCLOSEDAREA NUMBER           /* 5.5 */
    {
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "MINENCLOSEDAREA statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1581, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addMinenclosedarea($2);
    }
    layer_minen_width ';'
  | K_MINIMUMCUT NUMBER K_WIDTH NUMBER /* 5.5 */
    { /* pcr 409334 */
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addMinimumcut((int)$2, $4); 
      driver.hasLayerMincut = 0;
    }
    layer_minimumcut_within
    layer_minimumcut_from
    layer_minimumcut_length ';'
    {
      if (!driver.hasLayerMincut) {   /* FROMABOVE nor FROMBELOW is set */
         if (/*driver.lefrLayerCbk*/ 1)
             driver.lefrLayer.lefiLayer::addMinimumcutConnect((char*)"");
      }
    }
  | K_MINSTEP NUMBER               /* 5.5 */
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addMinstep($2);
    }
    layer_minstep_options ';'      /* 5.6 */
    {
    }
  | K_PROTRUSIONWIDTH NUMBER K_LENGTH NUMBER K_WIDTH NUMBER ';'  /* 5.5 */
    {
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "PROTRUSION RULE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1582, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setProtrusion($2, $4, $6);
    }
  | K_SPACINGTABLE                    /* 5.5 */
    {
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "SPACINGTABLE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1583, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      // 5.5 either SPACING or SPACINGTABLE in a layer, not both
      if (driver.lefrHasSpacing && driver.layerRout && driver.versionNum < 5.7) {
         if (/*driver.lefrLayerCbk*/ 1)  /* write warning only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefWarning(2010, "It is illegal to have both SPACING rules & SPACINGTABLE rules within a ROUTING layer");
            }
      } 
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addSpacingTable();
      driver.lefrHasSpacingTbl = 1;
    }
    sp_options ';'
  /* 10/12/2003 - 5.6 syntax */
  | K_ENCLOSURE layer_enclosure_type_opt NUMBER NUMBER
    {
      if (driver.versionNum < 5.6) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ENCLOSURE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1584, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addEnclosure((*$2).c_str(), $3, $4);
         delete $2;
    }
    layer_enclosure_width_opt ';'
  /* 12/30/2003 - 5.6 syntax */
  | K_PREFERENCLOSURE layer_enclosure_type_opt NUMBER NUMBER
    {
      if (driver.versionNum < 5.6) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "PREFERENCLOSURE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1585, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addPreferEnclosure((*$2).c_str(), $3, $4);
         delete $2;
    }
    layer_preferenclosure_width_opt ';'
  | K_RESISTANCE NUMBER ';'
    {
      if (driver.versionNum < 5.6) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "RESISTANCE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1586, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else {
         if (/*driver.lefrLayerCbk*/ 1)
            driver.lefrLayer.lefiLayer::setResPerCut($2);
      }
    }
  | K_DIAGMINEDGELENGTH NUMBER ';'
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1587, "DIAGMINEDGELENGTH can only be defined in LAYER with TYPE ROUTING. Parser stops executions.");
              /*CHKERR();*/
            }
         }
      } else if (driver.versionNum < 5.6) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "DIAGMINEDGELENGTH statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1588, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else {
         if (/*driver.lefrLayerCbk*/ 1)
            driver.lefrLayer.lefiLayer::setDiagMinEdgeLength($2);
      }
    }
  | K_MINSIZE
    {
      // Use the polygon code to retrieve the points for MINSIZE
      driver.lefrGeometriesPtr = (lefiGeometries*)lefMalloc(sizeof(lefiGeometries));
      driver.lefrGeometriesPtr->lefiGeometries::Init();
      driver.lefrDoGeometries = 1;
    }
    firstPt otherPts ';' 
    {
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrGeometriesPtr->lefiGeometries::addPolygon();
         driver.lefrLayer.lefiLayer::setMinSize(driver.lefrGeometriesPtr);
      }
     driver.lefrDoGeometries = 0;
      driver.lefrGeometriesPtr->lefiGeometries::Destroy();
                                       // Don't need the object anymore
      lefFree(driver.lefrGeometriesPtr);
    }

layer_arraySpacing_long:            // 5.7
  /* empty */
  | K_LONGARRAY
    {
        if (/*driver.lefrLayerCbk*/ 1)
           driver.lefrLayer.lefiLayer::setArraySpacingLongArray();
    }

layer_arraySpacing_width:           // 5.7
  /* empty */
  | K_WIDTH NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::setArraySpacingWidth($2);
    }

layer_arraySpacing_arraycuts:       // 5.7
  /* empty */
  | layer_arraySpacing_arraycut layer_arraySpacing_arraycuts

layer_arraySpacing_arraycut:
  K_ARRAYCUTS NUMBER K_SPACING NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addArraySpacingArray((int)$2, $4);
         if (driver.arrayCutsVal > (int)$2) {
            /* Mulitiple ARRAYCUTS value needs to me in ascending order */
            if (!driver.arrayCutsWar) {
               if (driver.layerWarnings++ < driver.lefrLayerWarnings)
                  driver.lefWarning(2080, "The number of cut values in multiple ARRAYSPACING ARRAYCUTS are not in increasing order.\nTo be consistent with the documentation, update the cut values to increasing order.");
               driver.arrayCutsWar = 1;
            }
         }
         driver.arrayCutsVal = (int)$2;
    }

sp_options:
  K_PARALLELRUNLENGTH NUMBER
    { 
      if (driver.hasInfluence) {  // 5.5 - INFLUENCE table must follow a PARALLEL
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1589, "An INFLUENCE table statement was defined before the PARALLELRUNLENGTH table statement.\nINFLUENCE table statement should be defined following the PARALLELRUNLENGTH.\nChange the LEF file and rerun the parser.");
              /*CHKERR();*/
            }
         }
      }
      if (driver.hasParallel) { // 5.5 - Only one PARALLEL table is allowed per layer
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1590, "There is multiple PARALLELRUNLENGTH table statements are defined within a layer.\nAccording to the LEF Reference Manual, only one PARALLELRUNLENGTH table statement is allowed per layer.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($2);
      driver.hasParallel = 1;
    }
    number_list
    {
      driver.spParallelLength = driver.lefrLayer.lefiLayer::getNumber();
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addSpParallelLength();
    }
    K_WIDTH NUMBER
    { 
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::addSpParallelWidth($7);
      }
    }
    number_list
    { 
      if (driver.lefrLayer.lefiLayer::getNumber() != driver.spParallelLength) {
         if (/*driver.lefrLayerCbk*/ 1) {
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1591, "The number of length in PARALLELRUNLENGTH is not the same as the number of spacing in WIDTH.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addSpParallelWidthSpacing();
    }
    layer_sp_parallel_widths

  | K_TWOWIDTHS K_WIDTH NUMBER layer_sp_TwoWidthsPRL NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($5);
    }
    number_list
    {
      if (driver.hasParallel) { // 5.7 - Either PARALLEL OR TWOWIDTHS per layer
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1592, "A PARALLELRUNLENGTH statement has already defined in the layer.\nOnly either PARALLELRUNLENGTH or TWOWIDTHS is allowed per layer.");
              /*CHKERR();*/
            }
         }
      }
      if (driver.hasTwoWidths) { // 5.7 - only 1 TWOWIDTHS per layer
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1593, "A TWOWIDTHS table statement has already defined in the layer.\nOnly one TWOWIDTHS statement is allowed per layer.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addSpTwoWidths($3, $4);
      driver.hasTwoWidths = 1;
    }
    layer_sp_TwoWidths
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "TWOWIDTHS is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1697, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      } 
    }
  | K_INFLUENCE K_WIDTH NUMBER K_WITHIN NUMBER K_SPACING NUMBER
    {
      if (driver.hasInfluence) {  // 5.5 - INFLUENCE table must follow a PARALLEL
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1594, "A INFLUENCE table statement has already defined in the layer.\nOnly one INFLUENCE statement is allowed per layer.");
              /*CHKERR();*/
            }
         }
      }
      if (!driver.hasParallel) {  // 5.5 - INFLUENCE must follow a PARALLEL
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1595, "An INFLUENCE table statement has already defined beofre the layer.\nINFLUENCE statement has to be defined after the PARALLELRUNLENGTH table statement in the layer.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::setInfluence();
         driver.lefrLayer.lefiLayer::addSpInfluence($3, $5, $7);
      }
    }
    layer_sp_influence_widths

layer_spacingtable_opts:      /* 5.7 */
  /* empty */
  | layer_spacingtable_opt layer_spacingtable_opts

layer_spacingtable_opt:
  K_WITHIN NUMBER K_SPACING NUMBER
  {
    if (/*driver.lefrLayerCbk*/ 1)
       driver.lefrLayer.lefiLayer::addSpacingTableOrthoWithin($2, $4);
  }

layer_enclosure_type_opt: 
    {$$ = new std::string ("NULL");}   /* empty */
  | K_ABOVE  {$$ = new std::string ("ABOVE");}
  | K_BELOW  {$$ = new std::string ("BELOW");}

layer_enclosure_width_opt:  /* empty */
  | K_WIDTH NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::addEnclosureWidth($2);
      }
    }
  layer_enclosure_width_except_opt
  | K_LENGTH NUMBER              /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
         driver.outMsg = (char*)lefMalloc(10000);
         sprintf(driver.outMsg,
           "LENGTH is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
         driver.lefError(1691, driver.outMsg);
         lefFree(driver.outMsg);
         /*CHKERR();*/
      } else {
         if (/*driver.lefrLayerCbk*/ 1) {
            driver.lefrLayer.lefiLayer::addEnclosureLength($2);
         }
      }
    }
    
layer_enclosure_width_except_opt: /* empty */
  | K_EXCEPTEXTRACUT NUMBER       /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
         driver.outMsg = (char*)lefMalloc(10000);
         sprintf(driver.outMsg,
           "EXCEPTEXTRACUT is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
         driver.lefError(1690, driver.outMsg);
         lefFree(driver.outMsg);
         /*CHKERR();*/
      } else {
         if (/*driver.lefrLayerCbk*/ 1) {
            driver.lefrLayer.lefiLayer::addEnclosureExceptEC($2);
         }
      }
    }

layer_preferenclosure_width_opt:  /* empty */
  | K_WIDTH NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::addPreferEnclosureWidth($2);
      }
    }
    
layer_minimumcut_within: /* empty */
  | K_WITHIN NUMBER
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "MINIMUMCUT WITHIN is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1700, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      } else {
         if (/*driver.lefrLayerCbk*/ 1) {
            driver.lefrLayer.lefiLayer::addMinimumcutWithin($2);
         }
      }
    }

layer_minimumcut_from: /* empty */
  | K_FROMABOVE
    {
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "FROMABOVE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1596, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      driver.hasLayerMincut = 1;
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addMinimumcutConnect((char*)"FROMABOVE");

    }
  | K_FROMBELOW
    {
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "FROMBELOW statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1597, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      }
      driver.hasLayerMincut = 1;
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addMinimumcutConnect((char*)"FROMBELOW");
    }

layer_minimumcut_length: /* empty */
  | K_LENGTH NUMBER K_WITHIN NUMBER
    {   
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "LENGTH WITHIN statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1598, driver.outMsg);
               lefFree(driver.outMsg);
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addMinimumcutLengDis($2, $4);
    }

layer_minstep_options: /* empty */
  | layer_minstep_options layer_minstep_option  /* Use left recursions */
/* WANDA
  | layer_minstep_option layer_minstep_options
*/

layer_minstep_option:
  layer_minstep_type
  {
    if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addMinstepType((*$1).c_str());
    delete $1;
  }
  | K_LENGTHSUM NUMBER
  {
    if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addMinstepLengthsum($2);
  }
  | K_MAXEDGES NUMBER                  /* 5.7 */
  {
    if (driver.versionNum < 5.7) {
      driver.outMsg = (char*)lefMalloc(10000);
      sprintf(driver.outMsg,
        "MAXEDGES is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
      driver.lefError(1685, driver.outMsg);
      lefFree(driver.outMsg);
      /*CHKERR();*/
    } else
       if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addMinstepMaxedges((int)$2);
  }

layer_minstep_type:
  K_INSIDECORNER {$$ = new std::string ("INSIDECORNER");}
  | K_OUTSIDECORNER {$$ = new std::string ("OUTSIDECORNER");}
  | K_STEP {$$ = new std::string ("STEP");}

layer_antenna_pwl:
  NUMBER
      { if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setAntennaValue(driver.antennaType, $1); }
  | K_PWL '(' pt pt
      { if (/*driver.lefrLayerCbk*/ 1) { /* require min 2 points, set the 1st 2 */
          driver.lefrAntennaPWLPtr = (lefiAntennaPWL*)lefMalloc(sizeof(lefiAntennaPWL));
          driver.lefrAntennaPWLPtr->lefiAntennaPWL::Init();
          driver.lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($3->x, $3->y);
          driver.lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($4->x, $4->y);
        }
        delete $3; delete $4;
      }
    layer_diffusion_ratios ')'
      { if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setAntennaPWL(driver.antennaType, driver.lefrAntennaPWLPtr);
      }

layer_diffusion_ratios: /* empty */
  | layer_diffusion_ratios layer_diffusion_ratio  /* Use left recursions */
/* WANDA
  | layer_diffusion_ratio layer_diffusion_ratios
*/
  ;

layer_diffusion_ratio:
  pt
  { if (/*driver.lefrLayerCbk*/ 1)
      driver.lefrAntennaPWLPtr->lefiAntennaPWL::addAntennaPWL($1->x, $1->y);
    delete $1;
  }

layer_antenna_duo: /* empty */
  | K_DIFFUSEONLY
      { 
        driver.use5_4 = 1;
        if (driver.ignoreVersion) {
           /* do nothing */
        }
        else if ((driver.antennaType == lefiAntennaAF) && (driver.versionNum <= 5.3)) {
           if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
              if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                   "ANTENNAAREAFACTOR with DIFFUSEONLY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                 driver.lefError(1599, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        } else if (driver.use5_3) {
           if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
              if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                   "ANTENNAAREAFACTOR with DIFFUSEONLY statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                 driver.lefError(1599, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        }
        if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setAntennaDUO(driver.antennaType);
      }

layer_table_type:
  K_PEAK       {$$ = new std::string ("PEAK");}
  | K_AVERAGE  {$$ = new std::string ("AVERAGE");}
  | K_RMS      {$$ = new std::string ("RMS");}

layer_frequency:
  K_FREQUENCY NUMBER
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($2); }
  number_list ';'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addAcFrequency(); }
  ac_layer_table_opt
  K_TABLEENTRIES NUMBER
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($9); }
    number_list ';'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addAcTableEntry(); }

ac_layer_table_opt:  /* empty */
  | K_CUTAREA NUMBER
    {
      if (!driver.layerCut) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1600, "CUTAREA statement can only be defined in LAYER with type CUT.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($2);
    }
    number_list ';'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addAcCutarea(); }
  | K_WIDTH NUMBER
    {
      if (!driver.layerRout) {
         if (/*driver.lefrLayerCbk*/ 1) { /* write error only if cbk is set */
            if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1601, "WIDTH can only be defined in LAYER with type ROUTING.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($2);
    }
    number_list ';'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addAcWidth(); }

dc_layer_table:
  K_TABLEENTRIES NUMBER
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($2); }
    number_list ';'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addDcTableEntry(); }

number_list:
  | number_list NUMBER
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($2); }

layer_prop_list:
  layer_prop
  | layer_prop_list layer_prop
  ;

layer_prop:
  STRING STRING
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        char propTp;
        propTp = driver.lefrLayerProp.lefiPropType::propType((*$1).c_str());
        driver.lefrLayer.lefiLayer::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING QSTRING
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        char propTp;
        propTp = driver.lefrLayerProp.lefiPropType::propType((*$1).c_str());
        driver.lefrLayer.lefiLayer::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING NUMBER
    {
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (/*driver.lefrLayerCbk*/ 1) {
        char propTp;
        propTp = driver.lefrLayerProp.lefiPropType::propType((*$1).c_str());
        driver.lefrLayer.lefiLayer::addNumProp((*$1).c_str(), $2, temp, propTp);
      }
      delete $1; 
    }

current_density_pwl_list :
  current_density_pwl
    { }
  | current_density_pwl_list current_density_pwl
    { }

current_density_pwl: '(' NUMBER NUMBER ')'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setCurrentPoint($2, $3); }

cap_points :
  cap_point
  | cap_points cap_point
  ;

cap_point: '(' NUMBER NUMBER ')'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setCapacitancePoint($2, $3); }

res_points :
  res_point
  | res_points res_point
    { }

res_point: '(' NUMBER NUMBER ')'
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::setResistancePoint($2, $3); }

layer_type:
  K_ROUTING       {$$ = new std::string ("ROUTING"); driver.layerRout = 1;}
  | K_CUT         {$$ = new std::string ("CUT"); driver.layerCut = 1;}
  | K_OVERLAP     {$$ = new std::string ("OVERLAP"); driver.layerMastOver = 1;}
  | K_MASTERSLICE {$$ = new std::string ("MASTERSLICE"); driver.layerMastOver = 1;}
  | K_VIRTUAL     {$$ = new std::string ("VIRTUAL");}
  | K_IMPLANT     {$$ = new std::string ("IMPLANT");}

layer_direction:
  K_HORIZONTAL      {$$ = new std::string ("HORIZONTAL");}
  |  K_VERTICAL     {$$ = new std::string ("VERTICAL");}
  |  K_DIAG45       {$$ = new std::string ("DIAG45");}
  |  K_DIAG135      {$$ = new std::string ("DIAG135");}

layer_minen_width:
  | K_WIDTH NUMBER
    {
    if (/*driver.lefrLayerCbk*/ 1)
       driver.lefrLayer.lefiLayer::addMinenclosedareaWidth($2);
    }

layer_oxide:
  K_OXIDE1
    {
    if (/*driver.lefrLayerCbk*/ 1)
       driver.lefrLayer.lefiLayer::addAntennaModel(1);
    }
  | K_OXIDE2
    {
    if (/*driver.lefrLayerCbk*/ 1)
       driver.lefrLayer.lefiLayer::addAntennaModel(2);
    }
  | K_OXIDE3
    {
    if (/*driver.lefrLayerCbk*/ 1)
       driver.lefrLayer.lefiLayer::addAntennaModel(3);
    }
  | K_OXIDE4
    {
    if (/*driver.lefrLayerCbk*/ 1)
       driver.lefrLayer.lefiLayer::addAntennaModel(4);
    }

layer_sp_parallel_widths: /* empty */
    { }
  | layer_sp_parallel_widths layer_sp_parallel_width  /* Use left recursions */
/* WANDA
  | layer_sp_parallel_width layer_sp_parallel_widths
*/
    { }

layer_sp_parallel_width: K_WIDTH NUMBER
    { 
      if (/*driver.lefrLayerCbk*/ 1) {
         driver.lefrLayer.lefiLayer::addSpParallelWidth($2);
      }
    }
    number_list
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addSpParallelWidthSpacing(); }
 
layer_sp_TwoWidths: /* empty */              // 5.7
    { }
  | layer_sp_TwoWidth layer_sp_TwoWidths
    { }

/*
layer_sp_TwoWidth: K_WIDTH NUMBER layer_sp_TwoWidthsPRL number_list   5.7
    { 
      if (driver.lefrLayerCbk) {
         driver.lefrLayer.lefiLayer::addSpTwoWidths($2, $3);
      }
    }
*/
/*
WANDA force at list one spacing value after layer_sp_TwoWidthsPRL
*/
layer_sp_TwoWidth: K_WIDTH NUMBER layer_sp_TwoWidthsPRL NUMBER
    {
       if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addNumber($4);
    }
    number_list 
    {
      if (/*driver.lefrLayerCbk*/ 1)
         driver.lefrLayer.lefiLayer::addSpTwoWidths($2, $3);
    }

layer_sp_TwoWidthsPRL:                       // 5.7
    { $$ = -1; }           /* cannot use 0, since PRL number can be 0 */
  | K_PRL NUMBER
    { $$ = $2; }
 
layer_sp_influence_widths: /* empty */
    { }
  | layer_sp_influence_widths layer_sp_influence_width
    { }

layer_sp_influence_width: K_WIDTH NUMBER K_WITHIN NUMBER K_SPACING NUMBER
    { if (/*driver.lefrLayerCbk*/ 1) driver.lefrLayer.lefiLayer::addSpInfluence($2, $4, $6); }

maxstack_via: K_MAXVIASTACK NUMBER ';'
    {
      if (!driver.lefrHasLayer) {  /* 5.5 */
        if (/*lefrMaxStackViaCbk*/ 1) { /* write error only if cbk is set */
           if (driver.maxStackViaWarnings++ < driver.lefrMaxStackViaWarnings) {
             driver.lefError(1602, "MAXVIASTACK statement has to be defined after the LAYER statement.");
             /*CHKERR();*/
           }
        }
      } else if (driver.lefrHasMaxVS) {
        if (/*lefrMaxStackViaCbk*/ 1) { /* write error only if cbk is set */
           if (driver.maxStackViaWarnings++ < driver.lefrMaxStackViaWarnings) {
             driver.lefError(1603, "A MAXVIASTACK has already defined.\nOnly one MAXVIASTACK is allowed per lef file.");
             /*CHKERR();*/
           }
        }
      } else {
        if (/*lefrMaxStackViaCbk*/ 1) {
           driver.lefrMaxStackVia.lefiMaxStackVia::setMaxStackVia((int)$2);
           driver.lefrMaxStackViaCbk( driver.lefrMaxStackVia);
        }
      }
      if (driver.versionNum < 5.5) {
        if (/*lefrMaxStackViaCbk*/ 1) { /* write error only if cbk is set */
           if (driver.maxStackViaWarnings++ < driver.lefrMaxStackViaWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                "MAXVIASTACK statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1604, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      driver.lefrHasMaxVS = 1;
    }
  | K_MAXVIASTACK NUMBER K_RANGE {driver.lefDumbMode = 2; driver.lefNoNum= 2;}
    STRING STRING ';'
    {
      if (!driver.lefrHasLayer) {  /* 5.5 */
        if (/*lefrMaxStackViaCbk*/ 1) { /* write error only if cbk is set */
           if (driver.maxStackViaWarnings++ < driver.lefrMaxStackViaWarnings) {
              driver.lefError(1602, "MAXVIASTACK statement has to be defined after the LAYER statement.");
              /*CHKERR();*/
           }
        }
      } else if (driver.lefrHasMaxVS) {
        if (/*lefrMaxStackViaCbk*/ 1) { /* write error only if cbk is set */
           if (driver.maxStackViaWarnings++ < driver.lefrMaxStackViaWarnings) {
             driver.lefError(1603, "A MAXVIASTACK has already defined.\nOnly one MAXVIASTACK is allowed per lef file.");
             /*CHKERR();*/
           }
        }
      } else {
        if (/*lefrMaxStackViaCbk*/ 1) {
           driver.lefrMaxStackVia.lefiMaxStackVia::setMaxStackVia((int)$2);
           driver.lefrMaxStackVia.lefiMaxStackVia::setMaxStackViaRange((*$5).c_str(), (*$6).c_str());
           driver.lefrMaxStackViaCbk( driver.lefrMaxStackVia);
        }
      }
      driver.lefrHasMaxVS = 1;
      delete $5; delete $6;
    }

via: start_via  { driver.hasViaRule_layer = 0; } via_option end_via
    { 
      if (/*lefrViaCbk*/ 1)
        driver.lefrViaCbk( driver.lefrVia);
    }

via_keyword : K_VIA                 /*needed to have a VIA named via*/
     { driver.lefDumbMode = 1; driver.lefNoNum = 1; }

start_via: via_keyword STRING 
    {
      /* 0 is nodefault */
      if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setName((*$2).c_str(), 0);
      driver.viaLayer = 0;
      driver.numVia++;
      //strcpy(driver.viaName, $2);
      driver.viaName = *$2;
      delete $2;
    }
  | via_keyword STRING K_DEFAULT
    {
      /* 1 is default */
      if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setName((*$2).c_str(), 1);
      driver.viaLayer = 0;
      //strcpy(driver.viaName, $2);
      driver.viaName = *$2;
      delete $2;
    }
  | via_keyword STRING K_GENERATED
    {
      /* 2 is generated */
      if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setName((*$2).c_str(), 2);
      driver.viaLayer = 0;
      //strcpy(driver.viaName, $2);
      driver.viaName = *$2;
      delete $2;
    }

via_viarule: K_VIARULE {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
  K_CUTSIZE NUMBER NUMBER ';'
  K_LAYERS {driver.lefDumbMode = 3; driver.lefNoNum = 1; } STRING STRING STRING ';'
  K_CUTSPACING NUMBER NUMBER ';'
  K_ENCLOSURE NUMBER NUMBER NUMBER NUMBER ';'
    {
       if (driver.versionNum < 5.6) {
         if (/*lefrViaCbk*/ 1) { /* write error only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                "VIARULE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1605, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
            }
         }
       }  else
          if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setViaRule((*$3).c_str(), $6, $7, (*$11).c_str(), (*$12).c_str(), (*$13).c_str(),
                          $16, $17, $20, $21, $22, $23);
       driver.viaLayer++;
       driver.hasViaRule_layer = 1;
       delete $3; delete $11; delete $12; delete $13;
    }
  via_viarule_options
  ;

via_viarule_options: /* empty */
  | via_viarule_options via_viarule_option
  ;

via_viarule_option: K_ROWCOL NUMBER NUMBER ';'
    {
       if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setRowCol((int)$2, (int)$3);
    }
  | K_ORIGIN NUMBER NUMBER ';'
    {
       if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setOrigin($2, $3);
    }
  | K_OFFSET NUMBER NUMBER NUMBER NUMBER ';'
    {
       if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setOffset($2, $3, $4, $5);
    }
  | K_PATTERN {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    {
       if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setPattern((*$3).c_str());
       delete $3;
    }
  ;

via_option: via_viarule
  | via_other_options

via_other_options: via_other_option
  via_more_options

via_more_options: /* empty */
  | via_more_options via_other_option
  ;

via_other_option:
  via_foreign 
    { }
  | via_layer_rule 
    { }
  | K_RESISTANCE NUMBER ';'
    { if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setResistance($2); }
  | K_PROPERTY { driver.lefDumbMode = 1000000; driver.lefRealNum = 1; driver.lefInProp = 1; } via_prop_list ';'
    { driver.lefDumbMode = 0;
      driver.lefRealNum = 0;
      driver.lefInProp = 0;
    }
  | K_TOPOFSTACKONLY
    { 
      if (driver.versionNum < 5.6) {
        if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setTopOfStack();
      } else
        if (/*lefrViaCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.viaWarnings++ < driver.lefrViaWarnings)
              driver.lefWarning(2019, "TOPOFSTACKONLY statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later");
    }

via_prop_list:
  via_name_value_pair
  | via_prop_list via_name_value_pair
  ;

via_name_value_pair:
  STRING NUMBER
    { 
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (/*lefrViaCbk*/ 1) {
         char propTp;
         propTp = driver.lefrViaProp.lefiPropType::propType((*$1).c_str());
         driver.lefrVia.lefiVia::addNumProp((*$1).c_str(), $2, temp, propTp);
      }
     delete $1;
    }
  | STRING QSTRING
    {
      if (/*lefrViaCbk*/ 1) {
         char propTp;
         propTp = driver.lefrViaProp.lefiPropType::propType((*$1).c_str());
         driver.lefrVia.lefiVia::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
     delete $1; delete $2;
    }
  | STRING STRING
    {
      if (/*lefrViaCbk*/ 1) {
         char propTp;
         propTp = driver.lefrViaProp.lefiPropType::propType((*$1).c_str());
         driver.lefrVia.lefiVia::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
     delete $1; delete $2;
    }

via_foreign:
  start_foreign ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setForeign((*$1).c_str(), 0, 0.0, 0.0, -1);
      } else
        if (/*lefrViaCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.viaWarnings++ < driver.lefrViaWarnings)
             driver.lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
        delete $1;
    }
  | start_foreign pt ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setForeign((*$1).c_str(), 1, $2->x, $2->y, -1);
      } else
        if (/*lefrViaCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.viaWarnings++ < driver.lefrViaWarnings)
             driver.lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
     delete $1; delete $2;
    }
  | start_foreign pt orientation ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setForeign((*$1).c_str(), 1, $2->x, $2->y, $3);
      } else
        if (/*lefrViaCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.viaWarnings++ < driver.lefrViaWarnings)
             driver.lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
     delete $1; delete $2;
    }
  | start_foreign orientation ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::setForeign((*$1).c_str(), 0, 0.0, 0.0, $2);
      } else
        if (/*lefrViaCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.viaWarnings++ < driver.lefrViaWarnings)
             driver.lefWarning(2020, "FOREIGN statement in VIA is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
     delete $1;
    }

start_foreign:	K_FOREIGN {driver.lefDumbMode = 1; driver.lefNoNum= 1;} STRING
    { $$ = $3; }

orientation:
  K_N         {$$ = 0;}
  | K_W       {$$ = 1;}
  | K_S       {$$ = 2;}
  | K_E       {$$ = 3;}
  | K_FN      {$$ = 4;}
  | K_FW      {$$ = 5;}
  | K_FS      {$$ = 6;}
  | K_FE      {$$ = 7;}
  | K_R0      {$$ = 0;}
  | K_R90     {$$ = 1;}
  | K_R180    {$$ = 2;}
  | K_R270    {$$ = 3;}
  | K_MY      {$$ = 4;}
  | K_MYR90   {$$ = 5;}
  | K_MX      {$$ = 6;}
  | K_MXR90   {$$ = 7;}

via_layer_rule: via_layer via_geometries
    { }

via_layer: K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    {
      if (/*lefrViaCbk*/ 1) driver.lefrVia.lefiVia::addLayer((*$3).c_str());
      driver.viaLayer++;
      driver.hasViaRule_layer = 1;
      delete $3;
    }

via_geometries:
  /* empty */
  | via_geometries via_geometry 
  ;

via_geometry:
  K_RECT pt pt ';'
    { if (/*lefrViaCbk*/ 1)
        driver.lefrVia.lefiVia::addRectToLayer($2->x, $2->y, $3->x, $3->y); 
    delete $2; delete $3;
    }
  | K_POLYGON                                               // 5.6
    {
      driver.lefrGeometriesPtr = (lefiGeometries*)lefMalloc(sizeof(lefiGeometries));
      driver.lefrGeometriesPtr->lefiGeometries::Init();
      driver.lefrDoGeometries = 1;
    }
    firstPt nextPt nextPt otherPts ';'
    { 
      if (/*lefrViaCbk*/ 1) {
        driver.lefrGeometriesPtr->lefiGeometries::addPolygon();
        driver.lefrVia.lefiVia::addPolyToLayer(driver.lefrGeometriesPtr);   // 5.6
      }
      driver.lefrGeometriesPtr->lefiGeometries::clearPolyItems(); // free items fields
      lefFree((char*)(driver.lefrGeometriesPtr)); // Don't need anymore, poly data has
      driver.lefrDoGeometries = 0;                // copied
    }

end_via: K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING 
    { 
      // 10/17/2001 - Wanda da Rosa, PCR 404149
      //              Error if no layer in via
      if (!driver.viaLayer) {
         if (/*lefrViaCbk*/ 1) {  /* write error only if cbk is set */
            if (driver.viaWarnings++ < driver.lefrViaWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                "A LAYER statement is missing in the VIA %s.\nAt least one LAYERis required per VIA statement.", (*$3).c_str());
              driver.lefError(1606, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
            }
         }
      }
      if (driver.viaName != *$3) {
         if (/*lefrViaCbk*/ 1) { /* write error only if cbk is set */
            if (driver.viaWarnings++ < driver.lefrViaWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                "END VIA name %s is different from the VIA name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.viaName.c_str());
              driver.lefError(1607, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
            } 
         } 
      } 
      delete $3;
    }

viarule_keyword : K_VIARULE { driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
    { 
      if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setName((*$3).c_str());
      driver.viaRuleLayer = 0;
      //strcpy(driver.viaRuleName, $3);
      driver.viaRuleName = (*$3);
      driver.isGenerate = 0;
      delete $3;
    }

viarule:
  viarule_keyword viarule_layer_list via_names opt_viarule_props end_viarule
    {
      // 10/17/2001 - Wanda da Rosa, PCR 404163
      //              Error if layer number is not equal 2.
      // 11/14/2001 - Wanda da Rosa,
      //              Commented out for pcr 411781
      //if (driver.viaRuleLayer != 2) {
         //driver.lefError("VIARULE requires two layers");
         ///*CHKERR();*/
      //}
      if (driver.viaRuleLayer == 0 || driver.viaRuleLayer > 2) {
         if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.lefError(1608, "A VIARULE statement requires two layers.");
              /*CHKERR();*/
            }
         }
      }
      if (/*driver.lefrViaRuleCbk*/ 1)
        driver.lefrViaRuleCbk( driver.lefrViaRule);
      // 2/19/2004 - reset the ENCLOSURE overhang values which may be
      // set by the old syntax OVERHANG -- Not necessary, but just incase
      if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::clearLayerOverhang();
    }

viarule_generate:
  viarule_keyword K_GENERATE viarule_generate_default
    {
      driver.isGenerate = 1;
    }
  viarule_layer_list opt_viarule_props end_viarule
    {
      // 10/17/2001 - Wanda da Rosa, PCR 404181
      //              Error if layer number is not equal 3.
      // 11/14/2001 - Wanda da Rosa,
      //              Commented out for pcr 411781
      //if (driver.viaRuleLayer != 3) {
         //driver.lefError("VIARULE requires three layers");
         ///*CHKERR();*/
      //}
      if (driver.viaRuleLayer == 0) {
         if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.lefError(1609, "A VIARULE GENERATE requires three layers.");
              /*CHKERR();*/
            }
         }
      } else if ((driver.viaRuleLayer < 3) && (driver.versionNum >= 5.6)) {
         if (/*driver.lefrViaRuleCbk*/ 1)  /* write warning only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings)
              driver.lefWarning(2021, "turn-via is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else {
         if (/*driver.lefrViaRuleCbk*/ 1) {
            driver.lefrViaRule.lefiViaRule::setGenerate();
            driver.lefrViaRuleCbk( driver.lefrViaRule);
         }
      }
      // 2/19/2004 - reset the ENCLOSURE overhang values which may be
      // set by the old syntax OVERHANG
      if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::clearLayerOverhang();
    }

viarule_generate_default:  /* optional */
  | K_DEFAULT   // 5.6 syntax
    {
      if (driver.versionNum < 5.6) {
         if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                "DEFAULT statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1605, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
            }
         }
      } else
        if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setDefault();
    }
 
viarule_layer_list :
  viarule_layer
  | viarule_layer_list viarule_layer
  ;

opt_viarule_props:
  /* empty */
  | viarule_props
  ;

viarule_props:
  viarule_prop
  | viarule_props viarule_prop
  ;

viarule_prop: K_PROPERTY { driver.lefDumbMode = 10000000; driver.lefRealNum = 1; driver.lefInProp = 1; } viarule_prop_list ';'
    { driver.lefDumbMode = 0;
      driver.lefRealNum = 0;
      driver.lefInProp = 0;
    }

viarule_prop_list:
  viarule_prop
  | viarule_prop_list viarule_prop
  ;

viarule_prop:
  STRING STRING
    {
      if (/*driver.lefrViaRuleCbk*/ 1) {
         char propTp;
         propTp = driver.lefrViaRuleProp.lefiPropType::propType((*$1).c_str());
         driver.lefrViaRule.lefiViaRule::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING QSTRING
    {
      if (/*driver.lefrViaRuleCbk*/ 1) {
         char propTp;
         propTp = driver.lefrViaRuleProp.lefiPropType::propType((*$1).c_str());
         driver.lefrViaRule.lefiViaRule::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING NUMBER
    {
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (/*driver.lefrViaRuleCbk*/ 1) {
         char propTp;
         propTp = driver.lefrViaRuleProp.lefiPropType::propType((*$1).c_str());
         driver.lefrViaRule.lefiViaRule::addNumProp((*$1).c_str(), $2, temp, propTp);
      }
      delete $1;
    }

viarule_layer: viarule_layer_name viarule_layer_options
    {
      // 10/18/2001 - Wanda da Rosa PCR 404181
      //              Make sure the 1st 2 layers in viarule has direction
      // 04/28/2004 - PCR 704072 - DIRECTION in viarule generate is
      //              obsolete in 5.6
      if (driver.versionNum >= 5.6) {
         if (driver.viaRuleLayer < 2 && !driver.viaRuleHasDir && !driver.viaRuleHasEnc &&
             !driver.isGenerate) {
            if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
               if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
                  driver.lefError(1606, "VIARULE statement in a layer, requires a DIRECTION construct statement.");
                  /*CHKERR();*/ 
               }
            }
         }
      } else {
         if (driver.viaRuleLayer < 2 && !driver.viaRuleHasDir && !driver.viaRuleHasEnc &&
             driver.isGenerate) {
            if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
               if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
                  driver.lefError(1606, "VIARULE statement in a layer, requires a DIRECTION construct statement.");
                  /*CHKERR();*/ 
               }
            }
         }
      }
      driver.viaRuleLayer++;
    }
  ;

via_names:
  /* empty */
  | via_names via_name
  ;

via_name: via_keyword STRING ';'
    { if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::addViaName((*$2).c_str()); delete $2;}

viarule_layer_name: K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    { if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setLayer((*$3).c_str());
      driver.viaRuleHasDir = 0;
      driver.viaRuleHasEnc = 0;
      delete $3;
    }

viarule_layer_options:
  /* empty */
  | viarule_layer_options viarule_layer_option
  ;

viarule_layer_option:
  K_DIRECTION K_HORIZONTAL ';'
    {
      if (driver.viaRuleHasEnc) {
        if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.lefError(1607, "An ENCLOSRE statement has already defined in the layer.\nOnly either DIRECTION or ENCLOSURE can be specified in a layer.");
              /*CHKERR();*/
           }
        }
      } else {
        if ((driver.versionNum < 5.6) || (!driver.isGenerate)) {
          if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setHorizontal();
        } else
          if (/*driver.lefrViaRuleCbk*/ 1)  /* write warning only if cbk is set */
             if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings)
               driver.lefWarning(2022, "DIRECTION statement in VIARULE is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      }
      driver.viaRuleHasDir = 1;
    }
  | K_DIRECTION K_VERTICAL ';'
    { 
      if (driver.viaRuleHasEnc) {
        if (/*driver.lefrViaRuleCbk*/ 1) { /* write error only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.lefError(1607, "An ENCLOSRE statement has already defined in the layer.\nOnly either DIRECTION or ENCLOSURE can be specified in a layer.");
              /*CHKERR();*/
           }
        }
      } else {
        if ((driver.versionNum < 5.6) || (!driver.isGenerate)) {
          if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setVertical();
        } else
          if (/*driver.lefrViaRuleCbk*/ 1) /* write warning only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings)
              driver.lefWarning(2022, "DIRECTION statement in VIARULE is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      }
      driver.viaRuleHasDir = 1;
    }
  | K_ENCLOSURE NUMBER NUMBER ';'    /* 5.5 */
    {
      if (driver.versionNum < 5.5) {
         if (/*driver.lefrViaRuleCbk*/ 1) { /* write error only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                "ENCLOSURE statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1608, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
         }
      }
      // 2/19/2004 - Enforced the rule that ENCLOSURE can only be defined
      // in VIARULE GENERATE
      if (!driver.isGenerate) {
         if (/*driver.lefrViaRuleCbk*/ 1) { /* write error only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.lefError(1614, "An ENCLOSURE statement is defined in a VIARULE statement only.\nOVERHANG statement can only be defined in VIARULE GENERATE.");
              /*CHKERR();*/
           }
         }
      }
      if (driver.viaRuleHasDir) {
         if (/*driver.lefrViaRuleCbk*/ 1) { /* write error only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.lefError(1609, "A DIRECTION statement has already defined in the layer.\nOnly either DIRECTION or ENCLOSURE can be specified in a layer.");
              /*CHKERR();*/
           }
         }
      } else {
         if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setEnclosure($2, $3);
      }
      driver.viaRuleHasEnc = 1;
    }
  | K_WIDTH NUMBER K_TO NUMBER ';'
    { if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setWidth($2,$4); }
  | K_RECT pt pt ';'
    { if (/*driver.lefrViaRuleCbk*/ 1)
	driver.lefrViaRule.lefiViaRule::setRect($2->x, $2->y, $3->x, $3->y); 
    delete $2; delete $3;
    }
  | K_SPACING NUMBER K_BY NUMBER ';'
    { if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setSpacing($2,$4); }
  | K_RESISTANCE NUMBER ';'
    { if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setResistance($2); }
  | K_OVERHANG NUMBER ';'
    {
      if (!driver.viaRuleHasDir) {
         if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
               driver.lefError(1610, "An OVERHANG statement is defined, but the required DIRECTION statement is not yet defined.\nUpdate the LEF file to define the DIRECTION statement before the OVERHANG.");
               /*CHKERR();*/
            }
         }
      }
      // 2/19/2004 - Enforced the rule that OVERHANG can only be defined
      // in VIARULE GENERATE after 5.3
      if ((driver.versionNum > 5.3) && (!driver.isGenerate)) {
         if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
               driver.lefError(1611, "An OVERHANG statement is defined in a VIARULE statement only.\nOVERHANG statement can only be defined in VIARULE GENERATE.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setOverhang($2);
      } else {
        if (/*driver.lefrViaRuleCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings)
              driver.lefWarning(2023, "OVERHANG statement will be translated into similar ENCLOSURE rule");
        // In 5.6 & later, set it to either ENCLOSURE overhang1 or overhang2
        if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setOverhangToEnclosure($2);
      }
    }
  | K_METALOVERHANG NUMBER ';'
    {
      // 2/19/2004 - Enforced the rule that METALOVERHANG can only be defined
      // in VIARULE GENERATE
      if ((driver.versionNum > 5.3) && (!driver.isGenerate)) {
         if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
            if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
               driver.lefError(1612, "An METALOVERHANG statement is defined in a VIARULE statement only.\nOVERHANG statement can only be defined in VIARULE GENERATE.");
               /*CHKERR();*/
            }
         }
      }
      if (driver.versionNum < 5.6) {
        if (!driver.viaRuleHasDir) {
           if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
             if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
                driver.lefError(1613, "An METALOVERHANG statement is defined, but the required DIRECTION statement is not yet defined.\nUpdate the LEF file to define the DIRECTION statement before the OVERHANG.");
                /*CHKERR();*/
             } 
           }
        }
        if (/*driver.lefrViaRuleCbk*/ 1) driver.lefrViaRule.lefiViaRule::setMetalOverhang($2);
      } else
        if (/*driver.lefrViaRuleCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings)
             driver.lefWarning(2024, "METALOVERHANG statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

end_viarule: K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1;}  STRING 
    {
      if (driver.viaRuleName != *$3) {
        if (/*driver.lefrViaRuleCbk*/ 1) {  /* write error only if cbk is set */
           if (driver.viaRuleWarnings++ < driver.lefrViaRuleWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "END VIARULE name %s is different from the VIARULE name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.viaRuleName.c_str());
              driver.lefError(1615, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           } 
        } 
      } 
      delete $3;
    }

spacing_rule: start_spacing spacings end_spacing 
    { }

start_spacing: K_SPACING
    {
      driver.hasSamenet = 0;
      if ((driver.versionNum < 5.6) || (!driver.ndRule)) {
        /* if 5.6 and in nondefaultrule, it should not get in here, */
        /* it should go to the else statement to write out a warning */
        /* if 5.6, not in nondefaultrule, it will get in here */
        /* if 5.5 and earlier in nondefaultrule is ok to get in here */
        if (driver.versionNum >= 5.7) { /* will get to this if statement if */ 
                           /* driver.versionNum is 5.6 and higher but driver.ndRule = 0 */
           if (driver.spacingWarnings == 0) {  /* only print once */
              driver.lefWarning(2077, "A SPACING SAMENET section is defined but it is not legal in a LEF 5.7 version file.\nIt will be ignored which will probably cause real DRC violations to be ignored, and may\ncause false DRC violations to occur.\n\nTo avoid this warning, and correctly handle these DRC rules, you should modify your\nLEF to use the appropriate SAMENET keywords as described in the LEF/DEF 5.7\nmanual under the SPACING statements in the LAYER (Routing) and LAYER (Cut)\nsections listed in the LEF Table of Contents.");
              driver.spacingWarnings++;
           }
        } else if (/*driver.lefrSpacingBeginCbk*/ 1)
	  driver.lefrSpacingBeginCbk( 0);
      } else
        if (/*driver.lefrSpacingBeginCbk*/ 1)  /* write warning only if cbk is set */
           if (driver.spacingWarnings++ < driver.lefrSpacingWarnings)
             driver.lefWarning(2025, "SAMENET statement in NONDEFAULTRULE is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

end_spacing: K_END K_SPACING
    {
      if ((driver.versionNum < 5.6) || (!driver.ndRule)) {
        if ((driver.versionNum <= 5.4) && (!driver.hasSamenet)) {
           driver.lefError(1616, "SAMENET statement is required inside SPACING for any lef file with version 5.4 and earlier, but is not defined in the parsed lef file.");
           /*CHKERR();*/
        } else if (driver.versionNum < 5.7) { /* obsolete in 5.7 and later */
           if (/*driver.lefrSpacingEndCbk*/ 1)
             driver.lefrSpacingEndCbk( 0);
        }
      }
    }

spacings:
  /* empty */
  | spacings spacing
  ;

spacing:  samenet_keyword STRING STRING NUMBER ';'
    {
      if ((driver.versionNum < 5.6) || (!driver.ndRule)) {
        if (driver.versionNum < 5.7) {
          if (/*driver.lefrSpacingCbk*/ 1) {
            driver.lefrSpacing.lefiSpacing::set((*$2).c_str(), (*$3).c_str(), $4, 0);
            driver.lefrSpacingCbk( driver.lefrSpacing);
          }
        }
      }
      delete $2; delete $3;
    }
  | samenet_keyword STRING STRING NUMBER K_STACK ';'
    {
      if ((driver.versionNum < 5.6) || (!driver.ndRule)) {
        if (driver.versionNum < 5.7) {
          if (/*driver.lefrSpacingCbk*/ 1) {
	    driver.lefrSpacing.lefiSpacing::set((*$2).c_str(), (*$3).c_str(), $4, 1);
	    driver.lefrSpacingCbk( driver.lefrSpacing);
          }
        }
      }
      delete $2; delete $3;
    }

samenet_keyword: K_SAMENET
    /* must be followed by two names */
    { driver.lefDumbMode = 2; driver.lefNoNum = 2; driver.hasSamenet = 1; }

irdrop: start_irdrop ir_tables end_irdrop
    { }

start_irdrop: K_IRDROP
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrIRDropBeginCbk*/ 1) 
	  driver.lefrIRDropBeginCbk( 0);
      } else
        if (/*driver.lefrIRDropBeginCbk*/ 1) /* write warning only if cbk is set */
          if (driver.iRDropWarnings++ < driver.lefrIRDropWarnings)
            driver.lefWarning(2026, "IRDROP statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

end_irdrop: K_END K_IRDROP
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrIRDropEndCbk*/ 1)
	  driver.lefrIRDropEndCbk( 0);
      }
    }
      

ir_tables:
  /* empty */
  | ir_tables ir_table
  ;

ir_table: ir_tablename ir_table_values ';'
    { 
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrIRDropCbk*/ 1)
          driver.lefrIRDropCbk( driver.lefrIRDrop);
      }
    }

ir_table_values:
  /* empty */
  | ir_table_values ir_table_value 
  ;

ir_table_value: NUMBER NUMBER 
  { if (/*driver.lefrIRDropCbk*/ 1) driver.lefrIRDrop.lefiIRDrop::setValues($1, $2); }

ir_tablename: K_TABLE STRING
  { if (/*driver.lefrIRDropCbk*/ 1) driver.lefrIRDrop.lefiIRDrop::setTableName((*$2).c_str()); delete $2; }

minfeature: K_MINFEATURE NUMBER NUMBER ';'
  {
    driver.hasMinfeature = 1;
    if (driver.versionNum < 5.4) {
       if (/*driver.lefrMinFeatureCbk*/ 1) {
         driver.lefrMinFeature.lefiMinFeature::set($2, $3);
         driver.lefrMinFeatureCbk( driver.lefrMinFeature);
       }
    } else
       if (/*driver.lefrMinFeatureCbk*/ 1) /* write warning only if cbk is set */
          if (driver.minFeatureWarnings++ < driver.lefrMinFeatureWarnings)
            driver.lefWarning(2027, "MINFEATURE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }

dielectric: K_DIELECTRIC NUMBER ';'
  {
    if (driver.versionNum < 5.4) {
       if (/*lefrDielectricCbk*/ 1)
         driver.lefrDielectricCbk( $2);
    } else
       if (/*lefrDielectricCbk*/ 1) /* write warning only if cbk is set */
         if (driver.dielectricWarnings++ < driver.lefrDielectricWarnings)
           driver.lefWarning(2028, "DIELECTRIC statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }

nondefault_rule: K_NONDEFAULTRULE {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
  {
    driver.lefSetNonDefault((*$3).c_str());
    if (/*driver.lefrNonDefaultCbk*/ 1) driver.lefrNonDefault.lefiNonDefault::setName((*$3).c_str());
    driver.ndLayer = 0;
    driver.ndRule = 1;
    driver.numVia = 0;
    //strcpy(driver.nonDefaultRuleName, $3);
    driver.nonDefaultRuleName = *$3;
    delete $3;
  }
  nd_hardspacing
  nd_rules {driver.lefNdRule = 1;} end_nd_rule
  {
    // 10/18/2001 - Wanda da Rosa, PCR 404189
    //              At least 1 layer is required
    if ((!driver.ndLayer) && (!driver.lefrRelaxMode)) {
       if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
         if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
            driver.lefError(1617, "NONDEFAULTRULE statement requires at least one LAYER statement.");
            /*CHKERR();*/
         }
       }
    }
    if ((!driver.numVia) && (!driver.lefrRelaxMode) && (driver.versionNum < 5.6)) {
       // VIA is no longer a required statement in 5.6
       if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
         if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
            driver.lefError(1618, "NONDEFAULTRULE statement requires at least one VIA statement.");
            /*CHKERR();*/
         }
       }
    }
    if (/*driver.lefrNonDefaultCbk*/ 1) {
      driver.lefrNonDefault.lefiNonDefault::end();
      driver.lefrNonDefaultCbk( driver.lefrNonDefault);
    }
    driver.ndRule = 0;
    driver.lefDumbMode = 0;
    driver.lefUnsetNonDefault();
  }

end_nd_rule: K_END
    {
      if (!driver.nonDefaultRuleName.empty())
        driver.nonDefaultRuleName = "";
    }
  | K_END STRING
    {
      if (driver.nonDefaultRuleName != *$2) {
        if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
          if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
             driver.outMsg = (char*)lefMalloc(10000);
             sprintf (driver.outMsg,
                "END NONDEFAULTRULE name %s is different from the NONDEFAULTRULE name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$2).c_str(), driver.nonDefaultRuleName.c_str());
             driver.lefError(1619, driver.outMsg);
             lefFree(driver.outMsg);
             /*CHKERR();*/
          } 
        } 
      } 
      delete $2;
    }
  ;

nd_hardspacing:
  /* empty */
  | K_HARDSPACING ';'   /* HARDSPACING is optional in 5.6 */
    {
       if (driver.versionNum < 5.6) {
          if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "HARDSPACING statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1620, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
          }
       } else 
          if (/*driver.lefrNonDefaultCbk*/ 1)
             driver.lefrNonDefault.lefiNonDefault::setHardspacing();
    }
  ;

nd_rules: /* empty */
  | nd_rules nd_rule
  ;

nd_rule:
  nd_layer
  | via
  | spacing_rule
  | nd_prop
  | usevia
  | useviarule
  | mincuts
  ;

usevia: K_USEVIA STRING ';'
    {
       if (driver.versionNum < 5.6) {
          if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
             driver.outMsg = (char*)lefMalloc(10000);
             sprintf (driver.outMsg,
               "USEVIA statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
             driver.lefError(1621, driver.outMsg);
             lefFree(driver.outMsg);
             /*CHKERR();*/
          }
       } else {
          if (/*driver.lefrNonDefaultCbk*/ 1)
             driver.lefrNonDefault.lefiNonDefault::addUseVia((*$2).c_str());
       }
       delete $2;
    }

useviarule:  K_USEVIARULE STRING ';'
    {
       if (driver.versionNum < 5.6) {
          if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
             if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
                driver.outMsg = (char*)lefMalloc(10000);
                sprintf (driver.outMsg,
                  "USEVIARULE statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                driver.lefError(1622, driver.outMsg);
                lefFree(driver.outMsg);
                /*CHKERR();*/
             }
          }
       } else {
          if (/*driver.lefrNonDefaultCbk*/ 1)
             driver.lefrNonDefault.lefiNonDefault::addUseViaRule((*$2).c_str());
       }
       delete $2;
    }

mincuts: K_MINCUTS STRING NUMBER ';'
    {
       if (driver.versionNum < 5.6) {
          if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
             if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
                driver.outMsg = (char*)lefMalloc(10000);
                sprintf (driver.outMsg,
                  "MINCUTS statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                driver.lefError(1623, driver.outMsg);
                lefFree(driver.outMsg);
                /*CHKERR();*/
             }
          }
       } else {
          if (/*driver.lefrNonDefaultCbk*/ 1)
             driver.lefrNonDefault.lefiNonDefault::addMinCuts((*$2).c_str(), (int)$3);
       }
       delete $2;
    }

nd_prop: K_PROPERTY { driver.lefDumbMode = 10000000; driver.lefRealNum = 1; driver.lefInProp = 1; } nd_prop_list ';'
    { driver.lefDumbMode = 0;
      driver.lefRealNum = 0;
      driver.lefInProp = 0;
    }

nd_prop_list:
  nd_prop
  | nd_prop_list nd_prop
  ;

nd_prop:
  STRING STRING
    {
      if (/*driver.lefrNonDefaultCbk*/ 1) {
         char propTp;
         propTp = driver.lefrNondefProp.lefiPropType::propType((*$1).c_str());
         driver.lefrNonDefault.lefiNonDefault::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING QSTRING
    {
      if (/*driver.lefrNonDefaultCbk*/ 1) {
         char propTp;
         propTp = driver.lefrNondefProp.lefiPropType::propType((*$1).c_str());
         driver.lefrNonDefault.lefiNonDefault::addProp((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING NUMBER
    {
      if (/*driver.lefrNonDefaultCbk*/ 1) {
         char temp[32];
         char propTp;
         sprintf(temp, "%.11g", $2);
         propTp = driver.lefrNondefProp.lefiPropType::propType((*$1).c_str());
         driver.lefrNonDefault.lefiNonDefault::addNumProp((*$1).c_str(), $2, temp, propTp);
      }
      delete $1;
    }

nd_layer: K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
  {
    if (/*driver.lefrNonDefaultCbk*/ 1) driver.lefrNonDefault.lefiNonDefault::addLayer((*$3).c_str());
    driver.ndLayer++;
    //strcpy(driver.layerName, $3);
    driver.layerName = *$3;
    driver.ndLayerWidth = 0;
    driver.ndLayerSpace = 0;
    delete $3;
  }
  K_WIDTH NUMBER ';'
  { 
    driver.ndLayerWidth = 1;
    if (/*driver.lefrNonDefaultCbk*/ 1) driver.lefrNonDefault.lefiNonDefault::addWidth($6);
  }
  nd_layer_stmts K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
  {
    if (driver.layerName != *$12) {
      if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
         if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
            driver.outMsg = (char*)lefMalloc(10000);
            sprintf (driver.outMsg,
               "END LAYER name %s is different from the LAYER name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.layerName.c_str());
            driver.lefError(1624, driver.outMsg);
            lefFree(driver.outMsg);
            /*CHKERR();*/
         }
      }
    }
    if (!driver.ndLayerWidth) {
      if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
         if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
            driver.lefError(1625, "A WIDTH statement is required in the LAYER statement in NONDEFULTRULE.");
            /*CHKERR();*/
         }
      }
    }
    if (!driver.ndLayerSpace && driver.versionNum < 5.6) {   // 5.6, SPACING is optional
      if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
         if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
            driver.outMsg = (char*)lefMalloc(10000);
            sprintf (driver.outMsg,
               "A SPACING statement is required in the LAYER statement in NONDEFAULTRULE for lef file with version 5.5 and earlier.\nYour lef file is defined with version %g. Update your lef to add a LAYER statement and try again.",
                driver.versionNum);
            driver.lefError(1626, driver.outMsg);
            lefFree(driver.outMsg);
            /*CHKERR();*/
         }
      }
    }
    delete $3; delete $12;
  }
  ;

nd_layer_stmts:
  /* empty */
  | nd_layer_stmts nd_layer_stmt
  ;

nd_layer_stmt:
  K_SPACING NUMBER ';'
    {
      driver.ndLayerSpace = 1;
      if (/*driver.lefrNonDefaultCbk*/ 1) driver.lefrNonDefault.lefiNonDefault::addSpacing($2);
    }
  | K_WIREEXTENSION NUMBER ';'
    { if (/*driver.lefrNonDefaultCbk*/ 1)
         driver.lefrNonDefault.lefiNonDefault::addWireExtension($2); }
  | K_RESISTANCE K_RPERSQ NUMBER ';'
    {
      if (driver.ignoreVersion) {
         if (/*driver.lefrNonDefaultCbk*/ 1)
            driver.lefrNonDefault.lefiNonDefault::addResistance($3);
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "RESISTANCE RPERSQ statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1627, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.versionNum > 5.5) {  // obsolete in 5.6
         if (/*driver.lefrNonDefaultCbk*/ 1) /* write warning only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings)
              driver.lefWarning(2029, "RESISTANCE RPERSQ statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else if (/*driver.lefrNonDefaultCbk*/ 1)
         driver.lefrNonDefault.lefiNonDefault::addResistance($3);
    } 
 
  | K_CAPACITANCE K_CPERSQDIST NUMBER ';'
    {
      if (driver.ignoreVersion) {
         if (/*driver.lefrNonDefaultCbk*/ 1)
            driver.lefrNonDefault.lefiNonDefault::addCapacitance($3);
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "CAPACITANCE CPERSQDIST statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1628, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
            }
         }
      } else if (driver.versionNum > 5.5) { // obsolete in 5.6
         if (/*driver.lefrNonDefaultCbk*/ 1) /* write warning only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings)
              driver.lefWarning(2030, "CAPACITANCE CPERSQDIST statement is obsolete in version 5.6. and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else if (/*driver.lefrNonDefaultCbk*/ 1)
         driver.lefrNonDefault.lefiNonDefault::addCapacitance($3);
    }
  | K_EDGECAPACITANCE NUMBER ';'
    {
      if (driver.ignoreVersion) {
         if (/*driver.lefrNonDefaultCbk*/ 1)
            driver.lefrNonDefault.lefiNonDefault::addEdgeCap($2);
      } else if (driver.versionNum < 5.4) {
         if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "EDGECAPACITANCE statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1629, driver.outMsg);
               lefFree(driver.outMsg);
              /*CHKERR();*/
            }
         }
      } else if (driver.versionNum > 5.5) {  // obsolete in 5.6
         if (/*driver.lefrNonDefaultCbk*/ 1) /* write warning only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings)
              driver.lefWarning(2031, "EDGECAPACITANCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
      } else if (/*driver.lefrNonDefaultCbk*/ 1)
         driver.lefrNonDefault.lefiNonDefault::addEdgeCap($2);
    }
  | K_DIAGWIDTH NUMBER ';'
    {
      if (driver.versionNum < 5.6) {  // 5.6 syntax
         if (/*driver.lefrNonDefaultCbk*/ 1) { /* write error only if cbk is set */
            if (driver.nonDefaultWarnings++ < driver.lefrNonDefaultWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                 "DIAGWIDTH statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
               driver.lefError(1630, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/ 
            }
         }
      } else {
         if (/*driver.lefrNonDefaultCbk*/ 1)
            driver.lefrNonDefault.lefiNonDefault::addDiagWidth($2);
      }
    }

site: start_site site_options end_site
    { 
      if (/*driver.lefrSiteCbk*/ 1)
        driver.lefrSiteCbk( driver.lefrSite);
    }

start_site: K_SITE {driver.lefDumbMode = 1; driver.lefNoNum = 1;} GSTRING 
    { 
      if (/*driver.lefrSiteCbk*/ 1) driver.lefrSite.lefiSite::setName((*$3).c_str());
      //strcpy(driver.siteName, $3);
      driver.siteName = (*$3);
      driver.hasSiteClass = 0;
      driver.hasSiteSize = 0;
      driver.hasSite = 1;
      delete $3;
    }

end_site: K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1;} GSTRING
    {
      if (driver.siteName != *$3) {
        if (/*driver.lefrSiteCbk*/ 1) { /* write error only if cbk is set */
           if (driver.siteWarnings++ < driver.lefrSiteWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "END SITE name %s is different from the SITE name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.siteName.c_str());
              driver.lefError(1631, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           } 
        } 
      } else {
        if (/*driver.lefrSiteCbk*/ 1) { /* write error only if cbk is set */
          if (driver.hasSiteClass == 0) {
             driver.lefError(1632, "A CLASS statement is required in the SITE statement.");
             /*CHKERR();*/
          }
          if (driver.hasSiteSize == 0) {
             driver.lefError(1633, "A SIZE  statement is required in the SITE statement.");
             /*CHKERR();*/
          }
        }
      }
      delete $3;
    }

site_options:
  /* empty */
  | site_options site_option
  ;

site_option:
  K_SIZE NUMBER K_BY NUMBER ';' 
    {
/* Workaround for pcr 640902
*/
      if (/*driver.lefrSiteCbk*/ 1) driver.lefrSite.lefiSite::setSize($2,$4);
      driver.hasSiteSize = 1;
    }
  | site_symmetry_statement
    { }
  | site_class 
    { 
      if (/*driver.lefrSiteCbk*/ 1) driver.lefrSite.lefiSite::setClass((*$1).c_str());
      driver.hasSiteClass = 1;
      delete $1;
    }
  | site_rowpattern_statement
    { }

site_class:
  K_CLASS K_PAD ';' {$$ = new std::string ("PAD"); }
  | K_CLASS K_CORE ';'  {$$ = new std::string ("CORE"); }
  | K_CLASS K_VIRTUAL ';'  {$$ = new std::string ("VIRTUAL"); }

site_symmetry_statement: K_SYMMETRY site_symmetries ';'
    { }

site_symmetries:
  /* empty */
  | site_symmetries site_symmetry
  ;

site_symmetry:
  K_X 
    { if (/*driver.lefrSiteCbk*/ 1) driver.lefrSite.lefiSite::setXSymmetry(); }
  | K_Y 
    { if (/*driver.lefrSiteCbk*/ 1) driver.lefrSite.lefiSite::setYSymmetry(); }
  | K_R90
    { if (/*driver.lefrSiteCbk*/ 1) driver.lefrSite.lefiSite::set90Symmetry(); }

site_rowpattern_statement: K_ROWPATTERN {driver.lefDumbMode = 1; driver.lefNoNum = 1;}
    site_rowpatterns ';'
    { }

site_rowpatterns:
  /* empty */
  | site_rowpatterns site_rowpattern
  ;

site_rowpattern: STRING orientation {driver.lefDumbMode = 1; driver.lefNoNum = 1;}
    { if (/*driver.lefrSiteCbk*/ 1) driver.lefrSite.lefiSite::addRowPattern((*$1).c_str(), $2); delete $1;}

pt:
  NUMBER NUMBER
    { $$ = new lefPoint; $$->x = $1; $$->y = $2; }
  | '(' NUMBER NUMBER ')'
    { $$ = new lefPoint; $$->x = $2; $$->y = $3; }

macro: start_macro macro_options
    { 
      if (/*driver.lefrMacroCbk*/ 1)
        driver.lefrMacroCbk( driver.lefrMacro);
      driver.lefrDoSite = 0;
    }
    end_macro

start_macro: K_MACRO {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING 
    {
      driver.siteDef = 0;
      driver.symDef = 0;
      driver.sizeDef = 0; 
      driver.pinDef = 0; 
      driver.obsDef = 0; 
      driver.origDef = 0;
      driver.lefrMacro.lefiMacro::clear();      
      if (/*driver.lefrMacroBeginCbk || driver.lefrMacroCbk*/ 1) {
        // some reader may not have MacroBeginCB, but has MacroCB set
        driver.lefrMacro.lefiMacro::setName((*$3).c_str());
        driver.lefrMacroBeginCbk(*$3);
      }
      //strcpy(driver.macroName, $3);
      driver.macroName = (*$3);
      delete $3;
    }

end_macro: K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
    {
      if (driver.macroName != *$3) {
        if (/*driver.lefrMacroEndCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "END MACRO name %s is different from the MACRO name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.macroName.c_str());
              driver.lefError(1634, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           } 
        } 
      } 
      if (/*driver.lefrMacroEndCbk*/ 1)
        driver.lefrMacroEndCbk(*$3);
        delete $3;
    }

macro_options:
  /* empty */
  | macro_options macro_option   /* Use left recursions */
/* WANDA
  | macro_option macro_options 
*/
  ;

macro_option:
  macro_class 
  | macro_generator 
  | macro_generate 
  | macro_source
  | macro_symmetry_statement 
  | macro_origin 
      { }
  | macro_power 
      { }
  | macro_foreign
      { }
  | macro_eeq 
  | macro_leq 
  | macro_size 
      { }
  | macro_site 
      { }
  | macro_pin 
      { }
  | K_FUNCTION K_BUFFER ';'
      { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setBuffer(); }
  | K_FUNCTION K_INVERTER ';'
      { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setInverter(); }
  | macro_obs 
      { }
  | macro_density 
      { }
  | macro_clocktype 
      { }
  | timing
      { }
  | K_PROPERTY {driver.lefDumbMode = 1000000; driver.lefRealNum = 1; driver.lefInProp = 1; } macro_prop_list  ';'
      { driver.lefDumbMode = 0;
        driver.lefRealNum = 0;
        driver.lefInProp = 0;
      }

macro_prop_list:
  macro_name_value_pair
  | macro_prop_list macro_name_value_pair
  ;

macro_symmetry_statement: K_SYMMETRY macro_symmetries ';'
    {
      if (driver.siteDef) { /* SITE is defined before SYMMETRY */
          /* pcr 283846 suppress warning */
          if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
             if (driver.macroWarnings++ < driver.lefrMacroWarnings)
               driver.lefWarning(2032, "A SITE statement is defined before SYMMETRY statement.\nTo avoid this warning in the future, define SITE after SYMMETRY");
      }
      driver.symDef = 1;
    }

macro_symmetries:
  /* empty */
  | macro_symmetries macro_symmetry
  ;

macro_symmetry:
  K_X 
    { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setXSymmetry(); }
  | K_Y 
    { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setYSymmetry(); }
  | K_R90
    { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::set90Symmetry(); }

macro_name_value_pair:
  STRING NUMBER
    {
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (/*driver.lefrMacroCbk*/ 1) {
         char propTp;
         propTp = driver.lefrMacroProp.lefiPropType::propType((*$1).c_str());
         driver.lefrMacro.lefiMacro::setNumProperty((*$1).c_str(), $2, temp,  propTp);
      }
      delete $1;
    }
  | STRING QSTRING
    {
      if (/*driver.lefrMacroCbk*/ 1) {
         char propTp;
         propTp = driver.lefrMacroProp.lefiPropType::propType((*$1).c_str());
         driver.lefrMacro.lefiMacro::setProperty((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING STRING
    {
      if (/*driver.lefrMacroCbk*/ 1) {
         char propTp;
         propTp = driver.lefrMacroProp.lefiPropType::propType((*$1).c_str());
         driver.lefrMacro.lefiMacro::setProperty((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }

macro_class: K_CLASS class_type ';'
    {
       if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setClass((*$2).c_str());
       if (/*driver.lefrMacroClassTypeCbk*/ 1)
          driver.lefrMacroClassTypeCbk(*$2);
        delete $2;
    }

class_type:
  K_COVER {$$ = new std::string ("COVER"); }
  | K_COVER K_BUMP
    { $$ = new std::string ("COVER BUMP");
      if (driver.versionNum < 5.5) {
        if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              if (driver.lefrRelaxMode)
                 driver.lefWarning(2033, "The statement COVER BUMP is a LEF verion 5.5 syntax.\nYour LEF file is version 5.4 or earlier which is illegal but will be allowed\nbecause this application does not enforce strict version checking.\nOther tools that enforce strict checking will have a syntax error when reading this file.\nYou can change the VERSION statement in this LEF file to 5.5 or higher to stop this warning.");
              else {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "COVER BUMP statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                 driver.lefError(1635, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        }
      }
    }
  | K_RING    {$$ = new std::string ("RING"); }
  | K_BLOCK   {$$ = new std::string ("BLOCK"); }
  | K_BLOCK K_BLACKBOX 
    { $$ = new std::string ("BLOCK BLACKBOX");
      if (driver.versionNum < 5.5) {
        if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
             if (driver.lefrRelaxMode)
                driver.lefWarning(2034, "The statement BLOCK BLACKBOX is a LEF verion 5.5 syntax.\nYour LEF file is version 5.4 or earlier which is illegal but will be allowed\nbecause this application does not enforce strict version checking.\nOther tools that enforce strict checking will have a syntax error when reading this file.\nYou can change the VERSION statement in this LEF file to 5.5 or higher to stop this warning.");
              else {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "BLOCK BLACKBOX statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                 driver.lefError(1636, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        }
      }
    }
  | K_BLOCK K_SOFT
    {
      if (driver.ignoreVersion) {
        $$ = new std::string ("BLOCK SOFT");
      } else if (driver.versionNum < 5.6) {
        if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "BLOCK SOFT statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1637, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      else
        $$ = new std::string ("BLOCK SOFT");
    }
  | K_NONE    {$$ = new std::string ("NONE"); }
  | K_BUMP                         /* 5.7 */
      {
        if (driver.versionNum < 5.7) {
          driver.outMsg = (char*)lefMalloc(10000);
          sprintf(driver.outMsg,
            "BUMP is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
          driver.lefError(1698, driver.outMsg);
          lefFree(driver.outMsg);
          /*CHKERR();*/
      } else
        $$ = new std::string ("BUMP");
      }
  | K_PAD     {$$ = new std::string ("PAD"); } 
  | K_VIRTUAL {$$ = new std::string ("VIRTUAL"); }
  | K_PAD  pad_type 
      {  sprintf(driver.temp_name, "PAD %s", (*$2).c_str());
        $$ = new std::string (driver.temp_name); 
        if (driver.versionNum < 5.5) {
           if ("AREAIO" != *$2) {
             sprintf(driver.temp_name, "PAD %s", (*$2).c_str());
			 $$ = new std::string (driver.temp_name); 
           } else if (/*driver.lefrMacroCbk*/ 1) { 
             if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
               if (driver.lefrRelaxMode)
                  driver.lefWarning(2035, "The statement PAD AREAIO is a LEF verion 5.5 syntax.\nYour LEF file is version 5.4 or earlier which is illegal but will be allowed\nbecause this application does not enforce strict version checking.\nOther tools that enforce strict checking will have a syntax error when reading this file.\nYou can change the VERSION statement in this LEF file to 5.5 or higher to stop this warning.");
               else {
                  driver.outMsg = (char*)lefMalloc(10000);
                  sprintf (driver.outMsg,
                     "PAD AREAIO statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                  driver.lefError(1638, driver.outMsg);
                  lefFree(driver.outMsg);
                  /*CHKERR();*/
               }
            }
          }
        }
        delete $2;
      }
  | K_CORE    {$$ = new std::string ("CORE"); }
  | K_CORNER 
      {$$ = new std::string ("CORNER");
      /* This token is NOT in the spec but has shown up in 
       * some lef files.  This exception came from LEFOUT
       * in 'frameworks'
       */
      }
  | K_CORE core_type
      {sprintf(driver.temp_name, "CORE %s", (*$2).c_str()); delete $2;
      $$ = new std::string (driver.temp_name);} 
  | K_ENDCAP endcap_type
      {sprintf(driver.temp_name, "ENDCAP %s", (*$2).c_str()); delete $2;
      $$ = new std::string (driver.temp_name);} 

pad_type: /*      {$$ = (char*)"";}
  | K_INPUT 	{$$ = new std::string ("INPUT");} */
  K_INPUT 	{$$ = new std::string ("INPUT");}
  | K_OUTPUT	{$$ = new std::string ("OUTPUT");}
  | K_INOUT 	{$$ = new std::string ("INOUT");}
  | K_POWER 	{$$ = new std::string ("POWER");}
  | K_SPACER	{$$ = new std::string ("SPACER");}
  | K_AREAIO    {$$ = new std::string ("AREAIO");}

core_type:
  K_FEEDTHRU        {$$ = new std::string ("FEEDTHRU");}
  | K_TIEHIGH       {$$ = new std::string ("TIEHIGH");}
  | K_TIELOW        {$$ = new std::string ("TIELOW");}
  | K_SPACER
    { 
      if (driver.ignoreVersion) {
        $$ = new std::string ("SPACER");
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "SPACER statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1639, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      else
        $$ = new std::string ("SPACER");
    }
  | K_ANTENNACELL
    { 
      if (driver.ignoreVersion) {
        $$ = new std::string ("ANTENNACELL");
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNACELL statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1640, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      else
        $$ = new std::string ("ANTENNACELL");
    }
  | K_WELLTAP
    { 
      if (driver.ignoreVersion) {
        $$ = new std::string ("WELLTAP");
      } else if (driver.versionNum < 5.6) {
        if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "WELLTAP statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1641, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      else
        $$ = new std::string ("WELLTAP");
    }

endcap_type:
  K_PRE 	{$$ = new std::string ("PRE");}
  | K_POST 	{$$ = new std::string ("POST");}
  | K_TOPLEFT 	{$$ = new std::string ("TOPLEFT");}
  | K_TOPRIGHT 	{$$ = new std::string ("TOPRIGHT");}
  | K_BOTTOMLEFT 	{$$ = new std::string ("BOTTOMLEFT");}
  | K_BOTTOMRIGHT	{$$ = new std::string ("BOTTOMRIGHT");}

macro_generator: K_GENERATOR STRING ';'
    { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setGenerator((*$2).c_str()); delete $2;}

macro_generate: K_GENERATE STRING STRING ';'
    { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setGenerate((*$2).c_str(), (*$3).c_str());  delete $2; delete $3;}

macro_source:
  K_SOURCE K_USER ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setSource("USER");
      } else
        if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings)
             driver.lefWarning(2036, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | K_SOURCE K_GENERATE ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setSource("GENERATE");
      } else
        if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings)
             driver.lefWarning(2037, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }
  | K_SOURCE K_BLOCK ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setSource("BLOCK");
      } else
        if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings)
             driver.lefWarning(2037, "SOURCE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    }

macro_power: K_POWER NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setPower($2);
      } else
        if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings)
             driver.lefWarning(2038, "MACRO POWER statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

macro_origin: K_ORIGIN pt ';'
    { 
       if (driver.origDef) { /* Has multiple ORIGIN defined in a macro, stop parsing*/
          if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
             if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
                driver.lefError(1642, "ORIGIN statement has defined more than once in a MACRO statement.\nOnly one ORIGIN statement can be defined in a Macro.\nParser stops executions.");
               /*CHKERR();*/
             }
          }
       }
       driver.origDef = 1;
       if (driver.siteDef) { /* SITE is defined before ORIGIN */
          /* pcr 283846 suppress warning */
          if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
             if (driver.macroWarnings++ < driver.lefrMacroWarnings)
               driver.lefWarning(2039, "A SITE statement is defined before ORIGIN statement.\nTo avoid this warning in the future, define SITE after ORIGIN");
       }
       if (driver.pinDef) { /* PIN is defined before ORIGIN */
          /* pcr 283846 suppress warning */
          if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
             if (driver.macroWarnings++ < driver.lefrMacroWarnings)
               driver.lefWarning(2040, "A PIN statement is defined before ORIGIN statement.\nTo avoid this warning in the future, define PIN after ORIGIN");
       }
       if (driver.obsDef) { /* OBS is defined before ORIGIN */
          /* pcr 283846 suppress warning */
          if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
             if (driver.macroWarnings++ < driver.lefrMacroWarnings)
               driver.lefWarning(2041, "A OBS statement is defined before ORIGIN statement.\nTo avoid this warning in the future, define OBS after ORIGIN");
       }
       /* 11/22/99 - Wanda da Rosa. PCR 283846 
                     can be defined any order.
       if (driver.symDef)  * SYMMETRY is defined before ORIGIN *
          driver.lefWarning("SYMMETRY is defined before ORIGIN.");
       */
       /* Add back it back in per Nora request on PCR 283846 */
       /* 1/14/2000 - Wanda da Rosa, PCR 288770
       if (driver.sizeDef)  * SIZE is defined before ORIGIN *
          driver.lefWarning("SIZE is defined before ORIGIN.");
       */
      
       /* Workaround for pcr 640902 */
       if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setOrigin($2->x, $2->y);
       if (/*driver.lefrMacroOriginCbk*/ 1) {
          driver.macroNum.x = $2->x; 
          driver.macroNum.y = $2->y; 
          driver.lefrMacroOriginCbk( driver.macroNum);
       }
       delete $2;
    }

macro_foreign:
  start_foreign ';'
    { if (/*driver.lefrMacroCbk*/ 1)
      driver.lefrMacro.lefiMacro::addForeign((*$1).c_str(), 0, 0.0, 0.0, -1);
      delete $1;
    }
  | start_foreign pt ';'
    { if (/*driver.lefrMacroCbk*/ 1)
      driver.lefrMacro.lefiMacro::addForeign((*$1).c_str(), 1, $2->x, $2->y, -1);
      delete $1; delete $2;
    }
  | start_foreign pt orientation ';'
    { if (/*driver.lefrMacroCbk*/ 1)
      driver.lefrMacro.lefiMacro::addForeign((*$1).c_str(), 1, $2->x, $2->y, $3);
      delete $1; delete $2;
    }
  | start_foreign orientation ';'
    { if (/*driver.lefrMacroCbk*/ 1)
      driver.lefrMacro.lefiMacro::addForeign((*$1).c_str(), 0, 0.0, 0.0, $2);
      delete $1;
    }

macro_eeq: K_EEQ { driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setEEQ((*$3).c_str()); delete $3;}

macro_leq: K_LEQ { driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setLEQ((*$3).c_str());
      } else
        if (/*driver.lefrMacroCbk*/ 1) /* write warning only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings)
             driver.lefWarning(2042, "LEQ statement in MACRO is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
     delete $3;
    }

macro_site:
  macro_site_word  GSTRING ';'
    {
      if (/*driver.lefrMacroCbk*/ 1) {
        driver.lefrMacro.lefiMacro::setSiteName((*$2).c_str());
/* For later than 5.6 release
        driver.lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
                              sizeof(lefiSitePattern));
        driver.lefrSitePatternPtr->lefiSitePattern::Init();
        driver.lefrSitePatternPtr->lefiSitePattern::setSiteName($2);
*/
      }
      delete $2;
    }
  | macro_site_word sitePattern ';'
    {
      if (/*driver.lefrMacroCbk*/ 1) {
        /* also set site name in the variable driver.siteName_ in lefiMacro */
        /* this, if user wants to use method driver.siteName will get the name also */
        /* Does not work, it will only set with the last sitename, if multiple
           SITEs are defined.  Therefore, data will not be correct
        driver.lefrMacro.lefiMacro::setSitePatternName(
            driver.lefrSitePatternPtr->lefiSitePattern::name());
        */
	driver.lefrMacro.lefiMacro::setSitePattern(driver.lefrSitePatternPtr);
	driver.lefrSitePatternPtr = 0;
      }
    }

macro_site_word: K_SITE
    { driver.lefDumbMode = 1; driver.lefNoNum = 1; driver.siteDef = 1;
        if (/*driver.lefrMacroCbk*/ 1) driver.lefrDoSite = 1; }

site_word: K_SITE
    { driver.lefDumbMode = 1; driver.lefNoNum = 1; }

macro_size: K_SIZE NUMBER K_BY NUMBER ';'
    { 
      if (driver.siteDef) { /* SITE is defined before SIZE */
         /* pcr 283846 suppress warning
         if (driver.siteWarnings++ < driver.lefrSiteWarnings)
           driver.lefWarning("SITE is defined before SIZE.");
         return 1; 
         */
      }
      driver.sizeDef = 1;
      if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setSize($2, $4);
      if (/*driver.lefrMacroSizeCbk*/ 1) {
         driver.macroNum.x = $2; 
         driver.macroNum.y = $4; 
         driver.lefrMacroSizeCbk( driver.macroNum);
      }
    }

/* This is confusing, since FEF and LEF have opposite definitions of
   ports and pins */

macro_pin: start_macro_pin macro_pin_options end_macro_pin
    { 
      if (/*driver.lefrPinCbk*/ 1)
        /*driver.lefrPinCbk( driver.lefrPin);*/
		driver.lefrMacro.addPin(driver.lefrPin);
      driver.lefrPin.lefiPin::clear();
    }

start_macro_pin: K_PIN {driver.lefDumbMode = 1; driver.lefNoNum = 1; driver.pinDef = 1;} GSTRING 
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setName((*$3).c_str());
      //strcpy(driver.pinName, $3);
      driver.pinName = (*$3);
      delete $3;
    }

end_macro_pin: K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1;} GSTRING
    {
      if (driver.pinName != *$3 ) {
        if (/*driver.lefrMacroCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "END PIN name %s is different from the PIN name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.pinName.c_str());
              driver.lefError(1643, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           } 
        } 
      } 
      delete $3;
    }

macro_pin_options:
  /* empty */
    { }
  | macro_pin_options macro_pin_option 
    { }

macro_pin_option:
  start_foreign ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addForeign((*$1).c_str(), 0, 0.0, 0.0, -1);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
     delete $1;
    }
  | start_foreign pt ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addForeign((*$1).c_str(), 1, $2->x, $2->y, -1);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $1; delete $2;
    }
  | start_foreign pt orientation ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addForeign((*$1).c_str(), 1, $2->x, $2->y, $3);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $1; delete $2;
    }
  | start_foreign K_STRUCTURE ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addForeign((*$1).c_str(), 0, 0.0, 0.0, -1);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $1;
    }
  | start_foreign K_STRUCTURE pt ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addForeign((*$1).c_str(), 1, $3->x, $3->y, -1);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $1; delete $3;
    }
  | start_foreign K_STRUCTURE pt orientation ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addForeign((*$1).c_str(), 1, $3->x, $3->y, $4);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2043, "FOREIGN statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $1; delete $3;
    }
  | K_LEQ { driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setLEQ((*$3).c_str());
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2044, "LEQ statement in MACRO PIN is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $3;
   }
  | K_POWER NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setPower($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2045, "MACRO POWER statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | electrical_direction
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setDirection((*$1).c_str()); delete $1;}
  | K_USE macro_pin_use ';'
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setUse((*$2).c_str()); delete $2;}
  | K_SCANUSE macro_scan_use ';'
    { }
  | K_LEAKAGE NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setLeakage($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2046, "MACRO LEAKAGE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, r emove this statement from the LEF file with version 5.4 or later.");
    }
  | K_RISETHRESH NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setRiseThresh($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2047, "MACRO RISETHRESH statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_FALLTHRESH NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setFallThresh($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2048, "MACRO FALLTHRESH statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_RISESATCUR NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setRiseSatcur($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2049, "MACRO RISESATCUR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_FALLSATCUR NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setFallSatcur($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2050, "MACRO FALLSATCUR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_VLO NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setVLO($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2051, "MACRO VLO statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_VHI NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setVHI($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2052, "MACRO VHI statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_TIEOFFR NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setTieoffr($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2053, "MACRO TIEOFFR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_SHAPE pin_shape ';'
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setShape((*$2).c_str()); delete $2;}
  | K_MUSTJOIN {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING ';'
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setMustjoin((*$3).c_str()); delete $3;}
  | K_OUTPUTNOISEMARGIN {driver.lefDumbMode = 1;} NUMBER NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setOutMargin($3, $4);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2054, "MACRO OUTPUTNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_OUTPUTRESISTANCE {driver.lefDumbMode = 1;} NUMBER NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setOutResistance($3, $4);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2055, "MACRO OUTPUTRESISTANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_INPUTNOISEMARGIN {driver.lefDumbMode = 1;} NUMBER NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setInMargin($3, $4);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2056, "MACRO INPUTNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_CAPACITANCE NUMBER ';' 
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setCapacitance($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2057, "MACRO CAPACITANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_MAXDELAY NUMBER ';' 
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setMaxdelay($2); }
  | K_MAXLOAD NUMBER ';' 
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setMaxload($2); }
  | K_RESISTANCE NUMBER ';' 
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setResistance($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2058, "MACRO RESISTANCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_PULLDOWNRES NUMBER ';' 
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setPulldownres($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2059, "MACRO PULLDOWNRES statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_CURRENTSOURCE K_ACTIVE ';' 
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setCurrentSource("ACTIVE");
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2060, "MACRO CURRENTSOURCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_CURRENTSOURCE K_RESISTIVE ';' 
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setCurrentSource("RESISTIVE");
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2061, "MACRO CURRENTSOURCE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_RISEVOLTAGETHRESHOLD NUMBER ';' 
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setRiseVoltage($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2062, "MACRO RISEVOLTAGETHRESHOLD statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_FALLVOLTAGETHRESHOLD NUMBER ';' 
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setFallVoltage($2);
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2063, "MACRO FALLVOLTAGETHRESHOLD statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }
  | K_IV_TABLES STRING STRING ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setTables((*$2).c_str(), (*$3).c_str());
      } else
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2064, "MACRO IV_TABLES statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    delete $2; delete $3;
    }
  | K_TAPERRULE STRING ';'
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setTaperRule((*$2).c_str()); delete $2;}
  | K_PROPERTY {driver.lefDumbMode = 1000000; driver.lefRealNum = 1; driver.lefInProp = 1; } pin_prop_list ';'
    { driver.lefDumbMode = 0;
      driver.lefRealNum = 0;
      driver.lefInProp = 0;
    }
  | start_macro_port macro_port_class_option geometries K_END
    {
      driver.lefDumbMode = 0;
      driver.hasGeoLayer = 0;
      if (/*driver.lefrPinCbk*/ 1) {
	driver.lefrPin.lefiPin::addPort(driver.lefrGeometriesPtr);
	driver.lefrGeometriesPtr = 0;
	driver.lefrDoGeometries = 0;
      }
      if ((driver.needGeometry) && (driver.needGeometry != 2))  // if the last LAYER in PORT
        if (/*driver.lefrPinCbk*/ 1) /* write warning only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings)
             driver.lefWarning(2065, "Either PATH, RECT or POLYGON statement is a required in MACRO/PIN/PORT.");
    }
  | start_macro_port K_END
    /* 06/07/2001 - Wanda da Rosa.  Allow empty PORT due to incorrect
     *              old lef files from customers
     */
    {
      // Since in start_macro_port it has call the Init method, here
      // we need to call the Destroy method.
      // Still add a null pointer to set the number of port
      if (/*driver.lefrPinCbk*/ 1) {
        driver.lefrPin.lefiPin::addPort(driver.lefrGeometriesPtr);
        driver.lefrGeometriesPtr = 0;
        driver.lefrDoGeometries = 0;
      }
      driver.hasGeoLayer = 0;
    }
  | K_ANTENNASIZE NUMBER opt_layer_name ';'
    {  /* a pre 5.4 syntax */
      driver.use5_3 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum >= 5.4) {
        if (driver.use5_4) {
           if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
             if (driver.pinWarnings++ < driver.lefrPinWarnings) {
                driver.outMsg = (char*)lefMalloc(10000);
                sprintf (driver.outMsg,
                   "ANTENNASIZE statement is a version 5.3 and earlier syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                driver.lefError(1644, driver.outMsg);
                lefFree(driver.outMsg);
                /*CHKERR();*/
             }
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaSize($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAMETALAREA NUMBER opt_layer_name ';'
    {  /* a pre 5.4 syntax */
      driver.use5_3 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum >= 5.4) {
        if (driver.use5_4) {
           if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
              if (driver.pinWarnings++ < driver.lefrPinWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "ANTENNAMETALAREA statement is a version 5.3 and earlier syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                 driver.lefError(1645, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaMetalArea($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAMETALLENGTH NUMBER opt_layer_name ';'
    { /* a pre 5.4 syntax */ 
      driver.use5_3 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum >= 5.4) {
        if (driver.use5_4) {
           if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
              if (driver.pinWarnings++ < driver.lefrPinWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "ANTENNAMETALLENGTH statement is a version 5.3 and earlier syntax.\nYour lef file is defined with version %g.", driver.versionNum);
                 driver.lefError(1646, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaMetalLength($2, (*$3).c_str());
      delete $3;
    }
  | K_RISESLEWLIMIT NUMBER ';'
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setRiseSlewLimit($2); }
  | K_FALLSLEWLIMIT NUMBER ';'
    { if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setFallSlewLimit($2); }
  | K_ANTENNAPARTIALMETALAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAPARTIALMETALAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1647, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAPARTIALMETALAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1647, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaPartialMetalArea($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAPARTIALMETALSIDEAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAPARTIALMETALSIDEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1648, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAPARTIALMETALSIDEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1648, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaPartialMetalSideArea($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAPARTIALCUTAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAPARTIALCUTAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1649, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAPARTIALCUTAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1649, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaPartialCutArea($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNADIFFAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNADIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1650, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNADIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1650, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaDiffArea($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAGATEAREA NUMBER opt_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAGATEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1651, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAGATEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1651, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaGateArea($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAMAXAREACAR NUMBER req_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMAXAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1652, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMAXAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1652, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaMaxAreaCar($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAMAXSIDEAREACAR NUMBER req_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMAXSIDEAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1653, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMAXSIDEAREACAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1653, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaMaxSideAreaCar($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAMAXCUTCAR NUMBER req_layer_name ';'
    { /* 5.4 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.4) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMAXCUTCAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1654, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMAXCUTCAR statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1654, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
      if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::addAntennaMaxCutCar($2, (*$3).c_str());
      delete $3;
    }
  | K_ANTENNAMODEL
    { /* 5.5 syntax */
      driver.use5_4 = 1;
      if (driver.ignoreVersion) {
        /* do nothing */
      } else if (driver.versionNum < 5.5) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMODEL statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1655, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else if (driver.use5_3) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ANTENNAMODEL statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1655, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      }
    }
    pin_layer_oxide ';'
  | K_NETEXPR {driver.lefDumbMode = 2; driver.lefNoNum = 2; } QSTRING ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "NETEXPR statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1656, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setNetExpr((*$3).c_str());
    delete $3;
    }
  | K_SUPPLYSENSITIVITY {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "SUPPLYSENSITIVITY statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1657, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setSupplySensitivity((*$3).c_str());
    delete $3;
    }
  | K_GROUNDSENSITIVITY {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    {
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrPinCbk*/ 1) { /* write error only if cbk is set */
           if (driver.pinWarnings++ < driver.lefrPinWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "GROUNDSENSITIVITY statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1658, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } else
        if (/*driver.lefrPinCbk*/ 1) driver.lefrPin.lefiPin::setGroundSensitivity((*$3).c_str());
    delete $3;
    }

pin_layer_oxide:
  K_OXIDE1
    {
    if (/*driver.lefrPinCbk*/ 1)
       driver.lefrPin.lefiPin::addAntennaModel(1);
    }
  | K_OXIDE2
    {
    if (/*driver.lefrPinCbk*/ 1)
       driver.lefrPin.lefiPin::addAntennaModel(2);
    }
  | K_OXIDE3
    {
    if (/*driver.lefrPinCbk*/ 1)
       driver.lefrPin.lefiPin::addAntennaModel(3);
    }
  | K_OXIDE4
    {
    if (/*driver.lefrPinCbk*/ 1)
       driver.lefrPin.lefiPin::addAntennaModel(4);
    }

pin_prop_list:
  pin_name_value_pair
  | pin_prop_list pin_name_value_pair
  ;

pin_name_value_pair:
  STRING NUMBER
    { 
      char temp[32];
      sprintf(temp, "%.11g", $2);
      if (/*driver.lefrPinCbk*/ 1) {
         char propTp;
         propTp = driver.lefrPinProp.lefiPropType::propType((*$1).c_str());
         driver.lefrPin.lefiPin::setNumProperty((*$1).c_str(), $2, temp, propTp);
      }
      delete $1;
    }
  | STRING QSTRING
    {
      if (/*driver.lefrPinCbk*/ 1) {
         char propTp;
         propTp = driver.lefrPinProp.lefiPropType::propType((*$1).c_str());
         driver.lefrPin.lefiPin::setProperty((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }
  | STRING STRING
    {
      if (/*driver.lefrPinCbk*/ 1) {
         char propTp;
         propTp = driver.lefrPinProp.lefiPropType::propType((*$1).c_str());
         driver.lefrPin.lefiPin::setProperty((*$1).c_str(), (*$2).c_str(), propTp);
      }
      delete $1; delete $2;
    }

electrical_direction:
  K_DIRECTION K_INPUT ';'         {$$ = new std::string ("INPUT");}
  | K_DIRECTION K_OUTPUT ';'            {$$ = new std::string ("OUTPUT");}
  | K_DIRECTION K_OUTPUT K_TRISTATE ';' {$$ = new std::string ("OUTPUT TRISTATE");}
  | K_DIRECTION K_INOUT  ';'            {$$ = new std::string ("INOUT");}
  | K_DIRECTION K_FEEDTHRU ';'          {$$ = new std::string ("FEEDTHRU");}

start_macro_port: K_PORT
    {
      if (/*driver.lefrPinCbk*/ 1) {
	driver.lefrDoGeometries = 1;
        driver.hasPRP = 0;
	driver.lefrGeometriesPtr = (lefiGeometries*)lefMalloc( sizeof(lefiGeometries));
	driver.lefrGeometriesPtr->lefiGeometries::Init();
      }
      driver.needGeometry = 0;  // don't need rect/path/poly define yet
      driver.hasGeoLayer = 0;   // make sure LAYER is set before geometry
    }

macro_port_class_option: /* empty */
  | K_CLASS class_type ';'
    { if (driver.lefrDoGeometries)
        driver.lefrGeometriesPtr->lefiGeometries::addClass((*$2).c_str()); 
    delete $2;
    }

macro_pin_use:
  K_SIGNAL	{$$ = new std::string ("SIGNAL");}
  | K_ANALOG 	{$$ = new std::string ("ANALOG");}
  | K_POWER 	{$$ = new std::string ("POWER");}
  | K_GROUND	{$$ = new std::string ("GROUND");}
  | K_CLOCK	{$$ = new std::string ("CLOCK");}
  | K_DATA	{$$ = new std::string ("DATA");}

macro_scan_use:
  K_INPUT {$$ = new std::string ("INPUT");}
  | K_OUTPUT	{$$ = new std::string ("OUTPUT");}
  | K_START	{$$ = new std::string ("START");}
  | K_STOP	{$$ = new std::string ("STOP");}

pin_shape:
  /* empty */   {$$ = new std::string (""); }      /* non-ring shape */
  | K_ABUTMENT	{$$ = new std::string ("ABUTMENT");}
  | K_RING 	{$$ = new std::string ("RING");}
  | K_FEEDTHRU	{$$ = new std::string ("FEEDTHRU");}

geometries: geometry geometry_options

geometry:
  K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING
    {
      if ((driver.needGeometry) && (driver.needGeometry != 2)) // 1 LAYER follow after another
        if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
          /* geometries is called by MACRO/OBS & MACRO/PIN/PORT */
          if (driver.obsDef)
             driver.lefWarning(2076, "Either PATH, RECT or POLYGON statement is a required in MACRO/OBS.");
          else
             driver.lefWarning(2065, "Either PATH, RECT or POLYGON statement is a required in MACRO/PIN/PORT.");
        }
      if (driver.lefrDoGeometries)
        driver.lefrGeometriesPtr->lefiGeometries::addLayer((*$3).c_str());
      driver.needGeometry = 1;    // within LAYER it requires either path, rect, poly
      driver.hasGeoLayer = 1;
      delete $3;
    }
  layer_exceptpgnet
  layer_spacing ';'
  | K_WIDTH NUMBER ';'
    { 
      if (driver.lefrDoGeometries) {
        if (driver.hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              /*CHKERR();*/
           }
        } else
           driver.lefrGeometriesPtr->lefiGeometries::addWidth($2); 
      } 
    }
  | K_PATH firstPt otherPts ';'
    { if (driver.lefrDoGeometries) {
        if (driver.hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              /*CHKERR();*/
           }
        } else
           driver.lefrGeometriesPtr->lefiGeometries::addPath();
      }
      driver.hasPRP = 1;
      driver.needGeometry = 2;
    }
  | K_PATH K_ITERATE firstPt otherPts stepPattern ';'
    { if (driver.lefrDoGeometries) {
        if (driver.hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              /*CHKERR();*/
           }
        } else
           driver.lefrGeometriesPtr->lefiGeometries::addPathIter();
      } 
      driver.hasPRP = 1;
      driver.needGeometry = 2;
    }
  | K_RECT pt pt';'
    { if (driver.lefrDoGeometries) {
        if (driver.hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              /*CHKERR();*/
           }
        } else
           driver.lefrGeometriesPtr->lefiGeometries::addRect($2->x, $2->y, $3->x, $3->y);
      }
      driver.needGeometry = 2;
      delete $2; delete $3;
    }
  | K_RECT K_ITERATE pt pt stepPattern ';'
    { if (driver.lefrDoGeometries) {
        if (driver.hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              /*CHKERR();*/
           }
        } else
           driver.lefrGeometriesPtr->lefiGeometries::addRectIter($3->x, $3->y, $4->x,
                                                          $4->y);
      }
      driver.needGeometry = 2;
      delete $3; delete $4;
    }
  | K_POLYGON firstPt nextPt nextPt otherPts ';'
    {
      if (driver.lefrDoGeometries) {
        if (driver.hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              /*CHKERR();*/
           }
        } else
           driver.lefrGeometriesPtr->lefiGeometries::addPolygon();
      }
      driver.hasPRP = 1;
      driver.needGeometry = 2;
    }
  | K_POLYGON K_ITERATE firstPt nextPt nextPt otherPts stepPattern ';'
    { if (driver.lefrDoGeometries) {
        if (driver.hasGeoLayer == 0) {   /* LAYER statement is missing */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.lefError(1701, "A LAYER statement is missing in Geometry.\nLAYER is a required statement before any geometry can be defined.");
              /*CHKERR();*/
           }
        } else
           driver.lefrGeometriesPtr->lefiGeometries::addPolygonIter();
      }
      driver.hasPRP = 1;
      driver.needGeometry = 2;
    }
  | via_placement
    { }

geometry_options: /* empty */
  | geometry_options geometry

layer_exceptpgnet: /* empty */
  | K_EXCEPTPGNET                   /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "EXCEPTPGNET is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1699, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      } else {
       if (driver.lefrDoGeometries)
        driver.lefrGeometriesPtr->lefiGeometries::addLayerExceptPgNet();
      }
    }

layer_spacing: /* empty */
  | K_SPACING NUMBER
    { if (driver.lefrDoGeometries) {
        if (driver.zeroOrGt($2))
           driver.lefrGeometriesPtr->lefiGeometries::addLayerMinSpacing($2);
        else {
           driver.outMsg = (char*)lefMalloc(10000);
           sprintf (driver.outMsg,
              "THE SPACING statement has the value %g in MACRO OBS.\nValue has to be 0 or greater.", $2);
           driver.lefError(1659, driver.outMsg);
           lefFree(driver.outMsg);
           /*CHKERR();*/
        }
      }
    }
  | K_DESIGNRULEWIDTH NUMBER
    { if (driver.lefrDoGeometries) {
        if (driver.zeroOrGt($2))
           driver.lefrGeometriesPtr->lefiGeometries::addLayerRuleWidth($2);
        else {
           driver.outMsg = (char*)lefMalloc(10000);
           sprintf (driver.outMsg,
              "THE DESIGNRULEWIDTH statement has the value %g in MACRO OBS.\nValue has to be 0 or greater.", $2);
           driver.lefError(1660, driver.outMsg);
           lefFree(driver.outMsg);
           /*CHKERR();*/
        }
      }
    }

firstPt: pt  
    { if (driver.lefrDoGeometries)
        driver.lefrGeometriesPtr->lefiGeometries::startList($1->x, $1->y); 
    delete $1;
    }

nextPt:  pt
    { if (driver.lefrDoGeometries)
        driver.lefrGeometriesPtr->lefiGeometries::addToList($1->x, $1->y); 
    delete $1;
    }

otherPts:
  /* empty */
/* switch to avoid stack overflow with large polygon
  | nextPt otherPts
*/
  | otherPts nextPt
  ;

via_placement:
  K_VIA pt {driver.lefDumbMode = 1;} STRING ';'
    { if (driver.lefrDoGeometries)
        driver.lefrGeometriesPtr->lefiGeometries::addVia($2->x, $2->y, (*$4).c_str()); 
    delete $2; delete $4;
    }
  | K_VIA K_ITERATE pt {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
    stepPattern ';'
    { if (driver.lefrDoGeometries)
        driver.lefrGeometriesPtr->lefiGeometries::addViaIter($3->x, $3->y, (*$5).c_str()); 
    delete $3; delete $5;
    }
        

stepPattern: K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
     { if (driver.lefrDoGeometries)
         driver.lefrGeometriesPtr->lefiGeometries::addStepPattern($2, $4, $6, $7); }

sitePattern: GSTRING NUMBER NUMBER orientation
  K_DO NUMBER K_BY NUMBER K_STEP NUMBER NUMBER
    {
      if (driver.lefrDoSite) {
	driver.lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
				   sizeof(lefiSitePattern));
	driver.lefrSitePatternPtr->lefiSitePattern::Init();
	driver.lefrSitePatternPtr->lefiSitePattern::set((*$1).c_str(), $2, $3, $4, $6, $8,
	  $10, $11);
	}
    delete $1;
    }
  | GSTRING NUMBER NUMBER orientation
    {
      if (driver.lefrDoSite) {
	driver.lefrSitePatternPtr = (lefiSitePattern*)lefMalloc(
				   sizeof(lefiSitePattern));
	driver.lefrSitePatternPtr->lefiSitePattern::Init();
	driver.lefrSitePatternPtr->lefiSitePattern::set((*$1).c_str(), $2, $3, $4, -1, -1,
	  -1, -1);
	}
    delete $1;
    }

trackPattern:
  K_X NUMBER K_DO NUMBER K_STEP NUMBER 
    { 
      if (driver.lefrDoTrack) {
	driver.lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
				sizeof(lefiTrackPattern));
	driver.lefrTrackPatternPtr->lefiTrackPattern::Init();
	driver.lefrTrackPatternPtr->lefiTrackPattern::set("X", $2, (int)$4, $6);
      }    
    }
    K_LAYER {driver.lefDumbMode = 1000000000;} trackLayers
    { driver.lefDumbMode = 0;}
  | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
    { 
      if (driver.lefrDoTrack) {
	driver.lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	driver.lefrTrackPatternPtr->lefiTrackPattern::Init();
	driver.lefrTrackPatternPtr->lefiTrackPattern::set("Y", $2, (int)$4, $6);
      }    
    }
    K_LAYER {driver.lefDumbMode = 1000000000;} trackLayers
    { driver.lefDumbMode = 0;}
  | K_X NUMBER K_DO NUMBER K_STEP NUMBER 
    { 
      if (driver.lefrDoTrack) {
	driver.lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	driver.lefrTrackPatternPtr->lefiTrackPattern::Init();
	driver.lefrTrackPatternPtr->lefiTrackPattern::set("X", $2, (int)$4, $6);
      }    
    }
  | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
    { 
      if (driver.lefrDoTrack) {
	driver.lefrTrackPatternPtr = (lefiTrackPattern*)lefMalloc(
                                    sizeof(lefiTrackPattern));
	driver.lefrTrackPatternPtr->lefiTrackPattern::Init();
	driver.lefrTrackPatternPtr->lefiTrackPattern::set("Y", $2, (int)$4, $6);
      }    
    }

trackLayers:
  /* empty */
  | trackLayers layer_name
  ;

layer_name: STRING
    { if (driver.lefrDoTrack) driver.lefrTrackPatternPtr->lefiTrackPattern::addLayer((*$1).c_str()); delete $1;}

gcellPattern: K_X NUMBER K_DO NUMBER K_STEP NUMBER
    {
      if (driver.lefrDoGcell) {
	driver.lefrGcellPatternPtr = (lefiGcellPattern*)lefMalloc(
                                    sizeof(lefiGcellPattern));
	driver.lefrGcellPatternPtr->lefiGcellPattern::Init();
	driver.lefrGcellPatternPtr->lefiGcellPattern::set("X", $2, (int)$4, $6);
      }    
    }
  | K_Y NUMBER K_DO NUMBER K_STEP NUMBER
    {
      if (driver.lefrDoGcell) {
	driver.lefrGcellPatternPtr = (lefiGcellPattern*)lefMalloc(
                                    sizeof(lefiGcellPattern));
	driver.lefrGcellPatternPtr->lefiGcellPattern::Init();
	driver.lefrGcellPatternPtr->lefiGcellPattern::set("Y", $2, (int)$4, $6);
      }    
    }

macro_obs: start_macro_obs geometries K_END
    { 
      if (/*driver.lefrObstructionCbk*/ 1) {
	driver.lefrObstruction.lefiObstruction::setGeometries(driver.lefrGeometriesPtr);
	driver.lefrGeometriesPtr = 0;
	driver.lefrDoGeometries = 0;
        driver.lefrObstructionCbk( driver.lefrObstruction);
      }
      driver.lefDumbMode = 0;
      driver.hasGeoLayer = 0;       /* reset */
    }
  | start_macro_obs K_END
    /* 08/14/00 - Wanda da Rosa.  Allow empty OBS due to so many lef files
     * have empty OBS
    {
       driver.lefError("OBS requires either a LAYER or a VIA.");
       CHKERR();
    }
    */
    {
       /* The pointer has malloced in start, need to free manually */
       if (driver.lefrGeometriesPtr) {
          driver.lefrGeometriesPtr->lefiGeometries::Destroy();
          lefFree(driver.lefrGeometriesPtr);
          driver.lefrGeometriesPtr = 0;
	  driver.lefrDoGeometries = 0;
       }
       driver.hasGeoLayer = 0;
    }

start_macro_obs: K_OBS
    {
      driver.obsDef = 1;
      if (/*driver.lefrObstructionCbk*/ 1) {
	driver.lefrDoGeometries = 1;
	driver.lefrGeometriesPtr = (lefiGeometries*)lefMalloc(
	    sizeof(lefiGeometries));
	driver.lefrGeometriesPtr->lefiGeometries::Init();
	}
      driver.hasGeoLayer = 0;
    }

macro_density: K_DENSITY density_layer density_layers K_END
    { 
      if (driver.versionNum < 5.6) {
        if (/*driver.lefrDensityCbk*/ 1) { /* write error only if cbk is set */
           if (driver.macroWarnings++ < driver.lefrMacroWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "DENSITY statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1661, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
      } 
      if (/*driver.lefrDensityCbk*/ 1) {
        driver.lefrDensityCbk( driver.lefrDensity);
        driver.lefrDensity.lefiDensity::clear();
      }
      driver.lefDumbMode = 0;
    }

density_layers: /* empty */
    | density_layers density_layer
    ;

density_layer: K_LAYER { driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    {
      if (/*driver.lefrDensityCbk*/ 1)
        driver.lefrDensity.lefiDensity::addLayer((*$3).c_str());
    delete $3;
    }
    density_layer_rect density_layer_rects

density_layer_rects: /* empty */
    | density_layer_rects density_layer_rect
    ;

density_layer_rect: K_RECT pt pt NUMBER ';'
    {
      if (/*driver.lefrDensityCbk*/ 1)
        driver.lefrDensity.lefiDensity::addRect($2->x, $2->y, $3->x, $3->y, $4); 
    delete $2; delete $3;
    }

macro_clocktype: K_CLOCKTYPE { driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING ';'
    { if (/*driver.lefrMacroCbk*/ 1) driver.lefrMacro.lefiMacro::setClockType((*$3).c_str()); delete $3;}

timing: start_timing timing_options end_timing
    { }

start_timing: K_TIMING 
    { /* XXXXX for macros */ }

end_timing: K_END K_TIMING 
  {
    if (driver.versionNum < 5.4) {
      if (/*driver.lefrTimingCbk*/ 1 && driver.lefrTiming.lefiTiming::hasData())
        driver.lefrTimingCbk( driver.lefrTiming);
      driver.lefrTiming.lefiTiming::clear();
    } else {
      if (/*driver.lefrTimingCbk*/ 1) /* write warning only if cbk is set */
        if (driver.timingWarnings++ < driver.lefrTimingWarnings)
          driver.lefWarning(2066, "MACRO TIMING statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
      driver.lefrTiming.lefiTiming::clear();
    }
  }

timing_options:
  /* empty */
  | timing_options timing_option
  ;

timing_option:
  K_FROMPIN 
    {
    if (driver.versionNum < 5.4) {
      if (/*driver.lefrTimingCbk*/ 1 && driver.lefrTiming.lefiTiming::hasData())
        driver.lefrTimingCbk( driver.lefrTiming);
    }
    driver.lefDumbMode = 1000000000;
    driver.lefrTiming.lefiTiming::clear();
    }
    list_of_from_strings ';'
    { driver.lefDumbMode = 0;}
  | K_TOPIN {driver.lefDumbMode = 1000000000;} list_of_to_strings ';'
    { driver.lefDumbMode = 0;}
  | risefall K_INTRINSIC NUMBER NUMBER
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addRiseFall((*$1).c_str(),$3,$4); delete $1;}
    slew_spec K_VARIABLE NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addRiseFallVariable($8,$9); }
  | risefall delay_or_transition K_UNATENESS unateness
    K_TABLEDIMENSION NUMBER NUMBER NUMBER ';' 
    { if (/*driver.lefrTimingCbk*/ 1) {
	if ((*$2)[0] == 'D' || (*$2)[0] == 'd') /* delay */
	  driver.lefrTiming.lefiTiming::addDelay((*$1).c_str(), (*$4).c_str(), $6, $7, $8);
	else
	  driver.lefrTiming.lefiTiming::addTransition((*$1).c_str(), (*$4).c_str(), $6, $7, $8);
      }
      delete $1; delete $2; delete $4;
    }
  | K_TABLEAXIS list_of_table_axis_numbers ';'
    { }
  | K_TABLEENTRIES list_of_table_entries ';'
    { }
  | K_RISERS NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setRiseRS($2,$3); }
  | K_FALLRS NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setFallRS($2,$3); }
  | K_RISECS NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setRiseCS($2,$3); }
  | K_FALLCS NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setFallCS($2,$3); }
  | K_RISESATT1 NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setRiseAtt1($2,$3); }
  | K_FALLSATT1 NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setFallAtt1($2,$3); }
  | K_RISET0 NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setRiseTo($2,$3); }
  | K_FALLT0 NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setFallTo($2,$3); }
  | K_UNATENESS unateness ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addUnateness((*$2).c_str()); delete $2;}
  | K_STABLE K_SETUP NUMBER K_HOLD NUMBER risefall ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setStable($3,$5,(*$6).c_str()); delete $6;}
  | two_pin_trigger from_pin_trigger to_pin_trigger K_TABLEDIMENSION NUMBER NUMBER NUMBER ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addSDF2Pins((*$1).c_str(),(*$2).c_str(),(*$3).c_str(),$5,$6,$7); delete $1; delete $2; delete $3;}
  | one_pin_trigger K_TABLEDIMENSION NUMBER NUMBER NUMBER ';' 
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addSDF1Pin((*$1).c_str(),$3,$4,$4); delete $1;}
  | K_SDFCONDSTART QSTRING ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setSDFcondStart((*$2).c_str()); delete $2;}
  | K_SDFCONDEND QSTRING ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setSDFcondEnd((*$2).c_str()); delete $2;}
  | K_SDFCOND QSTRING ';'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::setSDFcond((*$2).c_str()); delete $2;}
  | K_EXTENSION ';'
    { /* XXXXX */ }

one_pin_trigger:
  K_MPWH
    { $$ = new std::string ("MPWH");}
  | K_MPWL
    { $$ = new std::string ("MPWL");}
  | K_PERIOD
    { $$ = new std::string ("PERIOD");}

two_pin_trigger :
  K_SETUP
    { $$ = new std::string ("SETUP");}
  | K_HOLD
    { $$ = new std::string ("HOLD");}
  | K_RECOVERY
    { $$ = new std::string ("RECOVERY");}
  | K_SKEW
    { $$ = new std::string ("SKEW");}

from_pin_trigger:
  K_ANYEDGE
    { $$ = new std::string ("ANYEDGE");}
  | K_POSEDGE
    { $$ = new std::string ("POSEDGE");}
  | K_NEGEDGE 
    { $$ = new std::string ("NEGEDGE");}

to_pin_trigger:
  K_ANYEDGE
    { $$ = new std::string ("ANYEDGE");}
  | K_POSEDGE
    { $$ = new std::string ("POSEDGE");}
  | K_NEGEDGE 
    { $$ = new std::string ("NEGEDGE");}

delay_or_transition :
  K_DELAY
    { $$ = new std::string ("DELAY"); }
  | K_TRANSITIONTIME
    { $$ = new std::string ("TRANSITION"); }

list_of_table_entries:
  table_entry
    { }
  | list_of_table_entries table_entry
    { }

table_entry: '(' NUMBER NUMBER NUMBER ')'
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addTableEntry($2,$3,$4); }

list_of_table_axis_numbers:
  NUMBER
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addTableAxisNumber($1); }
  | list_of_table_axis_numbers NUMBER
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addTableAxisNumber($2); }

slew_spec:
  /* empty */
    { }
  | NUMBER NUMBER NUMBER NUMBER 
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addRiseFallSlew($1,$2,$3,$4); }
  |  NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER 
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addRiseFallSlew($1,$2,$3,$4);
      if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addRiseFallSlew2($5,$6,$7); }

risefall:
  K_RISE
    { $$ = new std::string ("RISE"); }
  | K_FALL 
    { $$ = new std::string ("FALL"); }

unateness:
  K_INVERT
    { $$ = new std::string ("INVERT"); }
  | K_NONINVERT
    { $$ = new std::string ("NONINVERT"); }
  | K_NONUNATE 
    { $$ = new std::string ("NONUNATE"); }

list_of_from_strings:
  STRING
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addFromPin((*$1).c_str()); delete $1;}
  | list_of_from_strings STRING 
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addFromPin((*$2).c_str()); delete $2;}

list_of_to_strings:
  STRING
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addToPin((*$1).c_str()); delete $1;}
  | list_of_to_strings STRING 
    { if (/*driver.lefrTimingCbk*/ 1) driver.lefrTiming.lefiTiming::addToPin((*$2).c_str()); delete $2;}

array: start_array array_rules
    {
      if (/*driver.lefrArrayCbk*/ 1) driver.lefrArrayCbk( driver.lefrArray);
      driver.lefrArray.lefiArray::clear();
      driver.lefrSitePatternPtr = 0;
      driver.lefrDoSite = 0;
   }
    end_array

start_array: K_ARRAY {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
    {
      if (/*driver.lefrArrayCbk*/ 1) {
	driver.lefrArray.lefiArray::setName((*$3).c_str());
	driver.lefrArrayBeginCbk( (*$3).c_str());
      }
      //strcpy(driver.arrayName, $3);
      driver.arrayName = (*$3);
      delete $3;
    }

end_array: K_END {driver.lefDumbMode = 1; driver.lefNoNum = 1;} STRING
    {
      if (/*driver.lefrArrayCbk && driver.lefrArrayEndCbk*/ 1)
	  driver.lefrArrayEndCbk( (*$3).c_str());
      if (driver.arrayName != *$3) {
        if (/*driver.lefrArrayCbk*/ 1) { /* write error only if cbk is set */
           if (driver.arrayWarnings++ < driver.lefrArrayWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "END ARRAY name %s is different from the ARRAY name %s.\nCorrect the LEF file before rerun it through the LEF parser.", (*$3).c_str(), driver.arrayName.c_str());
              driver.lefError(1662, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           } 
        } 
      } 
      delete $3;
    }

array_rules:
  /* empty */
    { }
  | array_rules array_rule
    { }

array_rule:
  site_word { if (/*driver.lefrArrayCbk*/ 1) driver.lefrDoSite = 1; driver.lefDumbMode = 1; }
    sitePattern  ';'
    {
      if (/*driver.lefrArrayCbk*/ 1) {
	driver.lefrArray.lefiArray::addSitePattern(driver.lefrSitePatternPtr);
      }
    }
  | K_CANPLACE {driver.lefDumbMode = 1; if (/*driver.lefrArrayCbk*/ 1) driver.lefrDoSite = 1; }
    sitePattern ';'
    {
      if (/*driver.lefrArrayCbk*/ 1) {
	driver.lefrArray.lefiArray::addCanPlace(driver.lefrSitePatternPtr);
      }
    }
  | K_CANNOTOCCUPY {driver.lefDumbMode = 1; if (/*driver.lefrArrayCbk*/ 1) driver.lefrDoSite = 1; }
    sitePattern ';'
    {
      if (/*driver.lefrArrayCbk*/ 1) {
	driver.lefrArray.lefiArray::addCannotOccupy(driver.lefrSitePatternPtr);
      }
    }
  | K_TRACKS { if (/*driver.lefrArrayCbk*/ 1) driver.lefrDoTrack = 1; } trackPattern ';'
    {
      if (/*driver.lefrArrayCbk*/ 1) {
	driver.lefrArray.lefiArray::addTrack(driver.lefrTrackPatternPtr);
      }
    }
  | floorplan_start floorplan_list K_END STRING 
    {
    }
  | K_GCELLGRID { if (/*driver.lefrArrayCbk*/ 1) driver.lefrDoGcell = 1; } gcellPattern ';'
    {
      if (/*driver.lefrArrayCbk*/ 1) {
	driver.lefrArray.lefiArray::addGcell(driver.lefrGcellPatternPtr);
      }
    }
  | K_DEFAULTCAP NUMBER cap_list K_END K_DEFAULTCAP
    {
      if (/*driver.lefrArrayCbk*/ 1) {
	driver.lefrArray.lefiArray::setTableSize((int)$2);
      }
    }
  | def_statement
    { }

floorplan_start: K_FLOORPLAN STRING
    { if (/*driver.lefrArrayCbk*/ 1) driver.lefrArray.lefiArray::addFloorPlan((*$2).c_str()); delete $2;}
	
floorplan_list:
  /* empty */
    { }
  | floorplan_list floorplan_element
    { }

floorplan_element:
  K_CANPLACE { driver.lefDumbMode = 1; if (/*driver.lefrArrayCbk*/ 1) driver.lefrDoSite = 1; }
    sitePattern ';'
    {
      if (/*driver.lefrArrayCbk*/ 1)
	driver.lefrArray.lefiArray::addSiteToFloorPlan("CANPLACE",
	driver.lefrSitePatternPtr);
    }
  | K_CANNOTOCCUPY { if (/*driver.lefrArrayCbk*/ 1) driver.lefrDoSite = 1; driver.lefDumbMode = 1; }
    sitePattern ';'
    {
      if (/*driver.lefrArrayCbk*/ 1)
	driver.lefrArray.lefiArray::addSiteToFloorPlan("CANNOTOCCUPY",
	driver.lefrSitePatternPtr);
     }

cap_list:
  /* empty */
    { }
  | cap_list one_cap
    { }

one_cap: K_MINPINS NUMBER K_WIRECAP NUMBER ';'
    { if (/*driver.lefrArrayCbk*/ 1) driver.lefrArray.lefiArray::addDefaultCap((int)$2, $4); }

msg_statement:
  K_MESSAGE {driver.lefDumbMode=1;driver.lefNlToken=TRUE;} STRING '=' s_expr dtrm
    { driver.lefAddStringMessage((*$3).c_str(), (*$5).c_str()); delete $3; delete $5;}

create_file_statement:
  K_CREATEFILE {driver.lefDumbMode=1;driver.lefNlToken=TRUE;} STRING '=' s_expr dtrm
    { }

def_statement:
  K_DEFINE {driver.lefDumbMode=1;driver.lefNlToken=TRUE;} STRING '=' expression dtrm
    {
      if (driver.versionNum < 5.6)
        driver.lefAddNumDefine((*$3).c_str(), $5);
      else
        if (/*driver.lefrArrayCbk*/ 1) /* write warning only if cbk is set */
           if (driver.arrayWarnings++ < driver.lefrArrayWarnings)
             driver.lefWarning(2067, "DEFINE statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $3;
    }
  |  K_DEFINES {driver.lefDumbMode=1;driver.lefNlToken=TRUE;} STRING '=' s_expr dtrm
    {
      if (driver.versionNum < 5.6)
        driver.lefAddStringDefine((*$3).c_str(), (*$5).c_str());
      else
        if (/*driver.lefrArrayCbk*/ 1) /* write warning only if cbk is set */
           if (driver.arrayWarnings++ < driver.lefrArrayWarnings)
             driver.lefWarning(2068, "DEFINES statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $3; delete $5;
    }
  |  K_DEFINEB {driver.lefDumbMode=1;driver.lefNlToken=TRUE;} STRING '=' b_expr dtrm
    {
      if (driver.versionNum < 5.6)
        driver.lefAddBooleanDefine((*$3).c_str(), $5);
      else
        if (/*driver.lefrArrayCbk*/ 1) /* write warning only if cbk is set */
           if (driver.arrayWarnings++ < driver.lefrArrayWarnings)
             driver.lefWarning(2069, "DEFINEB statement is obsolete in version 5.6 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.6 or later.");
    delete $3;
    }

/* terminator for &defines.  Can be semicolon or newline */
dtrm:
  |  ';' {driver.lefNlToken = FALSE;}
  |  '\n'	{driver.lefNlToken = FALSE;}

then:
  K_THEN
  | '\n' K_THEN
  ;

else:
  K_ELSE
  | '\n' K_ELSE
  ;

expression:
  expression '+' expression   {$$ = $1 + $3; }
  | expression '-' expression   {$$ = $1 - $3; }
  | expression '*' expression   {$$ = $1 * $3; }
  | expression '/' expression   {$$ = $1 / $3; }
  | '-' expression %prec UMINUS {$$ = -$2;}
  | '(' expression ')'		{$$ = $2;}
  | K_IF b_expr then expression else expression %prec IF
		{$$ = ($2 != 0) ? $4 : $6;}
  | NUMBER			{$$ = $1;}

b_expr:
  expression relop expression {$$ = driver.comp_num($1,$2,$3);}
  | expression K_AND expression {$$ = $1 != 0 && $3 != 0;}
  | expression K_OR  expression {$$ = $1 != 0 || $3 != 0;}
  | s_expr relop s_expr	      {$$ = driver.comp_str((*$1).c_str(),$2,(*$3).c_str()); delete $1; delete $3;}
  | s_expr K_AND s_expr	      {$$ = (*$1)[0] != 0 && (*$3)[0] != 0; delete $1; delete $3;}
  | s_expr K_OR  s_expr	      {$$ = (*$1)[0] != 0 || (*$3)[0] != 0; delete $1; delete $3;}
  | b_expr K_EQ b_expr	      {$$ = $1 == $3;}
  | b_expr K_NE b_expr	      {$$ = $1 != $3;}
  | b_expr K_AND b_expr	      {$$ = $1 && $3;}
  | b_expr K_OR  b_expr	      {$$ = $1 || $3;}
  | K_NOT b_expr		      %prec LNOT {$$ = !$$;}
  | '(' b_expr ')'	      {$$ = $2;}
  | K_IF b_expr then b_expr else b_expr %prec IF
	  {$$ = ($2 != 0) ? $4 : $6;}
  | K_TRUE		      {$$ = 1;}
  | K_FALSE		      {$$ = 0;}

s_expr:
  s_expr '+' s_expr
    {
	  $$ = new std::string ((*$1) + (*$3));
      delete $1; delete $3;
    }
  | '(' s_expr ')'
    { $$ = $2; }
  | K_IF b_expr then s_expr else s_expr %prec IF
    {
      driver.lefDefIf = TRUE;
      if ($2 != 0) {
	$$ = $4;	
      } else {
	$$ = $6;
      }
    }
  | QSTRING
    { $$ = $1; }

relop:
  K_LE {$$ = C_LE;}
  | K_LT {$$ = C_LT;}
  | K_GE {$$ = C_GE;}
  | K_GT {$$ = C_GT;}
  | K_EQ {$$ = C_EQ;}
  | K_NE {$$ = C_NE;}
  | '='  {$$ = C_EQ;}
  | '<'  {$$ = C_LT;}
  | '>'  {$$ = C_GT;}


prop_def_section: K_PROPERTYDEFINITIONS
    { 
      if (/*lefrPropBeginCbk*/ 1)
        driver.lefrPropBeginCbk( 0);
      driver.lefInPropDef = 1;  /* set flag as inside propertydefinitions */
    }
    prop_stmts K_END K_PROPERTYDEFINITIONS
    { 
      if (/*lefrPropEndCbk*/ 1)
        driver.lefrPropEndCbk( 0);
      driver.lefRealNum = 0;     /* just want to make sure it is reset */
      driver.lefInPropDef = 0;   /* reset flag */
    }

prop_stmts:
  /* empty */
    { }
  | prop_stmts prop_stmt
    { }

prop_stmt:
  K_LIBRARY {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("library", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrLibProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      delete $3;
    }
  | K_COMPONENTPIN {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("componentpin", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrCompProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      delete $3;
    }
  | K_PIN {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("pin", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrPinProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      
      delete $3;
    }
  | K_MACRO {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("macro", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrMacroProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      delete $3;
    }
  | K_VIA {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("via", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrViaProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      delete $3;
    }
  | K_VIARULE {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("viarule", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrViaRuleProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      delete $3;
    }
  | K_LAYER {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("layer", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrLayerProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      delete $3;
    }
  | K_NONDEFAULTRULE {driver.lefDumbMode = 1; driver.lefrProp.lefiProp::clear(); }
    STRING prop_define ';'
    { 
      if (/*lefrPropCbk*/ 1) {
        driver.lefrProp.lefiProp::setPropType("nondefaultrule", (*$3).c_str());
        driver.lefrPropCbk( driver.lefrProp);
      }
      driver.lefrNondefProp.lefiPropType::setPropType((*$3).c_str(), driver.lefPropDefType);
      delete $3;
    }
    
prop_define:
  K_INTEGER { driver.lefRealNum = 0; } opt_def_range opt_def_value 
    { 
      if (/*lefrPropCbk*/ 1) driver.lefrProp.lefiProp::setPropInteger();
      driver.lefPropDefType = 'I';
    }
  | K_REAL { driver.lefRealNum = 1; } opt_def_range opt_def_value
    { 
      if (/*lefrPropCbk*/ 1) driver.lefrProp.lefiProp::setPropReal();
      driver.lefPropDefType = 'R';
      driver.lefRealNum = 0;
    }
  | K_STRING
    {
      if (/*lefrPropCbk*/ 1) driver.lefrProp.lefiProp::setPropString();
      driver.lefPropDefType = 'S';
    }
  | K_STRING QSTRING
    {
      if (/*lefrPropCbk*/ 1) driver.lefrProp.lefiProp::setPropQString((*$2).c_str());
      driver.lefPropDefType = 'Q';
      delete $2;
    }
  | K_NAMEMAPSTRING STRING
    {
      if (/*lefrPropCbk*/ 1) driver.lefrProp.lefiProp::setPropNameMapString((*$2).c_str());
      driver.lefPropDefType = 'S';
      delete $2;
    }

opt_range_second:
  /* nothing */
    { }
  | K_USELENGTHTHRESHOLD
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingRangeUseLength();
    }
  | K_INFLUENCE NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        driver.lefrLayer.lefiLayer::setSpacingRangeInfluence($2);
        driver.lefrLayer.lefiLayer::setSpacingRangeInfluenceRange(-1, -1);
      }
    }
  | K_INFLUENCE NUMBER K_RANGE NUMBER NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        driver.lefrLayer.lefiLayer::setSpacingRangeInfluence($2);
        driver.lefrLayer.lefiLayer::setSpacingRangeInfluenceRange($4, $5);
      }
    }
  | K_RANGE NUMBER NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingRangeRange($2, $3);
    }

opt_endofline:                                      /* 5.7 */
  /* nothing */
    { }
  | K_PARALLELEDGE NUMBER K_WITHIN NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingParSW($2, $4);
    }
    opt_endofline_twoedges

opt_endofline_twoedges:                             /* 5.7 */
  /* nothing */
    { }
  | K_TWOEDGES
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingParTwoEdges();
    }

opt_samenetPGonly:                                  /* 5.7 */
  /* nothing */
    { }
  | K_PGONLY
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingSamenetPGonly();
    }

opt_def_range:
  /* nothing */
    { }
  | K_RANGE NUMBER NUMBER
    {  if (/*lefrPropCbk*/ 1) driver.lefrProp.lefiProp::setRange($2, $3); }

opt_def_value:
  /* empty */
    { }
  | NUMBER
    { if (/*lefrPropCbk*/ 1) driver.lefrProp.lefiProp::setNumber($1); }

layer_spacing_opts:
  /* empty */
  | layer_spacing_opt layer_spacing_opts

layer_spacing_opt: K_CENTERTOCENTER      /* 5.7 */
    {
      if (/*driver.lefrLayerCbk*/ 1) {
         if (driver.hasSpCenter) {
           if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1663, "A CENTERTOCENTER statement has already defined in SPACING\nCENTERTOCENTER can only be defined once per LAYER CUT SPACING.");
              /*CHKERR();*/
           }
        }
        driver.hasSpCenter = 1;
        if (driver.versionNum < 5.6) {
           if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "CENTERTOCENTER statement is a version 5.6 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1664, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
        if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setSpacingCenterToCenter();
      }
    }
  | K_SAMENET             /* 5.7 */
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        if (driver.hasSpSamenet) {
           if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.lefError(1665, "A SAMENET statement has already defined in SPACING\nSAMENET can only be defined once per LAYER CUT SPACING.");
              /*CHKERR();*/
           }
        }
        driver.hasSpSamenet = 1;
        if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setSpacingSamenet();
       }
    }
    opt_samenetPGonly
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "SAMENET is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1684, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      }
    }
  | K_PARALLELOVERLAP    /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "PARALLELOVERLAP is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1680, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/ 
      } else {
        if (/*driver.lefrLayerCbk*/ 1) {
          if (driver.hasSpParallel) {
             if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                driver.lefError(1666, "A PARALLELOVERLAP statement has already defined in SPACING\nPARALLELOVERLAP can only be defined once per LAYER CUT SPACING.");
                /*CHKERR();*/
             }
          }
          driver.hasSpParallel = 1;
          if (/*driver.lefrLayerCbk*/ 1)
            driver.lefrLayer.lefiLayer::setSpacingParallelOverlap();
        }
      }
    }

layer_spacing_cut_routing:
  /* empty */
  | K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING
    {
      if (/*driver.lefrLayerCbk*/ 1)
{
        if (driver.versionNum < 5.7) {
           if (driver.hasSpSamenet) {    /* 5.6 and earlier does not allow */
              if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                 driver.lefError(1667, "A SAMENET statement has already defined in SPACING\nEither SAMENET or LAYER can be defined, but not both.");
                 /*CHKERR();*/
              }
           }
        }
        driver.lefrLayer.lefiLayer::setSpacingName((*$3).c_str());
      }
      delete $3;
    }
    spacing_cut_layer_opt
  | K_ADJACENTCUTS NUMBER K_WITHIN NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        if (driver.versionNum < 5.5) {
           if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
              driver.outMsg = (char*)lefMalloc(10000);
              sprintf (driver.outMsg,
                 "ADJACENTCUTS statement is a version 5.5 and later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
              driver.lefError(1668, driver.outMsg);
              lefFree(driver.outMsg);
              /*CHKERR();*/
           }
        }
        if (driver.versionNum < 5.7) {
           if (driver.hasSpSamenet) {    /* 5.6 and earlier does not allow */
              if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                 driver.lefError(1669, "A SAMENET statement has already defined in SPACING\nEither SAMENET or ADJACENTCUTS can be defined, but not both.");
                 /*CHKERR();*/
              }
           }
        }
        driver.lefrLayer.lefiLayer::setSpacingAdjacent((int)$2, $4);
      }
    }
    opt_adjacentcuts_exceptsame
  | K_AREA NUMBER        /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "AREA is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1693, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      } else {
        if (/*driver.lefrLayerCbk*/ 1) {
          if (driver.versionNum < 5.7) {
             if (driver.hasSpSamenet) {    /* 5.6 and earlier does not allow */
                if (driver.layerWarnings++ < driver.lefrLayerWarnings) {
                   driver.lefError(1670, "A SAMENET statement has already defined in SPACING\nEither SAMENET or AREA can be defined, but not both.");
                   /*CHKERR();*/
                }
             }
          }
          driver.lefrLayer.lefiLayer::setSpacingArea($2);
        }
      }
    }
  | K_RANGE NUMBER NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingRange($2, $3);
    }
    opt_range_second
  | K_LENGTHTHRESHOLD NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        driver.lefrLayer.lefiLayer::setSpacingLength($2);
      }
    }
  | K_LENGTHTHRESHOLD NUMBER K_RANGE NUMBER NUMBER
    {
      if (/*driver.lefrLayerCbk*/ 1) {
        driver.lefrLayer.lefiLayer::setSpacingLength($2);
        driver.lefrLayer.lefiLayer::setSpacingLengthRange($4, $5);
      }
    }
  | K_ENDOFLINE NUMBER K_WITHIN NUMBER    /* 5.7 */
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingEol($2, $4);
    }
    opt_endofline
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "ENDOFLINE is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1681, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      }
    }
  | K_NOTCHLENGTH NUMBER      /* 5.7 */
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "NOTCHLENGTH is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1682, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      } else {
        if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setSpacingNotchLength($2);
      }
    }
  | K_ENDOFNOTCHWIDTH NUMBER K_NOTCHSPACING NUMBER K_NOTCHLENGTH NUMBER /*5.7*/
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "ENDOFNOTCHWIDTH is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1696, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      } else {
        if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setSpacingEndOfNotchWidth($2, $4, $6);
      }
    }

spacing_cut_layer_opt:                      /* 5.7 */
  /* empty */
    {}
  | K_STACK
    {
      if (/*driver.lefrLayerCbk*/ 1)
        driver.lefrLayer.lefiLayer::setSpacingLayerStack();
    }

opt_adjacentcuts_exceptsame:                /* 5.7 */
  /* empty */
    {}
  | K_EXCEPTSAMEPGNET
    {
      if (driver.versionNum < 5.7) {
        driver.outMsg = (char*)lefMalloc(10000);
        sprintf(driver.outMsg,
          "EXCEPTSAMEPGNET is a version 5.7 or later syntax.\nYour lef file is defined with version %g.", driver.versionNum);
        driver.lefError(1683, driver.outMsg);
        lefFree(driver.outMsg);
        /*CHKERR();*/
      } else {
        if (/*driver.lefrLayerCbk*/ 1)
          driver.lefrLayer.lefiLayer::setSpacingAdjacentExcept();
      }
    }

opt_layer_name:
  /* empty */
    { $$ = new std::string (""); }
  | K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING
    { $$ = $3; }

req_layer_name:
  /* pcr 355313 */
   K_LAYER {driver.lefDumbMode = 1; driver.lefNoNum = 1; } STRING
    { $$ = $3; }

/* 9/11/2001 - Wanda da Rosa.  The following are obsolete in 5.4 */
universalnoisemargin: K_UNIVERSALNOISEMARGIN NUMBER NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*driver.lefrNoiseMarginCbk*/ 1) {
          driver.lefrNoiseMargin.low = $2;
          driver.lefrNoiseMargin.high = $3;
          driver.lefrNoiseMarginCbk( driver.lefrNoiseMargin);
        }
      } else
        if (/*driver.lefrNoiseMarginCbk*/ 1) /* write warning only if cbk is set */
          if (driver.noiseMarginWarnings++ < driver.lefrNoiseMarginWarnings)
            driver.lefWarning(2070, "UNIVERSALNOISEMARGIN statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

edgeratethreshold1: K_EDGERATETHRESHOLD1 NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*lefrEdgeRateThreshold1Cbk*/ 1) {
		  driver.lefrEdgeRateThreshold1Cbk($2);
        }
      } else
        if (/*lefrEdgeRateThreshold1Cbk*/ 1) /* write warning only if cbk is set */
          if (driver.edgeRateThreshold1Warnings++ < driver.lefrEdgeRateThreshold1Warnings)
            driver.lefWarning(2071, "EDGERATETHRESHOLD1 statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

edgeratethreshold2: K_EDGERATETHRESHOLD2 NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*lefrEdgeRateThreshold2Cbk*/ 1) {
		driver.lefrEdgeRateThreshold2Cbk($2);
        }
      } else
        if (/*lefrEdgeRateThreshold2Cbk*/ 1) /* write warning only if cbk is set */
          if (driver.edgeRateThreshold2Warnings++ < driver.lefrEdgeRateThreshold2Warnings)
            driver.lefWarning(2072, "EDGERATETHRESHOLD2 statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

edgeratescalefactor: K_EDGERATESCALEFACTOR NUMBER ';'
    {
      if (driver.versionNum < 5.4) {
        if (/*lefrEdgeRateScaleFactorCbk*/ 1) {
		driver.lefrEdgeRateScaleFactorCbk($2);
        }
      } else
        if (/*lefrEdgeRateScaleFactorCbk*/ 1) /* write warning only if cbk is set */
          if (driver.edgeRateScaleFactorWarnings++ < driver.lefrEdgeRateScaleFactorWarnings)
            driver.lefWarning(2073, "EDGERATESCALEFACTOR statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
    }

noisetable: K_NOISETABLE NUMBER
    { if (/*driver.lefrNoiseTableCbk*/ 1) driver.lefrNoiseTable.lefiNoiseTable::setup((int)$2); }
    ';' noise_table_list end_noisetable dtrm
    { }

end_noisetable:
  K_END K_NOISETABLE
  {
    if (driver.versionNum < 5.4) {
      if (/*driver.lefrNoiseTableCbk*/ 1)
        driver.lefrNoiseTableCbk( driver.lefrNoiseTable);
    } else
      if (/*driver.lefrNoiseTableCbk*/ 1) /* write warning only if cbk is set */
        if (driver.noiseTableWarnings++ < driver.lefrNoiseTableWarnings)
          driver.lefWarning(2074, "NOISETABLE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }


noise_table_list :
  noise_table_entry
  | noise_table_list noise_table_entry
  ;

noise_table_entry:
  K_EDGERATE NUMBER ';'
    { if (/*driver.lefrNoiseTableCbk*/ 1)
         {
            driver.lefrNoiseTable.lefiNoiseTable::newEdge();
            driver.lefrNoiseTable.lefiNoiseTable::addEdge($2);
         }
    }
  | output_resistance_entry
    { }

output_resistance_entry: K_OUTPUTRESISTANCE
    { if (/*driver.lefrNoiseTableCbk*/ 1) driver.lefrNoiseTable.lefiNoiseTable::addResistance(); }
    num_list ';' victim_list
    ;

num_list:
  NUMBER
    { if (/*driver.lefrNoiseTableCbk*/ 1)
    driver.lefrNoiseTable.lefiNoiseTable::addResistanceNumber($1); }
   | num_list NUMBER
    { if (/*driver.lefrNoiseTableCbk*/ 1)
    driver.lefrNoiseTable.lefiNoiseTable::addResistanceNumber($2); }

victim_list:
  victim
  | victim_list victim
  ;

victim: K_VICTIMLENGTH NUMBER ';'
	{ if (/*driver.lefrNoiseTableCbk*/ 1)
	driver.lefrNoiseTable.lefiNoiseTable::addVictimLength($2); }
      K_VICTIMNOISE vnoiselist ';'
	{ }

vnoiselist:
  NUMBER
    { if (/*driver.lefrNoiseTableCbk*/ 1)
    driver.lefrNoiseTable.lefiNoiseTable::addVictimNoise($1); }
  | vnoiselist NUMBER
    { if (/*driver.lefrNoiseTableCbk*/ 1)
    driver.lefrNoiseTable.lefiNoiseTable::addVictimNoise($2); }

correctiontable: K_CORRECTIONTABLE NUMBER ';'
    { if (/*driver.lefrCorrectionTableCbk*/ 1)
    driver.lefrCorrectionTable.lefiCorrectionTable::setup((int)$2); }
    correction_table_list end_correctiontable dtrm
    { }

end_correctiontable:
  K_END K_CORRECTIONTABLE
  {
    if (driver.versionNum < 5.4) {
      if (/*driver.lefrCorrectionTableCbk*/ 1)
        driver.lefrCorrectionTableCbk(driver.lefrCorrectionTable);
    } else
      if (/*driver.lefrCorrectionTableCbk*/ 1) /* write warning only if cbk is set */
        if (driver.correctionTableWarnings++ < driver.lefrCorrectionTableWarnings)
          driver.lefWarning(2075, "CORRECTIONTABLE statement is obsolete in version 5.4 and later.\nThe LEF parser will ignore this statement.\nTo avoid this warning in the future, remove this statement from the LEF file with version 5.4 or later.");
  }

correction_table_list:
  correction_table_item
  | correction_table_list correction_table_item
  ;

correction_table_item:
  K_EDGERATE NUMBER ';'
    { if (/*driver.lefrCorrectionTableCbk*/ 1)
         {
            driver.lefrCorrectionTable.lefiCorrectionTable::newEdge();
            driver.lefrCorrectionTable.lefiCorrectionTable::addEdge($2);
         }
    }
  | output_list
    { }

output_list: K_OUTPUTRESISTANCE
  { if (/*driver.lefrCorrectionTableCbk*/ 1)
  driver.lefrCorrectionTable.lefiCorrectionTable::addResistance(); }
  numo_list ';' corr_victim_list
  { }

numo_list:
  NUMBER
    { if (/*driver.lefrCorrectionTableCbk*/ 1)
    driver.lefrCorrectionTable.lefiCorrectionTable::addResistanceNumber($1); }
  | numo_list NUMBER
    { if (/*driver.lefrCorrectionTableCbk*/ 1)
    driver.lefrCorrectionTable.lefiCorrectionTable::addResistanceNumber($2); }

corr_victim_list:
   corr_victim
   | corr_victim_list corr_victim
   ;

corr_victim:
  K_VICTIMLENGTH NUMBER ';'
     { if (/*driver.lefrCorrectionTableCbk*/ 1)
     driver.lefrCorrectionTable.lefiCorrectionTable::addVictimLength($2); }
  K_CORRECTIONFACTOR corr_list ';'
     { }

corr_list:
  NUMBER
    { if (/*driver.lefrCorrectionTableCbk*/ 1)
	driver.lefrCorrectionTable.lefiCorrectionTable::addVictimCorrection($1); }
  | corr_list NUMBER
    { if (/*driver.lefrCorrectionTableCbk*/ 1)
	driver.lefrCorrectionTable.lefiCorrectionTable::addVictimCorrection($2); }

/* end of 5.4 obsolete syntax */

input_antenna: K_INPUTPINANTENNASIZE NUMBER ';'
    { /* 5.3 syntax */
        driver.use5_3 = 1;
        if (driver.ignoreVersion) {
           /* do nothing */
        } else if (driver.versionNum > 5.3) {
           /* A 5.3 syntax in 5.4 */
           if (driver.use5_4) {
              if (/*lefrInputAntennaCbk*/ 1) { /* write warning only if cbk is set */
                if (driver.inputAntennaWarnings++ < driver.lefrInputAntennaWarnings) {
                   driver.outMsg = (char*)lefMalloc(10000);
                   sprintf (driver.outMsg,
                      "INPUTPINANTENNASIZE statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new INPUTPINANTENNASIZE syntax, which is illegal.", driver.versionNum);
                   driver.lefError(1671, driver.outMsg);
                   lefFree(driver.outMsg);
                   /*CHKERR();*/
                }
              }
           }
        }
        if (/*lefrInputAntennaCbk*/ 1)
          driver.lefrInputAntennaCbk( $2);
    }

output_antenna: K_OUTPUTPINANTENNASIZE NUMBER ';'
    { /* 5.3 syntax */
        driver.use5_3 = 1;
        if (driver.ignoreVersion) {
           /* do nothing */
        } else if (driver.versionNum > 5.3) {
           /* A 5.3 syntax in 5.4 */
           if (driver.use5_4) {
              if (/*lefrOutputAntennaCbk*/ 1) { /* write warning only if cbk is set */
                if (driver.outputAntennaWarnings++ < driver.lefrOutputAntennaWarnings) {
                   driver.outMsg = (char*)lefMalloc(10000);
                   sprintf (driver.outMsg,
                      "OUTPUTPINANTENNASIZE statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new OUTPUTPINANTENNASIZE syntax, which is illegal.", driver.versionNum);
                   driver.lefError(1672, driver.outMsg);
                   lefFree(driver.outMsg);
                   /*CHKERR();*/
                }
              }
           }
        }
        if (/*lefrOutputAntennaCbk*/ 1)
          driver.lefrOutputAntennaCbk( $2);
    }

inout_antenna: K_INOUTPINANTENNASIZE NUMBER ';'
    { /* 5.3 syntax */
        driver.use5_3 = 1;
        if (driver.ignoreVersion) {
           /* do nothing */
        } else if (driver.versionNum > 5.3) {
           /* A 5.3 syntax in 5.4 */
           if (driver.use5_4) {
              if (/*lefrInoutAntennaCbk*/ 1) { /* write warning only if cbk is set */
                if (driver.inoutAntennaWarnings++ < driver.lefrInoutAntennaWarnings) {
                   driver.outMsg = (char*)lefMalloc(10000);
                   sprintf (driver.outMsg,
                      "INOUTPINANTENNASIZE statement is a version 5.3 or earlier syntax.\nYour lef file with version %g, has both old and new INOUTPINANTENNASIZE syntax, which is illegal.", driver.versionNum);
                   driver.lefError(1673, driver.outMsg);
                   lefFree(driver.outMsg);
                   /*CHKERR();*/
                }
              }
           }
        }
        if (/*lefrInoutAntennaCbk*/ 1)
          driver.lefrInoutAntennaCbk( $2);
    }

antenna_input: K_ANTENNAINPUTGATEAREA NUMBER ';'
    { /* 5.4 syntax */
        /* 11/12/2002 - this is obsolete in 5.5, suppose should be ingored */
        /* 12/16/2002 - talked to Dave Noice, leave them in here for debugging*/
        driver.use5_4 = 1;
        if (driver.ignoreVersion) {
           /* do nothing */
        } else if (driver.versionNum < 5.4) {
           if (/*lefrAntennaInputCbk*/ 1) { /* write warning only if cbk is set */
             if (driver.antennaInputWarnings++ < driver.lefrAntennaInputWarnings) {
               driver.outMsg = (char*)lefMalloc(10000);
               sprintf (driver.outMsg,
                  "ANTENNAINPUTGATEAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.\nEither update your VERSION number or use the 5.3 syntax.", driver.versionNum);
               driver.lefError(1674, driver.outMsg);
               lefFree(driver.outMsg);
               /*CHKERR();*/
             }
           }
        } else if (driver.use5_3) {
           if (/*lefrAntennaInputCbk*/ 1) { /* write warning only if cbk is set */
             if (driver.antennaInputWarnings++ < driver.lefrAntennaInputWarnings) {
                driver.outMsg = (char*)lefMalloc(10000);
                sprintf (driver.outMsg,
                   "ANTENNAINPUTGATEAREA statement is a version 5.4 or later syntax.\nYour lef file with version %g, has both old and new ANTENNAINPUTGATEAREA syntax, which is illegal.", driver.versionNum);
                driver.lefError(1675, driver.outMsg);
                lefFree(driver.outMsg);
               /*CHKERR();*/
             }
           }
        }
        if (/*lefrAntennaInputCbk*/ 1)
          driver.lefrAntennaInputCbk( $2);
    }

antenna_inout: K_ANTENNAINOUTDIFFAREA NUMBER ';'
    { /* 5.4 syntax */
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        /* 12/16/2002 - talked to Dave Noice, leave them in here for debugging*/
        driver.use5_4 = 1;
        if (driver.ignoreVersion) {
           /* do nothing */
        } else if (driver.versionNum < 5.4) {
           if (/*lefrAntennaInoutCbk*/ 1) { /* write warning only if cbk is set */
              if (driver.antennaInoutWarnings++ < driver.lefrAntennaInoutWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "ANTENNAINOUTDIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.\nEither update your VERSION number or use the 5.3 syntax.", driver.versionNum);
                 driver.lefError(1676, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        } else if (driver.use5_3) {
           if (/*lefrAntennaInoutCbk*/ 1) { /* write warning only if cbk is set */
              if (driver.antennaInoutWarnings++ < driver.lefrAntennaInoutWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "ANTENNAINOUTDIFFAREA statement is a version 5.4 or later syntax.\nYour lef file with version %g, has both old and new ANTENNAINOUTDIFFAREA syntax, which is illegal.", driver.versionNum);
                 driver.lefError(1677, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        }
        if (/*lefrAntennaInoutCbk*/ 1)
          driver.lefrAntennaInoutCbk( $2);
    }

antenna_output: K_ANTENNAOUTPUTDIFFAREA NUMBER ';'
    { /* 5.4 syntax */
        /* 11/12/2002 - this is obsolete in 5.5, & will be ignored */
        /* 12/16/2002 - talked to Dave Noice, leave them in here for debugging*/
        driver.use5_4 = 1;
        if (driver.ignoreVersion) {
           /* do nothing */
        } else if (driver.versionNum < 5.4) {
           if (/*lefrAntennaOutputCbk*/ 1) { /* write warning only if cbk is set */
              if (driver.antennaOutputWarnings++ < driver.lefrAntennaOutputWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "ANTENNAOUTPUTDIFFAREA statement is a version 5.4 and later syntax.\nYour lef file is defined with version %g.\nEither update your VERSION number or use the 5.3 syntax.", driver.versionNum);
                 driver.lefError(1678, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        } else if (driver.use5_3) {
           if (/*lefrAntennaOutputCbk*/ 1) { /* write warning only if cbk is set */
              if (driver.antennaOutputWarnings++ < driver.lefrAntennaOutputWarnings) {
                 driver.outMsg = (char*)lefMalloc(10000);
                 sprintf (driver.outMsg,
                    "ANTENNAOUTPUTDIFFAREA statement is a version 5.4 or later syntax.\nYour lef file with version %g, has both old and new ANTENNAOUTPUTDIFFAREA syntax, which is illegal.", driver.versionNum);
                 driver.lefError(1679, driver.outMsg);
                 lefFree(driver.outMsg);
                 /*CHKERR();*/
              }
           }
        }
        if (/*lefrAntennaOutputCbk*/ 1)
          driver.lefrAntennaOutputCbk( $2);
    }

extension_opt:  /* empty */
    | extension

extension: K_BEGINEXT
    { 
        if (/*lefrExtensionCbk*/ 1)
          driver.lefrExtensionCbk( driver.Hist_text);
        if (driver.versionNum >= 5.6)
           driver.ge56almostDone = 1;
    }

 /*** END EXAMPLE - Change the example grammar rules above ***/

%% /*** Additional Code ***/

void LefParser::Parser::error(const Parser::location_type& l,
			    const std::string& m)
{
    driver.error(l, m);
}


/* $Id: scanner.ll 44 2008-10-23 09:03:19Z tb $ -*- mode: c++ -*- */
/** \file scanner.ll Define the example Flex lexical scanner */

%{ /*** C/C++ Declarations ***/

#include <string>

#include "LefScanner.h"

/* import the parser's token type into a local typedef */
typedef LefParser::Parser::token token;
typedef LefParser::Parser::token_type token_type;

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
%option prefix="LefParser"

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
 /* (?i:ab) indicates case insensitive = [aA][bB] */

(?i:DEFINE) {return token::K_DEFINE;}
(?i:DEFINEB) {return token::K_DEFINEB;}
(?i:DEFINES) {return token::K_DEFINES;}
(?i:MESSAGE) {return token::K_MESSAGE;}
(?i:CREATEFILE) {return token::K_CREATEFILE;}
(?i:OPENFILE) {return token::K_OPENFILE;}
(?i:CLOSEFILE) {return token::K_CLOSEFILE;}
(?i:WARNING) {return token::K_WARNING;}
(?i:ERROR) {return token::K_ERROR;}
(?i:FATALERROR) {return token::K_FATALERROR;}
(?i:ABOVE) {return token::K_ABOVE;}
(?i:ABUT) {return token::K_ABUT;}
(?i:ABUTMENT) {return token::K_ABUTMENT;}
(?i:ACCURRENTDENSITY) {return token::K_ACCURRENTDENSITY;}
(?i:ACTIVE) {return token::K_ACTIVE;}
(?i:ADJACENTCUTS) {return token::K_ADJACENTCUTS;}
(?i:ANALOG) {return token::K_ANALOG;}
(?i:AND) {return token::K_AND;}
(?i:ANTENNAAREAFACTOR) {return token::K_ANTENNAAREAFACTOR;}
(?i:ANTENNAAREADIFFREDUCEPWL) {return token::K_ANTENNAAREADIFFREDUCEPWL;}
(?i:ANTENNAAREAMINUSDIFF) {return token::K_ANTENNAAREAMINUSDIFF;}
(?i:ANTENNAAREARATIO) {return token::K_ANTENNAAREARATIO;}
(?i:ANTENNACELL) {return token::K_ANTENNACELL;}
(?i:ANTENNACUMAREARATIO) {return token::K_ANTENNACUMAREARATIO;}
(?i:ANTENNACUMDIFFAREARATIO) {return token::K_ANTENNACUMDIFFAREARATIO;}
(?i:ANTENNACUMDIFFSIDEAREARATIO) {return token::K_ANTENNACUMDIFFSIDEAREARATIO;}
(?i:ANTENNACUMROUTINGPLUSCUT) {return token::K_ANTENNACUMROUTINGPLUSCUT;}
(?i:ANTENNACUMSIDEAREARATIO) {return token::K_ANTENNACUMSIDEAREARATIO;}
(?i:ANTENNADIFFAREA) {return token::K_ANTENNADIFFAREA;}
(?i:ANTENNADIFFAREARATIO) {return token::K_ANTENNADIFFAREARATIO;}
(?i:ANTENNADIFFSIDEAREARATIO) {return token::K_ANTENNADIFFSIDEAREARATIO;}
(?i:ANTENNAGATEAREA) {return token::K_ANTENNAGATEAREA;}
(?i:ANTENNAGATEPLUSDIFF) {return token::K_ANTENNAGATEPLUSDIFF;}
(?i:ANTENNAINOUTDIFFAREA) {return token::K_ANTENNAINOUTDIFFAREA;}
(?i:ANTENNAINPUTGATEAREA) {return token::K_ANTENNAINPUTGATEAREA;}
(?i:ANTENNALENGTHFACTOR) {return token::K_ANTENNALENGTHFACTOR;}
(?i:ANTENNAMAXAREACAR) {return token::K_ANTENNAMAXAREACAR;}
(?i:ANTENNAMAXCUTCAR) {return token::K_ANTENNAMAXCUTCAR;}
(?i:ANTENNAMAXSIDEAREACAR) {return token::K_ANTENNAMAXSIDEAREACAR;}
(?i:ANTENNAMETALAREA) {return token::K_ANTENNAMETALAREA;}
(?i:ANTENNAMETALLENGTH) {return token::K_ANTENNAMETALLENGTH;}
(?i:ANTENNAMODEL) {return token::K_ANTENNAMODEL;}
(?i:ANTENNAOUTPUTDIFFAREA) {return token::K_ANTENNAOUTPUTDIFFAREA;}
(?i:ANTENNAPARTIALCUTAREA) {return token::K_ANTENNAPARTIALCUTAREA;}
(?i:ANTENNAPARTIALMETALAREA) {return token::K_ANTENNAPARTIALMETALAREA;}
(?i:ANTENNAPARTIALMETALSIDEAREA) {return token::K_ANTENNAPARTIALMETALSIDEAREA;}
(?i:ANTENNASIDEAREARATIO) {return token::K_ANTENNASIDEAREARATIO;}
(?i:ANTENNASIZE) {return token::K_ANTENNASIZE;}
(?i:ANTENNASIDEAREAFACTOR) {return token::K_ANTENNASIDEAREAFACTOR;}
(?i:ANYEDGE) {return token::K_ANYEDGE;}
(?i:AREA) {return token::K_AREA;}
(?i:AREAIO) {return token::K_AREAIO;}
(?i:ARRAY) {return token::K_ARRAY;}
(?i:ARRAYCUTS) {return token::K_ARRAYCUTS;}
(?i:ARRAYSPACING) {return token::K_ARRAYSPACING;}
(?i:AVERAGE) {return token::K_AVERAGE;}
(?i:BELOW) {return token::K_BELOW;}
(?i:BEGINEXT) {return token::K_BEGINEXT;}
(?i:BLACKBOX) {return token::K_BLACKBOX;}
(?i:BLOCK) {return token::K_BLOCK;}
(?i:BOTTOMLEFT) {return token::K_BOTTOMLEFT;}
(?i:BOTTOMRIGHT) {return token::K_BOTTOMRIGHT;}
(?i:BUMP) {return token::K_BUMP;}
(?i:BUSBITCHARS) {return token::K_BUSBITCHARS;}
(?i:BUFFER) {return token::K_BUFFER;}
(?i:BY) {return token::K_BY;}
(?i:CANNOTOCCUPY) {return token::K_CANNOTOCCUPY;}
(?i:CANPLACE) {return token::K_CANPLACE;}
(?i:CAPACITANCE) {return token::K_CAPACITANCE;}
(?i:CAPMULTIPLIER) {return token::K_CAPMULTIPLIER;}
(?i:CENTERTOCENTER) {return token::K_CENTERTOCENTER;}
(?i:CLASS) {return token::K_CLASS;}
(?i:CLEARANCEMEASURE) {return token::K_CLEARANCEMEASURE;}
(?i:CLOCK) {return token::K_CLOCK;}
(?i:CLOCKTYPE) {return token::K_CLOCKTYPE;}
(?i:COLUMNMAJOR) {return token::K_COLUMNMAJOR;}
(?i:CURRENTDEN) {return token::K_CURRENTDEN;}
(?i:COMPONENTPIN) {return token::K_COMPONENTPIN;}
(?i:CORE) {return token::K_CORE;}
(?i:CORNER) {return token::K_CORNER;}
(?i:CORRECTIONFACTOR) {return token::K_CORRECTIONFACTOR;}
(?i:CORRECTIONTABLE) {return token::K_CORRECTIONTABLE;}
(?i:COVER) {return token::K_COVER;}
(?i:CPERSQDIST) {return token::K_CPERSQDIST;}
(?i:CURRENT) {return token::K_CURRENT;}
(?i:CURRENTSOURCE) {return token::K_CURRENTSOURCE;}
(?i:CUT) {return token::K_CUT;}
(?i:CUTAREA) {return token::K_CUTAREA;}
(?i:CUTSIZE) {return token::K_CUTSIZE;}
(?i:CUTSPACING) {return token::K_CUTSPACING;}
(?i:DATA) {return token::K_DATA;}
(?i:DATABASE) {return token::K_DATABASE;}
(?i:DCCURRENTDENSITY) {return token::K_DCCURRENTDENSITY;}
(?i:DEFAULT) {return token::K_DEFAULT;}
(?i:DEFAULTCAP) {return token::K_DEFAULTCAP;}
(?i:DELAY) {return token::K_DELAY;}
(?i:DENSITY) {return token::K_DENSITY;}
(?i:DENSITYCHECKSTEP) {return token::K_DENSITYCHECKSTEP;}
(?i:DENSITYCHECKWINDOW) {return token::K_DENSITYCHECKWINDOW;}
(?i:DESIGNRULEWIDTH) {return token::K_DESIGNRULEWIDTH;}
(?i:DIAG45) {return token::K_DIAG45;}
(?i:DIAG135) {return token::K_DIAG135;}
(?i:DIAGMINEDGELENGTH) {return token::K_DIAGMINEDGELENGTH;}
(?i:DIAGSPACING) {return token::K_DIAGSPACING;}
(?i:DIAGPITCH) {return token::K_DIAGPITCH;}
(?i:DIAGWIDTH) {return token::K_DIAGWIDTH;}
(?i:DIELECTRIC) {return token::K_DIELECTRIC;}
(?i:DIFFUSEONLY) {return token::K_DIFFUSEONLY;}
(?i:DIRECTION) {return token::K_DIRECTION;}
(?i:DIVIDERCHAR) {return token::K_DIVIDERCHAR;}
(?i:DO) {return token::K_DO;}
(?i:E) {return token::K_E;}
(?i:EDGECAPACITANCE) {return token::K_EDGECAPACITANCE;}
(?i:EDGERATE) {return token::K_EDGERATE;}
(?i:EDGERATESCALEFACTOR) {return token::K_EDGERATESCALEFACTOR;}
(?i:EDGERATETHRESHOLD1) {return token::K_EDGERATETHRESHOLD1;}
(?i:EDGERATETHRESHOLD2) {return token::K_EDGERATETHRESHOLD2;}
(?i:EEQ) {return token::K_EEQ;}
(?i:ELSE) {return token::K_ELSE;}
(?i:ENCLOSURE) {return token::K_ENCLOSURE;}
(?i:END) {return token::K_END;}
(?i:ENDEXT) {return token::K_ENDEXT;}
(?i:ENDCAP) {return token::K_ENDCAP;}
(?i:ENDOFLINE) {return token::K_ENDOFLINE;}
(?i:ENDOFNOTCHWIDTH) {return token::K_ENDOFNOTCHWIDTH;}
(?i:EUCLIDEAN) {return token::K_EUCLIDEAN;}
(?i:EXCEPTEXTRACUT) {return token::K_EXCEPTEXTRACUT;}
(?i:EXCEPTSAMEPGNET) {return token::K_EXCEPTSAMEPGNET;}
(?i:EXCEPTPGNET) {return token::K_EXCEPTPGNET;}
(?i:EXTENSION) {return token::K_EXTENSION;}
(?i:FALL) {return token::K_FALL;}
(?i:FALLCS) {return token::K_FALLCS;}
(?i:FALLRS) {return token::K_FALLRS;}
(?i:FALLSATCUR) {return token::K_FALLSATCUR;}
(?i:FALLSATT1) {return token::K_FALLSATT1;}
(?i:FALLSLEWLIMIT) {return token::K_FALLSLEWLIMIT;}
(?i:FALLT0) {return token::K_FALLT0;}
(?i:FALLTHRESH) {return token::K_FALLTHRESH;}
(?i:FALLVOLTAGETHRESHOLD) {return token::K_FALLVOLTAGETHRESHOLD;}
(?i:FALSE) {return token::K_FALSE;}
(?i:FE) {return token::K_FE;}
(?i:FEEDTHRU) {return token::K_FEEDTHRU;}
(?i:FILLACTIVESPACING) {return token::K_FILLACTIVESPACING;}
(?i:FIXED) {return token::K_FIXED;}
(?i:FLIP) {return token::K_FLIP;}
(?i:FLOORPLAN) {return token::K_FLOORPLAN;}
(?i:FN) {return token::K_FN;}
(?i:FOREIGN) {return token::K_FOREIGN;}
(?i:FREQUENCY) {return token::K_FREQUENCY;}
(?i:FROMABOVE) {return token::K_FROMABOVE;}
(?i:FROMBELOW) {return token::K_FROMBELOW;}
(?i:FROMPIN) {return token::K_FROMPIN;}
(?i:FUNCTION) {return token::K_FUNCTION;}
(?i:FS) {return token::K_FS;}
(?i:FW) {return token::K_FW;}
(?i:GCELLGRID) {return token::K_GCELLGRID;}
(?i:GENERATE) {return token::K_GENERATE;}
(?i:GENERATED) {return token::K_GENERATED;}
(?i:GENERATOR) {return token::K_GENERATOR;}
(?i:GROUND) {return token::K_GROUND;}
(?i:GROUNDSENSITIVITY) {return token::K_GROUNDSENSITIVITY;}
(?i:HARDSPACING) {return token::K_HARDSPACING;}
(?i:HEIGHT) {return token::K_HEIGHT;}
(?i:HISTORY) {return token::K_HISTORY;}
(?i:HOLD) {return token::K_HOLD;}
(?i:HORIZONTAL) {return token::K_HORIZONTAL;}
(?i:IF) {return token::K_IF;}
(?i:IMPLANT) {return token::K_IMPLANT;}
(?i:INFLUENCE) {return token::K_INFLUENCE;}
(?i:INOUT) {return token::K_INOUT;}
(?i:INOUTPINANTENNASIZE) {return token::K_INOUTPINANTENNASIZE;}
(?i:INPUT) {return token::K_INPUT;}
(?i:INPUTPINANTENNASIZE) {return token::K_INPUTPINANTENNASIZE;}
(?i:INPUTNOISEMARGIN) {return token::K_INPUTNOISEMARGIN;}
(?i:INSIDECORNER) {return token::K_INSIDECORNER;}
(?i:INTEGER) {return token::K_INTEGER;}
(?i:INTRINSIC) {return token::K_INTRINSIC;}
(?i:INVERT) {return token::K_INVERT;}
(?i:INVERTER) {return token::K_INVERTER;}
(?i:IRDROP) {return token::K_IRDROP;}
(?i:ITERATE) {return token::K_ITERATE;}
(?i:IV_TABLES) {return token::K_IV_TABLES;}
(?i:LAYER) {return token::K_LAYER;}
(?i:LAYERS) {return token::K_LAYERS;}
(?i:LEAKAGE) {return token::K_LEAKAGE;}
(?i:LENGTH) {return token::K_LENGTH;}
(?i:LENGTHSUM) {return token::K_LENGTHSUM;}
(?i:LENGTHTHRESHOLD) {return token::K_LENGTHTHRESHOLD;}
(?i:LEQ) {return token::K_LEQ;}
(?i:LIBRARY) {return token::K_LIBRARY;}
(?i:LONGARRAY) {return token::K_LONGARRAY;}
(?i:MACRO) {return token::K_MACRO;}
(?i:MANUFACTURINGGRID) {return token::K_MANUFACTURINGGRID;}
(?i:MASTERSLICE) {return token::K_MASTERSLICE;}
(?i:MATCH) {return token::K_MATCH;}
(?i:MAXADJACENTSLOTSPACING) {return token::K_MAXADJACENTSLOTSPACING;}
(?i:MAXCOAXIALSLOTSPACING) {return token::K_MAXCOAXIALSLOTSPACING;}
(?i:MAXDELAY) {return token::K_MAXDELAY;}
(?i:MAXEDGES) {return token::K_MAXEDGES;}
(?i:MAXEDGESLOTSPACING) {return token::K_MAXEDGESLOTSPACING;}
(?i:MAXLOAD) {return token::K_MAXLOAD;}
(?i:MAXIMUMDENSITY) {return token::K_MAXIMUMDENSITY;}
(?i:MAXVIASTACK) {return token::K_MAXVIASTACK;}
(?i:MAXWIDTH) {return token::K_MAXWIDTH;}
(?i:MAXXY) {return token::K_MAXXY;}
(?i:MEGAHERTZ) {return token::K_MEGAHERTZ;}
(?i:METALOVERHANG) {return token::K_METALOVERHANG;}
(?i:MICRONS) {return token::K_MICRONS;}
(?i:MILLIAMPS) {return token::K_MILLIAMPS;}
(?i:MILLIWATTS) {return token::K_MILLIWATTS;}
(?i:MINCUTS) {return token::K_MINCUTS;}
(?i:MINENCLOSEDAREA) {return token::K_MINENCLOSEDAREA;}
(?i:MINFEATURE) {return token::K_MINFEATURE;}
(?i:MINIMUMCUT) {return token::K_MINIMUMCUT;}
(?i:MINIMUMDENSITY) {return token::K_MINIMUMDENSITY;}
(?i:MINPINS) {return token::K_MINPINS;}
(?i:MINSIZE) {return token::K_MINSIZE;}
(?i:MINSTEP) {return token::K_MINSTEP;}
(?i:MINWIDTH) {return token::K_MINWIDTH;}
(?i:MPWH) {return token::K_MPWH;}
(?i:MPWL) {return token::K_MPWL;}
(?i:MUSTJOIN) {return token::K_MUSTJOIN;}
(?i:MX) {return token::K_MX;}
(?i:MY) {return token::K_MY;}
(?i:MXR90) {return token::K_MXR90;}
(?i:MYR90) {return token::K_MYR90;}
(?i:N) {return token::K_N;}
(?i:NAMEMAPSTRING) {return token::K_NAMEMAPSTRING;}
(?i:NAMESCASESENSITIVE) {return token::K_NAMESCASESENSITIVE;}
(?i:NANOSECONDS) {return token::K_NANOSECONDS;}
(?i:NEGEDGE) {return token::K_NEGEDGE;}
(?i:NETEXPR) {return token::K_NETEXPR;}
(?i:NETS) {return token::K_NETS;}
(?i:NEW) {return token::K_NEW;}
(?i:NONDEFAULTRULE) {return token::K_NONDEFAULTRULE;}
(?i:NONE) {return token::K_NONE;}
(?i:NONINVERT) {return token::K_NONINVERT;}
(?i:NONUNATE) {return token::K_NONUNATE;}
(?i:NOISETABLE) {return token::K_NOISETABLE;}
(?i:NOTCHLENGTH) {return token::K_NOTCHLENGTH;}
(?i:NOTCHSPACING) {return token::K_NOTCHSPACING;}
(?i:NOWIREEXTENSIONATPIN) {return token::K_NOWIREEXTENSIONATPIN;}
(?i:OBS) {return token::K_OBS;}
(?i:OFF) {return token::K_OFF;}
(?i:OFFSET) {return token::K_OFFSET;}
(?i:OHMS) {return token::K_OHMS;}
(?i:ON) {return token::K_ON;}
(?i:OR) {return token::K_OR;}
(?i:ORIENT) {return token::K_ORIENT;}
(?i:ORIENTATION) {return token::K_ORIENTATION;}
(?i:ORIGIN) {return token::K_ORIGIN;}
(?i:ORTHOGONAL) {return token::K_ORTHOGONAL;}
(?i:OUTPUT) {return token::K_OUTPUT;}
(?i:OUTPUTPINANTENNASIZE) {return token::K_OUTPUTPINANTENNASIZE;}
(?i:OUTPUTNOISEMARGIN) {return token::K_OUTPUTNOISEMARGIN;}
(?i:OUTPUTRESISTANCE) {return token::K_OUTPUTRESISTANCE;}
(?i:OUTSIDECORNER) {return token::K_OUTSIDECORNER;}
(?i:OVERHANG) {return token::K_OVERHANG;}
(?i:OVERLAP) {return token::K_OVERLAP;}
(?i:OVERLAPS) {return token::K_OVERLAPS;}
(?i:OXIDE1) {return token::K_OXIDE1;}
(?i:OXIDE2) {return token::K_OXIDE2;}
(?i:OXIDE3) {return token::K_OXIDE3;}
(?i:OXIDE4) {return token::K_OXIDE4;}
(?i:PAD) {return token::K_PAD;}
(?i:PARALLELEDGE) {return token::K_PARALLELEDGE;}
(?i:PARALLELOVERLAP) {return token::K_PARALLELOVERLAP;}
(?i:PARALLELRUNLENGTH) {return token::K_PARALLELRUNLENGTH;}
(?i:PATH) {return token::K_PATH;}
(?i:PATTERN) {return token::K_PATTERN;}
(?i:PEAK) {return token::K_PEAK;}
(?i:PERIOD) {return token::K_PERIOD;}
(?i:PGONLY) {return token::K_PGONLY;}
(?i:PICOFARADS) {return token::K_PICOFARADS;}
(?i:PIN) {return token::K_PIN;}
(?i:PITCH) {return token::K_PITCH;}
(?i:PLACED) {return token::K_PLACED;}
(?i:POLYGON) {return token::K_POLYGON;}
(?i:PORT) {return token::K_PORT;}
(?i:POSEDGE) {return token::K_POSEDGE;}
(?i:POST) {return token::K_POST;}
(?i:POWER) {return token::K_POWER;}
(?i:PRE) {return token::K_PRE;}
(?i:PREFERENCLOSURE) {return token::K_PREFERENCLOSURE;}
(?i:PRL) {return token::K_PRL;}
(?i:PROPERTY) {return token::K_PROPERTY;}
(?i:PROPERTYDEFINITIONS) {return token::K_PROPERTYDEFINITIONS;}
(?i:PROTRUSIONWIDTH) {return token::K_PROTRUSIONWIDTH;}
(?i:PULLDOWNRES) {return token::K_PULLDOWNRES;}
(?i:PWL) {return token::K_PWL;}
(?i:R0) {return token::K_R0;}
(?i:R90) {return token::K_R90;}
(?i:R180) {return token::K_R180;}
(?i:R270) {return token::K_R270;}
(?i:RANGE) {return token::K_RANGE;}
(?i:REAL) {return token::K_REAL;}
(?i:RECOVERY) {return token::K_RECOVERY;}
(?i:RECT) {return token::K_RECT;}
(?i:RESISTANCE) {return token::K_RESISTANCE;}
(?i:RESISTIVE) {return token::K_RESISTIVE;}
(?i:RING) {return token::K_RING;}
(?i:RISE) {return token::K_RISE;}
(?i:RISECS) {return token::K_RISECS;}
(?i:RISERS) {return token::K_RISERS;}
(?i:RISESATCUR) {return token::K_RISESATCUR;}
(?i:RISESATT1) {return token::K_RISESATT1;}
(?i:RISESLEWLIMIT) {return token::K_RISESLEWLIMIT;}
(?i:RISET0) {return token::K_RISET0;}
(?i:RISETHRESH) {return token::K_RISETHRESH;}
(?i:RISEVOLTAGETHRESHOLD) {return token::K_RISEVOLTAGETHRESHOLD;}
(?i:RMS) {return token::K_RMS;}
(?i:ROUTING) {return token::K_ROUTING;}
(?i:ROWABUTSPACING) {return token::K_ROWABUTSPACING;}
(?i:ROWCOL) {return token::K_ROWCOL;}
(?i:ROWMAJOR) {return token::K_ROWMAJOR;}
(?i:ROWMINSPACING) {return token::K_ROWMINSPACING;}
(?i:ROWPATTERN) {return token::K_ROWPATTERN;}
(?i:RPERSQ) {return token::K_RPERSQ;}
(?i:S) {return token::K_S;}
(?i:SAMENET) {return token::K_SAMENET;}
(?i:SCANUSE) {return token::K_SCANUSE;}
(?i:SDFCOND) {return token::K_SDFCOND;}
(?i:SDFCONDEND) {return token::K_SDFCONDEND;}
(?i:SDFCONDSTART) {return token::K_SDFCONDSTART;}
(?i:SETUP) {return token::K_SETUP;}
(?i:SHAPE) {return token::K_SHAPE;}
(?i:SHRINKAGE) {return token::K_SHRINKAGE;}
(?i:SIGNAL) {return token::K_SIGNAL;}
(?i:SITE) {return token::K_SITE;}
(?i:SIZE) {return token::K_SIZE;}
(?i:SKEW) {return token::K_SKEW;}
(?i:SLOTLENGTH) {return token::K_SLOTLENGTH;}
(?i:SLOTWIDTH) {return token::K_SLOTWIDTH;}
(?i:SLOTWIRELENGTH) {return token::K_SLOTWIRELENGTH;}
(?i:SLOTWIREWIDTH) {return token::K_SLOTWIREWIDTH;}
(?i:SPLITWIREWIDTH) {return token::K_SPLITWIREWIDTH;}
(?i:SOFT) {return token::K_SOFT;}
(?i:SOURCE) {return token::K_SOURCE;}
(?i:SPACER) {return token::K_SPACER;}
(?i:SPACING) {return token::K_SPACING;}
(?i:SPACINGTABLE) {return token::K_SPACINGTABLE;}
(?i:SPECIALNETS) {return token::K_SPECIALNETS;}
(?i:STABLE) {return token::K_STABLE;}
(?i:STACK) {return token::K_STACK;}
(?i:START) {return token::K_START;}
(?i:STEP) {return token::K_STEP;}
(?i:STOP) {return token::K_STOP;}
(?i:STRING) {return token::K_STRING;}
(?i:STRUCTURE) {return token::K_STRUCTURE;}
(?i:SUPPLYSENSITIVITY) {return token::K_SUPPLYSENSITIVITY;}
(?i:SYMMETRY) {return token::K_SYMMETRY;}
(?i:TABLE) {return token::K_TABLE;}
(?i:TABLEAXIS) {return token::K_TABLEAXIS;}
(?i:TABLEDIMENSION) {return token::K_TABLEDIMENSION;}
(?i:TABLEENTRIES) {return token::K_TABLEENTRIES;}
(?i:TAPERRULE) {return token::K_TAPERRULE;}
(?i:THEN) {return token::K_THEN;}
(?i:THICKNESS) {return token::K_THICKNESS;}
(?i:TIEHIGH) {return token::K_TIEHIGH;}
(?i:TIELOW) {return token::K_TIELOW;}
(?i:TIEOFFR) {return token::K_TIEOFFR;}
(?i:TIME) {return token::K_TIME;}
(?i:TIMING) {return token::K_TIMING;}
(?i:TO) {return token::K_TO;}
(?i:TOPIN) {return token::K_TOPIN;}
(?i:TOPLEFT) {return token::K_TOPLEFT;}
(?i:TOPOFSTACKONLY) {return token::K_TOPOFSTACKONLY;}
(?i:TOPRIGHT) {return token::K_TOPRIGHT;}
(?i:TRACKS) {return token::K_TRACKS;}
(?i:TRANSITIONTIME) {return token::K_TRANSITIONTIME;}
(?i:TRISTATE) {return token::K_TRISTATE;}
(?i:TRUE) {return token::K_TRUE;}
(?i:TWOEDGES) {return token::K_TWOEDGES;}
(?i:TWOWIDTHS) {return token::K_TWOWIDTHS;}
(?i:TYPE) {return token::K_TYPE;}
(?i:UNATENESS) {return token::K_UNATENESS;}
(?i:UNITS) {return token::K_UNITS;}
(?i:UNIVERSALNOISEMARGIN) {return token::K_UNIVERSALNOISEMARGIN;}
(?i:USE) {return token::K_USE;}
(?i:USELENGTHTHRESHOLD) {return token::K_USELENGTHTHRESHOLD;}
(?i:USEMINSPACING) {return token::K_USEMINSPACING;}
(?i:USER) {return token::K_USER;}
(?i:USEVIA) {return token::K_USEVIA;}
(?i:USEVIARULE) {return token::K_USEVIARULE;}
(?i:VARIABLE) {return token::K_VARIABLE;}
(?i:VERSION) {return token::K_VERSION;}
(?i:VERTICAL) {return token::K_VERTICAL;}
(?i:VHI) {return token::K_VHI;}
(?i:VIA) {return token::K_VIA;}
(?i:VIARULE) {return token::K_VIARULE;}
(?i:VICTIMLENGTH) {return token::K_VICTIMLENGTH;}
(?i:VICTIMNOISE) {return token::K_VICTIMNOISE;}
(?i:VIRTUAL) {return token::K_VIRTUAL;}
(?i:VLO) {return token::K_VLO;}
(?i:VOLTAGE) {return token::K_VOLTAGE;}
(?i:VOLTS) {return token::K_VOLTS;}
(?i:W) {return token::K_W;}
(?i:WELLTAP) {return token::K_WELLTAP;}
(?i:WIDTH) {return token::K_WIDTH;}
(?i:WITHIN) {return token::K_WITHIN;}
(?i:WIRECAP) {return token::K_WIRECAP;}
(?i:WIREEXTENSION) {return token::K_WIREEXTENSION;}
(?i:X) {return token::K_X;}
(?i:Y) {return token::K_Y;}

[\+\-]?[0-9]+ {
    yylval->integerVal = atoi(yytext);
    return token::INTEGER;
}

[\+\-]?[0-9]+("."[0-9]*)?([eE][\+\-]?[0-9]+)? {
    yylval->doubleVal = atof(yytext);
    return token::DOUBLE;
}

[0-9]*\'[A-Za-z][0-9]+ {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::BINARY;
}

[A-Za-z0-9][A-Za-z0-9_,\.\-\[\]]* {
    yylval->stringVal = new std::string(yytext, yyleng);
    return token::STRING;
}

\"([^"])*\" {
    yylval->qstringVal = new std::string(yytext+1, yyleng-2);
    return token::QSTRING;
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

namespace LefParser {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : LefParserFlexLexer(in, out)
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

int LefParserFlexLexer::yylex()
{
    std::cerr << "in LefParserFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int LefParserFlexLexer::yywrap()
{
    return 1;
}

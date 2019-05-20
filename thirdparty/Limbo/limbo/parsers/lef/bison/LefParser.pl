#########################################################################
# File Name: LefParser.pl
# Author: Yibo Lin
# mail: yibolin@utexas.edu
# Created Time: Thu 11 Dec 2014 01:09:01 PM CST
#########################################################################
#!/bin/bash

# this script generates basic .ll and .yy files 
# be careful to backup existing files to avoid overwritting 

use strict;
use warnings;

my $parser_prefix = "Lef";
# define keyword array 
# add additional keywords here 
my @vkeywords = (
	"DEFINE",	
	"DEFINEB",	
	"DEFINES",	
	"MESSAGE",	
	"CREATEFILE", 
	"OPENFILE",	
	"CLOSEFILE",	
	"WARNING",	
	"ERROR",	
	"FATALERROR", 
	"namescasesensitive",
	"off",	
	"on", 	
	"ABOVE",	
	"ABUT",	
	"ABUTMENT",	
	"ACCURRENTDENSITY",
	"ACTIVE",	
	"ADJACENTCUTS", 
	"ANALOG",	
	"AND",	
	"ANTENNAAREAFACTOR", 
	"ANTENNAAREADIFFREDUCEPWL", 
	"ANTENNAAREAMINUSDIFF", 
	"ANTENNAAREARATIO",
	"ANTENNACELL", 
	"ANTENNACUMAREARATIO",
	"ANTENNACUMDIFFAREARATIO",
	"ANTENNACUMDIFFSIDEAREARATIO",
	"ANTENNACUMROUTINGPLUSCUT",
	"ANTENNACUMSIDEAREARATIO",
	"ANTENNADIFFAREA",
	"ANTENNADIFFAREARATIO",
	"ANTENNADIFFSIDEAREARATIO",
	"ANTENNAGATEAREA",
	"ANTENNAGATEPLUSDIFF",
	"ANTENNAINOUTDIFFAREA",
	"ANTENNAINPUTGATEAREA",
	"ANTENNALENGTHFACTOR", 
	"ANTENNAMAXAREACAR",
	"ANTENNAMAXCUTCAR",
	"ANTENNAMAXSIDEAREACAR",
	"ANTENNAMETALAREA",
	"ANTENNAMETALLENGTH",
	"ANTENNAMODEL", 
	"ANTENNAOUTPUTDIFFAREA",
	"ANTENNAPARTIALCUTAREA",
	"ANTENNAPARTIALMETALAREA",
	"ANTENNAPARTIALMETALSIDEAREA",
	"ANTENNASIDEAREARATIO",
	"ANTENNASIZE", 
	"ANTENNASIDEAREAFACTOR",
	"ANYEDGE",    
	"AREA",	
	"AREAIO",     
	"ARRAY",	
	"ARRAYCUTS",	
	"ARRAYSPACING", 
	"AVERAGE",    
	"BELOW",	
	"BEGINEXT",	
	"BLACKBOX",   
	"BLOCK",	
	"BOTTOMLEFT",	
	"BOTTOMRIGHT", 
	"BUMP",       
	"BUSBITCHARS", 
	"BUFFER",	
	"BY",		
	"CANNOTOCCUPY", 
	"CANPLACE",	
	"CAPACITANCE", 
	"CAPMULTIPLIER", 
	"CENTERTOCENTER", 
	"CLASS",	
	"CLEARANCEMEASURE",
	"CLOCK",	
	"CLOCKTYPE",	
	"COLUMNMAJOR", 
	"CURRENTDEN",	
	"COMPONENTPIN", 
	"CORE",	
	"CORNER",	
	"CORRECTIONFACTOR",
	"CORRECTIONTABLE",
	"COVER",	
	"CPERSQDIST",	
	"CURRENT",	
	"CURRENTSOURCE", 
	"CUT",	
	"CUTAREA",    
	"CUTSIZE",    
	"CUTSPACING",    
	"DATA",    	
	"DATABASE",	
	"DCCURRENTDENSITY",
	"DEFAULT",	
	"DEFAULTCAP",	
	"DELAY",	
	"DENSITY", 
	"DENSITYCHECKSTEP",
	"DENSITYCHECKWINDOW",
	"DESIGNRULEWIDTH",
	"DIAG45", 
	"DIAG135", 
	"DIAGMINEDGELENGTH",
	"DIAGSPACING", 
	"DIAGPITCH", 
	"DIAGWIDTH",	
	"DIELECTRIC",	
	"DIFFUSEONLY", 
	"DIRECTION",	
	"DIVIDERCHAR", 
	"DO",		
	"E",      	
	"EDGECAPACITANCE",
	"EDGERATE",   
	"EDGERATESCALEFACTOR",
	"EDGERATETHRESHOLD1",
	"EDGERATETHRESHOLD2",
	"EEQ",	
	"ELSE",	
	"ENCLOSURE",  
	"END",	
	"ENDEXT",	
	"ENDCAP",	
	"ENDOFLINE",	
	"ENDOFNOTCHWIDTH",
	"EUCLIDEAN",  
	"EXCEPTEXTRACUT", 
	"EXCEPTSAMEPGNET",
	"EXCEPTPGNET", 
	"EXTENSION",	
	"FALL",	
	"FALLCS",	
	"FALLRS",	
	"FALLSATCUR",	
	"FALLSATT1",	
	"FALLSLEWLIMIT", 
	"FALLT0", 	
	"FALLTHRESH",	
	"FALLVOLTAGETHRESHOLD",	
	"FALSE",	
	"FE", 	
	"FEEDTHRU",	
	"FILLACTIVESPACING",
	"FIXED",	
	"FLIP",       
	"FLOORPLAN",	
	"FN", 	
	"FOREIGN",	
	"FREQUENCY",  
	"FROMABOVE",  
	"FROMBELOW",  
	"FROMPIN",	
	"FUNCTION",	
	"FS", 	
	"FW", 	
	"GCELLGRID",	
	"GENERATE",	
	"GENERATED",	
	"GENERATOR",	
	"GROUND",	
	"GROUNDSENSITIVITY",
	"HARDSPACING", 
	"HEIGHT",	
	"HISTORY", 	
	"HOLD",	
	"HORIZONTAL",	
	"IF",		
	"IMPLANT",    
	"INFLUENCE",  
	"INOUT",	
	"INOUTPINANTENNASIZE",
	"INPUT",	
	"INPUTPINANTENNASIZE",
	"INPUTNOISEMARGIN",
	"INSIDECORNER", 
	"INTEGER",	
	"INTRINSIC",	
	"INVERT",	
	"INVERTER",	
	"IRDROP",	
	"ITERATE",	
	"IV_TABLES",	
	"LAYER",	
	"LAYERS",	
	"LEAKAGE",	
	"LENGTH",     
	"LENGTHSUM",  
	"LENGTHTHRESHOLD",
	"LEQ",	
	"LIBRARY",	
	"LONGARRAY",	
	"MACRO",	
	"MANUFACTURINGGRID",
	"MASTERSLICE", 
	"MATCH",	
	"MAXADJACENTSLOTSPACING",
	"MAXCOAXIALSLOTSPACING",
	"MAXDELAY",	
	"MAXEDGES", 
	"MAXEDGESLOTSPACING",
	"MAXLOAD",	
	"MAXIMUMDENSITY", 
	"MAXVIASTACK", 
	"MAXWIDTH",   
	"MAXXY",      
	"MEGAHERTZ",  
	"METALOVERHANG", 
	"MICRONS",	
	"MILLIAMPS",	
	"MILLIWATTS",	
	"MINCUTS",	
	"MINENCLOSEDAREA",
	"MINFEATURE",	
	"MINIMUMCUT", 
	"MINIMUMDENSITY", 
	"MINPINS",	
	"MINSIZE",    
	"MINSTEP",    
	"MINWIDTH",   
	"MPWH",       
	"MPWL",       
	"MUSTJOIN",	
	"MX",     	
	"MY",	        
	"MXR90",	
	"MYR90",	
	"N",        	
	"NAMEMAPSTRING", 
	"NAMESCASESENSITIVE",
	"NANOSECONDS", 
	"NEGEDGE",    
	"NETEXPR",	
	"NETS",	
	"NEW",	
	"NONDEFAULTRULE", 
	"NONE",       
	"NONINVERT",	
	"NONUNATE",	
	"NOISETABLE", 
	"NOTCHLENGTH", 
	"NOTCHSPACING", 
	"NOWIREEXTENSIONATPIN",
	"OBS",	
	"OFF",	
	"OFFSET",     
	"OHMS",	
	"ON",		
	"OR",		
	"ORIENT",     
	"ORIENTATION", 
	"ORIGIN",	
	"ORTHOGONAL",	
	"OUTPUT",	
	"OUTPUTPINANTENNASIZE",
	"OUTPUTNOISEMARGIN",
	"OUTPUTRESISTANCE",
	"OUTSIDECORNER", 
	"OVERHANG",	
	"OVERLAP",	
	"OVERLAPS",	
	"OXIDE1",     
	"OXIDE2",     
	"OXIDE3",     
	"OXIDE4",     
	"PAD",	
	"PARALLELEDGE", 
	"PARALLELOVERLAP",
	"PARALLELRUNLENGTH",
	"PATH",	
	"PATTERN",	
	"PEAK",       
	"PERIOD",     
	"PGONLY",     
	"PICOFARADS",	
	"PIN",	
	"PITCH",	
	"PLACED",	
	"POLYGON",	
	"PORT",	
	"POSEDGE",    
	"POST",	
	"POWER",	
	"PRE",	
	"PREFERENCLOSURE",
	"PRL",        
	"PROPERTY",	
	"PROPERTYDEFINITIONS", 
	"PROTRUSIONWIDTH",
	"PULLDOWNRES", 
	"PWL",	
	"R0", 	
	"R90",	
	"R180",	
	"R270",	
	"RANGE",	
	"REAL",	
	"RECOVERY",   
	"RECT",	
	"RESISTANCE",	
	"RESISTIVE",	
	"RING",	
	"RISE",	
	"RISECS",	
	"RISERS",	
	"RISESATCUR",	
	"RISESATT1",	
	"RISESLEWLIMIT", 
	"RISET0",    	
	"RISETHRESH",	
	"RISEVOLTAGETHRESHOLD",	
	"RMS",        
	"ROUTING",	
	"ROWABUTSPACING", 
	"ROWCOL",	
	"ROWMAJOR",	
	"ROWMINSPACING", 
	"ROWPATTERN", 
	"RPERSQ",	
	"S",   	
	"SAMENET",	
	"SCANUSE",	
	"SDFCOND",    
	"SDFCONDEND", 
	"SDFCONDSTART", 
	"SETUP",	
	"SHAPE",	
	"SHRINKAGE",	
	"SIGNAL",	
	"SITE",	
	"SIZE",	
	"SKEW",       
	"SLOTLENGTH", 
	"SLOTWIDTH", 
	"SLOTWIRELENGTH", 
	"SLOTWIREWIDTH", 
	"SPLITWIREWIDTH", 
	"SOFT",	
	"SOURCE",	
	"SPACER",	
	"SPACING",	
	"SPACINGTABLE", 
	"SPECIALNETS", 
	"STABLE",	
	"STACK",	
	"START",	
	"STEP",	
	"STOP",	
	"STRING",	
	"STRUCTURE",	
	"SUPPLYSENSITIVITY",
	"SYMMETRY",	
	"TABLE",	
	"TABLEAXIS",	
	"TABLEDIMENSION", 
	"TABLEENTRIES", 
	"TAPERRULE",	
	"THEN",	
	"THICKNESS",	
	"TIEHIGH",	
	"TIELOW",	
	"TIEOFFR",	
	"TIME",	
	"TIMING",	
	"TO",		
	"TOPIN",	
	"TOPLEFT",	
	"TOPOFSTACKONLY", 
	"TOPRIGHT",	
	"TRACKS",	
	"TRANSITIONTIME", 
	"TRISTATE",	
	"TRUE",	
	"TWOEDGES",	
	"TWOWIDTHS",	
	"TYPE",	
	"UNATENESS",	
	"UNITS",	
	"UNIVERSALNOISEMARGIN",
	"USE",	
	"USELENGTHTHRESHOLD",
	"USEMINSPACING", 
	"USER",	
	"USEVIA",	
	"USEVIARULE",	
	"VARIABLE",	
	"VERSION",	
	"VERTICAL",	
	"VHI",	
	"VIA",	
	"VIARULE",	
	"VICTIMLENGTH", 
	"VICTIMNOISE",  
	"VIRTUAL",	
	"VLO",	
	"VOLTAGE",	
	"VOLTS",	
	"W",	
	"WELLTAP",	
	"WIDTH",	
	"WITHIN",     
	"WIRECAP",	
	"WIREEXTENSION", 
	"X",	        
	"Y",      	
);

&generate_scanner($parser_prefix, \@vkeywords);
&generate_parser($parser_prefix, \@vkeywords);

###############################################################################
# generate lexical scanner .ll file 
sub generate_scanner
{
	my $prefix = $_[0];
	my @keywords = @{$_[1]};
	my $filename = "${prefix}Scanner.ll";

	open(my $scanner_handle, '>', $filename) or die "Could not open file '$filename' $!";
	
	################# write header #################

	print $scanner_handle "/* \$Id: scanner.ll 44 2008-10-23 09:03:19Z tb \$ -*- mode: c++ -*- */\n";
	print $scanner_handle "/** \\file scanner.ll Define the example Flex lexical scanner */\n";
	print $scanner_handle "\n";
	print $scanner_handle "%{ /*** C/C++ Declarations ***/\n";
	print $scanner_handle "\n";
	print $scanner_handle "#include <string>\n";
	print $scanner_handle "\n";
	print $scanner_handle "#include \"${prefix}Scanner.h\"\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* import the parser's token type into a local typedef */\n";
	print $scanner_handle "typedef ${prefix}Parser::Parser::token token;\n";
	print $scanner_handle "typedef ${prefix}Parser::Parser::token_type token_type;\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* By default yylex returns int, we use token_type. Unfortunately yyterminate\n";
	print $scanner_handle " * by default returns 0, which is not of token_type. */\n";
	print $scanner_handle "#define yyterminate() return token::END\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* This disables inclusion of unistd.h, which is not available under Visual C++\n";
	print $scanner_handle " * on Win32. The C++ scanner uses STL streams instead. */\n";
	print $scanner_handle "#define YY_NO_UNISTD_H\n";
	print $scanner_handle "\n";
	print $scanner_handle "%}\n";
	print $scanner_handle "\n";
	print $scanner_handle "/*** Flex Declarations and Options ***/\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* enable c++ scanner class generation */\n";
	print $scanner_handle "%option c++\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* change the name of the scanner class. results in \"ExampleFlexLexer\" */\n";
	print $scanner_handle "%option prefix=\"${prefix}Parser\"\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* the manual says \"somewhat more optimized\" */\n";
	print $scanner_handle "%option batch\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* enable scanner to generate debug output. disable this for release\n";
	print $scanner_handle " * versions. */\n";
	print $scanner_handle "%option debug\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* no support for include files is planned */\n";
	print $scanner_handle "%option yywrap nounput \n";
	print $scanner_handle "\n";
	print $scanner_handle "/* enables the use of start condition stacks */\n";
	print $scanner_handle "%option stack\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* The following paragraph suffices to track locations accurately. Each time\n";
	print $scanner_handle " * yylex is invoked, the begin position is moved onto the end position. */\n";
	print $scanner_handle "%{\n";
	print $scanner_handle "#define YY_USER_ACTION  yylloc->columns(yyleng);\n";
	print $scanner_handle "%}\n";
	print $scanner_handle "\n";
	print $scanner_handle "%% /*** Regular Expressions Part ***/\n";
	print $scanner_handle "\n";
	print $scanner_handle " /* code to place at the beginning of yylex() */\n";
	print $scanner_handle "%{\n";
	print $scanner_handle "    // reset location\n";
	print $scanner_handle "    yylloc->step();\n";
	print $scanner_handle "%}\n";
	print $scanner_handle "\n";

	################# write keywords #################
	print $scanner_handle " /*** BEGIN EXAMPLE - Change the example lexer rules below ***/\n";

	foreach my $keyword (@keywords)
	{
		print $scanner_handle "\"$keyword\" \{return token::K_$keyword;\}\n";
	}

	################# write additional tokens #################
	print $scanner_handle "\n";
	print $scanner_handle "[\\+\\-]?[0-9]+ {\n";
	print $scanner_handle "    yylval->integerVal = atoi(yytext);\n";
	print $scanner_handle "    return token::INTEGER;\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "[\\+\\-]?[0-9]+\".\"[0-9]* {\n";
	print $scanner_handle "    yylval->doubleVal = atof(yytext);\n";
	print $scanner_handle "    return token::DOUBLE;\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "[0-9]*\\'[A-Za-z][0-9]+ {\n";
	print $scanner_handle "    yylval->stringVal = new std::string(yytext, yyleng);\n";
	print $scanner_handle "    return token::BINARY;\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "[A-Za-z][A-Za-z0-9_,.\\-\\[\\]]* {\n";
	print $scanner_handle "    yylval->stringVal = new std::string(yytext, yyleng);\n";
	print $scanner_handle "    return token::STRING;\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "\\\"(\[^\"\])*\\\" {\n";
	print $scanner_handle "    yylval->qstringVal = new std::string(yytext+1, yyleng-2);\n";
	print $scanner_handle "    return token::QSTRING;\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle " /* gobble up comments */\n";
	print $scanner_handle "\"#\"(\[^\\n\])*\\n {\n";
	print $scanner_handle "    yylloc->step();\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle " /* gobble up white-spaces */\n";
	print $scanner_handle "\[ \\t\\r\]+ {\n";
	print $scanner_handle "    yylloc->step();\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle " /* gobble up end-of-lines */\n";
	print $scanner_handle "\\n {\n";
	print $scanner_handle "    yylloc->lines(yyleng); yylloc->step();\n";
	print $scanner_handle "    /* return token::EOL; */\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle " /* pass all other characters up to bison */\n";
	print $scanner_handle ". {\n";
	print $scanner_handle "    return static_cast<token_type>(*yytext);\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";

	print $scanner_handle " /*** END EXAMPLE - Change the example lexer rules above ***/\n";

	################# write tail #################
	print $scanner_handle "\n";
	print $scanner_handle "%% /*** Additional Code ***/\n";
	print $scanner_handle "\n";
	print $scanner_handle "namespace ${prefix}Parser {\n";
	print $scanner_handle "\n";
	print $scanner_handle "Scanner::Scanner(std::istream* in,\n";
	print $scanner_handle "		 std::ostream* out)\n";
	print $scanner_handle "    : ${prefix}ParserFlexLexer(in, out)\n";
	print $scanner_handle "{\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "Scanner::~Scanner()\n";
	print $scanner_handle "{\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "void Scanner::set_debug(bool b)\n";
	print $scanner_handle "{\n";
	print $scanner_handle "    yy_flex_debug = b;\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* This implementation of ExampleFlexLexer::yylex() is required to fill the\n";
	print $scanner_handle " * vtable of the class ExampleFlexLexer. We define the scanner's main yylex\n";
	print $scanner_handle " * function via YY_DECL to reside in the Scanner class instead. */\n";
	print $scanner_handle "\n";
	print $scanner_handle "#ifdef yylex\n";
	print $scanner_handle "#undef yylex\n";
	print $scanner_handle "#endif\n";
	print $scanner_handle "\n";
	print $scanner_handle "int ${prefix}ParserFlexLexer::yylex()\n";
	print $scanner_handle "{\n";
	print $scanner_handle "    std::cerr << \"in ${prefix}ParserFlexLexer::yylex() !\" << std::endl;\n";
	print $scanner_handle "    return 0;\n";
	print $scanner_handle "}\n";
	print $scanner_handle "\n";
	print $scanner_handle "/* When the scanner receives an end-of-file indication from YY_INPUT, it then\n";
	print $scanner_handle " * checks the yywrap() function. If yywrap() returns false (zero), then it is\n";
	print $scanner_handle " * assumed that the function has gone ahead and set up `yyin' to point to\n";
	print $scanner_handle " * another input file, and scanning continues. If it returns true (non-zero),\n";
	print $scanner_handle " * then the scanner terminates, returning 0 to its caller. */\n";
	print $scanner_handle "\n";
	print $scanner_handle "int ${prefix}ParserFlexLexer::yywrap()\n";
	print $scanner_handle "{\n";
	print $scanner_handle "    return 1;\n";
	print $scanner_handle "}\n";

	close($scanner_handle);
}

# generate header for yacc parser .yy 
sub generate_parser 
{
	my $prefix = $_[0];
	my @keywords = @{$_[1]};
	my $filename = "${prefix}Parser.yy";

	open(my $parser_handle, '>', $filename) or die "Could not open file '$filename' $!";
	
	################# write header #################
	print $parser_handle "/* \$Id: parser.yy 48 2009-09-05 08:07:10Z tb \$ -*- mode: c++ -*- */\n";
	print $parser_handle "/** \\file parser.yy Contains the example Bison parser source */\n";
	print $parser_handle "\n";
	print $parser_handle "%{ /*** C/C++ Declarations ***/\n";
	print $parser_handle "\n";
	print $parser_handle "#include <stdio.h>\n";
	print $parser_handle "#include <string>\n";
	print $parser_handle "#include <vector>\n";
	print $parser_handle "\n";
	print $parser_handle "/*#include \"expression.h\"*/\n";
	print $parser_handle "\n";
	print $parser_handle "%}\n";
	print $parser_handle "\n";
	print $parser_handle "/*** yacc/bison Declarations ***/\n";
	print $parser_handle "\n";
	print $parser_handle "/* Require bison 2.3 or later */\n";
	print $parser_handle "%require \"2.3\"\n";
	print $parser_handle "\n";
	print $parser_handle "/* add debug output code to generated parser. disable this for release\n";
	print $parser_handle " * versions. */\n";
	print $parser_handle "%debug\n";
	print $parser_handle "\n";
	print $parser_handle "/* start symbol is named \"start\" */\n";
	print $parser_handle "%start start\n";
	print $parser_handle "\n";
	print $parser_handle "/* write out a header file containing the token defines */\n";
	print $parser_handle "%defines\n";
	print $parser_handle "\n";
	print $parser_handle "/* use newer C++ skeleton file */\n";
	print $parser_handle "%skeleton \"lalr1.cc\"\n";
	print $parser_handle "\n";
	print $parser_handle "/* namespace to enclose parser in */\n";
	print $parser_handle "%name-prefix=\"${prefix}Parser\"\n";
	print $parser_handle "\n";
	print $parser_handle "/* set the parser's class identifier */\n";
	print $parser_handle "%define \"parser_class_name\" \"Parser\"\n";
	print $parser_handle "\n";
	print $parser_handle "/* keep track of the current position within the input */\n";
	print $parser_handle "%locations\n";
	print $parser_handle "%initial-action\n";
	print $parser_handle "{\n";
	print $parser_handle "    // initialize the initial location object\n";
	print $parser_handle "    \@\$.begin.filename = \@\$.end.filename = &driver.streamname;\n";
	print $parser_handle "};\n";
	print $parser_handle "\n";
	print $parser_handle "/* The driver is passed by reference to the parser and to the scanner. This\n";
	print $parser_handle " * provides a simple but effective pure interface, not relying on global\n";
	print $parser_handle " * variables. */\n";
	print $parser_handle "%parse-param { class Driver& driver }\n";
	print $parser_handle "\n";
	print $parser_handle "/* verbose error messages */\n";
	print $parser_handle "%error-verbose\n";
	print $parser_handle "\n";

	##################### write keywords ##################
	print $parser_handle " /*** BEGIN EXAMPLE - Change the example grammar's tokens below ***/\n";
	print $parser_handle "\n";
	print $parser_handle "%union {\n";
	print $parser_handle "    int  			integerVal;\n";
	print $parser_handle "    double 			doubleVal;\n";
	print $parser_handle "    std::string*		stringVal;\n";
	print $parser_handle "	std::string*		qstringVal;\n";
	print $parser_handle "	std::string*		binaryVal;\n";
	print $parser_handle "\n";
	print $parser_handle "	class IntegerArray* integerArrayVal;\n";
	print $parser_handle "	class StringArray* stringArrayVal;\n";
	print $parser_handle "}\n";

	print $parser_handle "\n";
	foreach my $keyword (@keywords)
	{
		print $parser_handle "%token K_$keyword \"$keyword\"\n";
	}
	print $parser_handle "\n";

	##################### write additional tokens ##################
	print $parser_handle "%token			END	     0	\"end of file\"\n";
	print $parser_handle "/* %token			EOL		\"end of line\" */\n";
	print $parser_handle "%token <integerVal> 	INTEGER		\"integer\"\n";
	print $parser_handle "%token <doubleVal> 	DOUBLE		\"double\"\n";
	print $parser_handle "%token <stringVal> 	STRING		\"string\"\n";
	print $parser_handle "%token <qstringVal> 	QSTRING		\"qstring\"\n";
	print $parser_handle "%token <binaryVal> 	BINARY		\"binary numbers\"\n";
	print $parser_handle "%type <integerArrayVal> integer_array\n";
	print $parser_handle "%type <stringArrayVal> string_array\n";

	print $parser_handle "\n";
	print $parser_handle "%destructor { delete \$\$; } STRING QSTRING BINARY\n";
	print $parser_handle "%destructor { delete \$\$; } integer_array string_array\n";

	##################### write grammar rules for additional tokens ##################
	print $parser_handle "\n";
	print $parser_handle " /*** END EXAMPLE - Change the example grammar's tokens above ***/\n";
	print $parser_handle "\n";
	print $parser_handle "%{\n";
	print $parser_handle "\n";
	print $parser_handle "#include \"${prefix}Driver.h\"\n";
	print $parser_handle "#include \"${prefix}Scanner.h\"\n";
	print $parser_handle "\n";
	print $parser_handle "/* this \"connects\" the bison parser in the driver to the flex scanner class\n";
	print $parser_handle " * object. it defines the yylex() function call to pull the next token from the\n";
	print $parser_handle " * current lexer object of the driver context. */\n";
	print $parser_handle "#undef yylex\n";
	print $parser_handle "#define yylex driver.lexer->lex\n";
	print $parser_handle "\n";
	print $parser_handle "%}\n";
	print $parser_handle "\n";
	print $parser_handle "%% /*** Grammar Rules ***/\n";
	print $parser_handle "\n";
	print $parser_handle " /*** BEGIN EXAMPLE - Change the example grammar rules below ***/\n";
	print $parser_handle "\n";
	print $parser_handle "\n";
	print $parser_handle "integer_array : INTEGER {\n";
	print $parser_handle "				\$\$ = new IntegerArray(1, \$1);\n";
	print $parser_handle "			  }\n";
	print $parser_handle "			  | integer_array INTEGER {\n";
	print $parser_handle "				\$1->push_back(\$2);\n";
	print $parser_handle "				\$\$ = \$1;\n";
	print $parser_handle "			  }\n";
	print $parser_handle "\n";
	print $parser_handle "string_array : STRING {\n";
	print $parser_handle "				\$\$ = new StringArray(1, *\$1);\n";
	print $parser_handle "			  }\n";
	print $parser_handle "			  | string_array STRING {\n";
	print $parser_handle "				\$1->push_back(*\$2);\n";
	print $parser_handle "				\$\$ = \$1;\n";
	print $parser_handle "			  }\n";

	print $parser_handle "\n";
	print $parser_handle " /*** END EXAMPLE - Change the example grammar rules above ***/\n";
	print $parser_handle "\n";
	##################### write tail ##################
	print $parser_handle "%% /*** Additional Code ***/\n";
	print $parser_handle "\n";
	print $parser_handle "void ${prefix}Parser::Parser::error(const Parser::location_type& l,\n";
	print $parser_handle "			    const std::string& m)\n";
	print $parser_handle "{\n";
	print $parser_handle "    driver.error(l, m);\n";
	print $parser_handle "}\n";
	print $parser_handle "\n";

	close($parser_handle);
}


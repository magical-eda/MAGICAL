/**
 * @file   LefDriver.h
 * @brief  Driver for Lef parser 
 *
 * - LefParser is modified from Cadence open source parser.
 * Original version stays in C style and has very poor support
 * to object-oriented programming.
 * LefParser borrows its data structure and bison grammar, 
 * but wrap it in C++ style.
 * Please follow the [LICENSE](@ref Parsers_LefParser_License) agreement from Cadence when use the parser. 
 *
 * - The original data structure abuses malloc and free without 
 * proper initialization to pointers, which may lead to 
 * memory allocating problems.
 * Although I fixed some of the bugs and it looks good with current 
 * benchmarks, there may still contain problems with other benchmarks.
 *
 * - Another issue is that keywords may have conflicts with STRING 
 * in LefParser.yy, so I created a generalized string type GSTRING 
 * which includes all string and keywords. 
 * I did not replace all STRING with GSTRING, because it may result in 
 * performance degradation. 
 * If a new benchmark causes syntax error during parsing, then replace 
 * STRING of specific grammar with GSTRING. 
 * The main idea is to reduce the usage of GSTRING. 
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef LEFPARSER_DRIVER_H
#define LEFPARSER_DRIVER_H

#include <map>
#include <limbo/parsers/lef/bison/LefDataBase.h>

/** The example namespace is used to encapsulate the three parser classes
 * example::Parser, example::Scanner and example::Driver */
/** @brief namespace for LefParser */
namespace LefParser {

using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;

#ifndef STRSIZE
/// define STRSIZE 
#define STRSIZE 4096
#endif 

/** 
 * @class LefParser::Driver
 * The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class Driver
{
public:
    /// construct a new parser driver context
    /// @param db reference to database 
    Driver(LefDataBase& db);
	~Driver();

    /// enable debug output in the flex scanner
    bool trace_scanning;

    /// enable debug output in the bison parser
    bool trace_parsing;

    /// stream name (file or input stream) used for error messages.
    string streamname;

    /** Invoke the scanner and parser for a stream.
     * @param in	input stream
     * @param sname	stream name for error messages
     * @return		true if successfully parsed
     */
    bool parse_stream(std::istream& in,
		      const string& sname = "stream input");

    /** Invoke the scanner and parser on an input string.
     * @param input	input string
     * @param sname	stream name for error messages
     * @return		true if successfully parsed
     */
    bool parse_string(const string& input,
		      const string& sname = "string stream");

    /** Invoke the scanner and parser on a file. Use parse_stream with a
     * std::ifstream if detection of file reading errors is required.
     * @param filename	input file name
     * @return		true if successfully parsed
     */
    bool parse_file(const string& filename);

    // To demonstrate pure handling of parse errors, instead of
    // simply dumping them on the standard error output, we will pass
    // them to the driver using the following two member functions.

    /** Error handling with associated line number. This can be modified to
     * output the error e.g. to a dialog box. */
    void error(const class location& l, const string& m);

    /** General error handling. This can be modified to output the error
     * e.g. to a dialog box. */
    void error(const string& m);

    /** Pointer to the current lexer instance, this is used to connect the
     * parser to the scanner. It is used in the yylex macro. */
    class Scanner* lexer;

    /** Reference to the database filled during parsing of the
     * expressions. */
    LefDataBase& m_db;

	/***************** custom callbacks here ******************/
    /// @cond
	void lefrVersionStrCbk(string const&);
	void lefrVersionCbk(double);
	void lefrDividerCharCbk(string const&);
	void lefrLibraryEndCbk();
	void lefrCaseSensitiveCbk(int);
	void lefrNoWireExtensionCbk(string const&);
	void lefrManufacturingCbk(double);
	void lefrUseMinSpacingCbk(lefiUseMinSpacing const&);
	void lefrClearanceMeasureCbk(string const&);
	void lefrUnitsCbk(lefiUnits const&);
	void lefrBusBitCharsCbk(string const&);
	void lefrLayerCbk(lefiLayer const&);
	void lefrMaxStackViaCbk(lefiMaxStackVia const&);
	void lefrViaCbk(lefiVia const&); // include non-default via cbk 
	void lefrViaRuleCbk(lefiViaRule const&); 
	void lefrSpacingBeginCbk(int); // disabled when in non-default rules 
	void lefrSpacingEndCbk(int); // disabled when in non-default rules 
	void lefrSpacingCbk(lefiSpacing const&); // include non-default spacing cbk 
	void lefrIRDropBeginCbk(int);
	void lefrIRDropEndCbk(int);
	void lefrIRDropCbk(lefiIRDrop const&);
	void lefrMinFeatureCbk(lefiMinFeature const&);
	void lefrDielectricCbk(double);
	void lefrNonDefaultCbk(lefiNonDefault const&);
	void lefrSiteCbk(lefiSite const&);
	void lefrMacroBeginCbk(string const&);
	void lefrMacroEndCbk(string const&);
	void lefrMacroCbk(lefiMacro const&);
	void lefrMacroClassTypeCbk(string const&); // maybe useless 
	void lefrMacroOriginCbk(lefiNum const&);
	void lefrMacroSizeCbk(lefiNum const&);
	void lefrPinCbk(lefiPin const&);
	void lefrObstructionCbk(lefiObstruction&);
	void lefrDensityCbk(lefiDensity const&);
	void lefrTimingCbk(lefiTiming const&);
	void lefrArrayCbk(lefiArray const&);
	void lefrArrayBeginCbk(string const&);
	void lefrArrayEndCbk(string const&);
	void lefrPropBeginCbk(int);
	void lefrPropEndCbk(int);
	void lefrPropCbk(lefiProp const&);
	void lefrNoiseMarginCbk(lefiNoiseMargin const&);
	void lefrEdgeRateThreshold1Cbk(double);
	void lefrEdgeRateThreshold2Cbk(double);
	void lefrEdgeRateScaleFactorCbk(double);
	void lefrNoiseTableCbk(lefiNoiseTable const&);
	void lefrCorrectionTableCbk(lefiCorrectionTable const&);
	void lefrInputAntennaCbk(double);
	void lefrOutputAntennaCbk(double);
	void lefrInoutAntennaCbk(double);
	void lefrAntennaInputCbk(double);
	void lefrAntennaInoutCbk(double);
	void lefrAntennaOutputCbk(double);
	void lefrExtensionCbk(string const&);

	/***************** custom help functions here ******************/
	// copy from lef.y 
	void resetVars();
	// copy from lex.cpph
	void lefSetNonDefault(const char*);
	void lefUnsetNonDefault();
	void lefAddStringDefine(string const& token, string const& val);
	void lefAddBooleanDefine(string const& token, int val);
	void lefAddNumDefine(string const& token, double val);
	void lefAddStringMessage(string const& token, string const& val);
	void lefError(int msgNum, const char *s);
	void lefWarning(int msgNum, const char *s);
	void lefInfo(int msgNum, const char *s);
	// copy from lef.y 
	int comp_str(const char *s1, int op, const char *s2) const;
	int comp_num(double s1, int op, double s2) const;
	int validNum(int values);
	int zeroOrGt(double values) const;
	double convert_name2num(char const* versionName) const;
    /// @endcond

public:
	int lefDumbMode;   ///< for communicating with parser
	int lefNoNum;      ///< likewise, says no numbers for the next N tokens
	int lefRealNum;    ///< Next token will be a real number
	int lefNlToken;    ///< likewise
	int lefDefIf;      ///< likewise /* if TRUE, is within the defines if-then-else */
	int inDefine;		///<  /* Keep track it is within a define statement */
	int lefNdRule;     ///< for communicating with parser
	int lefInPropDef;  ///< for tracking if inside propertydefinitions
	int lefInProp;     ///< for tracking if inside property

	int lef_errors;    ///< from lex.cpph /* number of errors */
	int lef_warnings; ///< number of warnings; 
	int lef_ntokens; ///< number of tokens 
	int lef_nlines; ///< number of lines 
	FILE* lefrLog; ///< log file handler 
	static int hasOpenedLogFile; /*!< flag on how to open the warning log file */
	static int spaceMissing;   /*!< flag to indicate if there is space after " */
	std::string Hist_text;   ///< for BEGINEXT - extension

	int doneLib;       ///< keep track if the file is done parsing
	int ge56almostDone;///< lef file has Extension section
	int ge56done;      ///< done reading 5.6 and beyond library

	const char* lefrFileName; ///< lef file name 

	int lefNamesCaseSensitive;  ///< always true in 5.6
	int lefReaderCaseSensitive;  ///< default to 0

	int lefrShiftCase;     ///< if user wants to shift to all uppercase

	int lefrRelaxMode;    ///< relax mode?

	/* XXXXXXXXXXXXX check out these variables */
    ///@{
	double lef_save_x, lef_save_y;  ///< for interpreting (*) notation of LEF/DEF
    ///@}

	/* #define STRING_LIST_SIZE 1024 */
	/* char string_list[STRING_LIST_SIZE]; */

	static int ignoreVersion; ///< ignore checking version number
	static char temp_name[258]; ///< temporary buffer for characters 
	static std::string layerName; ///< layer name 
	static std::string viaName; ///< via name 
	static std::string viaRuleName; ///< via rule name 
	static std::string nonDefaultRuleName; ///< non default rule name 
	static std::string siteName; ///< site name 
	static std::string arrayName; ///< array name 
	static std::string macroName; ///< macro name 
	static std::string pinName; ///< pin name 

    ///@{
	static int siteDef, symDef, sizeDef, pinDef, obsDef, origDef; ///< some definitions? 
    ///@}
	static int useLenThr; ///< not sure what this is for 
    ///@{
	static int layerCut, layerMastOver, layerRout, layerDir; ///< some properties 
    ///@}
	static lefiAntennaEnum antennaType;  /*!< 5.4 - antenna type */
	static int arrayCutsVal;       /*!< keep track the arraycuts value */
	static int arrayCutsWar;       /*!< keep track if warning has already printed */
	static int viaRuleLayer;       /*!< keep track number of layer in a viarule */
	static int viaLayer;           /*!< keep track number of layer in a via */
	static std::string ndName;		///< for ndName in lefSetNonDefault()
	static int ndLayer;            /*!< keep track number of layer in a nondefault */
	static int numVia;             /*!< keep track number of via */
	static int viaRuleHasDir;      /*!< viarule layer has direction construct */
	static int viaRuleHasEnc;      /*!< viarule layer has enclosure construct */
	static int ndRule;         /*!< keep track if inside nondefaultrule */
	static int ndLayerWidth;       /*!< keep track if width is set at ndLayer */
	static int ndLayerSpace;       /*!< keep track if spacing is set at ndLayer */
	static int isGenerate;         /*!< keep track if viarule has generate */
	static int hasType;            /*!< keep track of type in layer */
	static int hasPitch;           /*!< keep track of pitch in layer */
	static int hasWidth;           /*!< keep track of width in layer */
	static int hasDirection;       /*!< keep track of direction in layer */
	static int hasParallel;        /*!< keep track of parallelrunlength */
	static int hasInfluence;       /*!< keep track of influence */
	static int hasTwoWidths;       /*!< keep track of twoWidths */
	static int hasLayerMincut;     /*!< keep track of layer minimumcut */
	static int hasManufactur;  /*!< keep track of manufacture is after unit */
	static int hasMinfeature;  /*!< keep track of minfeature is after unit */
	static int hasPRP;             /*!< keep track if path, rect or poly is def */
	static int needGeometry;       /*!< keep track if path/rect/poly is defined */
	static int hasViaRule_layer; /*!< keep track at least viarule or layer */
	static int hasSamenet;         /*!< keep track if samenet is defined in spacing */
	static int hasSite;        /*!< keep track if SITE has defined for pre 5.6 */
	static int hasSiteClass;   /*!< keep track if SITE has CLASS */
	static int hasSiteSize;    /*!< keep track if SITE has SIZE */
	static int hasSpCenter;    /*!< keep track if LAYER SPACING has CENTER */
	static int hasSpSamenet;   /*!< keep track if LAYER SPACING has SAMENET */
	static int hasSpParallel;  /*!< keep track if LAYER SPACING has PARALLEL */
	static int hasSpLayer;     /*!< keep track if LAYER SPACING has LAYER */
	static int hasGeoLayer;    /*!< keep track if Geometries has LAYER */


	// the following variables to keep track the number of warnings printed.
	static int antennaInoutWarnings; ///< variable to keep track the number of warnings printed
	static int antennaInputWarnings; ///< variable to keep track the number of warnings printed
	static int antennaOutputWarnings; ///< variable to keep track the number of warnings printed
	static int arrayWarnings; ///< variable to keep track the number of warnings printed
	static int caseSensitiveWarnings; ///< variable to keep track the number of warnings printed
	static int correctionTableWarnings; ///< variable to keep track the number of warnings printed
	static int dielectricWarnings; ///< variable to keep track the number of warnings printed
	static int edgeRateThreshold1Warnings; ///< variable to keep track the number of warnings printed
	static int edgeRateThreshold2Warnings; ///< variable to keep track the number of warnings printed
	static int edgeRateScaleFactorWarnings; ///< variable to keep track the number of warnings printed
	static int inoutAntennaWarnings; ///< variable to keep track the number of warnings printed
	static int inputAntennaWarnings; ///< variable to keep track the number of warnings printed
	static int iRDropWarnings; ///< variable to keep track the number of warnings printed
	static int layerWarnings; ///< variable to keep track the number of warnings printed
	static int macroWarnings; ///< variable to keep track the number of warnings printed
	static int maxStackViaWarnings; ///< variable to keep track the number of warnings printed
	static int minFeatureWarnings; ///< variable to keep track the number of warnings printed
	static int noiseMarginWarnings; ///< variable to keep track the number of warnings printed
	static int noiseTableWarnings; ///< variable to keep track the number of warnings printed
	static int nonDefaultWarnings; ///< variable to keep track the number of warnings printed
	static int noWireExtensionWarnings; ///< variable to keep track the number of warnings printed
	static int outputAntennaWarnings; ///< variable to keep track the number of warnings printed
	static int pinWarnings; ///< variable to keep track the number of warnings printed
	static int siteWarnings; ///< variable to keep track the number of warnings printed
	static int spacingWarnings; ///< variable to keep track the number of warnings printed
	static int timingWarnings; ///< variable to keep track the number of warnings printed
	static int unitsWarnings; ///< variable to keep track the number of warnings printed
	static int useMinSpacingWarnings; ///< variable to keep track the number of warnings printed
	static int viaRuleWarnings; ///< variable to keep track the number of warnings printed
	static int viaWarnings; ///< variable to keep track the number of warnings printed
	static double layerCutSpacing; ///< variable to keep track the number of warnings printed
	static char* outMsg; ///< error messages
	static char lefPropDefType; /*!< save the current type of the property */
	static char cur_token[STRSIZE];      /*!< global so error message can print it */
	static char saved_token[STRSIZE];/*!< for an (illegal) usage ';TOKEN' */
	static char pv_token[STRSIZE];   /*!< previous token, for check ; without space */
	static int nDMsgs; ///< disable message 


	int spParallelLength;          /*!< the number of layer parallelrunlength */

	lefiNum macroNum;              /*!< for origin & size callback */

    /// @name Copied from Global variables
    ///@{ 
	lefiUserData lefrUserData; 
	//char* lefrFileName; 
	static int lefrRegisterUnused; 
	FILE* lefrFile; 
	lefiAntennaPWL* lefrAntennaPWLPtr; 
	lefiArray lefrArray; 
	lefiCorrectionTable lefrCorrectionTable; 
	lefiDensity lefrDensity; 
	lefiGcellPattern* lefrGcellPatternPtr; 
	lefiGeometries* lefrGeometriesPtr; 
	lefiIRDrop lefrIRDrop; 
	lefiLayer lefrLayer; 
	lefiMacro lefrMacro; 
	lefiMaxStackVia lefrMaxStackVia;  ///< 5.5
	lefiMinFeature lefrMinFeature; 
	struct lefiNoiseMargin lefrNoiseMargin; 
	lefiNoiseTable lefrNoiseTable; 
	lefiNonDefault lefrNonDefault; 
	lefiObstruction lefrObstruction; 
	lefiPin lefrPin; 
	lefiProp lefrProp; 
	lefiSite lefrSite; 
	lefiSitePattern* lefrSitePatternPtr; 
	lefiSpacing lefrSpacing; 
	lefiTiming lefrTiming; 
	lefiTrackPattern* lefrTrackPatternPtr; 
	lefiUnits lefrUnits; 
	lefiUseMinSpacing lefrUseMinSpacing; 
	lefiVia lefrVia; 
	lefiViaRule lefrViaRule; 
	int lefrIsReset; 
	int lefrDoGcell; 
	int lefrDoGeometries; 
	int lefrDoTrack; 
	int lefrDoSite; 
	int lefrHasLayer;    ///< 5.5 this & lefrHasMaxVS is to keep track that
	int lefrHasMaxVS;    ///< MAXVIASTACK has to be after all layers
	int lefrHasSpacingTbl;   ///< keep track of spacing table in a layer
	int lefrHasSpacing;      ///< keep track of spacing in a layer
	int userHasSetVer;   ///< if user has set default version value
	//int doneLib;         ///< keep track if the library is done parsing
	///< 5.6 END LIBRARY is optional.
	//int ge56almostDone;  ///< have reached the EXTENSION SECTION
	//int ge56done;        ///< a 5.6 and it has END LIBRARY statement
    ///@}

    /// @name Property definations
	///@{
    /// The following global variables are for storing the propertydefination
	/// types.  Only real & integer need to store since the parser can
	/// distinguish string and quote string
	lefiPropType lefrLibProp;
	lefiPropType lefrCompProp;
	lefiPropType lefrPinProp;
	lefiPropType lefrMacroProp;
	lefiPropType lefrViaProp;
	lefiPropType lefrViaRuleProp;
	lefiPropType lefrLayerProp;
	lefiPropType lefrNondefProp;
    ///@}

    /// @name Global variables are used in lef.y
    ///@{ 
	int use5_3;          /*!< keep track if there is any 5.3 on 5.4 file */
	int use5_4;          /*!< keep track if only 5.4 is allowed */
	int hasVer;          /*!< keep track if version is in file */
	int hasNameCase;     /*!< keep track if namescasesensitive is in file */
	int hasBusBit;       /*!< keep track if bustbitchars is in file */
	int hasDivChar;      /*!< keep track if dividerchar is in file */
	int aOxide;          /*!< keep track for oxide */
	double versionNum;
    ///@} 

    /// @name Warning variables  
    ///@{ 
    /// Flags to control number of warnings to print out, max will be 999. 
    /// I don't know if it is useful, but they are used in .yy 
	int lefrAntennaInoutWarnings;
	int lefrAntennaInputWarnings;
	int lefrAntennaOutputWarnings;
	int lefrArrayWarnings;
	int lefrCaseSensitiveWarnings;
	int lefrCorrectionTableWarnings;
	int lefrDielectricWarnings;
	int lefrEdgeRateThreshold1Warnings;
	int lefrEdgeRateThreshold2Warnings;
	int lefrEdgeRateScaleFactorWarnings;
	int lefrInoutAntennaWarnings;
	int lefrInputAntennaWarnings;
	int lefrIRDropWarnings;
	int lefrLayerWarnings;
	int lefrMacroWarnings;
	int lefrMaxStackViaWarnings;
	int lefrMinFeatureWarnings;
	int lefrNoiseMarginWarnings;
	int lefrNoiseTableWarnings;
	int lefrNonDefaultWarnings;
	int lefrNoWireExtensionWarnings;
	int lefrOutputAntennaWarnings;
	int lefrPinWarnings;
	int lefrSiteWarnings;
	int lefrSpacingWarnings;
	int lefrTimingWarnings;
	int lefrUnitsWarnings;
	int lefrUseMinSpacingWarnings;
	int lefrViaRuleWarnings;
	int lefrViaWarnings;
    ///@} 

    /// @name message table, alias table...
    ///@{ 
	std::map<string, string> message_set; ///< message table 
	std::map<string, string> defines_set; ///< string table 
	std::map<string, int> defineb_set; ///< boolean table 
	std::map<string, double> definen_set; ///< number table 
    ///@}
};

/// @brief API for LefParser. 
/// Read LEF file and initialize database by calling user-defined callback functions. 
/// @param db database which is derived from @ref LefParser::LefDataBase
/// @param lefFile LEF file 
bool read(LefDataBase& db, const string& lefFile);

} // namespace example

#endif // EXAMPLE_DRIVER_H

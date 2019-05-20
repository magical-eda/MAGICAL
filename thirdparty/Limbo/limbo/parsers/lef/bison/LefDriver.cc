/**
 * @file   LefDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref LefParser::Driver
 */

#include <limbo/parsers/lef/bison/LefDriver.h>
#include <limbo/parsers/lef/bison/LefScanner.h>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
#include <cctype>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cmath>

namespace LefParser {

Driver::Driver(LefDataBase& db)
    : trace_scanning(false),
      trace_parsing(false),
      m_db(db)
{
	lefNamesCaseSensitive = m_db.lefNamesCaseSensitive;  // always true in 5.6
	lefReaderCaseSensitive = FALSE;  // default to 0

	lefrShiftCase = m_db.lefrShiftCase;     // if user wants to shift to all uppercase

	lefrRelaxMode = FALSE;    // relax mode?
	///////////////////////////////////s
	//
	//   copied from Global variables
	//
	/////////////////////////////////////

	lefrUserData = 0;
	lefrFileName = 0;
	lefrRegisterUnused = 0;
	lefrFile = 0;
	lefrAntennaPWLPtr = 0;
//	lefrArray;
//	lefrCorrectionTable;
//	lefrDensity;
	lefrGcellPatternPtr = 0;
	lefrGeometriesPtr = 0;
//	lefrIRDrop;
//	lefrLayer;
//	lefrMacro;
//	lefrMaxStackVia;  // 5.5
//	lefrMinFeature;
//	lefrNoiseMargin;
//	lefrNoiseTable;
//	lefrNonDefault;
//	lefrObstruction;
//	lefrPin;
//	lefrProp;
//	lefrSite;
	lefrSitePatternPtr = 0;
//	lefrSpacing;
//	lefrTiming;
	lefrTrackPatternPtr = 0;
//	lefrUnits;
//	lefrUseMinSpacing;
//	lefrVia;
//	lefrViaRule;
	lefrIsReset = 0;
	lefrDoGcell = 0;
	lefrDoGeometries = 0;
	lefrDoTrack = 0;
	lefrDoSite = 0;
	lefrHasLayer = 0;    // 5.5 this & lefrHasMaxVS is to keep track that
	lefrHasMaxVS = 0;    // MAXVIASTACK has to be after all layers
//	lefrHasSpacingTbl;   // keep track of spacing table in a layer
//	lefrHasSpacing;      // keep track of spacing in a layer
	userHasSetVer = 0;   // if user has set default version value
	doneLib = 1;         // keep track if the library is done parsing
	// 5.6 END LIBRARY is optional.
	ge56almostDone = 0;  // have reached the EXTENSION SECTION
	ge56done = 0;        // a 5.6 and it has END LIBRARY statement

	// The following global variables are for storing the propertydefination
	// types.  Only real & integer need to store since the parser can
	// distinguish string and quote string
//	lefrLibProp;
//	lefrCompProp;
//	lefrPinProp;
//	lefrMacroProp;
//	lefrViaProp;
//	lefrViaRuleProp;
//	lefrLayerProp;
//	lefrNondefProp;

	// The following global variables are used in lef.y
	use5_3 = 0;          /* keep track if there is any 5.3 on 5.4 file */
	use5_4 = 0;          /* keep track if only 5.4 is allowed */
	hasVer = 0;          /* keep track if version is in file */
	hasNameCase = 0;     /* keep track if namescasesensitive is in file */
	hasBusBit = 0;       /* keep track if bustbitchars is in file */
	hasDivChar = 0;      /* keep track if dividerchar is in file */
	aOxide = 0;          /* keep track for oxide */
	versionNum = m_db.current_version();

	////////////////////////////////////
	//
	//  Flags to control number of warnings to print out, max will be 999
	//  I don't know if it is useful, but they are used in .yy 
	//
	////////////////////////////////////

	lefrAntennaInoutWarnings = 999;
	lefrAntennaInputWarnings = 999;
	lefrAntennaOutputWarnings = 999;
	lefrArrayWarnings = 999;
	lefrCaseSensitiveWarnings = 999;
	lefrCorrectionTableWarnings = 999;
	lefrDielectricWarnings = 999;
	lefrEdgeRateThreshold1Warnings = 999;
	lefrEdgeRateThreshold2Warnings = 999;
	lefrEdgeRateScaleFactorWarnings = 999;
	lefrInoutAntennaWarnings = 999;
	lefrInputAntennaWarnings = 999;
	lefrIRDropWarnings = 999;
	lefrLayerWarnings = 999;
	lefrMacroWarnings = 999;
	lefrMaxStackViaWarnings = 999;
	lefrMinFeatureWarnings = 999;
	lefrNoiseMarginWarnings = 999;
	lefrNoiseTableWarnings = 999;
	lefrNonDefaultWarnings = 999;
	lefrNoWireExtensionWarnings = 999;
	lefrOutputAntennaWarnings = 999;
	lefrPinWarnings = 999;
	lefrSiteWarnings = 999;
	lefrSpacingWarnings = 999;
	lefrTimingWarnings = 999;
	lefrUnitsWarnings = 999;
	lefrUseMinSpacingWarnings = 999;
	lefrViaRuleWarnings = 999;
	lefrViaWarnings = 999;

	lefNdRule = 0;// for communicating with parser
	lefDefIf = FALSE;
	inDefine = 0;

	lef_errors = 0;    // from lex.cpph /* number of errors */
	lef_warnings = 0;
	lef_ntokens = 0;
	lef_nlines = 1;

	lefrLog = stdout; // dump to screen, original is 0 
}
Driver::~Driver()
{
	if (hasOpenedLogFile) 
    {
        fclose(lefrLog);
        hasOpenedLogFile = 0;
    }
}

bool Driver::parse_stream(std::istream& in, const std::string& sname)
{
    streamname = sname;

    Scanner scanner(&in);
    scanner.set_debug(trace_scanning);
    this->lexer = &scanner;

    Parser parser(*this);
    parser.set_debug_level(trace_parsing);
    return (parser.parse() == 0);
}

bool Driver::parse_file(const std::string &filename)
{
    std::ifstream in(filename.c_str());
    if (!in.good()) {std::cerr << "failed to open " << filename << std::endl; return false;}
    lefrFileName = (const char*)filename.c_str();
    return parse_stream(in, filename);
}

bool Driver::parse_string(const std::string &input, const std::string& sname)
{
    std::istringstream iss(input);
    return parse_stream(iss, sname);
}

void Driver::error(const class location& l,
		   const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}

void Driver::error(const std::string& m)
{
    std::cerr << m << std::endl;
}

/***************** custom callbacks here ******************/
void Driver::lefrVersionStrCbk(string const& v)
{
	m_db.lef_version_cbk(v);
}
void Driver::lefrVersionCbk(double v)
{
	m_db.lef_version_cbk(v);
}
void Driver::lefrDividerCharCbk(string const& v)
{
	m_db.lef_dividerchar_cbk(v);
}
void Driver::lefrLibraryEndCbk()
{
}
void Driver::lefrCaseSensitiveCbk(int v)
{
	m_db.lef_casesensitive_cbk(v);
}
void Driver::lefrNoWireExtensionCbk(string const& v)
{
	m_db.lef_nowireextension_cbk(v);
}
void Driver::lefrManufacturingCbk(double v)
{
	m_db.lef_manufacturing_cbk(v);
}
void Driver::lefrUseMinSpacingCbk(lefiUseMinSpacing const& v)
{
	m_db.lef_useminspacing_cbk(v);
}
void Driver::lefrClearanceMeasureCbk(string const& v)
{
	m_db.lef_clearancemeasure_cbk(v);
}
void Driver::lefrUnitsCbk(lefiUnits const& v)
{
	m_db.lef_units_cbk(v);
}
void Driver::lefrBusBitCharsCbk(string const& v)
{
	m_db.lef_busbitchars_cbk(v);
}
void Driver::lefrLayerCbk(lefiLayer const& v)
{
	m_db.lef_layer_cbk(v);
}
void Driver::lefrMaxStackViaCbk(lefiMaxStackVia const& v)
{
	m_db.lef_maxstackvia_cbk(v);
}
void Driver::lefrViaCbk(lefiVia const& v)
{
	if (lefNdRule) // in non-default rules 
		lefrNonDefault.lefiNonDefault::addViaRule(v);
	else // normal mode 
		m_db.lef_via_cbk(v);
}
void Driver::lefrViaRuleCbk(lefiViaRule const& v)
{
	m_db.lef_viarule_cbk(v);
}
void Driver::lefrSpacingBeginCbk(int)
{
	// we do not need any begin/end cbk 
	if (lefNdRule) // in non-default rules 
		return;
	else // normal mode 
		return;
}
void Driver::lefrSpacingEndCbk(int)
{
	// we do not need any begin/end cbk 
	if (lefNdRule) // in non-default rules 
		return;
	else // normal mode 
		return;
}
void Driver::lefrSpacingCbk(lefiSpacing const& s)
{
	if (lefNdRule) // in non-default rules 
		lefrNonDefault.lefiNonDefault::addSpacingRule(s);
	else // normal mode 
		m_db.lef_spacing_cbk(s);
}
void Driver::lefrIRDropBeginCbk(int)
{
}
void Driver::lefrIRDropEndCbk(int)
{
}
void Driver::lefrIRDropCbk(lefiIRDrop const& v)
{
	m_db.lef_irdrop_cbk(v);
}
void Driver::lefrMinFeatureCbk(lefiMinFeature const& v)
{
	m_db.lef_minfeature_cbk(v);
}
void Driver::lefrDielectricCbk(double v)
{
	m_db.lef_dielectric_cbk(v);
}
void Driver::lefrNonDefaultCbk(lefiNonDefault const& v)
{
	m_db.lef_nondefault_cbk(v);
}
void Driver::lefrSiteCbk(lefiSite const& v)
{
	m_db.lef_site_cbk(v);
}
void Driver::lefrMacroBeginCbk(string const&)
{
}
void Driver::lefrMacroEndCbk(string const&)
{
}
void Driver::lefrMacroCbk(lefiMacro const& v)
{
	m_db.lef_macro_cbk(v);
    //lefrMacro.Init();
    //lefrMacro.obstruction().Destroy();
    lefrMacro.clear();
}
void Driver::lefrMacroClassTypeCbk(string const&)// maybe useless 
{} 
void Driver::lefrMacroOriginCbk(lefiNum const&)
{}
void Driver::lefrMacroSizeCbk(lefiNum const&)
{}
void Driver::lefrPinCbk(lefiPin const&) // directly added to macro 
{}
void Driver::lefrObstructionCbk(lefiObstruction& v)
{
	//m_db.lef_obstruction_cbk(v);
    // instead of a callback, add to macro 
    lefrMacro.obstructions().push_back(new lefiObstruction());
    lefrMacro.obstructions().back()->swap(v);
}
void Driver::lefrDensityCbk(lefiDensity const& v)
{
	m_db.lef_density_cbk(v);
}
void Driver::lefrTimingCbk(lefiTiming const& v)
{
	m_db.lef_timing_cbk(v);
}
void Driver::lefrArrayCbk(lefiArray const& v)
{
	m_db.lef_array_cbk(v);
}
void Driver::lefrArrayBeginCbk(string const&)
{}
void Driver::lefrArrayEndCbk(string const&)
{}
void Driver::lefrPropBeginCbk(int)
{}
void Driver::lefrPropEndCbk(int)
{}
void Driver::lefrPropCbk(lefiProp const& v)
{
	m_db.lef_prop_cbk(v);
}
void Driver::lefrNoiseMarginCbk(lefiNoiseMargin const& v)
{
	m_db.lef_noisemargin_cbk(v);
}
void Driver::lefrEdgeRateThreshold1Cbk(double v)
{
	m_db.lef_edgeratethreshold1_cbk(v);
}
void Driver::lefrEdgeRateThreshold2Cbk(double v)
{
	m_db.lef_edgeratethreshold2_cbk(v);
}
void Driver::lefrEdgeRateScaleFactorCbk(double v)
{
	m_db.lef_edgeratescalefactor_cbk(v);
}
void Driver::lefrNoiseTableCbk(lefiNoiseTable const& v)
{
	m_db.lef_noisetable_cbk(v);
}
void Driver::lefrCorrectionTableCbk(lefiCorrectionTable const& v)
{
	m_db.lef_correctiontable_cbk(v);
}
void Driver::lefrInputAntennaCbk(double v)
{
	m_db.lef_inputantenna_cbk(v);
}
void Driver::lefrOutputAntennaCbk(double v)
{
	m_db.lef_outputantenna_cbk(v);
}
void Driver::lefrInoutAntennaCbk(double v)
{
	m_db.lef_inoutantenna_cbk(v);
}
void Driver::lefrAntennaInputCbk(double v)
{
	m_db.lef_antennainput_cbk(v);
}
void Driver::lefrAntennaInoutCbk(double v)
{
	m_db.lef_antennainout_cbk(v);
}
void Driver::lefrAntennaOutputCbk(double v)
{
	m_db.lef_antennaoutput_cbk(v);
}
void Driver::lefrExtensionCbk(string const& v)
{
	m_db.lef_extension_cbk(v);
}

/***************** custom help functions here ******************/
void Driver::resetVars()
{
     hasVer = 0;
     hasNameCase = 0;
     hasBusBit = 0;
     hasDivChar = 0;
     hasManufactur = 0;
     hasMinfeature = 0;
     hasSite = 0;
     antennaInoutWarnings = 0;
     antennaInputWarnings = 0;
     antennaOutputWarnings = 0;
     arrayWarnings = 0;
     caseSensitiveWarnings = 0;
     correctionTableWarnings = 0;
     dielectricWarnings = 0;
     edgeRateThreshold1Warnings = 0;
     edgeRateThreshold2Warnings = 0;
     edgeRateScaleFactorWarnings = 0;
     inoutAntennaWarnings = 0;
     inputAntennaWarnings = 0;
     iRDropWarnings = 0;
     layerWarnings = 0;
     macroWarnings = 0;
     maxStackViaWarnings = 0;
     minFeatureWarnings = 0;
     noiseMarginWarnings = 0;
     noiseTableWarnings = 0;
     nonDefaultWarnings = 0;
     noWireExtensionWarnings = 0;
     outputAntennaWarnings = 0;
     siteWarnings = 0;
     spacingWarnings = 0;
     timingWarnings = 0;
     unitsWarnings = 0;
     useMinSpacingWarnings = 0;
     viaRuleWarnings = 0;
     viaWarnings = 0;
     return;
}
void Driver::lefSetNonDefault(const char* nd_name)
{
   ndName = nd_name;
}
void Driver::lefUnsetNonDefault() 
{
   lefNdRule = 0;
}
void Driver::lefAddStringDefine(string const&token, string const& val) 
{
	// rewrite it in c++ style 
	// maybe buggy 
	if (!lefNamesCaseSensitive)
	{
		string token_lc = token;
		for (string::iterator it = token_lc.begin(); it != token_lc.end(); ++it)
			(*it) = tolower(*it);
		defines_set.insert(std::make_pair(token_lc, val));
	}
	else 
	{
		defines_set.insert(std::make_pair(token, val));
	}
	lefDefIf = FALSE;
	inDefine = 0;
}
void Driver::lefAddBooleanDefine(string const& token, int val) 
{
	// rewrite it in c++ style 
	// maybe buggy 
	if (!lefNamesCaseSensitive)
	{
		string token_lc = token;
		for (string::iterator it = token_lc.begin(); it != token_lc.end(); ++it)
			(*it) = tolower(*it);
		defineb_set.insert(std::make_pair(token_lc, val));
	}
	else 
	{
		defineb_set.insert(std::make_pair(token, val));
	}
}
void Driver::lefAddNumDefine(string const& token, double val) 
{
	// rewrite it in c++ style 
	// maybe buggy 
	if (!lefNamesCaseSensitive)
	{
		string token_lc = token;
		for (string::iterator it = token_lc.begin(); it != token_lc.end(); ++it)
			(*it) = tolower(*it);
		definen_set.insert(std::make_pair(token_lc, val));
	}
	else 
	{
		definen_set.insert(std::make_pair(token, val));
	}
}
/* Store the message data in a stack */
void Driver::lefAddStringMessage(string const& token, string const& val) 
{
	// rewrite it in c++ style 
	// maybe buggy 
	if (!lefNamesCaseSensitive)
	{
		string token_lc = token;
		for (string::iterator it = token_lc.begin(); it != token_lc.end(); ++it)
			(*it) = tolower(*it);
		message_set.insert(std::make_pair(token_lc, val));
	}
	else 
	{
		message_set.insert(std::make_pair(token, val));
	}
}
void Driver::lefError(int msgNum, const char *s) 
{
   char* str;
   int len = strlen(cur_token)-1;
   int pvLen = strlen(pv_token)-1;

   /* PCR 690679, probably missing space before a ';' */
   if (strcmp(s, "parse error") == 0) {
      if ((len > 1) && (cur_token[len] == ';')) {
         str = (char*)lefMalloc(strlen(cur_token)+strlen(s)+strlen(lefrFileName)
                                +350);
         sprintf(str, "ERROR (LEFPARS-%d): %s, see file %s at line %d\nLast cur_token was <%s>, space is missing before <;>\n",
              msgNum, s, lefrFileName, lef_nlines, cur_token);
      } else if ((pvLen > 1) && (pv_token[pvLen] == ';')) {
         str = (char*)lefMalloc(strlen(pv_token)+strlen(s)+strlen(lefrFileName)
                                +350);
         sprintf(str, "ERROR (LEFPARS-%d): %s, see file %s at line %d\nLast cur_token was <%s>, space is missing before <;>\n",
              msgNum, s, lefrFileName, lef_nlines-1, pv_token);
      } else if ((cur_token[0] == '"') && (spaceMissing)) {
         /* most likely space is missing after the end " */
         str = (char*)lefMalloc(strlen(pv_token)+strlen(s)+strlen(lefrFileName)
                                +350);
         sprintf(str, "ERROR (LEFPARS-%d): %s, see file %s at line %d\nLast cur_token was <%s\">, space is missing between the closing \" of the string and ;.\n",
              1010, s, lefrFileName, lef_nlines, cur_token);
         spaceMissing = 0;
      } else {
         str = (char*)lefMalloc(strlen(cur_token) + strlen(lefrFileName) + 350);
         sprintf(str, "ERROR (LEFPARS-%d): Lef parser has encountered an error in file %s at line %d, on cur_token %s.\nProblem can be syntax error on the lef file or an invalid parameter name.\nDouble check the syntax on the lef file with the LEFDEF Reference Manual.\n",
              msgNum, lefrFileName, lef_nlines, cur_token);
      }
   } else if (strcmp(s, "syntax error") == 0) {  /* linux machines */
      if ((len > 1) && (cur_token[len] == ';')) {
         str = (char*)lefMalloc(strlen(cur_token)+strlen(s)+strlen(lefrFileName)
                                +350);
         sprintf(str, "ERROR (LEFPARS-%d): %s, see file %s at line %d\nLast cur_token was <%s>, space is missing before <;>\n",
              msgNum, s, lefrFileName, lef_nlines, cur_token);
      } else if ((pvLen > 1) && (pv_token[pvLen] == ';')) {
         str = (char*)lefMalloc(strlen(pv_token)+strlen(s)+strlen(lefrFileName)
                                +350);
         sprintf(str, "ERROR (LEFPARS-%d): %s, see file %s at line %d\nLast cur_token was <%s>, space is missing before <;>\n",
              msgNum, s, lefrFileName, lef_nlines-1, pv_token);
      } else if ((cur_token[0] == '"') && (spaceMissing)) {
         /* most likely space is missing after the end " */
         str = (char*)lefMalloc(strlen(pv_token)+strlen(s)+strlen(lefrFileName)
                                +350);
         sprintf(str, "ERROR (LEFPARS-%d): %s, see file %s at line %d\nLast cur_token was <%s\">, space is missing between the closing \" of the string and ;.\n",
              1011, s, lefrFileName, lef_nlines, cur_token);
         spaceMissing = 0;
      } else {
         str = (char*)lefMalloc(strlen(cur_token) + strlen(lefrFileName) + 350);
         sprintf(str, "ERROR (LEFPARS-%d): Lef parser has encountered an error in file %s at line %d, on cur_token %s.\nProblem can be syntax error on the lef file or an invalid parameter name.\nDouble check the syntax on the lef file with the LEFDEF Reference Manual.\n",
              msgNum, lefrFileName, lef_nlines, cur_token);
      }
   } else {
      str = (char*)lefMalloc(strlen(cur_token) + strlen(s) +
                             strlen(lefrFileName) + 350);
      sprintf(str, "ERROR (LEFPARS-%d): %s Error in file %s at line %d, on cur_token %s.\n",
           msgNum, s, lefrFileName, lef_nlines, cur_token);
   }
   fflush(stdout);
   lefiError(str);
   free(str);
   lef_errors++;
}
/* All warning starts with 2000 */
/* All warning within lefWarning starts with 2500 */
void Driver::lefWarning(int msgNum, const char *s) 
{
#if 0
   for (int i = 0; i < nDMsgs; i++) {  /* check if warning has been disable */
      if (disableMsgs[i] == msgNum)
         return;  /* don't print out any warning since msg has been disabled */
   }
#endif 

   /*
   if (lefiWarningLogFunction) 
   {
      char* str = (char*)lefMalloc(strlen(cur_token)+strlen(s)+strlen(lefrFileName)
                                   +350);
      sprintf(str, "WARNING (LEFPARS-%d): %s See file %s at line %d.\n",
              msgNum, s, lefrFileName, lef_nlines);
      (*lefiWarningLogFunction)(str);
      free(str);
   } 
   else */ if (lefrLog) 
   {
      fprintf(lefrLog, "WARNING (LEFPARS-%d): %s See file %s at line %d\n",
              msgNum, s, lefrFileName, lef_nlines);
   } 
   else 
   {
      if (!hasOpenedLogFile) 
	  {
         if ((lefrLog = fopen("lefRWarning.log", "w")) == 0) 
		 {
            printf("WARNING (LEFPARS-2500): Unable to open the file lefRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Warning messages will not be printed.\n");
         } 
		 else 
		 {
            hasOpenedLogFile = 1; 
            fprintf(lefrLog, "Warnings from file: %s\n\n", lefrFileName);
            fprintf(lefrLog, "WARNING (LEFPARS-%d): %s See file %s at line %d\n",
                    msgNum, s, lefrFileName, lef_nlines);
         }
      } 
	  else 
	  {
         if ((lefrLog = fopen("lefRWarning.log", "a")) == 0) 
		 {
            printf("WARNING (LEFPARS-2501): Unable to open the file lefRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Warning messages will not be printed.\n");
         }
		 else 
		 {
            fprintf(lefrLog, "\nWarnings from file: %s\n\n", lefrFileName);
            fprintf(lefrLog, "WARNING (LEFPARS-%d): %s See file %s at line %d\n",
                    msgNum, s, lefrFileName, lef_nlines);
         }
      }
   }
   lef_warnings++;
}
/* All info starts with 3000 */
/* All info within lefInfo starts with 3500 */
void Driver::lefInfo(int msgNum, const char *s) 
{
#if 0
   for (int i = 0; i < nDMsgs; i++) {  /* check if info has been disable */
      if (disableMsgs[i] == msgNum)
         return;  /* don't print out any info since msg has been disabled */
   }
#endif 

/*   if (lefiWarningLogFunction) 
   {
      char* str = (char*)lefMalloc(strlen(cur_token)+strlen(s)+strlen(lefrFileName)
                                   +350);
      sprintf(str, "INFO (LEFPARS-%d): %s See file %s at line %d.\n",
              msgNum, s, lefrFileName, lef_nlines);
      (*lefiWarningLogFunction)(str);
      free(str);
   } 
   else*/ if (lefrLog) 
   {
      fprintf(lefrLog, "INFO (LEFPARS-%d): %s See file %s at line %d\n",
              msgNum, s, lefrFileName, lef_nlines);
   } 
   else 
   {
      if (!hasOpenedLogFile) 
	  {
         if ((lefrLog = fopen("lefRWarning.log", "w")) == 0) 
		 {
            printf("WARNING (LEFPARS-3500): Unable to open the file lefRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Info messages will not be printed.\n");
         } 
		 else 
		 {
            hasOpenedLogFile = 1;
            fprintf(lefrLog, "Info from file: %s\n\n", lefrFileName);
            fprintf(lefrLog, "INFO (LEFPARS-%d): %s See file %s at line %d\n",
                    msgNum, s, lefrFileName, lef_nlines);
         }
      } 
	  else 
	  {
         if ((lefrLog = fopen("lefRWarning.log", "a")) == 0) 
		 {
            printf("WARNING (LEFPARS-3500): Unable to open the file lefRWarning.log in %s.\n",
            getcwd(NULL, 64));
            printf("Info messages will not be printed.\n");
         } 
		 else 
		 {
            fprintf(lefrLog, "\nInfo from file: %s\n\n", lefrFileName);
            fprintf(lefrLog, "INFO (LEFPARS-%d): %s See file %s at line %d\n",
                    msgNum, s, lefrFileName, lef_nlines);
         }
      }
   }
}
int Driver::comp_str(const char *s1, int op, const char *s2) const
{
	int k = strcmp(s1, s2);
	switch (op) {
		case C_EQ: return k == 0;
		case C_NE: return k != 0;
		case C_GT: return k >  0;
		case C_GE: return k >= 0;
		case C_LT: return k <  0;
		case C_LE: return k <= 0;
	}
	return 0;
}
int Driver::comp_num(double s1, int op, double s2) const
{
	double k = s1 - s2;
	switch (op) {
		case C_EQ: return k == 0;
		case C_NE: return k != 0;
		case C_GT: return k >  0;
		case C_GE: return k >= 0;
		case C_LT: return k <  0;
		case C_LE: return k <= 0;
	}
	return 0;
}
int Driver::validNum(int values)
{
    switch (values) {
        case 100:
        case 200:
        case 1000:
        case 2000:
                return 1; 
        case 10000:
        case 20000:
			if (versionNum < 5.6) 
			{
				if (/*lefrUnitsCbk*/ 1) 
				{
					if (unitsWarnings++ < lefrUnitsWarnings) 
					{
						outMsg = (char*)lefMalloc(10000);
						sprintf (outMsg,
								"Error found when processing LEF file '%s'\nUnit %d is a version 5.6 or later syntax\nYour lef file is defined with version %g",
								lefrFileName, values, versionNum);
						lefError(1501, outMsg);
						lefFree(outMsg);
					}
				}
				return 0;
			} 
			else return 1;
    }
    if (unitsWarnings++ < lefrUnitsWarnings) {
       outMsg = (char*)lefMalloc(10000);
       sprintf (outMsg,
          "The value %d defined for LEF UNITS DATABASE MICRONS is invalid\n. Correct value is 100, 200, 1000, 2000, 10000, or 20000", values);
       lefError(1502, outMsg);
       lefFree(outMsg);
    }
    /*CHKERR();*/
    return 0;
}
int Driver::zeroOrGt(double values) const
{
    if (values < 0)
      return 0;
    return 1;
}
double Driver::convert_name2num(char const* versionName) const
{
    char majorNm[80];
    char minorNm[80];
    char *subMinorNm = NULL;
    char *p1;
    char *versionNm = strdup(versionName);

    double major = 0, minor = 0, subMinor = 0;
    double version;

    sscanf(versionNm, "%[^.].%s", majorNm, minorNm);
    if ((p1 = strchr(minorNm, '.'))) {
       subMinorNm = p1+1;
       *p1 = '\0';
    }
    major = atof(majorNm);
    minor = atof(minorNm);
    if (subMinorNm)
       subMinor = atof(subMinorNm);

    version = major;

    if (minor > 0)
       version = major + minor/10;

    if (subMinor > 0)
       version = version + subMinor/1000;

    lefFree(versionNm);
    return version;
}

/***************** static variables ******************/
int Driver::ignoreVersion = 0; // ignore checking version number
char Driver::temp_name[258];
std::string Driver::layerName;
std::string Driver::viaName;
std::string Driver::viaRuleName;
std::string Driver::nonDefaultRuleName;
std::string Driver::siteName;
std::string Driver::arrayName;
std::string Driver::macroName;
std::string Driver::pinName;

int Driver::siteDef;
int Driver::symDef;
int Driver::sizeDef;
int Driver::pinDef;
int Driver::obsDef;
int Driver::origDef;
int Driver::useLenThr;
int Driver::layerCut;
int Driver::layerMastOver;
int Driver::layerRout;
int Driver::layerDir;
lefiAntennaEnum Driver::antennaType = lefiAntennaAR;  /* 5.4 - antenna type */
int Driver::arrayCutsVal;       /* keep track the arraycuts value */
int Driver::arrayCutsWar;       /* keep track if warning has already printed */
int Driver::viaRuleLayer;       /* keep track number of layer in a viarule */
int Driver::viaLayer;           /* keep track number of layer in a via */
std::string Driver::ndName;
int Driver::ndLayer;            /* keep track number of layer in a nondefault */
int Driver::numVia;             /* keep track number of via */
int Driver::viaRuleHasDir;      /* viarule layer has direction construct */
int Driver::viaRuleHasEnc;      /* viarule layer has enclosure construct */
int Driver::ndRule = 0;         /* keep track if inside nondefaultrule */
int Driver::ndLayerWidth;       /* keep track if width is set at ndLayer */
int Driver::ndLayerSpace;       /* keep track if spacing is set at ndLayer */
int Driver::isGenerate;         /* keep track if viarule has generate */
int Driver::hasType;            /* keep track of type in layer */
int Driver::hasPitch;           /* keep track of pitch in layer */
int Driver::hasWidth;           /* keep track of width in layer */
int Driver::hasDirection;       /* keep track of direction in layer */
int Driver::hasParallel;        /* keep track of parallelrunlength */
int Driver::hasInfluence;       /* keep track of influence */
int Driver::hasTwoWidths;       /* keep track of twoWidths */
int Driver::hasLayerMincut;     /* keep track of layer minimumcut */
int Driver::hasManufactur = 0;  /* keep track of manufacture is after unit */
int Driver::hasMinfeature = 0;  /* keep track of minfeature is after unit */
int Driver::hasPRP;             /* keep track if path, rect or poly is def */
int Driver::needGeometry;       /* keep track if path/rect/poly is defined */
int Driver::hasViaRule_layer = 0; /* keep track at least viarule or layer */
int Driver::hasSamenet;         /* keep track if samenet is defined in spacing */
int Driver::hasSite = 0;        /* keep track if SITE has defined for pre 5.6 */
int Driver::hasSiteClass = 0;   /* keep track if SITE has CLASS */
int Driver::hasSiteSize = 0;    /* keep track if SITE has SIZE */
int Driver::hasSpCenter = 0;    /* keep track if LAYER SPACING has CENTER */
int Driver::hasSpSamenet = 0;   /* keep track if LAYER SPACING has SAMENET */
int Driver::hasSpParallel = 0;  /* keep track if LAYER SPACING has PARALLEL */
int Driver::hasSpLayer = 0;     /* keep track if LAYER SPACING has LAYER */
int Driver::hasGeoLayer = 0;    /* keep track if Geometries has LAYER */


// the following variables to keep track the number of warnings printed.
int Driver::antennaInoutWarnings = 0;
int Driver::antennaInputWarnings = 0;
int Driver::antennaOutputWarnings = 0;
int Driver::arrayWarnings = 0;
int Driver::caseSensitiveWarnings = 0;
int Driver::correctionTableWarnings = 0;
int Driver::dielectricWarnings = 0;
int Driver::edgeRateThreshold1Warnings = 0;
int Driver::edgeRateThreshold2Warnings = 0;
int Driver::edgeRateScaleFactorWarnings = 0;
int Driver::inoutAntennaWarnings = 0;
int Driver::inputAntennaWarnings = 0;
int Driver::iRDropWarnings = 0;
int Driver::layerWarnings = 0;
int Driver::macroWarnings = 0;
int Driver::maxStackViaWarnings = 0;
int Driver::minFeatureWarnings = 0;
int Driver::noiseMarginWarnings = 0;
int Driver::noiseTableWarnings = 0;
int Driver::nonDefaultWarnings = 0;
int Driver::noWireExtensionWarnings = 0;
int Driver::outputAntennaWarnings = 0;
int Driver::pinWarnings = 0;
int Driver::siteWarnings = 0;
int Driver::spacingWarnings = 0;
int Driver::timingWarnings = 0;
int Driver::unitsWarnings = 0;
int Driver::useMinSpacingWarnings = 0;
int Driver::viaRuleWarnings = 0;
int Driver::viaWarnings = 0;
double Driver::layerCutSpacing = 0;

int Driver::hasOpenedLogFile = 0; /* flag on how to open the warning log file */
int Driver::spaceMissing = 0;   /* flag to indicate if there is space after " */
int Driver::nDMsgs = 0; // disable message 

char* Driver::outMsg = NULL; // error messages
char Driver::lefPropDefType; /* save the current type of the property */
char Driver::cur_token[STRSIZE];      /* global so error message can print it */
char Driver::saved_token[STRSIZE];/* for an (illegal) usage ';TOKEN' */
char Driver::pv_token[STRSIZE];   /* previous token, for check ; without space */
int Driver::lefrRegisterUnused = 0;

bool read(LefDataBase& db, const string& lefFile)
{
	Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	return driver.parse_file(lefFile);
}

} // namespace example

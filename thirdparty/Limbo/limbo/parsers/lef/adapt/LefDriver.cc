/**
 * @file   LefDriver.cc
 * @author Yibo Lin
 * @date   Oct 2014
 * @brief  Implementation of @ref LefParser::Driver
 */

#include <limbo/parsers/lef/adapt/LefDriver.h>
#include <limbo/preprocessor/Msg.h>
#include <cctype>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <cmath>

namespace LefParser
{

Driver::Driver(LefDataBase& db)
    : trace_scanning(false),
      trace_parsing(false),
      m_db(db)
{
}
Driver::~Driver()
{
}

/// =========== callbacks ============
///@{

/// a local temporary global variable 
/// refer to Driver::m_db everytime parsing a file 
/// reset to NULL once parsing finished 
LefDataBase* lefDB = NULL; 
int parse65nm = 0;
int parseLef58Type = 0;
int isSessionless = 0; 
int relax = 0; 
const char* version = "N/A";
int setVer = 0;
int verStr = 0; 
int msgCb = 0; 
char* userData = NULL;

void checkType(lefrCallbackType_e c) 
{
    if (c >= 0 && c <= lefrLibraryEndCbkType) 
    {
        // OK
    } 
    else 
    {
        limboPrint(limbo::kERROR, "callback type is out of bounds!\n");
    }
}

int antennaCB(lefrCallbackType_e c, double value, lefiUserData)
{
    checkType(c);
    

    switch (c)
    {
        case lefrAntennaInputCbkType:
            lefDB->lef_antennainput_cbk(value); 
            break;
        case lefrAntennaInoutCbkType:
            lefDB->lef_antennainout_cbk(value); 
            break;
        case lefrAntennaOutputCbkType:
            lefDB->lef_antennaoutput_cbk(value); 
            break;
        case lefrInputAntennaCbkType:
            lefDB->lef_inputantenna_cbk(value); 
            break;
        case lefrOutputAntennaCbkType:
            lefDB->lef_outputantenna_cbk(value); 
            break;
        case lefrInoutAntennaCbkType:
            lefDB->lef_inoutantenna_cbk(value); 
            break;
        default:
            fprintf(stderr, "BOGUS antenna type.\n");
            break;
    }
    return 0;
}

int arrayBeginCB(lefrCallbackType_e c, const char* /*name*/, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int arrayCB(lefrCallbackType_e c, lefiArray* a, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_array_cbk(*a); 

    return 0;
}

int arrayEndCB(lefrCallbackType_e c, const char* /*name*/, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int busBitCharsCB(lefrCallbackType_e c, const char* busBit, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_busbitchars_cbk(busBit); 
    return 0;
}

int caseSensCB(lefrCallbackType_e c, int caseSense, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_casesensitive_cbk(caseSense); 
    return 0;
}

int fixedMaskCB(lefrCallbackType_e c, int fixedMask, lefiUserData)
{
    checkType(c);

    if (fixedMask == 1) 
        fprintf(stderr, "FIXEDMASK ;\n");
    return 0;
}

int clearanceCB(lefrCallbackType_e c, const char* name, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_clearancemeasure_cbk(name); 
    return 0;
}

int dividerCB(lefrCallbackType_e c, const char* name, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_dividerchar_cbk(name); 
    return 0;
}

int noWireExtCB(lefrCallbackType_e c, const char* name, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_nowireextension_cbk(name); 
    return 0;
}

int noiseMarCB(lefrCallbackType_e c, lefiNoiseMargin *data, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_noisemargin_cbk(*data); 
    return 0;
}

int edge1CB(lefrCallbackType_e c, double name, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_edgeratethreshold1_cbk(name); 
    return 0;
}

int edge2CB(lefrCallbackType_e c, double name, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_edgeratethreshold2_cbk(name); 
    return 0;
}

int edgeScaleCB(lefrCallbackType_e c, double name, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_edgeratescalefactor_cbk(name); 
    return 0;
}

int noiseTableCB(lefrCallbackType_e c, lefiNoiseTable *table, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_noisetable_cbk(*table);
    return 0;
}

int correctionCB(lefrCallbackType_e c, lefiCorrectionTable *table, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_correctiontable_cbk(*table); 
    return 0;
}

int dielectricCB(lefrCallbackType_e c, double dielectric, lefiUserData)
{
    checkType(c);
    

    lefDB->lef_dielectric_cbk(dielectric); 
    return 0;
}

int irdropBeginCB(lefrCallbackType_e c, void*, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int irdropCB(lefrCallbackType_e c, lefiIRDrop* irdrop, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_irdrop_cbk(*irdrop); 
    return 0;
}

int irdropEndCB(lefrCallbackType_e c, void*, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int layerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData)
{
    checkType(c);
    

    lefrSetCaseSensitivity(0);

    // Call parse65nmRules for 5.7 syntax in 5.6
    if (parse65nm)
        layer->lefiLayer::parse65nmRules();

    // Call parseLef58Type for 5.8 syntax in 5.7
    if (parseLef58Type)
        layer->lefiLayer::parseLEF58Layer();

    lefDB->lef_layer_cbk(*layer); 

    // Set it to case sensitive from here on
    lefrSetCaseSensitivity(1);

    return 0;
}

int macroBeginCB(lefrCallbackType_e c, const char* macroName, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_macrobegin_cbk(macroName); 
    return 0;
}

int macroFixedMaskCB(lefrCallbackType_e c, int, 
        lefiUserData)
{
    checkType(c);

    return 0;
}

int macroClassTypeCB(lefrCallbackType_e c, const char* /*macroClassType*/,
        lefiUserData)
{
    checkType(c);
    
    return 0;
}

int macroOriginCB(lefrCallbackType_e c, lefiNum,
        lefiUserData)
{
    checkType(c);
    
    return 0;
}

int macroSizeCB(lefrCallbackType_e c, lefiNum,
        lefiUserData)
{
    checkType(c);
    
    // fprintf(stderr, "  SIZE %g BY %g ;\n", macroNum.x, macroNum.y);
    return 0;
}

int macroCB(lefrCallbackType_e c, lefiMacro* macro, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_macro_cbk(*macro); 
    return 0;
}

int macroEndCB(lefrCallbackType_e c, const char* /*macroName*/, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int manufacturingCB(lefrCallbackType_e c, double num, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_manufacturing_cbk(num); 
    return 0;
}

int maxStackViaCB(lefrCallbackType_e c, lefiMaxStackVia* maxStack,
        lefiUserData)
{
    checkType(c);
    
    lefDB->lef_maxstackvia_cbk(*maxStack); 
    return 0;
}

int minFeatureCB(lefrCallbackType_e c, lefiMinFeature* min, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_minfeature_cbk(*min); 
    return 0;
}

int nonDefaultCB(lefrCallbackType_e c, lefiNonDefault* def, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_nondefault_cbk(*def); 

    return 0;
}

int obstructionCB(lefrCallbackType_e c, lefiObstruction* obs, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_obstruction_cbk(*obs); 
    return 0;
}

int pinCB(lefrCallbackType_e c, lefiPin* pin, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_pin_cbk(*pin); 
    return 0;  
}

int densityCB(lefrCallbackType_e c, lefiDensity* density,
        lefiUserData)
{
    checkType(c);
    
    lefDB->lef_density_cbk(*density); 
    return 0;
}

int propDefBeginCB(lefrCallbackType_e c, void*, lefiUserData)
{

    checkType(c);
    
    return 0;
}

int propDefCB(lefrCallbackType_e c, lefiProp* prop, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_prop_cbk(*prop); 
    return 0;
}

int propDefEndCB(lefrCallbackType_e c, void*, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int siteCB(lefrCallbackType_e c, lefiSite* site, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_site_cbk(*site); 
    return 0;
}

int spacingBeginCB(lefrCallbackType_e c, void*, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int spacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_spacing_cbk(*spacing); 
    return 0;
}

int spacingEndCB(lefrCallbackType_e c, void*, lefiUserData)
{
    checkType(c);
    
    return 0;
}

int timingCB(lefrCallbackType_e c, lefiTiming* timing, lefiUserData)
{
    checkType(c);
    lefDB->lef_timing_cbk(*timing); 
    return 0;
}

int unitsCB(lefrCallbackType_e c, lefiUnits* unit, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_units_cbk(*unit); 
    return 0;
}

int useMinSpacingCB(lefrCallbackType_e c, lefiUseMinSpacing* spacing,
        lefiUserData)
{
    checkType(c);
    
    lefDB->lef_useminspacing_cbk(*spacing); 
    return 0;
}

int versionCB(lefrCallbackType_e c, double num, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_version_cbk(num); 
    return 0;
}

int versionStrCB(lefrCallbackType_e c, const char* versionName, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_version_cbk(versionName); 
    return 0;
}

int viaCB(lefrCallbackType_e c, lefiVia* via, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_via_cbk(*via); 
    return 0;
}

int viaRuleCB(lefrCallbackType_e c, lefiViaRule* viaRule, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_viarule_cbk(*viaRule); 
    return 0;
}

int extensionCB(lefrCallbackType_e c, const char* extsn, lefiUserData)
{
    checkType(c);
    
    lefDB->lef_extension_cbk(extsn); 
    return 0;
}

int doneCB(lefrCallbackType_e c, void*, lefiUserData)
{
    checkType(c);
    
    return 0;
}

void errorCB(const char* msg)
{
    printf ("%s : %s\n", (const char*)lefrGetUserData(), msg);
}

void warningCB(const char* msg)
{
    printf ("%s : %s\n", (const char*)lefrGetUserData(), msg);
}

void* mallocCB(int size)
{
    return malloc(size);
}

void* reallocCB(void* name, int size)
{
    return realloc(name, size);
}

void freeCB(void* name)
{
    free(name);
    return;
}

void lineNumberCB(int lineNo)
{
    limboPrint(limbo::kINFO, "Parsed %d number of lines!!\n", lineNo);
    return;
}
void printWarning(const char *str)
{
    limboPrint(limbo::kWARN, "%s\n", str);
}
///@} =========== end of callbacks ============

bool Driver::parse_file(const std::string &filename)
{
    parse65nm = 0;
    parseLef58Type = 0;
    isSessionless = 0; 
    relax = 0; 
    version = "N/A";
    setVer = 0;
    verStr = 1; 
    msgCb = 0; 
    userData = strdup ("(lefrw-5100)");

    if (isSessionless) 
    {
        lefrSetOpenLogFileAppend();
    }

    lefrInitSession(isSessionless ? 0 : 1);

    lefrSetWarningLogFunction(printWarning);
    lefrSetAntennaInputCbk(antennaCB);
    lefrSetAntennaInoutCbk(antennaCB);
    lefrSetAntennaOutputCbk(antennaCB);
    lefrSetArrayBeginCbk(arrayBeginCB);
    lefrSetArrayCbk(arrayCB);
    lefrSetArrayEndCbk(arrayEndCB);
    lefrSetBusBitCharsCbk(busBitCharsCB);
    lefrSetCaseSensitiveCbk(caseSensCB);
    lefrSetFixedMaskCbk(fixedMaskCB);
    lefrSetClearanceMeasureCbk(clearanceCB);
    lefrSetDensityCbk(densityCB);
    lefrSetDividerCharCbk(dividerCB);
    lefrSetNoWireExtensionCbk(noWireExtCB);
    lefrSetNoiseMarginCbk(noiseMarCB);
    lefrSetEdgeRateThreshold1Cbk(edge1CB);
    lefrSetEdgeRateThreshold2Cbk(edge2CB);
    lefrSetEdgeRateScaleFactorCbk(edgeScaleCB);
    lefrSetExtensionCbk(extensionCB);
    lefrSetNoiseTableCbk(noiseTableCB);
    lefrSetCorrectionTableCbk(correctionCB);
    lefrSetDielectricCbk(dielectricCB);
    lefrSetIRDropBeginCbk(irdropBeginCB);
    lefrSetIRDropCbk(irdropCB);
    lefrSetIRDropEndCbk(irdropEndCB);
    lefrSetLayerCbk(layerCB);
    lefrSetLibraryEndCbk(doneCB); 
    lefrSetMacroBeginCbk(macroBeginCB);
    lefrSetMacroCbk(macroCB);
    lefrSetMacroClassTypeCbk(macroClassTypeCB);
    lefrSetMacroOriginCbk(macroOriginCB);
    lefrSetMacroSizeCbk(macroSizeCB);
    lefrSetMacroFixedMaskCbk(macroFixedMaskCB);
    lefrSetMacroEndCbk(macroEndCB);
    lefrSetManufacturingCbk(manufacturingCB);
    lefrSetMaxStackViaCbk(maxStackViaCB);
    lefrSetMinFeatureCbk(minFeatureCB);
    lefrSetNonDefaultCbk(nonDefaultCB);
    lefrSetObstructionCbk(obstructionCB);
    lefrSetPinCbk(pinCB);
    lefrSetPropBeginCbk(propDefBeginCB);
    lefrSetPropCbk(propDefCB);
    lefrSetPropEndCbk(propDefEndCB);
    lefrSetSiteCbk(siteCB);
    lefrSetSpacingBeginCbk(spacingBeginCB);
    lefrSetSpacingCbk(spacingCB);
    lefrSetSpacingEndCbk(spacingEndCB);
    lefrSetTimingCbk(timingCB);
    lefrSetUnitsCbk(unitsCB);
    lefrSetUseMinSpacingCbk(useMinSpacingCB);
    lefrSetUserData((void*)3);
    if (!verStr)
        lefrSetVersionCbk(versionCB);
    else
        lefrSetVersionStrCbk(versionStrCB);
    lefrSetViaCbk(viaCB);
    lefrSetViaRuleCbk(viaRuleCB);
    lefrSetInputAntennaCbk(antennaCB);
    lefrSetOutputAntennaCbk(antennaCB);
    lefrSetInoutAntennaCbk(antennaCB);

    if (msgCb)
    {
        lefrSetLogFunction(errorCB);
        lefrSetWarningLogFunction(warningCB);
    }

    lefrSetMallocFunction(mallocCB);
    lefrSetReallocFunction(reallocCB);
    lefrSetFreeFunction(freeCB);

    //lefrSetLineNumberFunction(lineNumberCB);
    lefrSetDeltaNumberLines(50);

    lefrSetRegisterUnusedCallbacks();

    if (relax)
        lefrSetRelaxMode();

    if (setVer)
        (void)lefrSetVersionValue(version);

    lefrSetAntennaInoutWarnings(30);
    lefrSetAntennaInputWarnings(30);
    lefrSetAntennaOutputWarnings(30);
    lefrSetArrayWarnings(30);
    lefrSetCaseSensitiveWarnings(30);
    lefrSetCorrectionTableWarnings(30);
    lefrSetDielectricWarnings(30);
    lefrSetEdgeRateThreshold1Warnings(30);
    lefrSetEdgeRateThreshold2Warnings(30);
    lefrSetEdgeRateScaleFactorWarnings(30);
    lefrSetInoutAntennaWarnings(30);
    lefrSetInputAntennaWarnings(30);
    lefrSetIRDropWarnings(30);
    lefrSetLayerWarnings(30);
    lefrSetMacroWarnings(30);
    lefrSetMaxStackViaWarnings(30);
    lefrSetMinFeatureWarnings(30);
    lefrSetNoiseMarginWarnings(30);
    lefrSetNoiseTableWarnings(30);
    lefrSetNonDefaultWarnings(30);
    lefrSetNoWireExtensionWarnings(30);
    lefrSetOutputAntennaWarnings(30);
    lefrSetPinWarnings(30);
    lefrSetSiteWarnings(30);
    lefrSetSpacingWarnings(30);
    lefrSetTimingWarnings(30);
    lefrSetUnitsWarnings(30);
    lefrSetUseMinSpacingWarnings(30);
    lefrSetViaRuleWarnings(30);
    lefrSetViaWarnings(30);


    lefrReset(); 
    FILE* f = fopen(filename.c_str(), "r"); 
    if (!f)
    {
        std::cerr << "Could not open input file " << filename << "\n"; 
        return false; 
    }
    (void)lefrEnableReadEncrypted();

    // set lefDB 
    lefDB = &m_db; 
    // kernel to read lef file 
    int res = lefrRead(f, filename.c_str(), (void*)userData);
    // reset to NULL 
    lefDB = NULL; 
    if (res)
        std::cerr << "Reader returns bad status\n"; 

    (void)lefrPrintUnusedCallbacks(stderr);
    (void)lefrReleaseNResetMemory();
    //(void)lefrUnsetCallbacks();
    (void)lefrUnsetLayerCbk();
    (void)lefrUnsetNonDefaultCbk();
    (void)lefrUnsetViaCbk();
    // Unset all the callbacks
    void lefrUnsetAntennaInputCbk();
    void lefrUnsetAntennaInoutCbk();
    void lefrUnsetAntennaOutputCbk();
    void lefrUnsetArrayBeginCbk();
    void lefrUnsetArrayCbk();
    void lefrUnsetArrayEndCbk();
    void lefrUnsetBusBitCharsCbk();
    void lefrUnsetCaseSensitiveCbk();
    void lefrUnsetFixedMaskCbk();
    void lefrUnsetClearanceMeasureCbk();
    void lefrUnsetCorrectionTableCbk();
    void lefrUnsetDensityCbk();
    void lefrUnsetDielectricCbk();
    void lefrUnsetDividerCharCbk();
    void lefrUnsetEdgeRateScaleFactorCbk();
    void lefrUnsetEdgeRateThreshold1Cbk();
    void lefrUnsetEdgeRateThreshold2Cbk();
    void lefrUnsetExtensionCbk();
    void lefrUnsetInoutAntennaCbk();
    void lefrUnsetInputAntennaCbk();
    void lefrUnsetIRDropBeginCbk();
    void lefrUnsetIRDropCbk();
    void lefrUnsetIRDropEndCbk();
    void lefrUnsetLayerCbk();
    void lefrUnsetLibraryEndCbk();
    void lefrUnsetMacroBeginCbk();
    void lefrUnsetMacroCbk();
    void lefrUnsetMacroClassTypeCbk();
    void lefrUnsetMacroEndCbk();
    void lefrUnsetMacroOriginCbk();
    void lefrUnsetMacroSizeCbk();
    void lefrUnsetManufacturingCbk();
    void lefrUnsetMaxStackViaCbk();
    void lefrUnsetMinFeatureCbk();
    void lefrUnsetNoiseMarginCbk();
    void lefrUnsetNoiseTableCbk();
    void lefrUnsetNonDefaultCbk();
    void lefrUnsetNoWireExtensionCbk();
    void lefrUnsetObstructionCbk();
    void lefrUnsetOutputAntennaCbk();
    void lefrUnsetPinCbk();
    void lefrUnsetPropBeginCbk();
    void lefrUnsetPropCbk();
    void lefrUnsetPropEndCbk();
    void lefrUnsetSiteCbk();
    void lefrUnsetSpacingBeginCbk();
    void lefrUnsetSpacingCbk();
    void lefrUnsetSpacingEndCbk();
    void lefrUnsetTimingCbk();
    void lefrUnsetUseMinSpacingCbk();
    void lefrUnsetUnitsCbk();
    void lefrUnsetVersionCbk();
    void lefrUnsetVersionStrCbk();
    void lefrUnsetViaCbk();
    void lefrUnsetViaRuleCbk();

    lefrClear(); 
    fclose(f); 
    free(userData); 

    return true; 
}

bool read(LefDataBase& db, const string& lefFile)
{
	Driver driver (db);
	//driver.trace_scanning = true;
	//driver.trace_parsing = true;

	return driver.parse_file(lefFile);
}

} // namespace example

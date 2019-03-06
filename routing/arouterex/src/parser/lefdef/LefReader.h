//
// Created by anoynmous on 09/05/2018
//

#ifndef AROUTER_LEFREADER_H_
#define AROUTER_LEFREADER_H_

#include "db/techDB/TechDatabase.h"
#include "db/macroDB/MacroDatabase.h"
#include "LefDriver.h"
#include "LefReaderComponents.h"

/// ================================================================================ 
/// LefReader.h
/// 
/// Read technology and macro information from the lef files
/// 
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

class LefReader : public LefParser::LefDataBase
{
    public:

        explicit LefReader(TechDatabase &techDB, MacroDatabase &macroDB) : _techDB(techDB), _macroDB(macroDB) {}

        /////////////////////
        /// Wrapper Routines
        /////////////////////
        /// Post process the raw data and load to database
        void postProcessing();

        ////////////////
        /// Getters
        ////////////////
        const TechDatabase &    techDB() const      { return _techDB; }
        TechDatabase &          techDB()            { return _techDB; }
        const MacroDatabase &   macroDB() const     { return _macroDB; }
        MacroDatabase &         macroDB()           { return _macroDB; }

        ////////////////////////
        /// Override functions
        ////////////////////////
        void lef_version_cbk(const std::string &v) override;
        void lef_version_cbk(double v) override;
        void lef_dividerchar_cbk(const std::string &v) override;
        void lef_casesensitive_cbk(int v) override;
        void lef_nowireextension_cbk(const std::string &v) override;
        void lef_manufacturing_cbk(double v) override;
        void lef_useminspacing_cbk(lefiUseMinSpacing const &v) override;
        void lef_clearancemeasure_cbk(const std::string &v) override;
        void lef_units_cbk(lefiUnits const &v) override;
        void lef_busbitchars_cbk(const std::string &v) override;
        void lef_layer_cbk(lefiLayer &v) override;
        void lef_maxstackvia_cbk(lefiMaxStackVia const &v) override;
        void lef_via_cbk(lefiVia const &v) override;
        void lef_viarule_cbk(lefiViaRule const &v) override;
        void lef_spacing_cbk(lefiSpacing const &v) override;
        void lef_irdrop_cbk(lefiIRDrop const &v) override;
        void lef_minfeature_cbk(lefiMinFeature const &v) override;
        void lef_dielectric_cbk(double v) override;
        void lef_nondefault_cbk(lefiNonDefault const &v) override;
        void lef_site_cbk(lefiSite const &v) override;
        void lef_macrobegin_cbk(std::string const &v) override;
        void lef_macro_cbk(lefiMacro const &v) override;
        void lef_pin_cbk(lefiPin const &v) override;
        void lef_obstruction_cbk(lefiObstruction const &v) override;
        void lef_density_cbk(lefiDensity const &v) override;
        void lef_timing_cbk(lefiTiming const &v) override;
        void lef_array_cbk(lefiArray const &v) override;
        void lef_prop_cbk(lefiProp const &v) override;
        void lef_noisemargin_cbk(lefiNoiseMargin const &v) override;
        void lef_edgeratethreshold1_cbk(double v) override;
        void lef_edgeratethreshold2_cbk(double v) override;
        void lef_edgeratescalefactor_cbk(double v) override;
        void lef_noisetable_cbk(lefiNoiseTable const &v) override;
        void lef_correctiontable_cbk(lefiCorrectionTable const &v) override;
        void lef_inputantenna_cbk(double v) override;
        void lef_outputantenna_cbk(double v) override;
        void lef_inoutantenna_cbk(double v) override;
        void lef_antennainput_cbk(double v) override;
        void lef_antennaoutput_cbk(double v) override;
        void lef_antennainout_cbk(double v) override;
        void lef_extension_cbk(const std::string &v) override;
        void lef_macro_endcbk(const char *macroName) override;

    private:
        /// Read a CUT type layer, return the idx of this in the cut layers
        IndexType parseCutLayer(lefiLayer const &v); 
        /// Read a ROUTING type layer, return the index of the input in the ROUTING layers
        IndexType parseRoutingLayer(lefiLayer &v);
        ///  Read a MASTERSLICE type layer, return the index of the input the masterslice layers
        IndexType parseMastersliceLayer(const lefiLayer &v);
        /// Process the spacing information in a Routing layer
        void processRoutingLayerSpacing(LefLayerRouting &route, lefiLayer const &v, IntType spacingIdx);
        /// process the spacingtable parallelrun in a routing layer
        void processRoutingLayerSpacingTableParallel(LefLayerRouting &route, lefiSpacingTable const *v);

        /// via
        void parseFixedVia(const lefiVia &v);
        void postProcessFixedVia(const LefRawFixedVia &rawVia);
    private:
        TechDatabase &_techDB;
        MacroDatabase &_macroDB;

        /// raw data
        std::vector<LefRawFixedVia> _rawFixedViaArray;
};

namespace LefReaderNameSpace {
void readLef(std::string const &fileName, TechDatabase &techDB, MacroDatabase &macroDB);
}
PROJECT_NAMESPACE_END

#endif /// AROUTER_LEFREADER_H_

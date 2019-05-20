/**
 * @file   LefDataBase.h
 * @brief  Database for Lef parser 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef LEFPARSER_DATABASE_H
#define LEFPARSER_DATABASE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <limbo/thirdparty/lefdef/5.8/lef/include/lefrReader.hpp>
#include <limbo/thirdparty/lefdef/5.8/lef/include/lefwWriter.hpp>
#include <limbo/thirdparty/lefdef/5.8/lef/include/lefiDebug.hpp>
#include <limbo/thirdparty/lefdef/5.8/lef/include/lefiEncryptInt.hpp>
#include <limbo/thirdparty/lefdef/5.8/lef/include/lefiUtil.hpp>

/// namespace for LefParser
namespace LefParser {

/// @nowarn 
using std::cout;
using std::endl;
using std::cerr;
using std::string; 
using std::vector;
using std::pair;
using std::make_pair;
using std::ostringstream;
/// @endnowarn

/// @cond
typedef LefDefParser::lefiArrayFloorPlan lefiArrayFloorPlan;
typedef LefDefParser::lefiArray lefiArray;
typedef LefDefParser::lefiNoiseVictim lefiNoiseVictim;
typedef LefDefParser::lefiNoiseResistance lefiNoiseResistance;
typedef LefDefParser::lefiNoiseEdge lefiNoiseEdge;
typedef LefDefParser::lefiNoiseTable lefiNoiseTable;
typedef LefDefParser::lefiCorrectionVictim lefiCorrectionVictim;
typedef LefDefParser::lefiCorrectionResistance lefiCorrectionResistance;
typedef LefDefParser::lefiCorrectionEdge lefiCorrectionEdge;
typedef LefDefParser::lefiCorrectionTable lefiCorrectionTable;
typedef LefDefParser::lefiAntennaPWL lefiAntennaPWL;
typedef LefDefParser::lefiLayerDensity lefiLayerDensity;
typedef LefDefParser::lefiParallel lefiParallel;
typedef LefDefParser::lefiInfluence lefiInfluence;
typedef LefDefParser::lefiTwoWidths lefiTwoWidths;
typedef LefDefParser::lefiSpacingTable lefiSpacingTable;
typedef LefDefParser::lefiOrthogonal lefiOrthogonal;
typedef LefDefParser::lefiAntennaModel lefiAntennaModel;
typedef LefDefParser::lefiLayer lefiLayer;
typedef LefDefParser::lefiObstruction lefiObstruction;
typedef LefDefParser::lefiPinAntennaModel lefiPinAntennaModel;
typedef LefDefParser::lefiPin lefiPin;
typedef LefDefParser::lefiDensity lefiDensity;
typedef LefDefParser::lefiMacro lefiMacro;
typedef LefDefParser::lefiTiming lefiTiming;
typedef LefDefParser::lefiGeometries lefiGeometries;
typedef LefDefParser::lefiSpacing lefiSpacing;
typedef LefDefParser::lefiIRDrop lefiIRDrop;
typedef LefDefParser::lefiMinFeature lefiMinFeature;
typedef LefDefParser::lefiSite lefiSite;
typedef LefDefParser::lefiSitePattern lefiSitePattern;
typedef LefDefParser::lefiTrackPattern lefiTrackPattern;
typedef LefDefParser::lefiGcellPattern lefiGcellPattern;
typedef LefDefParser::lefiUseMinSpacing lefiUseMinSpacing;
typedef LefDefParser::lefiMaxStackVia lefiMaxStackVia;
typedef LefDefParser::lefiNonDefault lefiNonDefault;
typedef LefDefParser::lefiProp lefiProp;
typedef LefDefParser::lefiPropType lefiPropType;
typedef LefDefParser::lefiUnits lefiUnits;
typedef LefDefParser::lefiViaLayer lefiViaLayer;
typedef LefDefParser::lefiVia lefiVia;
typedef LefDefParser::lefiViaRuleLayer lefiViaRuleLayer;
typedef LefDefParser::lefiViaRule lefiViaRule;

typedef LefDefParser::lefiNoiseMargin lefiNoiseMargin;
typedef LefDefParser::lefiGeomRect lefiGeomRect;
typedef LefDefParser::lefiGeomRectIter lefiGeomRectIter;
typedef LefDefParser::lefiGeomPath lefiGeomPath;
typedef LefDefParser::lefiGeomPathIter lefiGeomPathIter;
typedef LefDefParser::lefiGeomPolygon lefiGeomPolygon;
typedef LefDefParser::lefiGeomPolygonIter lefiGeomPolygonIter;
typedef LefDefParser::lefiGeomVia lefiGeomVia;
typedef LefDefParser::lefiGeomViaIter lefiGeomViaIter;
typedef LefDefParser::lefiPoints lefiPoints;

enum lefiGeomEnum {
  lefiGeomUnknown = LefDefParser::lefiGeomUnknown,
  lefiGeomLayerE = LefDefParser::lefiGeomLayerE,
  lefiGeomLayerExceptPgNetE = LefDefParser::lefiGeomLayerExceptPgNetE,
  lefiGeomLayerMinSpacingE = LefDefParser::lefiGeomLayerMinSpacingE,
  lefiGeomLayerRuleWidthE = LefDefParser::lefiGeomLayerRuleWidthE,
  lefiGeomWidthE = LefDefParser::lefiGeomWidthE,
  lefiGeomPathE = LefDefParser::lefiGeomPathE,
  lefiGeomPathIterE = LefDefParser::lefiGeomPathIterE,
  lefiGeomRectE = LefDefParser::lefiGeomRectE,
  lefiGeomRectIterE = LefDefParser::lefiGeomRectIterE,
  lefiGeomPolygonE = LefDefParser::lefiGeomPolygonE,
  lefiGeomPolygonIterE = LefDefParser::lefiGeomPolygonIterE,
  lefiGeomViaE = LefDefParser::lefiGeomViaE,
  lefiGeomViaIterE = LefDefParser::lefiGeomViaIterE,
  lefiGeomClassE = LefDefParser::lefiGeomClassE,
  lefiGeomEnd = LefDefParser::lefiGeomEnd
};
/// @endcond

// forward declaration
/// @class LefParser::LefDataBase
/// @brief Base class for lef database. 
/// Only pure virtual functions are defined.  
/// User needs to inheritate this class and derive a custom database type with all callback functions defined.  
/// 
/// Although it is true that I can use pure virtual functions for all callbacks, 
/// run-time information is better than compile-time errors.
/// Because most LEF files are so simple that we only need several callbacks.  
/// Then user does not need to provide some callbacks.
class LefDataBase
{
	public:
        /// @brief constructor 
		LefDataBase();
        /// @return current version 
		double current_version() const;
        /// @brief set current version 
        /// @param cv current version 
		void current_version(double cv);

        // Although it is true that I can use pure virtual functions for all callbacks, 
        // run-time information is better than compile-time errors.
        // Because most LEF files are so simple that we only need several callbacks.  
        // Then user does not need to provide some callbacks.

        /// @brief set LEF version 
        /// @param v string of LEF version 
		virtual void lef_version_cbk(string const& v);
        /// @brief set LEF version 
        /// @param v floating point number of LEF version 
		virtual void lef_version_cbk(double v);
        /// @brief set divider characters 
        /// @param v divider characters
		virtual void lef_dividerchar_cbk(string const& v);
        /// @brief set case sensitive flag 
        /// @param v case sensitive flag 
		virtual void lef_casesensitive_cbk(int v);
        /// @brief set no wire extension flag 
        /// @param v no wire extension flag 
		virtual void lef_nowireextension_cbk(string const& v);
        /// @brief set manufacturing entry 
        /// @param v manufacturing entry 
		virtual void lef_manufacturing_cbk(double v);
        /// @brief set use min space 
        /// @param v an object for use min space 
		virtual void lef_useminspacing_cbk(lefiUseMinSpacing const& v);
        /// @brief set clearance measure 
        /// @param v clearance measure 
		virtual void lef_clearancemeasure_cbk(string const& v);
        /// @brief set unit 
        /// @param v an object for unit 
		virtual void lef_units_cbk(lefiUnits const& v);
        /// @brief set bus bit characters 
        /// @param v but bit characters 
		virtual void lef_busbitchars_cbk(string const& v);
        /// @brief add layer 
        /// @param v an object for layer 
		virtual void lef_layer_cbk(lefiLayer const& v);
        /// @brief max stack via 
        /// @param v an object for max stack via 
		virtual void lef_maxstackvia_cbk(lefiMaxStackVia const& v);
        /// @brief add via 
        /// @param v an object for via 
		virtual void lef_via_cbk(lefiVia const& v);
        /// @brief add via rule 
        /// @param v an object for via rule 
		virtual void lef_viarule_cbk(lefiViaRule const& v);
        /// @brief spacing callback 
        /// @param v an object for spacing 
		virtual void lef_spacing_cbk(lefiSpacing const& v);
        /// @brief IR drop callback 
        /// @param v an object for IR drop 
		virtual void lef_irdrop_cbk(lefiIRDrop const& v);
        /// @brief min feature callback 
        /// @param v an object for min feature 
		virtual void lef_minfeature_cbk(lefiMinFeature const& v);
        /// @brief dielectric callback 
        /// @param v an object for dielectric 
		virtual void lef_dielectric_cbk(double v);
        /// @brief non-default rule callback 
        /// @param v an object for non-default rule 
		virtual void lef_nondefault_cbk(lefiNonDefault const& v);
        /// @brief site callback 
        /// @param v an object for site 
		virtual void lef_site_cbk(lefiSite const& v);
        /// @brief macro begin callback, describe standard cell type 
        /// @param v name of macro 
		virtual void lef_macrobegin_cbk(std::string const& v);
        /// @brief macro callback, describe standard cell type 
        /// @param v an object for macro 
		virtual void lef_macro_cbk(lefiMacro const& v);
        /// @brief pin callback, describe pins in a standard cell 
        /// @param v an object for pin 
		virtual void lef_pin_cbk(lefiPin const& v);
        /// @brief obstruction callback, describe obstructions in a standard cell
        /// @param v an object for obstruction  
		virtual void lef_obstruction_cbk(lefiObstruction const& v);
        /// @brief density callback 
        /// @param v an object for density 
		virtual void lef_density_cbk(lefiDensity const& v);
        /// @brief timing callback 
        /// @param v an object for timing 
		virtual void lef_timing_cbk(lefiTiming const& v);
        /// @brief array callback 
        /// @param v an object for array 
		virtual void lef_array_cbk(lefiArray const& v);
        /// @brief property callback 
        /// @param v an object for property 
		virtual void lef_prop_cbk(lefiProp const& v);
        /// @brief noise margin callback 
        /// @param v an object for noise margin 
		virtual void lef_noisemargin_cbk(lefiNoiseMargin const& v);
        /// @brief edge rate threshold 1 
        /// @param v edge rate threshold 1 
		virtual void lef_edgeratethreshold1_cbk(double v);
        /// @brief edge rate threshold 2 
        /// @param v edge rate threshold 2 
		virtual void lef_edgeratethreshold2_cbk(double v);
        /// @brief edge rate scale factor 
        /// @param v edge rate scale factor 
		virtual void lef_edgeratescalefactor_cbk(double v);
        /// @brief noise table callback 
        /// @param v an object for noise table 
		virtual void lef_noisetable_cbk(lefiNoiseTable const& v);
        /// @brief correction table callback 
        /// @param v an object for correction table 
		virtual void lef_correctiontable_cbk(lefiCorrectionTable const& v);
        /// @brief input antenna callback 
        /// @param v input antenna 
		virtual void lef_inputantenna_cbk(double v);
        /// @brief output antenna callback 
        /// @param v output antenna 
		virtual void lef_outputantenna_cbk(double v);
        /// @brief inout antenna callback 
        /// @param v inout antenna 
		virtual void lef_inoutantenna_cbk(double v);
        /// @brief antenna input callback 
        /// @param v antenna input 
		virtual void lef_antennainput_cbk(double v);
        /// @brief antenna output callback 
        /// @param v antenna output 
		virtual void lef_antennaoutput_cbk(double v);
        /// @brief antenna inout callback 
        /// @param v antenna inout 
		virtual void lef_antennainout_cbk(double v);
        /// @brief extension callback 
        /// @param v extension 
		virtual void lef_extension_cbk(string const& v);
	protected:
		double m_current_version; /*!< default current lef version, use static variable to mimic definitions */
	public:
		static int lefNamesCaseSensitive; /*!< case sensitive or not, mimic global variable lefNamesCaseSensitive */
		static int lefrShiftCase; /*!< shift to upper case or not  */
	private:
        /// @brief remind users to define some optional callback functions at runtime 
        /// @param str message including the information to the callback function in the reminder 
		void lef_user_cbk_reminder(const char* str) const;
};

} // namespace LefParser

#endif

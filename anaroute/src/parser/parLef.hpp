/**
 * @file   parLef.hpp
 * @brief  Lef file parser using Limbo wrapper
 * @author Hao Chen
 * @date   09/09/2019
 *
 **/

#ifndef _PAR_LEF_HPP_
#define _PAR_LEF_HPP_

#include <limbo/parsers/lef/adapt/LefDriver.h>

#include "src/global/global.hpp"
#include "src/db/dbLef.hpp"

PROJECT_NAMESPACE_START

/**
 * @class LefReader
 * @brief Inherit the Limbo lef wrapper to define functions
 *
 * **/


class LefReader : public LefParser::LefDataBase { // LefParser::LefDataBase (Limbo)
 public:
  LefReader(LefDB& t) : _lef(t) {}
  ~LefReader() {}

  void parse(const String_t& filename);
  // Override functions
  void lef_version_cbk(String_t const& v) override;
  void lef_version_cbk(double v) override;
  void lef_dividerchar_cbk(const String_t &v) override;
  void lef_casesensitive_cbk(int v) override;
  void lef_nowireextension_cbk(const String_t &v) override;
  void lef_manufacturing_cbk(double v) override;
  void lef_useminspacing_cbk(lefiUseMinSpacing const &v) override;
  void lef_clearancemeasure_cbk(const String_t &v) override;
  void lef_units_cbk(lefiUnits const &v) override;
  void lef_busbitchars_cbk(const String_t &v) override;
  void lef_layer_cbk(lefiLayer const &v) override;
  void lef_maxstackvia_cbk(lefiMaxStackVia const &v) override;
  void lef_via_cbk(lefiVia const &v) override;
  void lef_viarule_cbk(lefiViaRule const &v) override;
  void lef_spacing_cbk(lefiSpacing const &v) override;
  void lef_irdrop_cbk(lefiIRDrop const &v) override;
  void lef_minfeature_cbk(lefiMinFeature const &v) override;
  void lef_dielectric_cbk(double v) override;
  void lef_nondefault_cbk(lefiNonDefault const &v) override;
  void lef_site_cbk(lefiSite const &v) override;
  void lef_macrobegin_cbk(String_t const &v) override;
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
  void lef_extension_cbk(const String_t &v) override;

 private:
  LefDB& _lef;

  /////////////////////////////////////////
  //    Private functions                //
  /////////////////////////////////////////
  void parseImplantLayer(const lefiLayer& v);
  void parseMastersliceLayer(const lefiLayer& v);
  void parseCutLayer(const lefiLayer& v);
  void parseRoutingLayer(const lefiLayer& v);
  void parseOverlapLayer(const lefiLayer& v);
  void parseDefaultVia(const lefiVia& v);
  /////////////////////////////////////////
  //    Helper functions                 //
  /////////////////////////////////////////
  Int_t to_lef_unit_1d(const Float_t n) const; // to unit length
  Int_t to_lef_unit_2d(const Float_t n) const; // to unit area
  Int_t to_lef_unit_resistance(const Float_t n) const; // to unit resistance
};


PROJECT_NAMESPACE_END

#endif /// _PAR_LEF_HPP_

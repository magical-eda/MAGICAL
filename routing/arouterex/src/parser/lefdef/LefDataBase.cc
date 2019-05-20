/**
 @file   LefDataBase.cpp
 @author Yibo Lin
 @date   Dec 2014
 @brief  Implementation of @ref LefParser::LefDataBase
 */

#include "LefDataBase.h"
#include <cstring>
#include <cstdlib>

namespace LefParser {

LefDataBase::LefDataBase()
{
	m_current_version = 5.6;
}
double LefDataBase::current_version() const 
{return m_current_version;}
void LefDataBase::current_version(double cv)
{m_current_version = cv;}

int LefDataBase::lefNamesCaseSensitive = TRUE;
int LefDataBase::lefrShiftCase = FALSE;


void LefDataBase::lef_version_cbk(string const& v)
{
	cout << "LEF version: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_version_cbk(double v)
{
	cout << "LEF version: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_dividerchar_cbk(string const& v)
{
	cout << "LEF dividerchar: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_casesensitive_cbk(int v)
{
	cout << "LEF casesensitive: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_nowireextension_cbk(string const& v)
{
	cout << "LEF nowireextension: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_manufacturing_cbk(double v)
{
	cout << "LEF manufacturing: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_useminspacing_cbk(lefiUseMinSpacing const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_clearancemeasure_cbk(string const& v)
{
	cout << "LEF clearancemeasure: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_units_cbk(lefiUnits const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_busbitchars_cbk(string const& v)
{
	cout << "LEF busbitchars: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_layer_cbk(lefiLayer & v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_maxstackvia_cbk(lefiMaxStackVia const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_via_cbk(lefiVia const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_viarule_cbk(lefiViaRule const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_spacing_cbk(lefiSpacing const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_irdrop_cbk(lefiIRDrop const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_minfeature_cbk(lefiMinFeature const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_dielectric_cbk(double v)
{
	cout << "LEF dielectric: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_nondefault_cbk(lefiNonDefault const& /*v*/)
{
	//v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_site_cbk(lefiSite const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_macrobegin_cbk(std::string const& v)
{
    //cout << "macro begin " << v << endl;
    lef_user_cbk_reminder(__func__); 
}
void LefDataBase::lef_macro_cbk(lefiMacro const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_pin_cbk(lefiPin const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_obstruction_cbk(lefiObstruction const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_density_cbk(lefiDensity const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_timing_cbk(lefiTiming const& )
{
	//v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_array_cbk(lefiArray const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_prop_cbk(lefiProp const& v)
{
	v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_noisemargin_cbk(lefiNoiseMargin const& )
{
	//v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_edgeratethreshold1_cbk(double v)
{
	cout << "LEF edgeratethreshold1: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_edgeratethreshold2_cbk(double v)
{
	cout << "LEF edgeratethreshold2: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_edgeratescalefactor_cbk(double v)
{
	cout << "LEF edgeratescalefactor: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_noisetable_cbk(lefiNoiseTable const& )
{
	//v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_correctiontable_cbk(lefiCorrectionTable const& )
{
	//v.print(stdout);
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_inputantenna_cbk(double v)
{
	cout << "LEF inputantenna: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_outputantenna_cbk(double v)
{
	cout << "LEF outputantenna: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_inoutantenna_cbk(double v)
{
	cout << "LEF inoutantenna: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_antennainput_cbk(double v)
{
	cout << "LEF antennainput: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_antennaoutput_cbk(double v)
{
	cout << "LEF antennaoutput: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_antennainout_cbk(double v)
{
	cout << "LEF antennainout: " << v << endl;
	lef_user_cbk_reminder(__func__);
}
void LefDataBase::lef_extension_cbk(string const& v)
{
	cout << "LEF extension: " << v << endl;
	lef_user_cbk_reminder(__func__);
}

void LefDataBase::lef_macro_endcbk(const char* macroName){
    cout<<"Macro end "<<macroName<<endl;
}

void LefDataBase::lef_user_cbk_reminder(const char* str) const 
{
	//cout << "A corresponding user-defined callback is necessary: " << str << endl;
	//exit(0);
}

} // namespace LefParser

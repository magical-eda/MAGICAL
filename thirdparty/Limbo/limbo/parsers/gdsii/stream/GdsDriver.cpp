/**
 @file   GdsDriver.cpp
 @author Yibo Lin
 @date   Nov 2014
 @brief  Driver for GDSII 
 */

#include <limbo/parsers/gdsii/stream/GdsDriver.h>

namespace GdsParser 
{

GdsDriver::GdsDriver(GdsDriver::database_type& db) : m_db(db) {}

bool GdsDriver::operator()(string const& filename)
{
	// call GdsReader to read gds file 
	return read(*this, filename);
}

void GdsDriver::bit_array_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vBitArray)
{this->general_cbk(ascii_record_type, ascii_data_type, vBitArray);}
void GdsDriver::integer_2_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger)
{this->general_cbk(ascii_record_type, ascii_data_type, vInteger);}
void GdsDriver::integer_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger)
{this->general_cbk(ascii_record_type, ascii_data_type, vInteger);}
void GdsDriver::real_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) 
{this->general_cbk(ascii_record_type, ascii_data_type, vFloat);}
void GdsDriver::real_8_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) 
{this->general_cbk(ascii_record_type, ascii_data_type, vFloat);}
void GdsDriver::string_cbk(const char* ascii_record_type, const char* ascii_data_type, string const& str) 
{this->general_cbk(ascii_record_type, ascii_data_type, str);}
void GdsDriver::begin_end_cbk(const char* ascii_record_type)
{this->general_cbk(ascii_record_type, "", vector<int>());}

/// top api function for GdsDriver
bool read(GdsDriverDataBase& db, string const& filename)
{
	return GdsDriver(db)(filename);
}

} // namespace GdsParser

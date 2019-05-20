/**
 * @file   GdsReader.h
 * @brief  read GDSII file 
 * @author Yibo Lin
 * @date   Oct 2014
 */

#ifndef _GDSPARSER_GDSREADER_H
#define _GDSPARSER_GDSREADER_H

#include <string>
#include <vector>
#include <istream>
#include <limbo/parsers/gdsii/stream/GdsRecords.h>
#include <limbo/string/String.h>

using std::string;
using std::vector;

/// namespace for Limbo.GdsParser 
namespace GdsParser 
{

/// =============================================================
/// Why do I need to wrap GdsDataBaseKernel into GdsDataBase?
/// This is mainly for backward compatibility reasons. 
/// In old version limbo library, user only needs to implement child database class for GdsDataBase 
/// and use those ascii callbacks. 
/// I hope user does not need to change their code in the new version, so the old api must still be available. 
/// By adding new base class GdsDataBaseKernel, it is possible to achieve both. 
/// If user want to use old api, they can write child class for GdsDataBase, 
/// while if they want to use new api, please inheritate GdsDataBaseKernel. 
/// =============================================================

/// @class GdsParser::GdsDataBaseKernel
/// @brief Kernel callbacks for GdsReader.
/// These callbacks use enum for record_type and data_type, 
/// which is faster than ascii callbacks.
class GdsDataBaseKernel
{
	public:
        /// @brief bit array callback 
        /// @param record_type record 
        /// @param data_type data type 
        /// @param vBitArray data array  
		virtual void bit_array_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vBitArray) = 0;
        /// @brief 2-byte integer callback 
        /// @param record_type record 
        /// @param data_type data type 
        /// @param vInteger data array  
		virtual void integer_2_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vInteger) = 0;
        /// @brief 4-byte integer callback 
        /// @param record_type record 
        /// @param data_type data type 
        /// @param vInteger data array  
		virtual void integer_4_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vInteger) = 0;
        /// @brief 4-byte floating point number callback 
        /// @param record_type record 
        /// @param data_type data type 
        /// @param vFloat data array  
		virtual void real_4_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<double> const& vFloat) = 0;
        /// @brief 8-byte floating point number callback 
        /// @param record_type record 
        /// @param data_type data type 
        /// @param vFloat data array  
		virtual void real_8_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<double> const& vFloat) = 0;
        /// @brief string callback 
        /// @param record_type record 
        /// @param data_type data type 
        /// @param str data 
		virtual void string_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, string const& str) = 0;
        /// @brief begin or end indicator of a block 
        /// @param record_type record 
		virtual void begin_end_cbk(GdsRecords::EnumType record_type) = 0; 
};

/// =============================================================
/// @class GdsParser::GdsDataBase
/// @brief GdsDataBase redirects callbacks of GdsDataBaseKernel to ascii callbacks
/// =============================================================
class GdsDataBase : public GdsDataBaseKernel
{
	public:
        /// @brief bit array callback 
        /// @param ascii_record_type record 
        /// @param ascii_data_type data type 
        /// @param vBitArray data array  
		virtual void bit_array_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vBitArray) = 0;
        /// @brief 2-byte integer callback 
        /// @param ascii_record_type record 
        /// @param ascii_data_type data type 
        /// @param vInteger data array  
		virtual void integer_2_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger) = 0;
        /// @brief 4-byte integer callback 
        /// @param ascii_record_type record 
        /// @param ascii_data_type data type 
        /// @param vInteger data array  
		virtual void integer_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<int> const& vInteger) = 0;
        /// @brief 4-byte floating point number callback 
        /// @param ascii_record_type record 
        /// @param ascii_data_type data type 
        /// @param vFloat data array  
		virtual void real_4_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) = 0;
        /// @brief 8-byte floating point number callback 
        /// @param ascii_record_type record 
        /// @param ascii_data_type data type 
        /// @param vFloat data array  
		virtual void real_8_cbk(const char* ascii_record_type, const char* ascii_data_type, vector<double> const& vFloat) = 0;
        /// @brief string callback 
        /// @param ascii_record_type record 
        /// @param ascii_data_type data type 
        /// @param str data 
		virtual void string_cbk(const char* ascii_record_type, const char* ascii_data_type, string const& str) = 0;
        /// @brief begin or end indicator of a block 
        /// @param ascii_record_type record 
		virtual void begin_end_cbk(const char* ascii_record_type) = 0; // begin or end indicater of a block 
    private:
        /// @name These callbacks are disabled for users 
        ///@{
        /// These callbacks will redirect to ascii callbacks 
		virtual void bit_array_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vBitArray);
		virtual void integer_2_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vInteger);
		virtual void integer_4_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vInteger);
		virtual void real_4_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<double> const& vFloat);
		virtual void real_8_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<double> const& vFloat);
		virtual void string_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, string const& str);
		virtual void begin_end_cbk(GdsRecords::EnumType record_type); // begin or end indicater of a block 
        ///@}
};

// inline redirection from enum callbacks to ascii callbacks
inline void GdsDataBase::bit_array_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vBitArray)
{this->bit_array_cbk(gds_record_ascii(record_type), gds_data_ascii(data_type), vBitArray);}
inline void GdsDataBase::integer_2_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vInteger)
{this->integer_2_cbk(gds_record_ascii(record_type), gds_data_ascii(data_type), vInteger);}
inline void GdsDataBase::integer_4_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<int> const& vInteger)
{this->integer_4_cbk(gds_record_ascii(record_type), gds_data_ascii(data_type), vInteger);}
inline void GdsDataBase::real_4_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<double> const& vFloat)
{this->real_4_cbk(gds_record_ascii(record_type), gds_data_ascii(data_type), vFloat);}
inline void GdsDataBase::real_8_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, vector<double> const& vFloat)
{this->real_8_cbk(gds_record_ascii(record_type), gds_data_ascii(data_type), vFloat);}
inline void GdsDataBase::string_cbk(GdsRecords::EnumType record_type, GdsData::EnumType data_type, string const& str)
{this->string_cbk(gds_record_ascii(record_type), gds_data_ascii(data_type), str);}
inline void GdsDataBase::begin_end_cbk(GdsRecords::EnumType record_type) // begin or end indicater of a block 
{this->begin_end_cbk(gds_record_ascii(record_type));}

/// @class GdsParser::GdsReader
/// @brief read GDSII 
class GdsReader
{
	public: 
        /// @brief constructor 
        /// @param db database 
		GdsReader(GdsDataBaseKernel& db); 
        /// @brief destructor 
        ~GdsReader(); 

        /// @brief read from file 
        /// @param filename file name 
		bool operator()(const char* filename);
        /// @brief read from stream
        /// @param fp input stream 
        bool operator()(std::istream& fp); 

	protected:
        /// @brief find record 
        /// @param numeric record 
        /// @param record_name enum type of record 
        /// @param expected_data_type expected data 
        void find_record_type (int numeric, GdsRecords::EnumType& record_name, int& expected_data_type);
        /// @brief find data type 
        /// @param numeric record 
        /// @param data_name enum type of data type 
        void find_data_type (int numeric, GdsData::EnumType& data_name);
        /// @brief print comments 
        /// @param enum_record_type enum type of record 
        /// @param bit_array bit array 
        /// @param indent_amount amount of indent
        void print_bit_array_comments (GdsRecords::EnumType enum_record_type, int bit_array, int indent_amount);

        /// read n bytes 
        /// @param fp file handler 
        /// @param no_read number of bytes read 
        /// @param n number of bytes to read 
        const char* gds_read(std::istream& fp, int& no_read, std::size_t n); 

		GdsDataBaseKernel& m_db; ///< database 
        char* m_buffer; ///< buffer for reading file 
        char* m_bptr; ///< start position in buffer 
        std::size_t m_bcap; ///< buffer capacity 
        std::size_t m_blen; ///< current buffer size, from m_bptr to m_buffer+m_bcap 
};

/// @brief read from stream 
/// @param db GDSII database 
/// @param fp input stream 
bool read(GdsDataBaseKernel& db, std::istream& fp);
/// @brief read from file
/// @param db GDSII database 
/// @param filename GDSII file 
bool read(GdsDataBaseKernel& db, string const& filename);

} // namespace GdsParser

#endif 

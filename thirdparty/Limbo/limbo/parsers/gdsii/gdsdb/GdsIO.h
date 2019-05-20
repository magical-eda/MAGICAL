/**
 * @file   GdsIO.h
 * @brief  Wrapper to GDSII read and write with GDSII database @ref GdsParser::GdsDB::GdsDB
 * @author Yibo Lin
 * @date   Nov 2016
 */

#ifndef LIMBO_PARSERS_GDSII_GDSDB_GDSIO_H
#define LIMBO_PARSERS_GDSII_GDSDB_GDSIO_H

#include <fstream>
#include <limits>
#include <limbo/parsers/gdsii/stream/GdsReader.h>
#include <limbo/parsers/gdsii/stream/GdsWriter.h>

#include <limbo/parsers/gdsii/gdsdb/GdsObjects.h>

/// namespace for Limbo.GdsParser 
namespace GdsParser 
{
/// namespace for Limbo.GdsParser.GdsDB 
namespace GdsDB 
{

/// read GDSII file with GDSII database @ref GdsParser::GdsDB::GdsDB
class GdsReader : ::GdsParser::GdsDataBaseKernel
{
	public:
        /// @nowarn 
		typedef GdsDB gdsdb_type;
		typedef gdsdb_type::coordinate_type coordinate_type;
		typedef gdsdb_type::point_type point_type;
        /// @endnowarn 

        /// @brief constructor
        /// @param db GDSII database 
		GdsReader(gdsdb_type& db) : m_db(db) {}

		/// @brief API to read GDSII file 
        /// @param filename GDSII file 
		bool operator() (std::string const& filename); 

		/// @name required callbacks in parser 
        ///@{
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param vBitArray data 
		virtual void bit_array_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vBitArray);
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param vInteger data 
		virtual void integer_2_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vInteger);
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param vInteger data 
		virtual void integer_4_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vInteger);
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param vFloat data 
		virtual void real_4_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<double> const& vFloat);
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param vFloat data 
		virtual void real_8_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<double> const& vFloat);
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param str data 
		virtual void string_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::string const& str);
        /// @param record_type GDSII record 
		virtual void begin_end_cbk(::GdsParser::GdsRecords::EnumType record_type);
        ///@}

		/// @name helper functions for easier processing 
        ///@{
        /// @brief callbacks for integer 
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param vData data 
		void integer_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vData);
        /// @brief callbacks for floating point number  
        /// @param record_type GDSII record 
        /// @param data_type GDSII data type 
        /// @param vData data 
		void float_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<double> const& vData);
        ///@}

	protected:
		/// @brief reset all temporary data to default values 
		void reset(); 
        /// @brief warn unsupported records 
		void printUnsupportRecords() const; 

		// temporary data 
		std::string m_string; ///< STRING 
		std::string m_sname; ///< SNAME 
		int m_layer; ///< LAYER 
		int m_datatype; ///< DATATYPE
		int m_pathtype; ///< PATHTYPE
		int m_texttype; ///< TEXTTYPE
		int m_spacing[2]; ///< SPACING 
		int m_width; ///< WIDTH 
		int m_columns; ///< COLROW, number of columns 
		int m_rows; ///< COLROW, number of rows 
		double m_angle; ///< ANGLE 
		double m_magnification; ///< MAG 
		int m_strans; ///< STRANS
		int m_presentation; ///< PRESENTATION
		std::vector<point_type> m_vPoint; ///< XY

		::GdsParser::GdsRecords::EnumType m_status; ///< current record status 
		int m_fileSize; ///< file size in bytes 
		gdsdb_type& m_db; ///< reference to GDSII database 

		std::vector<unsigned int> m_vUnsupportRecord; ///< try to be clean at screen output, record the times of unsupported records 
};

/// write GDSII file with GDSII database @ref GdsParser::GdsDB::GdsDB
class GdsWriter
{
	public:
        /// @nowarn
		typedef GdsDB gdsdb_type;
        /// @endnowarn

		/// @brief constructor
        /// @param db GDSII database 
		GdsWriter(gdsdb_type const& db) : m_db(db) {}

		/// @brief API to write GDSII file 
        /// @param filename GDSII file 
		void operator() (std::string const& filename) const;

		/// @name helper functions to write gdsii objects 
        ///@{
		// there is no need to use different name, since we pass different parameters
        /// @param gw GDSII writer handler 
        /// @param cell GDSII cell object 
		void write(::GdsParser::GdsWriter& gw, GdsCell const& cell) const; 
        /// @param gw GDSII writer handler 
        /// @param object GDSII polygon object 
		void write(::GdsParser::GdsWriter& gw, GdsPolygon const& object) const; 
        /// @param gw GDSII writer handler 
        /// @param object GDSII path object 
		void write(::GdsParser::GdsWriter& gw, GdsPath const& object) const; 
        /// @param gw GDSII writer handler 
        /// @param object GDSII text object 
		void write(::GdsParser::GdsWriter& gw, GdsText const& object) const; 
        /// @param gw GDSII writer handler 
        /// @param object GDSII cell reference object 
		void write(::GdsParser::GdsWriter& gw, GdsCellReference const& object) const; 
        /// @param gw GDSII writer handler 
        /// @param object GDSII cell array object 
		void write(::GdsParser::GdsWriter& gw, GdsCellArray const& object) const; 
        ///@}

	protected:
		gdsdb_type const& m_db; ///< reference to GDSII database 
};

} // namespace GdsDB
} // namespace GdsParser

#endif 

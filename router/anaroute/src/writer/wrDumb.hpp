/**
 * @file   wrDumb.hpp
 * @brief  Output file writer - Dumb
 * @author Hao Chen
 * @date   12/11/2019
 *
 **/

#ifndef _WR_DUMB_HPP_
#define _WR_DUMB_HPP_

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>

#include "src/db/dbCir.hpp"
#include "src/writer/wrGds.hpp"

PROJECT_NAMESPACE_START

class DumbWriter {
 public:
	DumbWriter(const CirDB& c)
		: _cir(c) {}
	~DumbWriter() {}

	void write(const String_t& placementFilename, const String_t& outputFilename);

 private:
	const CirDB& _cir;
	/////////////////////////////////////////
	//    Private functions                //
	/////////////////////////////////////////
	typedef GdsParser::GdsDB::GdsDB   GdsDB;
	typedef GdsParser::GdsDB::GdsCell GdsCell;
	typedef boost::polygon::point_data<Int_t> GdsPoint;

	bool      readPlacementGds(const String_t& filename, GdsParser::GdsDB::GdsDB& gdsDB);
	String_t  topCellName(const GdsDB& gdsDB);
};

namespace ExtractBBoxActionDetails {
	namespace gtl = boost::polygon;
	/// @brief default action
	template<typename ObjectType>
		inline void extractBBox(Box<Int_t>& bbox, ::GdsParser::GdsRecords::EnumType type, ObjectType *object) {}


	/// @brief process gds rectangle
	template<>
		inline void extractBBox(Box<Int_t>& bbox, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsRectangle *object) {
		}

	/// @brief process gds polygon
	template<>
		inline void extractBBox(Box<Int_t>& bbox, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPolygon *object) {
			/// Add points
			for (const auto& pt : *object) {
				if (pt.x() < bbox.xl()) {
					bbox.setXL(pt.x());
				}
				if (pt.y() < bbox.yl()) {
					bbox.setYL(pt.y());
				}
				if (pt.x() > bbox.xh()) {
					bbox.setXH(pt.x());
				}
				if (pt.y() > bbox.yh()) {
					bbox.setYH(pt.y());
				}
			}
		}

	/// @brief process path
	template<>
		inline void extractBBox(Box<Int_t>& bbox, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object) {
		}

}


/// @brief aution function object to process the the 
struct ExtractBBoxAction {
	/// @param first: the map between the gds layer indices to the router layers. Only care about the mapping within this map
	/// @param second: a reference to a vector of polygons, saved the read polygons in this vector
	ExtractBBoxAction(Box<Int_t> &bbox) : _bbox(bbox) {}
	template<typename ObjectType>
		void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object) {
			ExtractBBoxActionDetails::extractBBox(_bbox, type, object);
		}

	/// @return a message of action for debug
	String_t message() const {
		return "ExtractBBoxAction";
	}

	Box<Int_t>& _bbox; ///< The bounding box
};

PROJECT_NAMESPACE_END

#endif /// _WR_DUMB_HPP_

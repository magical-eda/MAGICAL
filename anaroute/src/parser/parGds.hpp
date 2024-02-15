/**
 * @file   parGds.hpp
 * @brief  Parser - Gds format parser
 * @author Hao Chen
 * @date   09/23/2019
 *
 **/

#ifndef _PAR_GDS_HPP_
#define _PAR_GDS_HPP_

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

struct PolygonLayer {
	explicit PolygonLayer() = default;
	String_t toStr() const {
		std::stringstream oss;
		oss<<"Layer "<<layer<<" ";
		for (auto pt : pts) {
			oss << " "<<pt<<" ";
		}
		return oss.str();
	}
	/// @brief scale the polygon
	void scale(Float_t scaleX, Float_t scaleY) {
		for (UInt_t ptIdx = 0; ptIdx < pts.size(); ++ptIdx) {
			Float_t x = pts.at(ptIdx).x();
			Float_t y = pts.at(ptIdx).y();
			Int_t newX = std::round(x * scaleX);
			Int_t newY = std::round(y * scaleY);
			pts.at(ptIdx).setX(newX);
			pts.at(ptIdx).setY(newY);
		}
	}

	///Members
	Vector_t<Point<Int_t>> pts; ///< The points of the polygon
	UInt_t layer = MAX_UINT; ///< The layer of the polygon
};

class GdsReader { // from limbo
 public:
	GdsReader(CirDB& c)
		: _cir(c) {}
	~GdsReader() {}

	void parse(const String_t& filename);

 private:
	CirDB&                            _cir;
	Vector_t<Pair_t<Int_t, UInt_t>>   _vMaskId2Layers;
	Vector_t<PolygonLayer>            _vPolygonLayers;
	Vector_t<PolygonLayer>            _vPoblks;
	/////////////////////////////////////////
	//    Private functions                //
	/////////////////////////////////////////
	String_t  topCell(const GdsParser::GdsDB::GdsDB& db);
	void      buildLayerMap();
  void      saveShapesAsBlockages();
  void      parseOdLayers(GdsParser::GdsDB::GdsCell &flatCell, Float_t sc);

};
//////////////////////////////////////////////////////////////////
/// @brief detailed struct for the functions of processing gds shapes
namespace ExtractShapeLayerActionDetails {
	constexpr Int_t DUMMY_BLOCKAGE_LAYER = 155;
	constexpr Int_t DUMMY_BLOCKAGE_DATATYPE = 100;

	namespace gtl = boost::polygon;
	/// @brief convert from gds layer to router layer
	/// @return MAX_UINT if not found, otherwise, the router layer
	inline UInt_t layerIdx(Int_t gdsLayer, Vector_t<Pair_t<Int_t, UInt_t>> &map) {
		/// Linear search in the map
		for (auto pair : map) {
			if (pair.first == gdsLayer) {
				return pair.second;
			}
		}
		return MAX_UINT;
	}
	/// @brief default action
	template<typename ObjectType>
		inline void extractShape(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ObjectType *object) {}


	/// @brief process gds rectangle
	template<>
		inline void extractShape(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsRectangle *object) {
			//DBG("Rect \n");
			assert(false);
			//AssertMsg(0, "%s: Rectangle type not support yet \n", __FUNCTION__);
			//DBG("Gds rectangle layer %d, (%d, %d ), (%d, %d) \n", object->layer(), gtl::xl(object), gtl::yl(object), gtl::xh(object), gtl::yh(object));
		}

	/// @brief process gds polygon
	template<>
		inline void extractShape(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPolygon *object) {
			/// Convert the gds layer to the router layer
			UInt_t routerLayer = layerIdx(object->layer(), map);
			if (routerLayer != MAX_UINT) {
				polygons.emplace_back(PolygonLayer());
				polygons.back().layer = routerLayer;
				/// Add points
				for (auto pt : *object) {
					polygons.back().pts.emplace_back(Point<Int_t>(pt.x(), pt.y()));
				}
			}
			else if (object->layer() == DUMMY_BLOCKAGE_LAYER && object->datatype() == DUMMY_BLOCKAGE_DATATYPE) {
				polygons.emplace_back(PolygonLayer());
				polygons.back().layer = MAX_UINT;
				/// Add points
				for (auto pt : *object) {
					polygons.back().pts.emplace_back(Point<Int_t>(pt.x(), pt.y()));
				}
			}
		}

	/// @brief process path
	template<>
		inline void extractShape(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object) {
			auto polygon = object->toPolygon();
			extractShape(map, polygons, type, &polygon);
		}

}

/// @brief detailed struct for the functions of processing gds shapes
namespace ExtractPoBlkActionDetails {
	constexpr Int_t POBLK_LAYER = 150;
	constexpr Int_t POBLK_DATATYPE = 2;

	namespace gtl = boost::polygon;
	/// @brief convert from gds layer to router layer
	/// @return MAX_UINT if not found, otherwise, the router layer
	inline UInt_t layerIdx(Int_t gdsLayer, Vector_t<Pair_t<Int_t, UInt_t>> &map) {
		/// Linear search in the map
		for (auto pair : map) {
			if (pair.first == gdsLayer) {
				return pair.second;
			}
		}
		return MAX_UINT;
	}
	/// @brief default action
	template<typename ObjectType>
		inline void extractPoblk(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ObjectType *object) {}



	/// @brief process gds polygon
	template<>
		inline void extractPoblk(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPolygon *object) {
			if (object->layer() == POBLK_LAYER && object->datatype() == POBLK_DATATYPE) {
				polygons.emplace_back(PolygonLayer());
				polygons.back().layer = MAX_UINT;
				/// Add points
				for (auto pt : *object) {
					polygons.back().pts.emplace_back(Point<Int_t>(pt.x(), pt.y()));
				}
			}
		}

	/// @brief process path
	template<>
		inline void extractPoblk(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object) {
			auto polygon = object->toPolygon();
			extractPoblk(map, polygons, type, &polygon);
		}

}


struct ExtractPoBlkAction {
	/// @param first: the map between the gds layer indices to the router layers. Only care about the mapping within this map
	/// @param second: a reference to a vector of polygons, saved the read polygons in this vector
	ExtractPoBlkAction(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons) : _vMaskId2Layers(map), _polygons(polygons) {}
	template<typename ObjectType>
		void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object) {
			ExtractPoBlkActionDetails::extractPoblk(_vMaskId2Layers, _polygons, type, object);
		}

	/// @return a message of action for debug
	String_t message() const {
		return "ExtractShapeLayerAction";
	}

	Vector_t<Pair_t<Int_t, UInt_t>> &_vMaskId2Layers; ///< Map gds layer id to all layers in router (masterslice + routing + cut)
	Vector_t<PolygonLayer> &_polygons; ///< The polygons read from the gds
};


/// @brief aution function object to process the the 
struct ExtractShapeLayerAction {
	/// @param first: the map between the gds layer indices to the router layers. Only care about the mapping within this map
	/// @param second: a reference to a vector of polygons, saved the read polygons in this vector
	ExtractShapeLayerAction(Vector_t<Pair_t<Int_t, UInt_t>> &map, Vector_t<PolygonLayer> &polygons) : _vMaskId2Layers(map), _polygons(polygons) {}
	template<typename ObjectType>
		void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object) {
			ExtractShapeLayerActionDetails::extractShape(_vMaskId2Layers, _polygons, type, object);
		}

	/// @return a message of action for debug
	String_t message() const {
		return "ExtractShapeLayerAction";
	}

	Vector_t<Pair_t<Int_t, UInt_t>> &_vMaskId2Layers; ///< Map gds layer id to all layers in router (masterslice + routing + cut)
	Vector_t<PolygonLayer> &_polygons; ///< The polygons read from the gds
};


namespace GetSRefNameActionDetailsParser {
	/// @brief default type
	template<typename ObjectType>
		inline void getSName(String_t &name,  ::GdsParser::GdsRecords::EnumType type, ObjectType *object) {}

	/// @brief SREF type
	template<>
		inline void getSName(String_t &name,  ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsCellReference *object) {
			name = object->refCell();
		}
}

/// @brief aution function object to get the cell reference name
struct GetSRefNameActionParser {
	/// @param A reference to the string to record the name of the sref
	GetSRefNameActionParser(String_t &name) : _name(name) {}
	template<typename ObjectType>
		void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object) {
			GetSRefNameActionDetailsParser::getSName(_name, type, object);
		}

	/// @return a message of action for debug
	String_t message() const  {
		return "GetSRefNameAction";
	}


	String_t &_name; ///< The cell reference name
};


namespace ExtractSpecificShapeLayerActionDetails {

	namespace gtl = boost::polygon;
	/// @brief default action
	template<typename ObjectType>
		inline void extractShape(Int_t layerIdx, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ObjectType *object) {}


	/// @brief process gds rectangle
	template<>
		inline void extractShape(Int_t layerIdx, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsRectangle *object) {
			assert(false);
		}

	/// @brief process gds polygon
	template<>
		inline void extractShape(Int_t layerIdx, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPolygon *object) {
			if (object->layer() == layerIdx) {
				polygons.emplace_back(PolygonLayer());
				polygons.back().layer = layerIdx;
				/// Add points
				for (auto pt : *object) {
					polygons.back().pts.emplace_back(Point<Int_t>(pt.x(), pt.y()));
				}
			}
		}

	/// @brief process path
	template<>
		inline void extractShape(Int_t layerIdx, Vector_t<PolygonLayer> &polygons, ::GdsParser::GdsRecords::EnumType type, ::GdsParser::GdsDB::GdsPath *object) {
			auto polygon = object->toPolygon();
			extractShape(layerIdx, polygons, type, &polygon);
		}

}

/// @brief aution function object to process the the 
struct ExtractSpecificShapeLayerAction {
	/// @param first: the map between the gds layer indices to the router layers. Only care about the mapping within this map
	/// @param second: a reference to a vector of polygons, saved the read polygons in this vector
	ExtractSpecificShapeLayerAction(Int_t layerIdx, Vector_t<PolygonLayer> &polygons) : _layerIdx(layerIdx), _polygons(polygons) {}
	template<typename ObjectType>
		void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object) {
			ExtractSpecificShapeLayerActionDetails::extractShape(_layerIdx, _polygons, type, object);
		}

	/// @return a message of action for debug
	String_t message() const {
		return "ExtractShapeLayerAction";
	}

	Int_t _layerIdx; ///< The target gds layer
	Vector_t<PolygonLayer> &_polygons; ///< The polygons read from the gds
};

PROJECT_NAMESPACE_END

#endif /// _PAR_GDS_HPP_

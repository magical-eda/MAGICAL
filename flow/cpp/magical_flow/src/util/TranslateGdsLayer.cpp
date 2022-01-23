#include "TranslateGdsLayer.h"
#include "util/GdsHelper.h"

PROJECT_NAMESPACE_BEGIN

bool TranslateGdsLayer::parseMappingFile(const std::string layerMappingFile) 
{
    std::ifstream inf(layerMappingFile.c_str());
    if (!inf.is_open()) 
    {
        ERR("TranslateGdsLayer:%s: cannot open file: %s \n", __FUNCTION__ , layerMappingFile.c_str());
        Assert(false);
        return false;
    }
    std::string lineStr;
    while (std::getline(inf, lineStr))
    {
        std::istringstream iss(lineStr);
        std::vector<std::string> split;
        std::string token;
        while(std::getline(iss, token, ' '))
        {
            split.emplace_back(token);
        }
        if (split.size() == 5)
        {
          int l1 = std::stoi(split.at(1));
          int d1 = std::stoi(split.at(2));
          int l2 = std::stoi(split.at(3));
          int d2 = std::stoi(split.at(4));
          _mapping[std::make_pair(l1, d1)] = std::make_pair(l2, d2);
        }
        else
        {
            Assert(false);
            return false;
        }
    }
    return true;

}


/// @brief detailed struct for the functions of processing gds shapes
namespace ProcessShapeLayerActionDetails
{
    namespace gtl = boost::polygon;
    using MapType = std::map<std::pair<int, int>, std::pair<int, int>>;
    /// @brief find the new layer ID
    /// @return the new layer ID, datatype pair. If not found, return -1, -1
    inline std::pair<IntType, IntType> layerIdx(IntType layerID, IntType datatype, MapType &map)
    {
        /// Search in the map
        auto findIter = map.find(std::make_pair(layerID, -1)); // -1 means for all datatype
        if (findIter != map.end())
        {
            return findIter->second;
        }
        findIter = map.find(std::make_pair(layerID, datatype));
        if (findIter != map.end())
        {
            return findIter->second;
        }
        return std::make_pair(-1, -1);
    }
    /// @brief default action
    template<typename ObjectType>
    inline void translateLayer(MapType &, ObjectType *)
    {
    }

    inline void changeLayer(MapType &map, ::GdsParser::GdsDB::GdsShape *object)
    {
        /// Convert the gds layer to the router layer
        auto newLayer = layerIdx(object->layer(), object->datatype(), map);
        AssertMsg(newLayer.first >= 0, "TranslateGdsLayer:: Unknown layer ID-datatype %d %d", object->layer(), object->datatype());
        object->setLayer(newLayer.first);
        object->setDatatype(newLayer.second);
    }

    /// @brief process gds shape
    template<>
    inline void translateLayer(MapType &map, ::GdsParser::GdsDB::GdsRectangle *object)
    {
      changeLayer(map, object);
    }
    template<>
    inline void translateLayer(MapType &map, ::GdsParser::GdsDB::GdsPolygon *object)
    {
      changeLayer(map, object);
    }
    template<>
    inline void translateLayer(MapType &map, ::GdsParser::GdsDB::GdsPath *object)
    {
      changeLayer(map, object);
    }
    template<>
    inline void translateLayer(MapType &map, ::GdsParser::GdsDB::GdsText *object)
    {
      changeLayer(map, object);
    }
}

/// @brief aution function object to process the the 
struct ProcessShapeLayerAction
{
    /// @param first: the map between the gds layer indices to the new layer ID
    ProcessShapeLayerAction(std::map<std::pair<int, int>, std::pair<int, int>> &map) : _layerIdMap(map) {}
    template<typename ObjectType>
    void operator()(::GdsParser::GdsRecords::EnumType, ObjectType* object)
    {
        ProcessShapeLayerActionDetails::translateLayer(_layerIdMap, object);
    }

    /// @return a message of action for debug
    std::string message() const 
    {
        return "ProcessShapeLayerAction";
    }

    std::map<std::pair<int, int>, std::pair<int, int>> & _layerIdMap; ///< A map from gds layer to IDEAPLACE layer. _layerIdxMap[
};


bool TranslateGdsLayer::translateGds(const std::string &inputGds, const std::string &outputGds)
{
  // Flaten the gds by the last cell
  ::GdsParser::GdsDB::GdsDB unflatenDb;
  ::GdsParser::GdsDB::GdsReader reader(unflatenDb);
  if (!reader(inputGds))
  {
      ERR("TranslateGdsLayer: cannot open file %s! \n", inputGds.c_str());
  }


  // Flaten the gds
  std::string topCellName = ::klib::topCell(unflatenDb);
  auto flatCell = unflatenDb.extractCell(topCellName);

  // Process the layer in the gds
  for (auto &object : flatCell.objects())
  {
      ::GdsParser::GdsDB::GdsObjectHelpers()(object.first, object.second, ProcessShapeLayerAction(_mapping));
  }


  ::GdsParser::GdsDB::GdsDB gdsDb;
  gdsDb.addCell(flatCell);
  gdsDb.setUnit(unflatenDb.unit());
  gdsDb.setPrecision(unflatenDb.precision());
  gdsDb.setHeader(unflatenDb.header());
  gdsDb.setLibname(unflatenDb.libname());


  // Write to gds
  GdsParser::GdsDB::GdsWriter gw (gdsDb);
  gw(outputGds.c_str());
  INF("TranslateGdsLayer: Write the gds to %s \n", outputGds.c_str());

  return true;
}

PROJECT_NAMESPACE_END

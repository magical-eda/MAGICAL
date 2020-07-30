/**
 * @file GdsWriter.h
 * @brief Writing GDSII layout file 
 * @author Keren Zhu
 * @date 07/08/2019
 */

#ifndef MAGICAL_FLOW_GDS_WRITER_H_
#define MAGICAL_FLOW_GDS_WRITER_H_

#include "db/DesignDB.h"
#include "db/TechDB.h"
#include "util/GdsHelper.h"

PROJECT_NAMESPACE_BEGIN

class GdsWriter
{
    // Point type for the gds db
    typedef boost::polygon::point_data<IntType> point_type;
    public:
        /// @brief constructor
        /// @param first: a design database
        /// @param second: a technology database
        explicit GdsWriter(DesignDB &designDB, TechDB &techDB) : _designDB(designDB), _techDB(techDB) {} 
        /// @brief write the layout of a circuit into GDSII 
        /// @param first: the index of circuit graph
        /// @param second: the output file name
        void writeGdsLayout(IndexType cktIdx, const std::string &filename);
    private:
        /// @brief add CktGraph to the gdsDB
        /// @param the index of CktGraph
        void addCktGraph(IndexType cktGraphIdx);
        /// @brief add box to the cell
        /// @param reference to the cell
        /// @param rectangle
        /// @param db layer
        /// @param datatype
        void addRect2Cell(::GdsParser::GdsDB::GdsCell &gdsCell, const Box<LocType> &rect, IndexType dbLayer, IntType datatype);
        /// @brief add text to the cell
        /// @param reference to the cell
        /// @param coordinate of the text
        /// @param db layer
        /// @param the string for the text
        void addText2Cell(::GdsParser::GdsDB::GdsCell &gdsCell, const XY<LocType> &coord, IndexType dbLayer, const std::string &str);
        /// @brief add a cellreference to a gdscell
        /// @param the reference to the gdscell
        /// @param the CktNode want to add
        void addCellRef2Cell(::GdsParser::GdsDB::GdsCell &gdsCell, CktNode &node);
        /// @brief add text to the cell
        /// @brief convert XY type to point_type
        point_type convertXY(const XY<LocType> &pt)
        {
            return point_type(pt.x(), pt.y());
        }
    private:
        ::GdsParser::GdsDB::GdsDB _gdsDB; ///< The database for the GDS
        DesignDB &_designDB; ///< The design database
        TechDB &_techDB; ///< The technology database
};

inline void GdsWriter::writeGdsLayout(IndexType cktIdx, const string &filename)
{
    // Config header and units
    _gdsDB.cells().clear();
    _gdsDB.setUnit(_techDB.units().dbuUU());
    _gdsDB.setPrecision(_techDB.units().dbuM());
    _gdsDB.setHeader(_techDB.units().gdsHeader());

    // Add layouts
    this->addCktGraph(cktIdx);

    // Write out
    ::GdsParser::GdsDB::GdsWriter gw (_gdsDB);
    gw(filename.c_str());
    INF("Flow::GdsWriter:: Write circuit %s layout to %s \n", _designDB.subCkt(cktIdx).name().c_str(), filename.c_str());
}

inline void GdsWriter::addCktGraph(IndexType cktGraphIdx)
{
    auto &cktGraph = _designDB.subCkt(cktGraphIdx); // CktGraph
    // Check if the cell has been added to the gdsDB
    ::GdsParser::GdsDB::GdsCell const* getCellResult = _gdsDB.getCell(cktGraph.name());
    if (getCellResult != NULL)
    {
        // If the return is not NULL, then the cell has already been added to the gdsDB
        // No need to add again
        return;
    }
    auto &gdsCell = _gdsDB.addCell(cktGraph.name()); // GdsCell
    
    // Add layout
    auto cktLayout = cktGraph.layout(); // Layout
    for (IndexType layerIdx = 0; layerIdx < cktLayout.numLayers(); ++layerIdx)
    {
        for (IndexType rectIdx = 0; rectIdx < cktLayout.numRects(layerIdx); ++rectIdx)
        {
            this->addRect2Cell(gdsCell, cktLayout.rect(layerIdx, rectIdx).rect(), layerIdx, cktLayout.rect(layerIdx, rectIdx).datatype()); // FIXME For >M6 layer, need to use datatype=40
        }
        for (IndexType textIdx = 0; textIdx < cktLayout.numTexts(layerIdx); ++textIdx)
        {
            this->addText2Cell(gdsCell, cktLayout.text(layerIdx, textIdx).coord(), layerIdx, cktLayout.text(layerIdx, textIdx).text());
        }
    }

    // Add cell reference
    for (IndexType nodeIdx = 0; nodeIdx < cktGraph.numNodes(); ++nodeIdx)
    {
        auto & node = cktGraph.node(nodeIdx);
        if (node.isLeaf())
        {
            continue;
        }
        // If it is not leaf, add a reference cell and recursively add the cell
        // this->addCktGraph(node.subgraphIdx());
        // this->addCellRef2Cell(gdsCell, node);
    }
}

inline void GdsWriter::addRect2Cell(::GdsParser::GdsDB::GdsCell &gdsCell, const Box<LocType> &rect, IndexType dbLayer, IntType datatype)
{
    IntType pdkLayer = static_cast<IntType>(_techDB.dbLayerToPdk(dbLayer));
    std::vector<point_type> pts;
    pts.emplace_back(this->convertXY(XY<LocType>(rect.xLo(), rect.yLo())));
    pts.emplace_back(this->convertXY(XY<LocType>(rect.xLo(), rect.yHi())));
    pts.emplace_back(this->convertXY(XY<LocType>(rect.xHi(), rect.yHi())));
    pts.emplace_back(this->convertXY(XY<LocType>(rect.xHi(), rect.yLo())));
    pts.emplace_back(this->convertXY(XY<LocType>(rect.xLo(), rect.yLo())));

    // Add to the cells
    gdsCell.addPolygon(pdkLayer, datatype, pts);

}

inline void GdsWriter::addText2Cell(::GdsParser::GdsDB::GdsCell &gdsCell, const XY<LocType> &coord, IndexType dbLayer, const std::string &str)
{
    IntType pdkLayer = static_cast<IntType>(_techDB.dbLayerToPdk(dbLayer));
    // Trick: set datatype to int max so that the GDS writter will be forced to skip the keyword
    gdsCell.addText(pdkLayer, std::numeric_limits<int>::max(), 0, str, convertXY(coord), std::numeric_limits<int>::max(), 5, 0, 0.2, 0);
}

inline void GdsWriter::addCellRef2Cell(GdsParser::GdsDB::GdsCell &gdsCell, CktNode &node)
{
    double angle = 0; 
    bool flip = false;
    if (node.orient() == OriType::N)
    {
        angle = 0;
        flip = false;
    }
    else
    {
        angle = 0;
        flip = true;
        // TODO:
        // AssertMsg(false, "GdsWriter::addCellRef2Cell. TODO: Not yet implemented. Only N orientation is supported right now. \n");
    }
    int strans;
    if (flip)
    {
        strans = 32768;
    }
    else
    {
        strans = 0;
    }
    // 1. reference cell name 2. angle 3. magnification 4. strans
    gdsCell.addCellReference(node.refName(), this->convertXY(node.offset()), angle, 1, strans);
}


namespace WRITER
{
    /// @brief write the layout for circuit to GDSII
    /// @param first: circuit graph index
    /// @param second: output file name
    /// @param third: design database
    /// @param fourth: technology database
    inline void writeGdsLayout(IndexType cktIdx, const std::string &filename, DesignDB &designDB, TechDB &techDB)
    {
        GdsWriter(designDB, techDB).writeGdsLayout(cktIdx, filename);
    }
}
PROJECT_NAMESPACE_END
#endif //MAGICAL_FLOW_GDS_WRITER_H_

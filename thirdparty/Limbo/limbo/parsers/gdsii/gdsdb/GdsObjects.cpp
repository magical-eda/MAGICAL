/**
 * @file   GdsObjects.cpp
 * @brief  Implementation of various GDSII objects and a top GDSII database 
 * @author Yibo Lin
 * @date   Oct 2016
 */

#include <limbo/parsers/gdsii/gdsdb/GdsObjects.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>
#include <limbo/preprocessor/Msg.h>
#include <limits>
#include <exception>
#include <cmath>
#include <deque>

namespace GdsParser { namespace GdsDB {

GdsObject::GdsObject() 
{
}

GdsObject::GdsObject(GdsObject const&)
{
}

GdsObject& GdsObject::operator=(GdsObject const&)
{
	return *this; 
}

GdsObject::~GdsObject() 
{
}

GdsShape::GdsShape()
	: GdsShape::base_type()
	, m_layer(std::numeric_limits<int>::max())
	, m_datatype(std::numeric_limits<int>::max())
{
}

GdsShape::GdsShape(GdsShape const& rhs) 
	: GdsShape::base_type(rhs)
{
	m_layer = rhs.m_layer; 
	m_datatype = rhs.m_datatype;
}

GdsShape& GdsShape::operator=(GdsShape const& rhs) 
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs);
		m_layer = rhs.m_layer; 
		m_datatype = rhs.m_datatype;
	}
	return *this; 
}

GdsShape::~GdsShape() 
{
}

GdsRectangle::GdsRectangle()
	: GdsRectangle::base_type()
	, GdsRectangle::base_ext_type()
{
}

GdsRectangle::GdsRectangle(GdsRectangle const& rhs) 
	: GdsRectangle::base_type(rhs)
	, GdsRectangle::base_ext_type(rhs)
{
}

GdsRectangle& GdsRectangle::operator=(GdsRectangle const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs); 
		this->base_ext_type::operator=((base_ext_type)rhs); 
	}
	return *this; 
}

GdsRectangle::~GdsRectangle() 
{
}

GdsPolygon::GdsPolygon()
	: GdsPolygon::base_type()
	, GdsPolygon::base_ext_type()
{
}

GdsPolygon::GdsPolygon(GdsPolygon const& rhs) 
	: GdsPolygon::base_type(rhs)
	, GdsPolygon::base_ext_type(rhs)
{
}

GdsPolygon& GdsPolygon::operator=(GdsPolygon const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs); 
		this->base_ext_type::operator=((base_ext_type)rhs); 
	}
	return *this; 
}

GdsPolygon::~GdsPolygon() 
{
}

GdsPath::GdsPath()
	: GdsPath::base_type()
	, GdsPath::base_ext_type()
	, m_pathtype(std::numeric_limits<int>::max())
	, m_width(std::numeric_limits<GdsPath::coordinate_type>::max())
{
}

GdsPath::GdsPath(GdsPath const& rhs) 
	: GdsPath::base_type(rhs)
	, GdsPath::base_ext_type(rhs)
	, m_pathtype(rhs.m_pathtype)
	, m_width(rhs.m_width)
{
}

GdsPath& GdsPath::operator=(GdsPath const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs); 
		this->base_ext_type::operator=((base_ext_type)rhs);
		m_pathtype = rhs.m_pathtype; 
		m_width = rhs.m_width; 
	}
	return *this; 
}

GdsPath::~GdsPath() 
{
}

GdsPolygon GdsPath::toPolygon() const 
{
	// do not handle single point path 
	limboAssertMsg(size() >= 2, "too few points for a path size = %lu", size());

	// points in polygons along a path 
	std::deque<point_type> vPoint; 
	// two angles for each line segment 
	double theta[2] = {0, 0};

	for (const_iterator it = begin(); it != end(); ++it)
	{
		// previous and next point 
		// set to the same as current point for line ends 
		const_iterator itPrev = it; 
		const_iterator itNext = it; 
		if (itPrev != begin())
			--itPrev;
		++itNext;
		if (itNext == end())
			--itNext;

		// get three points 
		point_type point = *it; 
		point_type neighborPoints[2] = {*itPrev, *itNext};
		// skip duplicate points 
		if (point == neighborPoints[0] && point == neighborPoints[1])
			continue; 
		// compute the line function for each line segment 
		// hy = kx + b, add h for the case that k = 1, h = 0
		std::fill(theta, theta+2, std::numeric_limits<double>::max());
		for (int i = 0; i < 2; ++i)
		{
			// always start with point, which is related to angle computation 
			double dx = neighborPoints[i].x()-point.x();
			double dy = neighborPoints[i].y()-point.y();

			if (dx != 0 || dy != 0)
			{
				double sinTheta = dy/sqrt(dx*dx+dy*dy);
				double cosTheta = dx/sqrt(dx*dx+dy*dy);
				theta[i] = (sinTheta >= 0)? acos(cosTheta) : 2*M_PI-acos(cosTheta);
			}
		}
		limboAssert(theta[0] != std::numeric_limits<double>::max() || theta[1] != std::numeric_limits<double>::max());
		if (theta[0] == std::numeric_limits<double>::max())
			theta[0] = theta[1]+M_PI;
		else if (theta[1] == std::numeric_limits<double>::max())
			theta[1] = theta[0]+M_PI;

        // extend line ends for half width 
        if (it == itPrev)
            point = gtl::construct<point_type>(round(point.x()+width()/2.0*cos(theta[0])), point.y()+width()/2.0*sin(theta[0]));
        if (it == itNext)
            point = gtl::construct<point_type>(round(point.x()+width()/2.0*cos(theta[1])), point.y()+width()/2.0*sin(theta[1]));

		double targetTheta = (theta[0]+theta[1])/2;
		// need to form a loop of points in order 
        point_type polygonPoints[2];
        if (vPoint.empty()) // first two points 
        {
            double targetDx = width()/2.0*cos(targetTheta);
            double targetDy = width()/2.0*sin(targetTheta);
            polygonPoints[0] = gtl::construct<point_type>(round(point.x()+targetDx), round(point.y()+targetDy)); 
            polygonPoints[1] = gtl::construct<point_type>(round(point.x()-targetDx), round(point.y()-targetDy));
        }
        else // following points need to be parallel 
        {
            for (int i = 0; i < 2; ++i)
            {
                point_type prevPoint = (i == 0)? vPoint.front() : vPoint.back();
                // a bit math here 
                // suppose the target point (tx, ty), middle point (x, y), previous two points (xi, yi) (i == 0, 1 considering front and back)
                // 1. the segment (tx, ty) to (x, y) has an angle of targetTheta in the coordinate system 
                // 2. the segment (tx, ty) to (xi, yi) has an angle of theta[0], because they are parallel to the segment of (x, y) to middle of (xi, yi)
                // I use the property for angle computation as follows 
                // dx/cos(theta) = dy/sin(theta) => dx*sin(theta) = dy*cos(theta) to avoid zero division 
                // solving the two equations with two variables, we have following solution
                polygonPoints[i] = gtl::construct<point_type>(
                        round(
                            (prevPoint.y()*cos(theta[0])*cos(targetTheta) - point.y()*cos(theta[0])*cos(targetTheta) - prevPoint.x()*cos(targetTheta)*sin(theta[0]) + point.x()*cos(theta[0])*sin(targetTheta)) / (cos(theta[0])*sin(targetTheta) - cos(targetTheta)*sin(theta[0]))
                            ), 
                        round(
                            (prevPoint.y()*cos(theta[0])*sin(targetTheta) - point.y()*cos(targetTheta)*sin(theta[0]) - prevPoint.x()*sin(theta[0])*sin(targetTheta) + point.x()*sin(theta[0])*sin(targetTheta)) / (cos(theta[0])*sin(targetTheta) - cos(targetTheta)*sin(theta[0]))
                            )); 
            }
        }
		vPoint.push_front(polygonPoints[0]);
		vPoint.push_back(polygonPoints[1]);
	}

	// construct polygon 
	GdsPolygon polygon; 
	polygon.setLayer(layer()); 
	polygon.setDatatype(datatype());
	polygon.set(vPoint.begin(), vPoint.end());
	return polygon;
}

GdsText::GdsText()
	: GdsText::base_type()
	, m_texttype(std::numeric_limits<int>::max())
	, m_text("")
	, m_position(gtl::construct<GdsText::point_type>(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()))
	, m_width(std::numeric_limits<int>::max())
	, m_presentation(std::numeric_limits<int>::max())
	, m_angle(std::numeric_limits<double>::max())
	, m_magnification(std::numeric_limits<double>::max())
	, m_strans(std::numeric_limits<int>::max())
{
}

GdsText::GdsText(GdsText const& rhs) 
	: GdsText::base_type(rhs)
	, m_texttype(rhs.m_texttype)
	, m_text(rhs.m_text)
	, m_position(rhs.m_position)
	, m_width(rhs.m_width)
	, m_presentation(rhs.m_presentation)
	, m_angle(rhs.m_angle)
	, m_magnification(rhs.m_magnification)
	, m_strans(rhs.m_strans)
{
}

GdsText& GdsText::operator=(GdsText const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs); 
		m_texttype = rhs.m_texttype;
		m_text = rhs.m_text;
		m_position = rhs.m_position; 
		m_width = rhs.m_width; 
		m_presentation = rhs.m_presentation; 
		m_angle = rhs.m_angle; 
		m_magnification = rhs.m_magnification; 
		m_strans = rhs.m_strans; 
	}
	return *this; 
}

GdsText::~GdsText() 
{
}

GdsCellReference::GdsCellReference()
	: GdsCellReference::base_type()
	, m_refCell()
	, m_position(gtl::construct<GdsCellReference::point_type>(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()))
	, m_angle(std::numeric_limits<double>::max())
	, m_magnification(std::numeric_limits<double>::max())
	, m_strans(std::numeric_limits<int>::max())
{
}

GdsCellReference::GdsCellReference(GdsCellReference const& rhs) 
	: GdsCellReference::base_type(rhs)
	, m_refCell(rhs.m_refCell)
	, m_position(rhs.m_position)
	, m_angle(rhs.m_angle)
	, m_magnification(rhs.m_magnification)
	, m_strans(rhs.m_strans)
{
}

GdsCellReference& GdsCellReference::operator=(GdsCellReference const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs); 
		m_refCell = rhs.m_refCell; 
		m_position = rhs.m_position; 
		m_angle = rhs.m_angle; 
		m_magnification = rhs.m_magnification; 
		m_strans = rhs.m_strans; 
	}
	return *this; 
}

GdsCellReference::~GdsCellReference() 
{
}

GdsCell GdsCellReference::extractCellRef(GdsDB const& gdsDB, GdsCell const& srcCell) const 
{
	GdsCell targetCell; 
	limboAssertMsg(refCell() == srcCell.name(), "mismatch between reference cell %s and source cell %s", refCell().c_str(), srcCell.name().c_str());

	targetCell.setName(srcCell.name()+"_SREF");
	// first, try to deep copy all the objects 
	for (std::vector<GdsCell::object_entry_type>::const_iterator it = srcCell.objects().begin(), ite = srcCell.objects().end(); it != ite; ++it)
	{
		GdsObjectHelpers()(it->first, it->second, ExtractCellObjectAction(gdsDB, srcCell, targetCell));
	}
	// second, apply rotation, magnification, shifting  
	for (std::vector<GdsCell::object_entry_type>::iterator it = targetCell.objects().begin(), ite = targetCell.objects().end(); it != ite; ++it)
	{
		GdsObjectHelpers()(it->first, it->second, ApplyCellReferenceAction(*this)); 
	}

	return targetCell; 
}

GdsCellArray::GdsCellArray()
	: GdsCellArray::base_type()
{
	m_refCell.clear();
	m_columns = std::numeric_limits<int>::max(); 
	m_rows = std::numeric_limits<int>::max(); 
	m_vPosition.clear();
	m_spacing[0] = std::numeric_limits<int>::max(); 
	m_spacing[1] = std::numeric_limits<int>::max(); 
	m_angle = std::numeric_limits<double>::max();
	m_magnification = std::numeric_limits<double>::max();
	m_strans = std::numeric_limits<int>::max();
}

GdsCellArray::GdsCellArray(GdsCellArray const& rhs) 
	: GdsCellArray::base_type(rhs)
{
	m_refCell = rhs.m_refCell; 
	m_columns = rhs.m_columns; 
	m_rows = rhs.m_rows; 
	m_spacing[0] = rhs.m_spacing[0]; 
	m_spacing[1] = rhs.m_spacing[1]; 
	m_vPosition = rhs.m_vPosition; 
	m_angle = rhs.m_angle; 
	m_magnification = rhs.m_magnification; 
	m_strans = rhs.m_strans; 
}

GdsCellArray& GdsCellArray::operator=(GdsCellArray const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs); 
		m_refCell = rhs.m_refCell; 
		m_columns = rhs.m_columns; 
		m_rows = rhs.m_rows; 
		m_spacing[0] = rhs.m_spacing[0]; 
		m_spacing[1] = rhs.m_spacing[1]; 
		m_vPosition = rhs.m_vPosition; 
		m_angle = rhs.m_angle; 
		m_magnification = rhs.m_magnification; 
		m_strans = rhs.m_strans; 
	}
	return *this; 
}

GdsCellArray::~GdsCellArray() 
{
}

GdsCell::GdsCell() 
	: GdsCell::base_type()
{
}

GdsCell::GdsCell(GdsCell const& rhs) 
	: GdsCell::base_type(rhs)
{
	copy(rhs);
}

GdsCell& GdsCell::operator=(GdsCell const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs);
		copy(rhs); 
	}
	return *this; 
}

GdsCell::~GdsCell() 
{
	destroy();
}

void GdsCell::copy(GdsCell const& rhs)
{
	m_name = rhs.m_name; 
	destroy(); 
	m_vObject.resize(rhs.m_vObject.size()); 
	for (std::size_t i = 0, ie = rhs.m_vObject.size(); i < ie; ++i)
	{
		GdsObjectHelpers()(rhs.m_vObject[i].first, rhs.m_vObject[i].second, CopyCellObjectAction(m_vObject[i])); 
	}
}

void GdsCell::destroy() 
{
	for (std::vector<object_entry_type>::iterator it = m_vObject.begin(), ite = m_vObject.end(); it != ite; ++it)
	{
		GdsObjectHelpers()(it->first, it->second, DeleteCellObjectAction(*it)); 
	}
}


void GdsCell::addPolygon(int layer, int datatype, std::vector<point_type> const& vPoint)
{
	GdsPolygon* polygon = new GdsPolygon(); 
	m_vObject.push_back(std::make_pair(::GdsParser::GdsRecords::BOUNDARY, polygon)); 
	polygon->setLayer(layer); 
	polygon->setDatatype(datatype); 
	polygon->set(vPoint.begin(), vPoint.end()); 
}

void GdsCell::addPath(int layer, int datatype, int pathtype, int width, std::vector<point_type> const& vPoint)
{
	GdsPath* path = new GdsPath(); 
	m_vObject.push_back(std::make_pair(::GdsParser::GdsRecords::PATH, path)); 
	path->setLayer(layer); 
	path->setDatatype(datatype); 
	path->setPathtype(pathtype); 
	path->setWidth(width); 
	path->set(vPoint.begin(), vPoint.end()); 
}

void GdsCell::addText(int layer, int datatype, int texttype, std::string const& str, point_type const& position, int width, int presentation, double angle, double magnification, int strans)
{
	GdsText* text = new GdsText(); 
	m_vObject.push_back(std::make_pair(::GdsParser::GdsRecords::TEXT, text)); 
	text->setLayer(layer); 
	text->setDatatype(datatype); 
	text->setTexttype(texttype); 
	text->setText(str); 
	text->setPosition(position); 
	text->setWidth(width);
	text->setPresentation(presentation); 
	text->setAngle(angle); 
	text->setMagnification(magnification); 
	text->setStrans(strans); 
}

void GdsCell::addCellReference(std::string const& sname, point_type const& position, double angle, double magnification, int strans)
{
	GdsCellReference* cellRef = new GdsCellReference(); 
	m_vObject.push_back(std::make_pair(::GdsParser::GdsRecords::SREF, cellRef)); 
	cellRef->setRefCell(sname); 
	cellRef->setPosition(position); 
	cellRef->setAngle(angle); 
	cellRef->setMagnification(magnification); 
	cellRef->setStrans(strans); 
}

void GdsCell::addCellArray(std::string const& sname, int columns, int rows, int spacing[2], std::vector<GdsCell::point_type> const& vPosition, double angle, double magnification, int strans)
{
	GdsCellArray* cellArray = new GdsCellArray(); 
	m_vObject.push_back(std::make_pair(::GdsParser::GdsRecords::AREF, cellArray)); 
	cellArray->setRefCell(sname); 
	cellArray->setColumns(columns); 
	cellArray->setRows(rows); 
	cellArray->setSpacing(0, spacing[0]); 
	cellArray->setSpacing(1, spacing[1]); 
	cellArray->positions().assign(vPosition.begin(), vPosition.end()); 
	cellArray->setAngle(angle); 
	cellArray->setMagnification(magnification); 
	cellArray->setStrans(strans); 
}

GdsDB::GdsDB()
	: GdsDB::base_type()
{
	m_header.clear();
	m_libname.clear();
	m_unit = std::numeric_limits<double>::max();
	m_precision = std::numeric_limits<double>::max();
	m_vCell.clear();
}

GdsDB::GdsDB(GdsDB const& rhs)
	: GdsDB::base_type(rhs)
	, m_header(rhs.m_header)
	, m_libname(rhs.m_libname)
	, m_unit(rhs.m_unit)
	, m_precision(rhs.m_precision)
	, m_vCell(rhs.m_vCell)
{
}

GdsDB& GdsDB::operator=(GdsDB const& rhs)
{
	if (this != &rhs)
	{
		this->base_type::operator=(rhs);
		m_header = rhs.m_header; 
		m_libname = rhs.m_libname;
		m_unit = rhs.m_unit;
		m_precision = rhs.m_precision;
		m_vCell = rhs.m_vCell; 
	}
	return *this; 
}

GdsDB::~GdsDB()
{
}

void GdsDB::setHeader(int h) 
{
	char buf[64]; 
	limboSPrint(limbo::kNONE, buf, "%d", h);
	m_header = buf; 
}

GdsCell& GdsDB::addCell(std::string const& name)
{
	// update cell map 
	limboAssertMsg(m_mCellName2Idx.insert(std::make_pair(name, (unsigned int)m_vCell.size())).second, "duplicate cell name %s", name.c_str()); 
	// add cell 
	m_vCell.push_back(GdsCell()); 
	m_vCell.back().setName(name); 
	return m_vCell.back();
}

GdsCell& GdsDB::addCell(GdsCell const& cell) 
{
	// update cell map 
	limboAssertMsg(m_mCellName2Idx.insert(std::make_pair(cell.name(), (unsigned int)m_vCell.size())).second, "duplicate cell name %s", cell.name().c_str()); 
	// add cell 
	m_vCell.push_back(cell); 
	return m_vCell.back();
}

GdsCell const* GdsDB::getCell(std::string const& cellName) const 
{
	std::map<std::string, unsigned int>::const_iterator found = m_mCellName2Idx.find(cellName); 
	if (found == m_mCellName2Idx.end())
		return NULL; 
	else return &m_vCell[found->second]; 
}

GdsCell* GdsDB::getCell(std::string const& cellName) 
{
	std::map<std::string, unsigned int>::const_iterator found = m_mCellName2Idx.find(cellName); 
	if (found == m_mCellName2Idx.end())
		return NULL; 
	else return &m_vCell[found->second]; 
}

GdsCell GdsDB::extractCell(std::string const& cellName) const 
{
	GdsCell const* srcCell = getCell(cellName);
	GdsCell targetCell; 
	// return empty cell if not found  
	if (!srcCell)
		return targetCell; 

	targetCell.setName(srcCell->name());
	for (std::vector<GdsCell::object_entry_type>::const_iterator it = srcCell->objects().begin(), ite = srcCell->objects().end(); it != ite; ++it)
	{
		GdsObjectHelpers()(it->first, it->second, ExtractCellObjectAction(*this, *srcCell, targetCell));
	}

	return targetCell; 
}

}} // namespace GdsParser // GdsDB

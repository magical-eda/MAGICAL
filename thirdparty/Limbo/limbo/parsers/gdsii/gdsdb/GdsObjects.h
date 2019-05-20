/**
 * @file   GdsObjects.h
 * @brief  Various GDSII objects and a top GDSII database 
 * @author Yibo Lin
 * @date   Oct 2016
 */

#ifndef LIMBO_PARSERS_GDSII_GDSDB_GDSOBJECTS_H
#define LIMBO_PARSERS_GDSII_GDSDB_GDSOBJECTS_H

#include <boost/geometry.hpp>
// use adapted boost.polygon in boost.geometry, which is compatible to rtree
#include <boost/geometry/geometries/adapted/boost_polygon.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <limbo/parsers/gdsii/stream/GdsReader.h>

/// namespace for Limbo.GdsParser 
namespace GdsParser 
{
/// namespace for Limbo.GdsParser.GdsDB 
namespace GdsDB 
{

// some shortcuts for namespace 
namespace gtl = boost::polygon;
namespace bg = boost::geometry;

// forward declaration 
class GdsCell; 
class GdsDB; 

/**
	Base class for all gdsii objects \n
	All data members are initialized to maximum infinity \n
*/
class GdsObject 
{
	public:
		/// coordinate types 
		typedef int coordinate_type;
		/// @name geometry types 
        ///@{
        /// point type 
		typedef gtl::point_data<coordinate_type> point_type;
        /// interval type 
		typedef gtl::interval_data<coordinate_type> interval_type; 
        /// rectangle type 
		typedef gtl::rectangle_data<coordinate_type> rectangle_type;
        /// polygon type 
		typedef gtl::polygon_data<coordinate_type> polygon_type;
        /// polygon set type 
		typedef gtl::polygon_90_set_data<coordinate_type> polygon_set_type;
        ///@}

		/// @brief default constructor
		GdsObject();
		/// @brief copy constructor 
        /// @param rhs an object 
		GdsObject(GdsObject const& rhs);
		/// @brief assignment 
        /// @param rhs an object 
		GdsObject& operator=(GdsObject const& rhs);
		/// @brief destructor
        /// 
		/// Meaningless to have pure virtual destructor 
		/// because we have to define it anyway 
		virtual ~GdsObject(); 
};

/**
	Base class for all gdsii shapes with layer and datatype attributes  
*/
class GdsShape : public GdsObject
{
	public:
        /// base type 
		typedef GdsObject base_type;

		/// default constructor 
		GdsShape();
		/// copy constructor 
        /// @param rhs an object 
		GdsShape(GdsShape const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsShape& operator=(GdsShape const& rhs); 
		/// destructor 
		~GdsShape(); 

		// accessors
        /// @return layer 
		int layer() const {return m_layer;}
        /// @param l layer 
		void setLayer(int l) {m_layer = l;}

        /// @return data type 
		int datatype() const {return m_datatype;}
        /// @param d data type 
		void setDatatype(int d) {m_datatype = d;}
	protected:
		int m_layer; ///< layer 
		int m_datatype; ///< data type 
};

/**
    Rectangular geometric object.\n
\n
    Parameters\n
    ----------\n
    point1 : array-like[2]\n
        Coordinates of a corner of the rectangle.\n
    point2 : array-like[2]\n
        Coordinates of the corner of the rectangle opposite to ``point1``.\n
    layer : integer\n
        The GDSII layer number for this element.\n
    datatype : integer\n
        The GDSII datatype for this element (between 0 and 255).\n
\n
    Examples\n
    --------\n
    >>> rectangle = gdspy.Rectangle((0, 0), (10, 20))\n
    >>> myCell.add(rectangle)\n
*/
class GdsRectangle : public GdsShape, public gtl::rectangle_data<int>
{
	public:
        /// base type  
		typedef GdsShape base_type; 
        /// rectangle type 
		typedef gtl::rectangle_data<int> base_ext_type; 

		/// default constructor
		GdsRectangle(); 
		/// copy constructor 
        /// @param rhs an object 
		GdsRectangle(GdsRectangle const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsRectangle& operator=(GdsRectangle const& rhs);
		/// destructor 
		~GdsRectangle(); 
};

/**
    Polygonal geometric object.\n
\n
    Parameters\n
    ----------\n
    points : array-like[N][2]\n
        Coordinates of the vertices of the polygon.\n
    layer : integer\n
        The GDSII layer number for this element.\n
    datatype : integer\n
        The GDSII datatype for this element (between 0 and 255).\n
    verbose : bool\n
        If False, warnings about the number of vertices of the polygon will\n
        be suppressed.\n
\n
    Notes\n
    -----\n
    The last point should not be equal to the first (polygons are\n
    automatically closed).\n
\n
    The GDSII specialization supports only a maximum of 199 vertices per\n
    polygon.\n
\n
    Examples\n
    --------\n
    >>> triangle_pts = [(0, 40), (15, 40), (10, 50)]\n
    >>> triangle = gdspy.Polygon(triangle_pts)\n
    >>> myCell.add(triangle)\n
*/
class GdsPolygon : public GdsShape, public gtl::polygon_data<GdsShape::coordinate_type>
{
	public:
        /// base type 
		typedef GdsShape base_type; 
        /// use polygon_data as polygon type 
		typedef gtl::polygon_data<GdsShape::coordinate_type> base_ext_type; 
        /// @nowarn 
		typedef base_type::coordinate_type coordinate_type; 
		typedef base_type::point_type point_type; 
        /// @endnowarn

		/// default constructor
		GdsPolygon(); 
		/// copy constructor 
        /// @param rhs an object 
		GdsPolygon(GdsPolygon const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsPolygon& operator=(GdsPolygon const& rhs);
		/// destructor 
		~GdsPolygon(); 
};

/**
    Series of geometric objects that form a path or a collection of parallel\n
    paths.\n
\n
    Parameters\n
    ----------\n
    width : number\n
        The width of each path.\n
    initial_point : array-like[2]\n
        Starting position of the path.\n
    number_of_paths : positive integer\n
        Number of parallel paths to create simultaneously.\n
    distance : number\n
        Distance between the centers of adjacent paths.\n
\n
    Attributes\n
    ----------\n
    x : number\n
        Current position of the path in the x direction.\n
    y : number\n
        Current position of the path in the y direction.\n
    w : number\n
        *Half*-width of each path.\n
    n : integer\n
        Number of parallel paths.\n
    direction : {'+x', '-x', '+y', '-y'} or number\n
        Direction or angle (in *radians*) the path points to.\n
    distance : number\n
        Distance between the centers of adjacent paths.\n
    length : number\n
        Length of the central path axis. If only one path is created, this\n
        is the real length of the path.\n
*/
class GdsPath : public GdsShape, public bg::model::linestring<GdsShape::point_type>
{
	public:
        /// base type 
		typedef GdsShape base_type; 
        /// use a linestring to describe path 
		typedef bg::model::linestring<GdsShape::point_type> base_ext_type; 

		/// default constructor
		GdsPath(); 
		/// copy constructor 
        /// @param rhs an object 
		GdsPath(GdsPath const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsPath& operator=(GdsPath const& rhs);
		/// destructor 
		~GdsPath(); 

		// accessors 
        /// @return path type 
		int pathtype() const {return m_pathtype;}
        /// @param p path type 
		void setPathtype(int p) {m_pathtype = p;}

        /// @return width 
		coordinate_type width() const {return m_width;}
        /// @param w width 
		void setWidth(coordinate_type w) {m_width = w;}

        /// @brief set path 
        /// @tparam Iterator iterator to points 
        /// @param first, last begin and end iterator to points 
		template <typename Iterator>
		void set(Iterator first, Iterator last) {this->assign(first, last);}

		/// @brief convert from path to polygon description 
        /// @return a GDSII polygon object  
		GdsPolygon toPolygon() const; 
	protected:
		int m_pathtype; ///< path type 
		coordinate_type m_width; ///< path width 
};

/**
    Polygonal text object.\n
\n
    Each letter is formed by a series of polygons.\n
\n
    Parameters\n
    ----------\n
    text : string\n
        The text to be converted in geometric objects.\n
    position : array-like[2]\n
        Text position (lower left corner).\n
	width : integer \n
		Width of the text ?\n
	presentation : integer \n
		Related to the direction of text \n
    angle : number\n
        The angle of rotation of the text.\n
    size (replaced by magnification) : number\n
        Base size of each character.\n
    x_reflection (replaced by strans) : bool\n
        If ``True``, the reference is reflected parallel to the x direction\n
        before being rotated.\n
    layer : integer\n
        The GDSII layer number for these elements.\n
    datatype : integer\n
        The GDSII datatype for this element (between 0 and 255).\n
\n
    Examples\n
    --------\n
    >>> text = gdspy.Text('Sample text', 20, (-10, -100))\n
    >>> myCell.add(text)\n
*/
class GdsText : public GdsShape
{
	public:
        /// base type 
		typedef GdsShape base_type; 

		/// default constructor
		GdsText(); 
		/// copy constructor 
        /// @param rhs an object 
		GdsText(GdsText const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsText& operator=(GdsText const& rhs);
		/// destructor 
		~GdsText(); 

		// accessors 
        /// @return text type 
		int texttype() const {return m_texttype;}
        /// @param t text type 
		void setTexttype(int t) {m_texttype = t;}

        /// @return text string 
		std::string const& text() const {return m_text;}
        /// @param t text string 
		void setText(std::string const& t) {m_text = t;}

        /// @return width 
		int width() const {return m_width;}
        /// @param w width 
		void setWidth(int w) {m_width = w;}

        /// @return position 
		point_type const& position() const {return m_position;}
        /// @param p position 
		void setPosition(point_type const& p) {m_position = p;}

        /// @return presentation 
		int presentation() const {return m_presentation;}
        /// @param p presentation 
		void setPresentation(int p) {m_presentation = p;}

        /// @return angle 
		double angle() const {return m_angle;}
        /// @param a angle 
		void setAngle(double a) {m_angle = a;}

        /// @return magnification 
		double magnification() const {return m_magnification;}
        /// @param m magnification 
		void setMagnification(double m) {m_magnification = m;}

        /// @return strans 
		int strans() const {return m_strans;}
        /// @param s strans 
		void setStrans(int s) {m_strans = s;}
	protected:
		int m_texttype; ///< text type 
		std::string m_text; ///< text string 
		point_type m_position; ///< position 
		int m_width; ///< width 
		int m_presentation; ///< presentation 
		double m_angle; ///< angle 
		double m_magnification; ///< magnification 
		int m_strans; ///< strans 
};

/**
    Simple reference to an existing cell.\n
\n
    Parameters\n
    ----------\n
    ref_cell : ``Cell`` or string\n
        The referenced cell or its name.\n
    origin (replaced with position) : array-like[2]\n
        Position where the reference is inserted.\n
    angle : number\n
        Angle of rotation of the reference (in *degrees*).\n
    magnification : number\n
        Magnification factor for the reference.\n
    x_reflection (replaced by strans) : bool\n
        If ``True``, the reference is reflected parallel to the x direction\n
        before being rotated.\n
*/
class GdsCellReference : public GdsObject
{
	public:
        /// base type 
		typedef GdsObject base_type; 

		/// default constructor
		GdsCellReference(); 
		/// copy constructor 
        /// @param rhs an object 
		GdsCellReference(GdsCellReference const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsCellReference& operator=(GdsCellReference const& rhs);
		/// destructor 
		~GdsCellReference(); 

		// accessors 
        /// @return reference cell 
		std::string const& refCell() const {return m_refCell;}
        /// @param r set reference cell 
		void setRefCell(std::string const& r) {m_refCell = r;}

        /// @return position 
		point_type const& position() const {return m_position;}
        /// @param p position 
		void setPosition(point_type const& p) {m_position = p;}

        /// @return angle 
		double angle() const {return m_angle;}
        /// @param a angle 
		void setAngle(double a) {m_angle = a;}

        /// @return magnification 
		double magnification() const {return m_magnification;}
        /// @param m magnification 
		void setMagnification(double m) {m_magnification = m;}

        /// @return strans 
		int strans() const {return m_strans;}
        /// @param s strans 
		void setStrans(int s) {m_strans = s;}

		/// extract cell objects from reference cell \n
		/// consider rotation and position offset 
        /// @param gdsDB database 
        /// @param srcCell source cell 
        /// @return a GDSII cell with absolute position 
		GdsCell extractCellRef(GdsDB const& gdsDB, GdsCell const& srcCell) const; 
	protected:
		std::string m_refCell; ///< string to reference cell 
		point_type m_position; ///< position 
		double m_angle; ///< angle 
		double m_magnification; ///< magnification 
		int m_strans; ///< strans 
};

/**
    Multiple references to an existing cell in an array format.\n
\n
    Parameters\n
    ----------\n
    ref_cell : ``Cell`` or string\n
        The referenced cell or its name.\n
    columns : positive integer\n
        Number of columns in the array.\n
    rows : positive integer\n
        Number of columns in the array.\n
    spacing : array-like[2]\n
        distances between adjacent columns and adjacent rows.\n
    origin (replaced with position) : array-like[2]\n
        Position where the cell is inserted.\n
    angle : number\n
        Angle of rotation of the reference (in *degrees*).\n
    magnification : number\n
        Magnification factor for the reference.\n
    x_reflection (replaced by strans) : bool\n
        If ``True``, the reference is reflected parallel to the x direction\n
        before being rotated.\n
*/
class GdsCellArray : public GdsObject
{
	public:
        /// base type 
		typedef GdsObject base_type; 

		/// default constructor
		GdsCellArray(); 
		/// copy constructor 
        /// @param rhs an object 
		GdsCellArray(GdsCellArray const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsCellArray& operator=(GdsCellArray const& rhs);
		/// destructor 
		~GdsCellArray(); 

		// accessors 
        /// @return reference cell 
		std::string const& refCell() const {return m_refCell;}
        /// @param r reference cell 
		void setRefCell(std::string const& r) {m_refCell = r;}

        /// @return number of columns 
		int columns() const {return m_columns;}
        /// @param c number of columns 
		void setColumns(int c) {m_columns = c;}

        /// @return number of rows 
		int rows() const {return m_rows;}
        /// @param r number of rows 
		void setRows(int r) {m_rows = r;}

        /// @param idx x or y 
        /// @return spacing 
		coordinate_type spacing(unsigned int idx) const {return m_spacing[idx];}
        /// @param idx x or y 
        /// @param s spacing 
		void setSpacing(unsigned int idx, coordinate_type s) {m_spacing[idx] = s;}

        /// @return array of positions 
		std::vector<point_type> const& positions() const {return m_vPosition;}
        /// @return array of positions 
		std::vector<point_type>& positions() {return m_vPosition;}

        /// @return angle 
		double angle() const {return m_angle;}
        /// @param a angle 
		void setAngle(double a) {m_angle = a;}

        /// @return magnification
		double magnification() const {return m_magnification;}
        /// @param m magnification
		void setMagnification(double m) {m_magnification = m;}

        /// @return strans 
		int strans() const {return m_strans;}
        /// @param s strans 
		void setStrans(int s) {m_strans = s;}
	protected:
		std::string m_refCell; ///< reference cell 
		int m_columns; ///< number of columns 
		int m_rows; ///< number of rows 
		coordinate_type m_spacing[2]; ///< spacing of x and y 
		std::vector<point_type> m_vPosition; ///< array of positions 
		double m_angle; ///< angle 
		double m_magnification; ///< magnification 
		int m_strans; ///< strans 
};

/**
    Collection of elements, both geometric objects and references to other\n
    cells.\n
\n
    Parameters\n
    ----------\n
    name : string\n
        The name of the cell.\n
    exclude_from_global : bool\n
        If ``True``, the cell will not be included in the global list of\n
        cells maintained by ``gdspy``.\n
*/
class GdsCell : public GdsObject
{
	public:
        /// base type 
		typedef GdsObject base_type; 
        /// use pair of GDSII record and GDSII object to describe each object entry 
		typedef std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*> object_entry_type; 

		/// default constructor
		GdsCell(); 
		/// copy constructor 
        /// @param rhs an object 
		GdsCell(GdsCell const& rhs); 
		/// assignment 
        /// @param rhs an object 
		GdsCell& operator=(GdsCell const& rhs);
		/// destructor 
		~GdsCell(); 

        /// @param layer layer 
        /// @param datatype data type 
        /// @param vPoint array of points 
		void addPolygon(int layer, int datatype, std::vector<point_type> const& vPoint); 
        /// @param layer layer 
        /// @param datatype data type 
        /// @param pathtype path type 
        /// @param width path width 
        /// @param vPoint array of points 
		void addPath(int layer, int datatype, int pathtype, int width, std::vector<point_type> const& vPoint);
        /// @param layer layer 
        /// @param datatype data type 
        /// @param texttype text type 
        /// @param str text string 
        /// @param position position 
        /// @param width width 
        /// @param presentation presentation
        /// @param angle angle 
        /// @param magnification magnification 
        /// @param strans strans 
		void addText(int layer, int datatype, int texttype, std::string const& str, point_type const& position, int width, int presentation, double angle, double magnification, int strans); 
        /// @param sname reference cell name 
        /// @param position offset position 
        /// @param angle angle 
        /// @param magnification magnification
        /// @param strans strans 
		void addCellReference(std::string const& sname, point_type const& position, double angle, double magnification, int strans); 
        /// @param sname reference cell name 
        /// @param columns number of columns 
        /// @param rows number of rows 
        /// @param spacing array[2] of spacing 
        /// @param vPosition array of positions 
        /// @param angle angle 
        /// @param magnification magnification 
        /// @param strans strans 
		void addCellArray(std::string const& sname, int columns, int rows, int spacing[2], std::vector<point_type> const& vPosition, double angle, double magnification, int strans); 

		// accessors 
        /// @return name of cell 
		std::string const& name() const {return m_name;}
        /// @param n name of cell 
		void setName(std::string const& n) {m_name = n;}

        /// @return reference to array of GDSII object entries 
		std::vector<std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*> > const& objects() const {return m_vObject;}
        /// @return reference to array of GDSII object entries 
		std::vector<std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*> >& objects() {return m_vObject;}
	protected:
		/// copy 
        /// @param rhs a GdsCell object 
		void copy(GdsCell const& rhs); 
		/// destroy objects in m_vObject
		void destroy(); 

		std::string m_name; ///< cell name 
		std::vector<object_entry_type> m_vObject; ///< gdsii objects with types 
};

/**
	Top level gdsii database \n
\n
    Parameters\n
    ----------\n
    header : string\n
        The header of gdsii file.\n
	libname : string \n
		The name of gdsii library \n
	unit : float \n
		The user unit, not very useful  \n
	precision : float \n
		The database unit in meter \n
*/
class GdsDB : public GdsObject
{
	public:
        /// base type 
		typedef GdsObject base_type; 

		/// constructor 
		GdsDB();
		/// copy constructor 
        /// @param rhs a GdsDB object 
		GdsDB(GdsDB const& rhs);
		/// assignment 
        /// @param rhs a GdsDB object 
		GdsDB& operator=(GdsDB const& rhs);
		/// destructor 
		~GdsDB();

		/// @brief add a cell and return the reference to the new cell 
        /// @param name cell name 
		GdsCell& addCell(std::string const& name = ""); 
        /// @brief add a cell 
        /// @param cell cell object 
        GdsCell& addCell(GdsCell const& cell); 

		// accessors 
        /// @return header string 
		std::string const& header() const {return m_header;}
        /// @param h header 
		void setHeader(std::string const& h) {m_header = h;}
        /// @param h header in integer 
		void setHeader(int h); 

        /// @return name of library 
		std::string const& libname() const {return m_libname;}
        /// @param n name of library 
		void setLibname(std::string const& n) {m_libname = n;}

        /// @return unit 
		double unit() const {return m_unit;}
        /// @param u unit 
		void setUnit(double u) {m_unit = u;}

        /// @return precision 
		double precision() const {return m_precision;}
        /// @param p precision 
		void setPrecision(double p) {m_precision = p;}

        /// @return reference to array of cells 
		std::vector<GdsCell> const& cells() const {return m_vCell;}
        /// @return reference to array of cells 
		std::vector<GdsCell>& cells() {return m_vCell;}

		/// @brief given cell name and return the pointer to the cell, 
		/// return NULL if not found 
        /// @param cellName cell name 
        /// @return pointer to the cell, NULL if not found 
		GdsCell const* getCell(std::string const& cellName) const; 
		/// @brief given cell name and return the pointer to the cell, 
		/// return NULL if not found 
        /// @param cellName cell name 
        /// @return pointer to the cell, NULL if not found 
		GdsCell* getCell(std::string const& cellName); 

		/// @brief extract a cell into a new cell with flatten hierarchies 
        /// @param cellName cell name 
		GdsCell extractCell(std::string const& cellName) const; 
	protected:
		std::string m_header; ///< header 
		std::string m_libname; ///< name of library 
		double m_unit; ///< unit 
		double m_precision; ///< precision 
		std::vector<GdsCell> m_vCell; ///< cell array  
		std::map<std::string, unsigned int> m_mCellName2Idx; ///< map from cell name to index 
};

} // namespace GdsDB 
} // namespace GdsParser


/// Boost.Geometry 
namespace boost { namespace geometry { namespace traits {

/// @name specialization of Boost.Geometry traits for @ref GdsParser::GdsDB::GdsRectangle
///@{

/// specialization for tag 
template <>
struct tag<GdsParser::GdsDB::GdsRectangle > : public tag<GdsParser::GdsDB::GdsRectangle::base_ext_type>
{
};
/// specialization for point_type 
template <>
struct point_type<GdsParser::GdsDB::GdsRectangle >
{
    /// @nowarn 
    typedef GdsParser::GdsDB::GdsRectangle::point_type type;
    /// @endnowarn
};
///@}

/// @name specialization of Boost.Geometry traits for @ref GdsParser::GdsDB::GdsPolygon
///@{

/// specialization for tag 
template <>
struct tag<GdsParser::GdsDB::GdsPolygon > : public tag<GdsParser::GdsDB::GdsPolygon::base_ext_type>
{
};
/// specialization for point_type 
template <>
struct point_type<GdsParser::GdsDB::GdsPolygon >
{
    /// @nowarn
    typedef GdsParser::GdsDB::GdsObject::point_type type;
    /// @endnowarn
};
///@}

}}} // namespace boost // namespace geometry // namespace traits

/// Boost.Polygon
namespace boost { namespace polygon {

/// specialization for Boost.Polygon traits for @ref GdsParser::GdsDB::GdsRectangle
template <>
struct geometry_concept<GdsParser::GdsDB::GdsRectangle> 
{
    /// @nowarn
	typedef rectangle_concept type;
    /// @endnowarn
};

/// specialization for Boost.Polygon traits for @ref GdsParser::GdsDB::GdsPolygon
template <>
struct geometry_concept<GdsParser::GdsDB::GdsPolygon> 
{
    /// @nowarn 
	typedef polygon_concept type;
    /// @endnowarn
};

}} // namespace boost // namespace polygon

#endif

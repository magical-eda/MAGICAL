//
// Created by Keren on 09/14/2018
//

#include "WriteGds.h"

PROJECT_NAMESPACE_BEGIN

bool WriteGds::initWriter()
{
    if (_isActive) 
    { 
        ERR("%s: Error: Try initing a gdsWriter while there is already one in active! \n", __FUNCTION__); 
        return false; 
    }
    //_gw = GdsParser::GdsWriter(outfile.c_str());
    _isActive = true;
    return true;
}


bool WriteGds::createLib(const std::string &libName, RealType dbu_uu, RealType dbu_m)
{
    checkActive();
	_gw.create_lib(libName.c_str(), dbu_uu, dbu_m);
    return true;
}

bool WriteGds::endLib()
{
    checkActive();
    _gw.gds_write_endlib();
    _isActive = false; 
    return true;
}

bool WriteGds::writeCellBgn(const std::string &cellName)
{
    checkActive();
    _gw.gds_write_bgnstr();
    _gw.gds_write_strname(cellName.c_str());
    return true;
}

bool WriteGds::writeCellEnd()
{
    checkActive();
    _gw.gds_write_endstr();
    return true;
}

bool WriteGds::writeCellRef(std::string cellName, XY<IntType> loc, RealType mag, RealType angle)
{
    checkActive();
    _gw.gds_write_sref();                      /// Contain an instance of ...
    _gw.gds_write_sname(cellName.c_str());     /// the cell ...
    int x = loc.x(); int y = loc.y();
    _gw.gds_write_xy(&x, &y, 1);     /// at these coordinates (database units)
    //_gw.gds_write_mag(mag);                    /// scale some magnitude
    //_gw.gds_write_angle(angle);                /// tilted at some angle
    _gw.gds_write_endel(  );                   /// end of element
    return true;
}

bool WriteGds::writeRectangle(const Box<IntType> &box, IntType layer, IntType dataType)
{
    checkActive();
    _gw.gds_write_boundary(  );       /// write just the token
    _gw.gds_write_layer(static_cast<short>(layer)); /// write layer
    _gw.gds_write_datatype(static_cast<short> (dataType)); /// write datatype

    IntType x[5], y[5];
    x[0] = box.xLo(); y[0] = box.yLo();
    x[1] = box.xHi(); y[1] = box.yLo();
    x[2] = box.xHi(); y[2] = box.yHi();
    x[3] = box.xLo(); y[3] = box.yHi();
    x[4] = box.xLo(); y[4] = box.yLo();

    _gw.gds_write_xy(  x, y, 5 );    // polygon, four vertices, first vertex repeated => 5 points
    _gw.gds_write_endel(  );          // end of element
    return true;
}

bool WriteGds::writeText(const std::string &text, IntType x, IntType y, IntType layer, IntType size)
{
    _gw.gds_create_text(text.c_str(), x, y, layer, size);
    return true;
}


bool WriteGds::writePath(const std::vector<IntType> x, const std::vector<IntType> y, IntType width, IntType layer, IntType dataType, IntType endCapType)
{

    checkActive();
    Assert(x.size() == y.size());
    Assert(x.size() > 0);
    // create a path

    _gw.gds_write_path(  );
    _gw.gds_write_layer(layer);               // layer 
    _gw.gds_write_datatype(dataType);            // datatype 
    _gw.gds_write_pathtype(endCapType);            // extended square ends
    _gw.gds_write_width(width);             //  nm wide
    _gw.gds_write_xy(  &x[0], &y[0], x.size() );
    _gw.gds_write_endel(  );
    return true;
}
PROJECT_NAMESPACE_END

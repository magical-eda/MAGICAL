/**
 * @file   GdsIO.cpp
 * @brief  Implementation of wrapper to GDSII read and write with GDSII database @ref GdsParser::GdsDB::GdsDB
 * @author Yibo Lin
 * @date   Nov 2016
 */

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>
#include <limbo/preprocessor/Msg.h>
#include <exception>

namespace GdsParser { namespace GdsDB {

namespace gtl = boost::polygon;
using namespace gtl::operators;

bool GdsReader::operator() (std::string const& filename)  
{
    // calculate file size 
    std::ifstream in (filename.c_str());
    if (!in.good()) return false;
    std::streampos begin = in.tellg();
    in.seekg(0, std::ios::end);
    std::streampos end = in.tellg();
    m_fileSize = (end-begin);
    in.close();
	// reset temporary data 
	m_status = ::GdsParser::GdsRecords::UNKNOWN; 
	reset();
	m_vUnsupportRecord.assign(::GdsParser::GdsRecords::UNKNOWN, 0); 
    // read gds 
    bool flag = ::GdsParser::read(*this, filename);
	printUnsupportRecords();
	return flag; 
}

void GdsReader::reset() 
{
	m_string.clear(); ///< STRING 
	m_sname.clear(); ///< SNAME 
	m_layer = std::numeric_limits<int>::max(); ///< LAYER 
	m_datatype = std::numeric_limits<int>::max(); ///< DATATYPE
	m_pathtype = std::numeric_limits<int>::max(); ///< PATHTYPE
	m_texttype = std::numeric_limits<int>::max(); ///< TEXTTYPE
	m_spacing[0] = std::numeric_limits<int>::max(); ///< SPACING
	m_spacing[1] = std::numeric_limits<int>::max(); 
	m_width = std::numeric_limits<int>::max(); ///< WIDTH 
	m_columns = std::numeric_limits<int>::max(); ///< COLROW 
	m_rows = std::numeric_limits<int>::max(); 
	m_angle = std::numeric_limits<double>::max(); ///< ANGLE 
	m_magnification = std::numeric_limits<double>::max(); ///< MAG 
	m_strans = std::numeric_limits<int>::max(); ///< STRANS
	m_presentation = std::numeric_limits<int>::max(); ///< PRESENTATION
	m_vPoint.clear(); ///< XY
}

void GdsReader::printUnsupportRecords() const 
{
	for (int i = 0, ie = m_vUnsupportRecord.size(); i < ie; ++i)
	{
		if (m_vUnsupportRecord[i])
		{
			limboPrint(limbo::kWARN, "Unsupported record_type = %s message suppressed %u times\n", ::GdsParser::gds_record_ascii(i), m_vUnsupportRecord[i]);
		}
	}
}

void GdsReader::bit_array_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vBitArray)
{
    this->integer_cbk(record_type, data_type, vBitArray);
}
void GdsReader::integer_2_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vInteger)
{
    this->integer_cbk(record_type, data_type, vInteger);
}
void GdsReader::integer_4_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vInteger)
{
    this->integer_cbk(record_type, data_type, vInteger);
}
void GdsReader::real_4_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<double> const& vFloat) 
{
    this->float_cbk(record_type, data_type, vFloat);
}
void GdsReader::real_8_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<double> const& vFloat) 
{
    this->float_cbk(record_type, data_type, vFloat);
}
void GdsReader::string_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::string const& str) 
{
    limboAssert(data_type == ::GdsParser::GdsData::STRING);
    switch (record_type)
    {
		case ::GdsParser::GdsRecords::HEADER: 
			limboAssert(m_status == ::GdsParser::GdsRecords::UNKNOWN); 
			m_db.setHeader(str); 
			break ;
        case ::GdsParser::GdsRecords::STRNAME: // a new cell begins 
			limboAssert(m_status == ::GdsParser::GdsRecords::BGNSTR); // in a cell 
			m_db.addCell(str); 
            break;
        case ::GdsParser::GdsRecords::LIBNAME: // a library 
			limboAssert(m_status == ::GdsParser::GdsRecords::BGNLIB); // in a lib 
			m_db.setLibname(str); 
			break; 
        case ::GdsParser::GdsRecords::STRING:
			limboAssert(m_status == ::GdsParser::GdsRecords::TEXT); // in a text 
			m_string = str; 
			break; 
        case ::GdsParser::GdsRecords::SNAME:
			m_sname = str; 
			break; 
        default: 
			{
				// only print invalid records or unsupported records for the first time 
				if (record_type >= (int)m_vUnsupportRecord.size() || m_vUnsupportRecord[record_type] == 0)
					limboPrint(limbo::kWARN, "%s() invalid record_type = %s, data_type = %s\n", __func__, ::GdsParser::gds_record_ascii(record_type), ::GdsParser::gds_data_ascii(data_type));
				m_vUnsupportRecord[record_type] += 1; 
			}
			break;
    }
}
void GdsReader::begin_end_cbk(::GdsParser::GdsRecords::EnumType record_type)
{
    switch (record_type)
    {
        case ::GdsParser::GdsRecords::BOX:
        case ::GdsParser::GdsRecords::BOUNDARY:
        case ::GdsParser::GdsRecords::PATH:
		case ::GdsParser::GdsRecords::TEXT:
		case ::GdsParser::GdsRecords::SREF:
		case ::GdsParser::GdsRecords::AREF:
            m_status = record_type;
            break;
        case ::GdsParser::GdsRecords::ENDEL:
            {
                switch (m_status)
                {
                    case ::GdsParser::GdsRecords::BOX:
                    case ::GdsParser::GdsRecords::BOUNDARY:
                        limboAssert(m_layer != -1);
						m_db.cells().back().addPolygon(m_layer, m_datatype, m_vPoint); 
						break; 
                    case ::GdsParser::GdsRecords::PATH:
                        limboAssert(m_layer != -1);
						m_db.cells().back().addPath(m_layer, m_datatype, m_pathtype, m_width, m_vPoint); 
                        break;
					case ::GdsParser::GdsRecords::TEXT:
                        limboAssert(m_layer != -1 && !m_string.empty());
						m_db.cells().back().addText(m_layer, m_datatype, m_texttype, m_string, m_vPoint.front(), m_width, m_presentation, m_angle, m_magnification, m_strans); 
						break; 
                    case ::GdsParser::GdsRecords::SREF:
						m_db.cells().back().addCellReference(m_sname, m_vPoint.front(), m_angle, m_magnification, m_strans); 
                        break;
                    case ::GdsParser::GdsRecords::AREF:
						m_db.cells().back().addCellArray(m_sname, m_columns, m_rows, m_spacing, m_vPoint, m_angle, m_magnification, m_strans); 
                        break;
                    default: break;
                }
                m_status = ::GdsParser::GdsRecords::BGNSTR; // go back to upper level 
				// reset temporary data 
				reset();
            }
            break;
        case ::GdsParser::GdsRecords::ENDLIB: // notify database on the end of lib 
			m_status = ::GdsParser::GdsRecords::UNKNOWN; // go back to upper level  
            break;
        case ::GdsParser::GdsRecords::ENDSTR: // currently not interested, add stuff here if needed 
			m_status = ::GdsParser::GdsRecords::BGNLIB; // go back to upper level 
            break;
        default: // be careful here, you may dump a lot of unnecessary error message for unknown record_type 
			{
				// only print invalid records or unsupported records for the first time 
				if (record_type >= (int)m_vUnsupportRecord.size() || m_vUnsupportRecord[record_type] == 0)
					limboPrint(limbo::kWARN, "%s() invalid record_type = %s\n", __func__, ::GdsParser::gds_record_ascii(record_type));
				m_vUnsupportRecord[record_type] += 1; 
			}
            break;
    }
}
/// helper functions 
void GdsReader::integer_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<int> const& vData)
{
    switch (record_type)
    {
		case ::GdsParser::GdsRecords::HEADER: 
			limboAssert(m_status == ::GdsParser::GdsRecords::UNKNOWN); 
			m_db.setHeader(vData.front()); 
			break ;
        case ::GdsParser::GdsRecords::LAYER:
            m_layer = vData.front();
            break;
		case ::GdsParser::GdsRecords::DATATYPE:
			m_datatype = vData.front();
			break; 
		case ::GdsParser::GdsRecords::PATHTYPE:
			m_pathtype = vData.front();
			break; 
		case ::GdsParser::GdsRecords::TEXTTYPE:
			m_texttype = vData.front();
			break; 
		case ::GdsParser::GdsRecords::SPACING:
			m_spacing[0] = vData.at(0);
			m_spacing[1] = vData.at(1);
			break; 
		case ::GdsParser::GdsRecords::WIDTH:
			m_width = vData.front();
			break; 
		case ::GdsParser::GdsRecords::STRANS:
			m_strans = vData.front();
			break; 
		case ::GdsParser::GdsRecords::PRESENTATION:
			m_presentation = vData.front();
			break; 
		case ::GdsParser::GdsRecords::COLROW:
			m_columns = vData.at(0);
			m_rows = vData.at(1);
			break; 
        case ::GdsParser::GdsRecords::XY:
			{
				for (unsigned int i = 0, ie = vData.size(); i < ie; i += 2)
					m_vPoint.push_back(gtl::construct<point_type>(vData[i], vData[i+1])); 
			}
			break;
        case ::GdsParser::GdsRecords::BGNLIB: // notify database on the begin of lib 
			m_status = ::GdsParser::GdsRecords::BGNLIB;
            break;
        case ::GdsParser::GdsRecords::BGNSTR: // just date of creation, not interesting
			m_status = ::GdsParser::GdsRecords::BGNSTR; 
            break;
		case ::GdsParser::GdsRecords::BGNEXTN: // appear in PATH with PATHTYPE 4, not really useful though 
		case ::GdsParser::GdsRecords::ENDTEXTN:
        default: // other not interested record_type
			{
				// only print invalid records or unsupported records for the first time 
				if (record_type >= (int)m_vUnsupportRecord.size() || m_vUnsupportRecord[record_type] == 0)
					limboPrint(limbo::kWARN, "%s() invalid record_type = %s, data_type = %s\n", __func__, ::GdsParser::gds_record_ascii(record_type), ::GdsParser::gds_data_ascii(data_type));
				m_vUnsupportRecord[record_type] += 1; 
			}
            break;
    }
}
void GdsReader::float_cbk(::GdsParser::GdsRecords::EnumType record_type, ::GdsParser::GdsData::EnumType data_type, std::vector<double> const& vData)
{
    switch (record_type)
    {
        case ::GdsParser::GdsRecords::UNITS:
			m_db.setUnit(vData.at(0));
			m_db.setPrecision(vData.at(1));
            break;
		case ::GdsParser::GdsRecords::ANGLE:
			m_angle = vData.front();
			break; 
		case ::GdsParser::GdsRecords::MAG:
			m_magnification = vData.front();
			break; 
        default:
			{
				// only print invalid records or unsupported records for the first time 
				if (record_type >= (int)m_vUnsupportRecord.size() || m_vUnsupportRecord[record_type] == 0)
					limboPrint(limbo::kWARN, "%s() invalid record_type = %s, data_type = %s\n", __func__, ::GdsParser::gds_record_ascii(record_type), ::GdsParser::gds_data_ascii(data_type));
				m_vUnsupportRecord[record_type] += 1; 
			}
            break;
    }
}

void GdsWriter::operator() (std::string const& filename) const 
{
    ::GdsParser::GdsWriter gw (filename.c_str());
    gw.create_lib(m_db.libname().c_str(), m_db.unit(), m_db.precision());

	for (std::vector<GdsCell>::const_iterator it = m_db.cells().begin(), ite = m_db.cells().end(); it != ite; ++it)
		write(gw, *it);

    gw.gds_write_endlib(); 
}

void GdsWriter::write(::GdsParser::GdsWriter& gw, GdsCell const& cell) const 
{
    gw.gds_write_bgnstr();
    gw.gds_write_strname(cell.name().c_str());

	for (std::vector<std::pair< ::GdsParser::GdsRecords::EnumType, GdsObject*> >::const_iterator it = cell.objects().begin(), ite = cell.objects().end(); it != ite; ++it)
	{
		GdsObjectHelpers()(it->first, it->second, WriteCellObjectAction<GdsWriter>(*this, gw, *it));
	}

    gw.gds_write_endstr();
}

void GdsWriter::write(::GdsParser::GdsWriter& gw, GdsPolygon const& object) const
{
#ifdef DEBUG
	limboAssert(object.datatype() < 255); 
#endif
	std::vector<int> vx(object.size());
	std::vector<int> vy(object.size());

	std::size_t count = 0; 
	for (GdsPolygon::iterator_type it = object.begin(), ite = object.end(); it != ite; ++it, ++count)
	{
		vx[count] = it->x();
		vy[count] = it->y();
	}

	gw.write_boundary(object.layer(), object.datatype(), vx, vy, true);
}

void GdsWriter::write(::GdsParser::GdsWriter& gw, GdsPath const& object) const 
{
#ifdef DEBUG
	limboAssert(object.datatype() < 255); 
#endif
    gw.gds_write_path();
	if (object.layer() != std::numeric_limits<int>::max())
		gw.gds_write_layer(object.layer());
	if (object.datatype() != std::numeric_limits<int>::max())
		gw.gds_write_datatype(object.datatype());
	if (object.pathtype() != std::numeric_limits<int>::max())
		gw.gds_write_pathtype(object.pathtype());
	if (object.width() != std::numeric_limits<int>::max())
		gw.gds_write_width(object.width());

	std::vector<int> vx(object.size());
	std::vector<int> vy(object.size());

	std::size_t count = 0; 
	for (GdsPath::const_iterator it = object.begin(); it != object.end(); ++it, ++count)
	{
		vx[count] = it->x();
		vy[count] = it->y();
	}

    gw.gds_write_xy(vx.data(), vy.data(), count);
    gw.gds_write_endel();

#ifdef DEBUG
    // write out equivalent polygon again 
    write(gw, object.toPolygon());
#endif
}

void GdsWriter::write(::GdsParser::GdsWriter& gw, GdsText const& object) const 
{
	// I do not have WIDTH attribute 
	// the api looks different, need an example of input  
    gw.gds_write_text();
	if (object.layer() != std::numeric_limits<int>::max())
		gw.gds_write_layer(object.layer());
	if (object.datatype() != std::numeric_limits<int>::max())
		gw.gds_write_datatype(object.datatype());
	if (object.texttype() != std::numeric_limits<int>::max())
		gw.gds_write_texttype(object.texttype());
	if (object.width() != std::numeric_limits<int>::max())
		gw.gds_write_width(object.width()); 
	if (object.presentation() != std::numeric_limits<int>::max())
		gw.gds_write_presentation(object.presentation()/16, (object.presentation()%16)/4, (object.presentation()%16)%4); // (font, vp, hp) presentation = hp + 4*vp + 16*font
	if (object.strans() != std::numeric_limits<int>::max())
		gw.gds_write_strans(object.strans()/32768, (object.strans()%32768)%2, (object.strans()%32768)/2); // (reflect, abs_angle, abs_mag) strans = 32768*reflect + 2*abs_mag + abs_angle
	if (object.magnification() != std::numeric_limits<double>::max())
		gw.gds_write_mag(object.magnification());             

	std::vector<GdsText::coordinate_type> vx (1, object.position().x());
	std::vector<GdsText::coordinate_type> vy (1, object.position().y());
    gw.gds_write_xy(vx.data(), vy.data(), 1);  
    gw.gds_write_string(object.text().c_str());
    gw.gds_write_endel();
}

void GdsWriter::write(::GdsParser::GdsWriter& gw, GdsCellReference const& object) const 
{
    gw.gds_write_sref();                    // contains an instance of...
    gw.gds_write_sname(object.refCell().c_str());        // the cell hotdogs
	if (object.magnification() != std::numeric_limits<double>::max())
		gw.gds_write_mag(object.magnification());                // which will be 5 times larger
	if (object.strans() != std::numeric_limits<int>::max())
		gw.gds_write_strans(object.strans()/32768, (object.strans()%32768)%2, (object.strans()%32768)/2); // (reflect, abs_angle, abs_mag) strans = 32768*reflect + 2*abs_mag + abs_angle
	if (object.angle() != std::numeric_limits<double>::max())
		gw.gds_write_angle(object.angle());             // and tilted at some weird angle
	if (object.magnification() != std::numeric_limits<double>::max())
		gw.gds_write_mag(object.magnification());             

    int x[1] = {object.position().x()};  
    int y[1] = {object.position().y()};  
    gw.gds_write_xy(x, y, 1);             // at these coordinates (database units)
    gw.gds_write_endel();                   // end of element
}

void GdsWriter::write(::GdsParser::GdsWriter& gw, GdsCellArray const& object) const 
{
	// I do not know how to write SPACING in AREF 
	
    gw.gds_write_aref();                    // containing an array of...
    gw.gds_write_sname(object.refCell().c_str());        // the cell hotdogs
    gw.gds_write_colrow(object.columns(), object.rows() );            // 2 columns, 5 rows
	if (object.strans() != std::numeric_limits<int>::max())
		gw.gds_write_strans(object.strans()/32768, (object.strans()%32768)%2, (object.strans()%32768)/2); // (reflect, abs_angle, abs_mag) strans = 32768*reflect + 2*abs_mag + abs_angle
	if (object.angle() != std::numeric_limits<double>::max())
		gw.gds_write_angle(object.angle());             // and tilted at some weird angle
	if (object.magnification() != std::numeric_limits<double>::max())
		gw.gds_write_mag(object.magnification());             

    // array anchor point
	// suppose colrow is (2, 5)
    // displacement from anchor plus ncols*pitch
    // which makes the column pitch 40 um
    // displacement from anchor plus nrows*pitch
    // which makes the row pitch 40 um
	std::vector<int> vx(object.positions().size());
	std::vector<int> vy(object.positions().size());

	std::size_t count = 0; 
	for (std::vector<GdsCellArray::point_type>::const_iterator it = object.positions().begin(), ite = object.positions().end(); it != ite; ++it, ++count)
	{
		vx[count] = it->x();
		vy[count] = it->y();
	}
    gw.gds_write_xy(vx.data(), vy.data(), count);             // See how you could have a diagonal matrix? 
                                             // That would be so goofy!
    gw.gds_write_endel();                   // end of element
}

}} // namespace GdsParser // GdsDB

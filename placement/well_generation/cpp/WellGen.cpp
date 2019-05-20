/*************************************************************************
    > File Name: WellGen.cpp
    > Author: Biying Xu
    > Mail: biying@utexas.edu
    > Created Time: Tue 06 Nov 2018 09:37:05 PM CST
 ************************************************************************/

#include "WellGen.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <typeinfo>

#define M_DEBUG(x) /* x */

using std::ifstream;
using std::ofstream;

void WellGen::parse_gds_file()
//{{{
{
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
	GdsParser::GdsDB::GdsDB db; 
    GdsParser::GdsDB::GdsReader reader (db); 
    reader(gds_file_name);

	// for hierarchical GDS
	GdsCell flatCell = db.extractCell(cell_name);
    M_DEBUG(
    cout<<"cell: "<<flatCell.name()<<endl; 
    );
	// NOTE: assume flat GDSII
	for(vector<GdsCell::object_entry_type>::const_iterator oit = flatCell.objects().begin(); oit != flatCell.objects().end(); ++oit) {
		if(oit->first == ::GdsParser::GdsRecords::BOUNDARY) {
			GdsPolygon* ptr = dynamic_cast<GdsPolygon*>(oit->second); 
			int lay = ptr->layer();
			// cout<<"datatype: "<<ptr->datatype()<<endl;
			// 17: PO layer; 30: CO layer; 31-44: M1-M14 layers; 51-63: VIA1-VIA13 layers; 75: PDK layer
			if(lay == 17 || (lay >= 30 && lay <= 44) || (lay >= 51 && lay <= 63) ||  lay == 75) continue;
			polygon_90_t p;
			gtl::set_points(p, ptr->begin(), ptr->end());
			polygons[lay].push_back(p);
		} // if BOUNDARY
	} // for objects

    M_DEBUG(
	for(auto it=polygons.begin(); it!=polygons.end(); ++it) {
		cout<<"Layer: "<<it->first<<", num polygons: "<<it->second.size()<<endl;
	}
    );
	
	// The GDSII file for display on Virtuoso
	GdsParser::GdsDB::GdsDB new_db(db); 
	new_db.cells().clear();
	GdsCell new_cell;
	new_cell.setName(cell_name);
	for(auto it=polygons.begin(); it!=polygons.end(); ++it) {
		// polygon_sets[it->first] = polygon_90_set_t();
		polygon_sets[it->first].insert(it->second.begin(), it->second.end());
		// add to GdsCell
		for(vector< polygon_90_t >::const_iterator pit=it->second.begin(); pit!=it->second.end(); ++pit) {
			vector< point_type > points;
			points.insert(points.begin(), pit->begin(), pit->end());
			points.push_back(points[0]); // add the last point, same as the first point
			new_cell.addPolygon(it->first, 0, points);
		}
	}
	new_db.addCell(new_cell);
    GdsParser::GdsDB::GdsWriter gw(new_db);
	gw(out_gds_file_name);
	
	// The GDSII file for training
	GdsParser::GdsDB::GdsDB train_db(db); 
	train_db.cells().clear();
	GdsCell train_cell;
	train_cell.setName(cell_name);
	process_polygons(train_cell); // only keep the NW and OD layer, offset the polygons and mark datatype
	train_db.addCell(train_cell);
    GdsParser::GdsDB::GdsWriter train_gw(train_db);
	train_gw(train_gds_file_name);
}
//}}}

void WellGen::process_polygons(GdsCell& cell)
//{{{
{
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
	// currently do not differentiate PMOS and other devices with NWELL
	polygon_90_set_t pset_nw = polygon_sets[3];
	polygon_90_set_t pset_od = polygon_sets[6];
	
	// find the offset
	int xmin = std::numeric_limits<int>::max(), ymin = std::numeric_limits<int>::max();
	// xmax = std::numeric_limits<int>::min(), ymax = std::numeric_limits<int>::min();
	for(vector< polygon_90_t >::const_iterator pit=polygons[3].begin(); pit!=polygons[3].end(); ++pit) {
		for(polygon_90_t::iterator_type vit=pit->begin(); vit!=pit->end(); ++vit) {
			xmin = std::min(xmin, gtl::x(*vit));
			ymin = std::min(ymin, gtl::y(*vit));
		}
	}
	for(vector< polygon_90_t >::const_iterator pit=polygons[6].begin(); pit!=polygons[6].end(); ++pit) {
		for(auto vit=pit->begin(); vit!=pit->end(); ++vit) {
			xmin = std::min(xmin, gtl::x(*vit));
			ymin = std::min(ymin, gtl::y(*vit));
		}
	}
	
	// rectangle_t bbox;
	// typedef bg::model::d2::point_xy<int> point;
	// bg::model::box<point_t> bbox;
	// bg::envelope(pset_nw, bbox);

	// move the polygons
	gtl::move(pset_nw, -xmin, -ymin);
	gtl::move(pset_od, -xmin, -ymin);

	// write_gds_to_image_offset_to_file xmin, ymin
	string gds2image_offset_fname = cell_name + ".gds2image.offset";
	ofstream offset_out_file;
	offset_out_file.open(gds2image_offset_fname);
	offset_out_file << xmin;
	offset_out_file << " ";
	offset_out_file << ymin;
	offset_out_file << endl;
	offset_out_file.close();
	
	vector< polygon_90_t > polygons_nw;
	vector< polygon_90_t > polygons_od;
	pset_nw.get_polygons(polygons_nw);
	pset_od.get_polygons(polygons_od);

	vector< rectangle_t > rectangles_od;
	pset_od.get_rectangles(rectangles_od);
	vector<bool> covered_by_nw( rectangles_od.size(), false );
	bgi::rtree< rtree_value, bgi::quadratic<16> > rtree_od;
	for(size_t i=0; i<rectangles_od.size(); ++i) {
		rtree_od.insert(std::make_pair(rectangles_od[i], i));
	}
	for(polygon_90_t p : polygons_nw) {
		int xl = std::numeric_limits<int>::max(), yl = std::numeric_limits<int>::max(), xh = std::numeric_limits<int>::min(), yh = std::numeric_limits<int>::min();
		for(polygon_90_t::iterator_type vit=p.begin(); vit!=p.end(); ++vit) {
			xl = std::min(xl, gtl::x(*vit));
			yl = std::min(yl, gtl::y(*vit));
			xh = std::max(xh, gtl::x(*vit));
			yh = std::max(yh, gtl::y(*vit));
		}
		rectangle_t rect(xl, yl, xh, yh);
		vector< rtree_value > result_s;
		rtree_od.query(bgi::covered_by(rect) && bgi::satisfies(mCoveredBy(p)), std::back_inserter(result_s));
		for(rtree_value r : result_s) {
			covered_by_nw[r.second] = true;
		}
	}

	for(size_t i=0; i<rectangles_od.size(); ++i) {
		int xl = gtl::xl(rectangles_od[i]), yl = gtl::yl(rectangles_od[i]), xh = gtl::xh(rectangles_od[i]), yh = gtl::yh(rectangles_od[i]);
		// clockwise
		vector< point_t> pts{point_t(xl, yl), point_t(xl, yh), point_t(xh, yh), point_t(xh, yl), point_t(xl, yl)};
		// vector< point_type > points;
		// points.insert(points.begin(), polygons_od[i].begin(), polygons_od[i].end());
		// points.push_back(points[0]); // add the last point, same as the first point
		if( covered_by_nw[i] ) {
			cell.addPolygon(6, 1, pts); // OD layer
		} else {
			cell.addPolygon(6, 0, pts); // OD layer
		}
	}
	
	for(polygon_90_t p : polygons_nw) {
		vector< point_type > points;
		points.insert(points.begin(), p.begin(), p.end());
		points.push_back(points[0]); // add the last point, same as the first point
		cell.addPolygon(3, 0, points); // OD layer
	}
	
}
//}}}

int main(int argc, char** argv) {
	WellGen* wg = new WellGen();
	wg->cell_name = argv[1]; 
	wg->gds_file_name = argv[2]; 
	wg->out_gds_file_name = argv[3];
	wg->train_gds_file_name = argv[4];

    std::cout << "cell_name = " << wg->cell_name << "\n";
    std::cout << "gds_file_name = " << wg->gds_file_name << "\n";
    std::cout << "out_gds_file_name = " << wg->out_gds_file_name << "\n";
    std::cout << "train_gds_file_name = " << wg->train_gds_file_name << "\n";

	wg->parse_gds_file();
	delete wg;
}

/*************************************************************************
    > File Name: Legalize.cpp
    > Author: anoynmous anoynmous
    > Mail: anoynmous@anoynmous
    > Created Time: Thu 13 Dec 2018 05:53:06 PM CST
 ************************************************************************/

#include <iostream>
#include <numeric> // std::iota
#include <typeinfo> // typeid
#ifdef USE_GUROBI
#include <limbo/solvers/api/GurobiApi.h>
#endif
#include <limbo/solvers/api/LPSolveApi.h>
#include <limbo/preprocessor/AssertMsg.h>
#include <limbo/programoptions/ProgramOptions.h>
#include <limbo/string/String.h>
#include "Legalize.h"
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::set;

#define M_DEBUG(x) /* x */

std::ostream& operator<< (std::ostream &out, dEdge const& e) {
	if(e.is_horizontal) { out<<"HORIZONTAL\t"; }
	else { out<<"VERTICAL\t"; }
	out<<"(";
	out<<e.src;
	out<<", ";
	out<<e.tgt;
	out<<")\tvalue: ";
	out<<e.value;
    return out;
}

void Legalize::read_contour_file(string const& file_name, vector< polygon_90_t > & contours)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ifstream in_file;
	in_file.open(file_name);

	string anoynmouse;
	while(std::getanoynmouse(in_file, anoynmouse)) {
		vector< point_t > pts;
		std::istringstream iss(anoynmouse);
		int x, y;
		while(iss >> x >> y) {
			// scale and offset
			// pts.push_back( point_t(x*scale, y*scale) );
			pts.push_back( point_t(x*scale+contour_offset.x(), y*scale+contour_offset.y()) );
		}
		polygon_90_t p;
		p.set(pts.begin(), pts.end());
		contours.push_back(p);
	}
	in_file.close();
}
//}}}

void Legalize::read_contour_offset_file(string const& file_name)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ifstream in_file;
	in_file.open(file_name);

	string anoynmouse;
	std::getanoynmouse(in_file, anoynmouse);
	std::istringstream iss(anoynmouse);
	int x, y;
	iss >> x >> y;
	// FIXME: hardcode
	contour_offset = Loc<data_type>(x, y);
	// if(ckt_name == "COMPARATOR_PRE_AMP_test_flow") {
	// 	contour_offset = Loc<data_type>(0, 0);
	// } else {
	// 	contour_offset = Loc<data_type>(x, y);
	// }
	in_file.close();
}
//}}}

int Legalize::get_grid_value(int orig_val, int pt_val, int val_a, int val_b, rtree_type const& rtree_inside, bool is_horizontal, bool incr)
//{{{
{
	double grid_loc = (double)(pt_val-orig_val)/(double)grid_size;
	int grid_val = orig_val + grid_size * lround(grid_loc); 
    M_DEBUG(
	cout<<"grid_val: "<<grid_val<<endl;
    );
	if(!check_value_legal(grid_val, val_a, val_b, rtree_inside, is_horizontal)) {
		if(incr) { grid_val += grid_size; }
		else { grid_val -= grid_size; }
        M_DEBUG(
		cout<<"grid_val not valid, change to: "<<grid_val<<endl;
        );
	} 
	return grid_val;
}
//}}}

/*
void Legalize::align_to_grid(polygon_90_t const& contour, point_t const& point, vector< polygon_90_t >& aligned_contours)
//{{{
{
	int orig_x = point.x();
	int orig_y = point.y();
	vector< point_t > pts;
	// int grid_size = 8; // FIXME: set according to the minimum width value
	for(auto it=contour.begin(); it!=contour.end(); ++it) {
		int new_x = get_grid_value(orig_x, gtl::x(*it)); 
		int new_y = get_grid_value(orig_y, gtl::y(*it)); 
		pts.push_back(point_t(new_x, new_y));
	}

	polygon_90_t ply;
	ply.set(pts.begin(), pts.end());
	polygon_90_set_t pset;
	pset.insert(ply);
	vector< polygon_90_t > pvec;
	pset.get_polygons(pvec);

	aligned_contours.insert(aligned_contours.end(), pvec.begin(), pvec.end());

}
//}}}
*/

/*
int Legalize::get_new_value(int orig_val, int val, int val_a, int val_b, bool incr, rtree_type const& rtree_inside, bool is_horizontal) 
//{{{
{
	int new_val = get_grid_value(orig_val, val); 
	vector< rtree_value > result_s;
	point_t point_a, point_b;
	if(is_horizontal) {
		point_a = point_t(val_a, new_val); 
		point_b = point_t(val_b, new_val); 
	} else {
		point_a = point_t(new_val, val_a);
		point_b = point_t(new_val, val_b);
	}
	segment_t seg(point_a, point_b);
	rtree_inside.query(bgi::satisfies(mSegmentIntersect(seg)), std::back_inserter(result_s));
	while(!result_s.empty()) {
		cout<<"increment new_val"<<endl;
		result_s.clear();
		if(incr) { new_val += grid_size; }
		else { new_val -= grid_size; }
	    if(is_horizontal) {
	    	point_a = point_t(val_a, new_val); 
	    	point_b = point_t(val_b, new_val); 
	    } else {
	    	point_a = point_t(new_val, val_a);
	    	point_b = point_t(new_val, val_b);
	    }
		segment_t seg_tmp(point_a, point_b);
		rtree_inside.query(bgi::satisfies(mSegmentIntersect(seg_tmp)), std::back_inserter(result_s));
	}
	return new_val;
}
//}}}
*/

bool Legalize::check_value_legal(int new_val, int val_a, int val_b, rtree_type const& rtree_inside, bool is_horizontal) 
//{{{
{
	// FIXME: this approach is heuristics, need to improve
	vector< rtree_value > result_s;
	point_t point_a, point_b;
	if(is_horizontal) {
		point_a = point_t(std::min(val_a, val_b), new_val); 
		point_b = point_t(std::max(val_a, val_b), new_val); 
	} else {
		point_a = point_t(new_val, std::min(val_a, val_b));
		point_b = point_t(new_val, std::max(val_a, val_b));
	}
	segment_t seg(point_a, point_b);
	rtree_inside.query(bgi::satisfies(mSegmentIntersect(seg)), std::back_inserter(result_s));
	return result_s.empty();
}
//}}}

void Legalize::write_contour_to_file_debug(vector< point_t > const& pts, string const& fname)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	/*
	polygon_90_t ply;
	ply.set(pts.begin(), pts.end());
	polygon_90_set_t pset;
	pset.insert(ply);
	vector< polygon_90_t > contours;
	pset.get_polygons(contours);
	*/
	ofstream out_file;
	out_file.open(fname);
	for(auto it=pts.begin(); it!=pts.end(); ++it) {
		out_file << gtl::x(*it);
		out_file << " ";
		out_file << gtl::y(*it);
		out_file << " ";
		// out_file << bg::dsv(*it);
	}
	out_file << endl;
	out_file.close();
}
//}}}

bool Legalize::check_devices_in_well(polygon_90_t const& contour, set<size_t> const& dset)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	using namespace boost::polygon::operators;

	bool contain_flag = true;
	for(size_t i : dset) {
		Device<data_type> d = devices[i];
		rectangle_t rect(d.loc.x(), d.loc.y(), d.loc.x()+d.size.w(), d.loc.y()+d.size.h());
		// NOTE: since adapted polygon is not working, I just use heuristics to decide coverage
	    if( !(gtl::contains(contour, gtl::ll(rect)) && gtl::contains(contour, gtl::lr(rect)) && 
			gtl::contains(contour, gtl::ul(rect)) && gtl::contains(contour, gtl::ur(rect)) )) {
	    	contain_flag = false;
            M_DEBUG(
			cout<<"device "<<i<<" not contained in well!"<<endl;
            );
	    }
	}

	return contain_flag;
}
//}}}

void Legalize::align_to_grid_legalize(polygon_90_t const& contour, point_t const& point, Loc<data_type> const& wcontact_loc, set<size_t> const& dset, polygon_90_t& aligned_contour, vector< polygon_90_t >& aligned_contours)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	aligned_contour = contour;
	subtract_and_union(aligned_contour, dset);
	// union with well contact
	using namespace boost::polygon::operators;
	rectangle_t rect_wcontact(wcontact_loc.x(), wcontact_loc.y(), wcontact_loc.x()+well_contact_size.w(), wcontact_loc.y()+well_contact_size.h());
	polygon_90_set_t pset_tmp = aligned_contour + rect_wcontact;
	vector< polygon_90_t > pvec_tmp;
	pset_tmp.get_polygons(pvec_tmp);
    M_DEBUG(
	cout<<"pvec_tmp.size(): "<<pvec_tmp.size()<<endl;
	for(polygon_90_t p : pvec_tmp) {
		size_t cnt_debug = 0;
		cout<<"polygon: ";
		for(auto it=p.begin(); it!=p.end(); ++it) {
			cout<<cnt_debug<<":"<<bg::dsv(*it)<<", ";
			cnt_debug++;
		}
		cout<<endl;
	}
    );

	// assert(("well shape incorrect!", pvec_tmp.size() == 1));
	assert(pvec_tmp.size() == 1);
	aligned_contour = pvec_tmp[0];
	// aligned_contours.insert(aligned_contours.end(), pvec_tmp.begin(), pvec_tmp.end());
	// return;
	
	bool check = check_devices_in_well(aligned_contour, dset);
    M_DEBUG(
	cout<<"check_devices_in_well: "<<check<<endl;
    );
    // assert(("assign devices to well incorrect!", check));
    assert(check);

	int orig_x = point.x();
	int orig_y = point.y();
	vector< point_t > pts(aligned_contour.begin(), aligned_contour.end());
	
	bool winding = 0; // 0: clockwise; 1: counter clockwise
	// if(gtl::winding(aligned_contour) == gtl::HIGH) { winding = 1; }
	// (gtl::unknown_winding) { cout<<"unknown_winding!"<<endl; }
	if(gtl::winding(aligned_contour) == gtl::CLOCKWISE) { 
		winding = 0; 
        M_DEBUG(
		cout<<"CLOCKWISE"<<endl;
        );
	} 
	if(gtl::winding(aligned_contour) == gtl::COUNTERCLOCKWISE) { 
		winding = 1; 
        M_DEBUG(
		cout<<"COUNTERCLOCKWISE"<<endl;
        );
	}

	rtree_type rtree_inside;
	size_t cnt = 0;
	for(size_t d : dset) {
		data_type xl = devices[d].loc.x();
		data_type yl = devices[d].loc.y();
		data_type xh = xl + devices[d].size.w();
		data_type yh = yl + devices[d].size.h();
		rtree_inside.insert(std::make_pair(rectangle_t(xl, yl, xh, yh), cnt));
		cnt++;
	}
	rtree_inside.insert(std::make_pair(rect_wcontact, cnt));

	// vector< int > edges;
	size_t num = pts.size();
	for(size_t i=0; i<num-1; ++i) {
        M_DEBUG(
		cout<<"point i: "<<i<<endl;
        );
	    bool incr = 0;
		if(pts[i].x() == pts[i+1].x()) {
			// VERTICAL
	    	if(winding == (pts[i].y() < pts[i+1].y())) { incr = 1; }
			int	new_x = get_grid_value(orig_x, pts[i].x(), pts[i].y(), pts[i+1].y(), rtree_inside, 0, incr);
			pts[i].set(gtl::HORIZONTAL, new_x);
			pts[i+1].set(gtl::HORIZONTAL, new_x);
            M_DEBUG(
			cout<<"new_x: "<<new_x<<endl;
            );
		} else {
			// HORIZONTAL
	    	if(winding == (pts[i].x() > pts[i+1].x())) { incr = 1; }
			int	new_y = get_grid_value(orig_y, pts[i].y(), pts[i].x(), pts[i+1].x(), rtree_inside, 1, incr);
			pts[i].set(gtl::VERTICAL, new_y);
			pts[i+1].set(gtl::VERTICAL, new_y);
            M_DEBUG(
			cout<<"new_y: "<<new_y<<endl;
            );
		}
	}
	// the last point
	// FIXME: simplify the code
    M_DEBUG(
	cout<<"last point"<<endl;
    );
	if(pts[num-1].x() == pts[0].x()) {
		// VERTICAL
	    bool incr = 0;
	    if(winding == (pts[num-1].y() < pts[0].y())) { incr = 1; }
		int	new_x = get_grid_value(orig_x, pts[num-1].x(), pts[num-1].y(), pts[0].y(), rtree_inside, 0, incr);
		pts[num-1].set(gtl::HORIZONTAL, new_x);
		pts[0].set(gtl::HORIZONTAL, new_x);
        M_DEBUG(
		cout<<"new_x: "<<new_x<<endl;
        );
	} else {
		// HORIZONTAL
	    bool incr = 0;
	    if(winding == (pts[num-1].x() > pts[0].x())) { incr = 1; }
		int	new_y = get_grid_value(orig_y, pts[num-1].y(), pts[num-1].x(), pts[0].x(), rtree_inside, 1, incr);
		pts[num-1].set(gtl::VERTICAL, new_y);
		pts[0].set(gtl::VERTICAL, new_y);
        M_DEBUG(
		cout<<"new_y: "<<new_y<<endl;
        );
	}

	// remove same points
	vector<int> pts_to_delete;
	for(int i=(int)num-1; i>0; --i) {
		if(pts[i]==pts[i-1]) {
            M_DEBUG(
			cout<<"remove same point"<<endl;
            );
			pts_to_delete.push_back(i);
		}
	}
	for(int pts_id : pts_to_delete) {
		pts.erase(pts.begin()+pts_id);
	}

	// merge edge
	vector< point_t > pts_merged;
	//bool dir;
	// assert(pts.size()>=2);
	// if(gtl::x(pts.at(0)) == gtl::x(pts.at(1))) dir = 0; // vertical
	// else dir = 1;
	for(size_t i=0; i<pts.size(); ++i) {
		if(pts_merged.size() >= 2) {
			bool same_x1 = gtl::x(pts_merged[pts_merged.size()-2]) == gtl::x(pts_merged[pts_merged.size()-1]);
			bool same_y1 = gtl::y(pts_merged[pts_merged.size()-2]) == gtl::y(pts_merged[pts_merged.size()-1]);
			bool same_x2 = gtl::x(pts_merged[pts_merged.size()-1]) == gtl::x(pts[i]);
			bool same_y2 = gtl::y(pts_merged[pts_merged.size()-1]) == gtl::y(pts[i]);
			if(same_x1 && same_x2) {
				pts_merged[pts_merged.size()-1].y( gtl::y(pts[i]) );
			} else if(same_y1 && same_y2) {
				pts_merged[pts_merged.size()-1].x( gtl::x(pts[i]) );
			} else {
				pts_merged.push_back( pts[i] );
			}
		} else {
			pts_merged.push_back( pts[i] );
		}
	}
	pts = pts_merged;

	// write_contour_to_file_debug(pts, ckt_name+".contours.debug."+std::to_string(num));

	polygon_90_t ply;
	ply.set(pts.begin(), pts.end());
	polygon_90_set_t pset;
	pset.insert(ply);
	vector< polygon_90_t > pvec;
	pset.get_polygons(pvec);
    M_DEBUG(
	cout<<"pvec.size(): "<<pvec.size()<<endl;
	for(polygon_90_t p : pvec) {
		cout<<"polygon: ";
		for(auto it=p.begin(); it!=p.end(); ++it) {
			cout<<bg::dsv(*it)<<", ";
		}
		cout<<endl;
	}
    );

	// FIXME: DEBUG
	if(pvec.empty()) {
		vector< rectangle_t > rvec;
		pset.get_rectangles(rvec);
		polygon_90_set_t pset_new;
		for(rectangle_t r : rvec) {
			pset_new.insert(r);
		}
		vector< polygon_90_t > pvec_new;
		pset_new.get_polygons(pvec_new);
        M_DEBUG(
		cout<<"pvec_new.size(): "<<pvec_new.size()<<endl;
		for(polygon_90_t p : pvec_new) {
			cout<<"polygon: ";
			for(auto it=p.begin(); it!=p.end(); ++it) {
				cout<<bg::dsv(*it)<<", ";
			}
			cout<<endl;
		}
        );
		aligned_contours.insert(aligned_contours.end(), pvec_new.begin(), pvec_new.end());
		// aligned_contours.push_back(ply);
	} else if (pvec.size() == 1){
		// aligned_contours.insert(aligned_contours.end(), pvec.begin(), pvec.end());
		aligned_contours.push_back(pvec[0]);
	} else {
		aligned_contours.push_back(ply);
	}

}
//}}}

void Legalize::parse_gds_file(size_t index, string file_name)
//{{{
{
    // cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    vector< std::pair<int, string> > feolLayer;
	std::map<int, vector< rectangle_t > > rectangles;
	
    GdsParser::GdsDB::GdsDB db; 
    GdsParser::GdsDB::GdsReader reader (db); 
    reader(file_name);

	// NOTE: fix the issue of small offset in GDS file
	int cell_xl=std::numeric_limits<int>::max(), cell_yl=std::numeric_limits<int>::max();
	string block_name = devices[index].name;
    for (vector<GdsCell>::const_iterator it = db.cells().begin(); it != db.cells().end(); ++it) {
		M_DEBUG(
        cout<<"cell: "<<it->name()<<endl; 
		);
		devices[index].gds_cell = new GdsCell(*it);
		// NOTE: assume flat GDSII
		for(vector<GdsCell::object_entry_type>::const_iterator oit = it->objects().begin(); oit != it->objects().end(); ++oit) {
			if(oit->first == ::GdsParser::GdsRecords::BOUNDARY) {
				GdsPolygon* ptr = dynamic_cast<GdsPolygon*>(oit->second); 
				int lay = ptr->layer();
				// 17: PO layer; 30: CO layer; 31-44: M1-M14 layers; 51-63: VIA1-VIA13 layers; 75: PDK layer; 208: LVSDMY
				if(lay == 75 || lay == 208) continue;
				polygon_type::iterator_type pit;
				// find the greatest and smallest point value
				int xl=std::numeric_limits<int>::max(), yl=std::numeric_limits<int>::max(), xh=std::numeric_limits<int>::min(), yh=std::numeric_limits<int>::min();
				for(pit=ptr->begin(); pit!=ptr->end(); ++pit) {
			        xl = std::min(xl, pit->x());
			        yl = std::min(yl, pit->y());
			        xh = std::max(xh, pit->x());
			        yh = std::max(yh, pit->y());
				}

				// offset, scale 
				int offset_x = devices[index].offset.x();
				int offset_y = devices[index].offset.y();
				M_DEBUG(
				cout<<"xl: "<<xl<<", yl: "<<yl<<", xh: "<<xh<<", yh: "<<yh<<", offset_x: "<<offset_x<<", offset_y: "<<offset_y<<endl;
				);
				rectangle_t rect(xl-offset_x, yl-offset_y, xh-offset_x, yh-offset_y);
				if(rectangles.find(lay) == rectangles.end()) {
					rectangles[lay] = vector< rectangle_t >();
				}
				rectangles[lay].push_back(rect);
			    cell_xl = std::min(cell_xl, xl-offset_x);
			    cell_yl = std::min(cell_yl, yl-offset_y);
			} // if BOUNDARY
		} // for objects
	} // for cells

	M_DEBUG(
	cout<<"block: "<<block_name<<endl;
	cout<<"cell_xl: "<<cell_xl<<", cell_yl: "<<cell_yl<<endl;
	);
	// merge same layer
	for(auto it=rectangles.begin(); it!=rectangles.end(); ++it) {
		int xl=std::numeric_limits<int>::max(), yl=std::numeric_limits<int>::max(), xh=std::numeric_limits<int>::min(), yh=std::numeric_limits<int>::min();
		for(auto rect : it->second) {
			xl = std::min(xl, gtl::xl(rect));
			yl = std::min(yl, gtl::yl(rect));
			xh = std::max(xh, gtl::xh(rect));
			yh = std::max(yh, gtl::yh(rect));
		}
		int layer = it->first;
		// NOTE: fix the issue of small offset in GDS file
		// rectangle_t rect(xl, yl, xh, yh);
		rectangle_t rect(xl-cell_xl, yl-cell_yl, xh-cell_xl, yh-cell_yl);
		devices[index].layers_shapes.push_back( LayerShape(layer, rect) );
		// FIXME: currently it only supports N well
		if(layer == 3) {
            M_DEBUG(
			cout<<"device with NW layer"<<endl;
            );
			devices[index].type = 1;
		}
		M_DEBUG(
		cout<<"layer "<<it->first<<": ("<<xl-cell_xl<<", "<<yl-cell_yl<<", "<<xh-cell_xl<<", "<<yh-cell_yl<<")"<<endl;
		);
	}
}
//}}}

// NOTE: init devices before calanoynmousg this function
void Legalize::parse_gds_files()
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	for(size_t i=0; i<devices.size(); ++i) {
		parse_gds_file(i, in_gds_file_prefix+devices[i].name+".gds");
	}
}
//}}}

void Legalize::parse_well_contact_gds_file(string const& file_name)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
    GdsParser::GdsDB::GdsDB db; 
    GdsParser::GdsDB::GdsReader reader (db); 
    reader(file_name);

    for (vector<GdsCell>::const_iterator it = db.cells().begin(); it != db.cells().end(); ++it) {
		M_DEBUG(
        cout<<"cell: "<<it->name()<<endl; 
		);
		wcon_gds_cell = new GdsCell(*it);
		// NOTE: assume flat GDSII
		int cell_xl=std::numeric_limits<int>::max(), cell_yl=std::numeric_limits<int>::max(), cell_xh=std::numeric_limits<int>::min(), cell_yh=std::numeric_limits<int>::min();
		for(vector<GdsCell::object_entry_type>::const_iterator oit = it->objects().begin(); oit != it->objects().end(); ++oit) {
			if(oit->first == ::GdsParser::GdsRecords::BOUNDARY) {
				GdsPolygon* ptr = dynamic_cast<GdsPolygon*>(oit->second); 
				int lay = ptr->layer();
				// 17: PO layer; 30: CO layer; 31-44: M1-M14 layers; 51-63: VIA1-VIA13 layers; 75: PDK layer; 208: LVSDMY
				if(lay == 75 || lay == 208) continue;
				polygon_type::iterator_type pit;
				// find the greatest and smallest point value
				int xl=std::numeric_limits<int>::max(), yl=std::numeric_limits<int>::max(), xh=std::numeric_limits<int>::min(), yh=std::numeric_limits<int>::min();
				for(pit=ptr->begin(); pit!=ptr->end(); ++pit) {
			        xl = std::min(xl, pit->x());
			        yl = std::min(yl, pit->y());
			        xh = std::max(xh, pit->x());
			        yh = std::max(yh, pit->y());
				}
				if(lay == 31) {
					// pin on M1 layer
					well_contact_pin_rect = rectangle_t(xl, yl, xh, yh);
				}
			    cell_xl = std::min(xl, cell_xl);
			    cell_yl = std::min(yl, cell_yl);
			    cell_xh = std::max(xh, cell_xh);
			    cell_yh = std::max(yh, cell_yh);
			} // if BOUNDARY
		} // for objects
		well_contact_rect = rectangle_t(cell_xl, cell_yl, cell_xh, cell_yh);
		well_contact_size = Loc<data_type>(cell_xh-cell_xl, cell_yh-cell_yl);
        M_DEBUG(
		cout<<"well contact size: "<<well_contact_size.w()<<", "<<well_contact_size.h()<<endl;
        );
	} // for cells
}
//}}}

void Legalize::read_offset_file()
//{{{ 
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ifstream in_offset_file;
	in_offset_file.open(in_offset_file_name);

	// NOTE: assume 3 contents in each anoynmouse
	string module_name;
	double offset_x, offset_y;
	while(in_offset_file >> module_name >> offset_x >> offset_y) {
		size_t vidx = m_vn_ids[module_name];
		// devices[vidx].offset = Loc<double>(offset_x, offset_y);
		devices[vidx].offset = Loc<int>((int)(offset_x/gds_scal_fac), (int)(offset_y/gds_scal_fac));
	}
	in_offset_file.close();
	M_DEBUG(
	cout<<"finish reading offset file"<<endl;
	);
}
//}}}

/*
void Legalize::read_ll_ur_file()
//{{{ 
{
	ifstream in_ll_ur_file;
	in_ll_ur_file.open(in_ll_ur_file_name);

	int l, b, r, t;
	in_ll_ur_file >> l >> b >> r >> t;
	in_ll_ur_file.close();

	// gds_ll = Loc<data_type>(l, b);
	// gds_ur = Loc<data_type>(r, t);
}
//}}}
*/

void Legalize::init_locs_and_sizes_from_file(string file_name)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	// assume same order as block file
	ifstream in_file;
	in_file.open(file_name);

	string module_name;
	double x, y, w, h;
	while(in_file >> module_name >> x >> y >> w >> h) {
		// cout<<"module_name: "<<module_name<<", locs: ("<<x<<", "<<y<<")"<<endl;
		m_vn_ids[module_name] = devices.size();
		// devices.push_back( Device(module_name, x, y, w, h) );
		devices.push_back( Device<data_type>(module_name, lround(x/gds_scal_fac), lround(y/gds_scal_fac), lround(w/gds_scal_fac), lround(h/gds_scal_fac)) );
	}
	in_file.close();
	M_DEBUG(
	cout<<"finish reading result from file"<<endl;
	);
}
//}}}

void Legalize::assign_devices_to_well(vector< set<size_t> >& devices_in_well, vector< polygon_90_t > const& aligned_contours, rtree_type const& rtree) 
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	using namespace boost::polygon::operators;
	for(polygon_90_t p : aligned_contours) {
		int xl = std::numeric_limits<int>::max(), yl = std::numeric_limits<int>::max(), xh = std::numeric_limits<int>::min(), yh = std::numeric_limits<int>::min();
		for(polygon_90_t::iterator_type vit=p.begin(); vit!=p.end(); ++vit) {
			xl = std::min(xl, gtl::x(*vit));
			yl = std::min(yl, gtl::y(*vit));
			xh = std::max(xh, gtl::x(*vit));
			yh = std::max(yh, gtl::y(*vit));
		}
		rectangle_t rect(xl, yl, xh, yh);
		vector< rtree_value > result_s;
		// rtree_devices.query(bgi::intersects(rect) && bgi::intersects(p), std::back_inserter(result_s));
		rtree.query(bgi::intersects(rect), std::back_inserter(result_s));
		devices_in_well.push_back(set<size_t>());
		for(rtree_value r : result_s) {
			polygon_90_set_t pset = r.first & p;
            M_DEBUG(
			cout<<"rectangle area: "<<gtl::area(r.first)<<", intersect area: "<<gtl::area(pset)<<endl;
            );
			if( (double)gtl::area(pset) / (double)gtl::area(r.first) > device_in_well_thresh ) {
				devices_in_well.back().insert(r.second);
			}
		}
	}
	
    M_DEBUG(
	cout<<"devices_in_well: "<<endl;
	for(auto dset : devices_in_well) {
		for(size_t d : dset) cout<<d<<", ";
		cout<<endl;
	}
    );
	
	// make sure no wrong assignment
	for(size_t i=0; i<devices.size(); ++i) {
		int cnt = 0;
		for(auto dw : devices_in_well) {
			if(dw.find(i) != dw.end()) {
				cnt++;
			}
		}
		// "assign device to well incorrect!"
        M_DEBUG(
		cout<<devices[i].name<<" type: "<<devices[i].type<<", cnt: "<<cnt<<endl;
        );
		// assert(("assign devices to well incorrect!", cnt == devices[i].type));
		assert(cnt == devices[i].type);
	}
	
}
//}}}

void Legalize::subtract_and_union(polygon_90_t& contour, set<size_t> const& dset)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	using namespace boost::polygon::operators;
	polygon_90_set_t pset;
	pset.insert(contour);

	// FIXME: subtract w/ spacing
	for(size_t i=0; i<devices.size(); ++i) {
		Device<data_type> d = devices[i];
		rectangle_t rect(d.loc.x(), d.loc.y(), d.loc.x()+d.size.w(), d.loc.y()+d.size.h());
		if(dset.find(i) == dset.end()) {
			pset -= rect;
		}
	}

	// FIXME: union w/ enclosure
	for(size_t i : dset) {
		Device<data_type> d = devices[i];
		rectangle_t rect(d.loc.x(), d.loc.y(), d.loc.x()+d.size.w(), d.loc.y()+d.size.h());
		pset += rect;
	}
	
	vector< polygon_90_t > pvec;
	pset.get_polygons(pvec);
	assert(pvec.size() == 1);
	contour = pvec[0];
}
//}}}

void Legalize::write_contour_to_file(vector< polygon_90_t > const& contours, string const& fname)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ofstream out_file;
	out_file.open(fname);
	for(auto contour : contours) {
		for(auto it=contour.begin(); it!=contour.end(); ++it) {
			out_file << gtl::x(*it);
			out_file << " ";
			out_file << gtl::y(*it);
			out_file << " ";
			// out_file << bg::dsv(*it);
		}
		out_file << endl;
	}
	out_file.close();
}
//}}}

void Legalize::get_polygon_envelop(rectangle_t& envelop, polygon_90_t const& p)
//{{{
{
	int xl=std::numeric_limits<int>::max(), yl=std::numeric_limits<int>::max(), xh=std::numeric_limits<int>::min(), yh=std::numeric_limits<int>::min();
    for(auto it=p.begin(); it!=p.end(); ++it) {
    	xl = std::min(xl, gtl::x(*it));
    	yl = std::min(yl, gtl::y(*it));
    	xh = std::max(xh, gtl::x(*it));
    	yh = std::max(yh, gtl::y(*it));
    }
	envelop = rectangle_t(xl, yl, xh, yh);
}
//}}}

void Legalize::get_polygon_set_envelop(rectangle_t& envelop, polygon_90_set_t const& p)
//{{{
{
	int xl=std::numeric_limits<int>::max(), yl=std::numeric_limits<int>::max(), xh=std::numeric_limits<int>::min(), yh=std::numeric_limits<int>::min();
	vector< polygon_90_t > pvec;
	p.get_polygons(pvec);
	for(auto ptmp : pvec) {
	    for(auto it=ptmp.begin(); it!=ptmp.end(); ++it) {
	    	xl = std::min(xl, gtl::x(*it));
	    	yl = std::min(yl, gtl::y(*it));
	    	xh = std::max(xh, gtl::x(*it));
	    	yh = std::max(yh, gtl::y(*it));
	    }
	}
	envelop = rectangle_t(xl, yl, xh, yh);
}
//}}}

void Legalize::bloat_polygon(polygon_90_t const& p1, polygon_90_t& p, data_type bloating)
//{{{
{
	polygon_90_set_t tmp;
	tmp.insert(p1);
	gtl::bloat(tmp, bloating);
	vector< polygon_90_t > tmp_vec;
	tmp.get_polygons(tmp_vec);
	p = tmp_vec[0];
}
//}}}

void Legalize::get_available_region(polygon_90_t const& p1, set<size_t> const& dset, data_type bloating, polygon_90_set_t& p)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	// DEBUG
	/*
	rectangle_t envelop1;
	get_polygon_envelop(envelop1, p1);
	cout<<"envelop before bloating: ";
	cout<<bg::dsv(envelop)<<endl;
	*/

	using namespace boost::polygon::operators;
	polygon_90_t ptmp;
	bloat_polygon(p1, ptmp, bloating);
	p.insert(ptmp);
	
	// DEBUG
	/*
	rectangle_t envelop;
	get_polygon_set_envelop(envelop, p);
	cout<<"envelop after bloating: ";
	cout<<bg::dsv(envelop)<<endl;
	*/

	for(size_t d : dset) {
		data_type xl = devices[d].loc.x();
		data_type yl = devices[d].loc.y();
		data_type xh = devices[d].loc.x() + devices[d].size.w();
		data_type yh = devices[d].loc.y() + devices[d].size.h();
		rectangle_t rect(xl, yl, xh, yh); 
		p -= rect;
	}
}
//}}}

int  Legalize::get_rects_intersect_area(rectangle_t const& r1, rectangle_t const& r2)
//{{{
{
	int x_overlap, y_overlap;
	if(gtl::xl(r1) >= gtl::xh(r2) || gtl::xl(r2) >= gtl::xh(r1)) {
		x_overlap = 0;
	} else {
		x_overlap = std::min(gtl::xh(r1) - gtl::xl(r2), gtl::xh(r2) - gtl::xl(r1));
	}
	if(gtl::yl(r1) >= gtl::yh(r2) || gtl::yl(r2) >= gtl::yh(r1)) {
		y_overlap = 0;
	} else {
		y_overlap = std::min(gtl::yh(r1) - gtl::yl(r2), gtl::yh(r2) - gtl::yl(r1));
	}
	return x_overlap * y_overlap;
}
//}}}

int	 Legalize::calculate_cost(int x, int y, set<size_t> const& dset, rtree_type const& rtree)
//{{{
{
	// cost is overlap area
	// TODO: if intersect devices_in_well, cost is larger
	int cost = 0;
	rectangle_t rect(x, y, x+well_contact_size.x(), y+well_contact_size.y());
	vector< rtree_value > result_s;
	rtree.query(bgi::intersects(rect), std::back_inserter(result_s));
	for(rtree_value r : result_s) {
		// TODO: consider spacing when calculating cost
		cost += get_rects_intersect_area(rect, r.first);
	}
	return cost;
}
//}}}

void Legalize::get_candidate_locations(cand_loc_type& cand_locs, cost_type& costs, conflict_cost_type& conflict_costs, vector< polygon_90_t > const& contours, vector< set<size_t> > const& devices_in_well, rtree_type const& rtree)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	using namespace boost::polygon::operators;
	// uniformly sample location according to some grid size
	int sample_dist = 40;
	rectangle_t well_contact(0, 0, well_contact_size.x(), well_contact_size.y());
	// currently just randomly sample and test if the location is feasible
	for(size_t i=0; i<contours.size(); ++i) {
		// polygon_90_t p = contours[i] + well_contact_size.x(); // bloating
		polygon_90_set_t p;
		// FIXME: DEBUG
		// FIXME: decide bloating value
		// hardcode
		int bloating = 0; // default
		// if(ckt_name == "COMPARATOR_PRE_AMP_test_flow") {
		// 	bloating = well_contact_size.x() - 20;
		// } 
        M_DEBUG(
		cout<<"bloating: "<<bloating<<endl;
        );
		// else if(ckt_name == "Core_test_flow")
		// int bloating = 0;
		// int bloating = well_contact_size.x() - 20;
		get_available_region(contours[i], devices_in_well[i], bloating, p);
		rectangle_t envelop;
		get_polygon_set_envelop(envelop, p);
		// subtract the devices inside
		int xl=gtl::xl(envelop), yl=gtl::yl(envelop), xh=gtl::xh(envelop), yh=gtl::yh(envelop);
        M_DEBUG(
		cout<<"envelop: "<<bg::dsv(envelop)<<endl;
        );
		cand_locs.push_back( vector< Loc<data_type> >() );
		costs.push_back( vector< int >() );
		for(int x=xl; x<=xh; x+=sample_dist) {
			for(int y=yl; y<=yh; y+=sample_dist) {
				rectangle_t rect = well_contact;
				gtl::move(rect, gtl::HORIZONTAL, x);
				gtl::move(rect, gtl::VERTICAL, y);
				polygon_90_set_t pset = p & rect; 
				if(!pset.empty()) {
					// TODO: add to candidate, and calculate the cost here
					cand_locs.back().push_back( Loc<data_type>(x, y) );
					int cost = calculate_cost(x, y, devices_in_well[i], rtree);
					costs.back().push_back( cost );
				}
			}
		}
	} // for each contour

	// TODO: get conflict_costs

}
//}}}

void Legalize::decide_well_contact_locations(cand_loc_type const& cand_locs, cost_type const& costs, conflict_cost_type const& conflict_costs, vector< Loc<data_type> >& wcontact_locs)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	// FIXME: currently greedily set well contact location
	for(size_t i=0; i<costs.size(); ++i) {
		int min_cost = std::numeric_limits<int>::max();
		int min_cost_index = 0;
		for(size_t j=0; j<costs[i].size(); ++j) {
			if(costs[i][j] < min_cost) {
				min_cost = costs[i][j];
				min_cost_index = j;
			}
		}
		wcontact_locs.push_back( cand_locs[i][min_cost_index] );
	}
	
    M_DEBUG(
	cout<<"wcontact_locs: ";
	for(auto loc : wcontact_locs) {
		cout<<"("<<loc.x()<<","<<loc.y()<<") ";
	}
	cout<<endl;
    );
}
//}}}

void Legalize::init_rtrees(rtree_type& rtree_devices, rtree_type& rtree_od)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	for(size_t i=0; i<devices.size(); ++i) {
		data_type xl = devices[i].loc.x();
		data_type yl = devices[i].loc.y();
		data_type xh = xl + devices[i].size.w();
		data_type yh = yl + devices[i].size.h();
		rtree_devices.insert(std::make_pair(rectangle_t(xl, yl, xh, yh), i));
	}
	for(size_t i=0; i<devices.size(); ++i) {
		data_type xl = devices[i].loc.x();
		data_type yl = devices[i].loc.y();
		for(LayerShape lshape : devices[i].layers_shapes) {
			// FIXME: OD layer
			if(lshape.layer == 6) {
				rectangle_t rect = lshape.shape; 
				M_DEBUG(
				cout<<"before:\t"<<bg::dsv(rect)<<endl;
				cout<<"xl: "<<xl<<", yl: "<<yl<<endl;
				);
				gtl::move(rect, gtl::HORIZONTAL, xl);
				gtl::move(rect, gtl::VERTICAL, yl);
				rtree_od.insert(std::make_pair(rect, i));
                M_DEBUG(
				cout<<"after:\t"<<bg::dsv(rect)<<endl;
				cout<<"OD rect area: "<<gtl::area(rect)<<endl;
                );
				break;
			}
		}
	}
}
//}}}

void Legalize::remove_empty_wells(vector< set<size_t> >& devices_in_well, vector< polygon_90_t >& contours)
//{{{
{
    set<size_t> wells_to_remove_set;

    typedef bg::model::d2::point_xy<data_type> point_xy;
    typedef bg::model::polygon<point_xy> polygon_xy;
    vector< polygon_xy > contours_xy;

    for(size_t i=0; i<contours.size(); ++i) {
        polygon_xy polygon;
        vector< point_xy > points;
        for(auto it=contours[i].begin(); it!=contours[i].end(); ++it) {
            points.push_back( point_xy(gtl::x(*it), gtl::y(*it)) );
        }
        bg::assign_points(polygon, points);
        contours_xy.push_back( polygon );
    }

    // FIXME: remove well if inside another well
    for(size_t i=0; i<contours_xy.size(); ++i) {
        for(size_t j=0; j<contours_xy.size(); ++j) {
            if(j == i) continue;
            if( bg::within(contours_xy[i], contours_xy[j]) ) {
                cout<<"contour "<<i<<" inside contour "<<j<<endl;
                // wells_to_remove.push_back(i);
                wells_to_remove_set.insert(i);
            }
        }
    }
    
    /*
    // FIXME: remove well if inside another well
    for(size_t i=0; i<contours.size(); ++i) {
        for(size_t j=0; j<contours.size(); ++j) {
            if(j == i) continue;
		    // NOTE: since adapted polygon is not working, I just use heuristics to decide coverage
		    bool contain_flag = true;
            for(auto it=contours[i].begin(); it!=contours[i].end(); ++it) {
		    	if( !gtl::contains(contours[j], *it) ) {
		    		contain_flag = false;
                    break;
		    	}
		    }
            if( contain_flag ) {
                wells_to_remove.push_back(i);
            }
        }
    }
    */
    // remove well if no devices in well
    for(size_t i=0; i<devices_in_well.size(); ++i) {
        if(devices_in_well[i].empty()) {
            cout<<"contour "<<i<<" empty"<<endl;
            // wells_to_remove.push_back(i);
            wells_to_remove_set.insert(i);
        }
    }

    // sort wells_to_remove
    vector<size_t> wells_to_remove_vec(wells_to_remove_set.begin(), wells_to_remove_set.end());
    std::sort(wells_to_remove_vec.begin(), wells_to_remove_vec.end(), std::greater<size_t>());

    // remove
    for(size_t idx : wells_to_remove_vec) {
        cout<<"remove contour "<<idx<<endl;
        devices_in_well.erase(devices_in_well.begin()+idx);
        contours.erase(contours.begin()+idx);
    }
}
//}}}

void Legalize::legalize()
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	// Step 1: union with the expanded GDS shapes (min enclosure)
	// FIXME: change init_locs_and_sizes_from_file and read_offset_file functions if change data_type
	read_techfile_simple();
	read_spacing_rules_file();
	read_enclosure_rules_file();

	init_locs_and_sizes_from_file(loc_file_name);
	read_offset_file();
	// read_ll_ur_file();
	parse_gds_files();
	read_symmetry_file();
	parse_well_contact_gds_file(in_well_contact_gds_name);
    M_DEBUG(
	cout<<"well_contact_rect: "<<bg::dsv(well_contact_rect)<<endl;
	cout<<"well_contact_pin_rect: "<<bg::dsv(well_contact_pin_rect)<<endl;
    );

	// check device layers
    M_DEBUG(
	cout<<"check devices layers: "<<endl;
	for(size_t i=0; i<devices.size(); ++i) {
		cout<<devices[i].name<<" layers: ";
		for(LayerShape lshape : devices[i].layers_shapes) {
			cout<<lshape.layer<<", ";
		}
		cout<<endl;
	}
	cout<<"layer_num2id: ";
	for(auto l : layer_num2id) {
		cout<<"("<<l.first<<"->"<<l.second<<"), ";
	}
	cout<<endl;
	cout<<"layers: ";
	for(auto l : layers) {
		cout<<"("<<l.first<<"->"<<l.second<<"), ";
	}
	cout<<endl;
    );
	
	read_contour_offset_file(contour_offset_file_name);
	vector< polygon_90_t > contours;
	read_contour_file(contour_file_name, contours);
	/*
	M_DEBUG(
	cout<<"device types: ";
	for(Device<data_type> d : devices) {
		cout<<d.type<<", ";
	}
	cout<<endl;
	);
	cout<<"contours.size(): "<<contours.size()<<endl;
	*/

	// Step: union with the device with NW, and subtract those without NW
	// union_and_subtract(contours);
	
	
	// Step: align to grid individually, and consider min enclosure and min spacing
	/*
	vector< polygon_90_t > aligned_contours;
	for(auto contour : contours) {
		// first point as the origin
		align_to_grid(contour, *contour.begin(), aligned_contours);
	}
	*/

	vector< polygon_90_t > aligned_contours = contours;
	
	// Step: add well contact and union with the well to satisfy min enclosure
	// assign devices to well islands
	rtree_type rtree_devices, rtree_od;
	// use OD layer as the indicator
	init_rtrees(rtree_devices, rtree_od);
	
	// write_contour_to_file(aligned_contours, ckt_name+".contours.aligned");
	
	vector< set<size_t> > devices_in_well;
	// assign_devices_to_well(devices_in_well, aligned_contours, rtree_devices);
	assign_devices_to_well(devices_in_well, aligned_contours, rtree_od);

    M_DEBUG(
    cout<<"before remove_empty_wells aligned_contours.size(): "<<aligned_contours.size()<<endl;
    );
    remove_empty_wells(devices_in_well, aligned_contours);
    M_DEBUG(
    cout<<"after remove_empty_wells aligned_contours.size(): "<<aligned_contours.size()<<endl;
    );

	// get candidate location and insert well contacts
	// FIXME: currently  well contact size : 280 x 280
	cand_loc_type		cand_locs;
	cost_type			costs; // the cost of the candidate location it self
	conflict_cost_type	conflict_costs; // the conflict cost between 2 candidate location
	get_candidate_locations(cand_locs, costs, conflict_costs, aligned_contours, devices_in_well, rtree_devices);
	vector< Loc<data_type> > wcontact_locs;
	decide_well_contact_locations(cand_locs, costs, conflict_costs, wcontact_locs);
	// FIXME: may need to legalize well contact location due to overlap and spacing 


	vector< polygon_90_t > legalized_contours;
	for(size_t i=0; i<aligned_contours.size(); ++i) {
		// first point as the origin
		point_t pt = *(aligned_contours[i].begin());
		polygon_90_t legalized_contour;
		align_to_grid_legalize(aligned_contours[i], pt, wcontact_locs[i], devices_in_well[i], legalized_contour, legalized_contours);
		// legalized_contours.push_back(legalized_contour);
	}
	
	write_contour_to_file(legalized_contours, result_dir+"/"+ckt_name+"/"+ckt_name+".contours.legalized");
	
	cout<<"contours.size(): "<<contours.size()<<endl;
	cout<<"aligned_contours.size(): "<<aligned_contours.size()<<endl;
	cout<<"legalized_contours.size(): "<<legalized_contours.size()<<endl;
	/*
	for(polygon_90_t p : aligned_contours) {
		cout<<"polygon: ";
		for(auto it=p.begin(); it!=p.end(); ++it) {
			cout<<bg::dsv(*it)<<", ";
		}
		cout<<endl;
	}
	*/

	// Step: LP to legalize the well islands
	legalize_well(legalized_contours, devices_in_well, wcontact_locs);
	
	// Step: write out NW file and updated location


	
}
//}}}

void Legalize::legalize_well(vector< polygon_90_t > const& legalized_contours, vector< set<size_t> > const& devices_in_well, vector< Loc<data_type> > const& wcontact_locs)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	// relative location of the devices inside well does not change
	// they change with the well
	vector< dEdge > well_edges;
	vector< dEdge > device_edges;
	for(size_t i=0; i<legalized_contours.size(); ++i) {
		polygon_90_t p = legalized_contours[i];
		point_t orig = *p.begin();
		get_well_edges(well_edges, p, orig, devices.size()+i); // the id of the well is above devices id 
	}
	get_device_edges(device_edges);

    M_DEBUG(
	cout<<"devices.size(): "<<devices.size()<<endl;
	cout<<"well_edges: "<<endl;
	for(dEdge e : well_edges) {
		cout<<e<<endl;
	}
	cout<<"device_edges: "<<endl;
	for(dEdge e : device_edges) {
		cout<<e<<endl;
	}
    );

	vector<int> x_values(devices.size()), y_values(devices.size());
	vector< pair<int, int> > well_disp;
	legalize_lp(well_edges, device_edges, legalized_contours, x_values, y_values, well_disp, devices_in_well);
	write_result_to_file(result_dir+"/"+ckt_name+"/"+ckt_name+".result.final", x_values, y_values);

	vector< polygon_90_t > contours;
	for(size_t i=0; i<legalized_contours.size(); ++i) {
		polygon_90_t p = legalized_contours[i];
		gtl::move(p, gtl::HORIZONTAL, well_disp[i].first);
		gtl::move(p, gtl::VERTICAL, well_disp[i].second);
		contours.push_back( p );
	}
	write_contour_to_file(contours, result_dir+"/"+ckt_name+"/"+ckt_name+".contours.final");

	vector< Loc<int> > wcontacts; // lower left corner
	for(size_t i=0; i<legalized_contours.size(); ++i) {
		// one contact for one well
		int wcon_x = wcontact_locs[i].x() + well_disp[i].first;
		int wcon_y = wcontact_locs[i].y() + well_disp[i].second;
		wcontacts.push_back( Loc<int>(wcon_x, wcon_y) );
	}
	write_wcontact_to_file(wcontacts, result_dir+"/"+ckt_name+"/"+ckt_name+".well_contacts");
	write_wcontact_pin_to_file(wcontacts, result_dir+"/"+ckt_name+"/"+ckt_name+".wcon");

	////// write GDSII layout /////
	string test_gds_fname = in_gds_file_prefix+devices[0].name+".gds";
    GdsParser::GdsDB::GdsDB test_db; 
    GdsParser::GdsDB::GdsReader reader (test_db); 
    reader(test_gds_fname);
	GdsParser::GdsDB::GdsDB db = test_db;
	GdsCell& cell = write_gds_cell(db, x_values, y_values);
	write_gds_well(cell, contours);
	write_gds_well_contact(db, cell, wcontacts);
	write_gds_guard_ring(db, cell, result_dir+"/"+ckt_name+"/"+ckt_name+".sub");
	
	// hierarchical
	GdsParser::GdsDB::GdsWriter writer (db);
	writer(result_dir+"/"+ckt_name+"/"+ckt_name+".wellgen.gds");
    
    // write flattened cell 
	/*
	GdsParser::GdsDB::GdsCell flatCell = db.extractCell(ckt_name);
	GdsParser::GdsDB::GdsDB flatDB; 
    flatDB.addCell(flatCell); 
    GdsParser::GdsDB::GdsWriter flatGw (flatDB); 
    flatGw(results_dir+"/"+ckt_name+"/"+ckt_name+".wellgen.gds");
	*/
}
//}}}

void Legalize::write_wcontact_to_file(vector< Loc<int> > const& wcontacts, string out_file_name)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	std::ofstream out_file;
	out_file.open(out_file_name);
	//format: name x y w h
	for(size_t i=0; i<wcontacts.size(); ++i) {
		// store result to location vector
		out_file<<(wcontacts[i].x() + well_contact_size.w()/2)*gds_scal_fac<<" ";
		out_file<<(wcontacts[i].y() + well_contact_size.h()/2)*gds_scal_fac<<endl;
	}
	out_file.close();
}
//}}}

void Legalize::write_wcontact_pin_to_file(vector< Loc<int> > const& wcontacts, string out_file_name)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	std::ofstream out_file;
	out_file.open(out_file_name);
	for(size_t i=0; i<wcontacts.size(); ++i) {
		// store result to location vector
		// FIXME: well contact offset is 0, 0
		out_file<<"1 "; // pin layer
		out_file<<(wcontacts[i].x() + gtl::xl(well_contact_pin_rect))<<" ";
		out_file<<(wcontacts[i].y() + gtl::yl(well_contact_pin_rect))<<" ";
		out_file<<(wcontacts[i].x() + gtl::xh(well_contact_pin_rect))<<" ";
		out_file<<(wcontacts[i].y() + gtl::yl(well_contact_pin_rect))<<" ";
		out_file<<(wcontacts[i].x() + gtl::xh(well_contact_pin_rect))<<" ";
		out_file<<(wcontacts[i].y() + gtl::yh(well_contact_pin_rect))<<" ";
		out_file<<(wcontacts[i].x() + gtl::xl(well_contact_pin_rect))<<" ";
		out_file<<(wcontacts[i].y() + gtl::yh(well_contact_pin_rect))<<" ";
		out_file<<endl;
	}
	out_file.close();
}
//}}}

void Legalize::write_result_to_file(string out_file_name, vector<int> const& x_values, vector<int> const& y_values)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	std::ofstream out_file;
	out_file.open(out_file_name);
	//format: name x y w h
	for(size_t i=0; i<devices.size(); ++i) {
		// store result to location vector
		out_file<<devices[i].name<<" ";
		out_file<<x_values[i]*gds_scal_fac<<" ";
		out_file<<y_values[i]*gds_scal_fac<<" ";
        M_DEBUG(
		cout<<"size: "<<devices[i].size.w()<<", "<<devices[i].size.h()<<endl;
        );
		out_file<<devices[i].size.w()*gds_scal_fac<<" ";
		out_file<<devices[i].size.h()*gds_scal_fac<<"\n";
	}
	out_file.close();
}
//}}}

void Legalize::legalize_lp(vector< dEdge > const& well_edges, vector< dEdge > const& device_edges, vector< polygon_90_t > const& legalized_contours, vector<int>& x_values, vector<int>& y_values, vector< pair<int, int> >& well_disp, vector< set<size_t> > const& devices_in_well)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	typedef limbo::solvers::anoynmousearModel<double, double> model_type; 
    model_type model; 
	
	std::map<size_t, size_t> did2id; // map device id to variable id
	size_t n = 0;
	for(auto d : devices) {
		if(d.type == 0) n++;
	}
	n += legalized_contours.size();
	// add coordinate variables
    size_t num_vars = 4*n; 
    model.reserveVariables(num_vars); 
    std::vector<model_type::variable_type> x(n), y(n), xdisp(n), ydisp(n); 
	size_t cnt = 0;
	for(size_t i=0; i<devices.size(); ++i) {
		if(devices[i].type == 1) continue; 
		did2id[i] = cnt;
        x[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "x_"+std::to_string(i)); 
        y[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "y_"+std::to_string(i)); 
        xdisp[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "xdisp_"+std::to_string(i)); 
        ydisp[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "ydisp_"+std::to_string(i)); 
		cnt++;
	}
	for(size_t i=0; i<legalized_contours.size(); ++i) {
		did2id[devices.size()+i] = cnt;
        x[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "x_"+std::to_string(devices.size()+i)); 
        y[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "y_"+std::to_string(devices.size()+i)); 
        xdisp[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "xdisp_"+std::to_string(devices.size()+i)); 
        ydisp[cnt] = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "ydisp_"+std::to_string(devices.size()+i)); 
		cnt++;
	}
	
	vector< dEdge > edges;
	edges.insert(edges.end(), well_edges.begin(), well_edges.end());
	edges.insert(edges.end(), device_edges.begin(), device_edges.end());
	// topology constraints
	for(dEdge e : edges) {
		size_t id1 = did2id[e.src];
		size_t id2 = did2id[e.tgt];
		if(e.is_horizontal) {
            model.addConstraint(x[id1] - x[id2] <= -e.value); 
		} else {
            model.addConstraint(y[id1] - y[id2] <= -e.value); 
		}
	}
	
	// symmetry constraints
	for (size_t i=0; i<symmetric_groups.size(); ++i) {
		symmetric_pairs_t ps = symmetric_groups[i].first;
		for (symmetric_pair_t p : ps) {
            if(did2id.find(p.first) != did2id.end() && did2id.find(p.second) != did2id.end()) {
		        size_t id1 = did2id.at(p.first);
		        size_t id2 = did2id.at(p.second);
				model.addConstraint(y[id1] - y[id2] == 0); 
            } 
		}
	}
	for (size_t i=0; i<symmetric_groups.size(); ++i) {
        model_type::variable_type xsym = model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "xsym_"+std::to_string(i)); 
		symmetric_pairs_t ps = symmetric_groups[i].first;
		self_symmetric_t ss = symmetric_groups[i].second;
		for (symmetric_pair_t p : ps) {
            M_DEBUG(
            cout<<"symmetry pair "<<p.first<<" and "<<p.second<<endl;
            );
            if(did2id.find(p.first) != did2id.end() && did2id.find(p.second) != did2id.end()) {
		        size_t id1 = did2id.at(p.first);
		        size_t id2 = did2id.at(p.second);
                M_DEBUG(
                cout<<"did2id: "<<id1<<" and "<<id2<<endl;
                );
				model.addConstraint(x[id1] + x[id2] - 2*xsym == -devices[p.first].size.w()); 
            } else if(did2id.find(p.first) == did2id.end() && did2id.find(p.second) == did2id.end()) {
				int orig_x_1 = devices[p.first].loc.x();
				int orig_x_2 = devices[p.second].loc.x();
				int orig_w = devices[p.first].size.w();
				size_t well_id = 0;
				for(well_id=0; well_id<devices_in_well.size(); ++well_id) {
					if(devices_in_well[well_id].find(p.first) != devices_in_well[well_id].end()) {
						break;
					}
				}
				size_t id = did2id[devices.size()+well_id];
				point_t orig = *(legalized_contours[well_id].begin());
				int well_orig_x = gtl::x(orig);
				model.addConstraint(2*x[id] - 2*xsym == -orig_x_1-orig_w-orig_x_2+2*well_orig_x);
			} else {
				// symmetric pairs should have the same type 
				assert(1);
			}
		}
		for (size_t s : ss) {
            if(did2id.find(s) != did2id.end()) {
			    size_t ids = did2id[s];
				model.addConstraint(2*x[ids] - 2*xsym == -devices[s].size.w());
            } else {
				int device_orig_x = devices[s].loc.x();
				int orig_w = devices[s].size.w();
				size_t well_id = 0;
				for(well_id=0; well_id<devices_in_well.size(); ++well_id) {
					if(devices_in_well[well_id].find(s) != devices_in_well[well_id].end()) {
						break;
					}
				}
				size_t id = did2id[devices.size()+well_id];
				point_t orig = *(legalized_contours[well_id].begin());
				int well_orig_x = gtl::x(orig);
				model.addConstraint(2*x[id] - 2*xsym == -2*device_orig_x-orig_w+2*well_orig_x);
			}
		}
	}
	
	// displacement constraints
	for(size_t i=0; i<devices.size(); ++i) {
		if(devices[i].type == 1) continue; 
		size_t id = did2id[i];
		int orig_x = devices[i].loc.x();
		int orig_y = devices[i].loc.y();
		model.addConstraint(xdisp[id] - x[id] >= -orig_x);
		model.addConstraint(xdisp[id] + x[id] >= orig_x);
		model.addConstraint(ydisp[id] - y[id] >= -orig_y);
		model.addConstraint(ydisp[id] + y[id] >= orig_y);
	}
	for(size_t i=0; i<legalized_contours.size(); ++i) {
		size_t id = did2id[devices.size()+i];
		point_t orig = *(legalized_contours[i].begin());
		cout<<"orig: "<<bg::dsv(orig)<<endl;
		int orig_x = gtl::x(orig);
		int orig_y = gtl::y(orig);
		model.addConstraint(xdisp[id] - x[id] >= -orig_x);
		model.addConstraint(xdisp[id] + x[id] >= orig_x);
		model.addConstraint(ydisp[id] - y[id] >= -orig_y);
		model.addConstraint(ydisp[id] + y[id] >= orig_y);
	}

	// objective
	model_type::expression_type obj; 
	for(size_t i=0; i<n; ++i) {
		obj += xdisp[i] + ydisp[i];
	}
    model.setObjective(obj); 
    model.setOptimizeType(limbo::solvers::MIN);

    limbo::solvers::SolverProperty optimstatus; 

    switch (optimizer_type) 
    {
        case OptimizerType::GUROBI:
            {
#ifdef USE_GUROBI
                // solve by Gurobi 
                typedef limbo::solvers::GurobianoynmousearApi<model_type::coefficient_value_type, model_type::variable_value_type> solver_type; 
                solver_type solver (&model); 
                limbo::solvers::GurobiParameters params; 
                params.setNumThreads(1);
                params.setOutputFlag(1); 

                optimstatus = solver(&params); 
#else
				cout << "Solver type was specified as GUROBI, but GUROBI_HOME was not defined" << endl; 
				assert(0);
#endif

            }
            break; 
        case OptimizerType::LPSOLVE:
            {
                // solve by LPSolve 
                typedef limbo::solvers::LPSolveanoynmousearApi<model_type::coefficient_value_type, model_type::variable_value_type> solver_type; 
                solver_type solver (&model); 
                limbo::solvers::LPSolveParameters params; 
                params.setVerbose(SEVERE); 

                optimstatus = solver(&params); 
            }
            break; 
        default:
            cout << "unknown optimizer_type " << int(optimizer_type) << endl; 
            assert(0); 
    }
    
	M_DEBUG(
	std::cout << "////////////////////// " << __func__ << "//////////////////////\n";
    model.print(std::cout); 
	);
	
	if (optimstatus == limbo::solvers::UNBOUNDED) {
		cout<<"Solver infinity or unbounded"<<endl;
	} else if (optimstatus == limbo::solvers::OPTIMAL) {
		double objval = model.evaluateObjective();
		cout << "Optimal objective: " << objval << endl;
		// print xdisp and ydisp
		cout<<"xdisp: ";
		for(size_t i=0; i<n; ++i) {
			cout<<model.variableSolution(xdisp[i])<<", ";
		}
		cout<<endl;
		cout<<"ydisp: ";
		for(size_t i=0; i<n; ++i) {
			cout<<model.variableSolution(ydisp[i])<<", ";
		}
		cout<<endl;
		for(size_t i=0; i<legalized_contours.size(); ++i) {
			size_t well_id = did2id[devices.size()+i];
			int well_x = lround(model.variableSolution(x[well_id]));
			int well_y = lround(model.variableSolution(y[well_id]));
			point_t orig = *(legalized_contours[i].begin());
            M_DEBUG(
			cout<<"orig: "<<bg::dsv(orig)<<endl;
            );
			int well_disp_x = well_x - gtl::x(orig);
			int well_disp_y = well_y - gtl::y(orig);
			cout<<"well_disp: "<<well_disp_x<<", "<<well_disp_y<<endl;
			well_disp.push_back( std::make_pair(well_disp_x, well_disp_y) );
			for(size_t d : devices_in_well[i]) {
				x_values[d] = devices[d].loc.x() + well_disp_x;
				y_values[d] = devices[d].loc.y() + well_disp_y;
			}
		}
		for(size_t i=0; i<devices.size(); ++i) {
			if(devices[i].type == 1) continue;
			size_t id = did2id[i];
			x_values[i] = lround(model.variableSolution(x[id]));
			y_values[i] = lround(model.variableSolution(y[id]));
		}
	} else if (optimstatus == limbo::solvers::INFEASIBLE) {
		cout << "lp_model is infeasible" << endl;
	} else {
		cout << "Optimization was stopped with status = "
			<< optimstatus << endl;
	}
}
//}}}

void Legalize::get_spacings(vector<int>& spacings, size_t device_id)
//{{{
{
	int w = devices[device_id].size.w(), h = devices[device_id].size.h();
	int spacing_l = 0, spacing_r = 0, spacing_b = 0, spacing_t = 0;
	for(auto lshape : devices[device_id].layers_shapes) {
		int lxl = gtl::xl(lshape.shape);
		int lyl = gtl::yl(lshape.shape);
		int lxh = gtl::xh(lshape.shape);
		int lyh = gtl::yh(lshape.shape);
		int xh_dist = w - lxh;
		int yh_dist = h - lyh;
		// FIXME: NW layer 3
        M_DEBUG(
		cout<<"lshape.layer: "<<lshape.layer<<endl;
        );
		if(layer_num2id.find(lshape.layer) == layer_num2id.end()) {
			M_DEBUG(
			cout<<"Unsuported layer!"<<endl;
			);
			continue;
		}
		int rule = spacing_rules[layer_num2id.at(3)][layer_num2id.at(lshape.layer)]; 
		if(rule != -1) {
			spacing_l = std::max(spacing_l, rule - lxl);
			spacing_r = std::max(spacing_r, rule - xh_dist);
			spacing_b = std::max(spacing_b, rule - lyl);
			spacing_t = std::max(spacing_t, rule - yh_dist);
		}
	} // for
	spacings = {spacing_l, spacing_r, spacing_b, spacing_t};
	// spacings = {0, 0, 0, 0};
}
//}}}

void Legalize::sort_displacement(rectangle_t const& drect, rectangle_t const& rect, vector<int>& v, vector<size_t>& idx, vector<int> const& spacings) 
//{{{
{
	int dxl = gtl::xl(drect), dyl = gtl::yl(drect), dxh = gtl::xh(drect), dyh = gtl::yh(drect);
	int xl = gtl::xl(rect), yl = gtl::yl(rect), xh = gtl::xh(rect), yh = gtl::yh(rect);
	// int w = dxh - dxl, h = dyh - dyl;
	// consider layers
	int spacing_l = spacings[0], spacing_r = spacings[1], spacing_b = spacings[2], spacing_t = spacings[3];
	// calculate displacement
	/*
	int disp_l = dxh - xl;
	int disp_r = xh - dxl;
	int disp_b = dyh - yl;
	int disp_t = yh - dyl;
	*/
	int disp_l = dxh - xl + spacing_r;
	int disp_r = xh - dxl + spacing_l;
	int disp_b = dyh - yl + spacing_t;
	int disp_t = yh - dyl + spacing_b;
	// vector<int> v{disp_l, disp_r, disp_b, disp_t};
	v = {disp_l, disp_r, disp_b, disp_t};
	// vector<size_t> idx;
	sort_indexes(v, idx);
}
//}}}

void Legalize::sort_indexes(vector<int> const&v, vector<size_t>& idx_arg)
//{{{
{
	// initialize original index locations
  	vector<size_t> idx(v.size());
	std::iota(idx.begin(), idx.end(), 0);

  	// sort indexes based on comparing values in v
	std::sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

  	idx_arg = idx;
}
//}}}

void Legalize::get_polygon_edges(polygon_90_t const& p, vector< segment_t >& edges)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	// compare low, high coords
	vector< point_t > pts(p.begin(), p.end());
	for(size_t i=0; i<pts.size()-1; ++i) {
		// edges.push_back( segment_t(pts[i], pts[i+1]) );
		if(gtl::x(pts[i]) == gtl::x(pts[i+1])) {
			// vertical segment
			point_t pt_low(gtl::x(pts[i]), std::min(gtl::y(pts[i]), gtl::y(pts[i+1])));
			point_t pt_high(gtl::x(pts[i]), std::max(gtl::y(pts[i]), gtl::y(pts[i+1])));
			edges.push_back( segment_t(pt_low, pt_high) );
		} else {
			// horizontal segment
			point_t pt_low(std::min(gtl::x(pts[i]), gtl::x(pts[i+1])), gtl::y(pts[i]));
			point_t pt_high(std::max(gtl::x(pts[i]), gtl::x(pts[i+1])), gtl::y(pts[i]));
			edges.push_back( segment_t(pt_low, pt_high) );
		}
	}
	if(gtl::x(pts[pts.size()-1]) == gtl::x(pts[0])) {
		// vertical segment
		point_t pt_low(gtl::x(pts[0]), std::min(gtl::y(pts[pts.size()-1]), gtl::y(pts[0])));
		point_t pt_high(gtl::x(pts[0]), std::max(gtl::y(pts[pts.size()-1]), gtl::y(pts[0])));
		edges.push_back( segment_t(pt_low, pt_high) );
	} else {
		// horizontal segment
		point_t pt_low(std::min(gtl::x(pts[pts.size()-1]), gtl::x(pts[0])), gtl::y(pts[0]));
		point_t pt_high(std::max(gtl::x(pts[pts.size()-1]), gtl::x(pts[0])), gtl::y(pts[0]));
		edges.push_back( segment_t(pt_low, pt_high) );
	}
}
//}}}

void Legalize::get_overlap_well_edge_new(rectangle_t const& drect, vector< rectangle_t > const& rects, size_t well_id, size_t device_id, polygon_90_t const& p, vector<dEdge>& edges, point_t const& orig, vector<int> const& spacings)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	//int dxl = gtl::xl(drect), dyl = gtl::yl(drect), dxh = gtl::xh(drect), dyh = gtl::yh(drect);
	int w = devices[device_id].size.w(), h = devices[device_id].size.h();
	vector< segment_t > polygon_edges;
	get_polygon_edges(p, polygon_edges);
	// FIXME: this method is heuristics
	// get the edges w/ horizontal or vertical overlap
	int ovl_xl=std::numeric_limits<int>::max(), ovl_yl=std::numeric_limits<int>::max(), ovl_xh=std::numeric_limits<int>::min(), ovl_yh=std::numeric_limits<int>::min();
	interval_t intv_h = gtl::horizontal(drect), intv_v = gtl::vertical(drect);
    M_DEBUG(
	cout<<"intv_h: "<<intv_h.low()<<", "<<intv_h.high()<<endl;
	cout<<"intv_v: "<<intv_v.low()<<", "<<intv_v.high()<<endl;
    );
	for(segment_t e : polygon_edges) {
		if(gtl::x(e.low()) == gtl::x(e.high())) {
			// if is vertical segment
			interval_t intv( gtl::y(e.low()), gtl::y(e.high()) );
            M_DEBUG(
			cout<<"vertical intv: "<<intv.low()<<", "<<intv.high()<<endl;
            );
			if(gtl::intersects(intv, intv_v, false)) {
                M_DEBUG(
				cout<<"intersects"<<endl;
                );
				ovl_xl = std::min(ovl_xl, gtl::x(e.low()));
				ovl_xh = std::max(ovl_xh, gtl::x(e.low()));
			}
		} else {
			// if is horizontal segment
			interval_t intv( gtl::x(e.low()), gtl::x(e.high()) );
            M_DEBUG(
			cout<<"horizontal intv: "<<intv.low()<<", "<<intv.high()<<endl;
            );
			if(gtl::intersects(intv, intv_h, false)) {
                M_DEBUG(
				cout<<"intersects"<<endl;
                );
				ovl_yl = std::min(ovl_yl, gtl::y(e.low()));
				ovl_yh = std::max(ovl_yh, gtl::y(e.low()));
			}
		}
	}
	rectangle_t ovl_rect(ovl_xl, ovl_yl, ovl_xh, ovl_yh);
    M_DEBUG(
	cout<<"ovl_rect: "<<bg::dsv(ovl_rect)<<endl;
	cout<<"orig: "<<bg::dsv(orig)<<endl;
    );

	vector<int> v;
	vector<size_t> idx;
	sort_displacement(drect, ovl_rect, v, idx, spacings);
    M_DEBUG(
	cout<<"v: ";
	for(auto vv : v) { cout<<vv<<", "; }
	cout<<endl;
	cout<<"idx: ";
	for(auto vv : idx) { cout<<vv<<", "; }
	cout<<endl;
    );
	// cout<<"well orig:\t"<<bg::dsv(orig)<<endl;
	// cout<<"well rect:\t"<<bg::dsv(rect)<<endl;
	int offset_x = ovl_xl - gtl::x(orig); 
	int offset_y = ovl_yl - gtl::y(orig); 
    M_DEBUG(
	cout<<"device rect:\t"<<bg::dsv(drect)<<endl;
	cout<<"idx[0]: "<<idx[0]<<endl;
    );
	if(idx[0] == 0) {
		// left, add edge to the left
		dEdge e(device_id, well_id, 1);
		// spacing_r
		e.value = w + spacings[1] - offset_x; 
        M_DEBUG(
		cout<<"w: "<<w<<endl;
		cout<<"spacing: "<<spacings[1]<<endl;
		cout<<"offset_x: "<<offset_x<<endl;
		cout<<"add overlap edge for well: "<<e<<endl;
        );
		edges.push_back(e);
	} else if (idx[0] == 1) {
		// right
		dEdge e(well_id, device_id, 1);
		// spacing_l
		e.value = (ovl_xh - ovl_xl) + spacings[0] + offset_x; 
        M_DEBUG(
		cout<<"ovl_xh-ovl_xl: "<<ovl_xh-ovl_xl<<endl;
		cout<<"spacing: "<<spacings[0]<<endl;
		cout<<"offset_x: "<<offset_x<<endl;
		cout<<"add overlap edge for well: "<<e<<endl;
        );
		edges.push_back(e);
	} else if (idx[0] == 2) {
		// bottom
		dEdge e(device_id, well_id, 0);
		// spacing_t
		e.value = h + spacings[3] - offset_y; 
        M_DEBUG(
		cout<<"h: "<<h<<endl;
		cout<<"spacing: "<<spacings[3]<<endl;
		cout<<"offset_y: "<<offset_y<<endl;
		cout<<"add overlap edge for well: "<<e<<endl;
        );
		edges.push_back(e);
	} else if (idx[0] == 3) {
		// top
		dEdge e(well_id, device_id, 0);
		// spacing_b
		e.value = (ovl_yh - ovl_yl) + spacings[2] + offset_y; 
        M_DEBUG(
		cout<<"ovl_yh-ovl_yl: "<<ovl_yh-ovl_yl<<endl;
		cout<<"spacing: "<<spacings[2]<<endl;
		cout<<"offset_y: "<<offset_y<<endl;
		cout<<"add overlap edge for well: "<<e<<endl;
        );
		edges.push_back(e);
	} else {
		// assert(("get overlap well edge error!", 1)); // error here
		assert(1); // error here
	}
}
//}}}

/*
void Legalize::get_overlap_well_edge(rectangle_t const& drect, vector< rectangle_t > const& rects, size_t well_id, size_t device_id, polygon_90_t const& p, vector<dEdge>& edges, point_t const& orig, vector<int> const& spacings)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    )
	using namespace boost::polygon::operators;
	// vector<int> spacings;
	// get_spacings(spacings, device_id);
	int dxl = gtl::xl(drect), dyl = gtl::yl(drect), dxh = gtl::xh(drect), dyh = gtl::yh(drect);
	int w = devices[device_id].size.w(), h = devices[device_id].size.h();
	for(rectangle_t rect : rects) {
	    int xl = gtl::xl(rect), yl = gtl::yl(rect), xh = gtl::xh(rect), yh = gtl::yh(rect);
		int offset_x = xl - gtl::x(orig); 
		int offset_y = yl - gtl::y(orig); 
		if( gtl::intersects(rect, drect, false) ) {
			//{{{
			// 4 directions, with min displacement and no overlap
			vector<int> v;
			vector<size_t> idx;
			sort_displacement(drect, rect, v, idx, spacings);
			cout<<"v: ";
			for(auto vv : v) { cout<<vv<<", "; }
			cout<<endl;
			cout<<"idx: ";
			for(auto vv : idx) { cout<<vv<<", "; }
			cout<<endl;
			cout<<"well orig:\t"<<bg::dsv(orig)<<endl;
			cout<<"well rect:\t"<<bg::dsv(rect)<<endl;
			cout<<"device rect:\t"<<bg::dsv(drect)<<endl;
			for(size_t i : idx) {
				cout<<"i: "<<i<<endl;
				if(i == 0) {
					// left
					// check if intersect after displacement
					polygon_90_set_t pset;
					rectangle_t rect_move = drect;
					gtl::move(rect_move, gtl::HORIZONTAL, -v[i]);
					pset = p & rect_move; 
					// FIXME: boundary touch?
					cout<<"rect_move:\t"<<bg::dsv(rect_move)<<endl;
					cout<<"pset area:\t"<<gtl::area(pset)<<endl;
					if(pset.empty()) {
						// add edge to the left
						dEdge e(device_id, well_id, 1);
						// spacing_r
						cout<<"w: "<<w<<endl;
						cout<<"spacing: "<<spacings[1]<<endl;
						cout<<"offset_x: "<<offset_x<<endl;
						e.value = w + spacings[1] - offset_x; 
						cout<<"add overlap edge for well: "<<e<<endl;
						edges.push_back(e);
						break;
					}
				} else if (i == 1) {
					// right
					polygon_90_set_t pset;
					rectangle_t rect_move = drect;
					gtl::move(rect_move, gtl::HORIZONTAL, v[i]);
					pset = p & rect_move; 
					cout<<"rect_move:\t"<<bg::dsv(rect_move)<<endl;
					cout<<"pset area:\t"<<gtl::area(pset)<<endl;
					if(pset.empty()) {
						dEdge e(well_id, device_id, 1);
						// spacing_l
						cout<<"xh-xl: "<<xh-xl<<endl;
						cout<<"spacing: "<<spacings[0]<<endl;
						cout<<"offset_x: "<<offset_x<<endl;
						e.value = (xh - xl) + spacings[0] + offset_x; 
						cout<<"add overlap edge for well: "<<e<<endl;
						edges.push_back(e);
						break;
					}
				} else if (i == 2) {
					// bottom
					polygon_90_set_t pset;
					rectangle_t rect_move = drect;
					gtl::move(rect_move, gtl::VERTICAL, -v[i]);
					pset = p & rect_move; 
					cout<<"rect_move:\t"<<bg::dsv(rect_move)<<endl;
					cout<<"pset area:\t"<<gtl::area(pset)<<endl;
					if(pset.empty()) {
						dEdge e(device_id, well_id, 0);
						// spacing_t
						cout<<"h: "<<h<<endl;
						cout<<"spacing: "<<spacings[3]<<endl;
						cout<<"offset_y: "<<offset_y<<endl;
						e.value = h + spacings[3] - offset_y; 
						cout<<"add overlap edge for well: "<<e<<endl;
						edges.push_back(e);
						break;
					}
				} else if (i == 3) {
					// top
					polygon_90_set_t pset;
					rectangle_t rect_move = drect;
					gtl::move(rect_move, gtl::VERTICAL, v[i]);
					pset = p & rect_move; 
					cout<<"rect_move:\t"<<bg::dsv(rect_move)<<endl;
					cout<<"pset area:\t"<<gtl::area(pset)<<endl;
					if(pset.empty()) {
						dEdge e(well_id, device_id, 0);
						// spacing_b
						cout<<"yh-yl: "<<yh-yl<<endl;
						cout<<"spacing: "<<spacings[2]<<endl;
						cout<<"offset_y: "<<offset_y<<endl;
						e.value = (yh - yl) + spacings[2] + offset_y; 
						cout<<"add overlap edge for well: "<<e<<endl;
						edges.push_back(e);
						break;
					}
				} else {
					assert(1); // error here
				}
			}
			//}}}
		} else {
			cout<<"well orig:\t"<<bg::dsv(orig)<<endl;
			cout<<"well rect:\t"<<bg::dsv(rect)<<endl;
			cout<<"device rect:\t"<<bg::dsv(drect)<<endl;
			// no overlap
			if(dxh <= xl) {
				dEdge e(device_id, well_id, 1);
				// spacing_r
				cout<<"w: "<<w<<endl;
				cout<<"spacing: "<<spacings[1]<<endl;
				cout<<"offset_x: "<<offset_x<<endl;
				e.value = w + spacings[1] - offset_x; 
				cout<<"add no overlap edge for well: "<<e<<endl;
				edges.push_back(e);
			}
			if(xh <= dxl) {
				dEdge e(well_id, device_id, 1);
				// spacing_l
				cout<<"xh-xl: "<<xh-xl<<endl;
				cout<<"spacing: "<<spacings[0]<<endl;
				cout<<"offset_x: "<<offset_x<<endl;
				e.value = (xh - xl) + spacings[0] + offset_x; 
				cout<<"add no overlap edge for well: "<<e<<endl;
				edges.push_back(e);
			}
			if(dyh <= yl) {
				dEdge e(device_id, well_id, 0);
				// spacing_t
				cout<<"h: "<<h<<endl;
				cout<<"spacing: "<<spacings[3]<<endl;
				cout<<"offset_y: "<<offset_y<<endl;
				e.value = h + spacings[3] - offset_y; 
				cout<<"add no overlap edge for well: "<<e<<endl;
				edges.push_back(e);
			}
			if(yh <= dyl) {
				dEdge e(well_id, device_id, 0);
				// spacing_b
				cout<<"yh-yl: "<<yh-yl<<endl;
				cout<<"spacing: "<<spacings[2]<<endl;
				cout<<"offset_y: "<<offset_y<<endl;
				e.value = (yh - yl) + spacings[2] + offset_y; 
				cout<<"add no overlap edge for well: "<<e<<endl;
				edges.push_back(e);
			}
		}
	}
	
}
//}}}
*/

void Legalize::get_well_edges(vector< dEdge >& edges, polygon_90_t const& p, point_t const& orig, size_t well_id)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	using namespace boost::polygon::operators;
	polygon_90_set_t pset;
	pset.insert(p);
	vector< rectangle_t > rects;
	gtl::get_rectangles(rects, pset);

	// int orig_x = gtl::x(orig);
	// int orig_y = gtl::y(orig);

	for(size_t i=0; i<devices.size(); ++i) {
		Device<data_type> d = devices[i];
		if(d.type == 1) continue;
		int w = d.size.w(), h = d.size.h();
		rectangle_t drect(d.loc.x(), d.loc.y(), d.loc.x()+w, d.loc.y()+h);
	    vector<int> spacings;
	    get_spacings(spacings, i);
		int dxl = gtl::xl(drect), dyl = gtl::yl(drect), dxh = gtl::xh(drect), dyh = gtl::yh(drect);
		pset = p & drect;
		if(!pset.empty()) {
			// overlaps
			// get_overlap_well_edge(drect, rects, well_id, i, p, edges, orig, spacings);
			get_overlap_well_edge_new(drect, rects, well_id, i, p, edges, orig, spacings);
		} else {
			// no overlap
			//{{{
	        for(rectangle_t rect : rects) {
	        	int xl = gtl::xl(rect), yl = gtl::yl(rect), xh = gtl::xh(rect), yh = gtl::yh(rect);
		        int offset_x = xl - gtl::x(orig); 
		        int offset_y = yl - gtl::y(orig); 
                M_DEBUG(
				cout<<"well orig:\t"<<bg::dsv(orig)<<endl;
				cout<<"well rect:\t"<<bg::dsv(rect)<<endl;
				cout<<"device rect:\t"<<bg::dsv(drect)<<endl;
                );
				// select the direction with maximum distance away
				int max_dist = std::max({xl-dxh, dxl-xh, yl-dyh, dyl-yh});
			    // if(dxh <= xl) {
			    if(max_dist == (xl - dxh)) {
			    	dEdge e(i, well_id, 1);
			    	// spacing_r
			    	e.value = w + spacings[1] - offset_x; 
                    M_DEBUG(
					cout<<"w: "<<w<<endl;
					cout<<"spacing: "<<spacings[1]<<endl;
					cout<<"offset_x: "<<offset_x<<endl;
					cout<<"add no overlap edge for well: "<<e<<endl;
                    );
			    	edges.push_back(e);
			    // } else if(xh <= dxl) {
			    } else if(max_dist == (dxl - xh)) {
			    	dEdge e(well_id, i, 1);
			    	// spacing_l
			    	e.value = (xh - xl) + spacings[0] + offset_x; 
                    M_DEBUG(
					cout<<"xh-xl: "<<xh-xl<<endl;
					cout<<"spacing: "<<spacings[0]<<endl;
					cout<<"offset_x: "<<offset_x<<endl;
					cout<<"add no overlap edge for well: "<<e<<endl;
                    );
			    	edges.push_back(e);
			    // } else if(dyh <= yl) {
			    } else if(max_dist == (yl - dyh)) {
			    	dEdge e(i, well_id, 0);
			    	// spacing_t
			    	e.value = h + spacings[3] - offset_y; 
                    M_DEBUG(
					cout<<"h: "<<h<<endl;
					cout<<"spacing: "<<spacings[3]<<endl;
					cout<<"offset_y: "<<offset_y<<endl;
					cout<<"add no overlap edge for well: "<<e<<endl;
                    );
			    	edges.push_back(e);
			    // } else if(yh <= dyl) {
			    } else if(max_dist == (dyl - yh)) {
			    	dEdge e(well_id, i, 0);
			    	// spacing_b
			    	e.value = (yh - yl) + spacings[2] + offset_y; 
                    M_DEBUG(
					cout<<"yh-yl: "<<yh-yl<<endl;
					cout<<"spacing: "<<spacings[2]<<endl;
					cout<<"offset_y: "<<offset_y<<endl;
					cout<<"add no overlap edge for well: "<<e<<endl;
                    );
			    	edges.push_back(e);
			    }
	        }
			//}}}
		}
	}

}
//}}}

void Legalize::get_spacings_between(vector<int>& spacings, size_t d1, size_t d2)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
	cout<<"get_spacings_between device 1: "<<devices[d1].name<<" and device 2: "<<devices[d2].name<<endl;
	cout<<"check devices layers: "<<endl;
	cout<<devices[d1].name<<" layers: ";
	for(LayerShape lshape : devices[d1].layers_shapes) {
		cout<<lshape.layer<<", ";
	}
	cout<<endl;
	cout<<devices[d2].name<<" layers: ";
	for(LayerShape lshape : devices[d2].layers_shapes) {
		cout<<lshape.layer<<", ";
	}
	cout<<endl;
    );
	int w1 = devices[d1].size.w(), h1 = devices[d1].size.h();
	int w2 = devices[d2].size.w(), h2 = devices[d2].size.h();
	int spacing_l = 0, spacing_r = 0, spacing_b = 0, spacing_t = 0;
	for(auto lshape1 : devices[d1].layers_shapes) {
		if(layer_num2id.find(lshape1.layer) == layer_num2id.end()) {
			M_DEBUG(
			cout<<"Unsuported layer!"<<endl;
			);
			continue;
		}
		string layer_name = layers[layer_num2id.at(lshape1.layer)].second;
        M_DEBUG(
		cout<<"lshape1.layer: "<<lshape1.layer<<endl;
		cout<<"lshape1 layer name: "<<layer_name<<endl;
        );
		int xl1 = gtl::xl(lshape1.shape);
		int yl1 = gtl::yl(lshape1.shape);
		int xh1 = gtl::xh(lshape1.shape);
		int yh1 = gtl::yh(lshape1.shape);
		int xh_dist1 = w1 - xh1;
		int yh_dist1 = h1 - yh1;
		for(auto lshape2 : devices[d2].layers_shapes) {
			if(layer_num2id.find(lshape2.layer) == layer_num2id.end()) {
				M_DEBUG(
				cout<<"Unsuported layer!"<<endl;
				);
				continue;
			}
			if(lshape1.layer == lshape2.layer && layer_name.at(0) != 'M') continue;
            M_DEBUG(
			cout<<"lshape1.layer: "<<layers[layer_num2id.at(lshape1.layer)].second<<", lshape2.layer: "<<layers[layer_num2id.at(lshape2.layer)].second<<endl;
            );
			int xl2 = gtl::xl(lshape2.shape);
			int yl2 = gtl::yl(lshape2.shape);
			int xh2 = gtl::xh(lshape2.shape);
			int yh2 = gtl::yh(lshape2.shape);
			int xh_dist2 = w2 - xh2;
			int yh_dist2 = h2 - yh2;
			
			int rule = spacing_rules[layer_num2id.at(lshape1.layer)][layer_num2id.at(lshape2.layer)]; 
			if(rule != -1) {
                M_DEBUG(
				cout<<"spacings before: "<<spacing_l<<", "<<spacing_r<<", "<<spacing_b<<", "<<spacing_t<<endl;
				cout<<"rule: "<<rule<<endl;
				cout<<"xl1, yl1: "<<xl1<<", "<<yl1<<endl;
				cout<<"xl2, yl2: "<<xl2<<", "<<yl2<<endl;
				cout<<"xh_dist1, yh_dist1: "<<xh_dist1<<", "<<yh_dist1<<endl;
				cout<<"xh_dist2, yh_dist2: "<<xh_dist2<<", "<<yh_dist2<<endl;
                );
				spacing_l = std::max(spacing_l, rule - xl2 - xh_dist1); // d1 on the left of d2
				spacing_r = std::max(spacing_r, rule - xl1 - xh_dist2); // d1 on the right of d2
				spacing_b = std::max(spacing_b, rule - yl2 - yh_dist1); // d1 to the bottom of d2
				spacing_t = std::max(spacing_t, rule - yl1 - yh_dist2);
                M_DEBUG(
				cout<<"spacings after: "<<spacing_l<<", "<<spacing_r<<", "<<spacing_b<<", "<<spacing_t<<endl;
                );
			}
		} // for d2
	} // for d1
	spacings = {spacing_l, spacing_r, spacing_b, spacing_t};
	// spacings = {0, 0, 0, 0};
}
//}}}

void Legalize::get_device_edges(vector< dEdge >& edges)
//{{{;
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	// only between the devices without NW
	for(size_t i=0; i<devices.size(); ++i) {
		if(devices[i].type == 1) continue;
		Device<int> di = devices[i];
		for(size_t j=i+1; j<devices.size(); ++j) {
			if(devices[j].type == 1) continue;
			Device<int> dj = devices[j];
			vector<int> spacings;
			get_spacings_between(spacings, i, j);
			// no overlap between these devices
			// i to the left of j
			if(di.loc.x() + di.size.w() <= dj.loc.x()) {
				dEdge e(i, j, 1);
				e.value = di.size.w() + spacings[0]; // spacing_l
				edges.push_back(e);
			}
			if(dj.loc.x() + dj.size.w() <= di.loc.x()) {
				dEdge e(j, i, 1);
				e.value = dj.size.w() + spacings[1]; // spacing_r 
				edges.push_back(e);
			}
			// i to the bottom of j
			if(di.loc.y() + di.size.h() <= dj.loc.y()) {
				dEdge e(i, j, 0);
				e.value = di.size.h() + spacings[2]; // spacing_b
				edges.push_back(e);
			}
			if(dj.loc.y() + dj.size.h() <= di.loc.y()) {
				dEdge e(j, i, 0);
				e.value = dj.size.h() + spacings[3]; // spacing_t
				edges.push_back(e);
			}
		} // for j
	} // for i
}
//}}}

void Legalize::read_pts_from_file(vector< point_t >& pts, string const& file_name)
//{{{;
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ifstream in_file;
	in_file.open(file_name);

	double x, y;
	while(in_file >> x >> y) {
		pts.push_back( point_t(x, y) );
	}
	in_file.close();
}
//}}}

void Legalize::read_techfile_simple()
//{{{;
{
	ifstream in_simple_techfile;
	in_simple_techfile.open(in_simple_techfile_name);
	
	string layer_name;
	int layer_num;
	while(in_simple_techfile >> layer_name >> layer_num) {
		layer_num2id[layer_num] = layers.size();
		layers.push_back(make_pair(layer_num, layer_name));
	}
	
	in_simple_techfile.close();
	M_DEBUG(
	cout<<"finish reading techfile.simple"<<endl;
	);
}
//}}}

void Legalize::read_spacing_rules_file()
//{{{;
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ifstream in_spacing_rules_file;
	in_spacing_rules_file.open(in_spacing_rules_file_name);

	int spacing;
	size_t num = layers.size();
	for(size_t i=0; i<num; ++i) {
		spacing_rules.push_back( vector<int>() );
		for(size_t j=0; j<num; ++j) {
			in_spacing_rules_file >> spacing; 
			spacing_rules[i].push_back( spacing ); 
		}
	}

	in_spacing_rules_file.close();
	M_DEBUG(
	cout<<"finish reading spacing rules file"<<endl;
	);
}
//}}}

void Legalize::read_enclosure_rules_file()
//{{{ 
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ifstream in_enclosure_rules_file;
	in_enclosure_rules_file.open(in_enclosure_rules_file_name);

	int enclosure;
	size_t num = layers.size();
	for(size_t i=0; i<num; ++i) {
		enclosure_rules.push_back( vector<int>() );
		for(size_t j=0; j<num; ++j) {
			in_enclosure_rules_file >> enclosure; 
			enclosure_rules[i].push_back( enclosure ); 
		}
	}

	in_enclosure_rules_file.close();
	M_DEBUG(
	cout<<"finish reading enclosure rules file"<<endl;
	);
}
//}}}

void Legalize::read_symmetry_file()
//{{{ 
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	ifstream in_symmetry_file;
	in_symmetry_file.open(in_symmetry_file_name);

	std::string anoynmouse;
	symmetric_pairs_t sym_pairs;
	self_symmetric_t self_sym;
	while(std::getanoynmouse(in_symmetry_file, anoynmouse)) {
	    std::istringstream iss(anoynmouse);
		string a, b;
		if(iss >> a) {
			if(iss >> b) {
				sym_pairs.insert(pair<size_t, size_t>(m_vn_ids[a], m_vn_ids[b]));
			}
			else {
				self_sym.insert(m_vn_ids[a]);
			}
		}
		else {
			if(!(sym_pairs.empty() && self_sym.empty())) {
				symmetric_group_t sym_group = symmetric_group_t(sym_pairs, self_sym);
				symmetric_groups.push_back(sym_group);
				sym_pairs.clear();
				self_sym.clear();
			}
		}
	}
	if(!(sym_pairs.empty() && self_sym.empty())) {
		symmetric_group_t sym_group = symmetric_group_t(sym_pairs, self_sym);
		symmetric_groups.push_back(sym_group);
	}
	in_symmetry_file.close();
	// check that the symmetric pairs must have same shape
	for(size_t i=0; i<symmetric_groups.size(); ++i) {
		for(symmetric_pairs_t::iterator it=symmetric_groups[i].first.begin(); it!=symmetric_groups[i].first.end(); ++it) {
			cout<<"Symmetric pair: "<<devices[it->first].name<<", "<<devices[it->second].name<<endl;
            limboAssertMsg(devices[it->first].size.w() == devices[it->second].size.w() && devices[it->first].size.h() == devices[it->second].size.h(), "symmetric pairs size different error"); 
		}
	}
	
	M_DEBUG(
	cout<<"finish reading symmetry file"<<endl;
	);
}
//}}}

void Legalize::init_file_names()
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	in_offset_file_name = benchmark_dir+"/"+ckt_name+"/"+ckt_name+".offset"; 
	in_gds_file_prefix = benchmark_dir+"/"+ckt_name+"/gds/";
	in_symmetry_file_name = benchmark_dir+"/"+ckt_name+"/"+ckt_name+".sym";
	// in_ll_ur_file_name = ckt_name+".llur";
	// in_simple_techfile_name = tech_dir+"/techfile.simple";
	// in_spacing_rules_file_name = tech_dir+"/spacing.rules";
	// in_enclosure_rules_file_name = tech_dir+"/enclosure.rules";
	contour_file_name = benchmark_dir+"/"+ckt_name+"/"+ckt_name+".contours.approx";
	contour_offset_file_name = benchmark_dir+"/"+ckt_name+"/"+ckt_name+".gds2image.offset";
	// loc_file_name = placement_result_dir+"/result_legal_detail.txt";
	// FIXME: DEBUG
	// gds_ll = Loc<data_type>(0, 0);
	// currently hardcode
	// in_well_contact_gds_name = wcon_gds_dir+"/M1_NW_x2.gds";

    cout << "in_offset_file_name = " << in_offset_file_name << "\n"; 
    cout << "in_gds_file_prefix = " << in_gds_file_prefix << "\n"; 
    cout << "in_symmetry_file_name = " << in_symmetry_file_name << "\n"; 
    cout << "contour_file_name = " << contour_file_name << "\n"; 
    cout << "contour_offset_file_name = " << contour_offset_file_name << "\n"; 
	
	/*
	if(ckt_name == "COMPARATOR_PRE_AMP_test_flow") {
		in_well_contact_gds_name = wcon_gds_dir+"/M1_NW_x1.gds";
	} else if(ckt_name == "Core_test_flow") {
		in_well_contact_gds_name = wcon_gds_dir+"/M1_NW_x2.gds";
	} else {
		// default
		in_well_contact_gds_name = wcon_gds_dir+"/M1_NW_x1.gds";
	}
	*/
	// well_contact_size = Loc<data_type>(280, 280);
}
//}}}

GdsParser::GdsDB::GdsCell&
Legalize::write_gds_cell(GdsParser::GdsDB::GdsDB& db, vector<int> const& x_values, vector<int> const& y_values)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	
	db.cells().clear();
	for(size_t i=0; i<devices.size(); ++i) {
		db.addCell(*devices[i].gds_cell);
		M_DEBUG(
		cout<<"cell name: "<<devices[i].gds_cell->name()<<", module name: "<<devices[i].name<<endl;
		);
	}
	GdsCell& cell = db.addCell(ckt_name);
	for(size_t i=0; i<devices.size(); ++i) {
		point_type pt ( x_values[i]-devices[i].offset.x(), y_values[i]-devices[i].offset.y() );
		cell.addCellReference(devices[i].name, pt, 0.0, std::numeric_limits<double>::max(), 0);
	}
	
	for(size_t i=0; i<devices.size(); ++i) {
		delete devices[i].gds_cell;
	}
	return cell;
}
//}}}

void Legalize::write_gds_well(GdsCell& cell, vector< polygon_90_t > const& contours)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	int nw_layer = 0;
	for(auto num_name_pair : layers) {
		if(num_name_pair.second == "NW") {
			nw_layer = num_name_pair.first;
			break;
		}
	}
	int datatype = 0;
	for(auto contour : contours) {
		vector< point_type > vPoint;
		for(auto it=contour.begin(); it!=contour.end(); ++it) {
			vPoint.emplace_back(gtl::x(*it), gtl::y(*it));
		}
		cell.addPolygon(nw_layer, datatype, vPoint);
	}
}
//}}}

void Legalize::write_gds_well_contact(GdsParser::GdsDB::GdsDB& db, GdsCell& cell, vector< Loc<int> > const& wcontacts)
//{{{
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
	db.addCell(*wcon_gds_cell);
	for(size_t i=0; i<wcontacts.size(); ++i) {
		// store result to location vector
		point_type pt( wcontacts[i].x(), wcontacts[i].y() );
		cell.addCellReference(wcon_gds_cell->name(), pt, 0.0, std::numeric_limits<double>::max(), 0);
	}
	delete wcon_gds_cell;
}
//}}}


void Legalize::rec2poly(std::vector<point_type> &poly, std::vector<point_type> &rec)
{
    poly.clear();
    poly.emplace_back(rec[0].x(), rec[0].y());
    poly.emplace_back(rec[1].x(), rec[0].y());
    poly.emplace_back(rec[1].x(), rec[1].y());
    poly.emplace_back(rec[0].x(), rec[1].y());
}

void Legalize::write_gds_guard_ring(GdsParser::GdsDB::GdsDB &db, GdsCell& cell, string out_file_name)
{
    M_DEBUG(
    cout<<typeid(*this).name()<<"::"<<__func__<<endl;
    );
    GdsCell fcell;
    fcell = db.extractCell(cell.name());
    std::vector<std::vector<point_type>> GR_shape;
    std::vector<int> boundary;
    int int_min = std::numeric_limits<int>::min();
    int int_max = std::numeric_limits<int>::max();
    boundary.clear();
    boundary.push_back(int_max);
    boundary.push_back(int_max);
    boundary.push_back(int_min);
    boundary.push_back(int_min);
    // Getting Boundary Shape
    // !!!!!! Do I need to flatten cell here?
	for(vector<GdsCell::object_entry_type>::const_iterator oit = fcell.objects().begin(); oit != fcell.objects().end(); ++oit) {
		if(oit->first == ::GdsParser::GdsRecords::BOUNDARY) {
			GdsPolygon* ptr = dynamic_cast<GdsPolygon*>(oit->second); 
			polygon_type::iterator_type pit;
			for(pit=ptr->begin(); pit!=ptr->end(); ++pit) {
		        boundary[0] = std::min(boundary[0], pit->x());
		        boundary[1] = std::min(boundary[1], pit->y());
		        boundary[2] = std::max(boundary[2], pit->x());
		        boundary[3] = std::max(boundary[3], pit->y());
			}
		} 
	} 
    int M1_W, CO_W, EN_PP_OD, SP_CO, GRID;
    int layer_M1, layer_PP, layer_CO, layer_OD;
    GRID = 5; //Manufactoring grid size
    // Could replace if have layer mapping
    layer_M1 = 31;
    layer_PP = 25;
    layer_CO = 30;
    layer_OD = 6;
    CO_W = 60; // This is rule width of CO.
    SP_CO = 80; // This is CO-CO min spacing rule.
    // The following are not actually following minimum requirement of techfile but some predetermined rule of foundary.
    M1_W = 170; // M1 and OD path width.
    EN_PP_OD = 25; // Enclosure of PP and OD. (This is larger than min. requirement.)
    boundary[0] = boundary[0] - 2*SP_CO; // Some spacing
    boundary[1] = boundary[1] - 2*SP_CO;
    boundary[2] = boundary[2] + 2*SP_CO;
    boundary[3] = boundary[3] + 2*SP_CO;
    int offset = EN_PP_OD + M1_W;
    int offset_c = (M1_W - CO_W)/2;
    std::vector<point_type> poly, rec, rec_c, rec_c2;
    int con_num, con_sp, con_offset;
    // Part 1: Bottom Horizontal Shape
    // M1 and OD
    point_type rec_ll = point_type(boundary[0]-offset, boundary[1]-offset);
    point_type rec_ur = point_type(boundary[2]+offset, rec_ll.y()+M1_W);
    rec = {rec_ll, rec_ur};
    rec_c = rec;
    rec_c2 = rec;
    rec2poly(poly, rec);
    cell.addPolygon(layer_M1, 0, poly);
    cell.addPolygon(layer_OD, 0, poly);
    GR_shape.push_back(poly);
    rec_c2[0] = point_type(rec[0].x()-EN_PP_OD, rec[0].y()-EN_PP_OD);
    rec_c2[1] = point_type(rec[1].x()+EN_PP_OD, rec[1].y()+EN_PP_OD);
    rec2poly(poly,rec_c2);
    cell.addPolygon(layer_PP, 0, poly);
    // CO corners
    rec_c[0] = point_type(rec_c[0].x()+offset_c, rec_c[0].y()+offset_c);
    rec_c[1] = point_type(rec_c[0].x()+CO_W, rec_c[0].y()+CO_W);
    rec2poly(poly, rec_c);
    cell.addPolygon(layer_CO, 0, poly);
    rec_c[0] = point_type(rec_c[0].x(), boundary[3]+offset-offset_c);
    rec_c[1] = point_type(rec_c[1].x(), rec_c[0].y()-CO_W);
    rec2poly(poly, rec_c);
    cell.addPolygon(layer_CO, 0, poly);
    rec_c[1] = point_type(rec[1].x()-offset_c, rec[1].y()-offset_c);
    rec_c[0] = point_type(rec_c[1].x()-CO_W, rec_c[1].y()-CO_W);
    rec2poly(poly, rec_c);
    cell.addPolygon(layer_CO, 0, poly);
    rec_c[0] = point_type(rec_c[0].x(), boundary[3]+offset-offset_c);
    rec_c[1] = point_type(rec_c[1].x(), rec_c[0].y()-CO_W);
    rec2poly(poly, rec_c);
    cell.addPolygon(layer_CO, 0, poly);
    // CO ARRAY: top and bottom
    con_num = (rec[1].x() - rec[0].x() - 2*offset_c -2*CO_W - SP_CO)/(SP_CO+CO_W);
    con_sp = (rec[1].x() - rec[0].x() - 2*offset_c -2*CO_W -2*SP_CO - CO_W*con_num)/(con_num-1);
    con_sp = (con_sp/GRID)*GRID;
    con_offset = (rec[1].x() - rec[0].x() - 2*offset_c -2*CO_W - con_num*CO_W - (con_num-1)*con_sp)/2;
    rec_c[0] = point_type(rec[0].x()+offset_c+CO_W+con_offset, rec[0].y()+offset_c);
    rec_c[1] = point_type(rec_c[0].x()+CO_W, rec_c[0].y()+CO_W);
    rec_c2[0] = point_type(rec_c[0].x(), boundary[3]+offset-offset_c-CO_W);
    rec_c2[1] = point_type(rec_c[1].x(), boundary[3]+offset-offset_c);
    for (int i=0; i<con_num; i++)
    {
        rec2poly(poly, rec_c);
        cell.addPolygon(layer_CO, 0, poly);
        rec2poly(poly, rec_c2);
        cell.addPolygon(layer_CO, 0, poly);
        rec_c[0] = point_type(rec_c[0].x()+con_sp+CO_W, rec_c[0].y());
        rec_c[1] = point_type(rec_c[1].x()+con_sp+CO_W, rec_c[1].y());
        rec_c2[0] = point_type(rec_c[0].x(), boundary[3]+offset-offset_c-CO_W);
        rec_c2[1] = point_type(rec_c[1].x(), boundary[3]+offset-offset_c);
    }
    // Part 2: Left Vertical Shape
    rec[1] = point_type(rec_ll.x()+M1_W, boundary[3]+offset);
    rec2poly(poly, rec);
    cell.addPolygon(layer_M1, 0, poly);
    cell.addPolygon(layer_OD, 0, poly);
    GR_shape.push_back(poly);
    rec_c2[0] = point_type(rec[0].x()-EN_PP_OD, rec[0].y()-EN_PP_OD);
    rec_c2[1] = point_type(rec[1].x()+EN_PP_OD, rec[1].y()+EN_PP_OD);
    rec2poly(poly,rec_c2);
    cell.addPolygon(layer_PP, 0, poly);
    // CO ARRAY: left and right
    con_num = (rec[1].y() - rec[0].y() - 2*offset_c -2*CO_W - SP_CO)/(SP_CO+CO_W);
    con_sp = (rec[1].y() - rec[0].y() - 2*offset_c -2*CO_W -2*SP_CO - CO_W*con_num)/(con_num-1);
    con_sp = (con_sp/GRID)*GRID;
    con_offset = (rec[1].y() - rec[0].y() - 2*offset_c -2*CO_W - con_num*CO_W - (con_num-1)*con_sp)/2;
    rec_c[0] = point_type(rec[0].x()+offset_c, rec[0].y()+offset_c+CO_W+con_offset);
    rec_c[1] = point_type(rec_c[0].x()+CO_W, rec_c[0].y()+CO_W);
    rec_c2[0] = point_type(boundary[2]+offset-offset_c-CO_W, rec_c[0].y());
    rec_c2[1] = point_type(boundary[2]+offset-offset_c, rec_c[1].y());
    for (int i=0; i<con_num; i++)
    {
        rec2poly(poly, rec_c);
        cell.addPolygon(layer_CO, 0, poly);
        rec2poly(poly, rec_c2);
        cell.addPolygon(layer_CO, 0, poly);
        rec_c[0] = point_type(rec_c[0].x(), rec_c[0].y()+con_sp+CO_W);
        rec_c[1] = point_type(rec_c[1].x(), rec_c[1].y()+con_sp+CO_W);
        rec_c2[0] = point_type(boundary[2]+offset-offset_c-CO_W, rec_c[0].y());
        rec_c2[1] = point_type(boundary[2]+offset-offset_c, rec_c[1].y());
    }
    // Part 3: Top Horizontal Shape
    rec[0] = point_type(rec[1].x()-M1_W, rec[1].y()-M1_W);
    rec[1] = point_type(boundary[2]+offset, rec[1].y());
    rec2poly(poly, rec);
    cell.addPolygon(layer_M1, 0, poly);
    cell.addPolygon(layer_OD, 0, poly);
    GR_shape.push_back(poly);
    rec_c2[0] = point_type(rec[0].x()-EN_PP_OD, rec[0].y()-EN_PP_OD);
    rec_c2[1] = point_type(rec[1].x()+EN_PP_OD, rec[1].y()+EN_PP_OD);
    rec2poly(poly,rec_c2);
    cell.addPolygon(layer_PP, 0, poly);
    // Part 4: Right Vertical Shape
    rec[0] = point_type(rec[1].x()-M1_W, boundary[1]-offset);
    rec2poly(poly, rec);
    cell.addPolygon(layer_M1, 0, poly);
    cell.addPolygon(layer_OD, 0, poly);
    GR_shape.push_back(poly);
    rec_c2[0] = point_type(rec[0].x()-EN_PP_OD, rec[0].y()-EN_PP_OD);
    rec_c2[1] = point_type(rec[1].x()+EN_PP_OD, rec[1].y()+EN_PP_OD);
    rec2poly(poly,rec_c2);
    cell.addPolygon(layer_PP, 0, poly);
    // Writing output file
    std::ofstream out_file;
    out_file.open(out_file_name);
    // Below if for output 4 rectangle
//    for(std::vector<point_type> &rec_poly : GR_shape)
//    {
//        out_file<<"1 ";
//        for (point_type pt : rec_poly)
//        {
//            out_file<<pt.x()<<" ";
//            out_file<<pt.y()<<" ";
//        }
//        out_file<<"\n";
//    }
//    out_file.close();
    // Below is for 10 point polygon
    std::vector<point_type> out_poly;
    out_poly.push_back(GR_shape[0][0]);
    out_poly.push_back(GR_shape[0][1]);
    out_poly.push_back(GR_shape[3][2]);
    out_poly.push_back(GR_shape[2][3]);
    out_poly.push_back(GR_shape[0][3]);
    out_poly.emplace_back(GR_shape[1][1].x(), GR_shape[0][3].y());
    out_poly.emplace_back(GR_shape[1][1].x(), GR_shape[2][0].y());
    out_poly.emplace_back(GR_shape[3][0].x(), GR_shape[2][0].y());
    out_poly.emplace_back(GR_shape[3][0].x(), GR_shape[0][3].y());
    out_poly.push_back(GR_shape[0][3]);

    out_file<<"1 ";
    for(point_type pt : out_poly)
    {
        out_file<<pt.x()<<" ";
        out_file<<pt.y()<<" ";
    }
}
//}}}


int main(int argc, char** argv) {
    // append options here 
    typedef limbo::programoptions::ProgramOptions po_type;
    using limbo::programoptions::Value;
    bool help = false;
    std::string ckt_name; 
    std::string benchmark_dir; 
    std::string placement_path; 
    std::string well_contact_gds; 
    std::string techfile_path; 
    std::string spacing_rules_path; 
    std::string enclosure_rules_path; 
    std::string optimizer_type_str = "GUROBI"; 
    OptimizerType optimizer_type = OptimizerType::GUROBI; 
    std::string result_dir; 
    po_type desc (std::string("Available options"));
    desc.add_option(Value<bool>("--help", &help, "print help message").default_value(help).help(true))
        .add_option(Value<std::string>("--circuit", &ckt_name, "circuit name"))
        .add_option(Value<std::string>("--benchmark_dir", &benchmark_dir, "input circuit directory"))
        .add_option(Value<std::string>("--placement", &placement_path, "input placement solution"))
        .add_option(Value<std::string>("--well_contact_gds", &well_contact_gds, "input well contact GDSII"))
        .add_option(Value<std::string>("--techfile", &techfile_path, "input technology file"))
        .add_option(Value<std::string>("--spacing", &spacing_rules_path, "input spacing rule file"))
        .add_option(Value<std::string>("--enclosure", &enclosure_rules_path, "input enclosure rule file"))
        .add_option(Value<std::string>("--optimizer", &optimizer_type_str, "optimization solver GUROBI|LPSOLVE").default_value(optimizer_type_str))
        .add_option(Value<std::string>("--result", &result_dir, "output result directory"))
        ;

    try
    {
        desc.parse(argc, argv);

        // print help message 
        if (help)
        {
            cout << desc << "\n";
            exit(1);
        }

        // post processing fileFormat
        if (limbo::iequals(optimizer_type_str, "GUROBI"))
            optimizer_type = OptimizerType::GUROBI; 
        else if (limbo::iequals(optimizer_type_str, "LPSOLVE"))
            optimizer_type = OptimizerType::LPSOLVE; 
		else 
		{
			assert(0);
		}
    }
    catch (std::exception& e)
    {
        // print help message and error message 
        cout << desc << "\n";
        cout << e.what() << "\n";
        return false;
    }

	Legalize* lg = new Legalize(ckt_name, optimizer_type);
	lg->result_dir = result_dir;
	lg->benchmark_dir = benchmark_dir;
	// lg->tech_dir = argv[4];
	// lg->placement_result_dir = argv[4];
	lg->loc_file_name = placement_path;
	// lg->wcon_gds_dir = argv[5];
	lg->in_well_contact_gds_name = well_contact_gds;
	lg->in_simple_techfile_name = techfile_path;
	lg->in_spacing_rules_file_name = spacing_rules_path;
	lg->in_enclosure_rules_file_name = enclosure_rules_path;

    cout << "ckt_name = " << ckt_name << "\n"; 
    cout << "benchmark_dir = " << lg->benchmark_dir << "\n"; 
    cout << "result_dir = " << lg->result_dir << "\n";
    cout << "loc_file_name = " << lg->loc_file_name << "\n";
    cout << "in_well_contact_gds_name = " << lg->in_well_contact_gds_name << "\n"; 
    cout << "in_simple_techfile_name = " << lg->in_simple_techfile_name << "\n"; 
    cout << "in_spacing_rules_file_name = " << lg->in_spacing_rules_file_name << "\n"; 
    cout << "in_enclosure_rules_file_name = " << lg->in_enclosure_rules_file_name << "\n"; 
    cout << "optimizer_type: " << optimizer_type_str << "\n"; 

	// lg->test_grid_func();
	lg->init_file_names();
	lg->legalize();
	delete lg;
}


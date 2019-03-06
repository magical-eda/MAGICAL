/*************************************************************************
    > File Name: nlp.cpp
    > Author: anoynmous anoynmous
    > Mail: anoynmous@anoynmous
    > Created Time: Fri 01 Jun 2018 05:01:54 PM CDT
 ************************************************************************/

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#ifdef USE_GUROBI
#include <limbo/solvers/api/GurobiApi.h>
#endif
#include <limbo/solvers/api/LPSolveApi.h>
#include <limbo/preprocessor/AssertMsg.h>

#include "nlp.h"

using std::ifstream;
using std::make_pair;

#define M_DEBUG(x) /* x */
#define HAS_FLEXIBILITY 0
#define INIT_GP_RESULT_FROM_FILE 0
#define INIT_DP_RESULT_FROM_FILE 0

// == static variables
vector<Vertex> NLPlace::vtcs;
vector<int> NLPlace::dtype;
NLPlace::m_en_vids_t NLPlace::m_en_vids;
vector< NLPlace::symmetric_group_t > NLPlace::symmetric_groups;
double NLPlace::lambda1;
double NLPlace::lambda2;
double NLPlace::lambda3;
double NLPlace::lambda4;
constexpr double NLPlace::alpha;
double NLPlace::ub_x;
double NLPlace::ub_y;

double log_sum_exp(double var1, double var2, double alpha) 
//{{{
{
	return alpha*log(exp(var1/alpha)+exp(var2/alpha)); 
}
//}}}

double log_sum_exp_0(double var, double alpha) 
//{{{
{
	// modify it to handle overflow issue
	// arg > 709.8 => overflow; arg < -709.8 => underflow
	if (var/alpha < 709.8) {
		return alpha*log(exp(var/alpha)+1); 
	}
	else {
		// -var + 32
		return alpha*log(exp(32/alpha)+exp((-var+32)/alpha)) + var - 32;
	}
}
//}}}

double log_sum_exp_vec(double *values, set<size_t> const& vids, double alpha) 
//{{{
{
	double xmax=0, xmin=0, ymax=0, ymin=0;
	for (size_t vid : vids) {
		double wi = NLPlace::vtcs[vid].scal_dimension.first;
		double hi = NLPlace::vtcs[vid].scal_dimension.second;
		xmax += exp((values[vid*2]+wi/2)/alpha); 
		xmin += exp(-(values[vid*2]+wi/2)/alpha); 
		ymax += exp((values[vid*2+1]+hi/2)/alpha); 
		ymin += exp(-(values[vid*2+1]+hi/2)/alpha); 
	}
	xmax = log(xmax);
	xmin = log(xmin);
	ymax = log(ymax);
	ymin = log(ymin);
	return alpha * (xmax+xmin+ymax+ymin);
}
//}}}

double grad_log_sum_exp(double var1, double var2, double alpha) 
//{{{
{
	return (exp(var1/alpha)-exp(var2/alpha))/(exp(var1/alpha)+exp(var2/alpha));
}
//}}}

double grad_log_sum_exp_0(double var, double alpha) 
//{{{
{
	return exp(var/alpha)/(exp(var/alpha)+1);
}
//}}}

double grad_log_sum_exp_vec(double *values, set<size_t> const& vids, double alpha, size_t cur_vid, bool is_x) 
//{{{
{
	double xmax=0, xmin=0, ymax=0, ymin=0;
	for (size_t vid : vids) {
		double wi = NLPlace::vtcs[vid].scal_dimension.first;
		double hi = NLPlace::vtcs[vid].scal_dimension.second;
		if (is_x) {
		    xmax += exp((values[vid*2]+wi/2)/alpha); 
		    xmin += exp(-(values[vid*2]+wi/2)/alpha); 
		}
		else {
		    ymax += exp((values[vid*2+1]+hi/2)/alpha); 
		    ymin += exp(-(values[vid*2+1]+hi/2)/alpha); 
		}
	}
	if (is_x) {
		double wi = NLPlace::vtcs[cur_vid].scal_dimension.first;
		xmax = exp((values[cur_vid*2]+wi/2)/alpha) / xmax; 
		xmin = exp(-(values[cur_vid*2]+wi/2)/alpha) / xmin;
		return (xmax-xmin);
	}
	else {
		double hi = NLPlace::vtcs[cur_vid].scal_dimension.second;
		ymax = exp((values[cur_vid*2+1]+hi/2)/alpha) / ymax; 
		ymin = exp(-(values[cur_vid*2+1]+hi/2)/alpha) / ymin; 
		return (ymax-ymin);
	}
}
//}}}

double obj_func(double *values)
//{{{
{
	size_t num = NLPlace::vtcs.size();

	/**** Below are parameters ****/
	double lambda1 = NLPlace::lambda1; // the coefficient for x and y overlap penalty
	double lambda2 = NLPlace::lambda2; // the coefficient for out of boundary penalty
	double lambda3 = NLPlace::lambda3; // the coefficient for wire length penalty
	double lambda4 = NLPlace::lambda4; // the coefficient for asymmetry penalty
	// alpha should be a very small value
	double alpha = NLPlace::alpha;
	// boundaries, assume lb_x, lb_y is 0
	double ub_x = NLPlace::ub_x;
	double ub_y = NLPlace::ub_y;

	double result = 0;
	// log-sum-exp model for overlap penalty
	for (size_t i=0; i<num; ++i) {
		double wi = NLPlace::vtcs[i].scal_dimension.first;
		double hi = NLPlace::vtcs[i].scal_dimension.second;
		for (size_t j=i+1; j<num; ++j) {
			// NOTE: add this for multi-layer flexibility, (0, 1) or (1, 0)
#if HAS_FLEXIBILITY == 1
			if (NLPlace::dtype[i] + NLPlace::dtype[j] == 1) { continue; }
#endif
		    double wj = NLPlace::vtcs[j].scal_dimension.first;
		    double hj = NLPlace::vtcs[j].scal_dimension.second;
			// values arrangement: x0, y0, x1, y1...
			// max( min(xi+wi-xj, xj+wj-xi), 0 )
			double var1 = values[2*i]+wi-values[2*j];
			double var2 = values[2*j]+wj-values[2*i];
			double ox = log_sum_exp(var1, var2, -alpha); // smoothed min
			ox = log_sum_exp_0(ox, alpha); // smoothed max with 0 
			// max( min(yi+hi-yj, yj+hj-yi), 0 )
			double var1_y = values[2*i+1]+hi-values[2*j+1];
			double var2_y = values[2*j+1]+hj-values[2*i+1];
			double oy = log_sum_exp(var1_y, var2_y, -alpha); // smoothed min
			oy = log_sum_exp_0(oy, alpha); // smoothed max with 0 
			result += lambda1 * ox * oy;
			M_DEBUG(
			cout<<"ox: "<<ox<<", oy: "<<oy<<", ox*oy: "<<ox*oy<<endl;
			cout<<"result: "<<result<<endl;
			);
		}
	}

	// out of boundary penalty
	for (size_t i=0; i<num; ++i) {
		double wi = NLPlace::vtcs[i].scal_dimension.first;
		double hi = NLPlace::vtcs[i].scal_dimension.second;
		// 0 <= x, x+w <= ub_x
		// max (-x, 0), max(x+w-ub_x, 0)
		double obx = log_sum_exp_0(-values[2*i], alpha);
		obx += log_sum_exp_0(values[2*i]+wi-ub_x, alpha);
		// 0 <= y, y+h <= ub_y
		// max (-y, 0), max(y+h-ub_y, 0)
		double oby = log_sum_exp_0(-values[2*i+1], alpha);
		oby += log_sum_exp_0(values[2*i+1]+hi-ub_y, alpha);
		result += lambda2 * (obx + oby);
		M_DEBUG(
		cout<<"i: "<<i<<endl;
		cout<<"-values[2*i]: "<<-values[2*i]<<endl;
		cout<<"values[2*i]+wi-ub_x: "<<values[2*i]+wi-ub_x<<endl;
		cout<<"-values[2*i+1]: "<<-values[2*i+1]<<endl;
		cout<<"values[2*i+1]+hi-ub_y: "<<values[2*i+1]+hi-ub_y<<endl;
		cout<<"obx: "<<obx<<", oby: "<<oby<<endl;
		cout<<"result: "<<result<<endl;
		);
	}

	// wire length penalty
	for (auto it=NLPlace::m_en_vids.begin(); it!=NLPlace::m_en_vids.end(); ++it) {
		result += lambda3 * log_sum_exp_vec(values, it->second, alpha);
	}
	
	// quadratic model for asymmetry penalty
	// FIXME: right now assume vertical symmetry axis
	double asym_penalty = 0;
	for (size_t i=0; i<NLPlace::symmetric_groups.size(); ++i) {
		auto sym_g = NLPlace::symmetric_groups[i];
		auto sps = sym_g.first; // symmetry pairs
		auto sss = sym_g.second; // self-symmetry
		for (auto sp : sps) {
			size_t id1=sp.first, id2=sp.second;
			double wi = NLPlace::vtcs[id1].scal_dimension.first;
			asym_penalty += pow(values[2*id1+1] - values[2*id2+1], 2.0); // (y1 - y2)^2
			asym_penalty += pow(values[2*id1]+values[2*id2]+wi-2*values[2*num+i], 2.0);	
		}
		for (auto ss : sss) {
			double wi = NLPlace::vtcs[ss].scal_dimension.first;
			asym_penalty += pow(values[2*ss]+wi/2-values[2*num+i], 2.0);
		}
	}
	result += lambda4 * asym_penalty;

	return result;
}
//}}}

double get_objectives(double *values)
//{{{
{
	size_t num = NLPlace::vtcs.size();

	/**** Below are parameters ****/
	double lambda1 = NLPlace::lambda1; // the coefficient for x and y overlap penalty
	double lambda2 = NLPlace::lambda2; // the coefficient for out of boundary penalty
	double lambda3 = NLPlace::lambda3; // the coefficient for wire length penalty
	double lambda4 = NLPlace::lambda4; // the coefficient for asymmetry penalty
	// alpha should be a very small value
	double alpha = NLPlace::alpha;
	// boundaries, assume lb_x, lb_y is 0
	double ub_x = NLPlace::ub_x;
	double ub_y = NLPlace::ub_y;

	double result = 0;
	// log-sum-exp model for overlap penalty
	for (size_t i=0; i<num; ++i) {
		double wi = NLPlace::vtcs[i].scal_dimension.first;
		double hi = NLPlace::vtcs[i].scal_dimension.second;
		for (size_t j=i+1; j<num; ++j) {
#if HAS_FLEXIBILITY == 1
			if (NLPlace::dtype[i] + NLPlace::dtype[j] == 1) { continue; }
#endif
		    double wj = NLPlace::vtcs[j].scal_dimension.first;
		    double hj = NLPlace::vtcs[j].scal_dimension.second;
			// values arrangement: x0, y0, x1, y1...
			// max( min(xi+wi-xj, xj+wj-xi), 0 )
			double var1 = values[2*i]+wi-values[2*j];
			double var2 = values[2*j]+wj-values[2*i];
			double ox = log_sum_exp(var1, var2, -alpha); // smoothed min
			ox = log_sum_exp_0(ox, alpha); // smoothed max with 0 
			// max( min(yi+hi-yj, yj+hj-yi), 0 )
			double var1_y = values[2*i+1]+hi-values[2*j+1];
			double var2_y = values[2*j+1]+hj-values[2*i+1];
			double oy = log_sum_exp(var1_y, var2_y, -alpha); // smoothed min
			oy = log_sum_exp_0(oy, alpha); // smoothed max with 0 
			result += lambda1 * ox * oy;
		}
	}

	M_DEBUG(
	double overlap_penalty = result;
	cout<<"overlap_penalty: "<<overlap_penalty<<endl;
	);

	// out of boundary penalty
	for (size_t i=0; i<num; ++i) {
		double wi = NLPlace::vtcs[i].scal_dimension.first;
		double hi = NLPlace::vtcs[i].scal_dimension.second;
		// 0 <= x <= ub_x
		// max (-x, 0), max(x+w-ub_x, 0)
		double obx = log_sum_exp_0(-values[2*i], alpha);
		obx += log_sum_exp_0(values[2*i]+wi-ub_x, alpha);
		// 0 <= y <= ub_y
		// max (-y, 0), max(y+h-ub_y, 0)
		double oby = log_sum_exp_0(-values[2*i+1], alpha);
		oby += log_sum_exp_0(values[2*i+1]+hi-ub_y, alpha);
		result += lambda2 * (obx + oby);
	}

	M_DEBUG(
	double oob_penalty = result - overlap_penalty;
	cout<<"oob_penalty: "<<oob_penalty<<endl;
	);

	// wire length penalty
	for (auto it=NLPlace::m_en_vids.begin(); it!=NLPlace::m_en_vids.end(); ++it) {
		result += lambda3 * log_sum_exp_vec(values, it->second, alpha);
	}
	
	M_DEBUG(
	double wl_penalty = result - overlap_penalty - oob_penalty;
	cout<<"wl_penalty: "<<wl_penalty<<endl;
	);
	
	// quadratic model for asymmetry penalty
	// FIXME: right now assume vertical symmetry axis
	double asym_penalty = 0;
	for (size_t i=0; i<NLPlace::symmetric_groups.size(); ++i) {
		auto sym_g = NLPlace::symmetric_groups[i];
		auto sps = sym_g.first; // symmetry pairs
		auto sss = sym_g.second; // self-symmetry
		for (auto sp : sps) {
			size_t id1=sp.first, id2=sp.second;
			double wi = NLPlace::vtcs[id1].scal_dimension.first;
			asym_penalty += pow(values[2*id1+1] - values[2*id2+1], 2.0); // (y1 - y2)^2
			asym_penalty += pow(values[2*id1]+values[2*id2]+wi-2*values[2*num+i], 2.0);	
		}
		for (auto ss : sss) {
			double wi = NLPlace::vtcs[ss].scal_dimension.first;
			asym_penalty += pow(values[2*ss]+wi/2-values[2*num+i], 2.0);
		}
	}
	result += lambda4 * asym_penalty;

	M_DEBUG(
	double asymmetry_penalty = result - overlap_penalty - oob_penalty - wl_penalty;
	cout<<"asymmetry_penalty: "<<asymmetry_penalty<<endl;
	);

	return result;
}
//}}}

void grad_func(double *grad, double *values)
//{{{
{
	size_t num = NLPlace::vtcs.size();

	/**** Below are parameters ****/
	double lambda1 = NLPlace::lambda1; // the coefficient for x and y overlap penalty
	double lambda2 = NLPlace::lambda2; // the coefficient for out of boundary penalty
	double lambda3 = NLPlace::lambda3; // the coefficient for wire length penalty
	double lambda4 = NLPlace::lambda4; // the coefficient for asymmetry penalty
	// alpha should be a very small value
	double alpha = NLPlace::alpha;
	// boundaries, assume lb_x, lb_y is 0
	double ub_x = NLPlace::ub_x;
	double ub_y = NLPlace::ub_y;

	M_DEBUG(
	cout<<"values: ";
	for (size_t i=0; i<num; ++i) {
		cout<<values[2*i]<<" ";
		cout<<values[2*i+1]<<" ";
	}
	for (size_t i=2*num; i<2*num+NLPlace::symmetric_groups.size(); ++i) {
		cout<<values[i]<<" ";
	}
	cout<<endl;
	);

	// log-sum-exp model for overlap penalty
	for (size_t i=0; i<num; ++i) {
		grad[2*i] = 0; // df/dx
		grad[2*i+1] = 0; // df/dy
		double wi = NLPlace::vtcs[i].scal_dimension.first;
		double hi = NLPlace::vtcs[i].scal_dimension.second;
		// partial gradient of x 
		for (size_t j=0; j<num; ++j) {
			if (j==i) continue;
#if HAS_FLEXIBILITY == 1
			if (NLPlace::dtype[i] + NLPlace::dtype[j] == 1) { continue; }
#endif
		    double wj = NLPlace::vtcs[j].scal_dimension.first;
		    double hj = NLPlace::vtcs[j].scal_dimension.second;
			// values arrangement: x0, y0, x1, y1...
			// max( min(xi+wi-xj, xj+wj-xi), 0 )
			double var1 = values[2*i]+wi-values[2*j];
			double var2 = values[2*j]+wj-values[2*i];
			double ox = log_sum_exp(var1, var2, -alpha); // smoothed min
			double grad_ox = grad_log_sum_exp_0(ox, alpha); // smoothed max with 0 
			// max( min(yi+hi-yj, yj+hj-yi), 0 )
			double var1_y = values[2*i+1]+hi-values[2*j+1];
			double var2_y = values[2*j+1]+hj-values[2*i+1];
			double oy = log_sum_exp(var1_y, var2_y, -alpha); // smoothed min
			oy = log_sum_exp_0(oy, alpha); // smoothed max with 0 
			grad[2*i] += lambda1 * oy * grad_ox * grad_log_sum_exp(var1, var2, -alpha);
		}
		// cout<<"grad xi: "<<grad[2*i]<<endl;
		// partial gradient of y 
		for (size_t j=0; j<num; ++j) {
			if (j==i) continue;
#if HAS_FLEXIBILITY == 1
			if (NLPlace::dtype[i] + NLPlace::dtype[j] == 1) { continue; }
#endif
		    double wj = NLPlace::vtcs[j].scal_dimension.first;
		    double hj = NLPlace::vtcs[j].scal_dimension.second;
			// values arrangement: x0, y0, x1, y1...
			// max( min(xi+wi-xj, xj+wj-xi), 0 )
			double var1 = values[2*i]+wi-values[2*j];
			double var2 = values[2*j]+wj-values[2*i];
			double ox = log_sum_exp(var1, var2, -alpha); // smoothed min
			ox = log_sum_exp_0(ox, alpha); // smoothed max with 0 
			// max( min(yi+hi-yj, yj+hj-yi), 0 )
			double var1_y = values[2*i+1]+hi-values[2*j+1];
			double var2_y = values[2*j+1]+hj-values[2*i+1];
			double oy = log_sum_exp(var1_y, var2_y, -alpha); // smoothed min
			double grad_oy = grad_log_sum_exp_0(oy, alpha); // smoothed max with 0 
			grad[2*i+1] += lambda1 * ox * grad_oy * grad_log_sum_exp(var1_y, var2_y, -alpha);
		}
		// cout<<"grad yi: "<<grad[2*i+1]<<endl;
	}

	// out of boundary penalty
	for (size_t i=0; i<num; ++i) {
		double wi = NLPlace::vtcs[i].scal_dimension.first;
		double hi = NLPlace::vtcs[i].scal_dimension.second;
		// 0 <= x <= ub_x
		// max (-x, 0), max(x+w-ub_x, 0)
		double grad_obx = -grad_log_sum_exp_0(-values[2*i], alpha);
		grad_obx += grad_log_sum_exp_0(values[2*i]+wi-ub_x, alpha);
		grad[2*i] += lambda2 * grad_obx;
		// 0 <= y <= ub_y
		// max (-y, 0), max(y+h-ub_y, 0)
		double grad_oby = -grad_log_sum_exp_0(-values[2*i+1], alpha);
		grad_oby += grad_log_sum_exp_0(values[2*i+1]+hi-ub_y, alpha);
		grad[2*i+1] += lambda2 * grad_oby;
		// cout<<"grad xi: "<<grad[2*i]<<endl;
		// cout<<"grad yi: "<<grad[2*i+1]<<endl;
	}

	// wire length penalty 
	for (auto it=NLPlace::m_en_vids.begin(); it!=NLPlace::m_en_vids.end(); ++it) {
		for (size_t i : it->second) {
			grad[2*i] += lambda3 * grad_log_sum_exp_vec(values, it->second, alpha, i, 1); // x coordinate
			grad[2*i+1] += lambda3 * grad_log_sum_exp_vec(values, it->second, alpha, i, 0); // y coordinate
		}
	}

	// quadratic model for asymmetry penalty
	// FIXME: right now assume vertical symmetry axis
	for (size_t i=2*num; i<2*num+NLPlace::symmetric_groups.size(); ++i) {
		   grad[i] = 0; // initialize gradient for auxiliary variable
	}
	for (size_t i=0; i<NLPlace::symmetric_groups.size(); ++i) {
		auto sym_g = NLPlace::symmetric_groups[i];
		auto sps = sym_g.first; // symmetry pairs
		auto sss = sym_g.second; // self-symmetry
		for (auto sp : sps) {
			size_t id1=sp.first;
			size_t id2=sp.second;
			double wi = NLPlace::vtcs[id1].scal_dimension.first;
			double grad_x = 2.0*(values[2*id1]+values[2*id2]+wi-2*values[2*num+i]);	
			grad[2*id1] += lambda4 * grad_x; 
			grad[2*id2] += lambda4 * grad_x; 
			double grad_y = 2.0*(values[2*id1+1] - values[2*id2+1]);
			grad[2*id1+1] += lambda4 * grad_y; 
			grad[2*id2+1] += lambda4 * (-grad_y); 
			grad[2*num+i] += lambda4 * (-2.0*grad_x); 
		}
		for (auto ss : sss) {
			double wi = NLPlace::vtcs[ss].scal_dimension.first;
			double grad_ss = 2.0*(values[2*ss]+wi/2-values[2*num+i]);
			grad[2*ss] += lambda4 * grad_ss;
			grad[2*num+i] += lambda4 * (-grad_ss);
		}
	}

	M_DEBUG(
	cout<<"grad: ";
	for (size_t i=0; i<num; ++i) {
		cout<<grad[2*i]<<" ";
		cout<<grad[2*i+1]<<" ";
	}
	for (size_t i=2*num; i<2*num+NLPlace::symmetric_groups.size(); ++i) {
		cout<<grad[i]<<" ";
	}
	cout<<endl;
	);
}
//}}}

// parameters to tune
void NLPlace::init_static_variables()
//{{{
{
	lambda1 = 1; // the coefficient for x and y overlap penalty
	lambda2 = 1; // the coefficient for out of boundary penalty
	lambda3 = 16; // the coefficient for wire length penalty
	lambda4 = 1; // the coefficient for asymmetry penalty

	// set ub_x and ub_y
	total_device_area = get_total_device_area(); // NOTE: this function depend on scal_dimension, so it should be executed after scale_vtcs_dimension
	M_DEBUG(
		cout<<"total_device_area: "<<total_device_area<<endl;
	);
	double max_tot_area = total_device_area * (1.0 + max_white_space);
	ub_x = std::sqrt(max_tot_area * aspect_ratio); 
	ub_y = max_tot_area / ub_x; 
	cout<<"ub_x, ub_y: "<<ub_x<<", "<<ub_y<<endl;
}
//}}}

void NLPlace::scale_vtcs_dimension()
//{{{
{
	// scale the dimension of the devices hoping to resolve the inf issue
	for (size_t i=0; i<vtcs.size(); ++i) {
		vtcs[i].scal_dimension.first = dim_scal_fac * vtcs[i].dimensions[0].first;
		vtcs[i].scal_dimension.second = dim_scal_fac * vtcs[i].dimensions[0].second;
	}
}
//}}}

void NLPlace::run()
//{{{
{
	read_params();
	init_file_names();
	read_netlist_files();
	scale_vtcs_dimension();
	init_static_variables();
#if INIT_GP_RESULT_FROM_FILE == 1
	init_locs_from_file(results_dir+"result.txt");
#else
	global_place();
#endif

#if INIT_DP_RESULT_FROM_FILE == 1
	init_locs_from_file(results_dir+"result_legal_detail.txt");
#else
	limbo::solvers::SolverProperty optimstatus = legal_detail_place();
	if(optimstatus == limbo::solvers::INFEASIBLE) {
		legal_detail_place(true);
	}
#endif

	// write out GDSII file
	write_gds();
}
//}}}

void NLPlace::init_locs_from_file(string file_name)
//{{{
{
	// assume same order as block file
	ifstream in_file;
	in_file.open(file_name);
    if (!in_file.good())
    {
        cout << "failed to open " << file_name << " for read\n";
        exit(-1);
    }

	string module_name;
	double x, y, w, h;
	locs.clear();
	while(in_file >> module_name >> x >> y >> w >> h) {
		// cout<<"module_name: "<<module_name<<", locs: ("<<x<<", "<<y<<")"<<endl;
		locs.push_back(make_pair(x, y));
	}
	in_file.close();
	M_DEBUG(
	cout<<"finish reading result from file"<<endl;
	);
}
//}}}

int NLPlace::global_place()
//{{{
{
    char buf[256]; 
    sprintf(buf, "%s takes %%t seconds CPU, %%w seconds real\n", __func__);
	boost::timer::auto_cpu_timer t (buf);

	M_DEBUG(
	cout<<__func__<<": write objective..."<<endl;
	);
	// write objective: h + alpha * hpwl
	int code;
	double val_min;
	double *solution_vect;
	// int len = 2*(int)vtcs.size();
	// 1 auxiliary variable per symmetry group
	int len = 2*(int)vtcs.size() + (int)symmetric_groups.size();
	solution_vect = (double*)malloc(sizeof(double)*len);
	for (int i=0; i<len; ++i) {
		solution_vect[i] = (double) (rand() % 20); // FIXME 
	}
	M_DEBUG(
	cout<<"solution_vect initial: ";
	for (int i=0; i<len; ++i) {
		cout<<solution_vect[i]<<", ";
	}
	cout<<endl;
	);
	// if symmetric pairs has same initial coordinate, make them different
	for (size_t i=0; i<symmetric_groups.size(); ++i) {
		auto sps = symmetric_groups[i].first; // symmetry pairs
		for (auto sp : sps) {
			size_t id1=sp.first, id2=sp.second;
			if (solution_vect[2*id1] == solution_vect[2*id2] && solution_vect[2*id1+1] == solution_vect[2*id2+1]) {
				solution_vect[2*id2] = solution_vect[2*id2] + 1;
				solution_vect[2*id2+1] = solution_vect[2*id2+1] + 1;
			}
		}
	}
	// if same shape pairs has same initial coordinate, make them different
	for (size_t i=0; i<vtcs.size(); ++i) {
		double wi = vtcs[i].scal_dimension.first;
		double hi = vtcs[i].scal_dimension.second;
		for (size_t j=i+1; j<vtcs.size(); ++j) {
		    double wj = vtcs[j].scal_dimension.first;
		    double hj = vtcs[j].scal_dimension.second;
			if (wi==wj && hi==hj) {
				if (solution_vect[2*i]==solution_vect[2*j] && solution_vect[2*i+1]==solution_vect[2*j+1]) {
					M_DEBUG(
					cout<<"initial solution_vect the same, increment"<<endl;
					);
					solution_vect[2*j] = solution_vect[2*j]+1;
					solution_vect[2*j+1] = solution_vect[2*j+1]+1;
				}
			}
		}
	}
	
	double *initial_coord_x0s;
	initial_coord_x0s = (double*)malloc(sizeof(double)*len);
	for (int i=0; i<len; ++i) {
		initial_coord_x0s[i] = 1.0; // FIXME
	}

	M_DEBUG(
	cout<<__func__<<": solve nlp..."<<endl;
	);
	// wn_conj_direction_method(&code, &val_min, solution_vect, initial_coord_x0s, len, obj_func, 5000);
	// wn_conj_gradient_method(&code, &val_min, solution_vect, len, obj_func, grad_func, 5000);
	
	// iteratively solve nlp, until total overlap is less than a certain threshold
    double cur_ovl_ratio = std::numeric_limits<double>::max(); // initial overlap ratio is 1 
    // double pre_ovl_ratio = std::numeric_limits<double>::max(); // previous overlap ratio 
	/*
	double total_device_area = get_total_device_area();
	M_DEBUG(
	cout<<"total_device_area: "<<total_device_area<<endl;
	);
	*/
	size_t num_iter = 0;
	size_t max_iter = 10;
	// size_t max_iter = 20;
	while (num_iter < max_iter) {
		wn_conj_gradient_method(&code, &val_min, solution_vect, len, obj_func, grad_func, 1000);
		// wn_conj_direction_method(&code, &val_min, solution_vect, initial_coord_x0s, len, obj_func, 1000);
		// DEBUG
		M_DEBUG(
		double tot_obj = get_objectives(solution_vect);
		cout<<"iteration "<<num_iter<<", code is: "<<code<<", cost is: "<<tot_obj<<endl;
		);
		double overlap_area = get_total_overlap_area(solution_vect);
		cur_ovl_ratio = overlap_area / total_device_area;
		M_DEBUG(
		cout<<"cur_ovl_ratio: "<<cur_ovl_ratio<<endl;
		);
		if (cur_ovl_ratio > overlap_threshold) {
			lambda1 *= 2; // double overlap penalty
		}
		// condition if oob penalty need to double
		double hor_l, hor_r, ver_b, ver_t;
		get_total_gp_area(solution_vect, hor_l, hor_r, ver_b, ver_t);
		double oob_width = std::max(hor_r, ub_x) - std::min(hor_l, (double)0);
		double oob_height = std::max(ver_t, ub_y) - std::min(ver_b, (double)0);
		double oob_area = oob_width * oob_height;
		// FIXME: may need to distinguish horizontal oob and vertical oob
		double cur_oob_ratio = (oob_area - ub_x*ub_y) / (ub_x*ub_y); 
		M_DEBUG(
		cout<<"cur_oob_ratio: "<<cur_oob_ratio<<endl;
		);
		if (cur_oob_ratio > oob_threshold) {
			lambda2 *= 2;
		}
		// condition if asymmetry penalty need to double
		double asym_dist = get_total_asym_distance(solution_vect);
		M_DEBUG(
		cout<<"asym_dist: "<<asym_dist<<endl;
		);
		if (asym_dist > asym_threshold) {
			lambda4 *= 2;
		}
		locs.clear();
	    for(size_t i=0; i<vtcs.size(); ++i) {
	    	locs.push_back(make_pair(solution_vect[2*i], solution_vect[2*i+1]));
	    }
	    write_result_to_file(results_dir+"result_"+std::to_string(num_iter)+".txt");
		// if all criterial are met, break
		if (code==0 && cur_ovl_ratio<=overlap_threshold && cur_oob_ratio<=oob_threshold && asym_dist<=asym_threshold) {
			break;
		}
		++num_iter;
	}

	M_DEBUG(
	cout<<"code: "<<code<<", val_min: "<<val_min<<endl;
	cout<<"solution_vect: ";
	for (int i=0; i<len; ++i) {
		cout<<solution_vect[i]<<", ";
	}
	cout<<endl;
	);
	
	// store result to location vector
	locs.clear();
	for(size_t i=0; i<vtcs.size(); ++i) {
		locs.push_back(make_pair(solution_vect[2*i], solution_vect[2*i+1]));
	}

	string out_file_name = results_dir+"result.txt"; 
	write_result_to_file(out_file_name);

	return code;
}
//}}}

limbo::solvers::SolverProperty
NLPlace::legal_detail_place(bool infeasible_rerun)
//{{{
{
    char buf[256]; 
    sprintf(buf, "%s takes %%t seconds CPU, %%w seconds real\n", __func__);
	boost::timer::auto_cpu_timer t (buf);
	d_boxes.clear();
	vector<dBox> d_boxes_b, d_boxes_t;
	for (size_t i=0; i<vtcs.size(); ++i) {
		double x = locs[i].first;
		double y = locs[i].second;
		double w = vtcs[i].scal_dimension.first;
		double h = vtcs[i].scal_dimension.second;
		d_boxes.push_back(dBox(i, x, y, w, h));
#if HAS_FLEXIBILITY == 1
		if (dtype[i] == 0 || dtype[i] == 2) {
			d_boxes_b.push_back(dBox(i, x, y, w, h));
		}
		if (dtype[i] == 1 || dtype[i] == 2) {
			d_boxes_t.push_back(dBox(i, x, y, w, h));
		}
#endif
	}

	// re-run legal_detail_place to fix the chain conflict edge bug: virtually move the symmetric pairs to the same horizontal location
	// FIXME: currently infeasible rerun does not support device-layer awareness
#if HAS_FLEXIBILITY != 1
	if(infeasible_rerun) {
		for (size_t i=0; i<symmetric_groups.size(); ++i) {
			symmetric_pairs_t ps = symmetric_groups[i].first;
			for (symmetric_pair_t p : ps) {
				size_t id1 = p.first, id2 = p.second; 
				// move the right hand side symmetric device
				d_boxes[id2].y = d_boxes[id1].y;
			}
		}
	}
#endif

	cg_h.clear();
	edge_vec_h.clear();
	weight_vec_h.clear();
	cg_v.clear();
	edge_vec_v.clear();
	weight_vec_v.clear();
	
	// init_edges(d_boxes, 1, edge_vec_h, weight_vec_h, true, false);
	// init_edges(d_boxes, 0, edge_vec_v, weight_vec_v, true, false);
#if HAS_FLEXIBILITY == 1
	init_irredundant_edges(d_boxes_b, 1, edge_vec_h, weight_vec_h);
	init_irredundant_edges(d_boxes_t, 1, edge_vec_h, weight_vec_h);
	init_irredundant_edges(d_boxes_b, 0, edge_vec_v, weight_vec_v);
	init_irredundant_edges(d_boxes_t, 0, edge_vec_v, weight_vec_v);
#else
	init_irredundant_edges(d_boxes, 1, edge_vec_h, weight_vec_h);
	init_irredundant_edges(d_boxes, 0, edge_vec_v, weight_vec_v);
#endif

	int nnum_h = (int)vtcs.size() + 2;
	int nnum_v = (int)vtcs.size() + 2;
	constraint_graph(edge_vec_h, weight_vec_h, cg_h, nnum_h); 
	constraint_graph(edge_vec_v, weight_vec_v, cg_v, nnum_v); 
	write_graph_dot(cg_h, results_dir + "horizontal_constraint_graph.dot");
	write_graph_dot(cg_v, results_dir + "vertical_constraint_graph.dot");

	dag_transitive_reduction(cg_h);
	dag_transitive_reduction(cg_v);
	write_graph_dot(cg_h, results_dir + "reduced_horizontal_constraint_graph.dot");
	write_graph_dot(cg_v, results_dir + "reduced_vertical_constraint_graph.dot");
	
	// get other necessary edges
	get_necessary_edges();
	dag_transitive_reduction(cg_h);
	dag_transitive_reduction(cg_v);
	write_graph_dot(cg_h, results_dir + "final_horizontal_constraint_graph.dot");
	write_graph_dot(cg_v, results_dir + "final_vertical_constraint_graph.dot");

	// FIXME: remove any vertical edge between symmetric pair, and add horizontal edge
	for (size_t i=0; i<symmetric_groups.size(); ++i) {
		symmetric_pairs_t ps = symmetric_groups[i].first;
		for (symmetric_pair_t p : ps) {
			boost::remove_edge(vertex(p.first, cg_v), vertex(p.second, cg_v), cg_v);
			boost::remove_edge(vertex(p.second, cg_v), vertex(p.first, cg_v), cg_v);
			bool has_hor_edge = boost::edge(vertex(p.first, cg_h), vertex(p.second, cg_h), cg_h).second || 
				boost::edge(vertex(p.second, cg_h), vertex(p.first, cg_h), cg_h).second;
			if(!has_hor_edge) {
				cout<<"no horizontal edge between symmetric pair!"<<endl;
				if(locs[p.first].first <= locs[p.second].first) {
					boost::add_edge(vertex(p.first, cg_h), vertex(p.second, cg_h), -vtcs[p.first].scal_dimension.first, cg_h);
				} else {
					boost::add_edge(vertex(p.second, cg_h), vertex(p.first, cg_h), -vtcs[p.second].scal_dimension.first, cg_h);
				}

			}
		}
	}

	// TODO: modify edge_vec_h, edge_vec_v, weight_vec_h and weight_vec_v
	reinit_edge_and_weight(cg_h, edge_vec_h, weight_vec_h);
	reinit_edge_and_weight(cg_v, edge_vec_v, weight_vec_v);

	// print constraint edges
	M_DEBUG(
	cout<<"edge_vec_h: "<<endl;
	for (Edge e : edge_vec_h) {
		cout<<"("<<e.first+1<<", "<<e.second+1<<")"<<endl;
	}
	cout<<"edge_vec_v: "<<endl;
	for (Edge e : edge_vec_v) {
		cout<<"("<<e.first+1<<", "<<e.second+1<<")"<<endl;
	}
	);

	vector<double> x_vec, y_vec;
	int n = (int)locs.size();
	M_DEBUG(
	cout<<"edge_vec_h.size(): "<<edge_vec_h.size()<<endl;
	cout<<"edge_vec_v.size(): "<<edge_vec_v.size()<<endl;
	);
	// double dim_x = solve_lp(edge_vec_h, 1, n, d_boxes, x_vec, 0, false, true);
	// double dim_y = solve_lp(edge_vec_v, 0, n, d_boxes, y_vec, 0, false, true);
	pair<double, limbo::solvers::SolverProperty> ret_status1 = solve_lp(edge_vec_h, 1, n, d_boxes, x_vec, 0, false, true);
	pair<double, limbo::solvers::SolverProperty> ret_status2 = solve_lp(edge_vec_v, 0, n, d_boxes, y_vec, 0, false, true);
	if(ret_status1.second == limbo::solvers::INFEASIBLE || ret_status2.second == limbo::solvers::INFEASIBLE) {
		return limbo::solvers::INFEASIBLE;
	}
	double dim_x = ret_status1.first;
	double dim_y = ret_status2.first;
	M_DEBUG(
	cout<<"x_vec.size(): "<<x_vec.size()<<endl;
	cout<<"y_vec.size(): "<<y_vec.size()<<endl;
	cout<<"locs.size(): "<<locs.size()<<endl;
	);
	assert(x_vec.size() == y_vec.size());
	assert(x_vec.size() == locs.size());
	for (size_t i=0; i<locs.size(); ++i) {
		locs[i] = make_pair(x_vec[i], y_vec[i]);
	}	
	string intermediate_file_name = results_dir+"result_legal_intermediate.txt";
	write_result_to_file(intermediate_file_name);

	x_vec.clear();
	y_vec.clear();
	solve_lp(edge_vec_h, 1, n, d_boxes, x_vec, dim_x);
	solve_lp(edge_vec_v, 0, n, d_boxes, y_vec, dim_y);
	M_DEBUG(
	cout<<"x_vec.size(): "<<x_vec.size()<<endl;
	cout<<"y_vec.size(): "<<y_vec.size()<<endl;
	cout<<"locs.size(): "<<locs.size()<<endl;
	);
	assert(x_vec.size() == y_vec.size());
	assert(x_vec.size() == locs.size());
	for (size_t i=0; i<locs.size(); ++i) {
		locs[i] = make_pair(x_vec[i], y_vec[i]);
	}	

	string out_file_name = results_dir+"result_legal_detail.txt";
	write_result_to_file(out_file_name);
	return limbo::solvers::OPTIMAL;
}
//}}}

void NLPlace::reinit_edge_and_weight(graph_t const& cg, vector< Edge >& edge_vec, vector<double>& weight_vec)
//{{{
{
	edge_vec.clear();
	weight_vec.clear();
	using namespace boost;
	IndexMap index_map = get(boost::vertex_index, cg);
	property_map<graph_t, edge_weight_t>::const_type WeightMap = get(edge_weight, cg);
	edge_iterator ei, ei_end;
	for(boost::tie(ei, ei_end)=edges(cg); ei!=ei_end; ++ei) {
		size_t source_vtx = index_map[source(*ei, cg)];
		size_t target_vtx = index_map[target(*ei, cg)];
		edge_vec.push_back(make_pair(source_vtx, target_vtx));
		double wgt = get(WeightMap, *ei);
		weight_vec.push_back(wgt);
	}
}
//}}}

void NLPlace::write_graph_dot(graph_t const& g, string file_name)
//{{{
{
	// DEBUG
	using namespace boost;
	std::ofstream dot_file(file_name);
    if (!dot_file.good())
    {
        cout << "failed to open " << file_name << " for write\n"; 
        exit(-1);
    }
    dot_file << "digraph D {\n"
      << "  rankdir=LR\n"
      << "  size=\"4,3\"\n"
      << "  ratio=\"fill\"\n"
      << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";
    property_map<graph_t, edge_weight_t>::const_type weightmap = get(edge_weight, g);
	property_map<graph_t, vertex_index_t>::const_type vertIndx = get(vertex_index, g);
    graph_traits < graph_t >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
      graph_traits < graph_t >::edge_descriptor e = *ei;
      graph_traits < graph_t >::vertex_descriptor
        u = source(e, g), v = target(e, g);
      dot_file << vertIndx[u] << " -> " << vertIndx[v]
        << "[label=\"" << get(weightmap, e) << "\"";
	  dot_file << ", color=\"black\"";
      dot_file << "]";
    }
    dot_file << "}";
	dot_file.close();
}
//}}}

void NLPlace::dag_transitive_reduction(graph_t& g)
//{{{
{
	// DFS based transitive reduction N(N+M)
	size_t num_nodes = boost::num_vertices(g);
	// build edge matrix
	vector< vector<bool> > edge_mat(num_nodes), reachable(num_nodes);
	for (size_t i=0; i<num_nodes; ++i) {
		edge_mat[i] = vector<bool>(num_nodes, false);
		reachable[i] = vector<bool>(num_nodes, false);
	}
    auto edges = boost::edges(g);
    for (auto it=edges.first; it!=edges.second; ++it) {
    	int source_node = boost::source(*it, cg_h);
    	int target_node = boost::target(*it, cg_h);
		edge_mat[source_node][target_node] = true;
    }
	// dfs remove edge
	vector<bool> visited(num_nodes);
	for (size_t i=0; i<num_nodes; ++i) {
		visited[i] = false;
	}
	M_DEBUG(
	IndexMap index_map = get(boost::vertex_index, g);
	bool has_transitive_edge = dfs_remove_transitive_edge(g, edge_mat, vtcs.size(), visited, reachable, index_map);
	cout<<"has_transitive_edge: "<<has_transitive_edge<<endl;
	);
}
//}}}

bool NLPlace::dfs_remove_transitive_edge(graph_t& g, vector< vector<bool> >& edge_mat, size_t node, vector<bool>& visited, vector< vector<bool> >& reachable, IndexMap const& index_map)
//{{{
{
	bool has_transitive_edge = false;
	visited[node] = true;
	pair<adjacency_iterator, adjacency_iterator> neighbors = boost::adjacent_vertices(vertex(node, g), g);
	for(; neighbors.first != neighbors.second; ++neighbors.first) {
		size_t nei_node = index_map[*neighbors.first];
		if (!visited[nei_node]) {
			if ( dfs_remove_transitive_edge(g, edge_mat, nei_node, visited, reachable, index_map) ) {
				has_transitive_edge = true;
			}
		}
		//bool all_ones = true;
		size_t num_nodes = boost::num_vertices(g);
		for (size_t i=0; i<num_nodes; ++i) {
			if (!reachable[node][i] && reachable[nei_node][i]) {
				// if has edge, remove it
				if (edge_mat[node][i]) {
					edge_mat[node][i] = false;
					boost::remove_edge(vertex(node, g), vertex(i, g),g);
					M_DEBUG(
					cout<<"nei_node: "<<nei_node+1<<endl;
					cout<<"removed edge ("<<node+1<<", "<<i+1<<") in graph"<<endl;
					);
					has_transitive_edge = true;
				}
				reachable[node][i] = true;
			}
			// reachable[node][i] = reachable[node][i] || reachable[nei_node][i];
		}
		reachable[node][nei_node] = true;
	}	
	return has_transitive_edge;
}
//}}}

void NLPlace::get_necessary_edges()
//{{{
{
	// DFS algorithm to get other necessary edges
	// FIXME
	// size_t num_nodes = vtcs.size() + 2 + symmetric_groups.size();
	// No symmetry nodes
	size_t num_nodes = vtcs.size();
	// vector< boost::dynamic_bitset<> > dp_table_h, dp_table_v;
	vector< vector<bool> > dp_table_h, dp_table_v;
	for (size_t i=0; i<num_nodes; ++i) {
		// dp_table_h.push_back( boost::dynamic_bitset<>(num_nodes) );
		// dp_table_h[i].reset();
		// dp_table_v.push_back( boost::dynamic_bitset<>(num_nodes) );
		// dp_table_v[i].reset();
		dp_table_h.push_back( vector<bool>(num_nodes, false) );
		dp_table_v.push_back( vector<bool>(num_nodes, false) );
	}
	M_DEBUG(
	cout<<"dp_table_h.size(): "<<dp_table_h.size()<<", dp_table_v.size(): "<<dp_table_v.size()<<endl;
	for (size_t i=0; i<num_nodes; ++i) {
		cout<<"dp_table_h[i].size(): "<<dp_table_h[i].size()<<", dp_table_v[i].size(): "<<dp_table_v[i].size()<<endl;
	}
	);
	vector<bool> visited_h(num_nodes), visited_v(num_nodes);
	for (size_t i=0; i<num_nodes; ++i) {
		visited_h[i] = false;
		visited_v[i] = false;
	}
	size_t source = vtcs.size();
	IndexMap index_map = get(boost::vertex_index, cg_h);
	dfs_graph(dp_table_h, visited_h, source, cg_h, index_map);
	dfs_graph(dp_table_v, visited_v, source, cg_v, index_map);
	for (size_t i=0; i<num_nodes; ++i) {
		for (size_t j=i+1; j<num_nodes; ++j) {
			if (!(dp_table_h[i][j] || dp_table_h[j][i] ||
					dp_table_v[i][j] || dp_table_v[j][i])) {
#if HAS_FLEXIBILITY == 1
				if (NLPlace::dtype[i] + NLPlace::dtype[j] == 1) {
					M_DEBUG(
					cout<<"there is no edge between node "<<i+1<<" and node "<<j+1<<" but they are on different layers"<<endl;
					);
					continue;
				}
#endif
				M_DEBUG(
				cout<<"there is no edge between node "<<i+1<<" and node "<<j+1<<endl;
				// check_boxes_separable(i, j);
				);
				// add edge
				add_edge_greedy(i, j, cg_h, cg_v);
			}
		}
	}
}
//}}}

void NLPlace::check_boxes_separable(size_t i, size_t j)
//{{{
{
	M_DEBUG(
	cout<<"checking boxes "<<i+1<<" and "<<j+1<<endl;
	);
	assert(!d_boxes[i].hor_overlapping(d_boxes[j]) && !d_boxes[i].ver_overlapping(d_boxes[j]));
}
//}}}

void NLPlace::add_edge_greedy(size_t i, size_t j, graph_t& cg_h, graph_t& cg_v)
//{{{
{
	double space_h = std::max(d_boxes[i].x-d_boxes[j].x-d_boxes[j].w, d_boxes[j].x-d_boxes[i].x-d_boxes[i].w); 
	double space_v = std::max(d_boxes[i].y-d_boxes[j].y-d_boxes[j].h, d_boxes[j].y-d_boxes[i].y-d_boxes[i].h); 
	if (space_h >= space_v) {
		// add edge to horizontal constraint graph
		if (d_boxes[i].x <= d_boxes[j].x) {
			boost::add_edge(vertex(i, cg_h), vertex(j, cg_h), -d_boxes[i].w, cg_h);
            M_DEBUG(
			cout<<"add_edge_greedy: add horizontal edge ("<<i+1<<", "<<j+1<<")"<<endl;
            );
		} else {
			boost::add_edge(vertex(j, cg_h), vertex(i, cg_h), -d_boxes[j].w, cg_h);
            M_DEBUG(
			cout<<"add_edge_greedy: add horizontal edge ("<<j+1<<", "<<i+1<<")"<<endl;
            );
		}
	} else {
		// add edge to vertical constraint graph
		if (d_boxes[i].y <= d_boxes[j].y) {
			boost::add_edge(vertex(i, cg_v), vertex(j, cg_v), -d_boxes[i].h, cg_v);
            M_DEBUG(
			cout<<"add_edge_greedy: add vertical edge ("<<i+1<<", "<<j+1<<")"<<endl;
            );
		} else {
			boost::add_edge(vertex(j, cg_v), vertex(i, cg_v), -d_boxes[j].h, cg_v);
            M_DEBUG(
			cout<<"add_edge_greedy: add vertical edge ("<<j+1<<", "<<i+1<<")"<<endl;
            );
		}
	}
}
//}}}

void NLPlace::dfs_graph(vector< vector<bool> >& dp_table, vector<bool>& visited, size_t node, graph_t const& g, IndexMap const& index_map)
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	cout<<"node: "<<node<<", dp_table[node].size(): "<<dp_table[node].size()<<endl;
	);
	size_t s = vtcs.size();
	size_t t = vtcs.size() + 1;
	if (node < s) {
		visited[node] = true; // FIXME
		// dp_table[node].set(node);
		dp_table[node][node] = true;
	}
	pair<adjacency_iterator, adjacency_iterator> neighbors = boost::adjacent_vertices(vertex(node, g), g);
	for(; neighbors.first != neighbors.second; ++neighbors.first) {
		size_t nei_node = index_map[*neighbors.first];
		if (nei_node == t) { continue; }
		if (!visited[nei_node]) {
			dfs_graph(dp_table, visited, nei_node, g, index_map);
		}
		// if (!dp_table[node].all()) {
		// 	dp_table[node] |= dp_table[nei_node];
		// }
		if (node == s) { continue; }
		bool all_ones = true;
		for (size_t i=0; i<dp_table[node].size(); ++i) {
			if (!dp_table[node][i]) {
				all_ones = false;
				break;
			}
		}
		M_DEBUG(
		cout<<"all_ones: "<<all_ones<<endl;
		cout<<"dp_table[node].size(): "<<dp_table[node].size()<<endl;
		);
		if (!all_ones) {
			for (size_t i=0; i<dp_table[node].size(); ++i) {
				M_DEBUG(
				cout<<"dp_table[node][i]: "<<dp_table[node][i]<<endl;
				cout<<"dp_table[nei_node][i]: "<<dp_table[nei_node][i]<<endl;
				);
				dp_table[node][i] = dp_table[node][i] || dp_table[nei_node][i];
			}
		}
	}
}
//}}}

void NLPlace::detect_negative_cycle()
//{{{
{
	int num = boost::num_vertices(cg_h);
    vector<int> d(num);
    vector<int> p(num, -1);
    int x = -1;
    for (int i=0; i<num; ++i) {
        x = -1;
	    auto edges = boost::edges(cg_h);
	    for (auto it=edges.first; it!=edges.second; ++it) {
	    	int source_node = boost::source(*it, cg_h);
	    	int target_node = boost::target(*it, cg_h);
	    	double wgt = get(boost::edge_weight_t(), cg_h, *it);
            if (d[source_node] + wgt < d[target_node]) {
                d[target_node] = d[source_node] + wgt;
                p[target_node] = source_node;
                x = target_node;
            }
	    }
    }

    if (x == -1) {
		M_DEBUG(
        cout << "No negative cycle found.";
		);
    } else {
        for (int i = 0; i < num; ++i)
            x = p[x];

        vector<int> cycle;
        for (int v = x;; v = p[v]) {
            cycle.push_back(v);
            if (v == x && cycle.size() > 1)
                break;
        }
        reverse(cycle.begin(), cycle.end());

		M_DEBUG(
        cout << "Negative cycle: ";
        for (int v : cycle)
            cout << v << ' ';
        cout << endl;
		);
    }
}
//}}}

double NLPlace::get_total_device_area()
//{{{
{
	double tot_area = 0;
	for (size_t i=0; i<vtcs.size(); ++i) {
	    tot_area += vtcs[i].scal_dimension.first * vtcs[i].scal_dimension.second;
	}
	return tot_area;
}
//}}}

double NLPlace::get_total_overlap_area(double * solution_vect)
//{{{
{
	double ovl_area = 0;
	for (size_t i=0; i<vtcs.size(); ++i) {
		double xi = solution_vect[2*i];
		double yi = solution_vect[2*i+1];
		double wi = vtcs[i].scal_dimension.first;
		double hi = vtcs[i].scal_dimension.second;
		dBox boxi (i, xi, yi, wi, hi);
		for (size_t j=i+1; j<vtcs.size(); ++j) {
#if HAS_FLEXIBILITY == 1
			if (NLPlace::dtype[i] + NLPlace::dtype[j] == 1) { continue; }
#endif
		    double xj = solution_vect[2*j];
		    double yj = solution_vect[2*j+1];
		    double wj = vtcs[j].scal_dimension.first;
		    double hj = vtcs[j].scal_dimension.second;
			dBox boxj (j, xj, yj, wj, hj);
			if (boxi.hor_overlapping(boxj) && boxi.ver_overlapping(boxj)) {
				ovl_area += boxi.get_hor_overlap(boxj) * boxi.get_ver_overlap(boxj);
				M_DEBUG(
				cout<<"ovl_area: "<<ovl_area<<endl;
				);
			}
		}
	}
	return ovl_area;
}
//}}}

void NLPlace::get_total_gp_area(double * solution_vect, double& hor_l, double& hor_r, double& ver_b, double& ver_t)
//{{{
{
	double l = std::numeric_limits<double>::max(), r = std::numeric_limits<double>::min();
	double b = l, t = r; 
	for (size_t i=0; i<vtcs.size(); ++i) {
		double xi = solution_vect[2*i];
		double yi = solution_vect[2*i+1];
		double wi = vtcs[i].scal_dimension.first;
		double hi = vtcs[i].scal_dimension.second;
		if (xi < l) l = xi;
		if (xi+wi > r) r = xi+wi;
		if (yi < b) b = yi;
		if (yi+hi > t) t = yi+hi;
	}
	assert(r >= l && t >= b);
	hor_l = l;
	hor_r = r;
	ver_b = b;
	ver_t = t;
}
//}}}

double NLPlace::get_total_asym_distance(double * solution_vect)
//{{{
{
	size_t num = vtcs.size();
	double asym_dist = 0;
	for (size_t i=0; i<symmetric_groups.size(); ++i) {
		auto sym_g = symmetric_groups[i];
		auto sps = sym_g.first; // symmetry pairs
		auto sss = sym_g.second; // self-symmetry
		for (auto sp : sps) {
			size_t id1=sp.first, id2=sp.second;
			double wi = NLPlace::vtcs[id1].scal_dimension.first;
			// cout<<"device "<<id1<<".y: "<<solution_vect[2*id1+1]<<", device "<<id2<<".y: "<<solution_vect[2*id2+1]<<endl;
			asym_dist += std::abs(solution_vect[2*id1+1] - solution_vect[2*id2+1]);
			// cout<<"asym_dist: "<<asym_dist<<endl;
			asym_dist += std::abs(solution_vect[2*id1]+solution_vect[2*id2]+wi-2*solution_vect[2*num+i]);
			// cout<<"asym_dist: "<<asym_dist<<endl;
		}
		for (auto ss : sss) {
			double wi = NLPlace::vtcs[ss].scal_dimension.first;
			asym_dist += std::abs(solution_vect[2*ss]+wi/2-solution_vect[2*num+i]);
			// cout<<"asym_dist: "<<asym_dist<<endl;
		}
	}
	return asym_dist;
}
//}}}

void NLPlace::write_result_to_file(string out_file_name)
//{{{
{
    M_DEBUG(
	cout << __func__ << ": " << out_file_name <<endl;
    );
	// string out_file_name = results_dir+"result.txt"; 
	std::ofstream out_file;
	out_file.open(out_file_name);
    if (!out_file.good())
    {
        cout << "failed to open " << out_file_name << " for write\n"; 
        exit(-1);
    }
	//format: name x y w h
	for(size_t i=0; i<vtcs.size(); ++i) {
		// store result to location vector
		out_file<<vtcs[i].name<<" ";
		out_file<<locs[i].first<<" ";
		out_file<<locs[i].second<<" ";
		out_file<<vtcs[i].scal_dimension.first<<" ";
		out_file<<vtcs[i].scal_dimension.second<<"\n";
	}
	out_file.close();
}
//}}}

void NLPlace::legalize(pair<double, double>& dims)
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	);
	// init d_boxes
	for (size_t i=0; i<vtcs.size(); ++i) {
		double x = locs[i].first;
		double y = locs[i].second;
		double w = vtcs[i].scal_dimension.first;
		double h = vtcs[i].scal_dimension.second;
		d_boxes.push_back(dBox(i, x, y, w, h));
	}
	build_constraint_graphs(d_boxes);
	solve_shortest_paths(d_boxes, dims);
}
//}}}

void NLPlace::solve_shortest_paths(vector<dBox> const& boxes, pair<double, double>& dims)
//{{{
{
	cout<<__func__<<endl;
	vector<double> x_vec, y_vec;
	double tot_w, tot_h;

	// iteratively solve shortest paths considering symmetry axis
	vector<size_t> order_vec;
	vector<bool> order_vec_visited;
	for (size_t i=0; i<boost::num_vertices(cg_h); ++i) {
		order_vec_visited.push_back(false);
	}
	IndexMap index_map = get(boost::vertex_index, cg_h);
	size_t s = boxes.size();
	//size_t t = boxes.size() + 1;

	// DEBUG
	using namespace boost;
	string file_name = results_dir + "horizontal_constraint_graph.dot";
	std::ofstream dot_file(file_name);
    if (!dot_file.good())
    {
        cout << "failed to open " << file_name << " for write\n"; 
        exit(-1);
    }
    dot_file << "digraph D {\n"
      << "  rankdir=LR\n"
      << "  size=\"4,3\"\n"
      << "  ratio=\"fill\"\n"
      << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";
    property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, cg_h);
	property_map<graph_t, vertex_index_t>::type vertIndx = get(vertex_index, cg_h);
    graph_traits < graph_t >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(cg_h); ei != ei_end; ++ei) {
      graph_traits < graph_t >::edge_descriptor e = *ei;
      graph_traits < graph_t >::vertex_descriptor
        u = source(e, cg_h), v = target(e, cg_h);
      dot_file << vertIndx[u] << " -> " << vertIndx[v]
        << "[label=\"" << get(weightmap, e) << "\"";
	  dot_file << ", color=\"black\"";
      dot_file << "]";
    }
    dot_file << "}";
	dot_file.close();

	vector<double> d_h;
	// detect_negative_cycle();
	solve_shortest_path(boxes, 1, x_vec, tot_w, cg_h, d_h); // horizontal CG
	for (size_t i=0; i<order_vec.size(); ++i) {
		if (order_vec[i] > boxes.size()+1) {
			// is symmetry axis node, update the self-symmetry nodes and the symmetry pair nodes
			// for symmetry pair nodes
			double d_axis = d_h.at(order_vec[i]); 
			size_t group_nid = order_vec[i] - boxes.size() - 2; 
			auto sym_group = symmetric_groups[group_nid];
			pair<adjacency_iterator, adjacency_iterator> neighbors = boost::adjacent_vertices(vertex(order_vec[i], cg_h), cg_h);
			for(; neighbors.first != neighbors.second; ++neighbors.first) {
				M_DEBUG(
				cout<<"neighbor: "<<index_map[*neighbors.first]<<endl;
				);
				size_t id_r = index_map[*neighbors.first];
				edge_descriptor e = boost::edge(order_vec[i], id_r, cg_h).first;
				// find the symmetric counterpart
				size_t id_l = s;
				for (auto sym_pair : sym_group.first) {
					if (sym_pair.first == id_r) {
						id_l = sym_pair.second;
						break;
					}
					if (sym_pair.second == id_r) {
						id_l = sym_pair.first;
						break;
					}
				}
				// has symmetric counterpart
				if (id_l != s) {
				    double d_lnode = d_h.at(id_l); 
				    double wi = vtcs.at(id_l).scal_dimension.first;
				    double dist = -d_axis + d_lnode - wi;
					M_DEBUG(
					cout<<"d_axis: "<<d_axis<<", d_lnode: "<<d_lnode<<", wi: "<<wi<<endl;
					cout<<"dist: "<<dist<<endl;
					);
				    assert(dist >= 0);
				    boost::put(boost::edge_weight_t(), cg_h, e, -dist);
				}
    		}
			// for self symmetry nodes
			for (auto ss : sym_group.second) {
				double wi = vtcs.at(ss).scal_dimension.first;
				double d_ss = d_h.at(ss); 
				double movement = -d_axis - wi/2 + d_ss; 
				assert(movement >= 0);
				auto edges = boost::edges(cg_h);
				for (auto it=edges.first; it!=edges.second; ++it) {
					if (boost::target(*it, cg_h) == ss) {
						//size_t in_node = boost::source(*it, cg_h);
						double wgt = get(boost::edge_weight_t(), cg_h, *it);
						boost::put(boost::edge_weight_t(), cg_h, *it, wgt-movement);
					}
				}
			}
			x_vec.clear();
			d_h.clear();
			solve_shortest_path(boxes, 1, x_vec, tot_w, cg_h, d_h);
		} // end if symmetry axis
	}
	
	vector<double> d_v;
	solve_shortest_path(boxes, 0, y_vec, tot_h, cg_v, d_v); // vertical CG
	
	// store results to locs
	// cout<<"x_vec.size(): "<<x_vec.size()<<endl;
	// cout<<"locs.size(): "<<locs.size()<<endl;
	assert(x_vec.size() == y_vec.size());
	assert(x_vec.size() == locs.size());
	for (size_t i=0; i<locs.size(); ++i) {
		locs[i] = make_pair(x_vec[i], y_vec[i]);
	}

	check_locs_symmetry(boxes);

	dims = make_pair(tot_w, tot_h); 
}
//}}}

void NLPlace::check_locs_symmetry(vector<dBox> const& boxes)
//{{{
{
	for (size_t i=0; i<symmetric_groups.size(); ++i) {
        M_DEBUG(
        bool sym_flag = true;
        );
		symmetric_pairs_t ps = symmetric_groups[i].first;
		self_symmetric_t ss = symmetric_groups[i].second;
		vector<double> axes;
		for (symmetric_pair_t p : ps) {
			double x1 = locs[p.first].first;
			double x2 = locs[p.second].first;
			double y1 = locs[p.first].second;
			double y2 = locs[p.second].second;
	        axes.push_back((x1+x2+boxes[p.first].w)/2);
			if (y1 != y2) {
				M_DEBUG(
				sym_flag = false; 
				cout<<"y locs not the same: "<<p.first<<" and "<<p.second<<endl;
				);
			}
		}
		for (size_t s : ss) {
			double xs = locs[s].first; 
			axes.push_back(xs+boxes[s].w/2);
		}
		for (size_t j=0; j<axes.size(); ++j) {
		    for (size_t k=j+1; k<axes.size(); ++k) {
				if (axes[j] != axes[k]) {
					M_DEBUG(
					sym_flag = false;
					cout<<"axes not the same for group "<<i<<endl;
					);
				}
		    }
		}
		M_DEBUG(
		if (sym_flag) {
			cout<<"Symmetric for group "<<i<<endl;
		} else {
			cout<<"Not symmetric for group "<<i<<endl;
		}
		);
	}
}
//}}}

void NLPlace::build_constraint_graphs(vector<dBox> const& d_boxes)
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	);
	init_edges(d_boxes, 1, edge_vec_h, weight_vec_h, false);
	init_edges(d_boxes, 0, edge_vec_v, weight_vec_v, false);

	int nnum_h = (int)d_boxes.size() + 2 + (int)symmetric_groups.size();
	int nnum_v = (int)d_boxes.size() + 2;
	constraint_graph(edge_vec_h, weight_vec_h, cg_h, nnum_h); 
	constraint_graph(edge_vec_v, weight_vec_v, cg_v, nnum_v); 
}
//}}}

void NLPlace::init_edges(vector<dBox> const& boxes, bool is_horizontal, vector< Edge >& edge_vec, vector<double>& weight_vec, bool no_redundant, bool has_symmetry_edges) 
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	);
	size_t n = boxes.size();
	vector<dBox> boxes_sorted = boxes; // for CG

    int num_nodes = n + 2; // 2 nodes for s and t, source node id: n, destination node id: n+1

	// Step 1: sort by left edges
	if (is_horizontal) {
        struct {
            bool operator()(dBox a, dBox b) const { 
	    		return (a.x < b.x);
            } 
        } compLeft; // left edge smaller to greater
	    std::sort(boxes_sorted.begin(), boxes_sorted.end(), compLeft);
	}
	else {
        struct {
            bool operator()(dBox a, dBox b) const { 
	    		return (a.y < b.y);
            } 
        } compBottom; // bottom edge smaller to greater
	    std::sort(boxes_sorted.begin(), boxes_sorted.end(), compBottom);
	}

	// Step 2: find neighbors
	vector< bool > visited( n ); // indicate whether the box is visited

	bool edge_inited = false;
	if (no_redundant) {
		get_cg_edges_no_redundant(boxes_sorted, edge_vec, weight_vec, visited, is_horizontal, n, edge_inited);
	}
	else {
		get_cg_edges(boxes_sorted, edge_vec, weight_vec, visited, is_horizontal, n, edge_inited);
	}
	
	// Step 3: add the source edges
	for (size_t i=0; i<visited.size(); ++i) {
		if ( !visited[i] ) {
			M_DEBUG(
			cout<<"visited["<<(i+1)<<"] = false"<<endl;
			);
			// source edge
			edge_vec.push_back( std::make_pair(n,i) ); // the device index is i
			weight_vec.push_back(0);
			M_DEBUG(
			cout<<"device "<<(i+1)<<" has no left neighbor, s points to it"<<endl;
			);
		}
	}

	// NOTE: this is to support LP detailed placement
	if (!has_symmetry_edges) return;

	// Step 4: add symmetry edges
	// FIXME: right now only consider vertical symmetry axis
	// num_nodes += (int)symmetric_groups.size();
	for (size_t i=0; i<symmetric_groups.size(); ++i) {
		size_t group_nid = (size_t)num_nodes + i;
		auto sym_g = symmetric_groups[i];
		auto sps = sym_g.first; // symmetry pairs
		auto sss = sym_g.second; // self-symmetry
		for (auto sp : sps) {
			size_t id1=sp.first, id2=sp.second;
			double wi = NLPlace::vtcs[id1].scal_dimension.first;
			// determine which one is the left node and which is the right node
			if (is_horizontal) {
			    size_t id_l = boxes[id1].x < boxes[id2].x ? id1 : id2;
			    size_t id_r = boxes[id1].x < boxes[id2].x ? id2 : id1;
			    // left node
			    edge_vec.push_back( std::make_pair(id_l,group_nid) );
			    weight_vec.push_back(-wi);
			    // right node
			    edge_vec.push_back( std::make_pair(group_nid, id_r) );
			    weight_vec.push_back(0);
			} else {
				// avoid the negative cycle, for symmetry pairs, should not have vertical edge
				vector< Edge >::iterator it1, it2;
				while ((it1=std::find(edge_vec.begin(), edge_vec.end(), std::make_pair(id1, id2))) != edge_vec.end()) {
					M_DEBUG(
					cout<<"erase 1"<<endl;
					);
					edge_vec.erase(it1);
					size_t dist = it1-edge_vec.begin();
					weight_vec.erase(weight_vec.begin()+dist);
				}
				while ((it2=std::find(edge_vec.begin(), edge_vec.end(), std::make_pair(id2, id1))) != edge_vec.end()) {
					M_DEBUG(
					cout<<"erase 2"<<endl;
					);
					edge_vec.erase(it2);
					size_t dist = it2-edge_vec.begin();
					weight_vec.erase(weight_vec.begin()+dist);
				}
			    edge_vec.push_back( std::make_pair(id1, id2) );
			    weight_vec.push_back(0);
			    edge_vec.push_back( std::make_pair(id2, id1) );
			    weight_vec.push_back(0);
			}
		}
		for (auto ss : sss) {
			double wi = NLPlace::vtcs[ss].scal_dimension.first;
			if (is_horizontal) {
				// find the out-edges of the self-symmetry device
				size_t num_edge_vec = edge_vec.size();
				for (size_t j=0; j<num_edge_vec; ++j) {
					if (edge_vec[j].first == ss) {
						edge_vec.push_back( std::make_pair(group_nid, edge_vec[j].second) );
						weight_vec.push_back(-wi/2);
					} 
				}
				// add the edge from self symmetry node to axis 
			    edge_vec.push_back( std::make_pair(ss,group_nid) );
			    weight_vec.push_back(-wi/2);
			}
		}
	} // end symmetry constraint edges

}
//}}}

void NLPlace::get_cg_edges(vector<dBox> const& boxes_sorted, vector< Edge > & edge_vec, vector<double> & weight_vec, vector<bool> & visited, bool is_horizontal, int n, bool edge_inited) 
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	);
	for (auto it=boxes_sorted.begin(); it!=boxes_sorted.end(); ++it) {
		M_DEBUG(
		cout<<__func__<<": index "<<it->index+1<<endl;
		);
		auto it2 = it+1;
		bool has_neighbor = false;
		while ( it2!=boxes_sorted.end() ) {
			M_DEBUG(
			cout<<__func__<<": index2 "<<it2->index+1<<endl;
			);
			// must be on the right of the current box
			// NLP version: change to consider overlap before legalization
			int ovl = it->overlapping(*it2);
			// NOTE: to resolve the negative cycle issue, forbids edges between self symmetry devices
			bool both_ss = false; 
	        for (size_t k=0; k<symmetric_groups.size(); ++k) {
	        	auto sss = symmetric_groups[k].second; // self-symmetry
				if (sss.find(it->index)!=sss.end() && sss.find(it2->index)!=sss.end()) {
					both_ss = true;
					break;
				}
	        }
			M_DEBUG(
			cout<<"ovl!=0: it->index: "<<it->index<<", it2->index: "<<it2->index<<endl;
			cout<<"both_ss: "<<both_ss<<endl;
			);
			if (both_ss) {
				if (!is_horizontal) {
					M_DEBUG(
					cout<<"both_ss: it->index: "<<it->index<<", it2->index: "<<it2->index<<endl;
					);
		            double disp_b = it2->y + it2->h - it->y;
		            double disp_t = it->y + it->h - it2->y;
					if (disp_b <= disp_t) {
			            visited[it->index] = true;
			            edge_vec.push_back( std::make_pair(it2->index, it->index) );
			            weight_vec.push_back(-it2->h);
					} else {
			            visited[it2->index] = true;
			            edge_vec.push_back( std::make_pair(it->index, it2->index) );
			            weight_vec.push_back(-it->h);
					}
				}
			    ++it2;
				continue;
			}
			// NOTE: end resolve negative cycle issue
			if ( (is_horizontal && it2->x < (it->x+it->w) && (ovl==0 || ovl==3 || ovl==4)) ||
					((!is_horizontal) && it2->y < (it->y+it->h) && (ovl==0 || ovl==1 || ovl==2)) ) {
				++it2;
				continue;
			}
			// if overlap
			if (ovl != 0) 
			//{{{
			{
				if (is_horizontal) {
					if (ovl == 1) {
						// move to left
				        visited[it->index] = true;
				        edge_vec.push_back( std::make_pair(it2->index, it->index) );
				        weight_vec.push_back(-it2->w); // the weight is the width of the current box
				        ++it2;
						continue;
					}
					else {
						assert(ovl == 2);
					}
				}
				else {
					if (ovl == 3) {
						// move to bottom
				        visited[it->index] = true;
				        edge_vec.push_back( std::make_pair(it2->index, it->index) );
				        weight_vec.push_back(-it2->h); // the weight is the height of the current box
				        ++it2;
						continue;
					}
					else {
						assert(ovl == 4);
					}
				}
			}
			//}}}
			has_neighbor = true;
			visited[it2->index] = true;
			edge_vec.push_back( std::make_pair(it->index, it2->index) );
			if (is_horizontal) {
				weight_vec.push_back(-it->w); // the weight is the width of the current box
			}
			else {
				weight_vec.push_back(-it->h); // the weight is the height of the current box
			}
			M_DEBUG(
			cout<<"device "<<(it->index+1)<<" has right neighbor: device "<<(it2->index+1)<<endl;
			);
			++it2;
		} // end while 
		// no neighbors, then it has an edge to t
		M_DEBUG(
		cout<<__func__<<": end while"<<endl;
		);
		if ( !has_neighbor ) {
			edge_vec.push_back( std::make_pair(it->index,n+1) );
			if (is_horizontal) {
				weight_vec.push_back(-it->w);
			}
			else {
				weight_vec.push_back(-it->h);
			}
			M_DEBUG(
			cout<<"device "<<(it->index+1)<<" has no right neighbor, point to t"<<endl;
			);
		}
	}
	// add 2 0-cost edges for each symmetric pairs
}
//}}}

void NLPlace::init_irredundant_edges(vector<dBox> const& boxes, bool is_horizontal, vector< Edge >& edge_vec, vector<double>& weight_vec)
//{{{
{
	// sort events
	vector< Event > events;
	for (size_t i=0; i<boxes.size(); ++i) {
		if (is_horizontal) {
		    events.push_back( Event(boxes[i].y, i, boxes[i].index, 0) );
		    events.push_back( Event(boxes[i].y+boxes[i].h, i, boxes[i].index, 1) );
		} else {
		    events.push_back( Event(boxes[i].x, i, boxes[i].index, 0) );
		    events.push_back( Event(boxes[i].x+boxes[i].w, i, boxes[i].index, 1) );
		}
	}
	std::sort(events.begin(), events.end());

	vector<bool> overlap_any(boxes.size(), false);
	// add overlap edges
	for (auto it=boxes.begin(); it!=boxes.end(); ++it) {
		for (auto it2=it+1; it2!=boxes.end(); ++it2) {
			int ovl = it->overlapping(*it2);
			// if overlap
			if (ovl != 0) {
				// overlap_any[it->index] = true; // mark it to be overlap with others
				// overlap_any[it2->index] = true; // mark it to be overlap with others
				overlap_any[it-boxes.begin()] = true; // mark it to be overlap with others
				overlap_any[it2-boxes.begin()] = true; // mark it to be overlap with others
				// if symmetry pair, only add horizontal edge
				bool sym_pair_flag = false;
				bool ss_pair_flag = false;
	            for (size_t k=0; k<symmetric_groups.size(); ++k) {
	            	auto sps = symmetric_groups[k].first; // symmetry pairs
	            	auto sss = symmetric_groups[k].second; // symmetry pairs
	            	for (auto sp : sps) {
	            		size_t id1=sp.first, id2=sp.second;
						if ((it->index==id1 && it2->index==id2) || (it->index==id2 && it2->index==id1)) {
							sym_pair_flag = true;
							break;
						}
	            	}
					auto find_it1 = std::find(sss.begin(), sss.end(), it->index);
					auto find_it2 = std::find(sss.begin(), sss.end(), it2->index);
					if (find_it1 != sss.end() && find_it2 != sss.end()) {
						ss_pair_flag = true;
						break;
					}
					if (sym_pair_flag) break;
				}
				if (sym_pair_flag) {
					M_DEBUG(
					cout<<"added horizontal overlap edge for symmetric pair ("<<it->index+1<<", "<<it2->index+1<<")"<<endl;
					);
					if (is_horizontal) {
						if (it->x <= it2->x) {
			                edge_vec.push_back( make_pair(it->index, it2->index) );
			                weight_vec.push_back(-it->w);
						} else {
			                edge_vec.push_back( make_pair(it2->index, it->index) );
			                weight_vec.push_back(-it2->w);
						}
					}
					continue;
				}
				if (ss_pair_flag) {
					M_DEBUG(
					cout<<"added vertical overlap edge for self-symmetric pair ("<<it->index+1<<", "<<it2->index+1<<")"<<endl;
					);
					if (!is_horizontal) {
						double disp_b = it2->y + it2->h - it->y;
						double disp_t = it->y + it->h - it2->y;
						if (disp_t <= disp_b) {
			                edge_vec.push_back( make_pair(it->index, it2->index) );
			                weight_vec.push_back(-it->h);
						} else {
			                edge_vec.push_back( make_pair(it2->index, it->index) );
			                weight_vec.push_back(-it2->h);
						}
					}
					continue;
				}
				if (is_horizontal) {
				    if (ovl == 1) {
				    	// move to left
					    M_DEBUG(
						cout<<"added horizontal overlap edge ("<<it2->index+1<<", "<<it->index+1<<")"<<endl;
						);
			            edge_vec.push_back( std::make_pair(it2->index, it->index) );
			            weight_vec.push_back(-it2->w); // the weight is the width of the current box
				    }
				    else if (ovl == 2) {
				    	// move to right
					    M_DEBUG(
						cout<<"added horizontal overlap edge ("<<it->index+1<<", "<<it->index+1<<")"<<endl;
						);
			            edge_vec.push_back( std::make_pair(it->index, it2->index) );
			            weight_vec.push_back(-it->w); // the weight is the width of the current box
				    }
				} else {
				    if (ovl == 3) {
				    	// move to bottom
					    M_DEBUG(
						cout<<"added vertical overlap edge ("<<it2->index+1<<", "<<it->index+1<<")"<<endl;
						);
			            edge_vec.push_back( std::make_pair(it2->index, it->index) );
			            weight_vec.push_back(-it2->h); // the weight is the height of the current box
				    }
				    else if (ovl == 4) {
				    	// move to top
					    M_DEBUG(
						cout<<"added vertical overlap edge ("<<it->index+1<<", "<<it2->index+1<<")"<<endl;
						);
			            edge_vec.push_back( std::make_pair(it->index, it2->index) );
			            weight_vec.push_back(-it->h); // the weight is the height of the current box
				    }
				}
			}
		} // end inner for loop
	} // end outer for loop

	// execute the events
	vector<dBox> orders; // TODO: change this to balanced tree
	// the right hand side candidate
	vector<int> cand(boxes.size()+2, -1);
	for (Event ev : events) {
		if (ev.edge_type) {
			init_irredundant_edges_delete(boxes, is_horizontal, edge_vec, weight_vec, orders, cand, ev.index, ev.real_index, overlap_any);
		} else {
			init_irredundant_edges_insert(boxes, is_horizontal, orders, cand, ev.index, ev.real_index);
		}
	}

}
//}}}

void NLPlace::init_irredundant_edges_insert(vector<dBox> const& boxes, bool is_horizontal, vector<dBox>& orders, vector<int>& cand, size_t idx, size_t ridx)
//{{{
{
	M_DEBUG(
	cout<<"insert device "<<ridx+1<<endl;
	);
	orders.push_back(boxes[idx]);
	if (is_horizontal) {
        struct {
            bool operator()(dBox a, dBox b) const { 
	    		return (a.x < b.x);
            } 
        } compLeft; // left edge smaller to greater
	    std::sort(orders.begin(), orders.end(), compLeft);
	} else {
        struct {
            bool operator()(dBox a, dBox b) const { 
	    		return (a.y < b.y);
            } 
        } compBottom; // left edge smaller to greater
	    std::sort(orders.begin(), orders.end(), compBottom);
	}
	auto it = std::find(orders.begin(), orders.end(), boxes[idx]);
	if (it!=orders.end()-1) {
		// has right neighbor
		auto it2 = it+1;
		M_DEBUG(
		cout<<"device "<<ridx+1<<" has right neighbor "<<it2->index+1<<endl;
		);
		// while (it2!=orders.end() && boxes[idx].hor_overlapping(boxes[it2->index]) &&
		// 		boxes[idx].ver_overlapping(boxes[it2->index])) {
		while (it2!=orders.end() && boxes[idx].hor_overlapping(*it2) &&
				boxes[idx].ver_overlapping(*it2)) {
			++it2;
		}
		if (it2 == orders.end()) {
			M_DEBUG(
			cout<<"changed to point to t"<<endl;
			);
			cand[idx] = (int)boxes.size()+1; // point to t 
		} else {
			M_DEBUG(
			cout<<"point to "<<it2->index+1<<endl;
			);
			// cand[idx] = (int)it2->index;
			auto it_idx = std::find(boxes.begin(), boxes.end(), *it2);
			cand[idx] = (int)(it_idx-boxes.begin());
		}
	} else {
		M_DEBUG(
		cout<<"device "<<ridx+1<<" points to t"<<endl;
		);
		cand[idx] = (int)boxes.size()+1; // point to t 
	}
	if (it!=orders.begin()) {
		// has left neighbor
		auto it2 = it-1;
		M_DEBUG(
		cout<<"device "<<ridx+1<<" has left neighbor "<<it2->index+1<<endl;
		);
		while (it2+1!=orders.begin() && boxes[idx].hor_overlapping(*it2) &&
				boxes[idx].ver_overlapping(*it2)) {
			--it2;
		}
		if (it2+1 == orders.begin()) {
			M_DEBUG(
			cout<<"changed to pointed by s"<<endl;
			);
			cand[boxes.size()] = (int)idx; // s points to it 
		} else {
			M_DEBUG(
			cout<<"pointed by "<<it2->index+1<<endl;
			);
			// cand[it2->index] = (int)idx;
			auto it_idx = std::find(boxes.begin(), boxes.end(), *it2);
			cand[it_idx-boxes.begin()] = (int)idx;
		}
	} else {
		M_DEBUG(
		cout<<"device "<<idx+1<<" pointed by s"<<endl;
		);
		cand[boxes.size()] = (int)idx; // s points to it 
	}
}
//}}}

void NLPlace::init_irredundant_edges_delete(vector<dBox> const& boxes, bool is_horizontal, vector< Edge >& edge_vec, vector<double>& weight_vec, vector<dBox>& orders, vector<int> const& cand, size_t idx, size_t ridx, vector<bool> const& overlap_any)
//{{{
{
	M_DEBUG(
	cout<<"delete device "<<ridx+1<<endl;
	);
	auto it = std::find(orders.begin(), orders.end(), boxes[idx]);
	if (it!=orders.end()-1) {
		// has right neighbor
		auto it2 = it+1;
		while (it2!=orders.end() && boxes[idx].hor_overlapping(*it2) &&
				boxes[idx].ver_overlapping(*it2)) {
			++it2;
		}
		if (it2 == orders.end()) {
		    // point to t 
		    if (cand[idx] == (int)boxes.size()+1) {
				M_DEBUG(
				cout<<"device "<<ridx+1<<" changed to point to t"<<endl;
				);
		    	// edge_vec.push_back(make_pair(idx, boxes.size()+1));
		    	edge_vec.push_back(make_pair(ridx, vtcs.size()+1));
		    	if (is_horizontal) {
		    	    weight_vec.push_back(-boxes[idx].w);
		    	} else {
		    	    weight_vec.push_back(-boxes[idx].h);
		    	}
		    }
		} else {
		    // if (cand[idx] == (int)(it+1)->index) {
			auto it_idx = std::find(boxes.begin(), boxes.end(), *it2);
			M_DEBUG(
			cout<<"overlap_any[it2->index]: "<<overlap_any[it_idx-boxes.begin()]<<endl;
			);
		    if ((cand[idx] == (int)(it_idx-boxes.begin()) || cand[idx] == (int)boxes.size()+1) && !overlap_any[it_idx-boxes.begin()]) {
				M_DEBUG(
				cout<<"device "<<ridx+1<<" has right neighbor "<<it2->index+1<<endl;
				);
		    	// edge_vec.push_back(make_pair(idx, (it+1)->index));
		    	// edge_vec.push_back(make_pair(idx, it2->index));
		    	edge_vec.push_back(make_pair(ridx, it2->index));
		    	if (is_horizontal) {
		    	    weight_vec.push_back(-boxes[idx].w);
		    	} else {
		    	    weight_vec.push_back(-boxes[idx].h);
		    	}
		    } else {
				// ensure no overlap
			    // while (it2 != orders.end() && overlap_any[it2->index] && !(boxes[idx].hor_overlapping(boxes[it2->index]) && boxes[idx].ver_overlapping(boxes[it2->index]))) {
			    while (it2 != orders.end() && overlap_any[it_idx-boxes.begin()] && !(boxes[idx].hor_overlapping(*it2) && boxes[idx].ver_overlapping(*it2))) {
				    M_DEBUG(
			    	cout<<"device "<<ridx+1<<" has right neighbor "<<it2->index+1<<endl;
					);
			        // edge_vec.push_back(make_pair(idx, it2->index));
			        edge_vec.push_back(make_pair(ridx, it2->index));
			        if (is_horizontal) {
			            weight_vec.push_back(-boxes[idx].w);
			        } else {
			            weight_vec.push_back(-boxes[idx].h);
			        }
			    	++it2;
					it_idx = std::find(boxes.begin(), boxes.end(), *it2);
			    }
			}
		}
	} else {
		// point to t 
		if (cand[idx] == (int)boxes.size()+1) {
			M_DEBUG(
			cout<<"device "<<ridx+1<<" points to t"<<endl;
			);
			// edge_vec.push_back(make_pair(idx, boxes.size()+1));
			edge_vec.push_back(make_pair(ridx, vtcs.size()+1));
			if (is_horizontal) {
			    weight_vec.push_back(-boxes[idx].w);
			} else {
			    weight_vec.push_back(-boxes[idx].h);
			}
		}
	}
	if (it!=orders.begin()) {
		// has left neighbor
		auto it2 = it-1;
		while (it2+1!=orders.begin() && boxes[idx].hor_overlapping(*it2) &&
				boxes[idx].ver_overlapping(*it2)) {
			--it2;
		}
		if (it2+1 == orders.begin()) {
		    // s points to it 
		    // if (cand[boxes.size()] == (int)idx) {
		    if (cand[boxes.size()] == (int)idx || overlap_any[idx]) {
			    M_DEBUG(
				cout<<"device "<<ridx+1<<" changes to pointed by s"<<endl;
				);
		    	// edge_vec.push_back(make_pair(boxes.size(), idx));
		    	edge_vec.push_back(make_pair(vtcs.size(), ridx));
		    	weight_vec.push_back(0);
		    }
		} else {
		    // if (cand[(it-1)->index] == (int)idx) {
			/*
		    if (cand[it2->index] == (int)idx) {
				cout<<"device "<<idx+1<<" has left neighbor "<<it2->index+1<<endl;
		    	// edge_vec.push_back(make_pair((it-1)->index, idx));
		    	edge_vec.push_back(make_pair(it2->index, idx));
		    	if (is_horizontal) {
		    	    // weight_vec.push_back(-boxes[(it-1)->index].w);
		    	    weight_vec.push_back(-boxes[it2->index].w);
		    	} else {
		    	    // weight_vec.push_back(-boxes[(it-1)->index].h);
		    	    weight_vec.push_back(-boxes[it2->index].h);
		    	}
		    }
			*/
			M_DEBUG(
			cout<<"device "<<ridx+1<<" has left neighbor "<<it2->index+1<<endl;
			);
			// edge_vec.push_back(make_pair(it2->index, idx));
			edge_vec.push_back(make_pair(it2->index, ridx));
			if (is_horizontal) {
			    weight_vec.push_back(-it2->w);
			} else {
			    weight_vec.push_back(-it2->h);
			}
			// ensure no overlap
			auto it_idx = std::find(boxes.begin(), boxes.end(), *it2);
		    while (it2 != orders.begin() && overlap_any[it_idx-boxes.begin()]) {
		    	--it2;
				it_idx = std::find(boxes.begin(), boxes.end(), *it2);
				// if (!overlap_any[it2->index]) continue;
				if (!overlap_any[it_idx-boxes.begin()]) continue;
				// if (boxes[idx].hor_overlapping(boxes[it2->index]) && boxes[idx].ver_overlapping(boxes[it2->index])) continue;
				if (boxes[idx].hor_overlapping(*it2) && boxes[idx].ver_overlapping(*it2)) continue;
			    M_DEBUG(
		    	cout<<"device "<<ridx+1<<" has left neighbor "<<it2->index+1<<endl;
				);
				// edge_vec.push_back(make_pair(it2->index, idx));
				edge_vec.push_back(make_pair(it2->index, ridx));
			    if (is_horizontal) {
			        weight_vec.push_back(-it2->w);
			    } else {
			        weight_vec.push_back(-it2->h);
			    }
		    }
		}
	} else {
		// s points to it 
		if (cand[boxes.size()] == (int)idx || overlap_any[idx]) {
			M_DEBUG(
		    cout<<"device "<<ridx+1<<" pointed by s"<<endl;
			);
			// edge_vec.push_back(make_pair(boxes.size(), idx));
			edge_vec.push_back(make_pair(vtcs.size(), ridx));
			weight_vec.push_back(0);
		}
	}
	orders.erase(it);
	// after erasing, orders maintain
}
//}}}

void NLPlace::get_cg_edges_no_redundant(vector<dBox> const& boxes_sorted, vector< Edge > & edge_vec, vector<double> & weight_vec, vector<bool> & visited, bool is_horizontal, int n, bool edge_inited)
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	);
	for (auto it=boxes_sorted.begin(); it!=boxes_sorted.end(); ++it) {
		dSegment seg;
		if (is_horizontal) {
			seg = dSegment(it->y, it->y+it->h); // the segment
		}
		else {
			seg = dSegment(it->x, it->x+it->w); // the segment
		}
		auto it2 = it+1;

		M_DEBUG(
		cout<<"device "<<(it->index+1)<<" seg: ("<<it->x<<", "<<it->y<<", "<<it->w<<", "<<it->h<<")"<<endl;
		cout<<"device "<<(it->index+1)<<" seg: ("<<seg.segs[0].first<<", "<<seg.segs[0].second<<")"<<endl;
		);
		bool has_neighbor = false;
		while ( !seg.segs.empty() && it2!=boxes_sorted.end() ) {
			M_DEBUG (
			for (auto s : seg.segs) {
			    cout<<"seg: ("<<s.first<<", "<<s.second<<")"<<endl;
			}
			);
			// must be on the right of the current box
			// NLP version: change to consider overlap before legalization
			int ovl = it->overlapping(*it2);
			if ( (is_horizontal && it2->x < (it->x+it->w) && (ovl==0 || ovl==3 || ovl==4)) ||
					((!is_horizontal) && it2->y < (it->y+it->h) && (ovl==0 || ovl==1 || ovl==2)) ) {
				++it2;
				continue;
			}
			// if overlap
			if (ovl != 0) {
				if (is_horizontal) {
					if (ovl == 1) {
						// move to left
				        visited[it->index] = true;
				        edge_vec.push_back( std::make_pair(it2->index, it->index) );
				        weight_vec.push_back(-it2->w); // the weight is the width of the current box
				        ++it2;
						continue;
					}
					else {
						assert(ovl == 2);
					}
				}
				else {
					if (ovl == 3) {
						// move to left
				        visited[it->index] = true;
				        edge_vec.push_back( std::make_pair(it2->index, it->index) );
				        weight_vec.push_back(-it2->h); // the weight is the height of the current box
				        ++it2;
						continue;
					}
					else {
						assert(ovl == 4);
					}
				}
			}
			if ( (is_horizontal && seg.trim(it2->y, it2->y+it2->h)) ||
					((!is_horizontal) && seg.trim(it2->x, it2->x+it2->w)) ) {
				// has overlap
				has_neighbor = true;
				visited[it2->index] = true;
				M_DEBUG (
				cout<<"visited["<<(it2->index+1)<<"] = true"<<endl;
				);
				edge_vec.push_back( std::make_pair(it->index, it2->index) );
				if (is_horizontal) {
					weight_vec.push_back(-it->w); // the weight is the width of the current box
				}
				else {
					weight_vec.push_back(-it->h); // the weight is the height of the current box
				}
				// cout<<"device "<<(it->index+1)<<" has right neighbor: device "<<(it2->index+1)<<endl;
			}
			++it2;
		} // end while 
		// no neighbors, then it has an edge to t
		if ( !has_neighbor ) {
			// if edge_inited is true, then the devices cannot both be type 2
			edge_vec.push_back( std::make_pair(it->index,n+1) );
			if (is_horizontal) {
				weight_vec.push_back(-it->w);
			}
			else {
				weight_vec.push_back(-it->h);
			}
			// cout<<"device "<<(it->index+1)<<" has no right neighbor, point to t"<<endl;
		}
	}
	// add 2 0-cost edges for each symmetric pairs
}
//}}}

void NLPlace::constraint_graph(vector< Edge >& edge_vec, vector<double>& weight_vec, graph_t& g, int nnum) 
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	);
	// size_t n = boxes.size();
	// 2 nodes for s and t, source node id: n, destination node id: n+1
	// 1 node for each symmetric group
    // int num_nodes = n + 2 + (int)symmetric_groups.size();
	
    g = graph_t(edge_vec.begin(), edge_vec.end(), weight_vec.begin(), nnum);
}
//}}}

void NLPlace::solve_shortest_path(vector<dBox> const& boxes, bool is_horizontal, vector<double> & reconst_vec, double& reconst_dim, graph_t& g, vector<double> & d) 
//{{{
{
	M_DEBUG(
	cout<<__func__<<endl;
	);
	using namespace boost;
	
	size_t n = boxes.size();
	// 2 nodes for s and t, source node id: n, destination node id: n+1
	// 1 node for each symmetric group
    // int num_nodes = n + 2 + (int)symmetric_groups.size();

    property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);

    vector<vertex_descriptor> p(num_vertices(g));
    // vector<double> d(num_vertices(g));
    d = vector<double>(num_vertices(g));
    //vertex_descriptor s = vertex(n, g); // the source node id is n
  
	bool r = bellman_ford_shortest_paths(g, num_vertices(g), weight_map(weightmap).distance_map(&d[0]).predecessor_map(&p[0]));

	if (r) {
        graph_traits < graph_t >::vertex_iterator vbegin, vend, vi;
		tie(vbegin, vend) = vertices(g);
        for (vi=vbegin; vi != vbegin+n; ++vi) {
			reconst_vec.push_back(-d[*vi]);
        }
		reconst_dim = -d[*(vbegin+n+1)];
	}
	M_DEBUG(
	cout<<"is_horizontal: "<<is_horizontal<<endl;
	cout<<"r: "<<r<<endl;
	cout<<"n: "<<n<<endl;
	cout<<"reconst_vec.size(): "<<reconst_vec.size()<<endl;
	);

	// M_DEBUG(
	if (r) {
        cout << "distances and parents:" << endl;
        graph_traits < graph_t >::vertex_iterator vi, vend;
        for (tie(vi, vend) = vertices(g); vi != vend; ++vi) {
			cout << "distance(" << *vi << ") = " << d[*vi] << ", ";
			cout << "parent(" << *vi << ") = " << p[*vi] << endl;
        }
	}
	else {
		cout << "negative cycle" << endl;
	}
	// );

	M_DEBUG(
	cout<<"num_vertices: "<<num_vertices(g)<<endl;
	cout<<"num_edges: "<<num_edges(g)<<endl;
	);
	string file_name;
	if (is_horizontal) file_name = results_dir + "bellman-ford-horizontal.dot";
	else file_name = results_dir + "bellman-ford-vertical.dot";
	std::ofstream dot_file(file_name);
    if (!dot_file.good())
    {
        cout << "failed to open " << file_name << " for write\n"; 
        exit(-1);
    }
  
    dot_file << "digraph D {\n"
      << "  rankdir=LR\n"
      << "  size=\"4,3\"\n"
      << "  ratio=\"fill\"\n"
      << "  edge[style=\"bold\"]\n" << "  node[shape=\"circle\"]\n";
  
	property_map<graph_t, vertex_index_t>::type vertIndx = get(vertex_index, g);

    graph_traits < graph_t >::edge_iterator ei, ei_end;
    for (tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
      graph_traits < graph_t >::edge_descriptor e = *ei;
      graph_traits < graph_t >::vertex_descriptor
        u = source(e, g), v = target(e, g);
      dot_file << vertIndx[u] << " -> " << vertIndx[v]
        << "[label=\"" << get(weightmap, e) << "\"";
      if (p[v] == u)
        dot_file << ", color=\"black\"";
      else
        dot_file << ", color=\"grey\"";
      dot_file << "]";
    }
    dot_file << "}";
}
//}}}

void NLPlace::read_params()
//{{{
{
	// scaanoynmousg factor
	string params_in_file_name = "params.in";
	std::ifstream params_in_file;
    if (!params_in_file.good())
    {
        cout << "failed to open " << params_in_file_name << " for read\n";
        exit(-1);
    }
	params_in_file.open(params_in_file_name);

	std::string anoynmouse;
	while(std::getanoynmouse(params_in_file, anoynmouse)) {
	    std::istringstream iss(anoynmouse);
	    string param_name;
		iss >> param_name;
		if(param_name=="SCAanoynmousG_FACTOR") {
			double sf;
			while(iss >> sf) {
				M_DEBUG(
				cout << "read in scaanoynmousg factor " << sf << endl;
				);
				sfs.push_back(sf);
			}
		} 
	}

	params_in_file.close();
	
}
//}}}

void NLPlace::init_file_names()
//{{{
{
	typedef vector<string> split_vector_t;
	split_vector_t SplitVec;
    boost::algorithm::split(SplitVec, files_path, boost::is_any_of("/"), boost::algorithm::token_compress_on);
	ckt_name = SplitVec[SplitVec.size()-1]; // the last one is the circuit name
	in_block_file_name = files_path+"/"+ckt_name+".block";
	in_net_file_name = files_path+"/"+ckt_name+".net";
	in_symmetry_file_name = files_path+"/"+ckt_name+".sym";
	in_overlap_file_name = files_path+"/"+ckt_name+".ove";
	in_offset_file_name = files_path+"/"+ckt_name+".offset";
	// in_type_file_name = files_path+"/"+ckt_name+".type";
	//in_simple_techfile_name = "./techfile.simple";
	//in_spacing_rules_file_name = "./spacing.rules";
	//in_width_area_rules_file_name = "./width_area.rules";
	in_gds_file_prefix = files_path+"/gds/";
}
//}}}

void NLPlace::read_netlist_files()
//{{{
{
	read_block_file_hypergraph();
	read_net_file_hypergraph();
	read_symmetry_file();
#if HAS_FLEXIBILITY == 1
	read_overlap_file();
#endif
	read_offset_file();
	// read_type_file();
	read_techfile_simple();
	read_spacing_rules_file();
	read_width_area_rules_file();
	parse_gds_files();

	//check symmetric
	M_DEBUG(
	for(size_t i=0; i<symmetric_groups.size(); ++i) {
		cout<<"symmetric group "<<i<<endl;
		cout<<"symmetric pairs: ";
		for(symmetric_pairs_t::iterator it2=symmetric_groups[i].first.begin(); it2!=symmetric_groups[i].first.end(); ++it2) {
			cout<<"("<<it2->first<<","<<it2->second<<"), ";
		}
		cout<<endl;
		cout<<"self symmetric: ";
		for(self_symmetric_t::iterator it2=symmetric_groups[i].second.begin(); it2!=symmetric_groups[i].second.end(); ++it2) {
			cout<<*it2<<", "; 
		}
		cout<<endl;
	}
	);

	M_DEBUG(
	cout<<"finish reading all netlist files"<<endl;
	);
}
//}}}

void NLPlace::read_block_file_hypergraph()
//{{{
{
	ifstream in_block_file;
	in_block_file.open(in_block_file_name);
    if (!in_block_file.good())
    {
        cout << "failed to open " << in_block_file_name << " for read\n";
        exit(-1);
    }

	// NOTE: assume 3 contents in each anoynmouse
	string module_name;
	size_t a, b;
	while(in_block_file >> module_name >> a >> b) {
		size_t vidx = vtcs.size();
		m_vn_ids[module_name] = vidx;
		vtcs.push_back(Vertex(vidx, module_name));
		vtcs.at(vidx).add_dimension(a, b);
	}
	in_block_file.close();
	M_DEBUG(
	cout<<"finish reading block file"<<endl;
	);
}
//}}}

void NLPlace::read_net_file_hypergraph()
//{{{
{
	ifstream in_net_file;
	in_net_file.open(in_net_file_name);
    if (!in_net_file.good())
    {
        cout << "failed to open " << in_net_file_name << " for read\n";
        exit(-1);
    }

	std::string anoynmouse;
	while(std::getanoynmouse(in_net_file, anoynmouse)) {
	    std::istringstream iss(anoynmouse);
	    string net_name;
		iss >> net_name;

		set<size_t> vids_in_net;
		string module_name;
		int pin_pos_x, pin_pos_y;
		// TODO: change to consider exact pin location
	    while(iss >> module_name >> pin_pos_x >> pin_pos_y) {
			vids_in_net.insert(m_vn_ids[module_name]);
		}
		// NOTE: if connects only one component, ignore
		if(vids_in_net.size() > 1) {
			m_en_vids[net_name] = vids_in_net;
		}
	}

	in_net_file.close();
	M_DEBUG(
	cout<<"finish reading net file"<<endl;
	);
}
//}}}

/******** Symmetry file *********/
void NLPlace::read_symmetry_file()
//{{{
{
	ifstream in_symmetry_file;
	in_symmetry_file.open(in_symmetry_file_name);
    if (!in_symmetry_file.good())
    {
        cout << "failed to open " << in_symmetry_file_name << " for read\n";
        exit(-1);
    }

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
			cout<<"Symmetric pair: "<<vtcs[it->first].name<<", "<<vtcs[it->second].name<<endl;
			limboAssertMsg(vtcs[it->first].dimensions == vtcs[it->second].dimensions, "symmetric pairs size different error");
		}
	}
	
	M_DEBUG(
	cout<<"finish reading symmetry file"<<endl;
	);
}
//}}}

void NLPlace::read_overlap_file()
//{{{
{
	ifstream in_overlap_file;
	in_overlap_file.open(in_overlap_file_name);
    if (!in_overlap_file.good())
    {
        cout << "failed to open " << in_overlap_file_name << " for read\n";
        exit(-1);
    }

	set<string> tra_names, cap_names;
	std::string anoynmouse;
	bool cap_flag = false;
	while(std::getanoynmouse(in_overlap_file, anoynmouse)) {
	    std::istringstream iss(anoynmouse);
		string a = "";
		while (iss >> a) {
			if (!cap_flag) { tra_names.insert(a); }
			else { cap_names.insert(a); }
		}
		// if empty anoynmouse
		if (a == "") {
			cap_flag = true;
		}
	}
	// assert(tra_names.size()>0);
	// assert(cap_names.size()>0);
	M_DEBUG(
	cout<<"tra_names: ";
	for (auto name : tra_names) {
		cout<<name<<", ";
	}
	cout<<endl;
	cout<<"cap_names: ";
	for (auto name : cap_names) {
		cout<<name<<", ";
	}
	cout<<endl;
	);

	for (auto v : vtcs) {
		if (tra_names.find(v.name) != tra_names.end()) {
			dtype.push_back(0);
		} else if (cap_names.find(v.name) != cap_names.end()) {
			dtype.push_back(1);
		} else {
			dtype.push_back(2);
		}
	}
	for (size_t i=0; i<vtcs.size(); ++i) {
		for (size_t j=i+1; j<vtcs.size(); ++j) {
			// NOTE: add this for multi-layer flexibility, (0, 1) or (1, 0)
			if (dtype[i] + dtype[j] == 1) { 
				M_DEBUG(
				cout<<"device "<<vtcs[i].name<<" and device "<<vtcs[j].name<<" has flexibility"<<endl;
				);
			}
		}
	}
	in_overlap_file.close();
}
//}}}

void NLPlace::parse_gds_file(size_t index, string file_name)
//{{{
{
	std::map<int, vector< rectangle_type_d > > rectangles;
	
    GdsParser::GdsDB::GdsDB db; 
    GdsParser::GdsDB::GdsReader reader (db); 
    reader(file_name);

	string block_name = vtcs[index].name;
    for (vector<GdsCell>::const_iterator it = db.cells().begin(); it != db.cells().end(); ++it) {
		M_DEBUG(
        cout<<"cell: "<<it->name()<<endl; 
		);
		vtcs[index].gds_cell = new GdsCell(*it);
		// NOTE: assume flat GDSII
		for(vector<GdsCell::object_entry_type>::const_iterator oit = it->objects().begin(); oit != it->objects().end(); ++oit) {
			if(oit->first == ::GdsParser::GdsRecords::BOUNDARY) {
				GdsPolygon* ptr = dynamic_cast<GdsPolygon*>(oit->second); 
				int lay = ptr->layer();
				// 17: PO layer; 30: CO layer; 31-44: M1-M14 layers; 51-63: VIA1-VIA13 layers; 75: PDK layer
				// if(lay == 17 || (lay >= 30 && lay <= 44) || (lay >= 51 && lay <= 63) ||  lay == 75) continue;
				// if(lay == 75) continue;
				if(lay == 75 || lay == 208) continue;
				polygon_type::iterator_type pit;
				vector< std::pair<coordinate_type, coordinate_type> > points;
				/*
				if(ptr->end()-ptr->begin() == 5) {
					M_DEBUG(
					cout<<"a rectangle on layer "<<ptr->layer()<<endl;
					);
					for(pit=ptr->begin(); pit!=ptr->end(); ++pit) {
						points.push_back(std::make_pair(pit->x(), pit->y()));
					}
				} else {
					cout<<"polygon is not a rectangle"<<endl;
				}
				*/
				// find the greatest and smallest point value
				double xl=std::numeric_limits<double>::max(), yl=std::numeric_limits<double>::max(), xh=std::numeric_limits<double>::min(), yh=std::numeric_limits<double>::min();
				for(pit=ptr->begin(); pit!=ptr->end(); ++pit) {
			        xl = std::min(xl, (double)pit->x() * gds_scal_fac);
			        yl = std::min(yl, (double)pit->y() * gds_scal_fac);
			        xh = std::max(xh, (double)pit->x() * gds_scal_fac);
			        yh = std::max(yh, (double)pit->y() * gds_scal_fac);
				}

				// offset, scale 
				double offset_x = vtcs[index].offset.first;
				double offset_y = vtcs[index].offset.second;
				/*
				double xl = (double)points[0].first * gds_scal_fac;
				double yl = (double)points[0].second * gds_scal_fac;
				double xh = (double)points[2].first * gds_scal_fac;
				double yh = (double)points[2].second * gds_scal_fac;
				*/
				M_DEBUG(
				cout<<"xl: "<<xl<<", yl: "<<yl<<", xh: "<<xh<<", yh: "<<yh<<", offset_x: "<<offset_x<<", offset_y: "<<offset_y<<endl;
				);
				rectangle_type_d rect(xl-offset_x, yl-offset_y, xh-offset_x, yh-offset_y);
				if(rectangles.find(lay) == rectangles.end()) {
					rectangles[lay] = vector< rectangle_type_d >();
				}
				rectangles[lay].push_back(rect);
			} // if BOUNDARY
		} // for objects
	} // for cells

	M_DEBUG(
	cout<<"block: "<<block_name<<endl;
	);
	// merge same layer, initialize vertex feol_layers
	for(auto it=rectangles.begin(); it!=rectangles.end(); ++it) {
		double xl=std::numeric_limits<double>::max(), yl=std::numeric_limits<double>::max(), xh=std::numeric_limits<double>::min(), yh=std::numeric_limits<double>::min();
		for(auto rect : it->second) {
			xl = std::min(xl, gtl::xl(rect));
			yl = std::min(yl, gtl::yl(rect));
			xh = std::max(xh, gtl::xh(rect));
			yh = std::max(yh, gtl::yh(rect));
		}
		vtcs[index].feol_layers.push_back(make_pair(it->first, rectangle_type_d(xl, yl, xh, yh)));
		M_DEBUG(
		cout<<"layer "<<it->first<<": ("<<xl<<", "<<yl<<", "<<xh<<", "<<yh<<")"<<endl;
		);
	}
}
//}}}

void NLPlace::parse_gds_files()
//{{{
{
	for(size_t i=0; i<vtcs.size(); ++i) {
		parse_gds_file(i, in_gds_file_prefix+vtcs[i].name+".gds");
	}
}
//}}}

void NLPlace::read_offset_file()
//{{{ 
{
	ifstream in_offset_file;
	in_offset_file.open(in_offset_file_name);
    if (!in_offset_file.good())
    {
        cout << "failed to open " << in_offset_file_name << " for read\n";
        exit(-1);
    }

	// NOTE: assume 3 contents in each anoynmouse
	string module_name;
	double offset_x, offset_y;
	while(in_offset_file >> module_name >> offset_x >> offset_y) {
		size_t vidx = m_vn_ids[module_name];
		vtcs[vidx].offset = make_pair(offset_x, offset_y);
	}
	in_offset_file.close();
	M_DEBUG(
	cout<<"finish reading offset file"<<endl;
	);
}
//}}}

void NLPlace::read_techfile_simple()
//{{{
{
	ifstream in_simple_techfile;
	in_simple_techfile.open(in_simple_techfile_name);
    if (!in_simple_techfile.good())
    {
        cout << "failed to open " << in_simple_techfile_name << " for read\n";
        exit(-1);
    }
	
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

void NLPlace::read_spacing_rules_file()
//{{{
{
	ifstream in_spacing_rules_file;
	in_spacing_rules_file.open(in_spacing_rules_file_name);
    if (!in_spacing_rules_file.good())
    {
        cout << "failed to open " << in_spacing_rules_file_name << " for read\n";
        exit(-1);
    }

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

void NLPlace::read_width_area_rules_file()
//{{{
{
	ifstream in_width_area_rules_file;
	in_width_area_rules_file.open(in_width_area_rules_file_name);
    if (!in_width_area_rules_file.good())
    {
        cout << "failed to open " << in_width_area_rules_file_name << " for read\n";
        exit(-1);
    }

	int width, area;
	size_t num = layers.size();
	for(size_t i=0; i<num; ++i) {
		in_width_area_rules_file >> width >> area; 
		width_rules.push_back(width);
		area_rules.push_back(area);
	}

	in_width_area_rules_file.close();
	M_DEBUG(
	cout<<"finish reading width area rules file"<<endl;
	);
}
//}}}

std::pair<double, limbo::solvers::SolverProperty> 
NLPlace::solve_lp(vector< Edge > const& edge_vec, bool is_horizontal, int n, vector<dBox> const& boxes, vector<double>& reconst_vec, double reconst_dim, bool opt_hpwl, bool opt_area) 
//{{{
{
	cout<<"solve_lp: ";
	if(is_horizontal) { cout<<"horizontal"<<endl; }
	else { cout<<"vertical"<<endl; }
    char buf[256]; 
    sprintf(buf, "%s takes %%t seconds CPU, %%w seconds real\n", __func__);
	boost::timer::auto_cpu_timer t (buf);
	// use Gurobi solver
	assert((opt_hpwl && !opt_area) || (!opt_hpwl && opt_area));
	int num_nets = m_en_vids.size(); 
    // ILP model 
    typedef limbo::solvers::anoynmousearModel<double, double> model_type; 
    model_type lp_model; 
	
    // objective 
    model_type::expression_type obj; 
    // add variables 
    int num_vars = n + num_nets*2*opt_hpwl + 1*opt_area; 
    lp_model.reserveVariables(num_vars); 
	// add coordinate variables
	// NOTE: the x variables here are general location variables
    std::vector<model_type::variable_type> x (n); 
    for (int i = 0; i < n; ++i)
    {
        x[i] = lp_model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "x_"+std::to_string(i)); 
    }

    std::vector<model_type::variable_type> wll (num_nets);
    std::vector<model_type::variable_type> wlr (num_nets);
	// add HPWL variables
	if (opt_hpwl) {
        for (int i = 0; i < num_nets; ++i)
        {
            wll[i] = lp_model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "wll_"+std::to_string(i));
            wlr[i] = lp_model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "wlr_"+std::to_string(i));
        }
	}
    model_type::variable_type dim; 
	if (opt_area) {
		dim = lp_model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "dim");
	}

	// add boundary constraints
	for (int i=0; i<n; ++i) {
		if (!opt_area) {
		    if (is_horizontal) {
                lp_model.addConstraint(x[i] <= reconst_dim-boxes[i].w); 
		    }
		    else {
                lp_model.addConstraint(x[i] <= reconst_dim-boxes[i].h); 
		    }
		}
		else {
		    if (is_horizontal) {
		    	lp_model.addConstraint(x[i] - dim <= -boxes[i].w);
		    }
		    else {
		    	lp_model.addConstraint(x[i] - dim <= -boxes[i].h);
		    }
		}
	}

	// add topology constraints
	for (Edge e : edge_vec) {
		// the s, t constraints
		if ((int)e.first == n || (int)e.second == n+1) continue;
		// FIXME: here I did not consider rotation
		size_t src = e.first, tgt = e.second;
		double xh_dim;
		if(is_horizontal) { xh_dim = boxes[src].w; } 
		else { xh_dim = boxes[src].h; }
		double spacing = 0;
		for(auto it=vtcs[src].feol_layers.begin(); it!=vtcs[src].feol_layers.end(); ++it) {
			size_t layer_id1 = layer_num2id[it->first];
			double xh;
			if(is_horizontal) { xh = gtl::xh(it->second); }
			else { xh = gtl::yh(it->second); }
			double xh_dist = xh_dim - xh;
		    for(auto it2=vtcs[tgt].feol_layers.begin(); it2!=vtcs[tgt].feol_layers.end(); ++it2) {
				size_t layer_id2 = layer_num2id[it2->first];
				// FIXME
				// if(layer_id1 == layer_id2) continue;
				string layer_name = layers[layer_id1].second;
				if(layer_name.at(0) != 'M' && layer_id1 == layer_id2) continue;
				double xl;
			    if(is_horizontal) { xl = gtl::xl(it2->second); }
			    else { xl = gtl::yl(it2->second); }
				int rule = spacing_rules[layer_id1][layer_id2]; 
				if(rule != -1) {
					M_DEBUG(
					cout<<"it2->second: "<<bg::dsv(it2->second)<<endl;
					cout<<"xl: "<<xl<<", xh_dist: "<<xh_dist<<endl;
					cout<<"spacing between "<<layers[layer_id1].second<<" and "<<layers[layer_id2].second<<" is "<<(double)rule*gds_scal_fac<<endl;
					);
					spacing = std::max(spacing, (double)rule*gds_scal_fac - xh_dist - xl);
				}
			}
		}
		M_DEBUG(
		cout<<"spacing: "<<spacing<<endl;
		);
	    lp_model.addConstraint(x[e.first] - x[e.second] <= -xh_dim-spacing);
	}

	// add symmetric constraints
	if (!is_horizontal) {
		for (size_t i=0; i<symmetric_groups.size(); ++i) {
			symmetric_pairs_t ps = symmetric_groups[i].first;
			for (symmetric_pair_t p : ps) {
		        lp_model.addConstraint(x[p.first] - x[p.second] == 0.0);
			}
		}
	} else {
		for (size_t i=0; i<symmetric_groups.size(); ++i) {
            model_type::variable_type xsym = lp_model.addVariable(0, std::numeric_limits<double>::max(), limbo::solvers::CONTINUOUS, "xsym_"+std::to_string(i)); 
			symmetric_pairs_t ps = symmetric_groups[i].first;
			self_symmetric_t ss = symmetric_groups[i].second;
			for (symmetric_pair_t p : ps) {
		        lp_model.addConstraint(x[p.first]+x[p.second] - 2*xsym == -boxes[p.first].w);
			}
			for (size_t s : ss) {
		        lp_model.addConstraint(2*x[s] - 2*xsym == -boxes[s].w);
			}
		}
	}

	// add HPWL constraints
	if (opt_hpwl) {
	    int net_cnt = 0;
	    for (auto eit : m_en_vids) {
	    	set<size_t> devices = eit.second;
	    	for (auto d : devices) {
                if (is_horizontal) {
                    lp_model.addConstraint(2*wll[net_cnt] - 2*x[d] <= boxes[d].w);
                    lp_model.addConstraint(2*wlr[net_cnt] - 2*x[d] >= boxes[d].w);
                }
                else {
                    lp_model.addConstraint(2*wll[net_cnt] - 2*x[d] <= boxes[d].h);
                    lp_model.addConstraint(2*wlr[net_cnt] - 2*x[d] >= boxes[d].h);
                }
	    	}
	    	++net_cnt;
	    }

	    // add objectives
        for (int i = 0; i < num_nets; ++i) {
            obj += wlr[i]-wll[i]; 
        }
	}

	if (opt_area) {
        obj += dim; 
	}
    lp_model.setObjective(obj); 
    lp_model.setOptimizeType(limbo::solvers::MIN);

	M_DEBUG(
	lp_model.print(results_dir+"test_lp_"+std::to_string(is_horizontal)+".lp");
	);

    limbo::solvers::SolverProperty optimstatus; 

    switch (optimizer_type) 
    {
        case OptimizerType::GUROBI:
            {
#ifdef USE_GUROBI
                // solve by Gurobi 
                typedef limbo::solvers::GurobianoynmousearApi<model_type::coefficient_value_type, model_type::variable_value_type> solver_type; 
                solver_type solver (&lp_model); 
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
                solver_type solver (&lp_model); 
                limbo::solvers::LPSolveParameters params; 
                params.setVerbose(SEVERE); 

                optimstatus = solver(&params); 
            }
            break; 
        default:
            cout << "unknown optimizer_type " << int(optimizer_type) << endl; 
            assert(0); 
    }
	
	if (optimstatus == limbo::solvers::UNBOUNDED) {
		cout<<"Solver infinity or unbounded"<<endl;
	} else if (optimstatus == limbo::solvers::OPTIMAL) {
		double objval = lp_model.evaluateObjective();
		cout << "Optimal objective: " << objval << endl;
		for (int i=0; i<n; ++i) {
			reconst_vec.push_back(lp_model.variableSolution(x[i]));
		}
	} 
	else if (optimstatus == limbo::solvers::INFEASIBLE) {
		cout << "lp_model is infeasible" << endl;
	} else {
		cout << "Optimization was stopped with status = "
			<< optimstatus << endl;
	}

	if (opt_area) {
		double objval = lp_model.evaluateObjective();
		return std::make_pair(objval, optimstatus);
	}
	else {
		return std::make_pair(0.0, optimstatus);
	}
}
//}}}

void NLPlace::write_gds()
//{{{
{
	string test_gds_fname = in_gds_file_prefix+vtcs[0].name+".gds";
    GdsParser::GdsDB::GdsDB test_db; 
    GdsParser::GdsDB::GdsReader reader (test_db); 
    reader(test_gds_fname);
	
	GdsParser::GdsDB::GdsDB db = test_db;
	db.cells().clear();

	for(size_t i=0; i<vtcs.size(); ++i) {
		db.addCell(*vtcs[i].gds_cell);
		M_DEBUG(
		cout<<"cell name: "<<vtcs[i].gds_cell->name()<<", module name: "<<vtcs[i].name<<endl;
		);
	}
	GdsCell& cell = db.addCell(ckt_name);
	for(size_t i=0; i<vtcs.size(); ++i) {
		point_type pt ( lround((locs[i].first-vtcs[i].offset.first)*uu2gdsu), lround((locs[i].second-vtcs[i].offset.second)*uu2gdsu) );
		// cell.addCellReference(vtcs[i].name, pt, 0.0, 1.0, 0);
		cell.addCellReference(vtcs[i].name, pt, 0.0, std::numeric_limits<double>::max(), 0);
	}
	
	// hierarchical
	GdsParser::GdsDB::GdsWriter writer (db);
	writer(results_dir+ckt_name+".place.gds");
    
    // write flattened cell 
	/*
	GdsParser::GdsDB::GdsCell flatCell = db.extractCell(ckt_name);
	GdsParser::GdsDB::GdsDB flatDB; 
    flatDB.addCell(flatCell); 
    GdsParser::GdsDB::GdsWriter flatGw (flatDB); 
    flatGw(results_dir+ckt_name+".flat.place.gds");
	*/
	
	for(size_t i=0; i<vtcs.size(); ++i) {
		delete vtcs[i].gds_cell;
	}
}
//}}}

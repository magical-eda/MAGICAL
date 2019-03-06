/*************************************************************************
    > File Name: main.cpp
    > Author: anoynmous anoynmous
    > Mail: anoynmous@anoynmous
    > Created Time: Mon 04 Jun 2018 05:27:08 PM CDT
 ************************************************************************/

#include <iostream>
#include <limbo/programoptions/ProgramOptions.h>
#include <limbo/string/String.h>
#include "nlp.h"

using std::cout;
using std::endl;

int main(int argc, char** argv)
{
    char buf[256]; 
    sprintf(buf, "idea_place takes %%t seconds CPU, %%w seconds real\n");
	boost::timer::auto_cpu_timer t (buf);

    // append options here 
    typedef limbo::programoptions::ProgramOptions po_type;
    using limbo::programoptions::Value;
    bool help = false;
    std::string files_path; 
    std::string techfile_path; 
    std::string spacing_rules_path; 
    std::string width_area_rules_path; 
    std::string results_dir; 
    std::string optimizer_type_str = "GUROBI"; 
    OptimizerType optimizer_type = OptimizerType::GUROBI; 
    po_type desc (std::string("Available options"));
    desc.add_option(Value<bool>("--help", &help, "print help message").default_value(help).help(true))
        .add_option(Value<std::string>("--input", &files_path, "input files directory"))
        .add_option(Value<std::string>("--techfile", &techfile_path, "input technology file"))
        .add_option(Value<std::string>("--spacing", &spacing_rules_path, "input spacing rule file"))
        .add_option(Value<std::string>("--width_area", &width_area_rules_path, "input width and area rule file"))
        .add_option(Value<std::string>("--result", &results_dir, "output result directory"))
        .add_option(Value<std::string>("--optimizer", &optimizer_type_str, "optimization solver GUROBI|LPSOLVE").default_value(optimizer_type_str))
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

	cout<<"***** debugging parameters *****"<<endl;
    cout<<"techfile_path: "<<techfile_path<<endl; 
    cout<<"spacing_rules_path: "<<spacing_rules_path<<endl; 
    cout<<"width_area_rules_path: "<<width_area_rules_path<<endl;
	cout<<"files_path: "<<files_path<<endl;
	cout<<"results_dir: "<<results_dir<<endl;
    cout<<"optimizer_type: "<<optimizer_type_str<<endl; 

	NLPlace placer(techfile_path, spacing_rules_path, width_area_rules_path, files_path, results_dir, optimizer_type);

	placer.run();
	// placer.solve();
	// placer.solve_unconstrained();

	return 0;
}

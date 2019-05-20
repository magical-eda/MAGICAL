/**
 @file   VerilogDataBase.h
 @author Yibo Lin
 @date   Oct 2014
 @brief  Implementation of @ref VerilogParser::VerilogDataBase
 */

#include "VerilogDataBase.h"
#include <cstdlib>

namespace VerilogParser {

void VerilogDataBase::verilog_assignment_cbk(std::string const& /*target_name*/, Range const& /*target_range*/, std::string const& /*source_name*/, Range const& /*source_range*/)
{
	verilog_user_cbk_reminder(__func__);
}

void VerilogDataBase::verilog_user_cbk_reminder(const char* str) const 
{
    std::cout << "A corresponding user-defined callback is necessary: " << str << std::endl;
	exit(0);
}

} // namespace VerilogParser

/**
 @file   DefDataBase.cc
 @author Yibo Lin
 @date   Dec 2014
 @brief  Implementation of @ref DefParser::DefDataBase
 */

#include "DefDataBase.h"
#include <cstring>
#include <cstdlib>

namespace DefParser {

void DefDataBase::resize_def_blockage(int) 
{
	def_user_cbk_reminder(__func__);
}
void DefDataBase::add_def_placement_blockage(int, int, int, int)
{
	def_user_cbk_reminder(__func__);
}
void DefDataBase::add_def_routing_blockage(int, int, int, int)
{
	def_user_cbk_reminder(__func__);
}
void DefDataBase::def_user_cbk_reminder(const char* str) const 
{
	cout << "A corresponding user-defined callback is necessary: " << str << endl;
	exit(0);
}

}

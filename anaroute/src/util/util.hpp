/**
 * @file   util.hpp
 * @brief  Utils
 * @author Hao Chen
 * @date   09/22/2019
 *
 **/

#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

namespace util {

  // sys
  extern void       showSysInfo();

  // filename
  extern bool       existFile (const String_t& filename);
  extern String_t   getFileDirName(const String_t& filePathName);
  extern String_t   getFileName(const String_t& filePathName);

	// memory
	extern double     getPeakMemoryUsage();
  extern double     getCurrMemoryUsage();
  extern void       showMemoryUsage(bool flag);

  // string
  extern void       splitString(const char* str, const String_t& delims, Vector_t<String_t>& tokens);
  extern void       splitString(const String_t& s, const String_t& delims, Vector_t<String_t>& tokens);
  extern void       splitString(const char* str, const char c, Vector_t<String_t>& tokens);
  extern void       trimString(String_t& str);
}

PROJECT_NAMESPACE_END

#endif /// _UTIL_HPP_

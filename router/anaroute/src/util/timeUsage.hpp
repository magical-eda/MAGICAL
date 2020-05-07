/**
 * @file   timeUsage.hpp
 * @brief  Time Usage
 * @author Hao Chen
 * @date   10/09/2019
 *
 **/

#ifndef _UTIL_TIME_USAGE_HPP_
#define _UTIL_TIME_USAGE_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

class TimeState {
 public:
  TimeState(double r = 0, double u = 0, double s = 0)
    : realTime(r), userTime(u), sysTime(s) {}
  double realTime, userTime, sysTime;
};

class TimeUsage {
 public:
  TimeUsage();
  
  enum TimeType { FULL, PARTIAL };
  
  void start(TimeType type);
  void showUsage(const char* comment, TimeType type);

 private:
  TimeState diff(TimeState& start, TimeState& end);
  void checkUsage(TimeState& st) const;
  TimeState _fullStart, _partialStart; // total, period
};

PROJECT_NAMESPACE_END

#endif /// _UTIL_TIME_USAGE_HPP_

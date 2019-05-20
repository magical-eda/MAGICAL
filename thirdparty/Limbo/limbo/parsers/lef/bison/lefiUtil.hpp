/*
 *     This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 *  Distribution,  Product Version 5.7, and is subject to the Cadence
 *  LEF/DEF Open Source License Agreement.   Your  continued  use  of
 *  this file constitutes your acceptance of the terms of the LEF/DEF
 *  Open Source License and an agreement to abide by its  terms.   If
 *  you  don't  agree  with  this, you must remove this and any other
 *  files which are part of the distribution and  destroy any  copies
 *  made.
 *
 *     For updates, support, or to become part of the LEF/DEF Community,
 *  check www.openeda.org for details.
 */
/**
 * @file lefiUtil.hpp
 */

#ifndef lefiUtil_h
#define lefiUtil_h

namespace LefParser {

// Structure to return Macro data to FE
struct lefiPoints {
  double x;
  double y;
};

typedef struct lefiPoints lefiNum;

int lefiValidTime();
int lefiValidUser();

//char* lefiUser();

char* lefiOrientStr(int orient);

//double convert_name2num(char *versionName);

} // namespace LefParser

#endif


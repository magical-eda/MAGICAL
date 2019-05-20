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

#include <string.h>
#include <stdlib.h>
//#include "lex.h"
#include <limbo/parsers/lef/bison/lefiUnits.hpp>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {

////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiUnits
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiUnits::lefiUnits() {
  this->lefiUnits::Init();
}


void lefiUnits::Init() {
	this->databaseName_ = 0; 
  this->lefiUnits::clear();
}


void lefiUnits::Destroy() {
  this->lefiUnits::clear();
}


lefiUnits::~lefiUnits() {
  this->lefiUnits::Destroy();
}


void lefiUnits::setDatabase(const char* name, double num) {
  int len = strlen(name) + 1;
  this->databaseName_ = (char*)lefMalloc(len);
  strcpy(this->databaseName_, CASE(name));
  this->databaseNumber_ = num;
  this->hasDatabase_ = 1;
}


void lefiUnits::clear() {
  if (this->databaseName_) lefFree(this->databaseName_);
  this->hasTime_ = 0;
  this->hasCapacitance_ = 0;
  this->hasResistance_ = 0;
  this->hasPower_ = 0;
  this->hasCurrent_ = 0;
  this->hasVoltage_ = 0;
  this->hasDatabase_ = 0;
  this->hasFrequency_ = 0;
  this->databaseName_ = 0;
}


void lefiUnits::setTime(double num) {
  this->hasTime_ = 1;
  this->time_ = num;
}


void lefiUnits::setCapacitance(double num) {
  this->hasCapacitance_ = 1;
  this->capacitance_ = num;
}


void lefiUnits::setResistance(double num) {
  this->hasResistance_ = 1;
  this->resistance_ = num;
}


void lefiUnits::setPower(double num) {
  this->hasPower_ = 1;
  this->power_ = num;
}


void lefiUnits::setCurrent(double num) {
  this->hasCurrent_ = 1;
  this->current_ = num;
}


void lefiUnits::setVoltage(double num) {
  this->hasVoltage_ = 1;
  this->voltage_ = num;
}


void lefiUnits::setFrequency(double num) {
  this->hasFrequency_ = 1;
  this->frequency_ = num;
}


int lefiUnits::hasDatabase() const {
  return this->hasDatabase_;
}


int lefiUnits::hasCapacitance() const {
  return this->hasCapacitance_;
}


int lefiUnits::hasResistance() const {
  return this->hasResistance_;
}


int lefiUnits::hasPower() const {
  return this->hasPower_;
}


int lefiUnits::hasCurrent() const {
  return this->hasCurrent_;
}


int lefiUnits::hasVoltage() const {
  return this->hasVoltage_;
}


int lefiUnits::hasFrequency() const {
  return this->hasFrequency_;
}


int lefiUnits::hasTime() const {
  return this->hasTime_;
}


const char* lefiUnits::databaseName() const {
  return this->databaseName_;
}


double lefiUnits::databaseNumber() const {
  return this->databaseNumber_;
}


double lefiUnits::capacitance() const {
  return this->capacitance_;
}


double lefiUnits::resistance() const {
  return this->resistance_;
}


double lefiUnits::power() const {
  return this->power_;
}


double lefiUnits::current() const {
  return this->current_;
}


double lefiUnits::time() const {
  return this->time_;
}


double lefiUnits::voltage() const {
  return this->voltage_;
}


double lefiUnits::frequency() const {
  return this->frequency_;
}



void lefiUnits::print(FILE* f) const {
  fprintf(f, "Units:\n");
  if (this->lefiUnits::hasTime())
    fprintf(f, "  %g nanoseconds\n", this->lefiUnits::time());
  if (this->lefiUnits::hasCapacitance())
    fprintf(f, "  %g picofarads\n", this->lefiUnits::capacitance());
  if (this->lefiUnits::hasResistance())
    fprintf(f, "  %g ohms\n", this->lefiUnits::resistance());
  if (this->lefiUnits::hasPower())
    fprintf(f, "  %g milliwatts\n", this->lefiUnits::power());
  if (this->lefiUnits::hasCurrent())
    fprintf(f, "  %g milliamps\n", this->lefiUnits::current());
  if (this->lefiUnits::hasVoltage())
    fprintf(f, "  %g volts\n", this->lefiUnits::voltage());
  if (this->lefiUnits::hasFrequency())
    fprintf(f, "  %g frequency\n", this->lefiUnits::frequency());
  if (this->lefiUnits::hasDatabase())
    fprintf(f, "  %s %g\n", this->lefiUnits::databaseName(),
	  this->lefiUnits::databaseNumber());
}

} // namespace LefParser

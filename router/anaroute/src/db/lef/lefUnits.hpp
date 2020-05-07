/**
 * @file   lefUnits.hpp
 * @brief  Technology configuration - Lef Units
 * @author Hao Chen
 * @date   09/11/2019
 *
 **/

#ifndef _DB_LEF_UNITS_HPP_
#define _DB_LEF_UNITS_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

class LefUnits {
  friend class LefReader;

 public:
  LefUnits()
    : _databaseName(""),
      _databaseNumber(100),
      _capacitance(1000000),
      _resistance(10000),
      _time(1000),
      _power(10000),
      _current(10000),
      _voltage(1000),
      _frequency(1) {}
  ~LefUnits() {}
  
  /////////////////////////////////
  //    Getter                   //
  /////////////////////////////////
  const String_t&    databaseName()    const { return _databaseName; }
  Int_t              databaseNumber()  const { return _databaseNumber; }
  Int_t              capacitance()     const { return _capacitance; }
  Int_t              resistance()      const { return _resistance; }
  Int_t              time()            const { return _time; }
  Int_t              power()           const { return _power; }
  Int_t              current()         const { return _current; }
  Int_t              voltage()         const { return _voltage; }
  Int_t              frequency()       const { return _frequency; }

 private:
  String_t    _databaseName;
  Int_t      _databaseNumber;
  Int_t      _capacitance;
  Int_t      _resistance;
  Int_t      _time;
  Int_t      _power;
  Int_t      _current;
  Int_t      _voltage;
  Int_t      _frequency;
  
  /////////////////////////////////
  //    Setter                   //
  /////////////////////////////////
  void setDatabaseName(const String_t& s)    { _databaseName = s; }
  void setDatabaseNumber(const Int_t n)      { _databaseNumber = n; }
  void setCapacitance(const Int_t c)         { _capacitance = c; }
  void setResistance(const Int_t r)          { _resistance = r; }
  void setTime(const Int_t t)                { _time = t; }
  void setPower(const Int_t p)               { _power = p; }
  void setCurrent(const Int_t c)             { _current = c; }
  void setVoltage(const Int_t v)             { _voltage = v; }
  void setFrequency(const Int_t f)           { _frequency = f; }
};


PROJECT_NAMESPACE_END

#endif /// _DB_LEF_UNITS_HPP_


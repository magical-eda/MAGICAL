/**
 * @file   parNetlist.hpp
 * @brief  Netlist parser
 * @author Hao Chen
 * @date   02/03/2020
 *
 **/

#ifndef _PAR_NETLIST_HPP_
#define _PAR_NETLIST_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class NetlistReader {
 public:
  NetlistReader(CirDB& c)
    : _cir(c), _scale(0) {}
  ~NetlistReader() {}

  void parse(const String_t& filename); // old format
  void parse2(const String_t& filename);

 private:
  CirDB& _cir;
  Int_t  _scale;

  void setScale();
  Int_t to_db_unit(const Int_t n) const;
  void updateNetBBox(Net& net, const Pin& pin);

};

PROJECT_NAMESPACE_END

#endif /// _PAR_NETLIST_HPP_

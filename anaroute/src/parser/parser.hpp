/**
 * @file   parser.hpp
 * @brief  Parser manager for database
 * @author Hao Chen
 * @date   09/09/2019
 *
 **/

#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START


class Parser {
public:
  Parser(CirDB& c) : _cir(c) {}
  ~Parser() {}
  
  void parseLef(const String_t& filename);    
  void parseTechfile(const String_t& filename);
  void parseIspd08(const String_t& filename);
  void parseGds(const String_t& filename); 
  void parseSymNet(const String_t& filename);
  void parseIOPin(const String_t& filename);
  void parseNetlist(const String_t& filename);
  void parsePower(const String_t& filename);
  void parseNetSpec(const String_t& filename);

  // patch for the bug from placement
  void correctPinNBlkLoc();

private:
  CirDB&           _cir;
};

PROJECT_NAMESPACE_END

#endif /// _PARSER_HPP_

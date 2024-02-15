/**
 * @file   wrGrGuide.hpp
 * @brief  Output file writer - Global routing guide
 * @author Hao Chen
 * @date   10/18/2019
 *
 **/

#ifndef _WRITER_GR_GUIDE_HPP_
#define _WRITER_GR_GUIDE_HPP_

#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class GrGuideWriter {
 public:
  GrGuideWriter(const CirDB& c)
    : _cir(c) {}
  ~GrGuideWriter() {}
 
  void write(const String_t& filename) const;
  void writeGds(const String_t& netName, const String_t& filename) const;
  void writeGds(const String_t& filename) const;

 private:
  const CirDB& _cir;
};

PROJECT_NAMESPACE_END

#endif /// _WRITER_GR_GUIDE_HPP_


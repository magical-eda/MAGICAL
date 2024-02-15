/**
 * @file   writer.hpp
 * @brief  Output file writer
 * @author Hao Chen
 * @date   10/18/2019
 *
 **/

#ifndef _WRITER_HPP_
#define _WRITER_HPP_

#include "wrGds.hpp"
#include "wrGrGuide.hpp"
#include "wrLayout.hpp"
#include "wrDumb.hpp"

PROJECT_NAMESPACE_START

class Writer {
 public:
  Writer(const CirDB& c)
    : _cir(c),
      _grGuidew(c),
      _layoutw(c),
      _dumbw(c)
    {}
  ~Writer() {}

  // result
  void writeLayoutGds(const String_t& placementFilename, const String_t& outputFilename, const bool bFlatten);

  // guide
  void writeGrGuide(const String_t& filename) const;
  void writeGrGuideGds(const String_t& netName, const String_t& filename) const; // for single net
  void writeGrGuideGds(const String_t& filename) const;
  
  // ....
  void writeDumb(const String_t& placementFilename, const String_t& outputFilename);
  void writeIspd08(const String_t& outputFilename); // for GeniusRoute benchmark

 private:
  const CirDB&  _cir;
  GrGuideWriter _grGuidew;
  LayoutWriter  _layoutw;
  DumbWriter    _dumbw;

};

PROJECT_NAMESPACE_END

#endif /// _WRITER_HPP_

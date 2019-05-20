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
 * @file lefiCrossTalk.hpp
 */

#ifndef lefiCrossTalk_h
#define lefiCrossTalk_h

#include <stdio.h>
//#include <limbo/parsers/lef/bison/lefiKRDefs.hpp>

namespace LefParser {

/* Structure returned for the noise margin callback.
 * This lef construct has two floating point numbers. */
struct lefiNoiseMargin {
  double high;
  double low;
};



class lefiNoiseVictim {
public:
  lefiNoiseVictim(double d);
  void Init(double d);

  void Destroy();
  ~lefiNoiseVictim();

  void clear();

  void addVictimNoise(double d);

  double length();
  int numNoises();
  double noise(int index);

protected:
  double length_;

  int numNoises_;
  int noisesAllocated_;
  double* noises_;
};


class lefiNoiseResistance {
public:
  lefiNoiseResistance();
  void Init();

  void Destroy();
  ~lefiNoiseResistance();

  void clear();

  void addResistanceNumber(double d);
  void addVictimLength(double d);
  void addVictimNoise(double d);

  int numNums();
  double num(int index);

  int numVictims();
  lefiNoiseVictim* victim(int index);

protected:
  int numNums_;
  int numsAllocated_;
  double* nums_;

  int numVictims_;
  int victimsAllocated_;
  lefiNoiseVictim** victims_;
};


class lefiNoiseEdge {
public:
  lefiNoiseEdge();
  void Init();

  void Destroy();
  ~lefiNoiseEdge();

  void clear();

  void addEdge(double d);
  void addResistance();
  void addResistanceNumber(double d);
  void addVictimLength(double d);
  void addVictimNoise(double d);

  double edge();
  int numResistances();
  lefiNoiseResistance* resistance(int index);

protected:
  double edge_;

  int numResistances_;
  int resistancesAllocated_;
  lefiNoiseResistance** resistances_;
};


class lefiNoiseTable {
public:
  lefiNoiseTable();
  void Init();

  void Destroy();
  ~lefiNoiseTable();

  void setup(int i);
  void newEdge();
  void addEdge(double d);
  void addResistance();
  void addResistanceNumber(double d);
  void addVictimLength(double d);
  void addVictimNoise(double d);

  void clear();

  int num();
  int numEdges();
  lefiNoiseEdge* edge(int index);

protected:
  int num_;

  int numEdges_;
  int edgesAllocated_;
  lefiNoiseEdge** edges_;
};


class lefiCorrectionVictim {
public:
  lefiCorrectionVictim(double d);
  void Init(double d);

  void Destroy();
  ~lefiCorrectionVictim();

  void clear();

  void addVictimCorrection(double d);

  double length();
  int numCorrections();
  double correction(int index);

protected:
  double length_;

  int numCorrections_;
  int correctionsAllocated_;
  double* corrections_;
};


class lefiCorrectionResistance {
public:
  lefiCorrectionResistance();
  void Init();

  void Destroy();
  ~lefiCorrectionResistance();

  void clear();

  void addResistanceNumber(double d);
  void addVictimLength(double d);
  void addVictimCorrection(double d);

  int numNums();
  double num(int index);

  int numVictims();
  lefiCorrectionVictim* victim(int index);

protected:
  int numNums_;
  int numsAllocated_;
  double* nums_;

  int numVictims_;
  int victimsAllocated_;
  lefiCorrectionVictim** victims_;
};


class lefiCorrectionEdge {
public:
  lefiCorrectionEdge();
  void Init();
 
  void Destroy();
  ~lefiCorrectionEdge();
 
  void clear();
 
  void addEdge(double d);
  void addResistance();
  void addResistanceNumber(double d);
  void addVictimLength(double d);
  void addVictimCorrection(double d);
 
  double edge();
  int numResistances();
  lefiCorrectionResistance* resistance(int index);
 
protected:
  double edge_;
 
  int numResistances_;
  int resistancesAllocated_;
  lefiCorrectionResistance** resistances_;
};



class lefiCorrectionTable {
public:
  lefiCorrectionTable();
  void Init();

  void Destroy();
  ~lefiCorrectionTable();

  void setup(int i);
  void newEdge();
  void addEdge(double d);
  void addResistance();
  void addResistanceNumber(double d);
  void addVictimLength(double d);
  void addVictimCorrection(double d);

  void clear();

  int num();
  int numEdges();
  lefiCorrectionEdge* edge(int index);

protected:
  int num_;

  int numEdges_;
  int edgesAllocated_;
  lefiCorrectionEdge** edges_;
};

} // namespace LefParser

#endif

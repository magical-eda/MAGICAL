/**
 * @file CSFlow.h
 * @brief Compute current & signal flow of a CktGraph
 * @author Hao Chen
 * @date 03/30/2020
 */

#ifndef _CSFLOW_H_
#define _CSFLOW_H_

#include "db/DesignDB.h"

PROJECT_NAMESPACE_BEGIN

class CSFlow {
 public:
  CSFlow(DesignDB& db) : _db(db) {}
  ~CSFlow() {}

  void computeCurrentFlow(CktGraph& ckt);
  void computeSignalFlow(CktGraph& ckt);

  /* Get */
  const IntType                                 numCurrentPaths()                   const { return _currentPinPaths.size(); }
  const std::vector<std::string>&               currentPinPath(const IndexType i)   const { return _currentPinPaths.at(i); }
  const std::vector<std::string>&               currentCellPath(const IndexType i)  const { return _currentCellPaths.at(i); }
  const std::vector<std::vector<std::string>>&  currentPinPaths()                   const { return _currentPinPaths; }
  const std::vector<std::vector<std::string>>&  currentCellPaths()                  const { return _currentCellPaths; }

 private:

  DesignDB& _db;

  std::vector<std::vector<std::string>> _currentPinPaths; // pin's net names
  std::vector<std::vector<std::string>> _currentCellPaths; // cell names

  
  void currentDFS(CktGraph& ckt, const IndexType sPinIdx, const IndexType tPinIdx, std::vector<unsigned char>& visited, std::vector<std::string>& pinNames, std::vector<std::string>& cellNames);

  ImplType    getCktNodeImplType(const CktNode& node);

}; 


PROJECT_NAMESPACE_END

#endif /// _CSFLOW_H_

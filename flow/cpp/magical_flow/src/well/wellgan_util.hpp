/**
 * @file wellgan_util.hpp
 * @brief The utilities for wellGAN
 * @author Keren Zhu
 * @date 03/14/2021
 */

#pragma once

#include "db/DesignDB.h"

PROJECT_NAMESPACE_BEGIN

/// @brief contain the layout data for running wellGAN
class DataWellGAN
{
  public:
    /// @param first: design database
    /// @param second: The layer ID for OD layer
    /// @param third: The layer ID for NW layer
    explicit DataWellGAN(DesignDB &ddb, const IntType odLayerId, const IntType _nwLayerId)
      : _ddb(ddb), _odLayerId(odLayerId), _nwLayerId(_nwLayerId) {}
    /// @brief Construct the layout for the circuit.
    /// It will combine the subcircuit layouts according to
    /// the current module position
    /// @param the index of the circuit
    void construct();

  private:
    DesignDB &_ddb; ///< The design database
    const IntType _odLayerId; ///< The layer ID for OD
    const IntType _nwLayerId; ///< The layer ID for NW
    std::vector<Box<LocType>> _nwShapes; ///< The N well shapes
    std::vector<Box<LocType>> _odOutsideShapes; ///< The OD shapes outside N wells
    std::vector<Box<LocType>> _odInsideShapes; ///< The OD shapes inside N wells
};

PROJECT_NAMESPACE_END

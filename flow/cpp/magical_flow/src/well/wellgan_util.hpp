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
    explicit DataWellGAN(DesignDB &ddb, const IntType odLayerId)
      : _ddb(ddb), _odLayerId(odLayerId) {}
    /// @brief Construct the layout for the circuit.
    /// It will combine the subcircuit layouts according to
    /// the current module position
    /// @param the index of the circuit
    void construct(IndexType cktIdx);
    /* Get the data */
    /// @brief Get the lower left coordinate of the layout
    const Point<LocType> & layoutOffset() const { return _layoutOffset; }
    /// @brief Get the number of PMOS OD shapes
    IndexType numPchOdRects() const { return _odPchShapes.size(); }
    /// @brief Get one rectangle shape from PMOS OD
    const Box<LocType> & odPchRect(IndexType rectIdx) const { return _odPchShapes.at(rectIdx); }
    /// @brief Get the number of other OD shapes
    IndexType numOtherOdRects() const { return _odOtherShapes.size(); }
    /// @brief Get one rectangle shape from other OD
    const Box<LocType> & odOtherRect(IndexType rectIdx) const { return _odOtherShapes.at(rectIdx); }

  private:
    DesignDB &_ddb; ///< The design database
    const IntType _odLayerId; ///< The layer ID for OD
    std::vector<Box<LocType>> _odOtherShapes; ///< The OD shapes outside N wells, from nmos to stdcell
    std::vector<Box<LocType>> _odPchShapes; ///< The OD shapes inside N wells, AKA., PMOS
    Point<LocType> _layoutOffset; ///< The offset of the layout. This is the lower left of the layout bounding box.
};

PROJECT_NAMESPACE_END

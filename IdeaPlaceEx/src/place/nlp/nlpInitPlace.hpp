/**
 * @file nlpInitPlace.hpp
 * @brief The non-lnear programming init place algorithms for global plaement
 * @author Keren Zhu
 * @date 04/02/2020
 */

#pragma once

#include <random>
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

namespace nlp {
    namespace init_place
    {

        /* Initial placement */
        template<typename T>
        struct init_place_trait
        {
            // static T construct(NlpType &)
            // static void initPlace(T&, NlpType &)
        };

        // @brief random spread among the boundary. With grid of number of cells
        struct init_random_placement_with_number_of_cells_uniform_distribution
        {
            static constexpr RealType range = 0.8; ///< Distrubute the location into range ratio of the boundary
        };

        template<>
        struct init_place_trait<init_random_placement_with_number_of_cells_uniform_distribution>
        {
            typedef init_random_placement_with_number_of_cells_uniform_distribution T;
            template<typename NlpType>
            static T construct(NlpType &) { return T(); }
            template<typename NlpType>
            static void initPlace(T &, NlpType &nlp)
            {
                using coord_type = typename NlpType::nlp_coordinate_type;
                const auto xCenter = (nlp._boundary.xLo() + nlp._boundary.xHi()) / 2;
                const auto yCenter = (nlp._boundary.yLo() + nlp._boundary.yHi()) / 2;
                XY<coord_type> initLoc(xCenter, yCenter);
                const IndexType numCells = nlp._db.numCells();
                for (IndexType cellIdx = 0; cellIdx < numCells; ++cellIdx)
                {
                    const auto xIdx = nlp.plIdx(cellIdx, Orient2DType::HORIZONTAL);
                    const auto yIdx = nlp.plIdx(cellIdx, Orient2DType::VERTICAL);
                    const IntType ranx = rand() % numCells;
                    const IntType rany = rand() % numCells;
                    const IntType adjustx =  ranx - numCells / 2;
                    const IntType adjusty =  rany - numCells / 2;
                    const auto ranfx = static_cast<RealType>(adjustx);
                    const auto ranfy = static_cast<RealType>(adjusty);
                    const auto locx = ranfx * T::range + xCenter;
                    const auto locy = ranfy * T::range + xCenter;
                    nlp._pl(xIdx) = locx;
                    nlp._pl(yIdx) = locy;
                }
            }
        };

        /// @brief randomly spread the cells in a normal distrubtion near the center
        struct init_random_placement_with_normal_distribution_near_center
        {
            static constexpr RealType randomInitPlaceStddev = 0.01; ///< The standard deviation of the initial random placement in terms of the width/height of the boundary
        };

        template<>
        struct init_place_trait<init_random_placement_with_normal_distribution_near_center>
        {
            typedef init_random_placement_with_normal_distribution_near_center T;
            template<typename NlpType>
            static T construct(NlpType &) { return T(); }
            template<typename NlpType>
            static void initPlace(T &, NlpType &nlp)
            {
                using coord_type = typename NlpType::nlp_coordinate_type;
                const auto xCenter = (nlp._boundary.xLo() + nlp._boundary.xHi()) / 2;
                const auto yCenter = (nlp._boundary.yLo() + nlp._boundary.yHi()) / 2;
                XY<coord_type> initLoc(xCenter, yCenter);
                const IndexType numCells = nlp._db.numCells();
                std::default_random_engine gen(0);
                RealType stddev = init_random_placement_with_normal_distribution_near_center::randomInitPlaceStddev * std::min(nlp._boundary.xLen(), nlp._boundary.yLen());
                std::normal_distribution<coord_type> movDistrX(initLoc.x(), stddev);
                std::normal_distribution<coord_type> movDistrY(initLoc.y(), stddev);
                for (IndexType cellIdx = 0; cellIdx < numCells; ++cellIdx)
                {
                    const coord_type xOffset = nlp._db.cell(cellIdx).cellBBox().xLen() * nlp._scale;
                    const coord_type yOffset = nlp._db.cell(cellIdx).cellBBox().yLen() * nlp._scale;
                    const auto xIdx = nlp.plIdx(cellIdx, Orient2DType::HORIZONTAL);
                    const auto yIdx = nlp.plIdx(cellIdx, Orient2DType::VERTICAL);
                    const coord_type locx = movDistrX(gen) -  xOffset * 0.5;
                    const coord_type locy = movDistrY(gen) -  yOffset * 0.5;
                    nlp._pl(xIdx) = locx;
                    nlp._pl(yIdx) = locy;
                }
                //nlp.alignToSym();
            }
        };
    } // namespace init_placement
} //namespae nlp

PROJECT_NAMESPACE_END

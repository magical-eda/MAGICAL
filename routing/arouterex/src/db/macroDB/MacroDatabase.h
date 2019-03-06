//
// Created by anoynmous on 09/18/2018
//

#ifndef AROUTER_MACRODATABASE_H_
#define AROUTER_MACRODATABASE_H_

#include "LefLibComponents.h"
#include "util/Vector2D.h"

/* MacroDatabase.h
/ Used to contain the information for the macro lib: cells, vias etc.
*/

PROJECT_NAMESPACE_BEGIN

class MacroDatabase
{
    public:
        explicit MacroDatabase() = default;

        // Getters
        const std::vector<std::vector<LefFixedVia>> & fixedViaVec2D() const { return _fixedViaVec2D; }
        std::vector<std::vector<LefFixedVia>> & fixedViaVec2D() { return _fixedViaVec2D; }

        /// @brief get the FixedVia with a specified via type
        /// @param layer: the index of access/routable layer. viaType: the index of the via in the array
        /// @return the VIA that is queried for
        const LefFixedVia & viaMacro(IndexType layer, IndexType viaType) const { return _fixedViaVec2D.at(layer).at(viaType); }
    private:
        std::vector<std::vector<LefFixedVia>> _fixedViaVec2D; ///< _viaVec2D[bottom access layer][via index of the layer]
};

PROJECT_NAMESPACE_END

#endif  /// AROUTER_MACRODATABASE_H_

/**
 * @file GdsHelper.h
 * @brief Use utilities of the Gds db
 * @author Keren Zhu
 * @date 02/07/2019
 */

#ifndef ZKUTIL_GDS_HELPER_H_
#define ZKUTIL_GDS_HELPER_H_

#include <limbo/parsers/gdsii/gdsdb/GdsIO.h>
#include <limbo/parsers/gdsii/gdsdb/GdsObjectHelpers.h>
#include "global/global.h"

namespace klib 
{

    /// @brief detailed struct for the functions of processing gds shapes
    namespace GetSRefNameActionDetails
    {

        /// @brief default type
        template<typename ObjectType>
        inline void getSName(std::string &name, ObjectType *object)
        {
        }

        /// @brief SREF type
        template<>
        inline void getSName(std::string &name,  ::GdsParser::GdsDB::GdsCellReference *object)
        {
            name = object->refCell();
        }
    }

    /// @brief aution function object to get the cell reference name
    struct GetSRefNameAction
    {
        /// @param A reference to the string to record the name of the sref
        GetSRefNameAction(std::string &name) : _name(name) {}
        template<typename ObjectType>
        void operator()(::GdsParser::GdsRecords::EnumType type, ObjectType* object)
        {
            GetSRefNameActionDetails::getSName(_name, object);
        }

        /// @return a message of action for debug
        std::string message() const 
        {
            return "GetSRefNameAction";
        }


        std::string &_name; ///< The cell reference name
    };
    /// @brief find the top cell of the db
    /// @param a gds db
    /// @return the name of the top cell
    inline std::string topCell(const ::GdsParser::GdsDB::GdsDB & db)
    {
        // Whether each cell is found as the subcell of the other
        std::map<std::string, bool> nameFound;
        // Iterate all the cells and record their names
        // Add reversely
        for (int idx = db.cells().size() - 1; idx >= 0; idx--)
        {
            nameFound[db.cells().at(idx).name()] = false;
        }
        for (auto &cell : db.cells())
        {
            for (auto obj : cell.objects())
            {
                std::string name = "";
                ::GdsParser::GdsDB::GdsObjectHelpers()(obj.first, obj.second, GetSRefNameAction(name));
                if (name != "")
                {
                    nameFound[name] = true;
                }
            }
        }
        // Return the name that was not included in the other cells
        for (auto pair : nameFound)
        {
            if (pair.first != "" &&  !pair.second)
            {
                return pair.first;
            }
        }

        AssertMsg(0 ,"Reading Gds::%s: all cells are referenced in other cells! \n", __FUNCTION__);
        return "";
    }
}
#endif //ZKUTIL_GDS_HELPER_H_

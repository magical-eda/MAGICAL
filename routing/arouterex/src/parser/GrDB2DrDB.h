/**
 * @file GrDB2DrDB.h
 * @brief Converting the global routing database to detailed routing base
 * @author anoynmous anoynmous
 * @date 10/16/2018
 */

#ifndef AROUTER_GRDBV22DRDBV1_H_
#define AROUTER_GRDBV22DRDBV1_H_

#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN

#if 0

/// @class AROUTER::GrDB_v2_to_DrDB_v1_converter
/// @brief Converting the global routing database (v2) to detailed routing base(v1) 
class GrDB_v2_to_DrDB_v1_converter
{
    public:
        /// @brief constructor
        /// @param the reference of the database
        explicit GrDB_v2_to_DrDB_v1_converter(Database &db) : _db(db), _drDB(db.drDB()), _grDB(db.grDB()) {}
        /// @brief convert global routing database result to detailed routing base
        /// @return if succesful
        bool convert();
        /// @brief set _convertSubNet
        /// @param whether to use the 2-pin spliting (subnet) from the global router
        void setConvertSubNet(bool convertSubNet) { _convertSubNet = convertSubNet; }
    private:
        /*------------------------------*/ 
        /* v2 to v1                     */
        /*------------------------------*/ 
        /// @brief convert the design information, such as die size
        /// @return if conversion is successful
        bool convertDesign();
        /// @brief convert the nets
        /// @return if the conversion is successful
        bool convertNets();
        /// @brief convert single net information
        /// @param the index of the net in the net array
        /// @return if the conversion is successful
        bool convertSingleNet(IndexType netIdx);
        /// @brief convert routing guide
        /// @param netIdx:  the index of the net in the net array. drNet: the reference to a drNet object
        /// @return if the conversion is successful
        bool convertRoutingGuide(IndexType netIdx, NetDR &drNet);
    
        
    private:
        Database &_db; ///< the overall database
        DatabaseDR &_drDB; ///< the detailed routing database
        DatabaseGR &_grDB; ///< the global routing database
        std::set<XYZ<LocType, IndexType>> _pinLocSet; ///< the set for pin locations
        bool _convertSubNet = READ_GR_BRANCH; ///< whether using the 2-pin net spliting from the global router

};

#endif

/// @class AROUTER::GrDB_v2_to_DrDB_v1_converter
/// @brief Converting the global routing database (v2) to detailed routing base(v1) 
class GrDB_v2_to_DrDB_v2_converter
{
    public:
        /// @brief constructor
        /// @param the reference of the database
        explicit GrDB_v2_to_DrDB_v2_converter(Database &db) : _db(db), _drDB(db.drDB()), _grDB(db.grDB()) {}
        /// @brief convert global routing database result to detailed routing base
        /// @return if succesful
        bool convert();
        /// @brief set _convertSubNet
        /// @param whether to use the 2-pin spliting (subnet) from the global router
        void setConvertSubNet(bool convertSubNet) { _convertSubNet = convertSubNet; }
    private:
        /*------------------------------*/ 
        /* v2 to v1                     */
        /*------------------------------*/ 
        /// @brief convert the design information, such as die size
        /// @return if conversion is successful
        bool convertDesign();
        /// @brief convert the nets
        /// @return if the conversion is successful
        bool convertNets();
        /// @brief convert single net information
        /// @param the index of the net in the net array
        /// @return if the conversion is successful
        bool convertSingleNet(IndexType netIdx);
        /// @brief convert routing guide
        /// @param netIdx:  the index of the net in the net array. drNet: the reference to a drNet object
        /// @return if the conversion is successful
        bool convertRoutingGuide(IndexType netIdx, NetDR &drNet);
    
        
    private:
        Database &_db; ///< the overall database
        DatabaseDR &_drDB; ///< the detailed routing database
        DatabaseGR &_grDB; ///< the global routing database
        std::set<XYZ<LocType, IndexType>> _pinLocSet; ///< the set for pin locations
        bool _convertSubNet = READ_GR_BRANCH; ///< whether using the 2-pin net spliting from the global router

};
PROJECT_NAMESPACE_END

#endif ///AROUTER_GRDBV22DRDBV1_H_

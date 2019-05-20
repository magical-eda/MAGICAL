//
// Created by Keren on 08/17/2018
//

#ifndef AROUTER_GR2DR_H
#define AROUTER_GR2DR_H

#include <unordered_set>
#include "db//drDB/DatabaseDR.h"
#include "gr/ARoute.h"

/// ================================================================================ 
/// GR2DR.h
///
/// Convert Derong's global routing solution to the guide of detailed routing
///
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

class GR2DR
{
    public:
        explicit GR2DR(DatabaseDR &drDB, ARoute &aroute) : _drDB(drDB), _aroute(aroute) { convert(); }

        bool convert();

    private:
        ////////////////
        /// Nets
        ////////////////
        /// Convert the net arrays;
        void convertNets();
        /// Convert myNet to NetDR
        void convertNet(IndexType netIdx);
        /// Convert the 3dedges (global routing edges) into NetDR routing guide
        void convertRoutingGuide(IndexType netIdx);
        /// Convert symNet information
        void convertSymNet(mySymNet *grSymNet);
        /// Convert net branch
        void convertBranch(IndexType netIdx);
        /////////////////
        /// Design/grid
        /////////////////
        void convertDesign();
    private:
        DatabaseDR &_drDB;
        ARoute    &_aroute;
        /// the map from the net idx defined in benchmark to the index in the net vector.
        /// Highly possible these two are always the same, but anyway...
        std::unordered_map<IntType, IndexType> _benchNetIdx2VecIdx;

#ifdef TRIM_OVERLAP_PIN
        /// Record the location of pins, for helping triming the overlapping pins
        std::unordered_set<XYZ<LocType, IndexType>> _pinDict;
#endif
 };

PROJECT_NAMESPACE_END

#endif ///AROUTER_GR2DR_H

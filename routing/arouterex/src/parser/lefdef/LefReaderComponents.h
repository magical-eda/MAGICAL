//
// Created by Keren on 09/17/2018
// 

#ifndef AROUTER_LEFREADERCOMPONENTS_H_
#define AROUTER_LEFREADERCOMPONENTS_H_

#include "global/global.h"
#include "util/Box.h"

// LefReaderComponent.h
//
// Some supporting data structure for lefReader
//

PROJECT_NAMESPACE_BEGIN

class LefRawFixedVia
{
    public:
        explicit LefRawFixedVia() = default;

        std::string name = "";
        bool isDefault = false;
        bool hasResistance = false;
        RealType resistance = 0;

        std::string bottomLayer = "";
        std::string cutLayer = "";
        std::string topLayer = "";

        Box<RealType> bottomLayerRect;
        IndexType numCutRect = 0;
        std::vector<Box<RealType>> cutLayerRectArray;
        Box<RealType> topLayerRect;
};

PROJECT_NAMESPACE_END

#endif /// AROUTER_LEFREADERCOMPONENTS_H_

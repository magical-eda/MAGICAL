// *****************************************************************************
// *****************************************************************************
// Copyright 2012 - 2014, Cadence Design Systems
// 
// This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
// Distribution,  Product Version 5.8. 
// 
// Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
// 
//        http://www.apache.org/licenses/LICENSE-2.0
// 
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
//    implied. See the License for the specific language governing
//    permissions and limitations under the License.
// 
// For updates, support, or to become part of the LEF/DEF Community,
// check www.openeda.org for details.
// 
//  $Author: arakhman $
//  $Revision: #11 $
//  $Date: 2013/04/23 $
//  $State:  $
// *****************************************************************************
// *****************************************************************************

#ifndef lefrSettings_h
#define lefrSettings_h

#include <cstring>
#include <string>
#include <map>

#include "lefrReader.hpp"

BEGIN_LEFDEF_PARSER_NAMESPACE

struct lefCompareCStrings 
{
    bool operator()(const char* lhs, const char* rhs) const {
        return std::strcmp(lhs, rhs) < 0;
    }
};

typedef std::map<const char*, int, lefCompareCStrings>  lefKeywordMap;
typedef std::map<int, int> MsgsDisableMap;

class lefrProps {
public:
    lefiPropType  lefrCompProp; 
    lefiPropType  lefrLayerProp; 
    lefiPropType  lefrLibProp; 
    lefiPropType  lefrMacroProp; 
    lefiPropType  lefrNondefProp; 
    lefiPropType  lefrPinProp; 
    lefiPropType  lefrViaProp; 
    lefiPropType  lefrViaRuleProp; 
};

class lefrSettings {
public:
    lefrSettings();

    void init_symbol_table();
    static void reset();

    void        disableMsg(int msgId);
    void        enableMsg(int msgId);
    void        enableAllMsgs();
    int         suppresMsg(int msgId);

    lefKeywordMap       Keyword_set; 

    char CommentChar;
    double VersionNum;
    int DisPropStrProcess;
    int CaseSensitive;
    int CaseSensitiveSet;
    int DeltaNumberLines;
    int AntennaInoutWarnings;
    int AntennaInputWarnings;
    int AntennaOutputWarnings;
    int ArrayWarnings;
    int CaseSensitiveWarnings;
    int CorrectionTableWarnings;
    int DielectricWarnings;
    int EdgeRateScaleFactorWarnings;
    int EdgeRateThreshold1Warnings;
    int EdgeRateThreshold2Warnings;
    int IRDropWarnings;
    int InoutAntennaWarnings;
    int InputAntennaWarnings;
    LEFI_LINE_NUMBER_FUNCTION LineNumberFunction;
    int LayerWarnings;
    int MacroWarnings;
    int MaxStackViaWarnings;
    int MinFeatureWarnings;
    int NoWireExtensionWarnings;
    int NoiseMarginWarnings;
    int NoiseTableWarnings;
    int NonDefaultWarnings;
    int OutputAntennaWarnings;
    int PinWarnings;
    LEFI_READ_FUNCTION ReadFunction;
    int ReadEncrypted; 
    int RegisterUnused;
    int RelaxMode;
    int ShiftCase;
    int SiteWarnings;
    int SpacingWarnings;
    int TimingWarnings;
    int UnitsWarnings;
    int UseMinSpacingWarnings;
    int ViaRuleWarnings;
    int ViaWarnings;
    int LogFileAppend;
    int TotalMsgLimit;
    lefiUserData UserData;

    LEFI_MALLOC_FUNCTION MallocFunction;
    LEFI_REALLOC_FUNCTION ReallocFunction;
    LEFI_FREE_FUNCTION FreeFunction;
    LEFI_LOG_FUNCTION ErrorLogFunction;
    LEFI_LOG_FUNCTION SetLogFunction;
    LEFI_WARNING_LOG_FUNCTION WarningLogFunction;

    int MsgLimit[MAX_LEF_MSGS];

    MsgsDisableMap msgsDisableMap;
    int  dAllMsgs; 

    lefrProps lefProps;
};

extern lefrSettings* lefSettings;

END_LEFDEF_PARSER_NAMESPACE

#endif

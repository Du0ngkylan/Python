/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRRqWaterReplaceAnalyzer.cpp
 * @brief       Analyze Sensor Request
 * @author      anhnguyen
 * @date        2018/12/11
 */

#include "FAMSDRRqWaterReplaceAnalyzer.h"
#include "FAMSDRCommon.h"
#include "FAMSIpcWaterReplaceData.h"

using namespace mdbcom;

// for setter function in base class, must cast base's function pointer to derived's function pointer
#define getBaseSetter(setterName,type) static_cast<void (FAMSWaterReplaceData::*)(type)>(&FAMSRqBaseData::setterName)

namespace famsdatareception {
    using std::string;
//    using famscommon::FAMSRqBaseData;
    using famscommon::FAMSWaterReplaceData;

    std::vector<mdbcom::MDBContentRuleEntry> FAMSDRRqWaterReplaceAnalyzer::inputRules =
    {
        {
            DATA, "", mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */}, Poco::Any { /* empty */}
        },
        {
            TIMESTAMP, {DATA}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSWaterReplaceData::setAccumulatedTime),
            true
        },
        {
            WATERLEVEL, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSWaterReplaceData::setWaterLevel),
            true
        },
        {
            WATERTEMP, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSWaterReplaceData::setWaterTemp),
            true
        }
    };

    FAMSDRRqWaterReplaceAnalyzer::FAMSDRRqWaterReplaceAnalyzer(MDBContentParser *parser)
        : mdbcom::MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<MDBContentRuleEntry> *FAMSDRRqWaterReplaceAnalyzer::getRuleTable(void)
    {
        return &inputRules;
    };
}
#undef getBaseSetter



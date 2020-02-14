/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRRqNitrificationAnalyzer.cpp
 * @brief       Analyze Sensor Request
 * @author      anhnguyen
 * @date        2018/12/11
 */

#include "FAMSDRRqNitrificationAnalyzer.h"
#include "FAMSDRCommon.h"
#include "FAMSIpcNitrificationData.h"

using namespace mdbcom;

// for setter function in base class, must cast base's function pointer to derived's function pointer
#define getBaseSetter(setterName,type) static_cast<void (FAMSNitrificationData::*)(type)>(&FAMSRqBaseData::setterName)

namespace famsdatareception {
    using std::string;
//    using famscommon::FAMSRqBaseData;
    using famscommon::FAMSNitrificationData;

    std::vector<mdbcom::MDBContentRuleEntry> FAMSDRRqNitrificationAnalyzer::inputRules =
    {
        {
            DATA, "", mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */}, Poco::Any { /* empty */}
        },
        {
            TIMESTAMP, {DATA}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSNitrificationData::setAccumulatedTime),
            true
        },
        {
            WATERTEMP, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSNitrificationData::setWaterTemp),
            true
        }
    };

    FAMSDRRqNitrificationAnalyzer::FAMSDRRqNitrificationAnalyzer(MDBContentParser *parser)
        : mdbcom::MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<MDBContentRuleEntry> *FAMSDRRqNitrificationAnalyzer::getRuleTable(void)
    {
        return &inputRules;
    };
}
#undef getBaseSetter



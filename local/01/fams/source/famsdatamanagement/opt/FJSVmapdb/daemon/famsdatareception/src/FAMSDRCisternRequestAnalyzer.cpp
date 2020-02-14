/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRCisternRequestAnalyzer.cpp
 * @brief       Analyze Sensor Request
 * @author      anhnguyen
 * @date        2018/12/11
 */

#include "FAMSDRCisternRequestAnalyzer.h"
#include "FAMSDRCommon.h"
#include "FAMSIpcCisternSensorData.h"

using namespace mdbcom;

// for setter function in base class, must cast base's function pointer to derived's function pointer
#define getBaseSetter(setterName,type) static_cast<void (FAMSNitrificationData::*)(type)>(&FAMSRqBaseData::setterName)

namespace famsdatareception {
    using std::string;
//    using famscommon::FAMSRqBaseData;
    using famscommon::FAMSCisternSenSorData;

    std::vector<mdbcom::MDBContentRuleEntry> FAMSDRCisternRequestAnalyzer::inputRules =
    {
        {
            DATA, "", mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */}, Poco::Any { /* empty */}
        },
        {
            TIMESTAMP, {DATA}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setAccumulatedTime),
            true
        },
        {
            CISTERNCODE, {DATA}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setCisternCode),
            true
        },
        {
            INFLOWTEMP, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setInFlowTemp),
            true
        },
        {
            OUTFLOWTEMP, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setOutFlowTemp),
            true
        },
        {
            SALT, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setSalt),
            true
        },
        {
            PH, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setPH),
            true
        },
        {
            UPPERCENTRALILL, {DATA}, mdbcom::MDBInputKind::ARRAY_INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setUpperCentralIll),
            true
        },
        {
            LOWERCENTRALILL, {DATA}, mdbcom::MDBInputKind::ARRAY_INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSCisternSenSorData::setLowerCentralIll),
            true
        }
    };

    FAMSDRCisternRequestAnalyzer::FAMSDRCisternRequestAnalyzer(MDBContentParser *parser)
        : mdbcom::MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<MDBContentRuleEntry> *FAMSDRCisternRequestAnalyzer::getRuleTable(void)
    {
        return &inputRules;
    };
}
#undef getBaseSetter



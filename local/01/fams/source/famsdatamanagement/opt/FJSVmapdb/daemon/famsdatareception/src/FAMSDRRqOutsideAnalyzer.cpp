/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRRqOutsideAnalyzer.cpp
 * @brief       Analyze Sensor Request
 * @author      anhnguyen
 * @date        2018/12/11
 */

#include "FAMSDRRqOutsideAnalyzer.h"
#include "FAMSDRCommon.h"
#include "FAMSIpcOutsideData.h"

using namespace mdbcom;

// for setter function in base class, must cast base's function pointer to derived's function pointer
#define getBaseSetter(setterName,type) static_cast<void (FAMSOutsideData::*)(type)>(&FAMSRqBaseData::setterName)

namespace famsdatareception {
    using std::string;
//    using famscommon::FAMSRqBaseData;
    using famscommon::FAMSOutsideData;

    std::vector<mdbcom::MDBContentRuleEntry> FAMSDRRqOutsideAnalyzer::inputRules =
    {
        {
            DATA, "", mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */}, Poco::Any { /* empty */}
        },
        {
            TIMESTAMP, {DATA}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSOutsideData::setAccumulatedTime),
            true
        },
        {
            ROOM_TEMP, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSOutsideData::setRoomTemp),
            true
        },
        {
            HUMIDITY, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSOutsideData::setHumidity),
            true
        },
        {
            ATMOSPHERIC_PRESSURE, {DATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSOutsideData::setAtmosphericPressure),
            true
        }
    };

    FAMSDRRqOutsideAnalyzer::FAMSDRRqOutsideAnalyzer(MDBContentParser *parser)
        : mdbcom::MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<MDBContentRuleEntry> *FAMSDRRqOutsideAnalyzer::getRuleTable(void)
    {
        return &inputRules;
    };
}
#undef getBaseSetter



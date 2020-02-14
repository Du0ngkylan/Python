/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRSensorRequestAnalyzer.cpp
 * @brief       Analyze cistern add Request
 * @author      thanglq
 * @date        2019/10/14
 */

#include "FAMSDRSensorRequestAnalyzer.h"
#include "FAMSDRCommon.h"
#include "FAMSSensorData.h"
using namespace mdbcom;

// for setter function in base class, must cast base's function pointer to derived's function pointer
#define getBaseSetter(setterName,type) static_cast<void (FAMSNitrificationData::*)(type)>(&FAMSRqBaseData::setterName)

namespace famsdatareception {
    using std::string;
    using famscommon::FAMSSensorData;

    std::vector<mdbcom::MDBContentRuleEntry> FAMSDRSensorRequestAnalyzer::inputRules =
    {
        {
            DATA, "", mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */}, Poco::Any { /* empty */}
        },
        {
        	TIMESTAMP, DATA, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSSensorData::setTimeStamp)
        },
    	{
        	RTUID, DATA, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSSensorData::setRtuId)
        },
        {
            SENSORDATA, DATA, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */}, Poco::Any { /* empty */}
        },
        {
            RTUCHANNEL, {DATA, SENSORDATA}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSSensorData::setRtuChannel),
            true
        },
        {
            VALUE, {DATA, SENSORDATA}, mdbcom::MDBInputKind::ARRAY_REAL, mdbcom::MDBContentRule::MUST_EXIST,
            Poco::Any { /* empty */ },
            std::mem_fn(&FAMSSensorData::setValue),
            true
        },
    };

    FAMSDRSensorRequestAnalyzer::FAMSDRSensorRequestAnalyzer(MDBContentParser *parser)
        : mdbcom::MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<MDBContentRuleEntry> *FAMSDRSensorRequestAnalyzer::getRuleTable(void)
    {
        return &inputRules;
    };
}
#undef getBaseSetter



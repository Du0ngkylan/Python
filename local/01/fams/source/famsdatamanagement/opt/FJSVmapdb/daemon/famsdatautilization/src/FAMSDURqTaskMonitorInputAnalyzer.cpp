/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskMonitorInputAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqTaskMonitorInputAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqTaskMonitorInputAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskHandler.h"
#include "FAMSDUTaskMonitorInputData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqTaskMonitorInputAnalyzer::inputRqRules =
    {
        {
            RequestType::TASKS_POST,
            {
                {
                    DATA, "", mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    Poco::Any {}
                },
                {
                    WATER_MONITOR_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setPICId)
                },
                {
                    CISTERN_INFORMATION_ID, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setCisternInformationId)
                },
                {
                    MONITOR_TYPE, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setMonitorType)
                },
                {
                    MONITOR_VALUE, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setMonitorValue)
                },
                {
                    MEMO, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setNote)
                },
            }
        },
        {
            RequestType::TASK_PUT,
            {
                {
                    DATA, "", mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    Poco::Any {}
                },
                {
                    WATER_MONITOR_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setPICId)
                },
                {
                    CISTERN_INFORMATION_ID, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setCisternInformationId)
                },
                {
                    MONITOR_TYPE, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setMonitorType)
                },
                {
                    MONITOR_VALUE, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setMonitorValue)
                },
                {
                    MEMO, {DATA, WATER_MONITOR_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMonitorInputData::setNote)
                },
            }
        }
    };


    FAMSDURqTaskMonitorInputAnalyzer::FAMSDURqTaskMonitorInputAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqTaskMonitorInputAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


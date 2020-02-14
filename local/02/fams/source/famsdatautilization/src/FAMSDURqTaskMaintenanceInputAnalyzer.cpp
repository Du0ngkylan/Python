/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskMaintenanceInputAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqTaskMaintenanceInputAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqTaskMaintenanceInputAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskHandler.h"
#include "FAMSDUTaskMaintenanceInputData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqTaskMaintenanceInputAnalyzer::inputRqRules =
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
                    MAINTENANCE_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setPICId)
                },
                {
                    CISTERN_INFORMATION_ID, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setCisternInformationId)
                },
                {
                    MAINTENANCE_TYPE, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setMaintenanceType)
                },
                {
                    MEMO, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setNote)
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
                    MAINTENANCE_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setPICId)
                },
                {
                    CISTERN_INFORMATION_ID, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setCisternInformationId)
                },
                {
                    MAINTENANCE_TYPE, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setMaintenanceType)
                },
                {
                    MEMO, {DATA, MAINTENANCE_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskMaintenanceInputData::setNote)
                },
            }
        }
    };


    FAMSDURqTaskMaintenanceInputAnalyzer::FAMSDURqTaskMaintenanceInputAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqTaskMaintenanceInputAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


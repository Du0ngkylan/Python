/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskRecoveryInputAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqTaskRecoveryInputAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqTaskRecoveryInputAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskHandler.h"
#include "FAMSDUTaskRecoveryInputData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqTaskRecoveryInputAnalyzer::inputRqRules =
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
                    RECOVERY_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setPICId)
                },
                {
                    MEMO, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setNote)
                },
                {
                    DETAILS, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    LOT_INFORMATION_ID, {DATA, RECOVERY_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setLotList),
                    true
                },

                {
                    AMOUNT, {DATA, RECOVERY_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setAmountList),
                    true
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
                    RECOVERY_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setPICId)
                },
                {
                    MEMO, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setNote)
                },
                {
                    DETAILS, {DATA, RECOVERY_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    LOT_INFORMATION_ID, {DATA, RECOVERY_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setLotList),
                    true
                },

                {
                    AMOUNT, {DATA, RECOVERY_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskRecoveryInputData::setAmountList),
                    true
                },
            }
        }
    };


    FAMSDURqTaskRecoveryInputAnalyzer::FAMSDURqTaskRecoveryInputAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqTaskRecoveryInputAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


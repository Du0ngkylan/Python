/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskDisposalInputAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqTaskDisposalInputAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqTaskDisposalInputAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskHandler.h"
#include "FAMSDUTaskDisposalInputData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqTaskDisposalInputAnalyzer::inputRqRules =
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
                    DISPOSAL_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setPICId)
                },
                {
                    DISPOSAL_TYPE, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setDisposalType)
                },
                {
                    MEMO, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setNote)
                },
                {
                    DETAILS, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    LOT_INFORMATION_ID, {DATA, DISPOSAL_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setLotList),
                    true
                },

                {
                    AMOUNT, {DATA, DISPOSAL_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setAmountList),
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
                    DISPOSAL_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setPICId)
                },
                {
                    DISPOSAL_TYPE, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setDisposalType)
                },
                {
                    MEMO, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setNote)
                },
                {
                    DETAILS, {DATA, DISPOSAL_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    LOT_INFORMATION_ID, {DATA, DISPOSAL_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setLotList),
                    true
                },

                {
                    AMOUNT, {DATA, DISPOSAL_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskDisposalInputData::setAmountList),
                    true
                },
            }
        }
    };


    FAMSDURqTaskDisposalInputAnalyzer::FAMSDURqTaskDisposalInputAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqTaskDisposalInputAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


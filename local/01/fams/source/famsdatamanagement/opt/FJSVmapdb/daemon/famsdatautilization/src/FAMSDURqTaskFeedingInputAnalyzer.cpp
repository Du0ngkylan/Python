/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskFeedingInputAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqTaskFeedingInputAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqTaskFeedingInputAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskHandler.h"
#include "FAMSDUTaskFeedingInputData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqTaskFeedingInputAnalyzer::inputRqRules =
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
                    FEEDING_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setPICId)
                },
                {
                    FEEDING_NAME, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setFeedingName)
                },
                {
                    MEMO, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setNote)
                },
                {
                    DETAILS, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    LOT_INFORMATION_ID, {DATA, FEEDING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setLotList),
                    true
                },
                {
                    AMOUNT, {DATA, FEEDING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setAmountList),
                    true
                }
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
                    FEEDING_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setPICId)
                },
                {
                    FEEDING_NAME, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setFeedingName)
                },
                {
                    MEMO, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setNote)
                },
                {
                    DETAILS, {DATA, FEEDING_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    LOT_INFORMATION_ID, {DATA, FEEDING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setLotList),
                    true
                },
                {
                    AMOUNT, {DATA, FEEDING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskFeedingInputData::setAmountList),
                    true
                }
            }
        }
    };


    FAMSDURqTaskFeedingInputAnalyzer::FAMSDURqTaskFeedingInputAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqTaskFeedingInputAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


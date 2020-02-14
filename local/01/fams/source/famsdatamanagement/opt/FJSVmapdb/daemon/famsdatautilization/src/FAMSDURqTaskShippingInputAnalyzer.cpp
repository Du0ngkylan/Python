/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskShippingInputAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqTaskShippingInputAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqTaskShippingInputAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskHandler.h"
#include "FAMSDUTaskShippingInputData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqTaskShippingInputAnalyzer::inputRqRules =
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
                    SHIPPING_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setPICId)
                },
                {
                    SHIPPING_DESTINATION_INPUT, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setDestination)
                },
                {
                    MEMO, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setNote)
                },
                {
                    DETAILS, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    LOT_INFORMATION_ID, {DATA, SHIPPING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskShippingInputData::setLotList),
                    true
                },

                {
                    AMOUNT, {DATA, SHIPPING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskShippingInputData::setAmountList),
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
                    SHIPPING_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setPICId)
                },
                {
                    SHIPPING_DESTINATION_INPUT, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setDestination)
                },
                {
                    MEMO, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskShippingInputData::setNote)
                },
                {
                    DETAILS, {DATA, SHIPPING_INPUT}, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    LOT_INFORMATION_ID, {DATA, SHIPPING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskShippingInputData::setLotList),
                    true
                },

                {
                    AMOUNT, {DATA, SHIPPING_INPUT, DETAILS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUTaskShippingInputData::setAmountList),
                    true
                },
            }
        }
    };


    FAMSDURqTaskShippingInputAnalyzer::FAMSDURqTaskShippingInputAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqTaskShippingInputAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


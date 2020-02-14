/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqObserversAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqObserversAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqObserversAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUObserversHandler.h"
#include "FAMSDUSerializeObserversData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqObserversAnalyzer::inputRqRules =
    {
        {
            RequestType::OBSERVERS_POST,
            {
                {
                    DATA, "", mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    Poco::Any {}
                }
                ,
                {
                    MAIL, {DATA}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUSerializeObserversData::setMail),
                    true
                }
            }
        },
        {
            RequestType::OBSERVERS_PUT,
            {
                {
                    DATA, "", mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    Poco::Any {}
                }
                ,
                {
                    MAIL, DATA, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUSerializeObserversData::setSingleMail)
                }
            }
        },
    };


    FAMSDURqObserversAnalyzer::FAMSDURqObserversAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {-1}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqObserversAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


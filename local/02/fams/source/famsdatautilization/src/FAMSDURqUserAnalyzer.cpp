/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqUserAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqUserAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqUserAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUUserData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqUserAnalyzer::inputRqRules =
    {
        {
            RequestType::LOGIN_POST,
            {
                {
                    DATA, "", mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    Poco::Any {}
                },
                {
                    USER_NAME, DATA, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUUserData::setUsername)
                },
                {
                    PASSWORD, DATA, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUUserData::setPassword)
                }
            }
        }
    };


    FAMSDURqUserAnalyzer::FAMSDURqUserAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqUserAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


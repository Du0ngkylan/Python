/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqCisternAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqCisternAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqCisternAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUUserData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqCisternAnalyzer::inputRqRules =
    {
    };


    FAMSDURqCisternAnalyzer::FAMSDURqCisternAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqCisternAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


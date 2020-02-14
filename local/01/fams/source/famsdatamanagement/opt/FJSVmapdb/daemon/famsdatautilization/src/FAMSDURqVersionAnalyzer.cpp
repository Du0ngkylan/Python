/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqVersionAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqVersionAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqVersionAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUVersionHandler.h"
#include "FAMSDUVersionData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqVersionAnalyzer::inputRqRules =
    {
    };

    FAMSDURqVersionAnalyzer::FAMSDURqVersionAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqVersionAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


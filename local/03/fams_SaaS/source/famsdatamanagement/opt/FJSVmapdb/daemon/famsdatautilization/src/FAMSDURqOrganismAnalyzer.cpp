/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqUserAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqUserAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqOrganismAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUPostOrganismData.h"
namespace famsdatautil {
    using famsdatautil::FAMSDUPostOrganismData;
    using std::string;

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqOrganismAnalyzer::inputRqRules =
    {
        {
            RequestType::ORGANISM_LIST_POST,
            {
                {
                    DATA, "", mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    NAME, DATA, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUPostOrganismData::setName)
                },

                {
                    THRESHOLDS, DATA, mdbcom::MDBInputKind::ARRAY_OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },

                {
                    ID, {DATA, THRESHOLDS}, mdbcom::MDBInputKind::ARRAY_INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUPostOrganismData::setId),
                    true
                },

                {
                    MIN_VALUE, {DATA, THRESHOLDS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUPostOrganismData::setMin),
                    true
                },

                {
                    MAX_VALUE, {DATA, THRESHOLDS}, mdbcom::MDBInputKind::ARRAY_STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any { /* empty */ },
                    std::mem_fn(&FAMSDUPostOrganismData::setMax),
                    true
                }
            }
        }
    };


    FAMSDURqOrganismAnalyzer::FAMSDURqOrganismAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType(-1)
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqOrganismAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}

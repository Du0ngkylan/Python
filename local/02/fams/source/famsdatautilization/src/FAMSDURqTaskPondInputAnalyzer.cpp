/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskPondInputAnalyzer.cpp
 * @brief       Source file for implementing FAMSDURqTaskPondInputAnalyzer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include "FAMSDURqTaskPondInputAnalyzer.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskHandler.h"
#include "FAMSDUTaskPondInputData.h"

namespace famsdatautil {

    std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> FAMSDURqTaskPondInputAnalyzer::inputRqRules =
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
                    POND_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setPICId)
                },
                {
                    CISTERN_INFORMATION_ID, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setCisternInformationId)
                },
                {
                    ORGANISM_INFORMATION_ID, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismInformationId)
                },
                {
                    ORGANISM_NUMBER, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismNumber)
                },
                {
                    ORGANISM_TAG, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismTag)
                },
                {
                    ORGANISM_UNIT, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismUnit)
                },
                {
                    ORGANISM_INITIAL_SIZE, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismInitialSize)
                },
                {
                    SHIPPABLE_TIME, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setShippableTime)
                },
                {
                    MEMO, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setNote)
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
                    POND_INPUT, DATA, mdbcom::MDBInputKind::OBJECT, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    Poco::Any {}
                },
                {
                    PROCESS_TIME, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setProcessTime)
                },
                {
                    PROCESS_USER_ID, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setPICId)
                },
                {
                    CISTERN_INFORMATION_ID, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setCisternInformationId)
                },
                {
                    ORGANISM_INFORMATION_ID, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismInformationId)
                },
                {
                    ORGANISM_NUMBER, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER32, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismNumber)
                },
                {
                    ORGANISM_TAG, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismTag)
                },
                {
                    ORGANISM_UNIT, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismUnit)
                },
                {
                    ORGANISM_INITIAL_SIZE, {DATA, POND_INPUT}, mdbcom::MDBInputKind::INTEGER64, mdbcom::MDBContentRule::MUST_EXIST,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setOrganismInitialSize)
                },
                {
                    SHIPPABLE_TIME, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setShippableTime)
                },
                {
                    MEMO, {DATA, POND_INPUT}, mdbcom::MDBInputKind::STRING, mdbcom::MDBContentRule::ANY,
                    Poco::Any {},
                    std::mem_fn(&FAMSDUTaskPondInputData::setNote)
                },
            }
        }
    };


    FAMSDURqTaskPondInputAnalyzer::FAMSDURqTaskPondInputAnalyzer(mdbcom::MDBContentParser *parser)
        : MDBContentAnalyzer {parser}, requestType {}
    {
    };

    /*!
     * @brief Searching method, override from MDBContentAnalyzer
     * @param None.
     * @return Pointer to rule table
     */
    const std::vector<mdbcom::MDBContentRuleEntry> *FAMSDURqTaskPondInputAnalyzer::getRuleTable(void)
    {
        return &inputRqRules.find(requestType)->second;
    };
}


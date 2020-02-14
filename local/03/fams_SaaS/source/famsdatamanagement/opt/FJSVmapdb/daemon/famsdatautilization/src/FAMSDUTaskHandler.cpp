/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskHandler.cpp
 * @brief       Source file for implementing  information.
 * @author      anhnguyen
 * @date        2018/12/10
 */
#include "../include/FAMSDUTaskHandler.h"

#include <future>
#include <sstream>
#include "MDBHTTPServerUtility.h"
#include "MDBConnectionManager.h"
#include "MDBJsonParser.h"

using famscommon::JSON_STRING_MISSING_VALUE;
using famscommon::JSON_INTEGER_MISSING_VALUE;
using famscommon::JSON_DOUBLE_MISSING_VALUE;
using famscommon::JSON_DOUBLE_TIME_MISSING_VALUE;

namespace famsdatautil {

    /*!
     * @brief The constructor.
     * @param [in] sharedMemory the shared memory from main.
     * @return None.
     */
    FAMSDUTaskHandler::FAMSDUTaskHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType)
        : FAMSDUBaseHandler { inputTemplate }, mRequestType {requestType}
    {
    }

    /*!
     *  @brief The main method for building difference map.
     *
     *  @param [in] request the HTTP request data
     *  @param [out] response the HTTP response data
     *
     *  @return None.
     */
    void FAMSDUTaskHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        auto startTime = famscommon::getCurrentTime(); /* pgr0745 */
        if (mRequestType == RequestType::TASKS_GET) {
            handleGetTasksRequest(request, response);
        }

        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
    }

    void FAMSDUTaskHandler::handleGetTasksRequest(
            Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response) {
       
    }

    /*!
     * @brief Validate JSON format.
     *
     * @param [in] jsonString the JSON string input.
     * @param [out] errKey first key not comply rule
     *
     * @retval OK json format  is OK.
     * @retval JSON_NG json format is NG.
     */
    int FAMSDUTaskHandler::validateJSONFormat(const std::string &jsonString, mdbcom::MDBContentParser::KeyMapType &jsonMap)
    {
        return 0;
    }
}


/*
 * FAMSDUOrganismHandler.cpp
 *
 *  Created on: Jan 30, 2019
 *      Author: mapdb
 */

#ifndef FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_SRC_FAMSDUORGANISMHANDLER_CPP_
#define FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_SRC_FAMSDUORGANISMHANDLER_CPP_


#include <future>
#include <sstream>
#include "MDBHTTPServerUtility.h"
#include "MDBConnectionManager.h"
#include "FAMSDUOrganismData.h"
#include "FAMSDUOrganismHandler.h"
#include "MDBJsonParser.h"
#include "MDBURIHelper.h"

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
    FAMSDUOrganismHandler::FAMSDUOrganismHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType)
        : FAMSDUBaseHandler { inputTemplate }, mRequestType {requestType}, mPostRequestData { }
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
    void FAMSDUOrganismHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        auto startTime = famscommon::getCurrentTime(); /* pgr0745 */
        if (mRequestType == RequestType::ORGANISM_LIST_GET) {
            handleOrganismListGetRequest(request, response);
        }
        else if (mRequestType == RequestType::ORGANISM_LIST_POST) {
            handleOrganismListPostRequest(request, response);
        }
        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
    }

    int FAMSDUOrganismHandler::validateJSONFormat(
        const std::string &jsonstring,
        mdbcom::MDBContentParser::KeyMapType &jsonMap,
        std::string &errKeys)
    {
        mdb_apl_log_start();
        auto ret = OK;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto errorKey = std::string {};
        // decode JSON std::string to object
        std::string rootKey {""};
        auto jsonRet = jsonDecoder.decodeKeyMapData(jsonstring, mJsonTemplate, rootKey, jsonMap, errorKey);
        if (mdbcom::Json::OK != jsonRet || false == errKeys.empty())
        {
            auto errKeyString = errKeys.c_str();
            mdb_apl_log_critical("JSON format is abnormal, errorKey = %s", errKeyString);
            mdb_apl_log_end();
            return JSON_NG;
        }
        auto inputAnalyzer = FAMSDURqOrganismAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(RequestType::ORGANISM_LIST_POST);

        if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mPostRequestData, errKeys))
        {
            auto errKeyString = errKeys.c_str();
            mdb_apl_log_error("Analyze result fail at %s", errKeyString);
            mdb_apl_log_end();
            return JSON_NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    void FAMSDUOrganismHandler::handleOrganismListPostRequest(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response)
    {

    }
   
    void FAMSDUOrganismHandler::handleOrganismListGetRequest(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response)
    {
       
    }
}
#endif

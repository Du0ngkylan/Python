/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUVersionHandler.cpp
 * @brief       Source file for implementing  information.
 * @author      anhnguyen
 * @date        2018/12/10
 */
#include "../include/FAMSDUVersionHandler.h"

#include <future>
#include <sstream>
#include "MDBHTTPServerUtility.h"
#include "MDBConnectionManager.h"
#include "FAMSDURqVersionAnalyzer.h"
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
    FAMSDUVersionHandler::FAMSDUVersionHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType)
        : FAMSDUBaseHandler { inputTemplate }, mRequestData { }, mResponseData { }, mRequestType {requestType}
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
    void FAMSDUVersionHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        auto startTime = famscommon::getCurrentTime(); /* pgr0745 */
        if (mRequestType == RequestType::VERSION_GET) {
            handleGetVersionRequest(request, response);
        }

        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
    }

    void FAMSDUVersionHandler::handleGetRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        mdb_apl_log_debug("---------------------------------------------------------------");
        mdb_apl_log_end();
    }

    void FAMSDUVersionHandler::handleGetVersionRequest(
            Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response) {
        mdb_apl_log_start();
        // Declare variable
        auto retCode =mdbcom::Result::OK;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto versionStore = std::vector<FAMSDUVersionData> {};

        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(SELECT_VERSION_QUERY); /*pgr0745*/
        auto retExec = selectStm->getResult();
        int result = mdbcom::Result::NG;

        FAMSDUVersionData versionModel;
        long long id;
        std::string version;
        if (mdbcom::Result::OK == retExec) {
            mdb_apl_log_debug("number of versionervers: %d", selectStm->getNumRows());

            for (int i = 0; i < selectStm->getNumRows(); i++) {
                selectStm->getValue(i, selectStm->getColumnIndex("version"), version);
                versionModel.setVersion(version);

                versionStore.push_back(versionModel);
            }
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
        }
        else {
            mdb_apl_log_debug("failed to get observers info");
        }

        mdb_apl_log_debug("number observer model: %d", versionStore.size());

        //build response JSON string
        auto resData = std::string {};

        resData = buildGetVersionsResponseData(retCode, errKeys, versionStore);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
        mdb_apl_log_end();
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
    int FAMSDUVersionHandler::validateJSONFormat(const std::string &jsonString, std::string &errKey)
    {
        mdb_apl_log_start();

        auto ret = mdbcom::Result::OK;
        mdbcom::MDBJsonParser jsonDecoder {};
        mdbcom::MDBContentParser::KeyMapType jsonMap;

        // decode JSON string to object
        auto jsonRet = jsonDecoder.decodeKeyMapData(jsonString, mJsonTemplate, "", jsonMap, errKey); /*pgr0745*/

        if (mdbcom::Json::OK != jsonRet || false == errKey.empty())
        {
            auto errKeyString = errKey.c_str();
            mdb_apl_log_critical("JSON format is abnormal, errorKey = %s", errKeyString);
            mdb_apl_log_end();
            return JSON_NG;
        }

        auto inputAnalyzer = FAMSDURqVersionAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(RequestType::OBSERVERS_POST);

        if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestData, errKey))
        {
            auto errKeyString = errKey.c_str();
            mdb_apl_log_error("Analyze result fail at %s", errKeyString);
            mdb_apl_log_end();
            return JSON_NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Build response data
     *
     * @param [in] retCode the return code
     * @param [in] errorCodes
     * @return the JSON response string
     */
    std::string FAMSDUVersionHandler::buildGetVersionsResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUVersionData> &versionStore)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (mdbcom::Result::OK == retCode)
        {
            auto versionsTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for(auto versionData: versionStore) {
                auto versionTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(versionTag, VERSION, versionData.getVersion());
                versionsTag.emplace_back(versionTag);
            }

            if (0 != versionsTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, DATA, versionsTag);
            }

        } else {
            auto errorsTag = buildErrorMessages(errorCodes);
            if (0 != errorsTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, ERRORS, errorsTag);
            }
        }

        auto ret = jsonEncoder.encodeKeyMapData(jsonMap, jsonResponse);/*pgr0745*/

        if (mdbcom::Json::OK != ret)
        {
            mdb_apl_log_critical("JSON encode is abnormal, cannot build response JSON");
        }

        mdb_apl_log_end();
        return jsonResponse;
    }

}


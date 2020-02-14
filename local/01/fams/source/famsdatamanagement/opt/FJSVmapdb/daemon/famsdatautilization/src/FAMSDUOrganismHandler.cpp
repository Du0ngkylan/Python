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
#include "FAMSDUOrganismThreshold.h"
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
        : FAMSDUBaseHandler { inputTemplate }, mRequestData { }, mResponseData { }, mRequestType {requestType}, mPostRequestData { }
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
        else if (mRequestType == RequestType::ORGANISM_GET) {
            handleOrganismGetRequest(request, response);
        }
        else if (mRequestType == RequestType::ORGANISM_DELETE) {
            handleOrganismDeleteRequest(request, response);
        }
        else if (mRequestType == RequestType::ORGANISM_PUT) {
            handleOrganismPutRequest(request, response);
        }
        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
    }
    Poco::Net::HTTPResponse::HTTPStatus FAMSDUOrganismHandler::getOrganismName (long long id, FAMSDUOrganismData &organismData, std::vector<std::string>& errKeys)
    {
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
         auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
         params.emplace_back(id);
         const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
         auto selecDetailStm = dbConnection.execute(SELECT_ORGANISM_ITEM_QUERY, params); /*pgr0745*/
         params.clear();
         auto retDetailExec = selecDetailStm->getResult();
         if (mdbcom::Result::OK == retDetailExec)
         {
             if (selecDetailStm->getNumRows() == 1)
             {
                 std::string name;
                 selecDetailStm->getValue(0, selecDetailStm->getColumnIndex("name"), name);
                 organismData.setName(name);
             }
             else
             {
                 mdb_sys_log_out("F110310");
                 errKeys.emplace_back("40400");
                 httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
             }
         }
         else
         {
             httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
             mdb_apl_log_critical("SQL search failed! ret=%d", retDetailExec);
             mdb_apl_log_end();
         }
         return httpRetCode;
    }
    Poco::Net::HTTPResponse::HTTPStatus FAMSDUOrganismHandler::getOrganismItemData (long long id, FAMSDUOrganismData &organismData)
    {
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
        params.emplace_back(id);
        mdb_apl_log_debug("id=%d", id);
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selecDetailStm = dbConnection.execute(SELECT_DETAIL_ORGANISM_QUERY, params); /*pgr0745*/
        params.clear();
        auto retDetailExec = selecDetailStm->getResult();
        if (mdbcom::Result::OK == retDetailExec)
        {
            mdb_apl_log_debug("number of threshold list: %d", selecDetailStm->getNumRows());
            for (int j = 0; j < selecDetailStm->getNumRows(); j++) {
                FAMSDUOrganismThreshold thresholdOrganismData;
                int type;
                selecDetailStm->getValue(j, selecDetailStm->getColumnIndex("type"), type);
                thresholdOrganismData.setType(type);
                std::string min_value;
                std::string min_unit;
                selecDetailStm->getValue(j, selecDetailStm->getColumnIndex("min_value"), min_value);
                selecDetailStm->getValue(j, selecDetailStm->getColumnIndex("min_unit"), min_unit);
                thresholdOrganismData.setMinValue(min_unit, min_value);
                std::string max_value;
                std::string max_unit;
                selecDetailStm->getValue(j, selecDetailStm->getColumnIndex("max_value"), max_value);
                selecDetailStm->getValue(j, selecDetailStm->getColumnIndex("max_unit"), max_unit);
                thresholdOrganismData.setMaxValue(max_unit, max_value);
                organismData.setThresHold(thresholdOrganismData);
            }
        }
        else
        {
            httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
            mdb_apl_log_critical("SQL search failed! ret=%d", retDetailExec);
            mdb_apl_log_end();
        }
        return httpRetCode;
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

    void FAMSDUOrganismHandler::handleOrganismPutRequest(Poco::Net::HTTPServerRequest &request,
                                                       Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        auto uriPath = std::string { }; /* pgr0745 */
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        double startTime = famscommon::getCurrentTime();
        // Declare variable
        std::vector<std::string> errKeys;
        auto resData = std::string {};
        auto retCode = mdbcom::Result::OK;
        // Extract URI path only
        auto uri = mdbcom::MDBURIHelper { request.getURI() }; /* pgr0745 */
        uriPath = uri.getPath();
        long long id = - 1;
        id = atoll(uriPath.c_str() + uriPath.find("/app/organisms/") + strlen("/app/organisms/"));
        if (id < 1)
        {
            mdb_sys_log_out("F110310");
            errKeys.emplace_back("40000");
            httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        }
        else
        {
            do
            {
                mdb_apl_log_debug("Input parameter check");
                //! Extract JSON data
                const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);
                //! Decode JSON data (std::string to map)
                auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};
                //JSON フォーマット定義の解析
                //Check request data
                std::string errKey;
                retCode = validateJSONFormat(jsonstring, jsonMap, errKey);

                //mRequestData is a structure that has data map with request body data
                if (mdbcom::Result::OK != retCode)
                {
                    mdb_sys_log_out("F110310");
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
                else {
                    httpRetCode = handlePostPutDatabase(id, true, errKeys);
                }
            }
            while (0);
            //write DB
//            if (httpRetCode == Poco::Net::HTTPResponse::HTTP_OK)
//            {
//            }

            double endTime = famscommon::getCurrentTime();
            mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        }

        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/
        ostr << resData;
        mdb_apl_log_end();
    }

    void FAMSDUOrganismHandler::handleOrganismListPostRequest(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        double startTime = famscommon::getCurrentTime();
        // Declare variable
        // ------------- return code START ---------------
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_OK; // HTTP return code
        // ------------- return code END   ---------------
        std::vector<std::string> errKeys;
        auto resData = std::string {};
        auto dynamicInfoList = mdbcom::MDBContentParser::KeyMapType {};
        auto dynamictId = std::string {};
        auto retCode = mdbcom::Result::OK;
        do
        {
            mdb_apl_log_debug("Input parameter check");
            //! Extract JSON data
            const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);
            //! Decode JSON data (std::string to map)
            auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};
            //JSON フォーマット定義の解析
            //Check request data
            std::string errKey;
            retCode = validateJSONFormat(jsonstring, jsonMap, errKey);

            //mRequestData is a structure that has data map with request body data
            if (mdbcom::Result::OK != retCode)
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
        }
        while (0);
        //write DB
        long long id;
        if (httpRetCode == Poco::Net::HTTPResponse::HTTP_OK)
        {
            httpRetCode = handlePostPutDatabase(id, false, errKeys);
        }

        resData = buildPostResponseData(httpRetCode, errKeys, id);

        if (httpRetCode == Poco::Net::HTTPResponse::HTTP_OK)
        {
            httpRetCode = Poco::Net::HTTPResponse::HTTP_CREATED;
        }

        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send();
        ostr << resData;

        double endTime = famscommon::getCurrentTime();
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));

        mdb_apl_log_end();
    }
    std::string FAMSDUOrganismHandler::buildPostResponseData(const Poco::Net::HTTPResponse::HTTPStatus &status, const std::vector<std::string> &errorCodes, const long long &id)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (Poco::Net::HTTPResponse::HTTP_OK != status)
        {
            auto errorsTag = buildErrorMessages(errorCodes);
            if (0 != errorsTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, ERRORS, errorsTag);
            }
        }
        else
        {
            if (id != -1)
            {
                auto organismTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(organismTag, ID, id);
                jsonEncoder.setKeyMapDataValue(jsonMap, DATA, organismTag);
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

    Poco::Net::HTTPResponse::HTTPStatus FAMSDUOrganismHandler::handlePostPutDatabase(long long &id, bool isPut, std::vector<std::string> &errKeys)
    {
        mdb_apl_log_start();
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
        std::string organismName = mPostRequestData.getName();
        params.emplace_back(organismName);
        if (isPut == true)
        {
            params.emplace_back(id);
            auto putStm = dbConnection.execute(PUT_ORGANISM_QUERY_INFO, params); /*pgr0745*/
            params.clear();
            auto retExec = putStm->getResult();
            std::string result;
            if (mdbcom::Result::OK == retExec)
            {
                putStm->getValue(0, 0, result);
                mdb_apl_log_debug("SQL update successfully : newID = %s", result.c_str());
                params.emplace_back(id);
                auto deleteStm = dbConnection.execute(DELETE_ORGANISM_QUERY_DETAIL, params); /*pgr0745*/
                if (mdbcom::Result::OK == retExec)
                {
                    std::string deleteResult;
                    deleteStm->getValue(0, 0, deleteResult);
                    mdb_apl_log_debug("SQL delete successfully : newID = %s", deleteResult.c_str());
                }
                else
                {
                    mdb_apl_log_debug("SQL delete failed!!!");
                }
            }
            else
                    {
                        id = -1;
                        mdb_apl_log_end();
                        return Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
                    }        
        }
        else
        {
            auto postStm = dbConnection.execute(POST_ORGANISM_QUERY_INFO, params); /*pgr0745*/
            params.clear();
            auto retExec = postStm->getResult();
            std::string result;
            if (mdbcom::Result::OK == retExec)
            {
                postStm->getValue(0, 0, result);
                mdb_apl_log_debug("SQL insert successfully : newID = %s", result.c_str());
                if (postStm->getNumRows() == 1)
                {
                    postStm->getValue(0, postStm->getColumnIndex("id"), id);
                } else {
                    id = -1;
                    mdb_apl_log_end();
                    return Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
                }
            }
            else
            {
                id = -1;
                mdb_apl_log_end();
                return Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
            }
        }
        params.clear();
        std::vector<std::string> maxUnitList = mPostRequestData.getMaxUnit();
        std::vector<std::string> minUnitList = mPostRequestData.getMinUnit();
        std::vector<std::string> maxValueList = mPostRequestData.getMaxValue();
        std::vector<std::string> minValueList = mPostRequestData.getMinValue();
        std::vector<int> organismType = mPostRequestData.getType();
        for (int index = 0; index < maxUnitList.size(); index++)
        {
            mdb_apl_log_debug("id %lld", id);
            params.emplace_back(id);
            params.emplace_back(organismType[index]);
			params.emplace_back(minValueList[index]);
            params.emplace_back(minUnitList[index]);
            params.emplace_back(maxValueList[index]);
            params.emplace_back(maxUnitList[index]);
            auto insertStm = dbConnection.execute(POST_ORGANISM_QUERY_DETAIL, params); /*pgr0745*/
            params.clear();
            auto retExec = insertStm->getResult();
            if (mdbcom::Result::OK == retExec)
            {
                std::string insertResult;
                insertStm->getValue(0, 0, insertResult);
                mdb_apl_log_debug("SQL insert successfully : newID = %s", insertResult.c_str());
            }
            else
            {
                mdb_apl_log_critical("SQL insert failed!!!");
            }
        }
        mdb_apl_log_end();
        return Poco::Net::HTTPResponse::HTTP_OK;
    }

    void FAMSDUOrganismHandler::handleOrganismGetRequest(Poco::Net::HTTPServerRequest &request,
                                       Poco::Net::HTTPServerResponse &response)
    {
        auto uriPath = std::string { }; /* pgr0745 */
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        // Extract URI path only
        auto uri = mdbcom::MDBURIHelper { request.getURI() }; /* pgr0745 */
        std::vector<std::string> errKeys;
        FAMSDUOrganismData organismData;
        uriPath = uri.getPath();
        long long id = - 1;
        id = atoll(uriPath.c_str() + uriPath.find("/app/organisms/") + strlen("/app/organisms/"));
        httpRetCode = getOrganismName(id,organismData, errKeys);
        if (httpRetCode == Poco::Net::HTTPResponse::HTTP_OK)
        {
            httpRetCode = getOrganismItemData(id, organismData);
        }
        auto resData = std::string {};

        resData = buildGetOrganismItemResponseData(httpRetCode, errKeys, organismData);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);
        auto &ostr = response.send(); /*pgr0745*/
        ostr << resData;
        mdb_apl_log_end();

    }

    void FAMSDUOrganismHandler::handleOrganismListGetRequest(Poco::Net::HTTPServerRequest &request,
                               Poco::Net::HTTPServerResponse &response)
    {
        // Declare variable
        auto retCode = OK; /*pgr0745*/
        std::vector<std::string> errKeys;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        auto dataStore = std::vector<FAMSDUOrganismData> {};

        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectListStm = dbConnection.execute(SELECT_ORGANISM_LIST_QUERY); /*pgr0745*/
        auto retListExec = selectListStm->getResult();

        if (mdbcom::Result::OK == retListExec) {
            mdb_apl_log_debug("number of organism list: %d", selectListStm->getNumRows());
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;

            for (int i = 0; i < selectListStm->getNumRows(); i++) {
                FAMSDUOrganismData organismData;
                long long id;
                std::string name;
                selectListStm->getValue(i, selectListStm->getColumnIndex("id"), id);
                organismData.setId(id);
                selectListStm->getValue(i, selectListStm->getColumnIndex("name"), name);
                organismData.setName(name);
                httpRetCode = getOrganismItemData(id, organismData);
                if (httpRetCode == Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR) {
                    break;
                }
                dataStore.push_back(organismData);
            }
        }
        else {
            mdb_apl_log_debug("failed to get organism info");
        }

        mdb_apl_log_debug("number organism list: %d", dataStore.size());

        //build response JSON string
        auto resData = std::string {};

        resData = buildGetOrganismListResponseData(retCode, errKeys, dataStore);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);
        auto &ostr = response.send(); /*pgr0745*/
        ostr << resData;
        mdb_apl_log_end();
    }
    void FAMSDUOrganismHandler::buildOrganismItemData (mdbcom::MDBJsonParser &jsonEncoder, const FAMSDUOrganismData &organismData, mdbcom::MDBContentParser::KeyMapType &dataTag, bool isShowID)
    {
        if (isShowID == true)
        {
            jsonEncoder.setKeyMapDataValue(dataTag, ID, organismData.getId());
        }
        jsonEncoder.setKeyMapDataValue(dataTag, NAME, organismData.getName());
        std::vector<FAMSDUOrganismThreshold> thresholdList = organismData.getThresHoldList();
        auto thresholdsTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
        for(auto threshold: thresholdList) {
            auto thresholdTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(thresholdTag, TYPE, threshold.getType());
            auto minTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(thresholdTag, MIN_VALUE, roundedUp2Digits(threshold.getMinValue().getValue()));
            jsonEncoder.setKeyMapDataValue(thresholdTag, MIN_UNIT, threshold.getMinValue().getUnit());
//            jsonEncoder.setKeyMapDataValue(thresholdTag, MIN, minTag);
            auto maxTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(thresholdTag, MAX_VALUE, roundedUp2Digits(threshold.getMaxValue().getValue()));
            jsonEncoder.setKeyMapDataValue(thresholdTag, MAX_UNIT, threshold.getMaxValue().getUnit());
//            jsonEncoder.setKeyMapDataValue(thresholdTag, MAX, maxTag);
            thresholdsTag.push_back(thresholdTag);
        }
        jsonEncoder.setKeyMapDataArrayValue(dataTag, THRESHOLDS, thresholdsTag);
    }
    std::string FAMSDUOrganismHandler::buildGetOrganismItemResponseData(Poco::Net::HTTPResponse::HTTPStatus &httpStatus,
                    const std::vector<std::string> &errorCodes,
                    const FAMSDUOrganismData &dataStore)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
        auto dataTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
        if (Poco::Net::HTTPResponse::HTTP_OK == httpStatus)
        {
            buildOrganismItemData(jsonEncoder, dataStore, dataTag, false);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, dataTag);
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
    std::string FAMSDUOrganismHandler::buildGetOrganismListResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUOrganismData> &dataStore)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (OK == retCode)
        {
            auto datasTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for(auto data: dataStore) {
                auto dataTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                buildOrganismItemData(jsonEncoder, data, dataTag, true);
                datasTag.push_back(dataTag);
             }

            if (0 != datasTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, DATA, datasTag);
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

    void FAMSDUOrganismHandler::handleOrganismDeleteRequest(Poco::Net::HTTPServerRequest &request,
                                                       Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::NG;;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto uri = request.getURI();
        auto i = uri.find_last_of(URI_ORGANISM_LIST_PATH + "\/");
        auto obsId = atoll(uri.substr(i + 1).c_str());

        if (obsId > 0) {
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(obsId);

            auto deleteStm = dbConnection.execute(DELETE_ORGANISM_QUERY, params); /*pgr0745*/
            auto retExec = deleteStm->getResult();
            if (mdbcom::Result::OK == retExec && deleteStm->getNumRows() == 1) {
                httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                retCode = mdbcom::Result::OK;
            }
            else {
                errKeys.emplace_back("40400");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                mdb_apl_log_debug("failed to delete organism id: %ll", obsId);
            }
        } else {
            mdb_apl_log_error("Can't parse the input organism ID");
            httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        }

        // build response JSON string
        auto resData = std::string {};

        resData = buildDeleteOrganismItemResponseData(retCode, errKeys);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
        mdb_apl_log_end();
    }
    Poco::Net::HTTPResponse::HTTPStatus FAMSDUOrganismHandler::deleteOrganismName (long long id,std::vector<std::string>& errKeys)
    {
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
         auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
         params.emplace_back(id);
         const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
         auto selecDetailStm = dbConnection.execute(DELETE_ORGANISM_QUERY, params); /*pgr0745*/
         params.clear();
         auto retDetailExec = selecDetailStm->getResult();
         if (mdbcom::Result::OK == retDetailExec)
         {
             if (selecDetailStm->getNumRows() == 0)
             {
                 mdb_sys_log_out("F110310");
                 errKeys.emplace_back("40900");
                 httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                 mdb_apl_log_trace("Delete fail=%lld", id);
                 mdb_apl_log_end();
             }
         }
         return httpRetCode;
    }
    std::string FAMSDUOrganismHandler::buildDeleteOrganismItemResponseData(int &retCode,
            const std::vector<std::string> &errorCodes)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (mdbcom::Result::NG == retCode)
        {
            auto errorsTag = buildErrorMessages(errorCodes);
            if (0 != errorsTag.size())
            {
               jsonEncoder.setKeyMapDataArrayValue(jsonMap, ERRORS, errorsTag);
            }
        }

        auto ret = jsonEncoder.encodeKeyMapData(jsonMap, jsonResponse);
        if (mdbcom::Json::OK != ret)
        {
            mdb_apl_log_critical("JSON encode is abnormal, cannot build response JSON");
        }

        mdb_apl_log_end();
        return jsonResponse;
    }
}
#endif

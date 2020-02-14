/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUObserversHandler.cpp
 * @brief       Source file for implementing  information.
 * @author      anhnguyen
 * @date        2018/12/10
 */
#include "../include/FAMSDUObserversHandler.h"

#include <future>
#include <sstream>
#include "MDBHTTPServerUtility.h"
#include "MDBConnectionManager.h"
#include "FAMSDURqObserversAnalyzer.h"
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
    FAMSDUObserversHandler::FAMSDUObserversHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType)
        : FAMSDUBaseHandler { inputTemplate }, mRequestData { }, mResponseData { }, mRequestType {requestType}, mSerializeRequestData {}
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
    void FAMSDUObserversHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        auto startTime = famscommon::getCurrentTime(); /* pgr0745 */
        if (mRequestType == RequestType::OBSERVERS_GET) {
            handleGetObserversRequest(request, response);
        } else if (mRequestType == RequestType::OBSERVERS_POST)
        {
            handlePostObserversRequest(request, response);
        } else if (mRequestType == RequestType::OBSERVERS_PUT)
        {
            handlePutObserversRequest(request, response);
        } else if (mRequestType == RequestType::OBSERVERS_DELETE) {
            handleDeleteObserversRequest(request, response);
        }

        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
    }

    void FAMSDUObserversHandler::handlePutObserversRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::OK; /*pgr0745*/
        std::vector<std::string> errKeys;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;

        do
        {
            mdb_apl_log_debug("Input parameter check");
            const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);

            //JSON フォーマット定義の解析
            //Check request data
            std::string validateErrs;
            mdb_apl_log_debug("handlePostObserversRequest Input parameter check %s", jsonstring.c_str());
            retCode = validateJSONFormat(jsonstring, validateErrs, RequestType::OBSERVERS_PUT);
            if (retCode != mdbcom::Result::OK)
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            mdb_apl_log_debug("Input parameter check: %s", mRequestData.getMail().c_str());
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;

        }
        while (0);

        if (httpRetCode == Poco::Net::HTTPResponse::HTTP_OK)
        {
            auto uri = mdbcom::MDBURIHelper { request.getURI() }; /* pgr0745 */
            auto uriPath = uri.getPath();
            long long id = - 1;
            id = atoll(uriPath.c_str() + uriPath.find("/app/observers/") + strlen("/app/observers/"));
            mdb_apl_log_debug("ID: %lld", id);
            if (id > 0)
            {
                mSerializeRequestData.setId(id);
                httpRetCode = handlePutDatabase(errKeys);
            }
            else
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
            }
        }

       //build response JSON string
        auto resData = std::string {};

        resData = buildPutResponseData(errKeys);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
        mdb_apl_log_end();
    }

    void FAMSDUObserversHandler::handleGetObserversRequest(
            Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response) {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::OK;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto obsStore = std::vector<FAMSDUObserversData> {};

        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(SELECT_OBSERVERS_QUERY); /*pgr0745*/
        auto retExec = selectStm->getResult();
        int result = mdbcom::Result::NG;

        FAMSDUObserversData obsModel;
        long long id;
        std::string mail;
        if (mdbcom::Result::OK == retExec) {
            mdb_apl_log_debug("number of observers: %d", selectStm->getNumRows());

            for (int i = 0; i < selectStm->getNumRows(); i++) {
                selectStm->getValue(i, selectStm->getColumnIndex("id"), id);
                selectStm->getValue(i, selectStm->getColumnIndex("mail"), mail);
                obsModel.setId(id);
                obsModel.setMail(mail);

                obsStore.push_back(obsModel);
            }
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
        }
        else {
            mdb_apl_log_debug("failed to get observers info");
        }

        mdb_apl_log_debug("number observer model: %d", obsStore.size());

        //build response JSON string
        auto resData = std::string {};

        resData = buildGetObserversResponseData(retCode, errKeys, obsStore);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
        mdb_apl_log_end();
    }

    int FAMSDUObserversHandler::getObserverByEmail(const std::string mail) {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::OK;
        std::vector<std::string> errKeys;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {};
        params.emplace_back(mail);

        auto selectStm = dbConnection.execute(SELECT_OBSERVERS_BY_EMAIL_QUERY, params); /*pgr0745*/

        auto retExec = selectStm->getResult();
        int result = mdbcom::Result::NG;

        if (mdbcom::Result::OK == retExec) {
            mdb_apl_log_debug("number of observers: %d", selectStm->getNumRows());
        }
        else {
            mdb_apl_log_debug("failed to get observers info");
        }

        mdb_apl_log_end();
        return selectStm->getNumRows();
    }

    void FAMSDUObserversHandler::handleDeleteObserversRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::NG;;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto uri = request.getURI();
        auto i = uri.find_last_of(URI_GET_OBSERVERS_PATH + "\/");
        auto obsId = atoll(uri.substr(i + 1).c_str());

        if (obsId > 0) {
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(obsId);

            auto deleteStm = dbConnection.execute(DELETE_OBSERVERS_QUERY, params); /*pgr0745*/
            auto retExec = deleteStm->getResult();

            if (mdbcom::Result::OK == retExec && deleteStm->getNumRows() == 1) {
                httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                retCode = mdbcom::Result::OK;
            }
            else {
                errKeys.emplace_back("40400");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                mdb_apl_log_debug("failed to delete observers id: %ll", obsId);
            }
        } else {
            mdb_apl_log_error("Can't parse the input observer ID");
            httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        }

        // build response JSON string
        auto resData = std::string {};

        resData = buildDeleteObserversResponseData(retCode, errKeys);
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
    int FAMSDUObserversHandler::validateJSONFormat(const std::string &jsonString, std::string &errKey,const RequestType &type)
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

        auto inputAnalyzer = FAMSDURqObserversAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(type);

        if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mSerializeRequestData, errKey))
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
    std::string FAMSDUObserversHandler::buildGetObserversResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUObserversData> &obsStore)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (mdbcom::Result::OK == retCode)
        {
            auto obssTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for(auto obsData: obsStore) {
                auto obsTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(obsTag, ID, obsData.getId());
                jsonEncoder.setKeyMapDataValue(obsTag, MAIL, obsData.getMail());
                obssTag.emplace_back(obsTag);
            }

            if (0 != obssTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, DATA, obssTag);
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

    void FAMSDUObserversHandler::handlePostObserversRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::OK; /*pgr0745*/
        std::vector<std::string> errKeys;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;

        do
        {
            mdb_apl_log_debug("Input parameter check");
            const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);

            //JSON フォーマット定義の解析
            //Check request data
            std::string validateErrs;
            mdb_apl_log_debug("handlePostObserversRequest Input parameter check %s", jsonstring.c_str());
            retCode = validateJSONFormat(jsonstring, validateErrs, RequestType::OBSERVERS_POST);
            if (retCode != mdbcom::Result::OK)
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            mdb_apl_log_debug("Input parameter check: %s", mRequestData.getMail().c_str());
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;

        }
        while (0);
        std::vector<long long> idList;
        if (httpRetCode == Poco::Net::HTTPResponse::HTTP_OK)
        {
            httpRetCode = handlePostDatabase(errKeys, idList);
        }

       //build response JSON string
        auto resData = std::string {};

        resData = buildPostResponseData(retCode, errKeys, idList);
        if (httpRetCode == Poco::Net::HTTPResponse::HTTP_OK)
        {
            httpRetCode = Poco::Net::HTTPResponse::HTTP_CREATED;
        }
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
        mdb_apl_log_end();
    }

    Poco::Net::HTTPResponse::HTTPStatus FAMSDUObserversHandler::handlePutDatabase(std::vector<std::string> &errKeys)
    {
        mdb_apl_log_start();
        Poco::Net::HTTPResponse::HTTPStatus status = Poco::Net::HTTPResponse::HTTP_OK;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
        params.emplace_back(mSerializeRequestData.getSingleMail().c_str());
        params.emplace_back(mSerializeRequestData.getId());
        auto putStm = dbConnection.execute(PUT_OBSERVERS_QUERY, params); /*pgr0745*/
        params.clear();
        auto retExec = putStm->getResult();
        if (mdbcom::Result::OK == retExec)
        {
            if (putStm->getNumRows() == 0) {
                errKeys.emplace_back("40400");
                status = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
            }
        }
        else
        {
            status = Poco::Net::HTTPResponse::HTTP_CONFLICT;
            mdb_sys_log_out("F110310");
            errKeys.emplace_back("40900");
        }

        mdb_apl_log_end();
        return status;
    }

    Poco::Net::HTTPResponse::HTTPStatus FAMSDUObserversHandler::handlePostDatabase(std::vector<std::string> errKeys, std::vector<long long> &idList)
    {
        mdb_apl_log_start();
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
        std::vector<std::string> mailList = mSerializeRequestData.getMail();
        Poco::Net::HTTPResponse::HTTPStatus retStatus = Poco::Net::HTTPResponse::HTTP_OK;
        for (auto index = 0u; index < mailList.size(); index ++)
        {
            const char *mail = mailList[index].c_str();
            params.emplace_back(mail);
            auto selectStm = dbConnection.execute(POST_OBSERVERS_QUERY, params); /*pgr0745*/
            params.clear();
            auto retExec = selectStm->getResult();
            std::string result;
            if (mdbcom::Result::OK == retExec)
            {
                selectStm->getValue(0, 0, result);
                long long id;
                mdb_apl_log_debug("SQL insert successfully : newID = %s", result.c_str());
                if (selectStm->getNumRows() == 1)
                {
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), id);
                    idList.push_back(id);
                } else {
                    idList.push_back(-1);
                    retStatus = Poco::Net::HTTPResponse::HTTP_CONFLICT;
                }
            }
            else
            {
                idList.push_back(-1);
                retStatus = Poco::Net::HTTPResponse::HTTP_CONFLICT;
            }
        }
        mdb_apl_log_end();
        return retStatus;
    }

    std::string  FAMSDUObserversHandler::buildPostResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            std::vector<long long> idList)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (mdbcom::Result::OK != retCode)
        {
            auto errorsTag = buildErrorMessages(errorCodes);
            if (0 != errorsTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, ERRORS, errorsTag);
            }
        }
        else
        {
            auto obssTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for(auto id: idList) {
                auto obsTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(obsTag, ID, id);
                obssTag.emplace_back(obsTag);
            }

            if (0 != obssTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, DATA, obssTag);
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

    std::string  FAMSDUObserversHandler::buildPutResponseData(const std::vector<std::string> &errorCodes)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (errorCodes.size() > 0)
        {
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
    /*!
     * @brief Build response data
     *
     * @param [in] retCode the return code
     * @param [in] errorCodes
     * @return the JSON response string
     */
    std::string FAMSDUObserversHandler::buildDeleteObserversResponseData(int &retCode,
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


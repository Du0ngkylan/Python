/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUUserHandler.cpp
 * @brief       Source file for implementing  information.
 * @author      anhnguyen
 * @date        2018/12/10
 */
#include <future>
#include <sstream>
#include "MDBHTTPServerUtility.h"
#include "MDBConnectionManager.h"
#include "FAMSDUUserHandler.h"
#include "FAMSDURqUserAnalyzer.h"
#include "MDBJsonParser.h"

using famscommon::JSON_STRING_MISSING_VALUE;
using famscommon::JSON_INTEGER_MISSING_VALUE;
using famscommon::JSON_DOUBLE_MISSING_VALUE;
using famscommon::JSON_DOUBLE_TIME_MISSING_VALUE;

namespace famsdatautil {
    constexpr int FAMSDUBaseHandler::REST_RET_INVALID_PARAMETERS;
    constexpr int FAMSDUBaseHandler::REST_RET_ABNORMAL_OTHERS;
    constexpr const char *const FAMSDUBaseHandler::INVALID_PARAMETERS_MESS;
    constexpr const char *const FAMSDUBaseHandler::ABNORMAL_OTHERS_MESS;

    /*!
     * @brief The constructor.
     * @param [in] sharedMemory the shared memory from main.
     * @return None.
     */
    FAMSDUUserHandler::FAMSDUUserHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType)
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
    void FAMSDUUserHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        auto startTime = famscommon::getCurrentTime(); /* pgr0745 */
        if (mRequestType == RequestType::LOGIN_POST) {
            handleLoginRequest(request, response);
        } else if (mRequestType == RequestType::USERS_GET) {
            handleGetUsersRequest(request, response);
        }
        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
    }

    void FAMSDUUserHandler::handleLoginRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        // Declare variable
        auto retCode = OK; /*pgr0745*/
        std::vector<std::string> errKeys;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        FAMSDUUserData userData;

        do
        {
            mdb_apl_log_debug("Input parameter check");
            const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);

            //JSON フォーマット定義の解析
            //Check request data
            std::string validateErrs;
            mdb_apl_log_debug("Input parameter check %s", jsonstring.c_str());
            retCode = validateJSONFormat(jsonstring, validateErrs);
            if (retCode != OK)
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            mdb_apl_log_debug("Input parameter check: %s", mRequestData.getUsername().c_str());
            if (authenticate(mRequestData.getUsername(), mRequestData.getPassword(), userData) == mdbcom::Result::NG) {
                mdb_apl_log_error("Invalid credential value");
                retCode = NG;
            } else {
                mdb_apl_log_info("Authenticated successful");
            }
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;

        }
        while (0);

        //build response JSON string
        auto resData = std::string {};

        resData = buildLoginResponseData(retCode, errKeys, userData);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
    }

    void FAMSDUUserHandler::handleGetRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {

    }

    void FAMSDUUserHandler::handleGetUsersRequest(
            Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response) {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = OK;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto userStore = std::vector<FAMSDUUserData> {};

        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(SELECT_USERS_QUERY); /*pgr0745*/
        auto retExec = selectStm->getResult();
        int result = mdbcom::Result::NG;

        if (mdbcom::Result::OK == retExec) {
            mdb_apl_log_debug("number of users: %d", selectStm->getNumRows());

            FAMSDUUserData userModel;
            long long id;
            std::string username;
            std::string mail;
            std::string fullName;
            for (int i = 0; i < selectStm->getNumRows(); i++) {
                selectStm->getValue(i, selectStm->getColumnIndex("id"), id);
                selectStm->getValue(i, selectStm->getColumnIndex("username"), username);
                selectStm->getValue(i, selectStm->getColumnIndex("mail"), mail);
                selectStm->getValue(i, selectStm->getColumnIndex("full_name"), fullName);
                userModel.setId(id);
                userModel.setUsername(username);
                userModel.setMail(mail);
                userModel.setFullName(fullName);

                userStore.emplace_back(userModel);
            }
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
        }
        else {
            mdb_apl_log_debug("failed to get users info");
        }

        mdb_apl_log_debug("number user model: %d", userStore.size());

        //build response JSON string
        auto resData = std::string {};

        resData = buildGetUsersInfoResponseData(retCode, errKeys, userStore);
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
    int FAMSDUUserHandler::validateJSONFormat(const std::string &jsonString, std::string &errKey)
    {
        mdb_apl_log_start();

        auto ret = OK;
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

        auto inputAnalyzer = FAMSDURqUserAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(RequestType::LOGIN_POST);

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
     * @param [in] message message to return when return code = 1 or 999
     * @param [in] position cause error in case return code =1 or 999
     * @param [in] sensorInfoList JSON key-value after building
     * @return the JSON response string
     */
    std::string FAMSDUUserHandler::buildLoginResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const FAMSDUUserData &userData)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
        jsonEncoder.setKeyMapDataValue(jsonMap, RESULT, retCode);
        
        if (OK == retCode)
        {
            auto userTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(userTag, ID, userData.getId());
            jsonEncoder.setKeyMapDataValue(userTag, USER_NAME, userData.getUsername());
            jsonEncoder.setKeyMapDataValue(userTag, FULL_NAME, userData.getFullName());
            jsonEncoder.setKeyMapDataValue(userTag, MAIL, userData.getMail());

            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, userTag);
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

    /*!
     * @brief Build response data
     *
     * @param [in] retCode the return code
     * @param [in] errorCodes
     * @return the JSON response string
     */
    std::string FAMSDUUserHandler::buildGetUsersInfoResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUUserData> &userStore)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (OK == retCode)
        {
            auto usersTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for(auto userData: userStore) {
                auto userTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(userTag, ID, userData.getId());
                jsonEncoder.setKeyMapDataValue(userTag, USER_NAME, userData.getUsername());
                jsonEncoder.setKeyMapDataValue(userTag, FULL_NAME, userData.getFullName());
                jsonEncoder.setKeyMapDataValue(userTag, MAIL, userData.getMail());
                usersTag.emplace_back(userTag);
            }

            if (0 != usersTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, DATA, usersTag);
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


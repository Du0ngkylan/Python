/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRBaseHandler.cpp
 * @brief       The implement of FAMSDRBaseHandler class.
 * @author      anhnguyen.
 * @date        2018/12/11
 */

#include "FAMSDRBaseHandler.h"
#include "MDBJsonParser.h"
#include "FAMSIpcCisternSensorData.h"
#include <Poco/String.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/NetException.h>
#include <Poco/RegularExpression.h>
#include "MDBURIHelper.h"
#include "MDBResourceMapper.h"
#include "MDBResourceNotFoundHandler.h"
#include "MDBYamlConfig.h"
namespace famsdatareception {
    const std::string FAMSDRBaseHandler::RESPONSE_MESSAGE_BY_WRONG_INPUT {"型が正しくありません。"};
    const std::string FAMSDRBaseHandler::RESPONSE_MESSAGE_BY_FAILED_REGISTER {"マッピング処理で異常が発生しました。"};
    /*!
     * @brief Send ipc information for event processing process
     * @param [in] eventType event type to send to queue
     * @retval REST_RET_NORMAL normally
     * @retval REST_RET_ABNORMAL_SERVER abnormally
     */
    int FAMSDRBaseHandler::sendIpcInfoData(famscommon::MessageType eventType)
    {
        mdb_apl_log_start();

        auto ret = mdbcom::Result::OK; 
        auto dataLength = std::size_t { 0 };
        // fix Valgrind mem leak. DONT FIX PLEASE
        auto sensorInfo = getIpcInfoData(eventType, dataLength);
        if ((nullptr != mSharedMemory) && (nullptr != sensorInfo))
        {
            auto sensorInfoData = sensorInfo.get(); /*pgr0745*/

            auto recData = reinterpret_cast<const famscommon::FAMSIpcCisternSensorData *>(sensorInfoData);
//            std::size_t size = 0;
//            const char *accumulated_time = recData->getAccumulatedTime(size);
//            const char *cistern_code = recData->getCisternCode(size);
//            const double *inflowTemp = recData->getInflowTemp(size);
//            const double *outflowTemp = recData->getOutflowTemp(size);
//            const int *upperCentralIll = recData->getUpperCentralIll(size);
//            const int *lowerCentralIll = recData->getLowerCentralIll(size);
//            const double *salt = recData->getSalt(size);
//            const double *ph = recData->getPH(size);
//
//            for(std::vector<double>::size_type i = 0; i != size; i++) {
//                /* std::cout << v[i]; ... */
//                mdb_apl_log_info("accumulated_time = %s", accumulated_time + (i *famscommon::ACCUMULATED_TIME_LEN));
//                mdb_apl_log_info("cistern_code = %s", cistern_code + (i *famscommon::CISTERN_CODE_LEN));
//                mdb_apl_log_info("inflowTemp = %f", inflowTemp[i]);
//                mdb_apl_log_info("outflowTemp = %f", outflowTemp[i]);
//                mdb_apl_log_info("upperCentralIll = %d", upperCentralIll[i]);
//                mdb_apl_log_info("lowerCentralIll = %d", lowerCentralIll[i]);
//                mdb_apl_log_info("salt = %f", salt[i]);
//                mdb_apl_log_info("ph = %f", ph[i]);
//
//            }

            if (mdbcom::Result::OK != mSharedMemory->writeData(sensorInfoData, dataLength))
            {
                mdb_apl_log_critical("Write Data to shared memory failed");
                ret = mdbcom::Result::NG;
            }
            else
            {
                ret = mdbcom::Result::OK;
            }
        }

        mdb_apl_log_end();
        return ret;
    }

    std::string FAMSDRBaseHandler::buildErrorResponseData(
        const std::vector<std::string> errorCodes)
    {
        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        auto errorsTag = buildErrorMessages(errorCodes);
        if (0 != errorsTag.size())
        {
            jsonEncoder.setKeyMapDataArrayValue(jsonMap, ERRORS, errorsTag);
        }
        jsonEncoder.encodeKeyMapData(jsonMap, jsonResponse);
        return jsonResponse;
    }


    /*!
     *  @brief handler for request receive from Big Data Server.
     *
     *  @param [in] request the HTTP request data
     *  @param [out] response the HTTP response data
     *
     *  @return None.
     */
    void FAMSDRBaseHandler::handleRequest(
        Poco::Net::HTTPServerRequest &request,
        Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        double startTime = famscommon::getCurrentTime();

        // Declare variable
        // ------------- return code START ---------------
        auto apiRetCode = REST_RET_NORMAL; // RestAPI return code
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_OK; // HTTP return code
        auto retCode = mdbcom::Result::OK; /* pgr0745 */ // function return code
        std::string retMsg { };
        // ------------- return code END   ---------------

        std::vector<std::string> errKeys;
        std::vector<std::string> returnErrors;
        auto resData = std::string {};
        auto dynamicInfoList = mdbcom::MDBContentParser::KeyMapType {};
        auto dynamictId = std::string {};

        do
        {
            mdb_apl_log_debug("Input parameter check");
            //! Extract JSON data
            const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);
            //! Decode JSON data (std::string to map)
            auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};
            //JSON フォーマット定義の解析
            //Check request data
            retCode = validateJSONFormat(jsonstring, jsonMap, errKeys);

            //mRequestData is a structure that has data map with request body data
            if (mdbcom::Result::OK != retCode)
            {
                // Send it to Automobile Big Data part
                // syslog : に障害物情報登録のInput値が不正であったので、本機能が異常終了した旨のログを出力する
                // syslog (en) : Since Input value of Obstacle information registration was invalid, this function ended abnormally
                // CODE = 1
                // Message = “型が正しくありません。” (EN: Format is not correct)
                mdb_sys_log_out("F110310");
                returnErrors.emplace_back("40000");
                apiRetCode = REST_RET_INCORRECT_FORMAT;
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                retMsg = HTTP_RESPONSE_CODE_1;
            }
        }
        while (0);

        response.setStatusAndReason(httpRetCode);

        auto &ostr = response.send();
        if (httpRetCode != Poco::Net::HTTPResponse::HTTP_OK)
        {
            resData = buildErrorResponseData(returnErrors);
            ostr << resData;
        }
        response.setContentLength(resData.length());
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        registerEvent();
        double endTime = famscommon::getCurrentTime();
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));

        mdb_apl_log_end();
    }


    //! Key string constant
    const std::string FAMSDRBaseHandler::JSON_MAP_KEYS_STR
    { "Keys" };
    //! Code string constant
    const std::string FAMSDRBaseHandler::JSON_MAP_CODE_STR
    { "Code" };
    //! Message string constant
    const std::string FAMSDRBaseHandler::JSON_MAP_MSG_STR
    { "Message" };
}


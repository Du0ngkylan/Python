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
#include "FAMSDURqTaskPondInputAnalyzer.h"
#include "FAMSDURqTaskFeedingInputAnalyzer.h"
#include "FAMSDURqTaskRecoveryInputAnalyzer.h"
#include "FAMSDURqTaskDisposalInputAnalyzer.h"
#include "FAMSDURqTaskShippingInputAnalyzer.h"
#include "FAMSDURqTaskWaterInputAnalyzer.h"
#include "FAMSDURqTaskMaintenanceInputAnalyzer.h"
#include "FAMSDURqTaskMonitorInputAnalyzer.h"
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
        : FAMSDUBaseHandler { inputTemplate }, mRequestPondInputData { }, mRequestFeedingInputData { },
            mRequestRecoveryInputData { }, mRequestDisposalInputData { }, mRequestShippingInputData { },
            mRequestWaterInputData { }, mRequestMaintenanceInputData { }, mRequestMonitorInputData { },
            mRequestType {requestType}
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
        } else if (mRequestType == RequestType::TASK_DELETE) {
            handleDeleteTaskRequest(request, response);
        } else if (mRequestType == RequestType::TASKS_POST) {
            handlePostTaskRequest(request, response);
        } else if (mRequestType == RequestType::TASK_GET) {
            handleGetTaskRequest(request, response);
        } else if (mRequestType == RequestType::TASK_PUT) {
            handlePutTaskRequest(request, response);
        }

        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
    }

    void FAMSDUTaskHandler::handlePostTaskRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        // Declare variable
        auto retCode = mdbcom::Result::OK; /*pgr0745*/
        std::vector<std::string> errKeys;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        long long newId;

        do
        {
            mdb_apl_log_debug("Input parameter check");
            const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);

            //JSON フォーマット定義の解析
            //Check request data
            std::string validateErr;
            mdbcom::MDBContentParser::KeyMapType jsonMap;
            mdb_apl_log_debug("Input parameter check %s", jsonstring.c_str());
            retCode = validateJSONFormat(jsonstring, jsonMap);
            if (retCode != mdbcom::Result::OK)
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            mdbcom::MDBJsonParser jsonDecoder {};
            if (jsonDecoder.isExistKey(POND_INPUT, DATA, jsonMap)) {
                operatePondDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            } else if (jsonDecoder.isExistKey(FEEDING_INPUT, DATA, jsonMap)) {
                operateFeedingDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            } else if (jsonDecoder.isExistKey(RECOVERY_INPUT, DATA, jsonMap)) {
                operateRecoveryDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            } else if (jsonDecoder.isExistKey(DISPOSAL_INPUT, DATA, jsonMap)) {
                operateDisposalDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            } else if (jsonDecoder.isExistKey(SHIPPING_INPUT, DATA, jsonMap)) {
                operateShippingDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            } else if (jsonDecoder.isExistKey(WATER_INPUT, DATA, jsonMap)) {
                operateWaterDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            } else if (jsonDecoder.isExistKey(MAINTENANCE_INPUT, DATA, jsonMap)) {
                operateMaintenanceDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            } else if (jsonDecoder.isExistKey(WATER_MONITOR_INPUT, DATA, jsonMap)) {
                operateMonitorDataInDB(RequestType::TASKS_POST, errKeys, newId, jsonMap);
            }
            httpRetCode = Poco::Net::HTTPResponse::HTTP_CREATED;

        }
        while (0);

        //build response JSON string
        auto resData = std::string {};

        resData = buildPostResponseData(errKeys, newId);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
    }

    std::string FAMSDUTaskHandler::buildPostResponseData(std::vector<std::string> errKeys, long long newId) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
        
        if ( errKeys.size()== 0)
        {
            auto idTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(idTag, ID, newId);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, idTag);
        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    void FAMSDUTaskHandler::handlePutTaskRequest(
            Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response) {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::OK;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto resData = std::string {};
        double startTime = famscommon::getCurrentTime();

        do
        {
            auto uri = mdbcom::MDBURIHelper { request.getURI() }; /* pgr0745 */
            auto uriPath = std::string { }; /* pgr0745 */
            uriPath = uri.getPath();
            long long id = - 1;
            id = atoll(uriPath.c_str() + uriPath.find("/app/tasks/") + strlen("/app/tasks/"));
            if (id < 1)
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            else
            {
                const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
                auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                params.emplace_back(id);

                auto selectStm = dbConnection.execute(SELECT_TASK_TYPE_QUERY, params); /*pgr0745*/
                auto retExec = selectStm->getResult();
                int taskType;

                if (mdbcom::Result::OK != retExec) {
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
                else if (selectStm->getNumRows() == 0)
                {
                    errKeys.emplace_back("40400");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                    break;  
                }
                
                const auto &jsonstring = mdbcom::MDBHTTPServerUtility::extractJSONFromHTTPRequest(request);

                //JSON フォーマット定義の解析
                //Check request data
                std::string validateErr;
                mdbcom::MDBContentParser::KeyMapType jsonMap;
                mdb_apl_log_debug("Input parameter check %s", jsonstring.c_str());
                retCode = validateJSONFormat(jsonstring, jsonMap);
                if (retCode != mdbcom::Result::OK)
                {
                    mdb_sys_log_out("F110310");
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
                selectStm->getValue(0, selectStm->getColumnIndex("type"), taskType);
                if (taskType == TaskType::POND) {
                    operatePondDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else if (taskType == TaskType::FEEDING) {
                    operateFeedingDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else if (taskType == TaskType::RECOVERY) {
                    operateRecoveryDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else if (taskType == TaskType::DISPOSAL) {
                    operateDisposalDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else if (taskType == TaskType::SHIPPING) {
                    operateShippingDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else if (taskType == TaskType::WATER) {
                    operateWaterDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else if (taskType == TaskType::MAINTENANCE) {
                    operateMaintenanceDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else if (taskType == TaskType::MONITOR) {
                    operateMonitorDataInDB(RequestType::TASK_PUT, errKeys, id, jsonMap);
                } else {
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
                httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
            }
        }
        while (0);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send();
        double endTime = famscommon::getCurrentTime();
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
    }

    void FAMSDUTaskHandler::handleGetTaskRequest(
            Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response) {
        mdb_apl_log_start();
        // Declare const
        const int maxSizeOfProcessTime = 25;
        const int sizeIncludedSecond = 22;
        const int sizeNotIncludedSecond = 19;
        
        // Declare variable
        auto retCode = mdbcom::Result::OK;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto resData = std::string {};
        double startTime = famscommon::getCurrentTime();

        do
        {
            auto uri = mdbcom::MDBURIHelper { request.getURI() }; /* pgr0745 */
            auto uriPath = std::string { }; /* pgr0745 */
            uriPath = uri.getPath();
            long long id = - 1;
            id = atoll(uriPath.c_str() + uriPath.find("/app/tasks/") + strlen("/app/tasks/"));
            if (id < 1)
            {
                mdb_sys_log_out("F110310");
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            else
            {
                const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
                auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                params.emplace_back(id);
                auto selectStm = dbConnection.execute(SELECT_TASK_TYPE_QUERY, params); /*pgr0745*/
                auto retExec = selectStm->getResult();
                int taskType;
                if (mdbcom::Result::OK != retExec) {
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
                else if (selectStm->getNumRows() == 0)
                {
                    errKeys.emplace_back("40400");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                    break;
                }

                selectStm->getValue(0, selectStm->getColumnIndex("type"), taskType);
                if (taskType == 1) {
                    selectStm = dbConnection.execute(SELECT_TASK_POND_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskPondInputData pondInputData;
                    if (mdbcom::Result::OK == retExec) {
                        if (selectStm->getNumRows() == 0)
                        {
                            errKeys.emplace_back("40400");
                            httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                            break;
                        }
                        std::string returnedProcessTime;
                        long long returnedLotInfoId;
                        long long returnedCisternInfoId;
                        long long returnedUserId;
                        long long returnedOrganismInfoId;
                        int returnedOrganismNumber;
                        std::string returnedOrganismTag;
                        std::string returnedOrganismUnit;
                        long long returnedOrganismInitSize;
                        std::string returnedShippableTime;
                        std::string returnedMemo;
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("lot_information_id"), returnedLotInfoId);
                        selectStm->getValue(0, selectStm->getColumnIndex("cistern_information_id"), returnedCisternInfoId);
                        selectStm->getValue(0, selectStm->getColumnIndex("organism_information_id"), returnedOrganismInfoId);
                        selectStm->getValue(0, selectStm->getColumnIndex("organism_number"), returnedOrganismNumber);
                        selectStm->getValue(0, selectStm->getColumnIndex("organism_tag"), returnedOrganismTag);
                        selectStm->getValue(0, selectStm->getColumnIndex("organism_unit"), returnedOrganismUnit);
                        selectStm->getValue(0, selectStm->getColumnIndex("organism_initial_size"), returnedOrganismInitSize);
                        selectStm->getValue(0, selectStm->getColumnIndex("shippable_time"), returnedShippableTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        pondInputData.setProcessTime(returnedProcessTime);
                        pondInputData.setLotInformationId(returnedLotInfoId);
                        pondInputData.setCisternInformationId(returnedCisternInfoId);
                        pondInputData.setPICId(returnedUserId);
                        pondInputData.setOrganismInformationId(returnedOrganismInfoId);
                        pondInputData.setOrganismNumber(returnedOrganismNumber);
                        pondInputData.setOrganismTag(returnedOrganismTag);
                        pondInputData.setOrganismUnit(returnedOrganismUnit);
                        pondInputData.setOrganismInitialSize(returnedOrganismInitSize);
                        pondInputData.setShippableTime(returnedShippableTime);
                        pondInputData.setNote(returnedMemo);
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetPondData(errKeys, pondInputData);
                } else if (taskType == 2) {
                    selectStm = dbConnection.execute(SELECT_TASK_FEEDING_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskFeedingInputData feedingInputData;
                    if (mdbcom::Result::OK == retExec) {
                        std::string returnedProcessTime;
                        long long returnedUserId;
                        std::string returnedFeedingName;
                        std::string returnedMemo;
                        long long resourceId;
                        selectStm->getValue(0, selectStm->getColumnIndex("id"), resourceId);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("feeding_name"), returnedFeedingName);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        feedingInputData.setProcessTime(returnedProcessTime);
                        feedingInputData.setPICId(returnedUserId);
                        feedingInputData.setFeedingName(returnedFeedingName);
                        feedingInputData.setNote(returnedMemo);
                        auto detailParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                        detailParams.emplace_back(2);
                        detailParams.emplace_back(resourceId);
                        selectStm = dbConnection.execute(SELECT_TASK_INPUT_DETAILS_QUERY, detailParams); /*pgr0745*/
                        retExec = selectStm->getResult();
                        if (mdbcom::Result::OK == retExec) {
                            for (int i = 0; i < selectStm->getNumRows(); i++) {
                                long long lotId;
                                std::string amount;
                                selectStm->getValue(i, selectStm->getColumnIndex("lot_information_id"), lotId);
                                selectStm->getValue(i, selectStm->getColumnIndex("amount"), amount);
                                feedingInputData.setDetail(lotId, amount);
                            }
                        }
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetFeedingData(errKeys, feedingInputData);
                } else if (taskType == 3) {
                    selectStm = dbConnection.execute(SELECT_TASK_RECOVERY_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskRecoveryInputData recoveryInputData;
                    if (mdbcom::Result::OK == retExec) {
                        std::string returnedProcessTime;
                        long long returnedUserId;
                        std::string returnedMemo;
                        long long resourceId;
                        selectStm->getValue(0, selectStm->getColumnIndex("id"), resourceId);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        recoveryInputData.setProcessTime(returnedProcessTime);
                        recoveryInputData.setPICId(returnedUserId);
                        recoveryInputData.setNote(returnedMemo);
                        auto detailParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                        detailParams.emplace_back(3);
                        detailParams.emplace_back(resourceId);
                        selectStm = dbConnection.execute(SELECT_TASK_INPUT_DETAILS_QUERY, detailParams); /*pgr0745*/
                        retExec = selectStm->getResult();
                        if (mdbcom::Result::OK == retExec) {
                            for (int i = 0; i < selectStm->getNumRows(); i++) {
                                long long lotId;
                                double amount;
                                selectStm->getValue(i, selectStm->getColumnIndex("lot_information_id"), lotId);
                                selectStm->getValue(i, selectStm->getColumnIndex("amount"), amount);
                                recoveryInputData.setDetail(lotId, amount);
                            }
                        }
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetRecoveryData(errKeys, recoveryInputData);
                } else if (taskType == 4) {
                    selectStm = dbConnection.execute(SELECT_TASK_DISPOSAL_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskDisposalInputData disposalInputData;
                    if (mdbcom::Result::OK == retExec) {
                        std::string returnedProcessTime;
                        long long returnedUserId;
                        int returnedDisposalType;
                        std::string returnedMemo;
                        long long resourceId;
                        selectStm->getValue(0, selectStm->getColumnIndex("id"), resourceId);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("disposalType"), returnedDisposalType);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        disposalInputData.setProcessTime(returnedProcessTime);
                        disposalInputData.setPICId(returnedUserId);
                        disposalInputData.setDisposalType(returnedDisposalType);
                        disposalInputData.setNote(returnedMemo);
                        auto detailParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                        detailParams.emplace_back(4);
                        detailParams.emplace_back(resourceId);
                        selectStm = dbConnection.execute(SELECT_TASK_INPUT_DETAILS_QUERY, detailParams); /*pgr0745*/
                        retExec = selectStm->getResult();
                        if (mdbcom::Result::OK == retExec) {
                            for (int i = 0; i < selectStm->getNumRows(); i++) {
                                long long lotId;
                                double amount;
                                selectStm->getValue(i, selectStm->getColumnIndex("lot_information_id"), lotId);
                                selectStm->getValue(i, selectStm->getColumnIndex("amount"), amount);
                                disposalInputData.setDetail(lotId, amount);
                            }
                        }
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetDisposalData(errKeys, disposalInputData);
                } else if (taskType == 5) {
                    selectStm = dbConnection.execute(SELECT_TASK_SHIPPING_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskShippingInputData shippingInputData;
                    if (mdbcom::Result::OK == retExec) {
                        std::string returnedProcessTime;
                        long long returnedUserId;
                        std::string returnedDestination;
                        std::string returnedMemo;
                        long long resourceId;
                        selectStm->getValue(0, selectStm->getColumnIndex("id"), resourceId);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("shipping_destination"), returnedDestination);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        shippingInputData.setProcessTime(returnedProcessTime);
                        shippingInputData.setPICId(returnedUserId);
                        shippingInputData.setDestination(returnedDestination);
                        shippingInputData.setNote(returnedMemo);
                        auto detailParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                        detailParams.emplace_back(5);
                        detailParams.emplace_back(resourceId);
                        selectStm = dbConnection.execute(SELECT_TASK_INPUT_DETAILS_QUERY, detailParams); /*pgr0745*/
                        retExec = selectStm->getResult();
                        if (mdbcom::Result::OK == retExec) {
                            for (int i = 0; i < selectStm->getNumRows(); i++) {
                                long long lotId;
                                double amount;
                                selectStm->getValue(i, selectStm->getColumnIndex("lot_information_id"), lotId);
                                selectStm->getValue(i, selectStm->getColumnIndex("amount"), amount);
                                shippingInputData.setDetail(lotId, amount);
                            }
                        }
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetShippingData(errKeys, shippingInputData);
                } else if (taskType == 6) {
                    selectStm = dbConnection.execute(SELECT_TASK_WATER_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskWaterInputData waterInputData;
                    if (mdbcom::Result::OK == retExec) {
                        std::string returnedProcessTime;
                        long long returnedUserId;
                        long long returnedCisternInfoId;
                        int returnedOperation;
                        std::string returnedAmount;
                        std::string returnedMemo;
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("cistern_information_id"), returnedCisternInfoId);
                        selectStm->getValue(0, selectStm->getColumnIndex("operation"), returnedOperation);
                        selectStm->getValue(0, selectStm->getColumnIndex("amount"), returnedAmount);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        waterInputData.setProcessTime(returnedProcessTime);
                        waterInputData.setPICId(returnedUserId);
                        waterInputData.setCisternInformationId(returnedCisternInfoId);
                        waterInputData.setOperation(returnedOperation);
                        waterInputData.setAmount(returnedAmount);
                        waterInputData.setNote(returnedMemo);

                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetWaterData(errKeys, waterInputData);
                } else if (taskType == 7) {
                    selectStm = dbConnection.execute(SELECT_TASK_MAINTENANCE_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskMaintenanceInputData maintenanceInputData;
                    if (mdbcom::Result::OK == retExec) {
                        std::string returnedProcessTime;
                        long long returnedUserId;
                        long long returnedCisternInfoId;
                        int returnedMaintenanceType;
                        std::string returnedMemo;
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("cistern_information_id"), returnedCisternInfoId);
                        selectStm->getValue(0, selectStm->getColumnIndex("maintenance_type"), returnedMaintenanceType);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        maintenanceInputData.setProcessTime(returnedProcessTime);
                        maintenanceInputData.setPICId(returnedUserId);
                        maintenanceInputData.setCisternInformationId(returnedCisternInfoId);
                        maintenanceInputData.setMaintenanceType(returnedMaintenanceType);
                        maintenanceInputData.setNote(returnedMemo);
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetMaintenanceData(errKeys, maintenanceInputData);
                } else if (taskType == 8) {
                    selectStm = dbConnection.execute(SELECT_TASK_WATER_MONITOR_INPUT_QUERY, params); /*pgr0745*/
                    retExec = selectStm->getResult();
                    FAMSDUTaskMonitorInputData monitorInputData;
                    if (mdbcom::Result::OK == retExec) {
                        std::string returnedProcessTime, processTimeConverted;
                        long long returnedUserId;
                        long long returnedCisternInfoId;
                        int returnedMonitorType;
                        std::string returnedMonitorValue;
                        std::string returnedMemo;
                        selectStm->getValue(0, selectStm->getColumnIndex("process_time"), returnedProcessTime);
                        selectStm->getValue(0, selectStm->getColumnIndex("process_user_id"), returnedUserId);
                        selectStm->getValue(0, selectStm->getColumnIndex("cistern_information_id"), returnedCisternInfoId);
                        selectStm->getValue(0, selectStm->getColumnIndex("monitor_type"), returnedMonitorType);
                        selectStm->getValue(0, selectStm->getColumnIndex("monitor_value"), returnedMonitorValue);
                        selectStm->getValue(0, selectStm->getColumnIndex("memo"), returnedMemo);
                        //convert time
                        auto convert1 = famscommon::convertProcessTime(returnedProcessTime, processTimeConverted);
                        
                        mdb_apl_log_debug("=========== returnedProcessTime: %s, processTimeConverted: %s", returnedProcessTime.c_str(), processTimeConverted.c_str());
                        monitorInputData.setProcessTime(processTimeConverted);
                        monitorInputData.setPICId(returnedUserId);
                        monitorInputData.setCisternInformationId(returnedCisternInfoId);
                        monitorInputData.setMonitorType(returnedMonitorType);
                        monitorInputData.setMonitorValue(returnedMonitorValue);
                        monitorInputData.setNote(returnedMemo);
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
                    }
                    resData = buildGetMonitorData(errKeys, monitorInputData);
                }
            }
        }
        while (0);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send();
        ostr << resData;
        double endTime = famscommon::getCurrentTime();
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
    }

    void FAMSDUTaskHandler::handleGetTasksRequest(
            Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response) {
        mdb_apl_log_start();
        // Declare const
        const int maxSizeOfProcessTime = 25;
        const int sizeIncludedSecond = 22;
        const int sizeNotIncludedSecond = 19;

        // Declare variable
        auto retCode =mdbcom::Result::OK;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;

        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(SELECT_ALL_TASKS_QUERY); /*pgr0745*/
        auto retExec = selectStm->getResult();
        int result = mdbcom::Result::NG;
        auto taskStore = std::vector<FAMSDUTaskSummaryData> {};

        if (mdbcom::Result::OK == retExec) {
            mdb_apl_log_debug("number of task: %d", selectStm->getNumRows());

            long long taskId;
            int type;
            std::string processTime, processTimeConverted;
            long long picUserId;
            std::string summary;
            std::string name;
            int size;

            auto selectShippingStm = dbConnection.execute(SELECT_TASK_SHIPPING_QUERY); /*pgr0745*/
            auto retShippingExec = selectShippingStm->getResult();
            std::map<long long, std::string> shippingDestination;
            if (mdbcom::Result::OK == retShippingExec) {
                for (int i = 0; i < selectShippingStm->getNumRows(); i++) {
                    std::string destination;
                    long long shippingId;
                    selectShippingStm->getValue(i, selectShippingStm->getColumnIndex("shipping_destination"), destination);
                    selectShippingStm->getValue(i, selectShippingStm->getColumnIndex("id"), shippingId);
                    shippingDestination.insert ( std::pair<long long,std::string>(shippingId, destination) );
                }
            }
            for (int i = 0; i < selectStm->getNumRows(); i++) {
                FAMSDUTaskSummaryData taskModel;
                mdb_apl_log_debug("==============Task taskId=%d", taskId);
                mdb_apl_log_debug("==============Task Type=%d", type);

                selectStm->getValue(i, selectStm->getColumnIndex("id"), taskId);
                selectStm->getValue(i, selectStm->getColumnIndex("type"), type);
                selectStm->getValue(i, selectStm->getColumnIndex("process_time"), processTime);
                selectStm->getValue(i, selectStm->getColumnIndex("process_user_id"), picUserId);
                selectStm->getValue(i, selectStm->getColumnIndex("name"), name);
                selectStm->getValue(i, selectStm->getColumnIndex("summary"), summary);

                //convert time
                auto convert1 = famscommon::convertProcessTime(processTime, processTimeConverted);
               
                mdb_apl_log_debug("=========== processTime: %s, processTimeConverted: %s", processTime.c_str(), processTimeConverted.c_str());
                taskModel.setId(taskId);
                taskModel.setType(type);
                if (type == 8)
                {
                    processTime = processTimeConverted;
                }
                taskModel.setProcessTime(processTime);
                taskModel.setPicUserId(picUserId);
                taskModel.setSummary(summary);
                if ( type == TaskType::SHIPPING ) {
                    taskModel.setSummary(shippingDestination.find(taskId)->second);
                }
                taskStore.emplace_back(taskModel);
            }
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;
        }
        else {
            mdb_apl_log_debug("failed to get task info");
        }

        mdb_apl_log_debug("number task model: %d", taskStore.size());

        //build response JSON string
        auto resData = std::string {};

        resData = buildGetTasksResponseData(retCode, errKeys, taskStore);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
        mdb_apl_log_end();
    }

    void FAMSDUTaskHandler::handleDeleteTaskRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        // Declare variable
        auto retCode = mdbcom::Result::NG;;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<std::string> errKeys;
        auto uri = request.getURI();
        auto i = uri.find_last_of(URI_TASKS_PATH + "\/");
        long long taskId = atoll(uri.substr(i + 1).c_str());

        if (taskId > 0) {
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(taskId);

            auto deletePondStm = dbConnection.execute(DELETE_TASK_POND_QUERY, params); /*pgr0745*/
            auto retExec = deletePondStm->getResult();
            if (mdbcom::Result::OK == retExec && deletePondStm->getNumRows() == 1) {
                httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;

                params.clear();
                long long lotId;
                deletePondStm->getValue(0, deletePondStm->getColumnIndex("lot_information_id"), lotId);
                params.emplace_back(lotId);
                if (mdbcom::Result::OK == deletePondStm->getResult()) {
                    auto deleteLotStm = dbConnection.execute(DELETE_LOT_QUERY, params); /*pgr0745*/
                }

            }
            else if (mdbcom::Result::OK != retExec){
                errKeys.emplace_back("40400");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                mdb_apl_log_debug("failed to delete task id: %ll", taskId);
            }
            params.clear();
            params.emplace_back(taskId);
			auto deleteTaskStm = dbConnection.execute(DELETE_TASK_QUERY, params); /*pgr0745*/
			retCode = deleteTaskStm->getResult();
			if (mdbcom::Result::OK == retCode)
			{
				retCode = mdbcom::Result::OK;
			}
			else
			{
				errKeys.emplace_back("40400");
				httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
				mdb_apl_log_debug("failed to delete task id: %ll", taskId);
			}
        } else {
            mdb_apl_log_error("Can't parse the input taskId");
            httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
        }

        //build response JSON string
        auto resData = std::string {};

        resData = buildDeleteTasksResponseData(retCode, errKeys);
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
    int FAMSDUTaskHandler::validateJSONFormat(const std::string &jsonString, mdbcom::MDBContentParser::KeyMapType &jsonMap)
    {
        mdb_apl_log_start();

        auto ret = mdbcom::Result::OK;
        mdbcom::MDBJsonParser jsonDecoder {};
        std::string errKey;

        // decode JSON string to object
        auto jsonRet = jsonDecoder.decodeKeyMapData(jsonString, mJsonTemplate, "", jsonMap, errKey); /*pgr0745*/

        if (mdbcom::Json::OK != jsonRet || false == errKey.empty())
        {
            auto errKeyString = errKey.c_str();
            mdb_apl_log_critical("JSON format is abnormal, errorKey = %s", errKeyString);
            mdb_apl_log_end();
            return JSON_NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    std::string FAMSDUTaskHandler::buildGetPondData(std::vector<std::string> &errKeys, FAMSDUTaskPondInputData &pondInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate = "";
            famscommon::convertICTToUTC(pondInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, pondInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, LOT_INFORMATION_ID, pondInputData.getLotInformationId());
            jsonEncoder.setKeyMapDataValue(inputTag, CISTERN_INFORMATION_ID, pondInputData.getCisternInformationId());
            jsonEncoder.setKeyMapDataValue(inputTag, ORGANISM_INFORMATION_ID, pondInputData.getOrganismInformationId());
            jsonEncoder.setKeyMapDataValue(inputTag, ORGANISM_NUMBER, pondInputData.getOrganismNumber());
            jsonEncoder.setKeyMapDataValue(inputTag, ORGANISM_TAG, pondInputData.getOrganismTag());
            jsonEncoder.setKeyMapDataValue(inputTag, ORGANISM_UNIT, pondInputData.getOrganismUnit());
            jsonEncoder.setKeyMapDataValue(inputTag, ORGANISM_INITIAL_SIZE, pondInputData.getOrganismInitialSize());
            famscommon::convertICTToUTC(pondInputData.getShippableTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, SHIPPABLE_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, pondInputData.getNote());
            auto pondInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(pondInputTag, POND_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, pondInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetFeedingData(std::vector<std::string> &errKeys, FAMSDUTaskFeedingInputData &feedingInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate;
            famscommon::convertICTToUTC(feedingInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, feedingInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, FEEDING_NAME, feedingInputData.getFeedingName());
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, feedingInputData.getNote());
            auto detailsTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for (auto const &it : feedingInputData.getDetails())
            {
                auto detailTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(detailTag, LOT_INFORMATION_ID, it.first);
                jsonEncoder.setKeyMapDataValue(detailTag, AMOUNT, it.second);
                detailsTag.emplace_back(detailTag);
            }
            jsonEncoder.setKeyMapDataArrayValue(inputTag, DETAILS, detailsTag);
            auto feedingInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(feedingInputTag, FEEDING_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, feedingInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetRecoveryData(std::vector<std::string> &errKeys, FAMSDUTaskRecoveryInputData &recoveryInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate;
            famscommon::convertICTToUTC(recoveryInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, recoveryInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, recoveryInputData.getNote());
            auto detailsTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for (auto const &it : recoveryInputData.getDetails())
            {
                auto detailTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(detailTag, LOT_INFORMATION_ID, it.first);
                jsonEncoder.setKeyMapDataValue(detailTag, AMOUNT, it.second);
                detailsTag.emplace_back(detailTag);
            }
            jsonEncoder.setKeyMapDataArrayValue(inputTag, DETAILS, detailsTag);
            auto recoveryInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(recoveryInputTag, RECOVERY_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, recoveryInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetDisposalData(std::vector<std::string> &errKeys, FAMSDUTaskDisposalInputData &disposalInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate;
            famscommon::convertICTToUTC(disposalInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, disposalInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, DISPOSAL_TYPE, disposalInputData.getDisposalType());
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, disposalInputData.getNote());
            auto detailsTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for (auto const &it : disposalInputData.getDetails())
            {
                auto detailTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(detailTag, LOT_INFORMATION_ID, it.first);
                jsonEncoder.setKeyMapDataValue(detailTag, AMOUNT, it.second);
                detailsTag.emplace_back(detailTag);
            }
            jsonEncoder.setKeyMapDataArrayValue(inputTag, DETAILS, detailsTag);
            auto disposalInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(disposalInputTag, DISPOSAL_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, disposalInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetShippingData(std::vector<std::string> &errKeys, FAMSDUTaskShippingInputData &shippingInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate;
            famscommon::convertICTToUTC(shippingInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, shippingInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, SHIPPING_DESTINATION_INPUT, shippingInputData.getDestination());
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, shippingInputData.getNote());
            auto detailsTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for (auto const &it : shippingInputData.getDetails())
            {
                auto detailTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(detailTag, LOT_INFORMATION_ID, it.first);
                jsonEncoder.setKeyMapDataValue(detailTag, AMOUNT, it.second);
                detailsTag.emplace_back(detailTag);
            }
            jsonEncoder.setKeyMapDataArrayValue(inputTag, DETAILS, detailsTag);
            auto shippingInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(shippingInputTag, SHIPPING_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, shippingInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetWaterData(std::vector<std::string> &errKeys, FAMSDUTaskWaterInputData &waterInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate;
            famscommon::convertICTToUTC(waterInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, waterInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, CISTERN_INFORMATION_ID, waterInputData.getCisternInformationId());
            jsonEncoder.setKeyMapDataValue(inputTag, OPERATION, waterInputData.getOperation());
            jsonEncoder.setKeyMapDataValue(inputTag, AMOUNT, waterInputData.getAmount());
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, waterInputData.getNote());
            auto waterInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(waterInputTag, WATER_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, waterInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetMaintenanceData(std::vector<std::string> &errKeys, FAMSDUTaskMaintenanceInputData &maintenanceInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate;
            famscommon::convertICTToUTC(maintenanceInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, maintenanceInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, CISTERN_INFORMATION_ID, maintenanceInputData.getCisternInformationId());
            jsonEncoder.setKeyMapDataValue(inputTag, MAINTENANCE_TYPE, maintenanceInputData.getMaintenanceType());
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, maintenanceInputData.getNote());
            auto maintenanceInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(maintenanceInputTag, MAINTENANCE_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, maintenanceInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetMonitorData(std::vector<std::string> &errKeys, FAMSDUTaskMonitorInputData &monitorInputData) {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (0 == errKeys.size())
        {
            auto inputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            std::string utcDate;
            famscommon::convertICTToUTC(monitorInputData.getProcessTime(), utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_TIME, utcDate);
            jsonEncoder.setKeyMapDataValue(inputTag, PROCESS_USER_ID, monitorInputData.getPICId());
            jsonEncoder.setKeyMapDataValue(inputTag, CISTERN_INFORMATION_ID, monitorInputData.getCisternInformationId());
            jsonEncoder.setKeyMapDataValue(inputTag, MONITOR_TYPE, monitorInputData.getMonitorType());
            jsonEncoder.setKeyMapDataValue(inputTag, MONITOR_VALUE, monitorInputData.getMonitorValue());
            jsonEncoder.setKeyMapDataValue(inputTag, MEMO, monitorInputData.getNote());
            auto monitorInputTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            jsonEncoder.setKeyMapDataValue(monitorInputTag, WATER_MONITOR_INPUT, inputTag);
            jsonEncoder.setKeyMapDataValue(jsonMap, DATA, monitorInputTag);

        } else {
            auto errorsTag = buildErrorMessages(errKeys);
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
    std::string FAMSDUTaskHandler::buildGetTasksResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUTaskSummaryData> &taskStore)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (mdbcom::Result::OK == retCode)
        {
            auto tasksTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            for(auto taskData: taskStore) {
                auto taskTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(taskTag, ID, taskData.getId());
                jsonEncoder.setKeyMapDataValue(taskTag, "type", (long long)taskData.getType());
                std::string utcDate;
                auto ret = famscommon::convertICTToUTC(taskData.getProcessTime(), utcDate);
                if (ret == mdbcom::Result::OK) {
                    jsonEncoder.setKeyMapDataValue(taskTag, "processTime", utcDate);
                } else {
                    jsonEncoder.setKeyMapDataValue(taskTag, "processTime", taskData.getProcessTime());
                }
                jsonEncoder.setKeyMapDataValue(taskTag, "picUserId", taskData.getPicUserId());
                jsonEncoder.setKeyMapDataArrayValue(taskTag, "summary", taskData.getSummary());
                tasksTag.emplace_back(taskTag);
            }

            if (0 != tasksTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, DATA, tasksTag);
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

    /*!
     * @brief Build response data
     *
     * @param [in] retCode the return code
     * @param [in] errorCodes
     * @return the JSON response string
     */
    std::string FAMSDUTaskHandler::buildDeleteTasksResponseData(int &retCode,
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
    void FAMSDUTaskHandler::operatePondDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_debug("Handle for POND_INPUT");
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskPondInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;            
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestPondInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(mRequestPondInputData.getOrganismInformationId());
            
            std::string sqlStatement;
            sqlStatement = SELECT_ORGANISM_ITEM_QUERY;

            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            auto retExec = selectStm->getResult();
            std::string organismName;
            if (mdbcom::Result::OK == retExec) {
                if (selectStm->getNumRows() == 0) {
                        errKeys.emplace_back("40400");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                        break;
                    }    
                selectStm->getValue(0, selectStm->getColumnIndex("name"), organismName);
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }   
            
            params.clear();
            params.emplace_back(organismName);
            params.emplace_back(mRequestPondInputData.getNote());
            params.emplace_back(mRequestPondInputData.getOrganismTag());
            params.emplace_back(mRequestPondInputData.getOrganismUnit());
            params.emplace_back(mRequestPondInputData.getCisternInformationId());

            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_INSERT_LOT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_INSERT_LOT_QUERY;
            }

            selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            retExec = selectStm->getResult();
            int newLotId;
            if (mdbcom::Result::OK == retExec) {
                if (selectStm->getNumRows() == 0) {
                        errKeys.emplace_back("40400");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                        break;
                    }    
                selectStm->getValue(0, selectStm->getColumnIndex("id"), newLotId);
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }

            //Insert into lot_organism_details;
            params.clear();
            params.emplace_back(mRequestPondInputData.getOrganismInformationId());
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_LOT_ORGANISM_DETAIL_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                params.emplace_back(newLotId);
                sqlStatement = POST_LOT_ORGANISM_DETAIL_QUERY;
            }

            selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec) {
                if (selectStm->getNumRows() == 0) {
                        errKeys.emplace_back("40400");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                        break;
                    }    
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }

            //Insert Update task_pond_inputs         
            params.clear();
            params.emplace_back(mRequestPondInputData.getProcessTime());
            params.emplace_back(mRequestPondInputData.getPICId());
            params.emplace_back(newLotId);
            params.emplace_back(mRequestPondInputData.getCisternInformationId());
            params.emplace_back(mRequestPondInputData.getOrganismInformationId());
            params.emplace_back(mRequestPondInputData.getOrganismNumber());
            params.emplace_back(mRequestPondInputData.getOrganismTag());
            params.emplace_back(mRequestPondInputData.getOrganismUnit());
            params.emplace_back(mRequestPondInputData.getOrganismInitialSize());
            params.emplace_back(mRequestPondInputData.getShippableTime());
            params.emplace_back(mRequestPondInputData.getNote());
            
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_TASK_POND_INPUT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_POND_INPUT_QUERY;
            }
            
            selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec) {
                if (requestType == RequestType::TASK_PUT) {
                    if (selectStm->getNumRows() == 0) {
                        errKeys.emplace_back("40400");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                }
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
        } while (0);
    }
    void FAMSDUTaskHandler::operateFeedingDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_debug("Handle for FEEDING_INPUT");
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskFeedingInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestFeedingInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(mRequestFeedingInputData.getProcessTime());
            params.emplace_back(mRequestFeedingInputData.getPICId());
            params.emplace_back(mRequestFeedingInputData.getFeedingName());
            params.emplace_back(mRequestFeedingInputData.getNote());
            std::string sqlStatement;
            long long subTaskId;
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_TASK_FEEDING_INPUT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_FEEDING_INPUT_QUERY;
            }
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            params.clear();
            auto retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec)
            {
                if (requestType == RequestType::TASK_PUT) {
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                    params.emplace_back(2);
                    params.emplace_back(subTaskId);
                    selectStm = dbConnection.execute(DELETE_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                    params.clear();
                    retExec = selectStm->getResult();
                    if (mdbcom::Result::OK != retExec) {
                        errKeys.emplace_back("40000");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                }
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            std::vector<std::string> amountList = mRequestFeedingInputData.getAmountList();
            std::vector<long long> lotList = mRequestFeedingInputData.getLotList();
            
            for (int index = 0; index < lotList.size(); index++)
            {
                params.emplace_back(2);
                params.emplace_back(subTaskId);
                params.emplace_back(lotList.at(index));
                params.emplace_back(amountList.at(index));
                auto insertStm = dbConnection.execute(POST_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                params.clear();
                auto retInsertExec = insertStm->getResult();
                if (mdbcom::Result::OK == retInsertExec) {
                    mdb_apl_log_debug("SQL insert successfully!");
                } else {
                    mdb_apl_log_critical("SQL insert failed!!!");
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
            }
        } while (0);
    }
    void FAMSDUTaskHandler::operateRecoveryDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_start();
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskRecoveryInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestRecoveryInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(mRequestRecoveryInputData.getProcessTime());
            params.emplace_back(mRequestRecoveryInputData.getPICId());
            params.emplace_back(mRequestRecoveryInputData.getNote());
            std::string sqlStatement;
            long long subTaskId;
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_TASK_RECOVERY_INPUT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_RECOVERY_INPUT_QUERY;
            }
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            params.clear();
            auto retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec)
            {
                if (requestType == RequestType::TASK_PUT) {
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                    params.emplace_back(3);
                    params.emplace_back(subTaskId);
                    selectStm = dbConnection.execute(DELETE_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                    params.clear();
                    retExec = selectStm->getResult();
                    if (mdbcom::Result::OK != retExec) {
                        errKeys.emplace_back("40000");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                    std::vector<std::string> amountList = mRequestRecoveryInputData.getAmountList();
                    std::vector<long long> lotList = mRequestRecoveryInputData.getLotList();
                    for (int index = 0; index < lotList.size(); index++)
                    {
                        params.emplace_back(3);
                        params.emplace_back(subTaskId);
                        params.emplace_back(lotList.at(index));
                        params.emplace_back(amountList.at(index));
                        auto insertStm = dbConnection.execute(POST_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                        params.clear();
                        auto retInsertExec = insertStm->getResult();
                        if (mdbcom::Result::OK == retInsertExec) {
                            mdb_apl_log_debug("SQL insert successfully!");
                        } else {
                            mdb_apl_log_critical("SQL insert failed!!!");
                            errKeys.emplace_back("40000");
                            httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                            break;
                        }
                    }
                }
            } else {
                errKeys.emplace_back("40000");
                break;
            }
        } while (0);

        mdb_apl_log_end();
    }
    void FAMSDUTaskHandler::operateDisposalDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_debug("Handle for DISPOSAL_INPUT");
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskDisposalInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestDisposalInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(mRequestDisposalInputData.getProcessTime());
            params.emplace_back(mRequestDisposalInputData.getPICId());
            params.emplace_back(mRequestDisposalInputData.getDisposalType());
            params.emplace_back(mRequestDisposalInputData.getNote());
            std::string sqlStatement;
            long long subTaskId;
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_TASK_DISPOSAL_INPUT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_DISPOSAL_INPUT_QUERY;
            }
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            params.clear();
            auto retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec)
            {
                if (requestType == RequestType::TASK_PUT) {
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                    params.emplace_back(4);
                    params.emplace_back(subTaskId);
                    selectStm = dbConnection.execute(DELETE_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                    params.clear();
                    retExec = selectStm->getResult();
                    if (mdbcom::Result::OK != retExec) {
                        errKeys.emplace_back("40000");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                }
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            std::vector<std::string> amountList = mRequestDisposalInputData.getAmountList();
            std::vector<long long> lotList = mRequestDisposalInputData.getLotList();
            
            for (int index = 0; index < lotList.size(); index++)
            {
                params.emplace_back(4);
                params.emplace_back(subTaskId);
                params.emplace_back(lotList.at(index));
                params.emplace_back(amountList.at(index));
                auto insertStm = dbConnection.execute(POST_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                params.clear();
                auto retInsertExec = insertStm->getResult();
                if (mdbcom::Result::OK == retInsertExec) {
                    mdb_apl_log_debug("SQL insert successfully!");
                } else {
                    mdb_apl_log_critical("SQL insert failed!!!");
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
            }
        } while (0);

    }
    void FAMSDUTaskHandler::operateShippingDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_debug("Handle for SHIPPING_INPUT");
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskShippingInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestShippingInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(mRequestShippingInputData.getProcessTime());
            params.emplace_back(mRequestShippingInputData.getPICId());
            params.emplace_back(mRequestShippingInputData.getDestination());
            params.emplace_back(mRequestShippingInputData.getNote());
            std::string sqlStatement;
            long long subTaskId;
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_TASK_SHIPPING_INPUT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_SHIPPING_INPUT_QUERY;
            }
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            params.clear();
            auto retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec)
            {
                if (requestType == RequestType::TASK_PUT) {
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                    params.emplace_back(5);
                    params.emplace_back(subTaskId);
                    selectStm = dbConnection.execute(DELETE_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                    params.clear();
                    retExec = selectStm->getResult();
                    if (mdbcom::Result::OK != retExec) {
                        errKeys.emplace_back("40000");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), subTaskId);
                }
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            std::vector<std::string> amountList = mRequestShippingInputData.getAmountList();
            std::vector<long long> lotList = mRequestShippingInputData.getLotList();
            
            for (int index = 0; index < lotList.size(); index++)
            {
                params.emplace_back(5);
                params.emplace_back(subTaskId);
                params.emplace_back(lotList.at(index));
                params.emplace_back(amountList.at(index));
                auto insertStm = dbConnection.execute(POST_TASK_INPUT_DETAILS_QUERY, params); /*pgr0745*/
                params.clear();
                auto retInsertExec = insertStm->getResult();
                if (mdbcom::Result::OK == retInsertExec) {
                    mdb_apl_log_debug("SQL insert successfully!");
                } else {
                    mdb_apl_log_critical("SQL insert failed!!!");
                    errKeys.emplace_back("40000");
                    httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                    break;
                }
            }
        } while (0);
    }
    void FAMSDUTaskHandler::operateWaterDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_debug("Handle for WATER_INPUT");
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskWaterInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestWaterInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(mRequestWaterInputData.getProcessTime());
            params.emplace_back(mRequestWaterInputData.getPICId());
            params.emplace_back(mRequestWaterInputData.getCisternInformationId());
            params.emplace_back(mRequestWaterInputData.getOperation());
            params.emplace_back(mRequestWaterInputData.getAmount());
            params.emplace_back(mRequestWaterInputData.getNote());
            std::string sqlStatement;
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_TASK_WATER_INPUT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_WATER_INPUT_QUERY;
            }
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            auto retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec) {
                if (requestType == RequestType::TASK_PUT) {
                    if (selectStm->getNumRows() == 0) {
                        errKeys.emplace_back("40400");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                }
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
        } while (0);
    }
    void FAMSDUTaskHandler::operateMaintenanceDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_debug("Handle for MAINTENANCE_INPUT");
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskMaintenanceInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestMaintenanceInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            params.emplace_back(mRequestMaintenanceInputData.getProcessTime());
            params.emplace_back(mRequestMaintenanceInputData.getPICId());
            params.emplace_back(mRequestMaintenanceInputData.getCisternInformationId());
            params.emplace_back(mRequestMaintenanceInputData.getMaintenanceType());
            params.emplace_back(mRequestMaintenanceInputData.getNote());
            std::string sqlStatement;
            if (requestType == RequestType::TASK_PUT) {
                params.emplace_back(taskId);
                sqlStatement = PUT_TASK_MAINTENANCE_INPUT_QUERY;
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_MAINTENANCE_INPUT_QUERY;
            }
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            auto retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec) {
                if (requestType == RequestType::TASK_PUT) {
                    if (selectStm->getNumRows() == 0) {
                        errKeys.emplace_back("40400");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                }
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
        } while (0);
    }
    void FAMSDUTaskHandler::operateMonitorDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap) {
        mdb_apl_log_start();
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        mdbcom::MDBJsonParser jsonDecoder {};
        auto inputAnalyzer = FAMSDURqTaskMonitorInputAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(requestType);

        do {
            std::string validateErr;
            if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestMonitorInputData, validateErr))
            {
                auto errKeyString = validateErr.c_str();
                mdb_apl_log_error("Analyze result fail at %s", errKeyString);
                errKeys.emplace_back("40000");                                                                                                                                                                                                                                                  
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
            auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
            std::string stime = mRequestMonitorInputData.getProcessTime();
            //convert time
            char convertTime[30];
            memset(convertTime, '\0', sizeof(convertTime));
            strncpy(convertTime , stime.c_str() ,23);
            strncat(convertTime, "+9:00", 6);

            auto picId = mRequestMonitorInputData.getPICId();
            long long cisternId = mRequestMonitorInputData.getCisternInformationId();
            int monitorType = mRequestMonitorInputData.getMonitorType();
            double monitorValue = std::stod(mRequestMonitorInputData.getMonitorValue());
            std::string note = mRequestMonitorInputData.getNote();
            std::vector<std::string> error_messages, error_ats;
            std::vector<long long> lot_information_ids;
            int statusOfThisRecord = OCCURRING_ERRMSG;
            int convertMonitorType = monitorType + 3;

            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();

            //check if lastest process_time
            //params.emplace_back(picId);
            params.emplace_back(cisternId);
            params.emplace_back(monitorType);
            params.emplace_back(convertTime);
            auto selectStm = dbConnection.execute(SELECT_PROCESS_TIME_TO_CHANGE_ERROR_STATUS, params); /*pgr0745*/
            auto retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec) {
                if (selectStm->getNumRows() > 0) {
                    //the inputed record is not the lastest, set default status of this record = 2
                    statusOfThisRecord = RESOLVED_ERRMSG;
                } else {
                    //update status = 2 for all lot which are no longer violated
                    auto numDelMsgRecordeds = changeStatusResolved(convertMonitorType);
                }
            }

            //check violation then import to error_message
            auto numViolations = checkSensorValue(convertTime, monitorValue, convertMonitorType, cisternId, statusOfThisRecord);

            //add task monitor
            params.clear();
            params.emplace_back(convertTime);
            params.emplace_back(picId);
            params.emplace_back(cisternId);
            params.emplace_back(monitorType);
            params.emplace_back(monitorValue);
            params.emplace_back(note);


            std::string sqlStatement;
            if (requestType == RequestType::TASK_PUT) {
                sqlStatement = PUT_TASK_WATER_MONITOR_INPUT_QUERY;
                params.emplace_back(taskId);
            } else if (requestType == RequestType::TASKS_POST)  {
                sqlStatement = POST_TASK_WATER_MONITOR_INPUT_QUERY;
            }

            selectStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            retExec = selectStm->getResult();
            if (mdbcom::Result::OK == retExec) {
                if (requestType == RequestType::TASK_PUT) {
                    if (selectStm->getNumRows() == 0) {
                        errKeys.emplace_back("40400");
                        httpRetCode = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                        break;
                    }
                } else if (requestType == RequestType::TASKS_POST)  {
                    selectStm->getValue(0, selectStm->getColumnIndex("task_information_id"), taskId);
                }
            } else {
                errKeys.emplace_back("40000");
                httpRetCode = Poco::Net::HTTPResponse::HTTP_BAD_REQUEST;
                break;
            }
        } while (0);
    }
}


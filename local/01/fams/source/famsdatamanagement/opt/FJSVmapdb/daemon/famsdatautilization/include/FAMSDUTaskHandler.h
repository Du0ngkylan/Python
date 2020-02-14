/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskHandler.h
 * @brief       Sensor search request handler file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_TASK_REQUEST_HANDLER_H_
#define FAMS_DATA_UTIL_TASK_REQUEST_HANDLER_H_
#include <string>
#include <future>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "MDBContentParser.h"
#include "MDBURIHelper.h"
#include "FAMSDUCommon.h"
#include "FAMSDUTaskPondInputData.h"
#include "FAMSDUTaskFeedingInputData.h"
#include "FAMSDUTaskRecoveryInputData.h"
#include "FAMSDUTaskDisposalInputData.h"
#include "FAMSDUTaskShippingInputData.h"
#include "FAMSDUTaskWaterInputData.h"
#include "FAMSDUTaskMaintenanceInputData.h"
#include "FAMSDUTaskMonitorInputData.h"
#include "FAMSDUBaseHandler.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUTaskHandler
     * @brief The class supports handle Sensor search request.
     */
    class FAMSDUTaskHandler: public FAMSDUBaseHandler {/*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDUTaskHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUTaskHandler(const FAMSDUTaskHandler &) = delete; /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUTaskHandler &operator=(const FAMSDUTaskHandler &) = delete; /*pgr0055*//* pgr2223*//*pgr0309*/

        /*!
         * @brief The destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDUTaskHandler(void) = default;

        /*!
         *  @brief The main method for building sensor info.
         *
         *  @param [in] request the HTTP request data
         *  @param [out] response the HTTP response data
         *
         *  @return None.
         */
        void handleRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);
    private:

        //! the request data
        FAMSDUTaskPondInputData mRequestPondInputData;
        FAMSDUTaskFeedingInputData mRequestFeedingInputData;
        FAMSDUTaskRecoveryInputData mRequestRecoveryInputData;
        FAMSDUTaskDisposalInputData mRequestDisposalInputData;
        FAMSDUTaskShippingInputData mRequestShippingInputData;
        FAMSDUTaskWaterInputData mRequestWaterInputData;
        FAMSDUTaskMaintenanceInputData mRequestMaintenanceInputData;
        FAMSDUTaskMonitorInputData mRequestMonitorInputData;

        //! Request Type for classify what kind of request are handling
        RequestType mRequestType;

        //! Json Paramater NG
        static const int JSON_NG = 2;
        //! DB Access NG
        static const int DB_NG = 3;
        //! Data NG
        static const int DATE_NG = 4;
        //! passed DB Access, Internal Error
        static const int INTERNAL_NG = 5;

        /*!
         * @brief Validate JSON format.
         *
         * @param [in] jsonString the JSON string input.
         * @param [out] errKey first key not comply rule
         *
         * @retval OK json format  is OK.
         * @retval JSON_NG json format is invalid
         */
        int validateJSONFormat(const std::string &jsonString, mdbcom::MDBContentParser::KeyMapType &jsonMap);


        std::string buildGetTasksResponseData(int &retCode, const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUTaskSummaryData> &obsStore);

        std::string buildPostResponseData(std::vector<std::string> errKeys, long long newId);
        std::string buildGetPondData(std::vector<std::string> &errKeys, FAMSDUTaskPondInputData &pondInputData);
        std::string buildGetFeedingData(std::vector<std::string> &errKeys, FAMSDUTaskFeedingInputData &feedingInputData);
        std::string buildGetRecoveryData(std::vector<std::string> &errKeys, FAMSDUTaskRecoveryInputData &recoveryInputData);
        std::string buildGetDisposalData(std::vector<std::string> &errKeys, FAMSDUTaskDisposalInputData &disposalInputData);
        std::string buildGetShippingData(std::vector<std::string> &errKeys, FAMSDUTaskShippingInputData &shippingInputData);
        std::string buildGetWaterData(std::vector<std::string> &errKeys, FAMSDUTaskWaterInputData &waterInputData);
        std::string buildGetMaintenanceData(std::vector<std::string> &errKeys, FAMSDUTaskMaintenanceInputData &maintenanceInputData);
        std::string buildGetMonitorData(std::vector<std::string> &errKeys, FAMSDUTaskMonitorInputData &monitorInputData);

        void handleGetRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void handleGetTasksRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void handleDeleteTaskRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void handleGetTaskRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void handlePutTaskRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void handlePostTaskRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);

        std::string buildDeleteTasksResponseData(int &retCode, const std::vector<std::string> &errorCodes);

        void operatePondDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);
        void operateFeedingDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);
        void operateRecoveryDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);
        void operateDisposalDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);
        void operateShippingDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);
        void operateWaterDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);
        void operateMaintenanceDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);
        void operateMonitorDataInDB(RequestType requestType, std::vector<std::string> &errKeys, long long &taskId, mdbcom::MDBContentParser::KeyMapType &jsonMap);

    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_HANDLER_H_ */

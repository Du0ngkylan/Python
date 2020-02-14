/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUObserversHandler.h
 * @brief       Sensor search request handler file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_OBSERVER_REQUEST_HANDLER_H_
#define FAMS_DATA_UTIL_OBSERVER_REQUEST_HANDLER_H_
#include <string>
#include <future>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "MDBContentParser.h"
#include "FAMSDUCommon.h"
#include "FAMSDUResponseData.h"
#include "FAMSDUSerializeObserversData.h"
#include "FAMSDUBaseHandler.h"

namespace famsdatautil {

    /*!
     * @class FAMSDUObserversHandler
     * @brief The class supports handle Sensor search request.
     */
    class FAMSDUObserversHandler: public FAMSDUBaseHandler {/*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDUObserversHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUObserversHandler(const FAMSDUObserversHandler &) = delete; /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUObserversHandler &operator=(const FAMSDUObserversHandler &) = delete; /*pgr0055*//* pgr2223*//*pgr0309*/

        /*!
         * @brief The destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDUObserversHandler(void) = default;

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

        //! the PUT/POST request data
        FAMSDUSerializeObserversData mSerializeRequestData;

        //! the request data
        FAMSDUObserversData mRequestData;

        //! the response data
        FAMSApResponseData mResponseData;

        //! Request Type for classify what kind of request are handling
        RequestType mRequestType;

        //private function return code
        //! Return OK
        //! Others(Internal error)
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
        int validateJSONFormat(const std::string &jsonString, std::string &errKey, const RequestType &type);


        std::string buildGetObserversResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUObserversData> &obsStore);
        /*
         * POST method
         */
        void handlePostObserversRequest(Poco::Net::HTTPServerRequest &request,
                          Poco::Net::HTTPServerResponse &response);
        Poco::Net::HTTPResponse::HTTPStatus handlePostDatabase(std::vector<std::string> errKeys, std::vector<long long> &idList);
        Poco::Net::HTTPResponse::HTTPStatus handlePutDatabase(std::vector<std::string> &errKeys);
        /*
         * POST method
         */
        void handlePutObserversRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);

        void handleGetObserversRequest(Poco::Net::HTTPServerRequest &request,
                                   Poco::Net::HTTPServerResponse &response);
        int getObserverByEmail(const std::string mail);
        std::string buildPostResponseData(int &retCode,
                const std::vector<std::string> &errorCodes,
                std::vector<long long> idList);
        std::string  buildPutResponseData(const std::vector<std::string> &errorCodes);

        void handleDeleteObserversRequest(Poco::Net::HTTPServerRequest &request,
                                   Poco::Net::HTTPServerResponse &response);

        std::string buildDeleteObserversResponseData(int &retCode,
            const std::vector<std::string> &errorCodes);

    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_HANDLER_H_ */

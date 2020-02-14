/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUVersionHandler.h
 * @brief       Sensor search request handler file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_VERSION_REQUEST_HANDLER_H_
#define FAMS_DATA_UTIL_VERSION_REQUEST_HANDLER_H_
#include <string>
#include <future>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "MDBContentParser.h"
#include "FAMSDUCommon.h"
#include "FAMSDUResponseData.h"
#include "FAMSDUBaseHandler.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUVersionHandler
     * @brief The class supports handle Sensor search request.
     */
    class FAMSDUVersionHandler: public FAMSDUBaseHandler {/*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDUVersionHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUVersionHandler(const FAMSDUVersionHandler &) = delete; /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUVersionHandler &operator=(const FAMSDUVersionHandler &) = delete; /*pgr0055*//* pgr2223*//*pgr0309*/

        /*!
         * @brief The destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDUVersionHandler(void) = default;

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
        FAMSDUVersionData mRequestData;

        //! the response data
        FAMSApResponseData mResponseData;

        //! Request Type for classify what kind of request are handling
        RequestType mRequestType;

        //private function return code
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
        int validateJSONFormat(const std::string &jsonString, std::string &errKey);


        std::string buildGetVersionsResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUVersionData> &versionStore);

        /*
         * POST method
         */
        void handleGetRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);

        void handleGetVersionRequest(Poco::Net::HTTPServerRequest &request,
                                   Poco::Net::HTTPServerResponse &response);

    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_HANDLER_H_ */

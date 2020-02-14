/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUCisternHandler.h
 * @brief       Sensor search request handler file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_CISTERN_REQUEST_HANDLER_H_
#define FAMS_DATA_UTIL_CISTERN_REQUEST_HANDLER_H_
#include <string>
#include <future>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "MDBContentParser.h"
#include "FAMSDUCommon.h"
#include "FAMSDUCisternData.h"
#include "FAMSDUBaseHandler.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUCisternHandler
     * @brief The class supports handle Sensor search request.
     */
    class FAMSDUCisternHandler: public FAMSDUBaseHandler {/*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDUCisternHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUCisternHandler(const FAMSDUCisternHandler &) = delete; /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUCisternHandler &operator=(const FAMSDUCisternHandler &) = delete; /*pgr0055*//* pgr2223*//*pgr0309*/

        /*!
         * @brief The destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDUCisternHandler(void) = default;

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
        FAMSDUUserData mRequestData;

        //! Request Type for classify what kind of request are handling
        RequestType mRequestType;

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

        std::string buildGetCisternsResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUCisternData> &cisternList);

        void handleGetCisternsRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);

        void handleGetRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);
        //private function return code
        //! Return OK
        static const int OK = 0;
        //! Others(Internal error)
        static const int NG = 1;
        //! Json Paramater NG
        static const int JSON_NG = 2;
        //! DB Access NG
        static const int DB_NG = 3;
        //! Data NG
        static const int DATE_NG = 4;
        //! passed DB Access, Internal Error
        static const int INTERNAL_NG = 5;
    };
}

#endif /* FAMS_DATA_UTIL_CISTERN_REQUEST_HANDLER_H_ */

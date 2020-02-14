/*
 * FAMSDUOrganismHandler.h
 *
 *  Created on: Jan 30, 2019
 *      Author: mapdb
 */

#ifndef FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMHANDLER_H_
#define FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMHANDLER_H_


#include <string>
#include <future>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "MDBContentParser.h"
#include "FAMSDUCommon.h"
#include "FAMSDUBaseHandler.h"
#include "FAMSDUOrganismData.h"
#include "FAMSDUPostOrganismData.h"
#include "FAMSDURqOrganismAnalyzer.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUOrganismHandler
     * @brief The class supports handle Sensor search request.
     */
    class FAMSDUOrganismHandler: public FAMSDUBaseHandler {/*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDUOrganismHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUOrganismHandler(const FAMSDUOrganismHandler &) = delete; /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUOrganismHandler &operator=(const FAMSDUOrganismHandler &) = delete; /*pgr0055*//* pgr2223*//*pgr0309*/

        /*!
         * @brief The destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDUOrganismHandler(void) = default;

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

        FAMSDUPostOrganismData mPostRequestData;

        //! Request Type for classify what kind of request are handling
        RequestType mRequestType;

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
        void handleOrganismListPostRequest(Poco::Net::HTTPServerRequest &request,
                                   Poco::Net::HTTPServerResponse &response);
        void handleOrganismListGetRequest(Poco::Net::HTTPServerRequest &request,
                                   Poco::Net::HTTPServerResponse &response);       
        int validateJSONFormat(
                const std::string &jsonstring,
                mdbcom::MDBContentParser::KeyMapType &jsonMap,
                std::string &errKeys);
    };
}



#endif /* FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMHANDLER_H_ */

/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPNitrificationHandler.h
 * @brief       Header file of class FAMSEPNitrificationHandler.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_EVENT_PROCESSING_NITRIFICATION_HANDLER_H_
#define FAMS_EVENT_PROCESSING_NITRIFICATION_HANDLER_H_
#include <string>
#include <future>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "FAMSEPCommon.h"
#include "FAMSEPConfig.h"
#include "MDBJsonParser.h"
#include "FAMSEPEventHandler.h"
#include "FAMSCommon.h"

namespace famseventprocessing {
    class FAMSIpcHeader;

    /*!
     * @class FAMSEPNitrificationHandler
     * @brief Base handler class for history db message
     */
    class FAMSEPNitrificationHandler: public FAMSEPEventHandler { /* pgr2205 */
        /* pgr2252 */

    public:
        /*!
         * @brief Data constructor
         * @param [in] msg Message data to handle.
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        FAMSEPNitrificationHandler(const famscommon::FAMSIpcHeader *msg);

        /*!
         * @brief Default constructpr
         * @param None.
         * @return None.
         */
        FAMSEPNitrificationHandler(void) = default;

        /*!
         * @brief Default destructor
         * @param None.
         * @return None.
         */
        virtual ~FAMSEPNitrificationHandler(void) = default;

        /*!
         * @brief Copy constructor
         * @param None.
         * @return None.
         */
        FAMSEPNitrificationHandler(const FAMSEPNitrificationHandler &) = default;

        /*!
         * @brief Copy Assignment
         * @param None.
         * @return None.
         */
        FAMSEPNitrificationHandler &operator =(const FAMSEPNitrificationHandler &) = default; /* pgr0055 *//* pgr0309 */

        /*!
         * @brief Move constructor
         * @param None.
         * @return None.
         */
        FAMSEPNitrificationHandler(FAMSEPNitrificationHandler &&) = default;

        /*!
         * @brief Move Assignment
         * @param None.
         * @return None.
         */
        FAMSEPNitrificationHandler &operator=(FAMSEPNitrificationHandler &&) = default; /* pgr0055 *//* pgr0309 */

        /*!
         * @brief Handle current message
         * @param None.
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        int handleEvent(void) override;

        /*!
         * @brief build request JSON data to post to server
         * this method build KeyMapType to JSON string
         *
         * @param None.
         * @retval string form of JSON body
         */
        std::string buildRequestData(void);

        /*!
         * @brief build request JSON data to post to server
         * this method build KeyMapType to JSON string
         *
         * @param [in] type Type of delivery.
         * @param [in] jsonMap data map to build response body
         * @retval string form of JSON body
         */
        std::string buildRequestData(int type,
                                     const mdbcom::MDBContentParser::KeyMapType &jsonMap);

    private:
    };
}

#endif /* FAMS_EVENT_PROCESSING_NITRIFICATION_HANDLER_H_ */

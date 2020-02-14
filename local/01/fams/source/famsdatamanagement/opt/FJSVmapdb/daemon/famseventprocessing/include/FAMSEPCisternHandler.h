/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPCisternHandler.h
 * @brief       Header file of class FAMSEPCisternHandler.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_EVENT_PROCESSING_CISTERN_HANDLER_H_
#define FAMS_EVENT_PROCESSING_CISTERN_HANDLER_H_
#include <string>
#include <future>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "FAMSEPCommon.h"
#include "FAMSEPConfig.h"
#include "MDBJsonParser.h"
#include "FAMSEPEventHandler.h"
#include "FAMSCommon.h"
#include "FAMSEPSalinityCalculation.h"

#define CONDUCTIVITY 42.914
#define PRESSURE 1013
#define DIVFACTOR_TMP 1.00024

namespace famseventprocessing {
    class FAMSIpcHeader;

    /*!
     * @class FAMSEPCisternHandler
     * @brief Base handler class for history db message
     */
    class FAMSEPCisternHandler: public FAMSEPEventHandler { /* pgr2205 */
        /* pgr2252 */

    public:
        /*!
         * @brief Data constructor
         * @param [in] msg Message data to handle.
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        FAMSEPCisternHandler(const famscommon::FAMSIpcHeader *msg);

        /*!
         * @brief Default constructpr
         * @param None.
         * @return None.
         */
        FAMSEPCisternHandler(void) = default;

        /*!
         * @brief Default destructor
         * @param None.
         * @return None.
         */
        virtual ~FAMSEPCisternHandler(void) = default;

        /*!
         * @brief Copy constructor
         * @param None.
         * @return None.
         */
        FAMSEPCisternHandler(const FAMSEPCisternHandler &) = default;

        /*!
         * @brief Copy Assignment
         * @param None.
         * @return None.
         */
        FAMSEPCisternHandler &operator =(const FAMSEPCisternHandler &) = default; /* pgr0055 *//* pgr0309 */

        /*!
         * @brief Move constructor
         * @param None.
         * @return None.
         */
        FAMSEPCisternHandler(FAMSEPCisternHandler &&) = default;

        /*!
         * @brief Move Assignment
         * @param None.
         * @return None.
         */
        FAMSEPCisternHandler &operator=(FAMSEPCisternHandler &&) = default; /* pgr0055 *//* pgr0309 */

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

#endif /* FAMS_EVENT_PROCESSING_CISTERN_HANDLER_H_ */

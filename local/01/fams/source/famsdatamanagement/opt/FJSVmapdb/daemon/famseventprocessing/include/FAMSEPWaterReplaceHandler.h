/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPWaterReplaceHandler.h
 * @brief       Header file of class FAMSEPWaterReplaceHandler.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_EVENT_PROCESSING_WATER_REPLACE_HANDLER_H_
#define FAMS_EVENT_PROCESSING_WATER_REPLACE_HANDLER_H_
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
     * @class FAMSEPWaterReplaceHandler
     * @brief Base handler class for history db message
     */
    class FAMSEPWaterReplaceHandler: public FAMSEPEventHandler { /* pgr2205 */
        /* pgr2252 */

    public:
        /*!
         * @brief Data constructor
         * @param [in] msg Message data to handle.
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        FAMSEPWaterReplaceHandler(const famscommon::FAMSIpcHeader *msg);

        /*!
         * @brief Default constructpr
         * @param None.
         * @return None.
         */
        FAMSEPWaterReplaceHandler(void) = default;

        /*!
         * @brief Default destructor
         * @param None.
         * @return None.
         */
        virtual ~FAMSEPWaterReplaceHandler(void) = default;

        /*!
         * @brief Copy constructor
         * @param None.
         * @return None.
         */
        FAMSEPWaterReplaceHandler(const FAMSEPWaterReplaceHandler &) = default;

        /*!
         * @brief Copy Assignment
         * @param None.
         * @return None.
         */
        FAMSEPWaterReplaceHandler &operator =(const FAMSEPWaterReplaceHandler &) = default; /* pgr0055 *//* pgr0309 */

        /*!
         * @brief Move constructor
         * @param None.
         * @return None.
         */
        FAMSEPWaterReplaceHandler(FAMSEPWaterReplaceHandler &&) = default;

        /*!
         * @brief Move Assignment
         * @param None.
         * @return None.
         */
        FAMSEPWaterReplaceHandler &operator=(FAMSEPWaterReplaceHandler &&) = default; /* pgr0055 *//* pgr0309 */

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

#endif /* FAMS_EVENT_PROCESSING_WATER_REPLACE_HANDLER_H_ */

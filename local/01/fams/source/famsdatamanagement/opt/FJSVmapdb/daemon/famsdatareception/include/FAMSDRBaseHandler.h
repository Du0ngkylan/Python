/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRBaseHandler.h
 * @brief       Header file for define FAMSDRBaseHandler class.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_DATA_RECEPTION_BASE_HANDLER_H_
#define FAMS_DATA_RECEPTION_BASE_HANDLER_H_
#include <thread>
#include <sstream>
#include "MDBIPCShm.h"
#include <Poco/Net/HTTPRequestHandler.h>

#include "FAMSIpcHeader.h"
#include "MDBJsonParser.h"
#include "MDBContentDecodingFormat.h"
#include "FAMSDRConfig.h"
#include "FAMSCommon.h"
#include "MDBConnectionManager.h"
#include "MDBHTTPServerUtility.h"
#include "FAMSDRCommon.h"

namespace famsdatareception {
    using famscommon::FAMSWaterReplaceData;

    /*! @class FAMSDRBaseHandler
     *  @brief This base class of all data reception app handler
     */
    class FAMSDRBaseHandler: public Poco::Net::HTTPRequestHandler { /*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] sharedMemory the shared memory from main.
         * @return None.
         */
        explicit FAMSDRBaseHandler(mdbcom::MDBIPCShm *sharedMemory,
                                        const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate) :
            mSharedMemory { sharedMemory }, mJsonTemplate(inputTemplate) {
        }

        /*!
         * @brief The destructor.
         * @param None.
         * @return None.
         */
        virtual ~FAMSDRBaseHandler(void) {
        }

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRBaseHandler(const FAMSDRBaseHandler &) = delete;

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRBaseHandler &operator=(const FAMSDRBaseHandler &) = delete;


        /*!
         *  @brief The main method for building Mod MessageType(Object) info at MAPDB..
         *
         *  @param [in] request the HTTP request data
         *  @param [out] response the HTTP response data
         *
         *  @return None.
         */
        void handleRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);

    private:
        // Response include : code, message, error keys
        static const int RESPONSE_CODE_BY_WRONG_INPUT = 1;
        static const std::string RESPONSE_MESSAGE_BY_WRONG_INPUT;
        // Response include : code, message
        static const int RESPONSE_CODE_BY_FAILED_REGISTER = 2;
        static const std::string RESPONSE_MESSAGE_BY_FAILED_REGISTER;


    protected:
        /*!
         * @brief Get ipc data buffer and sise
         * @param [out] dataLength Length of data
         * @retval pointer to ipc buffer prepared to be sent
         */
        virtual std::unique_ptr<famscommon::FAMSIpcHeader> getIpcInfoData(famscommon::MessageType eventType,
                std::size_t &dataLength) = 0;
        /*!
         * @brief Register an event processing
         *
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        virtual int registerEvent(void) = 0;

        /*!
         * @brief Build response data.
         *
         * @param [in] code the return code
         * @param [in] mess the return message
         * @param [in] errKeys the return error keys
         * @return the JSON response string
         */
        virtual std::string buildErrorResponseData(const std::vector<std::string> errorCodes);

        static std::vector<mdbcom::MDBContentParser::KeyMapType> buildErrorMessages(const std::vector<std::string> &errorCodes) {
            auto errorsTag = std::vector<mdbcom::MDBContentParser::KeyMapType> {};/*pgr0745*/
            mdbcom::MDBJsonParser jsonEncoder {};
            jsonEncoder.setSerialKey(0);
            auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
            for(auto error: errorCodes) {
                auto errorTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(errorTag, CODE, error);
                jsonEncoder.setKeyMapDataValue(errorTag, MESSAGE, getConfig().getErrorMessage(error));
                errorsTag.emplace_back(errorTag);
            }
            return errorsTag;
        }

        /*!
         * @brief Validate JSON format.
         *
         * @param [in] jsonString the JSON string input.
         * @param [out] jsonMap the map of JSON data
         *
         * @retval REST_RET_NORMAL json format  is OK.
         * @retval REST_RET_INCORRECT_FORMAT json format is invalid
         */
        virtual int validateJSONFormat(const std::string &jsonString,
                               mdbcom::MDBContentParser::KeyMapType &jsonMap,
                               std::vector<std::string> &errKeys) = 0;

        /*!
         * @brief Send Ipc info data for event processing process
         * @param [in] eventType event type to send to queue
         * @retval REST_RET_NORMAL normally
         * @retval REST_RET_ABNORMAL_SERVER abnormally
         */
        int sendIpcInfoData(famscommon::MessageType eventType);

        //! The shared memory from main
        mdbcom::MDBIPCShm *mSharedMemory;

        //! JSON template for decoding
        const std::vector<mdbcom::MDBContentDecodingFormat> &mJsonTemplate;

        /*!
         * @brief The method to get the configuration
         * @param none
         * @return reference to configuration object
         */
        inline static FAMSDRConfig &getConfig() {
            return FAMSDRConfig::getInstance();
        }

        // ------------- REST API return code START ------------
        //! RESTful API return code normal
        static constexpr int REST_RET_NORMAL = 0;/* pgr0746 */
        //! RESTful API return code when JSON request error
        static constexpr int REST_RET_INCORRECT_FORMAT = 1;/* pgr0746 */
        //! RESTful API return code in case access DB error
        static constexpr int REST_RET_DB_ERROR = 2;
        //! RESTful API return code Sensor mismatch (UNUSED)
        static constexpr int REST_RET_SENSOR_ID_MISMATCH = 3;
        //! RESTful API return code other error
        static constexpr int REST_RET_OTHER_ERROR = 999;
        // ------------- REST API return code END ------------

        /*!
         * @brief The method to get the configuration for HttpServer.
         * @param None.
         * @return Reference to configuration object
         */
        inline static const FAMSDRConfig &getDataReceptionConfig() {
            return FAMSDRConfig::getInstance();
        }

        //! Key string constant
        static const std::string JSON_MAP_KEYS_STR;
        //! Code string constant
        static const std::string JSON_MAP_CODE_STR;
        //! Message string constant
        static const std::string JSON_MAP_MSG_STR;
    };
}

#endif /* FAMS_DATA_RECEPTION_BASE_HANDLER_H_ */

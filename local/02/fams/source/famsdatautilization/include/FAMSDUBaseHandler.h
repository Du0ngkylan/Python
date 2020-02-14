/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUBaseHandler.h
 * @brief       base handler file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_BASE_HANDLER_H_
#define FAMS_DATA_UTIL_BASE_HANDLER_H_

#include <Poco/Net/HTTPRequestHandler.h>

#include "MDBException.h"
#include "MDBContentDecodingFormat.h"
#include "FAMSDUConfig.h"

namespace famsdatautil {
    /*! @class FAMSDUBaseHandler
     *  @brief This is base class of all Data Utilization handler
     */
    class FAMSDUBaseHandler: public Poco::Net::HTTPRequestHandler { /*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDUBaseHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate) :
            mJsonTemplate(inputTemplate) {
        }

        /*!
         * @brief The destructor.
         * @param None.
         * @return None.
         */
        virtual ~FAMSDUBaseHandler(void) {
            std::this_thread::yield();
        }

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUBaseHandler(const FAMSDUBaseHandler &) = delete; /*pgr2222*//*pgr0571*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUBaseHandler &operator=(const FAMSDUBaseHandler &) = delete; /*pgr0055*/ /*pgr0309*/

        /*
         * title: over/below
         */
        virtual int checkSensorValue(std::string convertTime, const double value, const int type, const long long cistern_id, const int statusOfThisRecord);
        virtual int changeStatusResolved(const int type, const long long cistern_id);
        virtual int changeStatusViolated(const int type, const int resourceId);
        virtual int registerErrorMessages(std::string convertTime, std::string error_messages,
                const int lot_information_id, const int error_at, const int statusOfThisRecord);

    protected:
        //! RESTful API return code normal
        static constexpr int REST_RET_NORMAL = 0;
        //! RESTful API return code invalid parameters
        static constexpr int REST_RET_INVALID_PARAMETERS = 1;
        //! RESTful API return comide abnormal other
        static constexpr int REST_RET_ABNORMAL_OTHERS = 999;

        //! Message for return code 1
        static constexpr const char *const INVALID_PARAMETERS_MESS {"が不正な型です"};
        //! Message for return code 999
        static constexpr const char *const ABNORMAL_OTHERS_MESS {"内部異常が発生しました"};

        //! SQL Result number of Lines.
        static constexpr int SQL_RESULT_ZERO = 0;

        //! template to parse JSON
        const std::vector<mdbcom::MDBContentDecodingFormat> mJsonTemplate;

        /*!
         * @brief The method to get the configuration
         * @param none
         * @return reference to configuration object
         */
        inline static FAMSDUConfig &getConfig() {
            return FAMSDUConfig::getInstance();
        }

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
    };
}

#endif /* FAMS_DATA_UTIL_BASE_HANDLER_H_ */

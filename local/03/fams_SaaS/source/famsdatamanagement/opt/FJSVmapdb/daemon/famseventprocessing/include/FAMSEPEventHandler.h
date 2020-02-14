/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file    FAMSEPEventHandler.h
 * @brief   Base handler class for FAMSEPEventHandler
 * @author  anhnguyen
 * @date    2018/12/12
 */

#ifndef FAMS_EVENT_PROCESSING_EVENT_HANDLER_H_
#define FAMS_EVENT_PROCESSING_EVENT_HANDLER_H_
#include "FAMSIpcHeader.h"
#include "FAMSEPCommon.h"
#include "FAMSEPConfig.h"
#include "FAMSCommon.h"

#include <string>
#include <future>

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace famseventprocessing {

    /*!
     * @class FAMSEPEventHandler
     * @brief Base handler class for incoming message
     */
    class FAMSEPEventHandler {
    public:

        /*!
         * @brief Data constructor
         * @param [in] msg Message data to handle.
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        FAMSEPEventHandler(const famscommon::FAMSIpcHeader *msg)
            : mShrMessage { msg } {
        }

        /*!
         * @brief Default constructpr
         * @param None.
         * @return None.
         */
        FAMSEPEventHandler(void) : mShrMessage { } {

        }

        /*!
         * @brief Default destructor
         * @param None.
         * @return None.
         */
        virtual ~FAMSEPEventHandler(void) = default;

        /*!
         * @brief Copy constructor
         * @param None.
         * @return None.
         */
        FAMSEPEventHandler(const FAMSEPEventHandler &) = default; /*pgr0571*//*pgr2266*/

        /*!
         * @brief Copy Assignment
         * @param None.
         * @return None.
         */
        FAMSEPEventHandler &operator = (const FAMSEPEventHandler &) = default; /* pgr0055*//*pgr0309*/

        /*!
         * @brief Move constructor
         * @param None.
         * @return None.
         */
        FAMSEPEventHandler(FAMSEPEventHandler &&) = default;

        /*!
         * @brief Move Assignment
         * @param None.
         * @return None.
         */
        FAMSEPEventHandler &operator = (FAMSEPEventHandler &&) = default;

        /*!
         * @brief Handle current message
         * @param None.
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        virtual int handleEvent(void) = 0;

        /*!
         * @brief The method to get the configuration for HttpServer.
         * @param None.
         * @return Reference to configuration object
         */
        inline static FAMSEPConfig &getEventProcessingConfig() {
            return FAMSEPConfig::getInstance();
        }

        virtual int getMailConfig(std::string &hostname, std::string &sender, std::string &subject, std::string &body,
                std::string &subjectEx, std::string &bodyEx, std::string &username, std::string &password, int &port, int &numViolationCfg);
        virtual int getMailRecipents(std::vector<std::string> &recipients);
        virtual int checkSensorValue(const double value, const int type, const std::string cistern_code,
                    std::vector<std::string> &title, std::vector<std::string> &cisternName, std::string accumulatedTime, std::vector<std::string> &organishName, std::vector<std::string> &distinguishName);

        virtual int addMailSection(const double value, const int type, const std::string cistern_code,  std::string &pHContent, std::string accumulatedTime,
                    std::vector<std::string> &titles, std::vector<std::string> &cisternNames, std::vector<std::string> &organismNames, std::vector<std::string> &distinguishNames);

        virtual int addMailSections(const double value, const int type, const std::string cistern_code,  std::string &pHContent, std::string accumulatedTime,
                            std::vector<std::string> &titles, std::vector<std::string> &cisternNames, std::vector<std::string> &organismNames, std::vector<std::string> &distinguishNames);
        virtual bool addErrorMessage(const std::string errorMessage, const std::string accumulated_time, const int resource_id, const int resource_type, const int status, const int error_at);
        virtual bool changeStatusResolved(const int type, const int status);
        virtual bool changeStatusViolated(const int type, const int resourceId, const int status);
    protected:
        //! mShrMessage data
        const famscommon::FAMSIpcHeader *mShrMessage;

        //! RESTful API return code normal
        static constexpr int REST_RET_NORMAL = 0;
        //! RESTful API return code invalid others
        static constexpr int REST_RET_INVALID_OTHERS = 102;
        //! RESTful API return code abnormal server
        static constexpr int REST_RET_ABNORMAL_SERVER = 103;
        //! RESTful API return code abnormal other
        static constexpr int REST_RET_ABNORMAL_OTHERS = 999;
        static constexpr int RET_ABNORMAL_MAPMATCHING = -1;

    };
}
#endif /* FAMS_EVENT_PROCESSING_EVENT_HANDLER_H_ */

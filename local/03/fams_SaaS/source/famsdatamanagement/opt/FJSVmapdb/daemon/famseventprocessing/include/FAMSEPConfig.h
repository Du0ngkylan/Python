/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPConfig.h
 * @brief       Header file of class FAMSEPConfig.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_EVENT_PROCESSING_CONFIG_H_
#define FAMS_EVENT_PROCESSING_CONFIG_H_

#include <string>
#include <vector>
#include <set>
#include "MDBConfigFile.h"

namespace famseventprocessing {
    // thread name
    extern const std::string SENSOR_THREAD_NAME;
    extern const std::string EVENT_RECEPTION_THREAD_NAME;
    /*!
     * @class FAMSEPConfig
     * @brief Manage vehicle info  management application configuration.
     */
    class FAMSEPConfig {
    public:

        /*!
         * @brief Destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSEPConfig(void) = default;

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        explicit FAMSEPConfig(const FAMSEPConfig &) = delete;

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSEPConfig &operator=(const FAMSEPConfig &) = delete;

        /*!
         * @brief Get an instance of FAMSEPConfig.
         * @param None.
         * @return Instance of FAMSEPConfig
         */
        inline static FAMSEPConfig &getInstance(void) {
            // Constructor with event processing app configuration file path
            static FAMSEPConfig eventProcessingConfig(EVENT_PROCESSING_CONF);
            return eventProcessingConfig;
        }

        // ------------------------- START configs getters for threads -----------------------
        /*!
         * @brief Get number of Reception threads
         * @param [in].
         * @return number of threads
         */
        inline unsigned long long getThreadsNum(const std::string &threadName) const {
            auto threadNumKey = mThreadNameThreadNumKeyMap[threadName]; /*pgr0745*/
            return mIntegerValuesMap[threadNumKey];
        }

        /*!
         * @brief Get number of Reception threads
         * @param [in].
         * @return number of threads
         */
        inline unsigned long long getThreadsShmSize(const std::string &threadName) const {
            auto threadShmKey = mThreadNameShmKeyMap[threadName]; /*pgr0745*/
            return mIntegerValuesMap[threadShmKey];
        }

        inline const std::string &getConductivityCoEfficient(void) const {
            return mStringKeyMap[CONDUCTIVITYCOEFFICIENT];
        }

        inline const std::string &getHostname(void) const {
            return mStringKeyMap[HOSTNAME];
        }

        inline const std::string &getNumberViolation(void) const {
            return mStringKeyMap[NUMBER_VIOLATION];
        }

        inline const std::string &getSuject(void) const {
            return mStringKeyMap[SUBJECT];
        }

        inline const std::string &getBody(void) const {
            return mStringKeyMap[BODY];
        }

        inline const std::string &getBodyExceed(void) const {
            return mStringKeyMap[BODY_EXCEEDED];
        }

        inline const std::string &getSujectExceed(void) const {
            return mStringKeyMap[SUBJECT_EXCEEDED];
        }

        inline const std::string &getSender(void) const {
            return mStringKeyMap[SENDER];
        }

        inline const std::string &getRecipient(void) const {
            return mStringKeyMap[RECIPIENT];
        }

        inline const std::string &getUsername(void) const {
            return mStringKeyMap[USERNAME];
        }

        inline const std::string &getPassword(void) const {
            return mStringKeyMap[PASSWORD];
        }

        // ------------------------- END configs getters for threads -----------------------

        static const int EXECUTE;
        static const int NO_EXECUTE;

    protected:
        //! The config pointers
        std::unique_ptr<mdbcom::MDBConfigFile> mConfigFile;

    private:
        explicit FAMSEPConfig(const std::string &configPath);
        //! Constant event processing configure path
        static const std::string EVENT_PROCESSING_CONF;
        static const std::string MAIL_PROCESSING_CONF;
        //! map to hold threads names and corresponding shame mem size keys
        static std::map<std::string, std::string> mThreadNameShmKeyMap;

        //! map to hold threads names and corresponding threads num keys
        static std::map<std::string, std::string> mThreadNameThreadNumKeyMap;
        static const std::string SENSOR_THREADS_KEY;

        static const std::string RECEPTION_THREADS_KEY;
        static const std::string SENSOR_SHM_SIZE_KEY;

        //ConductivityCoEfficient configuration
        static const std::string CONDUCTIVITYCOEFFICIENT;

        // Mail configuration
        static const std::string HOSTNAME;
        static const std::string PORT;
        static const std::string NUMBER_VIOLATION;
        static const std::string SENDER;
        static const std::string RECIPIENT;
        static const std::string SUBJECT;
        static const std::string BODY;
        static const std::string SUBJECT_EXCEEDED;
        static const std::string BODY_EXCEEDED;
        static const std::string USERNAME;
        static const std::string PASSWORD;

        //! map to hold values having type unsigned int, unsigned long long likes size, time...
        static std::map<std::string, unsigned long long> mIntegerValuesMap;
        static std::map<std::string, std::string> mStringKeyMap;
    };

}

#endif /* FAMS_EVENT_PROCESSING_CONFIG_H_ */

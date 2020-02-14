/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPConfig.cpp
 * @brief       Source file for implementing FAMSEPConfig information.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include "FAMSEPConfig.h"
#include "MDBException.h"
#include "MDBLogger.h"
#include <Poco/Dynamic/Var.h>
#include "MDBHTTPServerConfig.h"
#include "FAMSCommon.h"

namespace famseventprocessing {

    const std::string FAMSEPConfig::EVENT_PROCESSING_CONF
    { "/opt/FJSVmapdb/etc/fams_event_processing.conf" };

    const int FAMSEPConfig::EXECUTE = 1;
    const int FAMSEPConfig::NO_EXECUTE = 0;

    const std::string SENSOR_THREAD_NAME {"sensorhandler"};
    const std::string EVENT_RECEPTION_THREAD_NAME {"receptionhandler"};

    // THREADS
    const std::string FAMSEPConfig::SENSOR_SHM_SIZE_KEY {"sensorMsgQueueSize"};


    const std::string FAMSEPConfig::RECEPTION_THREADS_KEY {"receptionMaxThreads"};
    const std::string FAMSEPConfig::SENSOR_THREADS_KEY {"sensorMaxThreads"};
    const std::string FAMSEPConfig::CONDUCTIVITYCOEFFICIENT {"conductivityCoefficient"};

    const std::string FAMSEPConfig::HOSTNAME {"hostname"};
    const std::string FAMSEPConfig::PORT {"port"};
    const std::string FAMSEPConfig::NUMBER_VIOLATION {"numberViolation"};
    const std::string FAMSEPConfig::SENDER {"sender"};
    const std::string FAMSEPConfig::RECIPIENT {"recipients"};
    const std::string FAMSEPConfig::BODY {"body"};
    const std::string FAMSEPConfig::SUBJECT {"subject"};
    const std::string FAMSEPConfig::BODY_EXCEEDED {"bodyexceeded"};
    const std::string FAMSEPConfig::SUBJECT_EXCEEDED {"subjectexceeded"};
    const std::string FAMSEPConfig::USERNAME {"username"};
    const std::string FAMSEPConfig::PASSWORD {"password"};

    std::map<std::string, std::string> FAMSEPConfig::mStringKeyMap
    {
        {HOSTNAME, "smtp.gmail.com"},
        {PORT, "25"},
        {NUMBER_VIOLATION, "5"},
        {SENDER, ""},
        {RECIPIENT, ""},
        {SUBJECT, ""},
        {BODY, ""},
        {SUBJECT_EXCEEDED, ""},
        {BODY_EXCEEDED, ""},
        {USERNAME, ""},
        {PASSWORD, ""},
        {CONDUCTIVITYCOEFFICIENT, "0.757"}
    };

    // setting key map to search in ShmMap and ThreadNumMap using thread name
    std::map<std::string, std::string> FAMSEPConfig::mThreadNameShmKeyMap
    {
        {SENSOR_THREAD_NAME, SENSOR_SHM_SIZE_KEY},
    };
    std::map<std::string, std::string> FAMSEPConfig::mThreadNameThreadNumKeyMap
    {
        {EVENT_RECEPTION_THREAD_NAME, RECEPTION_THREADS_KEY},
        {SENSOR_THREAD_NAME, SENSOR_THREADS_KEY},
    };

    // set default integer values for keys
    std::map<std::string, unsigned long long> FAMSEPConfig::mIntegerValuesMap
    {
        {SENSOR_SHM_SIZE_KEY, 2048}, // default share mem size
        {RECEPTION_THREADS_KEY, 1}, //default number of reception threads
        {SENSOR_THREADS_KEY, 10}, //default number of obstacle judgment threads
    };

    /*!
     * @brief Constructor with config path.
     * It will throw exception when config file or config format is abnormal.
     *
     * @param [in] configPath the configuration path.
     * @return None.
     */
    FAMSEPConfig::FAMSEPConfig(const std::string &configPath) :
        mConfigFile { nullptr }
    {
        mdb_apl_log_start();
        mConfigFile = mdbcom::MDBMemoryUtility::make_unique<mdbcom::MDBConfigFile>(configPath);

        auto errorKeyList = std::vector<std::string> {}; /* pgr0745 */

        auto cfgVal = std::string {}; /* pgr0745 */
        auto converter = Poco::Dynamic::Var {}; /* pgr0745 */

        for (auto &i:mStringKeyMap) {
            try {
                auto ret = mConfigFile->getValue(i.first, cfgVal);
                if (0 != ret || true == cfgVal.empty()) {
                    errorKeyList.push_back(i.first);
                    continue;
                }
                i.second = cfgVal;
            } catch (...) {
                errorKeyList.push_back(i.first);
            }
        }

         for (auto &i:mIntegerValuesMap) {
             try {
                 auto ret = mConfigFile->getValue(i.first, cfgVal);
                 if (0 != ret || true == cfgVal.empty()) {
                     errorKeyList.push_back(i.first);
                     continue;
                 }
                 i.second = atoll(cfgVal.c_str());
             } catch (...) {
                 errorKeyList.push_back(i.first);
             }
         }

        for (auto &errorKey : errorKeyList)
        {
            auto tmpErrorString = errorKey.c_str(); /* pgr0745 *//*pgr0039*/
            mdb_apl_log_debug("an exception occur at key %s, this key got default value", tmpErrorString);
        }

        mdb_apl_log_end();
    }

}


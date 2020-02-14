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
    const std::string CISTERN_SENSOR_THREAD_NAME {"cisternsensorhandler"};
    const std::string NITRIFICATION_SENSOR_THREAD_NAME {"nitrificationsensorhandler"};
    const std::string OUTSIDE_SENSOR_THREAD_NAME {"outsidesensorhandler"};
    const std::string SENSOR_ADD_THREAD_NAME {"sensoraddhandler"};
    const std::string EVENT_RECEPTION_THREAD_NAME {"receptionhandler"};

    // THREADS
    const std::string FAMSEPConfig::SENSOR_SHM_SIZE_KEY {"sensorMsgQueueSize"};
    const std::string FAMSEPConfig::CISTERN_SENSOR_SHM_SIZE_KEY {"cisternSensorMsgQueueSize"};
    const std::string FAMSEPConfig::NITRIFICATION_SENSOR_SHM_SIZE_KEY {"nitrificationSensorMsgQueueSize"};
    const std::string FAMSEPConfig::OUTSIDE_SENSOR_SHM_SIZE_KEY {"outsideSensorMsgQueueSize"};
    const std::string FAMSEPConfig::SENSOR_ADD_SHM_SIZE_KEY {"sensorAddMsgQueueSize"};

    const std::string FAMSEPConfig::RECEPTION_THREADS_KEY {"receptionMaxThreads"};
    const std::string FAMSEPConfig::SENSOR_THREADS_KEY {"sensorMaxThreads"};
    const std::string FAMSEPConfig::CISTERN_SENSOR_THREADS_KEY {"cisternSensorMaxThreads"};
    const std::string FAMSEPConfig::OUTSIDE_SENSOR_THREADS_KEY {"outsideSensorMaxThreads"};
    const std::string FAMSEPConfig::NITRIFICATION_SENSOR_THREADS_KEY {"nitrificationSensorMaxThreads"};
    const std::string FAMSEPConfig::SENSOR_ADD_THREADS_KEY {"sensorAddMaxThreads"};
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

	std::map<std::string, int> FAMSEPConfig::mPortTypeKeyMap;

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
        {CISTERN_SENSOR_THREAD_NAME, CISTERN_SENSOR_SHM_SIZE_KEY},
        {NITRIFICATION_SENSOR_THREAD_NAME, NITRIFICATION_SENSOR_SHM_SIZE_KEY},
        {OUTSIDE_SENSOR_THREAD_NAME, OUTSIDE_SENSOR_SHM_SIZE_KEY},
        {SENSOR_ADD_THREAD_NAME, SENSOR_ADD_SHM_SIZE_KEY},
    };
    std::map<std::string, std::string> FAMSEPConfig::mThreadNameThreadNumKeyMap
    {
        {EVENT_RECEPTION_THREAD_NAME, RECEPTION_THREADS_KEY},
        {SENSOR_THREAD_NAME, SENSOR_THREADS_KEY},
        {CISTERN_SENSOR_THREAD_NAME, CISTERN_SENSOR_THREADS_KEY},
        {NITRIFICATION_SENSOR_THREAD_NAME, NITRIFICATION_SENSOR_THREADS_KEY},
        {OUTSIDE_SENSOR_THREAD_NAME, OUTSIDE_SENSOR_THREADS_KEY},
        {SENSOR_ADD_THREAD_NAME, SENSOR_ADD_THREADS_KEY},
    };

    // set default integer values for keys
    std::map<std::string, unsigned long long> FAMSEPConfig::mIntegerValuesMap
    {
        {SENSOR_SHM_SIZE_KEY, 2048}, // default share mem size
        {RECEPTION_THREADS_KEY, 1}, //default number of reception threads
        {SENSOR_THREADS_KEY, 10}, //default number of obstacle judgment threads
        {CISTERN_SENSOR_SHM_SIZE_KEY, 2048}, // default share mem size
        {CISTERN_SENSOR_THREADS_KEY, 10}, //default number of obstacle judgment threads
        {NITRIFICATION_SENSOR_SHM_SIZE_KEY, 2048}, // default share mem size
        {NITRIFICATION_SENSOR_THREADS_KEY, 10}, //default number of obstacle judgment threads
        {OUTSIDE_SENSOR_SHM_SIZE_KEY, 2048}, // default share mem size
        {OUTSIDE_SENSOR_THREADS_KEY, 10}, //default number of obstacle judgment threads
        {SENSOR_ADD_SHM_SIZE_KEY, 2048}, // default share mem size
        {SENSOR_ADD_THREADS_KEY, 50}, //default number of obstacle judgment threads
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

        try {
            auto ret = mConfigFile->getMapValue();
			for (auto& i : ret)
			{
				if (i.first.rfind(PORT, 0) == 0)
				{
					mPortTypeKeyMap.emplace(std::pair<std::string, int>(i.first, atoi(i.second.c_str())));
				}
			}
        } catch (...) {
			mdb_apl_log_debug("Error load port config");
        }

        for (auto &errorKey : errorKeyList)
        {
            auto tmpErrorString = errorKey.c_str(); /* pgr0745 *//*pgr0039*/
            mdb_apl_log_debug("an exception occur at key %s, this key got default value", tmpErrorString);
        }

        mdb_apl_log_end();
    }

}


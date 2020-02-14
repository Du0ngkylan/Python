/* Copyright© 2020 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRConfig.cpp
 * @brief       Source file for implementing FAMSDRConfig information.
 * @author      ThangLQ.
 * @date        2020/01/20
 */

#include "FAMSDRConfig.h"
#include "MDBException.h"
#include "MDBLogger.h"
#include <Poco/Dynamic/Var.h>
#include <yaml-cpp/yaml.h>
#include "FAMSCommon.h"

namespace famsdatareception {

    // configuration file
    const std::string FAMSDRConfig::APP_DATA_RECEPTION_SERVER_CONF
    {
        "/opt/FJSVmapdb/etc/fams_receiver.conf"
    };
    // YAML file
    const std::string FAMSDRConfig::TEMPLATE_CONF_KEY {"templateConfig"};
    const std::string FAMSDRConfig::DEFAULT_TEMPLATE_CONF {"/opt/FJSVmapdb/etc/fams_receiver_api.yaml"};
    const std::string FAMSDRConfig::ERROR_MESSAGE_CONF {"/opt/FJSVmapdb/etc/fams_error_messages.yaml"};

    std::map<std::string, std::string> FAMSDRConfig::mStringValuesMap
    {
        {TEMPLATE_CONF_KEY, DEFAULT_TEMPLATE_CONF}
    };

    /*!
     * @brief Constructor with config path.
     * It will throw exception when config file or config format is abnormal.
     *
     * @param [in] configPath the configuration path.
     * @return None.
     */
    FAMSDRConfig::FAMSDRConfig(const std::string &configPath) :
        mdbcom::MDBHTTPServerConfig { configPath }
    {
        mdb_apl_log_start();

        auto errorKeyList = std::vector<std::string> {}; /* pgr0745 */

        auto cfgVal = std::string {}; /* pgr0745 */
        auto converter = Poco::Dynamic::Var {}; /* pgr0745 */

        for (auto &keyVal : mStringValuesMap)
        {
            auto ret = mConfigFile->getValue(keyVal.first, cfgVal); /* pgr0745 */

            if ((mdbcom::Result::OK != ret) || (true == cfgVal.empty()))
            {
                errorKeyList.push_back(keyVal.first);
                continue;
            }

            keyVal.second = cfgVal;
        }

        for (auto &errorKey : errorKeyList)
        {
            auto tmpErrorString = errorKey.c_str(); /* pgr0745 */ /*pgr0039*/
            mdb_apl_log_debug("an exception occur at key %s, this key got default value", tmpErrorString);
        }
        auto errorMessages = YAML::LoadFile(ERROR_MESSAGE_CONF); /* pgr0745 */

        if (true == errorMessages.IsNull())
        {
            mdb_apl_log_critical("Cannot load configuration file: %s", ERROR_MESSAGE_CONF.c_str());
        } else {
            for (auto errElement = errorMessages.begin(); errElement != errorMessages.end(); errElement++) /* pgr0745 */
            {
                mErrorMap.insert ( std::pair<std::string,std::string>(errElement->first.as<std::string>(),errElement->second.as<std::string>()) );
            }
        }

        mdb_apl_log_end();
    }
}


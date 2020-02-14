/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUConfig.cpp
 * @brief       config file for mdbapmanage process
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include <Poco/Dynamic/Var.h>
#include <yaml-cpp/yaml.h>
#include "MDBException.h"
#include "MDBLogger.h"
#include "FAMSDUConfig.h"

namespace famsdatautil {
    const std::string FAMSDUConfig::DATA_UTILIZATION_CONF { "/opt/FJSVmapdb/etc/fams_data_util.conf" };
    const std::string FAMSDUConfig::TEMPLATE_CONF_KEY {"templateConfig"};
    const std::string FAMSDUConfig::DEFAULT_TEMPLATE_CONF {"/opt/FJSVmapdb/etc/fams_data_util_api.yaml"};
    const std::string FAMSDUConfig::ERROR_MESSAGE_CONF {"/opt/FJSVmapdb/etc/fams_error_messages.yaml"};

    FAMSDUConfig::FAMSDUConfig(const std::string &configPath) :
        mdbcom::MDBHTTPServerConfig { configPath }, mTemplateConfig {DEFAULT_TEMPLATE_CONF}
    {
        mdb_apl_log_start();
        auto errorKeyList = std::vector<std::string> {}; /* pgr0745 */
        auto cfgVal = std::string {}; /* pgr0745 */
        auto ret = mConfigFile->getValue(TEMPLATE_CONF_KEY, cfgVal); /* pgr0745 */

        if ((mdbcom::Result::OK != ret) || (true == cfgVal.empty()))
        {
            errorKeyList.push_back(TEMPLATE_CONF_KEY);
        }
        else
        {
            mTemplateConfig = cfgVal;
        }

        for (auto &errorKey : errorKeyList)
        {
            auto tmpErrorString = errorKey.c_str(); /* pgr0745 */ /*pgr0039*/
            mdb_apl_log_critical("an exception occur at key %s, this key got default value", tmpErrorString);
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

} /* namespace famsdatautil */

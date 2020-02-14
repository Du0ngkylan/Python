/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUConfig.h
 * @brief       config file for Data Utilization process
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_CONFIG_H_
#define FAMS_DATA_UTIL_CONFIG_H_
#include <string>
#include <map>
#include <set>
#include <cinttypes>
#include "MDBHTTPServerConfig.h"
#include "FAMSDUCommon.h"

namespace famsdatautil {

    /*!
     * @class FAMSDUConfig
     * @brief Manage Data Utilization process configuration.
     */
    class FAMSDUConfig: public mdbcom::MDBHTTPServerConfig {/*pgr2252*/
    public:

        /*!
         * @brief Destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDUConfig(void) = default;

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUConfig(const FAMSDUConfig &) = delete; /*pgr2222*/ /*pgr0571*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUConfig &operator=(const FAMSDUConfig &) = delete; /*pgr0055*//*pgr0309*//*pgr2223*/

        /*!
         * @brief Get an instance of FAMSDUConfig.
         * @param None.
         * @return Instance of FAMSDUConfig
         */

        inline static FAMSDUConfig &getInstance(void) {
            // Constructor with Data Utilization process configuration file path
            static FAMSDUConfig serverConfig(DATA_UTILIZATION_CONF);
            return serverConfig;
        }

        /*!
         * @brief get configuration file path
         * @param None.
         * @return Path to template configuration file.
         */
        inline const std::string &getTemplateConf(void) const {
            return mTemplateConfig;
        }

        inline std::string getErrorMessage(std::string key) {
            return mErrorMap[key];
        }

    private:
        //! Constant Data Utilization configure path
        static const std::string DATA_UTILIZATION_CONF;
        //! Constant key to template configuration file
        static const std::string TEMPLATE_CONF_KEY;
        //! Constant default value to template configuration file
        static const std::string DEFAULT_TEMPLATE_CONF;
        //! Constant value of error message configuration file
        static const std::string ERROR_MESSAGE_CONF;
        //! Constant path to template configuration file
        std::string mTemplateConfig;

        //! map to hold values having numeric integer type
        std::map<std::string, std::string> mErrorMap;

        /*!
         * @brief Constructor with config path.
         *
         * It will throw exception when config file or config format is abnormal.
         *
         * @param [in] configPath the configuration path.
         * @return None.
         */
        explicit FAMSDUConfig(const std::string &configPath);

    };

} /* namespace famsdatautil */

#endif /* FAMS_DATA_UTIL_CONFIG_H_ */


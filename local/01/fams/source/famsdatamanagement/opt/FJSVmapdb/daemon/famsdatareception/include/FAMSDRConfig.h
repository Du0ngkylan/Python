/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRConfig.h
 * @brief       Header file of class FAMSDRConfig.
 * @author      anhnguyen
 * @date        2018/12/11
 */

#ifndef FAMS_DATA_RECEPTION_CONFIG_H_
#define FAMS_DATA_RECEPTION_CONFIG_H_
#include "FAMSDRCommon.h"
#include "MDBHTTPServerConfig.h"
namespace famsdatareception {
    /*!
     * @class FAMSDRConfig
     * @brief Manage Data reception server application configuration.
     */
    class FAMSDRConfig: public mdbcom::MDBHTTPServerConfig {
    public:

        /*!
         * @brief Destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDRConfig(void) = default;

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRConfig(const FAMSDRConfig &) = delete; /*pgr2222*/ /*pgr0571*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRConfig &operator=(const FAMSDRConfig &) = delete; /*pgr0309*/ /*pgr0055*/ /*pgr2223*/

        /*!
         * @brief Get an instance of FAMSDRConfig.
         * @param None.
         * @return Instance of FAMSDRConfig
         */
        inline static FAMSDRConfig &getInstance(void) {
            // Constructor with data reception configuration file path
            static FAMSDRConfig serverConfig(APP_DATA_RECEPTION_SERVER_CONF);
            return serverConfig;
        }

        /*!
         * @brief get configuration file path for vehicle location request
         * @param None.
         * @return Path to template configuration file.
         */
        inline const std::string &getTemplateConf(void) const {
            return mStringValuesMap[TEMPLATE_CONF_KEY];
        }

        inline std::string getErrorMessage(std::string key) {
            return mErrorMap[key];
        }
        static const int EXECUTE;
        static const int NO_EXECUTE;
    private:
        //! template config key in config file
        static const std::string TEMPLATE_CONF_KEY;
        //! default path of YAML file
        static const std::string DEFAULT_TEMPLATE_CONF;

        //! yaml file for vehicle location
        static const std::string TEMPLATE_CONF;
        //! config file for data reception server
        static const std::string APP_DATA_RECEPTION_SERVER_CONF;
        //! Constant value of error message configuration file
        static const std::string ERROR_MESSAGE_CONF;
        static std::map<std::string, std::string> mStringValuesMap;
        std::map<std::string, std::string> mErrorMap;
        /*!
         * @brief Constructor with config path.
         * It will throw exception when config file or config format is abnormal.
         *
         * @param [in] configPath the configuration path.
         * @return None.
         */
        explicit FAMSDRConfig(const std::string &configPath);
    };

}

#endif /* FAMS_DATA_RECEPTION_CONFIG_H_ */

/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUAppServer.h
 * @brief       Data Utilization process file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMSDATA_UTILIZATIONSERVER_H_
#define FAMSDATA_UTILIZATIONSERVER_H_

#include <Poco/SingletonHolder.h>
#include "MDBHttpApplicationServer.h"
#include "FAMSDUConfig.h"

namespace famsdatautil {

    /*! @class FAMSDUAppServer
     *  @brief Web server for Data Utilization
     */
    class FAMSDUAppServer: public mdbcom::MDBHttpApplicationServer {/*pgr2252*/
    public:
        /*!
         * @brief Constructor.
         * @param [in] processName Name of process.
         * @return None.
         */
        explicit FAMSDUAppServer(const std::string &processName);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUAppServer(const FAMSDUAppServer &) = delete; /*pgr2222*/
        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDUAppServer &operator=(const FAMSDUAppServer &) = delete; /*pgr0055*//*pgr0309*/

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~FAMSDUAppServer(void);

        /*!
         * @brief Get web server config
         * @param None.
         * @return Server config
         */
        const FAMSDUConfig &getWebServerConfig(void); /* pgr2613 */

    protected:
        /*!
         * @brief The method does initialization when server is started.
         * @param None.
         * @retval true Initialize OK.
         * @retval false Initialize NG.
         */
        bool initializeApplication(void) override;

        /*!
         * @brief The method does un-initialization when process is stopped.
         * @param None.
         * @retval true uninitialize successfully.
         * @retval false uninitialize failed.
         */
        bool uninitializeApplication(void) override;

    };

} /* namespace MapDB */

#endif /* FAMSDATA_UTILIZATIONSERVER_H_ */

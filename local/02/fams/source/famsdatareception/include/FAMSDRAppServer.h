/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 *	@file        FAMSDRAppServer.h
 *	@brief       Header file for define FAMSDRAppServer class.
 *	@author      anhnguyen
 *	@date        2018/12/11
 */

#ifndef FAMS_DATA_RECEPTION_MANAGEMENT_SERVER_H_
#define FAMS_DATA_RECEPTION_MANAGEMENT_SERVER_H_

#include "MDBHttpApplicationServer.h"
#include "FAMSDRConfig.h"
#include "MDBIPCShm.h"

namespace famsdatareception {

    /*! @class FAMSDRAppServer
     *  @brief Web server for data reception management
     */
    class FAMSDRAppServer: public mdbcom::MDBHttpApplicationServer {
    public:
        /*!
         * @brief Constructor.
         * @param [in] processName Name of process.
         * @return None.
         */
        explicit FAMSDRAppServer(const std::string &processName);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRAppServer(const FAMSDRAppServer &) = delete; /*pgr0571*/ /*pgr2222*/
        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRAppServer &operator=(const FAMSDRAppServer &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~FAMSDRAppServer(void);

        /*!
         * @brief The method to get the configuration for HttpServer.
         * @param None.
         * @return Reference to configuration object
         */
        virtual const FAMSDRConfig &getWebServerConfig(void) override; /*pgr2613*/

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

    private:
        //! Data reception process name
        static const std::string DATA_RECEPTION_SERVER_PROCESS_NAME;

        //! Shared memory name
        static std::string mDataReceptionShmName;

        //! IPC shared memory pointer
        std::unique_ptr<mdbcom::MDBIPCShm> mSharedQueue;

    };

}

#endif /* FAMS_DATA_RECEPTION_MANAGEMENT_SERVER_H_ */

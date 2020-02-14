/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPApplication.h
 * @brief       Header file of class FAMSEPApplication.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_EVENT_PROCESSING_MANAGEMENT_H_
#define FAMS_EVENT_PROCESSING_MANAGEMENT_H_

#include <atomic>
#include <Poco/Net/HTTPRequest.h>

#include "MDBContentDecodingFormat.h"
#include "MDBIPCShm.h"
#include "MDBApplication.h"
#include "MDBJsonParser.h"
#include "MDBThreadDatabaseConfig.h"

namespace famseventprocessing {
    /*!
     * @class FAMSEPApplication
     * @brief information management
     */
    class FAMSEPApplication: public mdbcom::MDBApplication {
    public:
        /*!
         * @brief Constructor
         * @param [in] processName Calling process
         * @return None.
         */
        explicit FAMSEPApplication(const std::string &processName);

        /*!
         * @brief Destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSEPApplication(void) = default;

        /*!
         * @brief Copy constructor, using default
         * @param None.
         * @return None.
         */
        FAMSEPApplication(const FAMSEPApplication &) = default; /* pgr0571 */ /* pgr2222 */

        /*!
         * @brief Copy assignment, using default
         * @param None.
         * @return This DMAPVehicleInforManagement
         */
        FAMSEPApplication &operator=(const FAMSEPApplication &) = default; /* pgr0055 */ /* pgr0309 */ /* pgr2223 */

         /*!
         * @brief Sig RealTime handler for the Vehicle Process
         *
         * @param [in] signum the signum
         * @param [in] args argument list when handler is called (received SIG REALTIME)
         * @return return code of handler processing.
         */
        int handleSigRealTime(const int signum, const void *arg);
        /*!
         * @brief thread function of reception thread
         * @param [in] args input parameter
         * @return None.
         */
        static void eventHandler(void *args);

        /*!
         * @brief thread function of obstacle detection processing threads
         * @param [in] args input parameter
         * @return None.
         */
        static void sensorHandler(void *args);

        /*!
         * @brief thread function of obstacle detection processing threads
         * @param [in] args input parameter
         * @return None.
         */

    protected:
        /*!
         * @brief The method does initialization when server is started.
         * @param None.
         * @retval true initialization process is succeeded.
         * @retval false initialization failed.
         */

        bool initializeApplication(void) override;

        /*!
         * @brief The method does uninitialization
         * @param None.
         * @retval true uninitialize successfully.
         * @retval false uninitialize failed.
         *
         */
        bool uninitializeApplication(void) override;

        /*!
         * @brief The method perform processing logic of process
         * @param None.
         * @return mdbcom::ExitCode::OK on successed otherwise return error code.
         * @return mdbcom::ExitCode::NG on failed.
         */
        int execute(const std::vector<std::string> &args) override;

    private:

        //! status of main thread
        static std::atomic_bool isExecThreadEnd;

        //! ending mutex
        static std::mutex execThreadEndMutex;

        //!The maximum time that process wait for all thread to end (milliseconds).
        static const unsigned int WAIT_FOR_TERMINATING_TIMER = 10000;

        //! IPC shared memory name of queue between datareception and eventprocessing process
        static const std::string mEventReceptionShmName;
        //! IPC shared memory name of queue for sensor threads
        static const std::string mSensorShmName;

        //! IPC shared memory pointer of Reception
        static std::unique_ptr<mdbcom::MDBIPCShm> mEventReceptionSharedQueue;
        //! IPC shared memory pointer of sensor processing
        static std::unique_ptr<mdbcom::MDBIPCShm> mSensorSharedQueue;

        //! Thread initialization OK or NG
        bool isInitThreadsNormal;

        void getDbInfo(const std::string &threadName, std::vector<mdbcom::MDBThreadDatabaseConfig> &dbConfig,
                       mdbcom::ThreadFunction &threadFunction);
    };
}

#endif /* FAMS_EVENT_PROCESSING_MANAGEMENT_H_ */

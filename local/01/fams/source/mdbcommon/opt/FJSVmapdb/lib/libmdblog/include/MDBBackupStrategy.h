/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBBackupStrategy.h
 * @brief   Extend moving rotated log function
 * @author  cuongadp
 * @date    2016/12/01
 */

#ifndef LIBMDB_MDBBACKUPSTRATEGY_H_
#define LIBMDB_MDBBACKUPSTRATEGY_H_

#include <string>
#include <memory>
#include <thread>
#include <Poco/NotificationQueue.h>

#include "MDBBackupInfo.h"

namespace mdbcom {
    /*! @class MDBBackupStrategy
     *  @brief Data for communication with backup thread
     */
    class MDBBackupStrategy { /*pgr2205*/
    public:
        /*!
         * @brief Start backup thread
         *
         * Start asynchronous thread to execute backup to storage folder
         *
         * @return Result of starting thread
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static int startBackupThread(void);

        /*!
         * @brief Stop backup thread
         *
         * Stop asynchronous thread to execute backup to storage folder
         *
         * @return Result of stopping thread
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static int stopBackupThread(void);

        /*!
         * @brief Constructor
         *
         * Constructor for MDBBackupStrategy
         *
         * @param [in] source Log file or log folder
         * @param [in] storeDir Storage directory for rotated log file
         * @param None
         * @return None
         */
        MDBBackupStrategy(const std::string &source, const std::string &storeDir);

        /*!
         * @brief Default Destructor
         *
         * Call default Destructor of MDBBackupStrategy
         *
         * @param None
         * @return None
         */
        ~MDBBackupStrategy(void) = default;

        /*!
         * @brief Perform backup in async
         *
         * If backup thread is not started, nothing will happen.
         *
         * @param None
         * @return None
         */
        void backupAsync(void);

        /*!
         * @brief Perform backup in synchronous
         *
         * @param None
         * @return None
         */
        void backupSync(void);
    private:
        //! Constant used for sleepNano
        static constexpr int SLEEP_1_MS = 1000000;

        //! Thread to rotate file to storage directory
        static std::thread backupThread;

        //! Message queue of backup thread
        static Poco::NotificationQueue waitQueue;

        /*!
         * @brief Internal implementaion of backup process
         *
         * @param [in] info Backup information
         *
         * @param None
         * @return None
         */
        static void backupImpl(const MDBBackupInfo &info);

        //! Back up location informations
        std::shared_ptr<MDBBackupInfo> backupInfo;
    };
}

#endif

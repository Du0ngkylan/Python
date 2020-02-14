/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBFileChannel.h
 * @brief   Extend function of FileChannel
 * @author  trunghn:
 * @date    2016/10/10
 */

#ifndef LIBMDBLOG_MDBFILECHANNEL_H_
#define LIBMDBLOG_MDBFILECHANNEL_H_

#include <Poco/FileChannel.h>
#include <Poco/Message.h>

#include "MDBBackupStrategy.h"

namespace mdbcom {
    /*! @class MDBFileChannel
     *  @brief Extended FileChannel class that support rotation
     *         to different path
     */
    class MDBFileChannel: public Poco::FileChannel { /* pgr2252 */
    public:
        /*!
         * @brief Start rotation thread
         *
         * Start asynchronous thread to execute rotate to storage folder
         *
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static inline int startBackupThread(void) {
            return MDBBackupStrategy::startBackupThread();
        }

        /*!
         * @brief Stop rotation thread
         *
         * Stop asynchronous thread to execute rotate to storage folder
         *
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static inline int stopBackupThread(void) {
            return MDBBackupStrategy::stopBackupThread();
        }

        /*!
         * @brief Constructor with file path
         *
         * Initialize FileChannel to specified path
         *
         * @param [in] size Maximum log size
         * @param [in] path Log file path
         * @param [in] storeDir Directory to store rotated files
         * @return None
         */
        MDBFileChannel(unsigned int size, const std::string &path, const std::string &storeDir);

        /*!
         * @brief Default destructor
         *
         * @param None
         * @return None
         */
        ~MDBFileChannel(void) = default;

        /*!
         * @brief Log message to file
         *
         * Output message to file
         *
         * @param [in] msg Message structure
         * @return None
         */
        void log(const Poco::Message &msg);

        /**
         * @brief Copy constructor
         *
         * @param None.
         * @return None.
         */
        MDBFileChannel(const MDBFileChannel &) = delete; /* pgr2222 */ /* pgr0571 */

        /**
         * @brief Copy Assignment
         *
         * @param None.
         * @return None.
         */
        MDBFileChannel &operator =(const MDBFileChannel &) = delete; /* pgr0055 *//* pgr2223 *//* pgr0309 */ /* pgr2252 */

    private:
        //! Default setting value for archive prop
        static const std::string PROP_ARCHIVE_VALUE;

        //! Default setting value for times prop
        static const std::string PROP_TIMES_VALUE;

        //! Max size of each log file
        unsigned long long maxSize;

        //! Handle moving rotated file
        MDBBackupStrategy backupStrategy;
    };

}

#endif

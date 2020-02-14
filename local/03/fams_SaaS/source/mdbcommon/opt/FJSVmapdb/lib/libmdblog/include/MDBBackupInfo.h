/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBBackupInfo.h
 * @brief   Storing backup infomation for MDBBackupStrategy
 * @author  cuongadp
 * @date    2016/12/1
 */

#ifndef LIBMDB_MDBBACKUPINFO_H_
#define LIBMDB_MDBBACKUPINFO_H_

#include <string>

namespace mdbcom {
    /*! @class MDBBackupInfo
     *  @brief Data for communication with backup thread
     */
    class MDBBackupInfo {
    public:
        /*!
         * @brief Data constructor
         *
         * Call constructor with log information
         *
         * @param [in] source Source log files.
         * @param [in] storeDir Directory storing rotated log files.
         * @return None
         */
        MDBBackupInfo(const std::string &source, const std::string &storeDir);

        /*!
         * @brief Getter for storeDirectory
         *
         * @param None
         * @return Log source
         */
        inline const std::string &getLogSource(void) const {
            return logSource;
        };

        /*!
         * @brief Getter for storeDirectory
         *
         * @param None
         * @return Log storage directory
         */
        inline const std::string &getStoreDirectory(void) const {
            return storeDirectory;
        };

    private:
        //! Directory path for latest log file
        std::string logSource;

        //! Directory path to rotated log files
        std::string storeDirectory;
    };
}

#endif

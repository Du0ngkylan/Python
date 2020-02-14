/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBBackupInfo.cpp
 * @brief   Storing backup infomation for MDBBackupStrategy
 * @author  cuongadp
 * @date    2016/12/1
 */

#include "MDBBackupInfo.h"

namespace mdbcom {
    /*!
     * @brief Data constructor
     *
     * Call constructor with log information
     *
     * @param [in] source Source log files.
     * @param [in] storeDir Directory storing rotated log files.
     * @return None
     */
    MDBBackupInfo::MDBBackupInfo(const std::string &source, const std::string &storeDir) :
        logSource { source }, storeDirectory {storeDir}
    {

    }
}

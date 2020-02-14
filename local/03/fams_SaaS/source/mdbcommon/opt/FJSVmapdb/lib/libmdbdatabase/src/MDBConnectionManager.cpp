/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConnectionManager.cpp
 * @brief       Header file for define MDBConnectionManager information.
 * @author      cuongadp: create to support thread local db connection
 * @date        2016/10/26
 */

#include "MDBConnectionManager.h"
#include "MDBLogger.h"

namespace mdbcom {

    //! Thread connection for dynamic management database
    thread_local std::unique_ptr<const MDBDatabaseAccess> MDBConnectionManager::mDynamicDBConn
    { nullptr };

    /*!
     * @brief Operator= of MDBConnectionManager.
     * Unsupported
     * @param [in/out] conn Connection to be set.
     * @param [in/out] newConnection Connection to be set.
     * @return None.
     */
    int MDBConnectionManager::setConnection(std::unique_ptr<const MDBDatabaseAccess> &conn,
                                            std::unique_ptr<const MDBDatabaseAccess> &newConnection)
    {
        mdb_apl_log_start();

        if ((nullptr == conn) && (nullptr != newConnection))
        {
            conn = std::move(newConnection);
            mdb_apl_log_end();
            return Result::OK;
        }

        auto oldConn = conn.get(); /* pgr0745 */
        auto newConn = newConnection.get(); /* pgr0745 */
        mdb_apl_log_critical("Invalid parameter [oldConnection:%p], [newConnection:%p]", oldConn, newConn);
        mdb_apl_log_end();
        return Result::NG;
    }
}


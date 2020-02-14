/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConnectionManager.h
 * @brief       Header file for define MDBConnectionManager information.
 * @author      cuongadp: create to support thread local db connection
 * @date        2016/10/26
 */

#ifndef LIBMDBDB_MDBCONNECTIONMANAGER_H_
#define LIBMDBDB_MDBCONNECTIONMANAGER_H_

#include <memory>

#include "MDBLogger.h"
#include "MDBException.h"
#include "MDBDatabaseAccess.h"

namespace mdbcom {

    /*! @class  MDBConnectionManager.
     *  @brief The class supports manaagin
     */
    class MDBConnectionManager {
    public:

        /**
         * @brief Set connection for dynamic management db.
         *
         * @param [in/out] newConnection Pre initialized connection.
         * @retval Result::OK Success
         * @retval Result::NG Failure
         */
        static inline int setDynamicDBConnection(
            std::unique_ptr<const MDBDatabaseAccess> &newConnection) {
            return setConnection(mDynamicDBConn, newConnection);
        }

        /**
         * @brief Set connection for dynamic management db.
         *
         * @param [in] newConnection Pre initialized connection.
         * @retval Result::OK Success
         * @retval Result::NG Failure
         */
        static inline int setDynamicDBConnection(const MDBDatabaseAccess *newConnection) {
            std::unique_ptr<const MDBDatabaseAccess> holder { newConnection };
            return setConnection(mDynamicDBConn, holder);
        }

        /**
         * @brief Get connection for dynamic management db.
         *
         * @param None.
         * @return Database connection for calling thread.
         */
        static inline const MDBDatabaseAccess &getDynamicDBConnection(void) {
            if (nullptr == mDynamicDBConn) {
                mdb_apl_log_critical("Attemp getting uninit connection for dynamic management database");
                throw MDBException("Attemp getting uninit connection for dynamic management database");
            }

            return *mDynamicDBConn;
        }

        /**
         * @brief Check resource
         *
         * Check if current thread is using Dynamic database
         *
         * @param None.
         * @retval true  If using Dynamic database
         * @retval false If not using Dynamic database
         */
        static inline bool hasDynamicDB(void) {
            return (nullptr != mDynamicDBConn);
        }

        /*!
         * @brief Default constructor of MDBConnectionManager.
         * @param None.
         * @return None.
         */
        MDBConnectionManager(void) = delete;

        /*!
         * @brief Default destructor of MDBConnectionManager.
         * @param None.
         * @return None.
         */
        ~MDBConnectionManager(void) = delete;

        /*!
         * @brief Copy constructor of MDBConnectionManager.
         * Unsupported
         * @param None.
         * @return None.
         */
        MDBConnectionManager(const MDBConnectionManager &) = delete; /*pgr2222*/ /*pgr0571*/

        /*!
         * @brief Operator= of MDBConnectionManager.
         * Unsupported
         * @param None.
         * @return None.
         */
        MDBConnectionManager &operator=(const MDBConnectionManager &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

    private:

        //! Thread connection for Dynamic management database
        static thread_local std::unique_ptr<const MDBDatabaseAccess> mDynamicDBConn;

        /*!
         * @brief Operator= of MDBConnectionManager.
         * Unsupported
         * @param [in/out] conn Connection to be set.
         * @param [in/out] newConnection Connection to be set.
         * @return None.
         */
        static int setConnection(std::unique_ptr<const MDBDatabaseAccess> &conn,
                                 std::unique_ptr<const MDBDatabaseAccess> &newConnection);

    };
}

#endif /* LIBMDBDB_MDBCONNECTIONMANAGER_H_ */

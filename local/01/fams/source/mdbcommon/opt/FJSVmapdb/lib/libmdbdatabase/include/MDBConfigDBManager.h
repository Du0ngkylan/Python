/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConfigDBManager.h
 * @brief       Header file for define MDBConfigDBManager class.
 * @author      trangvp: create it for support MDBConfigDBManager code.
 * @date        2016/09/24
 */

#ifndef LIBMDBDB_MDBCONFIGDBMANAGER_H_
#define LIBMDBDB_MDBCONFIGDBMANAGER_H_

#include <Poco/RegularExpression.h>
#include "MDBConnectionInfo.h"

namespace mdbcom {
    /*! @class  MDBConfigDBManager.
     *  @brief The class supports managing database config
     */
    class MDBConfigDBManager {
    public:
        /*!
         * @brief A constructor of MDBConfigDBManager with path of config file
         *
         * @param [in] dbConfigPath path of config file
         * @return None.
         */
        explicit MDBConfigDBManager(const std::string &dbConfigPath);

        /*!
         * @brief Copy constructor of MDBConfigDBManager.
         * @param None.
         * @return None.
         */
        MDBConfigDBManager(const MDBConfigDBManager &) = default; /*pgr2222*/ /*pgr0571*/

        /*!
         * @brief Default destructor of MDBConfigDBManager.
         * @param None.
         * @return None.
         */
        ~MDBConfigDBManager(void) = default;

        /*!
         * @brief Operator=.
         * @param None.
         * @return This MDBConfigDBManager.
         */
        MDBConfigDBManager &operator=(const MDBConfigDBManager &) = default; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief get connection information.
         *
         * @param None.
         *
         * @return Connection information.
         */
        MDBConnectionInfo getConnectionConfig(void) const;

    private:
        //! constant default port
        static constexpr unsigned short DB_PORT_DEFAULT = 5432;

        //! constant minimum port range
        static constexpr unsigned short DB_PORT_MIN_RANGE = 1024;

        //! constant maximum port range
        static constexpr unsigned short DB_PORT_MAX_RANGE = 32767;

        //! constant for host key
        static const std::string DB_HOST_KEY;

        //! constant for port key
        static const std::string DB_PORT_KEY;

        //! constant for user name key
        static const std::string DB_USERNAME_KEY;

        //! constant for password key
        static const std::string DB_PASSWORD_KEY;

        //! constant for database name key
        static const std::string DB_DBNAME_KEY;

        //! constant for retry number key
        static const std::string DB_ACCESS_RETRY_NUMBER_KEY;

        //! constant for access waiting time key
        static const std::string DB_ACCESS_WAITING_TIME_KEY;

        //! constant for connection time out key
        static const std::string DB_CONNECTION_TIME_OUT_KEY;

        //! store config data
        MDBConnectionInfo connInfo;

        /*!
         * @brief Validate SQL identifier - simple rule
         * @param [in] name the identifier
         * @retval true match
         * @retval false not match
         */
        inline bool isValidSQLIdentifier(const std::string &name) {
            Poco::RegularExpression regex("^[a-zA-Z_][a-zA-Z0-9_\\$]*$");
            return regex.match(name);
        }
    };
}

#endif /* LIBMDBDB_MDBCONFIGDBMANAGER_H_ */

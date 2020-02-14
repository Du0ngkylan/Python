/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConnectionInfo.h
 * @brief       Header file for define MDBConnectionInfo class.
 * @author      trangvp: create it for support MDBConnectionInfo code.
 * @date        2016/10/26
 */

#ifndef LIBMDBDB_MDBCONNECTIONINFO_H_
#define LIBMDBDB_MDBCONNECTIONINFO_H_

#include <string>

namespace mdbcom {
    /*!
     * @class MDBConnectionInfo
     * @brief Connection information DTO
     */
    class MDBConnectionInfo {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBConnectionInfo(void);

        /*!
         * @brief Constructor.
         * @param [in] other the other MDBConnectionInfo.
         * @return None.
         */
        MDBConnectionInfo(const MDBConnectionInfo &other);

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBConnectionInfo(void) = default;

        /*!
         * @brief Operator=.
         * @param [in] other the other MDBConnectionInfo.
         * @return the MDBConnectionInfo.
         */
        MDBConnectionInfo &operator=(const MDBConnectionInfo &other);

        /*!
         * @brief get username
         * @param None.
         * @return username
         */
        inline const std::string &getUsername(void) const {
            return mUsername;
        }

        /*!
         * @brief get password
         * @param None.
         * @return user's password
         */
        inline const std::string &getPassword(void) const {
            return mPassword;
        }

        /*!
         * @brief get db name
         * @param None.
         * @return database name
         */
        inline const std::string &getDbname(void) const {
            return mDbname;
        }

        /*!
         * @brief get host
         * @param None.
         * @return host string
         */
        inline const std::string &getHost(void) const {
            return mHost;
        }

        /*!
         * @brief get port number
         * @param None.
         * @return port number
         */
        inline unsigned int getPort(void) const {
            return mPort;
        }

        /*!
         * @brief get access retry number
         * @param None.
         * @return access retry number
         */
        inline unsigned int getAccessRetryNumber(void) const {
            return mAccessRetryNumber;
        }

        /*!
         * @brief get access waiting time
         * @param None.
         * @return access waiting time (in miliseconds)
         */
        inline unsigned int getAccessWaitingTime(void) const {
            return mAccessWaitingTime;
        }

        /*!
         * @brief get connection time out number
         * @param None.
         * @return connection time out (in seconds)
         */
        inline unsigned int getConnectionTimeOut(void) const {
            return mConnectionTimeOut;
        }

        /*!
         * @brief Set username
         * @param [in] username the username.
         * @return None.
         */
        inline void setUsername(const std::string &username) {
            mUsername = username;
        }

        /*!
         * @brief Set password
         * @param [in] password the user password.
         * @return None.
         */
        inline void setPassword(const std::string &password) {
            mPassword = password;
        }

        /*!
         * @brief Set database name
         * @param [in] dbname the database name.
         * @return None.
         */
        inline void setDbname(const std::string &dbname) {
            mDbname = dbname;
        }

        /*!
         * @brief Set database host
         * @param [in] host the database host.
         * @return None.
         */
        inline void setHost(const std::string &host) {
            mHost = host;
        }

        /*!
         * @brief Set database port
         * @param [in] port the database port.
         * @return None.
         */
        inline void setPort(unsigned int port) {
            mPort = port;
        }

        /*!
         * @brief Set database accessRetryNumber
         * @param [in] accessRetryNumber the database accessRetryNumber.
         * @return None.
         */
        inline void setAccessRetryNumber(unsigned int accessRetryNumber) {
            mAccessRetryNumber = accessRetryNumber;
        }

        /*!
         * @brief Set database accessWaitingTime
         * @param [in] accessWaitingTime the database accessWaitingTime.
         * @return None.
         */
        inline void setAccessWaitingTime(unsigned int accessWaitingTime) {
            mAccessWaitingTime = accessWaitingTime;
        }

        /*!
         * @brief Set database connectionTimeOut
         * @param [in] connectionTimeOut the database connectionTimeOut.
         * @return None.
         */
        inline void setConnectionTimeOut(unsigned int connectionTimeOut) {
            mConnectionTimeOut = connectionTimeOut;
        }

    private:
        //! username to access database
        std::string mUsername;

        //! password of username
        std::string mPassword;

        //! database name to work with
        std::string mDbname;

        //! host to database
        std::string mHost;

        //!port to database
        unsigned int mPort;

        //! retry time
        unsigned int mAccessRetryNumber;

        //! time to retry
        unsigned int mAccessWaitingTime;

        //! connection TimeOut
        unsigned int mConnectionTimeOut;
    };
}

#endif /* LIBMDBDB_MDBCONNECTIONINFO_H_ */

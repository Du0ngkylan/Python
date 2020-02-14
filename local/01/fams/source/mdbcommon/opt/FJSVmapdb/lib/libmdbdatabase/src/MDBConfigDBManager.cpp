/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConfigDBManager.cpp
 * @brief       Source file for implementing MDBConfigDBManager information.
 * @author      trangvp: create it for support MDBConfigDBManager code.
 * @date        2016/09/24
 */

#include <string>

#include <Poco/Dynamic/Var.h>
#include <Poco/Net/IPAddress.h>
#include <Poco/Net/NetException.h>

#include "MDBConfigDBManager.h"
#include "MDBLogger.h"
#include "MDBConfigFile.h"
#include "MDBConstants.h"
#include "MDBException.h"

namespace mdbcom {
    // Constant
    const std::string MDBConfigDBManager::DB_HOST_KEY
    {
        "host"
    };
    const std::string MDBConfigDBManager::DB_PORT_KEY { "port" };
    const std::string MDBConfigDBManager::DB_USERNAME_KEY { "username" };
    const std::string MDBConfigDBManager::DB_PASSWORD_KEY { "password" };
    const std::string MDBConfigDBManager::DB_DBNAME_KEY { "dbname" };
    const std::string MDBConfigDBManager::DB_ACCESS_RETRY_NUMBER_KEY { "accessRetryNumber" };
    const std::string MDBConfigDBManager::DB_ACCESS_WAITING_TIME_KEY { "accessWaitingTime" };
    const std::string MDBConfigDBManager::DB_CONNECTION_TIME_OUT_KEY { "connectionTimeOut" };

    /*!
     * @brief A constructor of MDBConfigDBManager with path of config file
     *
     * @param [in] dbConfigPath path of config file
     * @return None.
     */
    MDBConfigDBManager::MDBConfigDBManager(const std::string &dbConfigPath) :
        connInfo { }
    {
        mdb_apl_log_start();

        try
        {
            auto parseConfig = MDBConfigFile {dbConfigPath}; /* pgr0745 */
            // Get Host key
            auto cfgVal = std::string {}; /* pgr0745 */
            auto ret = parseConfig.getValue(DB_HOST_KEY, cfgVal); /* pgr0745 */

            // Default host string is empty
            if (Result::OK == ret)
            {
                if ((true != cfgVal.empty()) && (Database::LOCAL_HOST_STRING != cfgVal))
                {
                    try
                    {
                        Poco::Net::IPAddress(cfgVal, Poco::Net::IPAddress::Family::IPv4);
                        connInfo.setHost(cfgVal);
                    }
                    catch (Poco::Net::InvalidAddressException &ex)
                    {
                        auto exDetail = ex.what(); /* pgr0745 */
                        mdb_apl_log_critical("Ipv4 validation is invalid:%s", exDetail);
                        throw MDBException {"Ipv4 validation is invalid"};
                    }
                    catch (...)
                    {
                        mdb_apl_log_critical("Ipv4 validation has unknown exception");
                        throw MDBException {"Ipv4 validation has unknown exception"};
                    }
                }
            }

            // get port key
            ret = parseConfig.getValue(DB_PORT_KEY, cfgVal);
            auto converter = Poco::Dynamic::Var {}; /* pgr0745 */
            // Default postgresql port is 5432
            auto port = DB_PORT_DEFAULT; /* pgr0745 */

            if ((Result::OK == ret) && (true != cfgVal.empty()))
            {
                try
                {
                    converter = cfgVal;
                    converter.convert(port);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Port number is invalid, use default port");
                }
            }

            // When port is out of range,
            if ((DB_PORT_MIN_RANGE > port) || (DB_PORT_MAX_RANGE < port))
            {
                port = DB_PORT_DEFAULT;
            }

            connInfo.setPort(port);
            // get username key - optional
            ret = parseConfig.getValue(DB_USERNAME_KEY, cfgVal);

            if (Result::OK == ret)
            {
                if (true != cfgVal.empty())
                {
                    if (true == isValidSQLIdentifier(cfgVal))
                    {
                        connInfo.setUsername(cfgVal);
                    }
                    else
                    {
                        throw MDBException {"Invalid username"};
                    }
                }
            }
            else
            {
                mdb_apl_log_warn("Username is not exist, use username that run application");
            }

            // get password key - optional
            ret = parseConfig.getValue(DB_PASSWORD_KEY, cfgVal);

            if (Result::OK == ret)
            {
                connInfo.setPassword(cfgVal);
            }
            else
            {
                mdb_apl_log_warn("Password is not exist, use empty password");
            }

            // get database name key - optional
            ret = parseConfig.getValue(DB_DBNAME_KEY, cfgVal);

            if (Result::OK == ret)
            {
                if (true != cfgVal.empty())
                {
                    if (true == isValidSQLIdentifier(cfgVal))
                    {
                        connInfo.setDbname(cfgVal);
                    }
                    else
                    {
                        throw MDBException {"Invalid dbname"};
                    }
                }
            }
            else
            {
                mdb_apl_log_warn("Dbname is not exist, use name same with user name");
            }

            // get re-connect DB times
            ret = parseConfig.getValue(DB_ACCESS_RETRY_NUMBER_KEY, cfgVal);
            // Default access retry number is 1 follow basic design
            auto accessRetryNumber = 1u; /* pgr0745 */

            if ((Result::OK == ret) && (true != cfgVal.empty()))
            {
                try
                {
                    converter = cfgVal;
                    converter.convert(accessRetryNumber);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Retry number is invalid, use default value is 2 times");
                }
            }

            connInfo.setAccessRetryNumber(accessRetryNumber);
            // get waiting time before re-connecting
            ret = parseConfig.getValue(DB_ACCESS_WAITING_TIME_KEY, cfgVal);
            // Default value of access waiting time is 0 miliseconds
            auto accessWaitingTime = 0u; /* pgr0745 */

            if ((Result::OK == ret) && (true != cfgVal.empty()))
            {
                try
                {
                    converter = cfgVal;
                    converter.convert(accessWaitingTime);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Access waiting time before reconnecting is invalid, use default value is 0 ms");
                }
            }

            connInfo.setAccessWaitingTime(accessWaitingTime);
            // get connection timeout key
            ret = parseConfig.getValue(DB_CONNECTION_TIME_OUT_KEY, cfgVal);
            // Default value of connection timeout is 0 second
            auto connectionTimeOut = 0u; /* pgr0745 */

            if ((Result::OK == ret) && (true != cfgVal.empty()))
            {
                try
                {
                    converter = cfgVal;
                    converter.convert(connectionTimeOut);
                }
                catch (...)
                {
                    mdb_apl_log_critical("connectionTimeOut is invalid");
                }
            }
            else
            {
                mdb_apl_log_critical("connectionTimeOut is invalid");
            }

            // When connection is invalid, throw exception
            if (0 == connectionTimeOut)
            {
                throw MDBException("connectionTimeOut is invalid");
            }

            connInfo.setConnectionTimeOut(connectionTimeOut);
        }
        catch (const MDBException &ex)
        {
            auto error = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("MDBConfigDBManager %s", error);
            mdb_apl_log_end();
            throw MDBException {ex};
        }
        catch (...)
        {
            mdb_apl_log_critical("MDBConfigDBManager unknown exception");
            mdb_apl_log_end();
            throw MDBException {"MDBConfigDBManager unknown exception"};
        }

        mdb_apl_log_end();
    }

    /*!
     * @brief get connection information.
     *
     * @param None.
     *
     * @return Connection information.
     */
    MDBConnectionInfo MDBConfigDBManager::getConnectionConfig() const
    {
        return connInfo;
    }
}

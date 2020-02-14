/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDatabaseAccess.cpp
 * @brief       Source file for implementing Connection information.
 * @author      trangvp: create it for support Connection code.
 * @date        2016/09/24
 */

#include "MDBDatabaseAccess.h"
#include "MDBLogger.h"
#include "MDBConfigDBManager.h"
#include "MDBCommonUtility.h"
#include "MDBCryptoUtility.h"
#include "MDBException.h"
#include "MDBNetworkUtility.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {
    /*! @class  Impl.
     *  @brief The class supports interacting with database.
     */
    class MDBDatabaseAccess::Impl {
    public:
        /*!
         * @brief Constructor of Impl with config path.
         * @param [in] configPath the config path.
         * @return None.
         */
        explicit Impl(const std::string &configPath);

        /**
         * @brief Copy constructor.
         *  This is unsupported
         * @param None.
         * @return None.
         */
        Impl(const Impl &) = delete; /*pgr2222*/ /*pgr0571*/

        /**
         * @brief Copy assignment.
         *  This is unsupported
         * @param None.
         * @return This Impl.
         */
        Impl &operator=(const Impl &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief send vector of prepare statement.
         *
         * @param [in] preparedStatementVec vector of statement.
         *
         * @retval Result::OK prepare a query successfully.
         * @retval Result::NG prepare a query failed.
         */
        int prepareAll(const PrepareStatements &preparedStatementVec);
        /*!
         * @brief execute a query with statement name and input params.
         *
         * @param [in] stmName statement name.
         * @param [in] params input params.
         *
         * @return a share pointer MDBStatement statement result and data.
         */
        inline std::shared_ptr<MDBStatement> execute(const std::string &stmName,
                const std::vector<MDBDatabaseParam> &params = std::vector<MDBDatabaseParam>()) const {
            // get the maximum number to retry
            auto retryNumberMax = mConnInfo.getAccessRetryNumber(); /* pgr0745 */
            return executeInternal(stmName, params, retryNumberMax);
        }

        /*!
         * @brief execute a query with statement name and input params at transactional mode.
         *
         * @param [in] stmName statement name.
         * @param [in] params input params.
         *
         * @return a share pointer MDBStatement statement result and data.
         */
        inline std::shared_ptr<MDBStatement> executeWithTransaction(const std::string &stmName,
                const std::vector<MDBDatabaseParam> &params = std::vector<MDBDatabaseParam>()) const {
            // Do not retry when executing sql query at transactional mode.
            return executeInternal(stmName, params);
        }

        /*!
         * @brief Get connection state
         * @param None.
         * @retval Result::NG Connection is disconnected.
         * @retval Result::OK Connection is connected.
         */
        inline int getConnectionState(void) const {
            auto status = PQstatus(const_cast<PGconn *>(pgConnection)); /* pgr0745 */

            if (CONNECTION_OK != status) {
                return Result::NG;
            }

            return Result::OK;
        }

        /*!
         * @brief get the connection information
         *
         * This method is used to get the connection information.
         * @param None.
         * @return  The connection information
         */
        inline const MDBConnectionInfo &getConnInfo() const {
            return mConnInfo;
        }

        /*!
         * @brief set the connection information
         *
         * This method is used to set the connection information.
         * @param [in] connInfo The connection information.
         * @return  None.
         */
        inline void setConnInfo(const MDBConnectionInfo &connInfo) {
            mConnInfo = connInfo;
        }

        /*!
         * @brief get the prepare statement vector
         *
         * This method is used to get the prepare statement vector.
         * @param None.
         * @return  The prepare statement vector
         */
        inline const PrepareStatements &getPreparedStatementVec() const {
            return mPreparedStatementVec;
        }

        /*!
         * @brief set the  prepare statement vector
         *
         * This method is used to set the prepare statement vector
         * @param [in] preparedStatementVec The prepare statement vector.
         * @return  None.
         */
        inline void setPreparedStatementVec(const PrepareStatements &preparedStatementVec) { /* pgr0304 */
            mPreparedStatementVec = preparedStatementVec;
        }

        /*!
         * @brief get the PG connection
         *
         * This method is used to get the PG connection.
         * @param None.
         * @return  The PG connection
         */
        inline const PGconn *getPgConnection() const {
            return pgConnection;
        }

        /*!
         * @brief set the  PG connection
         *
         * This method is used to set the PG connection
         * @param [in] preparedStatementVec The PG connection.
         * @return  None.
         */
        inline void setPgConnection(const PGconn *pgConnection) {
            this->pgConnection = pgConnection;
        }

        /*!
         * @brief Execute an internal SQL command.
         * @param [in] sqlCommand the SQL command
         * @param [in] retryNumberMax the retry number if need
         * @param [out] errCode sql error code of postgresql
         *
         * @retval Result::OK execute a command successfully.
         * @retval Result::NG execute a command failed.
         */
        int executeTransactionCommand(const std::string &sqlCommand,
                                      std::string &errCode,
                                      unsigned int retryNumberMax = 0) const;

        /*!
         * @brief execute a query with statement name and input params.
         *
         * @param [in] execString sql statement or statement name was prepared.
         * @param [in] params input params.
         * @param [in] retryNumberMax the retry number if need
         * @param [in] isPrepared this is true if execString is statement name
         *
         * @return a share pointer MDBStatement statement result and data.
         */
        std::shared_ptr<MDBStatement> executeInternal(const std::string &execString,
                const std::vector<MDBDatabaseParam> &params = std::vector<MDBDatabaseParam>(),
                unsigned int retryNumberMax = 0, bool isPrepared = true) const;

        /*!
         * @brief Reset Impl and re-send prepared statements.
         * @param None.
         * @retval Result::OK Reconnect and resend prepareStatements successfully.
         * @retval Result::NG Reconnect or resend prepareStatements failed.
         */
        int reset(void) const;

    private:
        //! Postgresql connection string length for reserve
        static size_t MDB_POSTGRESQL_CONNECTION_STRING_LEN;
        //! Constant local host IP
        static const std::string DB_LOCAL_HOST_IP;

        //! PostgreSQL Error Codes that syslog will not be outputted
        static std::vector<std::string> mWarningErrorCodes;

        //! Impl information to database
        MDBConnectionInfo mConnInfo;

        //! The PGconn of libpq
        const PGconn *pgConnection; /*pgr0541*/

        //! preparedStatementVector
        PrepareStatements mPreparedStatementVec;

        /*!
         * @brief Helper to generate postgresql Impl string
         * @param None.
         * @return Impl string
         */
        std::string generateConnectionString(void);

        /*!
         * @brief Send prepare statements
         * @param None.
         * @retval Result::OK Send prepare statements successfully.
         * @retval Result::NG Send prepare statements failed.
         */
        int sendPrepareStatement(void) const;
    };

    size_t MDBDatabaseAccess::Impl::MDB_POSTGRESQL_CONNECTION_STRING_LEN { 100 };
    const std::string MDBDatabaseAccess::Impl::DB_LOCAL_HOST_IP { "127.0.0.1" };
    std::vector<std::string> MDBDatabaseAccess::Impl::mWarningErrorCodes { "23505" /*unique_violation*/,
            "22001" /*string_data_right_truncation*/,
            "02000" /*no_data*/,
            "02001" /*no_additional_dynamic_result_sets_returned*/,
            "01000" /*warning*/,
            "0100C" /*dynamic_result_sets_returned*/,
            "01008" /*implicit_zero_bit_padding*/,
            "01003" /*null_value_eliminated_in_set_function*/,
            "01007" /*privilege_not_granted*/,
            "01006" /*privilege_not_revoked*/,
            "01004" /*string_data_right_truncation*/,
            "01P01" /*deprecated_feature*/
                                                                         };

    //---------------------------- Impl-----------------------------

    MDBDatabaseAccess::Impl::Impl(const std::string &configPath) :
        mConnInfo { }, pgConnection { nullptr }, mPreparedStatementVec { }
    {
        mdb_apl_log_start();

        try
        {
            auto dbConfig = MDBConfigDBManager {configPath}; /* pgr0745 */
            setConnInfo(dbConfig.getConnectionConfig());
            // Connection DB with connection string via PQconnectdb function of libpq
            const auto &stdConnString = generateConnectionString(); /* pgr0745 */
            auto charConnString = stdConnString.c_str(); /* pgr0745 */
            pgConnection = PQconnectdb(charConnString);
            auto status = PQstatus(const_cast<PGconn *>(pgConnection)); /* pgr0745 */

            if (CONNECTION_OK != status)
            {
                auto sError = PQerrorMessage(const_cast<PGconn *>(pgConnection)); /* pgr0745 */
                mdb_apl_log_critical("Connect DB failed %s", sError);
                PQfinish(const_cast<PGconn *>(pgConnection));
                pgConnection = nullptr;
                throw MDBException {"Connect database failed"};
            }
        }
        catch (const MDBException &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical(exDetail);
            mdb_sys_log_out("F000501");
            mdb_apl_log_end();
            throw MDBException {ex};
        }
        catch (...)
        {
            mdb_apl_log_critical("Connect database failed by unknown exception");
            mdb_sys_log_out("F000501");
            mdb_apl_log_end();
            throw MDBException {"Unknown exception"};
        }

        mdb_apl_log_end();
    }

    int MDBDatabaseAccess::Impl::prepareAll(
        const PrepareStatements &preparedStatementVec)   /* pgr0304 */
    {
        mdb_apl_log_start();
        // Store prepared statement vector
        setPreparedStatementVec(preparedStatementVec);
        auto queryResult = Result::NG; /* pgr0745 */

        if (nullptr != pgConnection)
        {
            queryResult = Result::OK;
            // The retry counter to count
            auto retryCounter = 0u; /* pgr0745 */
            // The retry number maximum
            auto retryNumberMax = mConnInfo.getAccessRetryNumber(); /* pgr0745 */

            do
            {
                queryResult = sendPrepareStatement();

                if (Result::OK != queryResult)
                {
                    if (CONNECTION_OK != PQstatus(const_cast<PGconn *>(pgConnection)))
                    {
                        if (0 != retryNumberMax)
                        {
                            // Increase retry counter if SQL error and connection is bad
                            retryCounter++;
                            // SLeep for awhile before re-connecting (convert from milisecond to nanosecond)
                            auto secondAccessWaitingTime = static_cast<int>(mConnInfo.getAccessWaitingTime() / 1000); /* pgr0745 */
                            auto nanoAccessWaitingTime = (mConnInfo.getAccessWaitingTime() % 1000) * 1000000L; /* pgr0745 */

                            if (Result::OK != MDBCommonUtility::sleepNano(secondAccessWaitingTime, nanoAccessWaitingTime))
                            {
                                mdb_apl_log_warn("sleepNano failed: %d s %ld ns", secondAccessWaitingTime, nanoAccessWaitingTime);
                            }

                            // Re-connect DB
                            PQreset(const_cast<PGconn *>(pgConnection));
                        }
                        else
                        {
                            // When retryNumberMax = 0 <=> don't retry, need to exit loop
                            mdb_apl_log_critical("The retry times is 0");
                            mdb_sys_log_out("F000501");
                            break;
                        }
                    }
                    else
                    {
                        // Exit loop when SQL is NG (Connection is still OK)
                        mdb_apl_log_critical("The connection is OK but SQL has error");
                        mdb_sys_log_out("F000501");
                        break;
                    }
                }
                else
                {
                    // Exit loop when SQL is OK
                    break;
                }
            }
            while (retryCounter <= retryNumberMax);

            // When retry out, output syslog
            if (retryCounter > retryNumberMax)
            {
                mdb_apl_log_critical("Re-connect retried out");
                mdb_sys_log_out("F000501");
            }
        }

        mdb_apl_log_debug("queryResult=%d", queryResult);
        mdb_apl_log_end();
        return queryResult;
    }

    int MDBDatabaseAccess::Impl::executeTransactionCommand(
        const std::string &sqlCommand, std::string &errCode,
        unsigned int retryNumberMax) const
    {
        mdb_apl_log_start();
        auto queryResult = Result::NG; /* pgr0745 */
        auto tmpSqlCommand = sqlCommand.c_str(); /* pgr0745 */
        mdb_apl_log_info("Execute SQL: %s", tmpSqlCommand);

        if (nullptr != pgConnection)
        {
            // The counter to count retry number
            auto retryCounter = 0u; /* pgr0745 */

            do
            {
                queryResult = Result::OK;
                // Send begin command to start a transaction
                auto result = PQexec(const_cast<PGconn *>(pgConnection), tmpSqlCommand); /* pgr0745 */

                if (PGRES_COMMAND_OK != PQresultStatus(result))
                {
                    queryResult = Result::NG;
                    auto resultError = PQresultErrorMessage(result); /* pgr0745 */
                    auto details = PQresultErrorField(result, PG_DIAG_MESSAGE_DETAIL); /* pgr0745 */
                    mdb_apl_log_warn("SQL execution has an error: %s (details: %s)", resultError,
                                     ((nullptr == details) ? "none" : details));

                    auto state = PQresultErrorField(result, PG_DIAG_SQLSTATE); /* pgr0745 */

                    if (nullptr != state)
                    {
                        errCode = state;
                    }

                    PQclear(result);

                    if (CONNECTION_OK != PQstatus(const_cast<PGconn *>(pgConnection)))
                    {
                        queryResult = Result::EXT_NG;

                        if (0 != retryNumberMax)
                        {
                            // Increase retry counter
                            retryCounter++;
                            // SLeep for awhile before re-connecting (convert from milisecond to nanosecond)
                            auto secondAccessWaitingTime = static_cast<int>(mConnInfo.getAccessWaitingTime() / 1000); /* pgr0745 */
                            auto nanoAccessWaitingTime = (mConnInfo.getAccessWaitingTime() % 1000) * 1000000L; /* pgr0745 */

                            if (Result::OK != MDBCommonUtility::sleepNano(secondAccessWaitingTime, nanoAccessWaitingTime))
                            {
                                mdb_apl_log_warn("sleepNano failed: %d s %ld ns", secondAccessWaitingTime, nanoAccessWaitingTime);
                            }

                            // Re-connect with DB
                            if (Result::OK != reset())
                            {
                                mdb_apl_log_warn(" Can't reconnect to database");
                            }
                            else
                            {
                                mdb_apl_log_info(" Attempt to reset connection: success");
                            }
                        }
                        else
                        {
                            // When retryNumberMax is 0 <=> don't retry, need to exit loop
                            mdb_apl_log_critical("The retry times is 0");
                            mdb_sys_log_out("F000501");
                            PQclear(result);
                            break;
                        }
                    }
                    else
                    {
                        // When connection is OK and SQL error, exit loop
                        mdb_apl_log_critical("The connection is OK but SQL has error");
                        mdb_sys_log_out("F000501");
                        PQclear(result);
                        break;
                    }
                }
                else
                {
                    // Exit loop when SQL is OK
                    PQclear(result);
                    break;
                }
            }
            while (retryCounter <= retryNumberMax);  /* pgr0697 */

            // When retry out, output syslog
            if (retryCounter > retryNumberMax)
            {
                mdb_apl_log_critical("Re-connect retried out");
                mdb_sys_log_out("F000501");
            }
        }

        mdb_apl_log_debug("queryResult=%d", queryResult);
        mdb_apl_log_end();
        return queryResult;
    }

    std::shared_ptr<MDBStatement> MDBDatabaseAccess::Impl::executeInternal(
        const std::string &execString,
        const std::vector<MDBDatabaseParam> &params,
        unsigned int retryNumberMax, bool isPrepared) const
    {
        mdb_apl_log_start();
        auto result = static_cast<PGresult *>(nullptr); /* pgr0745 */
        auto retCode = Result::NG; /* pgr0745 */
        auto theExecString = execString.c_str(); /* pgr0745 */
        auto errCode = std::string { }; /* pgr0745 */
        // Number of parameter
        auto size = static_cast<unsigned int>(params.size()); /* pgr0745 */
        mdb_apl_log_info("Statement: %s, number of parameters: %d", theExecString, size);

        if (nullptr != pgConnection)
        {
            // The counter to count retry number
            auto retryCounter = 0u; /* pgr0745 */

            // array of const char* data
            auto paramData = std::unique_ptr<const char *[]>(nullptr); /* pgr0745 */ /*pgr0694*/
            // array of data length
            auto paramLength = std::unique_ptr<int[]>(nullptr); /* pgr0745 */ /*pgr0694*/
            // array of data format
            auto paramFormat = std::unique_ptr<int[]>(nullptr); /* pgr0745 */ /*pgr0694*/

            try
            {
                // Allocate memory
                if (0 != size)
                {
                    // array of const char* data
                    paramData = MDBMemoryUtility::make_unique<const char *[]>(size); /*pgr0694*/
                    // array of data length
                    paramLength = MDBMemoryUtility::make_unique<int[]>(size); /*pgr0694*/
                    // array of data format
                    paramFormat = MDBMemoryUtility::make_unique<int[]>(size); /*pgr0694*/

                    // Assign data from StringVector
                    for (auto i = 0u; i < size; i++)   /* pgr0745 */
                    {
                        paramData[i] = params[i].getParamValue();
                        paramLength[i] = params[i].getParamLength();
                        paramFormat[i] = params[i].getParamFormat();
                    }
                }

                do
                {
                    retCode = Result::OK;
                    // Pass parameters and execute prepared statement
                    auto pParamData = paramData.get(); /* pgr0745 */
                    auto pParamLength = paramLength.get(); /* pgr0745 */
                    auto pParamFormat = paramFormat.get(); /* pgr0745 */
                    long long startTime = std::chrono::duration_cast<std::chrono::milliseconds>
                                          (std::chrono::system_clock::now().time_since_epoch()).count(); /* pgr0745 */

                    if (true == isPrepared)
                    {
                        result = PQexecPrepared(const_cast<PGconn *>(pgConnection), theExecString, size,
                                                pParamData, pParamLength, pParamFormat, 0);
                    }
                    else
                    {
                        result = PQexecParams(const_cast<PGconn *>(pgConnection), theExecString, size, nullptr,
                                              pParamData, pParamLength, pParamFormat, 0);
                    }

                    long long endTime = std::chrono::duration_cast<std::chrono::milliseconds>
                                        (std::chrono::system_clock::now().time_since_epoch()).count(); /* pgr0745 */
                    auto execTime = endTime - startTime; /* pgr0745 */
                    mdb_apl_log_info("Excute time of SQL [%s]: %lld ms", theExecString, execTime);

                    auto pqExecCode = PQresultStatus(result); /* pgr0745 */

                    if ((PGRES_COMMAND_OK != pqExecCode) && (PGRES_TUPLES_OK != pqExecCode))
                    {
                        retCode = Result::NG;
                        auto theError = PQresultErrorMessage(result); /* pgr0745 */
                        auto details = PQresultErrorField(result, PG_DIAG_MESSAGE_DETAIL); /* pgr0745 */

                        mdb_apl_log_warn("SQL execution has an error execString=%s - %s (details: %s)", theExecString,
                                         theError, ((nullptr == details) ? "none" : details));

                        auto state = PQresultErrorField(result, PG_DIAG_SQLSTATE); /* pgr0745 */

                        if (nullptr != state)
                        {
                            errCode = state;
                        }

                        // Cleanup resource
                        PQclear(result);
                        result = nullptr;

                        if (CONNECTION_OK != PQstatus(const_cast<PGconn *>(pgConnection)))
                        {
                            retCode = Result::EXT_NG;

                            if (0 != retryNumberMax)
                            {
                                // Increase retry counter when SQL is error and connection is bad
                                retryCounter++;
                                // SLeep for awhile before re-connecting (convert from milisecond to nanosecond)
                                auto secondAccessWaitingTime = /* pgr0745 */
                                    static_cast<int>(mConnInfo.getAccessWaitingTime() / 1000); /* pgr0745 */
                                auto nanoAccessWaitingTime = (mConnInfo.getAccessWaitingTime() /* pgr0745 */
                                                              % 1000) * 1000000L;

                                if (Result::OK
                                        != MDBCommonUtility::sleepNano(secondAccessWaitingTime,
                                                                       nanoAccessWaitingTime))
                                {
                                    mdb_apl_log_warn("sleepNano failed: %d s %ld ns",
                                                     secondAccessWaitingTime, nanoAccessWaitingTime);
                                }

                                // Re-connect with DB
                                if (Result::OK != reset())
                                {
                                    mdb_apl_log_warn(" Can't reconnect to database");
                                }
                                else
                                {
                                    mdb_apl_log_info(" Attempt to reset connection: success");
                                }
                            }
                            else
                            {
                                // When retryNumberMax = 0 <=> don't retry, need to exit loop
                                mdb_apl_log_critical("The retry times is 0");
                                mdb_sys_log_out("F000501");
                                break;
                            }
                        }
                        else
                        {
                            // When connection is OK and SQL is error, exit loop
                            auto tmpErrCode = errCode.c_str(); /* pgr0745 */
                            auto isNormalErr = false; /* pgr0745 */

                            for (const auto &i : mWarningErrorCodes)
                            {
                                if (errCode == i)
                                {
                                    mdb_apl_log_warn("The connection is OK but SQL has warning! SQL ErrorCode=%s", tmpErrCode);
                                    isNormalErr = true;
                                    break;
                                }
                            }

                            if (false == isNormalErr)
                            {
                                mdb_apl_log_critical("The connection is OK but SQL has error! SQL ErrorCode=%s", tmpErrCode);
                                mdb_sys_log_out("F000501");
                            }

                            break;
                        }
                    }
                    else
                    {
                        // Exit loop when SQL is OK
                        break;
                    }
                }
                while (retryCounter <= retryNumberMax);  /* pgr0697 */

                // When retry out, output syslog
                if (retryCounter > retryNumberMax)
                {
                    mdb_apl_log_critical("Re-connect retried out");
                    mdb_sys_log_out("F000501");
                }
            }
            catch (const std::bad_alloc &ex)
            {
                retCode = Result::NG;
                auto exDetail = ex.what(); /* pgr0745 */
                mdb_apl_log_critical("Fail to allocate memory: %s", exDetail);
            }
            catch (...)
            {
                retCode = Result::NG;
                mdb_apl_log_critical("Unknown exception occurs");
            }
        }

        if (Result::OK == retCode)
        {
            auto qrCode = PQresultStatus(result); /* pgr0745 */

            if ((PGRES_COMMAND_OK != qrCode) && (PGRES_TUPLES_OK != qrCode))
            {
                retCode = Result::NG;
            }
        }

        mdb_apl_log_end();
        return std::make_shared < MDBStatement > (((nullptr == result) ? nullptr : result), retCode, errCode); /*pgr0014*//*pgr2711*/
    }

    int MDBDatabaseAccess::Impl::reset(void) const
    {
        mdb_apl_log_start();
        auto ret = Result::OK; /* pgr0745 */
        // Reset the connection, it is blocking call
        PQreset(const_cast<PGconn *>(pgConnection));

        // Re-send prepared statements to Database server after connection is OK
        if (CONNECTION_OK == PQstatus(const_cast<PGconn *>(pgConnection)))
        {
            ret = sendPrepareStatement();

            if (Result::OK != ret)
            {
                mdb_apl_log_critical("Fail to prepare all statement");
            }
        }
        else
        {
            ret = Result::EXT_NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    std::string MDBDatabaseAccess::Impl::generateConnectionString(void)
    {
        mdb_apl_log_start();
        // Connection string
        auto connectionString = std::string {}; /* pgr0745 */
        connectionString.reserve(MDB_POSTGRESQL_CONNECTION_STRING_LEN);
        // Convert host string to lower case before comparing
        auto host = mConnInfo.getHost(); /* pgr0745 */
        mConnInfo.setHost(Poco::toLowerInPlace(host));
        // update value of host
        host = mConnInfo.getHost();
        auto isLocalhost = MDBNetworkUtility::checkIPInNetworkInterfaces(host); /* pgr0745 */

        // If host is empty or localhost connection(or IP of this host), don't add host into connection string
        if ((true != isLocalhost) && (false == host.empty()) && (Database::LOCAL_HOST_STRING != host)
                && (DB_LOCAL_HOST_IP != host))
        {
            connectionString += "host=";
            connectionString += host;
            connectionString += " port=";
            connectionString += std::to_string(mConnInfo.getPort());
        }

        if (true != mConnInfo.getUsername().empty())
        {
            connectionString += " user=";
            connectionString += mConnInfo.getUsername();
        }

        if (true != mConnInfo.getPassword().empty())
        {
            connectionString += " password=";
            auto decryptedPassword = std::string {}; /* pgr0745 */

            if (Result::OK != MDBCryptoUtility::decryptAES256(mConnInfo.getPassword(), decryptedPassword))
            {
                auto tmpDecrypedPassword = decryptedPassword.c_str(); /* pgr0745 */
                mdb_apl_log_critical("Fail to decrypt password: %s", tmpDecrypedPassword);
                mdb_apl_log_end();
                throw MDBException {"Decrypt DB password failed"};
            }

            connectionString += decryptedPassword;
        }

        if (true != mConnInfo.getDbname().empty())
        {
            connectionString += " dbname=";
            connectionString += mConnInfo.getDbname();
        }

        auto timeOut = std::to_string(mConnInfo.getConnectionTimeOut()); /* pgr0745 */
        connectionString += " connect_timeout=";
        connectionString += timeOut;
        mdb_apl_log_end();
        return connectionString;
    }

    int MDBDatabaseAccess::Impl::sendPrepareStatement(void) const
    {
        mdb_apl_log_start();
        auto queryResult = Result::OK; /* pgr0745 */

        // Retrieve vector elements to send prepare statement
        for (size_t i = 0; i < getPreparedStatementVec().size(); i++) /* pgr0745 */ /*pgr0694*/
        {
            // Send prepare statement to Database server
            auto strSqlQuery = getPreparedStatementVec()[i].getSqlQuery().c_str(); /* pgr0745 */
            auto strStmName = getPreparedStatementVec()[i].getStmName().c_str(); /* pgr0745 */
            auto result = PQprepare(const_cast<PGconn *>(pgConnection), strStmName, strSqlQuery, /* pgr0745 */
                                    getPreparedStatementVec()[i].getParamNumber(), nullptr);

            // Output syslog when SQL is error
            if (PGRES_COMMAND_OK != PQresultStatus(result))
            {
                auto resultError = PQresultErrorMessage(result); /* pgr0745 */
                auto details = PQresultErrorField(result, PG_DIAG_MESSAGE_DETAIL); /* pgr0745 */

                mdb_apl_log_warn("SQL execution has an error: %s (details: %s)", resultError,
                                 ((nullptr == details) ? "none" : details));
                queryResult = Result::NG;

                if (CONNECTION_OK != PQstatus(const_cast<PGconn *>(pgConnection)))
                {
                    queryResult = Result::EXT_NG;
                }

                // Cleanup resource
                PQclear(result);
                break;
            }
            else
            {
                // Cleanup resource
                PQclear(result);
            }
        }

        mdb_apl_log_end();
        return queryResult;
    }

    //---------------------------- MDBDatabaseAccess-----------------------------

    /*!
     * @brief Default destructor of Connection.
     * @param None.
     * @return None.
     */
    MDBDatabaseAccess::~MDBDatabaseAccess(void)
    {
        // Cleanup resource
        PQfinish(const_cast<PGconn *>(pImpl->getPgConnection()));
        pImpl->setPgConnection(nullptr);
    }

    /*!
     * @brief Constructor of MDBDatabaseAccess with config path.
     * @param [in] configPath  Config path
     * @return None.
     */
    MDBDatabaseAccess::MDBDatabaseAccess(const std::string &configPath) :
        pImpl { MDBMemoryUtility::make_unique<Impl>(configPath) }
    {
    }

    /*!
     * @brief send vector of prepare statement.
     *
     * @param [in] preparedStatementVec vector of statement.
     *
     * @retval Result::OK prepare a query successfully.
     * @retval Result::NG prepare a query failed.
     */
    int MDBDatabaseAccess::prepareAll(const PrepareStatements &preparedStatementVec)
    {
        return pImpl->prepareAll(preparedStatementVec);
    }

    /*!
     * @brief Start a transaction.
     * @param None.
     * @retval Result::NG start a transaction failed.
     * @retval Result::OK start a transaction successfully.
     */
    int MDBDatabaseAccess::startTransaction(void) const
    {
        // Int case of transaction, do not need to retry so inputting number of retry
        auto errCode = std::string {}; /* pgr0745 */
        auto ret = pImpl->executeTransactionCommand("BEGIN;", errCode, 1); /* pgr0745 */
        return ret;
    }

    /*!
     * @brief Start a transaction.
     * @param [out] errCode Postgresql error code (https://www.postgresql.org/docs/9.2/static/errcodes-appendix.html)
     * @retval Result::NG start a transaction failed.
     * @retval Result::OK start a transaction successfully.
     */
    int MDBDatabaseAccess::startTransaction(std::string &errCode) const /*pgr2227*/
    {
        // Int case of transaction, do not need to retry so inputting number of retry
        auto ret = pImpl->executeTransactionCommand("BEGIN;", errCode, 1); /* pgr0745 */
        return ret;
    }

    /*!
     * @brief commit a transaction.
     *
     * @return result of commit transaction.
     * @retval Result::OK commit a transaction successfully.
     * @retval Result::NG commit a transaction failed.
     */
    int MDBDatabaseAccess::commit(void) const
    {
        auto errCode = std::string {}; /* pgr0745 */
        auto ret = pImpl->executeTransactionCommand("COMMIT;", errCode); /* pgr0745 */
        return ret;
    }

    /*!
     * @brief commit a transaction.
     *
     * @param [out] errCode Postgresql error code (https://www.postgresql.org/docs/9.2/static/errcodes-appendix.html)
     *
     * @return result of commit transaction.
     * @retval Result::OK commit a transaction successfully.
     * @retval Result::NG commit a transaction failed.
     */
    int MDBDatabaseAccess::commit(std::string &errCode) const /*pgr2227*/
    {
        auto ret = pImpl->executeTransactionCommand("COMMIT;", errCode); /* pgr0745 */
        return ret;
    }

    /*!
     * @brief execute a SQL (not prepared before).
     *
     * @param [in] sql the sql statement to excute.
     * @param [out] error output error
     *
     * @return a share pointer MDBStatement statement result and data.
     */
    std::shared_ptr<MDBStatement> MDBDatabaseAccess::executeSQL(const std::string &sql,
            const std::vector<MDBDatabaseParam> &params) const /*pgr2227*/
    {
        // get the maximum number to retry
        auto retryNumberMax = pImpl->getConnInfo().getAccessRetryNumber(); /* pgr0745 */
        return pImpl->executeInternal(sql, params, retryNumberMax, false);
    }
    /*!
     * @brief execute a query with statement name and input params.
     *
     * @param [in] stmName statement name.
     * @param [in] params input params.
     *
     * @return a share pointer MDBStatement statement result and data.
     */
    std::shared_ptr<MDBStatement> MDBDatabaseAccess::execute(const std::string &stmName,
            const std::vector<MDBDatabaseParam> &params) const
    {
        // get the maximum number to retry
        auto retryNumberMax = pImpl->getConnInfo().getAccessRetryNumber(); /* pgr0745 */
        return pImpl->executeInternal(stmName, params, retryNumberMax);
    }

    /*!
     * @brief execute a query with statement name and input params at transactional mode.
     *
     * @param [in] stmName statement name.
     * @param [in] params input params.
     *
     * @return a share pointer MDBStatement statement result and data.
     */
    std::shared_ptr<MDBStatement> MDBDatabaseAccess::executeWithTransaction(
        const std::string &stmName, const std::vector<MDBDatabaseParam> &params) const
    {
        // Do not retry when executing sql query at transactional mode.
        return pImpl->executeInternal(stmName, params);
    }

    /*!
     * @brief execute a rollback.
     * @param None.
     *
     * @retval Result::OK execute a rollback query successfully.
     * @retval Result::NG execute a rollback failed.
     */
    int MDBDatabaseAccess::rollback(void) const
    {
        auto errCode = std::string {}; /* pgr0745 */
        auto ret = pImpl->executeTransactionCommand("ROLLBACK;", errCode); /* pgr0745 */
        return ret;
    }

    /*!
     * @brief execute a rollback.
     * @param [out] errCode Postgresql error code (https://www.postgresql.org/docs/9.2/static/errcodes-appendix.html)
     *
     * @retval Result::OK execute a rollback query successfully.
     * @retval Result::NG execute a rollback failed.
     */
    int MDBDatabaseAccess::rollback(std::string &errCode) const /*pgr2227*/
    {
        auto ret = pImpl->executeTransactionCommand("ROLLBACK;", errCode); /* pgr0745 */
        return ret;
    }

    /*!
     * @brief Get connection state
     * @param None.
     * @retval Result::NG Connection is disconnected.
     * @retval Result::OK Connection is connected.
     */
    int MDBDatabaseAccess::getConnectionState(void) const
    {
        return pImpl->getConnectionState();
    }
    /*!
     * @brief Reconnect MDBDatabaseAccess and re-send prepared statements.
     * @param None.
     * @retval Result::OK Reconnect and resend prepareStatements successfully.
     * @retval Result::NG Reconnect or resend prepareStatements failed.
     */
    int MDBDatabaseAccess::reset(void) const
    {
        return pImpl->reset();
    }
}

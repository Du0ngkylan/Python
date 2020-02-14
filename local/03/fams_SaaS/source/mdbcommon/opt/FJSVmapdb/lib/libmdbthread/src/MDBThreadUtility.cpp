/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBThreadUtility.cpp
 * @brief       Source code for defined utility functions.
 * @author      trangvp: create it for supporting thread utility class.
 * @date        2016/10/04
 */

#include <iostream>
#include <thread>
#include <system_error>

#include "MDBThreadUtility.h"
#include "MDBMemoryUtility.h"
#include "MDBLogger.h"
#include "MDBConstants.h"
#include "MDBDynamicDatabaseAccess.h"
#include "MDBConnectionManager.h"

namespace mdbcom {

    /**
     * @brief Thread's function for startThread
     *
     * processing additional step before executing actual work:
     *  - init log
     *  - init db connections
     *
     * @param [in] args Thread start argument
     * @retval nullptr Always return nullptr
     */
    void *MDBThreadUtility::threadFunctionWrapper(void *args)
    {
        if (nullptr == args)
        {
            mdb_sys_log_out("F000601", "Normal Thread");
            return nullptr;
        }

        auto threadId = std::this_thread::get_id(); /* pgr0745 */
        auto arguments = reinterpret_cast<MDBThreadStartArguments *>(args); /* pgr0745 */
        auto param = std::unique_ptr < MDBThreadStartArguments > (arguments); /* pgr0745 */

        // init log
        if (Result::OK != MDBLogger::openDebugLog(param->getThreadName()))   /* pgr0689 */
        {
            mdb_sys_log_out("F000601", "Normal Thread");
            param->setThreadResult(Result::NG);
            return nullptr;
        }

        mdb_apl_log_start();

        auto connType = ConnectionType::NONE; /* pgr0745 */

        for (auto &database : param->getDatabaseInfo())   /* pgr0745 */
        {
            auto connection = std::unique_ptr<MDBDatabaseAccess> { nullptr }; /* pgr0745 */
            connType = database.getConnType();

            try
            {
                switch (connType)
                {
                    case ConnectionType::DYNAMIC:
                        connection = MDBMemoryUtility::make_unique<MDBDynamicDatabaseAccess>();
                        break;

                    default:
                        // no db connection
                        break;
                }
            }
            catch (std::bad_alloc &)
            {
                mdb_apl_log_critical("Can't create db connection for thread [%llu]", threadId);
                mdb_apl_log_end();
                param->setThreadResult(Result::NG);
                return nullptr;
            }
            catch (...)
            {
                mdb_apl_log_critical("Unknown error creating db connection for thread [%llu]",
                                     threadId);
                mdb_apl_log_end();
                param->setThreadResult(Result::NG);
                return nullptr;
            }

            auto &prep = database.getPrepareStatements(); /* pgr0745 */

            // call prepare statement
            if ((nullptr != connection) && (Result::OK != connection->prepareAll(prep)))
            {
                mdb_apl_log_critical(" Can't prepare statements for thread: [%llu]", threadId);
                param->setThreadResult(Result::NG);
            }

            auto ret = Result::OK; /* pgr0745 */

            switch (connType)
            {
                case ConnectionType::DYNAMIC:
                    ret = MDBConnectionManager::setDynamicDBConnection(connection.release());
                    break;

                default:
                    // no db connection
                    break;
            }

            if (Result::OK != ret)
            {
                mdb_apl_log_critical("Can't set database connection for thread: [%llu]", threadId);
                mdb_apl_log_end();
                param->setThreadResult(Result::NG);
                return nullptr;
            }
        }

        auto arg = const_cast<void *>(param->getArguments()); /* pgr0745 */
        auto function = param->getThreadFunction(); /* pgr0745 */
        param->setThreadResult(Result::OK);
        // release to save memory
        param.reset();

        // execute user's thread function
        if (nullptr != function)
        {
            static_cast<void>((function)(arg));
        }
        else
        {
            mdb_apl_log_critical(" Invalid parameter for normal thread [type:%d][function:%p]",
                                 connType, function);
        }

        mdb_apl_log_end();
        return nullptr;
    }

    /**
     * @brief Start DB worker
     *
     * Start thread with normal DB connection
     *
     * @param [in] threadName  Name of module (function)
     * @param [in] threadFunction  Worker function to be executed
     * @param [in] databaseNum Number of database to be used in threa
     * @param [in] databaseInfo Database information to be used in thread
     * @param [in] arguments Arguments passing to user function
     * @param [in] isDetach Detach the created thread if true
     * @return Thread instance representing execution thread
     */
    int MDBThreadUtility::startThread(const std::string &threadName,
                                      ThreadFunction threadFunction, const std::vector<MDBThreadDatabaseConfig> &databaseInfo,
                                      void *arguments, bool isDetach, std::thread &threadObj)
    {
        mdb_apl_log_start();

        auto ret = Result::NG; /* pgr0745 */
        auto param = std::unique_ptr<MDBThreadStartArguments> {nullptr}; /* pgr0745 */

        try
        {
            param = MDBMemoryUtility::make_unique<MDBThreadStartArguments>(threadName, threadFunction,
                    databaseInfo, arguments); /* pgr0541 */
        }
        catch (std::bad_alloc &ex)
        {
            auto exStr = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("Can't allocate memory: %s", exStr);
            mdb_apl_log_end();
            return ret;
        }
        catch (...)
        {
            mdb_apl_log_critical("Unknown error occur while allocating memory");
            mdb_apl_log_end();
            return ret;
        }

        auto startRet = param->getThreadResult(); /* pgr0745 */

        try
        {
            threadObj = std::thread {threadFunctionWrapper, param.get()};
            static_cast<void>(param.release());
        }
        catch (const std::system_error &ex)
        {
            auto errMsg = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("can't start thread [%s]", errMsg);
            mdb_apl_log_end();
            return ret;
        }
        catch (...)
        {
            mdb_apl_log_critical("can't start thread");
            mdb_apl_log_end();
            return ret;
        }

        if (true == isDetach)
        {
            threadObj.detach();
        }

        // ensure that user function can be called by new thread.
        startRet.wait();
        mdb_apl_log_end();
        return startRet.get(); /* pgr0840 */
    }
}

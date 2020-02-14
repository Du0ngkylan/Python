/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBApplication.cpp
 * @brief       Header file to define common ServerApplication as a based class for MIoT server application.
 * @author      hunglt
 * @date        2016/12/12
 */
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>

#include "MDBLogger.h"
#include "MDBApplication.h"
#include "MDBConstants.h"
#include "MDBApplicationUtility.h"
#include "MDBSignalHandler.h"
namespace mdbcom {

    /*!
     * @brief Constructor
     * @param None.
     * @return None.
     */
    MDBApplication::MDBApplication(const std::string &processName) :
        mProcessName { processName }, isSignalHandlerStarted { false }
    {
    }

    /*!
     * @brief Do common logic for initialization
     * @param [in] isInitialized flag to check initialization is normally or not.
     * @retval true Un-initialization process successed
     * @retval false Un-initialization process fail
     */
    bool MDBApplication::uninitializeInternal(bool isInitialized)
    {
        auto ret = true; /* pgr0745 */

        if (true == isInitialized)
        {
            ret = uninitializeApplication();
        }

        // De-activate the SIG REALTIME handler before de-structor is called
        auto &signalHandler = MDBSignalHandler::getInstance(); /* pgr0745 */

        if (true == isSignalHandlerStarted)
        {
            MDBSignalHandlerFunctionInformation handleSigRealtimeManagement
            {
                [this](int signum, const void *usrData)
                {
                    return this->handleSigRealTime(signum, usrData);
                }, nullptr, false, true };

            auto retVal = signalHandler.updateHandleSignal(SIGRTMIN + 1, handleSigRealtimeManagement); /* pgr0745 */

            if (Signal::OK != retVal)
            {
                mdb_apl_log_critical("Failed to deactivate SIG REALTIME handler ");
                ret = false;
            }
            else
            {
                // Do nothing
            }

        }

        MDBApplicationUtility::unInitApplication();

        return ret;
    }

    /*!
     * @brief Do common logic for uninitialization
     * @param None.
     * @retval true Initialization process successed
     * @retval false Initialization process fail
     */
    bool MDBApplication::initializeInternal(void)
    {
        static_cast<void>(umask(0007));

        //! Init syslog, debug log, signal handler
        auto ret = MDBApplicationUtility::initApplication(mProcessName, /* pgr0745 */
                   [this](int signum, const void *usrData)
        {
            return this->handleSigRealTime(signum, usrData);
        }
                                                         );

        if (Result::OK != ret)
        {

            // Failed at debug log --> the signal handler is started successfully
            if (Result::START_ROTATE_LOG_FAILED == ret)
            {
                isSignalHandlerStarted = true;
            }

            return false;
        }
        else
        {
            isSignalHandlerStarted = true;
        }

        //! Initialization is OK
        return initializeApplication();
    }

    /*!
     * @brief default SIG REALTIME handler for MDBApplication (use SIG REALTIME 33)
     *
     * @param [in] signum the signum
     * @param [in] args argument list when handler is called (received SIG REALTIME)
     * @return return code of handler processing.
     */
    int MDBApplication::handleSigRealTime(const int signum __attribute__((unused)), /* pgr0364 */ /* pgr2201 */
                                          const void *arg __attribute__((unused)))   /* pgr0364 */
    {
        return Result::OK;
    }

    /*!
     * @brief The method to help daemon, cron main() thread waiting for signal
     * @param None.
     * @retval Signal::OK If success.
     * @retval Signal::ERROR_THREAD_JOIN_FAILED If "Signal Handler Thread" can not joined.
     */
    int MDBApplication::waitForSignalHandler()
    {

        // Wait SignalHandler thread ends (when receiving SIG REALTIME)
        auto ret = mdbcom::MDBSignalHandler::getInstance().waitForTermination(); /* pgr0745 */

        isSignalHandlerStarted = false;
        return ret;
    }

    /*!
     * @brief The main method that POCO will call when application is started.
     *
     * @param [in] args argument list when run application
     * @return return code of application
     */
    int MDBApplication::main(const std::vector<std::string> &args)
    {

        //! Initialization process
        auto ret = mdbcom::ExitCode::NG; /* pgr0745 */
        // Flag to check system initialize normally or not
        auto isInitialized = initializeInternal(); /* pgr0745 */

        if (true == isInitialized)
        {
            ret = execute(args);

            if (mdbcom::ExitCode::OK != ret)
            {
                mdb_apl_log_critical("Application exit with error");
            }

        }
        else
        {
            auto processName = mProcessName.c_str(); /* pgr0745 */
            mdb_sys_log_out("F000901", processName);
            mdb_apl_log_critical("Application failed to started");
        }

        //! deinitialization process
        if (false == uninitializeInternal(isInitialized))
        {
            mdb_apl_log_critical("Application uninitialize fail");
        }

        return ret;
    }
}

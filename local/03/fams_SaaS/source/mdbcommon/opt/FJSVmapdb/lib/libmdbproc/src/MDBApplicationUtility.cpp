/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBApplicationUtility.cpp
 * @brief       Source code for defined utility functions.
 * @author      trangvp: create it for supporting thread utility class.
 * @date        2016/10/04
 */

#include <iostream>

#include "MDBApplicationUtility.h"
#include "MDBLogger.h"
#include "MDBSignalHandler.h"
#include "MDBException.h"
#include "MDBConstants.h"

namespace mdbcom {
    //! Default name for main thread
    std::string MDBApplicationUtility::MAIN_THREAD_NAME {"main"};

    /*!
     * @brief  init resources for application
     *
     * processing additional step before executing actual work:
     *  - init log
     *  - open debug log for main thread of application.
     *  - register Sig Realtime handler
     *  - init and start "Signal handler Thread"
     *  - start rotating log
     *
     * @param [in] moduleName module name
     * @param [in] handleSigRealTime Sig RealTime handler
     *
     * @retval Result::OK init successfully
     * @retval Result::INIT_LOG_FAILED init log failed
     * @retval Result::OPEN_DEBUG_LOG_FAILED open debug log failed
     * @retval Result::START_SIGNAL_HANDLER_FAILED start signal handler failed
     * @retval Result::START_ROTATE_LOG_FAILED init start rotate log failed
     *
     */
    int MDBApplicationUtility::initApplication(const std::string &moduleName, HandleSignalFunction handleSigRealTime) /*pgr2227*/
    {
        if (Result::OK != MDBLogger::initLog(moduleName))
        {
            return Result::INIT_LOG_FAILED;
        }

        if (Result::OK != MDBLogger::openDebugLog(MAIN_THREAD_NAME))
        {
            return Result::OPEN_DEBUG_LOG_FAILED;
        }

        try
        {
            auto &signalHandler = MDBSignalHandler::getInstance(); /* pgr0745 */

            //Update signal handler: call handleSigRealTime when received SIG REALTIME
            if (nullptr != handleSigRealTime)
            {
                MDBSignalHandlerFunctionInformation handleSigRealtimeManagement { handleSigRealTime,
                        nullptr,
                        true,
                        true };
                auto ret = signalHandler.addHandleSignal(SIGRTMIN + 1, handleSigRealtimeManagement); /* pgr0745 */

                if (Signal::OK_THREAD_STARTED == ret)
                {
                    mdb_apl_log_critical("Signal Handler Thread started, can't add SIGT REALTIME handler ");
                    return Result::START_SIGNAL_HANDLER_FAILED;
                }
                else if (Signal::OK != ret)
                {
                    mdb_apl_log_critical("Failed to add SIG REALTIME handler ");
                    return Result::START_SIGNAL_HANDLER_FAILED;
                }
                else
                {
                    // Do nothing
                }
            }

            if (Signal::OK != signalHandler.startSignalHandlerThread())
            {
                mdb_apl_log_critical("Fail to start SignalHandler Thread ");
                return Result::START_SIGNAL_HANDLER_FAILED;
            }

        }
        catch (const MDBException &ex)
        {
            auto errMsg = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("Fail to initialize signal handler [%s]", errMsg);
            return Result::START_SIGNAL_HANDLER_FAILED;
        }
        catch (...)
        {
            mdb_apl_log_critical("Fail to initialize signal handler");
            return Result::START_SIGNAL_HANDLER_FAILED;
        }

        if (Result::OK != MDBLogger::startBackupThread())
        {
            mdb_apl_log_critical("Fail to start rotating log");
            return Result::START_ROTATE_LOG_FAILED;
        }

        return Result::OK;
    }

    /*!
     * @brief  uninit resources for application
     *
     * @return None.
     *
     */
    void MDBApplicationUtility::unInitApplication(void)
    {
        static_cast<void>(MDBLogger::stopBackupThread());
    }
}

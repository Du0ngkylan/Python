/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBBackupStrategy.h
 * @brief   Extend function of MDBBackupStrategy
 * @author  cuongadp
 * @date    2016/12/1
 */

#include <set>
#include <system_error>

#include <Poco/Glob.h>
#include <Poco/File.h>

#include "MDBLogger.h"
#include "MDBBackupStrategy.h"
#include "MDBCommonUtility.h"
#include "MDBMemoryUtility.h"
#include "MDBLogger.h"
#include "MDBMContextNotification.h"

namespace mdbcom {

    //! Asynchronous moving thread
    std::thread MDBBackupStrategy::backupThread;

    //! Message queue of backup thread
    Poco::NotificationQueue MDBBackupStrategy::waitQueue;

    /*!
     * @brief Constructor
     *
     * Constructor for MDBBackupStrategy
     *
     * @param [in] source Log file or log folder
     * @param [in] storeDir Storage directory for rotated log file
     * @param None
     * @return None
     */
    MDBBackupStrategy::MDBBackupStrategy(const std::string &source, const std::string &storeDir) :
        backupInfo { std::make_shared<MDBBackupInfo>(source, storeDir) }
    {
    }

    /*!
     * @brief Perform backup in async
     *
     * If backup thread is not started, nothing will happen.
     *
     * @param None
     * @return None
     */
    void MDBBackupStrategy::backupAsync(void)
    {
        if (true == backupThread.joinable())   /* pgr0685 */
        {
            auto noti = /* pgr0745 */
                MDBMemoryUtility::make_unique<MDBMContextNotification>(backupInfo);
            waitQueue.enqueueNotification(noti.release());
        }
    }

    /*!
     * @brief Internal implementaion of backup process
     *
     * @param [in] info Backup information
     *
     * @param None
     * @return None
     */
    void MDBBackupStrategy::backupImpl(const MDBBackupInfo &info)
    {
        auto basePath = info.getLogSource(); /* pgr0745 */
        auto baseFile = Poco::File { basePath }; /* pgr0745 */

        if (true == baseFile.isFile())
        {
            // move specific rotated files
            basePath += ".*";
        }
        else
        {
            // move whole folder
            basePath += "/*";
        }

        auto listFile = std::set<std::string> { }; /* pgr0745 */
        Poco::Glob::glob(basePath, listFile);

        auto fileIt = Poco::File { }; /* pgr0745 */

        for (auto &file : listFile)
        {
            fileIt = file;

            try
            {
                fileIt.moveTo(info.getStoreDirectory());
            }
            catch (Poco::FileAccessDeniedException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (Poco::FileNotFoundException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (Poco::OpenFileException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (Poco::FileReadOnlyException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (Poco::FileExistsException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (Poco::PathSyntaxException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (Poco::FileException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (Poco::IOException &ex)
            {
                auto message = ex.displayText();
                mdb_sys_log_out("F000701", message.c_str());
            }
            catch (...)
            {
                mdb_sys_log_out("F000701", "Unknown exception when rotate file");
            }
        }
    }

    /*!
     * @brief Start backup thread
     *
     * Start asynchronous thread to execute backup to storage folder
     *
     * @return Result of starting thread
     * @retval Result::OK success
     * @retval Result::NG failure
     */
    int MDBBackupStrategy::startBackupThread(void)
    {
        mdb_apl_log_start();

        if (false == backupThread.joinable())   /* pgr0685 */
        {
            auto function = []()   /* pgr0745 */
            {
                auto noti = Poco::AutoPtr<Poco::Notification> {MDBBackupStrategy::waitQueue.waitDequeueNotification()}; /* pgr0745 */

                while (nullptr != noti)
                {
                    auto context = dynamic_cast<MDBMContextNotification *>(noti.get()); /* pgr0745 */

                    if ((nullptr != context)
                            && (nullptr != context->get()))
                    {
                        auto &info = context->get(); /* pgr0745 */
                        MDBBackupStrategy::backupImpl(*info);

                        static_cast<void>(MDBCommonUtility::sleepNano(0, SLEEP_1_MS));
                    }
                    else
                    {
                        break;
                    }

                    noti = MDBBackupStrategy::waitQueue.waitDequeueNotification();
                }
            };

            try
            {
                backupThread = std::thread {function};
            }
            catch (const std::system_error &ex)
            {
                auto errorMessage = ex.what(); /* pgr0745 */
                mdb_apl_log_critical("Error creating backup thread [%s]", errorMessage);
                return Result::NG;
            }
            catch (...)
            {
                mdb_apl_log_critical("Unknown Error creating backup thread ");
                return Result::NG;
            }
        }
        else
        {
            mdb_apl_log_trace("Thread is already started");
        }

        mdb_apl_log_end();
        return Result::OK;
    }

    /*!
     * @brief Stop backup thread
     *
     * Stop asynchronous thread to execute backup to storage folder
     *
     * @return Result of stopping thread
     * @retval Result::OK success
     * @retval Result::NG failure
     */
    int MDBBackupStrategy::stopBackupThread(void)
    {
        mdb_apl_log_start();

        if (true == backupThread.joinable())   /* pgr0685 */
        {
            mdb_apl_log_trace("Waiting for backup thread");

            try
            {
                auto noti = MDBMemoryUtility::make_unique<MDBMContextNotification>(); /* pgr0745 */
                waitQueue.enqueueNotification(noti.release());
            }
            catch (const std::bad_alloc &ex)
            {
                auto errorMessage = ex.what(); /* pgr0745 */
                mdb_apl_log_critical("Error stopping backup thread [%s]", errorMessage);
                mdb_apl_log_end();
                return Result::NG; /* pgr0840 */
            }
            catch (...)
            {
                mdb_apl_log_critical("Unknown Error creating backup thread ");
                mdb_apl_log_end();
                return Result::NG; /* pgr0840 */
            }

            backupThread.join();
            mdb_apl_log_trace("Backup thread end");
            waitQueue.clear();
        }

        mdb_apl_log_end();
        return Result::OK; /* pgr0840 */
    }
}

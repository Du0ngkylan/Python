/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPApplication.cpp
 * @brief       Source file of class FAMSEPApplication.
 * @author      anhnguyen
 * @date        2018/12/12
 *
 */

#include <future>
#include <sstream>
#include <Poco/Base64Decoder.h>
#include "FAMSEPCommon.h"

#include "FAMSEPApplication.h"
#include "FAMSEPConfig.h"
#include "MDBCommonUtility.h"
#include "MDBConnectionManager.h"
#include "MDBThreadUtility.h"
#include "FAMSReceptionAndProcessingShmConfig.h"
#include "MDBYamlConfig.h"
#include "FAMSIpcHeader.h"
#include "FAMSEPWaterReplaceHandler.h"
#include "FAMSEPCisternHandler.h"
#include "FAMSEPNitrificationHandler.h"
#include "FAMSEPOutsideHandler.h"
#include "MDBSignalHandler.h"

namespace famseventprocessing {

    std::atomic_bool FAMSEPApplication::isExecThreadEnd { false };

    std::mutex FAMSEPApplication::execThreadEndMutex { };

    const std::string FAMSEPApplication::mEventReceptionShmName { "DataReception_EventProcessing" };

    const std::string FAMSEPApplication::mSensorShmName { "Processing_Sensor" };
    const std::string FAMSEPApplication::mCisternSensorShmName { "Cistern_Processing_Sensor" };
    const std::string FAMSEPApplication::mNitrificationSensorShmName { "Nitrification_Processing_Sensor" };
    const std::string FAMSEPApplication::mOutsideSensorShmName { "Outside_Processing_Sensor" };
    std::unique_ptr<mdbcom::MDBIPCShm> FAMSEPApplication::mEventReceptionSharedQueue
    { nullptr };

    std::unique_ptr<mdbcom::MDBIPCShm> FAMSEPApplication::mSensorSharedQueue
    { nullptr };
    std::unique_ptr<mdbcom::MDBIPCShm> FAMSEPApplication::mCisternSensorSharedQueue
    { nullptr };
    std::unique_ptr<mdbcom::MDBIPCShm> FAMSEPApplication::mNitrificationSensorSharedQueue
    { nullptr };
    std::unique_ptr<mdbcom::MDBIPCShm> FAMSEPApplication::mOutsideSensorSharedQueue
    { nullptr };
    /*!
     * @brief Constructor
     * @param None.
     * @return None.
     */
    FAMSEPApplication::FAMSEPApplication(const std::string &processName) :
        mdbcom::MDBApplication(processName), isInitThreadsNormal(true)
    {

    }

    /*!
     * @brief Sig RealTime handler for the Event Processing Process
     *
     * @param [in] signum the signum
     * @param [in] args argument list when handler is called (received SIG REALTIME)
     * @return return code of handler
     */
    int FAMSEPApplication::handleSigRealTime(const int signum __attribute__((unused)),
            const void *arg __attribute__((unused)))
    {
        mdb_apl_log_start();

        //! End reading shared memory.
        if (nullptr != mEventReceptionSharedQueue)
        {
            (mEventReceptionSharedQueue.get())->endRead();
        }

        if (nullptr != mSensorSharedQueue)
        {
            (mSensorSharedQueue.get())->endRead();
        }
        if (nullptr != mNitrificationSensorSharedQueue)
        {
            (mNitrificationSensorSharedQueue.get())->endRead();
        }

        if (nullptr != mOutsideSensorSharedQueue)
        {
            (mOutsideSensorSharedQueue.get())->endRead();
        }

        if (nullptr != mCisternSensorSharedQueue)
        {
            (mCisternSensorSharedQueue.get())->endRead();
        }

        mdb_apl_log_end();
        return mdbcom::Signal::OK;
    }

    /*!
     * @brief thread function of event reception thread
     * @param [in] args input parameter
     * @return None.
     */
    void FAMSEPApplication::eventHandler(void *args __attribute__((unused)))   /* pgr0304 */
    {
        mdb_apl_log_start();
        static_cast<void>(args);  //warning

        while (true)
        {
            auto messageSize = std::size_t {0};
            //! Read data from shared memory queue with Data Reception Management Server
            auto message = mEventReceptionSharedQueue->readData<famscommon::FAMSIpcHeader>(messageSize); /* pgr0745 */
            // Reading failed abnormally
            if (nullptr == message)
            {
                // detect system end
                // Reading terminated by User
                if (true == mEventReceptionSharedQueue->isEnded())
                {
                    mdb_apl_log_info("eventReceptionSharedMemory reading is stopped by signal");
                }
                else
                {
                    mdb_apl_log_critical("eventHandler read data error");
                }
                break;
            }
            mdb_apl_log_trace("Receive message with size %lu bytes", messageSize);
            //! Forward data to shared memory queue
            auto writeRetCode = mdbcom::Result::NG;/*pgr0745*/
            if (famscommon::MessageType::SENSOR_DATA == message->ipcType)
            {
                mdb_apl_log_trace("Forward SENSOR message ");
                auto messagePtr = message.get();/*pgr0745*/
                // sensor queue: FAMSIpcHeader part
                writeRetCode = mSensorSharedQueue->writeData(messagePtr, messageSize); // forward = false, data size = real size of data
            }
            else if (famscommon::MessageType::CISTERN_SENSOR_DATA == message->ipcType)
            {
                mdb_apl_log_trace("Forward Cistern SENSOR message ");
                auto messagePtr = message.get();/*pgr0745*/
                // sensor queue: FAMSIpcHeader part
                writeRetCode = mCisternSensorSharedQueue->writeData(messagePtr, messageSize); // forward = false, data size = real size of data
            }
            else if (famscommon::MessageType::NITRIFICATION_DATA == message->ipcType)
            {
                mdb_apl_log_trace("Forward Nitrification SENSOR message ");
                auto messagePtr = message.get();/*pgr0745*/
                // sensor queue: FAMSIpcHeader part
                writeRetCode = mNitrificationSensorSharedQueue->writeData(messagePtr, messageSize); // forward = false, data size = real size of data
            }
            else if (famscommon::MessageType::OUTSIDE_DATA == message->ipcType)
            {
                mdb_apl_log_trace("Forward Outside SENSOR message ");
                auto messagePtr = message.get();/*pgr0745*/
                // sensor queue: FAMSIpcHeader part
                writeRetCode = mOutsideSensorSharedQueue->writeData(messagePtr, messageSize); // forward = false, data size = real size of data
            }
            else
            {
                // Not supported in this phase
                // Add implementation if there are more type of data
                mdb_apl_log_error("eventHandler read wrong DataType");
            }
            // Write data to shared memory queue of deliveryInfor
            if (mdbcom::Result::OK != writeRetCode)
            {
                mdb_apl_log_critical("Write Data to shared memory failed, drop message");
            }

        }

        mdb_apl_log_end();
    }

    /*!
     * @brief thread function of obstacle detection handler threads.
     * @param [in] args input parameter
     * @return None.
     */
    void FAMSEPApplication::sensorCisternHandler(void *args)
    {
        mdb_apl_log_start();
        static_cast<void>(args);
        auto handler = FAMSEPCisternHandler {};/*pgr0745*/

        while (true)
        {
            auto messageSize = std::size_t { 0 };/*pgr0745*/
            auto message = mCisternSensorSharedQueue->readData<famscommon::FAMSIpcHeader>(messageSize); /* pgr0745 */
            // Reading failed abnormally
            if (nullptr == message)
            {
                // detect system end
                // Reading terminated by User
                if (true == mEventReceptionSharedQueue->isEnded())
                {
                    mdb_apl_log_info("obstacleDetectionSharedQueue reading is stopped by signal");
                }
                else
                {
                    mdb_apl_log_critical("obstacleDetectionHandler read data error");
                }

                break;
            }

            handler = message.get();
            if (mdbcom::Result::OK != handler.handleEvent())
            {
                mdb_apl_log_error("Failure processing message");
            }

        }
    }
    /*!
     * @brief thread function of obstacle detection handler threads.
     * @param [in] args input parameter
     * @return None.
     */
    void FAMSEPApplication::sensorOutsideHandler(void *args)
    {
        mdb_apl_log_start();
        static_cast<void>(args);
        auto handler = FAMSEPOutsideHandler {};/*pgr0745*/

        while (true)
        {
            auto messageSize = std::size_t { 0 };/*pgr0745*/
            auto message = mOutsideSensorSharedQueue->readData<famscommon::FAMSIpcHeader>(messageSize); /* pgr0745 */
            // Reading failed abnormally
            if (nullptr == message)
            {
                // detect system end
                // Reading terminated by User
                if (true == mEventReceptionSharedQueue->isEnded())
                {
                    mdb_apl_log_info("obstacleDetectionSharedQueue reading is stopped by signal");
                }
                else
                {
                    mdb_apl_log_critical("obstacleDetectionHandler read data error");
                }

                break;
            }

            handler = message.get();
            if (mdbcom::Result::OK != handler.handleEvent())
            {
                mdb_apl_log_error("Failure processing message");
            }

        }
    }
    /*!
     * @brief thread function of obstacle detection handler threads.
     * @param [in] args input parameter
     * @return None.
     */
    void FAMSEPApplication::sensorNitrificationnHandler(void *args)
    {
        mdb_apl_log_start();
        static_cast<void>(args);
        auto handler = FAMSEPNitrificationHandler{};/*pgr0745*/

        while (true)
        {
            auto messageSize = std::size_t { 0 };/*pgr0745*/
            auto message = mNitrificationSensorSharedQueue->readData<famscommon::FAMSIpcHeader>(messageSize); /* pgr0745 */
            // Reading failed abnormally
            if (nullptr == message)
            {
                // detect system end
                // Reading terminated by User
                if (true == mEventReceptionSharedQueue->isEnded())
                {
                    mdb_apl_log_info("obstacleDetectionSharedQueue reading is stopped by signal");
                }
                else
                {
                    mdb_apl_log_critical("obstacleDetectionHandler read data error");
                }

                break;
            }

            handler = message.get();
            if (mdbcom::Result::OK != handler.handleEvent())
            {
                mdb_apl_log_error("Failure processing message");
            }

        }
    }
    /*!
     * @brief thread function of obstacle detection handler threads.
     * @param [in] args input parameter
     * @return None.
     */
    void FAMSEPApplication::sensorHandler(void *args)
    {
        mdb_apl_log_start();
        static_cast<void>(args);
        auto handler = FAMSEPWaterReplaceHandler {};/*pgr0745*/

        while (true)
        {
            auto messageSize = std::size_t { 0 };/*pgr0745*/
            auto message = mSensorSharedQueue->readData<famscommon::FAMSIpcHeader>(messageSize); /* pgr0745 */
            // Reading failed abnormally
            if (nullptr == message)
            {
                // detect system end
                // Reading terminated by User
                if (true == mEventReceptionSharedQueue->isEnded())
                {
                    mdb_apl_log_info("obstacleDetectionSharedQueue reading is stopped by signal");
                }
                else
                {
                    mdb_apl_log_critical("obstacleDetectionHandler read data error");
                }

                break;
            }

            handler = message.get();
            if (mdbcom::Result::OK != handler.handleEvent())
            {
                mdb_apl_log_error("Failure processing message");
            }

        }
    }

    /*!
     * @brief The method does initialization when server is started.
     * @param None.
     * @retval true initialization process is succeeded.
     * @retval false initialization failed.
     */

    bool FAMSEPApplication::initializeApplication(void)
    {
        using mdbcom::MDBException;
        mdb_apl_log_start();
        mdb_apl_log_info("Event processing process is starting");

        auto ret = true; /* pgr0745 */
        std::string runningPosition;
        // Get Config instance
        auto &eventProcessingConfig = FAMSEPConfig::getInstance(); /* pgr0745 */

        try
        {
            auto receptionAndProcessingShmConf = famscommon::FAMSReceptionAndProcessingShmConfig {};/*pgr0745*/
            // Init for Shared memorys
            runningPosition = std::string{"initializing event reception shared queue"};
            mEventReceptionSharedQueue = mdbcom::MDBMemoryUtility::make_unique <
                                         mdbcom::MDBIPCShm > (mEventReceptionShmName,
                                                 receptionAndProcessingShmConf.getReceptionAndProcessingShmSize());
            runningPosition = std::string{"initializing mSensorSharedQueue shared queue"};
            mSensorSharedQueue = mdbcom::MDBMemoryUtility::make_unique <
                                           mdbcom::MDBIPCShm > (mSensorShmName,
                                                   eventProcessingConfig.getThreadsShmSize(SENSOR_THREAD_NAME));
            runningPosition = std::string{"initializing mCisternSensorSharedQueue shared queue"};
            mCisternSensorSharedQueue = mdbcom::MDBMemoryUtility::make_unique <
                                           mdbcom::MDBIPCShm > (mCisternSensorShmName,
                                                   eventProcessingConfig.getThreadsShmSize(CISTERN_SENSOR_THREAD_NAME));
            runningPosition = std::string{"initializing mOutsideSensorSharedQueue shared queue"};
            mOutsideSensorSharedQueue = mdbcom::MDBMemoryUtility::make_unique <
                                           mdbcom::MDBIPCShm > (mOutsideSensorShmName,
                                                   eventProcessingConfig.getThreadsShmSize(OUTSIDE_SENSOR_THREAD_NAME));
            runningPosition = std::string{"initializing mNitrificationSensorSharedQueue shared queue"};
            mNitrificationSensorSharedQueue = mdbcom::MDBMemoryUtility::make_unique <
                                           mdbcom::MDBIPCShm > (mNitrificationSensorShmName,
                                                   eventProcessingConfig.getThreadsShmSize(NITRIFICATION_SENSOR_THREAD_NAME));

        }
        catch (const std::bad_alloc &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("Bad alloc exception: %s when %s", exDetail, runningPosition.c_str());
            ret = false;
        }
        catch (const mdbcom::MDBException &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("Exception: %s when %s", exDetail, runningPosition.c_str());
            ret =  false;
        }
        catch (...)
        {
            mdb_apl_log_critical("Unknown exception when %s", runningPosition.c_str());
            ret = false;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief The method does un-initialization when process is stopped.
     * @param None.
     * @retval true uninitialize successfully.
     * @retval false uninitialize failed.
     */
    bool FAMSEPApplication::uninitializeApplication(void)
    {
        mdb_apl_log_start();

        // Only ouput syslog when thread is stop by signal
        if (true == isInitThreadsNormal)
        {
            mdb_apl_log_info("event processing process is stopped");
        }

        auto ret = true;/*pgr0745*/

        try
        {

            //! End reading shared memory.
            if (nullptr != mEventReceptionSharedQueue)
            {
                (mEventReceptionSharedQueue.get())->unlink();
            }

            if (nullptr != mSensorSharedQueue)
            {
                (mSensorSharedQueue.get())->unlink();
            }
            if (nullptr != mCisternSensorSharedQueue)
            {
                (mCisternSensorSharedQueue.get())->unlink();
            }
            if (nullptr != mNitrificationSensorSharedQueue)
            {
                (mNitrificationSensorSharedQueue.get())->unlink();
            }
            if (nullptr != mOutsideSensorSharedQueue)
            {
                (mOutsideSensorSharedQueue.get())->unlink();
            }

            mdb_apl_log_info("EventProcessingManagement is ended normally.");
            mdb_sys_log_out("F110202");

        }
        catch (const mdbcom::MDBException &ex)
        {
            //un-Initialize Error!
            ret = false;
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("EventProcessingManagement un-initialize is Failed!(%s)", exDetail);
            mdb_sys_log_out("F110213");
        }
        catch (...)
        {
            //Initialize Error!
            ret = false;
            mdb_apl_log_critical("EventProcessingManagement un-initialize Catched no-MDBException.");
            mdb_sys_log_out("F110213");
        }

        mdb_apl_log_end();
        return ret;
    }

    void FAMSEPApplication::getDbInfo(const std::string &threadName,
            std::vector<mdbcom::MDBThreadDatabaseConfig> &dbConfig, mdbcom::ThreadFunction &threadFunction)
    {
        // make a copy of common statements and add more statement for specific thread
        static auto commonStatements = famscommon::getCommonStatement(); /*pgr0745*/
        auto commonStatementsEndIt = commonStatements.end(); /*pgr0745*/

        if (SENSOR_THREAD_NAME == threadName)
        {
            commonStatements.insert(commonStatementsEndIt, sensorPreparedStatements.begin(), sensorPreparedStatements.end());
            dbConfig.emplace_back(commonStatements, mdbcom::ConnectionType::DYNAMIC);
            threadFunction = sensorHandler;
        }
        else if (CISTERN_SENSOR_THREAD_NAME == threadName)
        {
            commonStatements.insert(commonStatementsEndIt, sensorPreparedStatements.begin(), sensorPreparedStatements.end());
            dbConfig.emplace_back(commonStatements, mdbcom::ConnectionType::DYNAMIC);
            threadFunction = sensorCisternHandler;
        }
        else if (NITRIFICATION_SENSOR_THREAD_NAME == threadName)
        {
            commonStatements.insert(commonStatementsEndIt, sensorPreparedStatements.begin(), sensorPreparedStatements.end());
            dbConfig.emplace_back(commonStatements, mdbcom::ConnectionType::DYNAMIC);
            threadFunction = sensorNitrificationnHandler;
        }
        else if (OUTSIDE_SENSOR_THREAD_NAME == threadName)
        {
            commonStatements.insert(commonStatementsEndIt, sensorPreparedStatements.begin(), sensorPreparedStatements.end());
            dbConfig.emplace_back(commonStatements, mdbcom::ConnectionType::DYNAMIC);
            threadFunction = sensorOutsideHandler;
        }
        else
        {
            dbConfig.clear();
            threadFunction = eventHandler;
        }

    }

    /*!
     * @brief The method perform processing logic of process
     * @param None.
     * @return mdbcom::ExitCode::OK on successed otherwise return error code.
     * @return mdbcom::ExitCode::NG on failed.
     */
    int FAMSEPApplication::execute(const std::vector<std::string> &args __attribute__((unused)))
    {
        //! Syslog output "start"
        mdb_apl_log_start();

        auto retExecute = mdbcom::ExitCode::OK;/*pgr0745*/
        // Flag to check if there is failed-to-start thread
        auto isThreadStartFailed = false; /* pgr0745 */

        auto &eventProcessingConfig = FAMSEPConfig::getInstance(); /* pgr0745 */

        //! Init reception Threads
        const auto numberOfReceptionThreads = eventProcessingConfig.getThreadsNum(EVENT_RECEPTION_THREAD_NAME); /* pgr0745 */
        std::promise<bool> receptionPromise[numberOfReceptionThreads];
        std::future<bool> receptionFuture[numberOfReceptionThreads];
        std::thread receptionThread[numberOfReceptionThreads];

        mdb_apl_log_debug("starting all reception thread...");

        // Check if the threads executed.
        for (auto i = 0u; i < numberOfReceptionThreads; i++) /* pgr0745 *//* pgr0692 */
        {
            receptionFuture[i] = receptionPromise[i].get_future();

            auto ret = mdbcom::MDBThreadUtility::createWorkUnit( /* pgr0745 */
                           EVENT_RECEPTION_THREAD_NAME, eventHandler, &receptionPromise[i], receptionThread[i]);

            // Check if the thread executed.
            if (mdbcom::Result::OK != ret)
            {
                mdb_apl_log_critical("Can't create receptionHandler thread #%u", i);
                isThreadStartFailed = true;
                // Set promise value to prevent waiting for a non-executing thread
                receptionPromise[i].set_value(true);

                // Set promise value to prevent waiting for a non-executing thread
                for (auto j = i + 1; j < numberOfReceptionThreads; j++)
                {
                    receptionFuture[j] = receptionPromise[j].get_future();
                    receptionPromise[j].set_value(true);
                }
            }
        }

        mdb_apl_log_debug("starting all driving info thread...");

        auto commonStatements = famscommon::getCommonStatement(); /*pgr0745*/
        auto commonStatementsBeginIt = commonStatements.begin(); /*pgr0745*/
        auto commonStatementsEndIt = commonStatements.end(); /*pgr0745*/

        //! Init sensor Threads
        auto numberOfSensorThreads = eventProcessingConfig.getThreadsNum(SENSOR_THREAD_NAME); /* pgr0745 */
        std::promise<bool> sensorPromise[numberOfSensorThreads];
        std::future<bool> sensorFuture[numberOfSensorThreads];
        std::thread sensorThreads[numberOfSensorThreads];
        auto sensorDBConfig = std::vector<mdbcom::MDBThreadDatabaseConfig> { }; /* pgr0745 */
        auto sensorStatements = sensorPreparedStatements;
        mdb_apl_log_debug("starting all obstacle detection thread...");

        if (false == isThreadStartFailed)
        {
            sensorStatements.insert(sensorStatements.end(), commonStatementsBeginIt, commonStatementsEndIt);
            sensorDBConfig.emplace_back(sensorStatements, mdbcom::ConnectionType::DYNAMIC);

            // Check if the threads executed.
            for (auto i = 0u; i < numberOfSensorThreads; i++) /* pgr0745 *//* pgr0692 */
            {
                sensorFuture[i] = sensorPromise[i].get_future();

                if (mdbcom::Result::OK != mdbcom::MDBThreadUtility::createDBWorkUnit(SENSOR_THREAD_NAME,
                        sensorHandler, sensorDBConfig, &sensorPromise[i], sensorThreads[i]))
                {
                    mdb_apl_log_critical("Can't create obstacle detection thread #%u", i);
                    isThreadStartFailed = true;
                    sensorPromise[i].set_value(true);

                    // Set promise value to prevent waiting for a non-executing thread
                    for (auto j = i + 1; j < numberOfSensorThreads; j++)
                    {
                        sensorFuture[j] = sensorPromise[j].get_future();
                        sensorPromise[j].set_value(true);
                    }

                    break;
                }
            }
        }
        else
        {
            numberOfSensorThreads = 0;
        }



        //! Init cistern sensor Threads
        auto numberOfCisternSensorThreads = eventProcessingConfig.getThreadsNum(CISTERN_SENSOR_THREAD_NAME); /* pgr0745 */
        std::promise<bool> cisternSensorPromise[numberOfCisternSensorThreads];
        std::future<bool> cisternSensorFuture[numberOfCisternSensorThreads];
        std::thread cisternSensorThreads[numberOfCisternSensorThreads];
        auto cisternSensorDBConfig = std::vector<mdbcom::MDBThreadDatabaseConfig> { }; /* pgr0745 */
        auto cisternSensorStatements = cisternSensorPreparedStatements;
        mdb_apl_log_debug("starting all obstacle detection thread...");

         if (false == isThreadStartFailed)
         {
            cisternSensorStatements.insert(cisternSensorStatements.end(), commonStatementsBeginIt, commonStatementsEndIt);
            cisternSensorDBConfig.emplace_back(cisternSensorStatements, mdbcom::ConnectionType::DYNAMIC);

            // Check if the threads executed.
            for (auto i = 0u; i < numberOfCisternSensorThreads; i++) /* pgr0745 *//* pgr0692 */
            {
                cisternSensorFuture[i] = cisternSensorPromise[i].get_future();

                if (mdbcom::Result::OK != mdbcom::MDBThreadUtility::createDBWorkUnit(CISTERN_SENSOR_THREAD_NAME,
                        sensorCisternHandler, cisternSensorDBConfig, &cisternSensorPromise[i], cisternSensorThreads[i]))
                {
                    mdb_apl_log_critical("Can't create obstacle detection thread #%u", i);
                    isThreadStartFailed = true;
                    cisternSensorPromise[i].set_value(true);

                    // Set promise value to prevent waiting for a non-executing thread
                    for (auto j = i + 1; j < numberOfCisternSensorThreads; j++)
                    {
                        cisternSensorFuture[j] = cisternSensorPromise[j].get_future();
                        cisternSensorPromise[j].set_value(true);
                    }

                    break;
                }
            }
        }
        else
        {
            numberOfCisternSensorThreads = 0;
        }


         //! Init nitrification sensor Threads
         auto numberOfNitrificationSensorThreads = eventProcessingConfig.getThreadsNum(NITRIFICATION_SENSOR_THREAD_NAME); /* pgr0745 */
         std::promise<bool> nitrificationSensorPromise[numberOfNitrificationSensorThreads];
         std::future<bool> nitrificationSensorFuture[numberOfNitrificationSensorThreads];
         std::thread nitrificationSensorThreads[numberOfNitrificationSensorThreads];
         auto nitrificationSensorDBConfig = std::vector<mdbcom::MDBThreadDatabaseConfig> { }; /* pgr0745 */
         auto nitrificationSensorStatements = nitrificationSensorPreparedStatements;
         mdb_apl_log_debug("starting all obstacle detection thread...");

          if (false == isThreadStartFailed)
          {
             nitrificationSensorStatements.insert(nitrificationSensorStatements.end(), commonStatementsBeginIt, commonStatementsEndIt);
             nitrificationSensorDBConfig.emplace_back(nitrificationSensorStatements, mdbcom::ConnectionType::DYNAMIC);

             // Check if the threads executed.
             for (auto i = 0u; i < numberOfNitrificationSensorThreads; i++) /* pgr0745 *//* pgr0692 */
             {
                 nitrificationSensorFuture[i] = nitrificationSensorPromise[i].get_future();

                 if (mdbcom::Result::OK != mdbcom::MDBThreadUtility::createDBWorkUnit(NITRIFICATION_SENSOR_THREAD_NAME,
                         sensorNitrificationnHandler, nitrificationSensorDBConfig, &nitrificationSensorPromise[i], nitrificationSensorThreads[i]))
                 {
                     mdb_apl_log_critical("Can't create obstacle detection thread #%u", i);
                     isThreadStartFailed = true;
                     nitrificationSensorPromise[i].set_value(true);

                     // Set promise value to prevent waiting for a non-executing thread
                     for (auto j = i + 1; j < numberOfNitrificationSensorThreads; j++)
                     {
                         nitrificationSensorFuture[j] = nitrificationSensorPromise[j].get_future();
                         nitrificationSensorPromise[j].set_value(true);
                     }

                     break;
                 }
             }
         }
         else
         {
             numberOfNitrificationSensorThreads = 0;
         }

          //! Init nitrification sensor Threads
          auto numberOfOutsideSensorThreads = eventProcessingConfig.getThreadsNum(OUTSIDE_SENSOR_THREAD_NAME); /* pgr0745 */
          std::promise<bool> outsideSensorPromise[numberOfOutsideSensorThreads];
          std::future<bool> outsideSensorFuture[numberOfOutsideSensorThreads];
          std::thread outsideSensorThreads[numberOfOutsideSensorThreads];
          auto outsideSensorDBConfig = std::vector<mdbcom::MDBThreadDatabaseConfig> { }; /* pgr0745 */
          auto outsideSensorStatements = outsideSensorPreparedStatements;
          mdb_apl_log_debug("starting all obstacle detection thread...");

           if (false == isThreadStartFailed)
           {
               outsideSensorStatements.insert(outsideSensorStatements.end(), commonStatementsBeginIt, commonStatementsEndIt);
              outsideSensorDBConfig.emplace_back(outsideSensorStatements, mdbcom::ConnectionType::DYNAMIC);

              // Check if the threads executed.
              for (auto i = 0u; i < numberOfOutsideSensorThreads; i++) /* pgr0745 *//* pgr0692 */
              {
                  outsideSensorFuture[i] = outsideSensorPromise[i].get_future();

                  if (mdbcom::Result::OK != mdbcom::MDBThreadUtility::createDBWorkUnit(OUTSIDE_SENSOR_THREAD_NAME,
                          sensorOutsideHandler, outsideSensorDBConfig, &outsideSensorPromise[i], outsideSensorThreads[i]))
                  {
                      mdb_apl_log_critical("Can't create obstacle detection thread #%u", i);
                      isThreadStartFailed = true;
                      outsideSensorPromise[i].set_value(true);

                      // Set promise value to prevent waiting for a non-executing thread
                      for (auto j = i + 1; j < numberOfOutsideSensorThreads; j++)
                      {
                          outsideSensorFuture[j] = outsideSensorPromise[j].get_future();
                          outsideSensorPromise[j].set_value(true);
                      }

                      break;
                  }
              }
          }
          else
          {
              numberOfOutsideSensorThreads = 0;
          }

        if (false == isThreadStartFailed)
        {
            mdb_sys_log_out("F110201");
            mdb_apl_log_info("event processing process is started");

            // Wait SignalHandler thread ends (when receiving SIG REALTIME)
            static_cast<void>(mdbcom::MDBSignalHandler::getInstance().waitForTermination());
            mdb_apl_log_info("Waiting for all threads finish...");
        }
        else
        {
            mdb_apl_log_critical("Exit event processing process");
            mdb_sys_log_out("F110212");
            isInitThreadsNormal = false;

            //! End reading shared memory.
            if (nullptr != mEventReceptionSharedQueue)
            {
                (mEventReceptionSharedQueue.get())->endRead();
            }

            if (nullptr != mSensorSharedQueue)
            {
                (mSensorSharedQueue.get())->endRead();
            }
            if (nullptr != mCisternSensorSharedQueue)
            {
                (mCisternSensorSharedQueue.get())->endRead();
            }
            if (nullptr != mOutsideSensorSharedQueue)
            {
                (mOutsideSensorSharedQueue.get())->endRead();
            }
            if (nullptr != mNitrificationSensorSharedQueue)
            {
                (mNitrificationSensorSharedQueue.get())->endRead();
            }
            retExecute = mdbcom::ExitCode::NG;
        }

        //auto isThreadHang = false; /* pgr0745 */
        //auto timeout = WAIT_FOR_TERMINATING_TIMER; /* pgr0745 */

        //! Check if all threads ends normally to wait them or not
        // Check status of receptionHandler thread to see if it is alive or not
        // Wait maxTimeOut milliseconds
        for (auto j = 0u; j < numberOfReceptionThreads; j++) /* pgr0745 *//* pgr0692 */
        {
            if (false == receptionThread[j].joinable())
            {
                mdb_apl_log_warn("reception thread [%d] is unjoinable", j);
            }
            else
            {
                try
                {
                    mdb_apl_log_info("waiting reception thread [%d]", j);
                    receptionThread[j].join();
                    mdb_apl_log_info("reception thread [%d] ended", j);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Error waiting for reception thread [%d]", j);
                }
            }
        }

        mdb_apl_log_info("All reception Threads finished");

        for (auto j = 0u; j < numberOfSensorThreads; j++) /* pgr0745 *//* pgr0692 */
        {
            if (false == sensorThreads[j].joinable())
            {
                mdb_apl_log_warn("obstacleDetectionThreads [%u] is unjoinable", j);
            }
            else
            {
                try
                {
                    sensorThreads[j].join();
                    mdb_apl_log_info("obstacleDetectionThreads [%u] ended", j);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Error waiting for sensorThreads [%u]", j);
                }
            }

        }
        for (auto j = 0u; j < numberOfCisternSensorThreads; j++) /* pgr0745 *//* pgr0692 */
        {
            if (false == cisternSensorThreads[j].joinable())
            {
                mdb_apl_log_warn("obstacleDetectionThreads [%u] is unjoinable", j);
            }
            else
            {
                try
                {
                    cisternSensorThreads[j].join();
                    mdb_apl_log_info("obstacleDetectionThreads [%u] ended", j);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Error waiting for sensorThreads [%u]", j);
                }
            }
        }
        for (auto j = 0u; j < numberOfNitrificationSensorThreads; j++) /* pgr0745 *//* pgr0692 */
        {
            if (false == nitrificationSensorThreads[j].joinable())
            {
                mdb_apl_log_warn("obstacleDetectionThreads [%u] is unjoinable", j);
            }
            else
            {
                try
                {
                    nitrificationSensorThreads[j].join();
                    mdb_apl_log_info("obstacleDetectionThreads [%u] ended", j);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Error waiting for sensorThreads [%u]", j);
                }
            }
        }
        for (auto j = 0u; j < numberOfOutsideSensorThreads; j++) /* pgr0745 *//* pgr0692 */
        {
            if (false == outsideSensorThreads[j].joinable())
            {
                mdb_apl_log_warn("obstacleDetectionThreads [%u] is unjoinable", j);
            }
            else
            {
                try
                {
                    outsideSensorThreads[j].join();
                    mdb_apl_log_info("obstacleDetectionThreads [%u] ended", j);
                }
                catch (...)
                {
                    mdb_apl_log_warn("Error waiting for sensorThreads [%u]", j);
                }
            }
        }
        mdb_apl_log_end();
        return retExecute;
    }
}

#ifndef BUILD_FJSVDMAPTEST
MIOT_MAINAPP(famseventprocessing::FAMSEPApplication); /*pgr0745*/ /*pgr0552*/
#endif



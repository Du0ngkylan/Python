/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRAppServer.cpp
 * @brief       Source file for implementing FAMSDRAppServer information.
 * @author      anhnguyen
 * @date        2018/12/11
 */

#include <MDBConnectionManager.h>
#include "MDBDynamicDatabaseAccess.h"
#undef REAL
#include "FAMSDRCommon.h"
#include "FAMSDRAppServer.h"
#include "FAMSDRRequestHandlerFactory.h"
#include "MDBSignalHandler.h"
#include "FAMSReceptionAndProcessingShmConfig.h"

namespace famsdatareception {

    std::string FAMSDRAppServer::mDataReceptionShmName { "DataReception_EventProcessing" };

    /*!
     * @brief Constructor.
     * @param None.
     * @return None.
     */
    FAMSDRAppServer::FAMSDRAppServer(const std::string &processName) :
        mdbcom::MDBHttpApplicationServer(processName), mSharedQueue { nullptr }
    {
    }

    /*!
     * @brief Destructor.
     * @param None.
     * @return None.
     */
    FAMSDRAppServer::~FAMSDRAppServer(void)
    {

    }

    /*!
     * @brief The method does initialization when server is started.
     * @param None.
     * @retval true Initialize OK.
     * @retval false Initialize NG.
     */
    bool FAMSDRAppServer::initializeApplication(void)
    {
        //! Syslog "start" is outputted
        mdb_apl_log_start();
        mdb_apl_log_info("Data reception server is starting");

        auto ret = true; /* pgr0745 */
        std::string runningPosition;

        try
        {
            //! Init IPC shared memory
            runningPosition = std::string {"creating share queue"};
            auto receptionAndProcessingShmConf = famscommon::FAMSReceptionAndProcessingShmConfig {}; /* pgr0745 */

            mSharedQueue = mdbcom::MDBMemoryUtility::make_unique <mdbcom::MDBIPCShm > (mDataReceptionShmName,
                           receptionAndProcessingShmConf.getReceptionAndProcessingShmSize());

            // Initialize ResourceMapper for threads
            runningPosition = std::string {"creating resource for threads"};
            const auto &webServerConfig = getWebServerConfig(); /* pgr0745 */
            auto maxThreads = webServerConfig.getMaxThreads(); /* pgr0745 */

            static auto commonStatements = famscommon::getCommonStatement(); /* pgr0745 *//*pgr2084*/
            auto receptionStatementBegin = dataReceptionPreparedStatements.begin();/* pgr0745 */
            auto receptionStatementEnd = dataReceptionPreparedStatements.end();/* pgr0745 */
            commonStatements.insert(commonStatements.end(), receptionStatementBegin, receptionStatementEnd);
            mResources = mdbcom::MDBMemoryUtility::make_unique<mdbcom::MDBResourceMapper>(
                             maxThreads, &commonStatements);

            // Init FAMSDRWaterTankRequestHandlerFactory
            auto queue = mSharedQueue.get();
            runningPosition = std::string {"creating DynamicRequestHandlerFactory"};
            mHandlerFactory = mdbcom::MDBMemoryUtility::make_unique <FAMSDRWaterTankRequestHandlerFactory >(
                                  queue, webServerConfig.getTemplateConf());
        }
        catch (const std::bad_alloc &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            auto tmpRunningPos = runningPosition.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Bad alloc exception: %s when %s", exDetail, tmpRunningPos);
            ret = false;
        }
        catch (const mdbcom::MDBException &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            auto tmpRunningPos = runningPosition.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Exception: %s when %s", exDetail, tmpRunningPos);
            ret = false;
        }
        catch (...)
        {
            auto tmpRunningPos = runningPosition.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Unknown exception when %s", tmpRunningPos);
            ret = false;
        }

        if (true == ret)
        {
            mdb_sys_log_out("F110301");
            mdb_apl_log_info("Data reception server is started");
        }
        else
        {
            mdb_apl_log_critical("Fail to initialze Data reception server");
            mdb_sys_log_out("F110303");
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief The method does un-initialization when process is stopped.
     * @param None.
     * @retval true uninitialize successfully.
     * @retval false uninitialize failed.
     * @note This function is called by DMAPApplication::main when the Zenrin initialization is not executed. Then, We ignore the ERROR in this function.
     */
    bool FAMSDRAppServer::uninitializeApplication(void)
    {
        mdb_apl_log_start();
        auto ret = true; /* pgr0745 */

        try
        {

            if (nullptr != mSharedQueue)
            {
                (mSharedQueue.get())->unlink(); /*pgr1171*/
            }

            auto dynamicDBConnection = mdbcom::MDBMemoryUtility::make_unique<mdbcom::MDBDynamicDatabaseAccess>(); /* pgr0745 */

            if (nullptr == dynamicDBConnection)
            {
                throw mdbcom::MDBException {"cannot make dynamic DB connection to truncate"};
            }

            mdb_apl_log_info("DynamicManagement is ended normally.");
            mdb_sys_log_out("F110302");
        }
        catch (const mdbcom::MDBException &ex)
        {
            //un-Initialize Error!
            ret = false;
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("DynamicManagement un-initialize is Failed!(%s)", exDetail);
            mdb_sys_log_out("F110304");
        }
        catch (...)
        {

            //Initialize Error!
            ret = false;
            mdb_apl_log_critical("DynamicManagement un-initialize Catched no-MDBException.");
            mdb_sys_log_out("F110304");
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief The method to get the configuration for HttpServer.
     * @param None.
     * @return Reference to configuration object
     */
    const FAMSDRConfig &FAMSDRAppServer::getWebServerConfig(void)
    {
        return FAMSDRConfig::getInstance();
    }
}
#ifndef BUILD_FJSVDMAPTEST
MIOT_MAINAPP(famsdatareception::FAMSDRAppServer) /* pgr0745 */
#endif

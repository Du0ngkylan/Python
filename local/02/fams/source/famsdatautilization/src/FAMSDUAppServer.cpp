/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUAppServer.cpp
 * @brief       Source file for implementing FAMSDUAppServer information.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include <iostream>
#include "FAMSDUAppServer.h"
#include "FAMSDURequestHandlerFactory.h"
#include "MDBSignalHandler.h"

namespace famsdatautil {

    /*!
     * @brief Constructor.
     * @param None.
     * @return None.
     */
    FAMSDUAppServer::FAMSDUAppServer(const std::string &processName) :
        mdbcom::MDBHttpApplicationServer(processName)
    {
    }

    /*!
     * @brief Destructor.
     * @param None.
     * @return None.
     */
    FAMSDUAppServer::~FAMSDUAppServer(void)
    {

    }

    /*!
     * @brief The method does initialization when server is started.
     * @param None.
     * @retval true Initialize OK.
     * @retval false Initialize NG.
     */
    bool FAMSDUAppServer::initializeApplication(void)
    {
        mdb_apl_log_start();
        auto ret = true; /* pgr0745 */
        std::string runningPos;

        //MDBException
        try
        {
            const auto &webServerConfig = getWebServerConfig(); /* pgr0745 */

            // Initialize ResourceMapper for threads
            runningPos = std::string {"Initialize ResourceMapper for threads"};
            auto maxThreads = webServerConfig.getMaxThreads(); /* pgr0745 */
            mResources = mdbcom::MDBMemoryUtility::make_unique<mdbcom::MDBResourceMapper>(
                             maxThreads, &dataUtilPreparedStatements);

            // Initialize handler with yaml file
            runningPos = std::string {"Initialize handler with yaml file"};
            mHandlerFactory = mdbcom::MDBMemoryUtility::make_unique<FAMSDURequestHandlerFactory>(
                                  webServerConfig.getTemplateConf());
        }
        catch (const mdbcom::MDBException &ex)
        {
            //Initialize Error!
            ret = false;
            auto exDetail = ex.what(); /* pgr0745 */
            auto runningPosStr = runningPos.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Catch exception [%s] at [%s]", exDetail, runningPosStr);
        }
        catch (const std::bad_alloc &ex)
        {
            ret = false;
            auto exDetail = ex.what(); /* pgr0745 */
            auto runningPosStr = runningPos.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Catch Exception: [%s] at [%s]", exDetail, runningPosStr);
        }
        catch (...)
        {
            //Initialize Error!
            ret = false;
            auto runningPosStr = runningPos.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Catched unknown exception at [%s]", runningPosStr);
        }

        // print syslog
        if (false == ret)
        {
            mdb_apl_log_critical("Fail to start AppIF");
            mdb_sys_log_out("F110403");
        }
        else
        {
            mdb_apl_log_info("AppIF is started");
            mdb_sys_log_out("F110401");
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
    bool FAMSDUAppServer::uninitializeApplication(void)
    {
        mdb_apl_log_start();
        mdb_sys_log_out("F110402");
        mdb_apl_log_end();
        return true;
    }

    /*!
     * @brief Get web server config
     * @param None.
     * @return Server config
     */
    const FAMSDUConfig &FAMSDUAppServer::getWebServerConfig(void) /* pgr2613 */
    {
        return FAMSDUConfig::getInstance();
    }

}
#ifndef BUILD_FJSVFAMSTEST
MIOT_MAINAPP(famsdatautil::FAMSDUAppServer) /* pgr0745 */
#endif


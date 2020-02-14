/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBHTTPServerConfig.cpp
 * @brief Implementation for MDBHTTPServerConfig
 * @author trangvp:
 * @date 2016/12/05
 */

#include <Poco/Dynamic/Var.h>

#include "MDBHTTPServerConfig.h"
#include "MDBLogger.h"
#include "MDBException.h"

namespace mdbcom {
    // Constants
    const std::string MDBHTTPServerConfig::HTTP_SERVER_PORT_STR
    {
        "serverPort"
    };
    const std::string MDBHTTPServerConfig::HTTP_SERVER_MAX_THREAD_STR { "maxThreads" };
    const std::string MDBHTTPServerConfig::HTTP_SERVER_MAX_QUEUED_STR { "maxQueueds" };
    const std::string MDBHTTPServerConfig::HTTP_SERVER_STACK_SIZE_STR { "stackSize" };

    MDBHTTPServerConfig::MDBHTTPServerConfig(const std::string &configPath) :
        mConfigFile { nullptr }, mServerPort { 0 }, mMaxThreads { 0 }, mMaxQueueds { 0 }, mStackSize
    {
        0 }
    {
        mdb_apl_log_start();

        auto isNormalConfig = false; /* pgr0745 */
        auto errorString = std::string {}; /* pgr0745 */

        do
        {

            try
            {
                mConfigFile = MDBMemoryUtility::make_unique<MDBConfigFile>(configPath);
            }
            catch (const MDBException &ex)
            {
                errorString = ex.what();
                break;
            }
            catch (const std::bad_alloc &ex)
            {
                errorString = ex.what();
                break;
            }
            catch (...)
            {
                errorString = "Unknown exception when open configuration file";
                break;
            }

            auto cfgVal = std::string {}; /* pgr0745 */
            // get ServerPort key
            auto ret = mConfigFile->getValue(HTTP_SERVER_PORT_STR, cfgVal); /* pgr0745 */
            auto converter = Poco::Dynamic::Var {}; /* pgr0745 */

            if ((Result::OK != ret) || (true == cfgVal.empty()))
            {
                errorString = "key serverPort has abnormal setting";
                break;
            }

            try
            {
                converter = cfgVal;
                converter.convert(mServerPort);
                mdb_apl_log_debug("mServerPort=%hu", mServerPort);
            }
            catch (...)
            {
                errorString = "key serverPort has abnormal setting";
                break;
            }

            // get maxThreads key
            ret = mConfigFile->getValue(HTTP_SERVER_MAX_THREAD_STR, cfgVal);

            if ((Result::OK != ret) || (true == cfgVal.empty()))
            {
                errorString = "key maxThreads has abnormal setting";
                break;
            }

            try
            {
                converter = cfgVal;
                converter.convert(mMaxThreads);

                if ((0 >= mMaxThreads) || (HTTP_SERVER_MAX_THREADS < mMaxThreads))
                {
                    errorString = "key maxThreads has value out of range (0, 400]";
                    break;
                }

                mdb_apl_log_debug("mMaxThreads=%hu", mMaxThreads);
            }
            catch (...)
            {
                errorString = "key maxThreads has abnormal setting";
                break;
            }

            // get maxQueueds key
            ret = mConfigFile->getValue(HTTP_SERVER_MAX_QUEUED_STR, cfgVal);

            if ((Result::OK != ret) || (true == cfgVal.empty()))
            {
                errorString = "key maxQueueds has abnormal setting";
                break;
            }

            try
            {
                converter = cfgVal;
                converter.convert(mMaxQueueds);

                if (0 >= mMaxQueueds)
                {
                    errorString = "key maxThreads has negative value";
                    break;
                }

                mdb_apl_log_debug("mMaxQueueds=%hu", mMaxQueueds);
            }
            catch (...)
            {
                errorString = "key mMaxQueueds has abnormal setting";
                break;
            }

            // get stackSize key
            ret = mConfigFile->getValue(HTTP_SERVER_STACK_SIZE_STR, cfgVal);

            if ((Result::OK != ret) || (true == cfgVal.empty()))
            {
                errorString = "key stackSize has abnormal setting";
                break;
            }

            try
            {
                converter = cfgVal;
                converter.convert(mStackSize);

                if (0 >= mStackSize)
                {
                    errorString = "key stackSize has negative value";
                    break;
                }

                mdb_apl_log_debug("mStackSize=%hu", mStackSize);
            }
            catch (...)
            {
                errorString = "key stackSize has abnormal setting";
                break;
            }

            isNormalConfig = true;
        }
        while (0);

        if (true != isNormalConfig)
        {
            auto tmpErrorString = errorString.c_str(); /*pgr0745*/
            mdb_apl_log_critical(tmpErrorString);
            mdb_apl_log_end();
            throw MDBException {errorString};
        }

        mdb_apl_log_end();
    }
}


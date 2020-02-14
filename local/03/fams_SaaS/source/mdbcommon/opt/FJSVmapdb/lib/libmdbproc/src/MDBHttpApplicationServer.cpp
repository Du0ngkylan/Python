/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBHttpApplicationServer.cpp
 * @brief       Source file for implementing MDBHttpApplicationServer based class.
 * @author      hunglt: Implement webserver application logic
 * @date        2016/12/12
 */

#include <future>

#include <Poco/DateTimeParser.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/Timestamp.h>

#include "MDBHttpApplicationServer.h"
#include "MDBRequestHandlerFactory.h"
#include "MDBSignalHandler.h"
#include "MDBCryptoUtility.h"
#include "MDBHTTPServerUtility.h"

namespace mdbcom {

    /*!
     * @brief Constructor.
     * @param None.
     * @return None.
     */
    MDBHttpApplicationServer::MDBHttpApplicationServer(const std::string &processName) :
        MDBApplication{processName}, mHandlerFactory {nullptr},
        mResources {nullptr}, pServer {nullptr}
    {
    }

    /*!
     * @brief handle Sig RealTime for MDBHttpApplicationServer.
     *
     * @param [in] signum the signum
     * @param [in] args argument list when handler is called (received SIG REALTIME)
     * @return return code of handler processing.
     */
    int MDBHttpApplicationServer::handleSigRealTime(const int signum __attribute__((unused)),
            const void *arg __attribute__((unused)))
    {
        mdb_apl_log_start();

        if (Result::OK != stopServer())
        {
            mdb_apl_log_critical("Server end with error");
        }
        else
        {
            mdb_apl_log_info("Server end normally");
        }

        mdb_apl_log_end();
        return Signal::OK;
    }

    /*!
     * @brief handle stopping logic for http server.
     *
     * @param None.
     * @retval Result::OK Success
     * @retval Result::NG Fail
     */
    int MDBHttpApplicationServer::stopServer(void)
    {
        mdb_apl_log_start();
        auto ret = Result::NG; /*pgr0745*/
        auto endSignal = std::promise<void> {}; /* pgr0745 */
        auto endResult = endSignal.get_future(); /* pgr0745 */

        std::thread([&endSignal, this]()
        {
            auto pHttpServer = this->getHttpServerPtr(); /* pgr0745 */

            if (nullptr != pHttpServer)
            {
                pHttpServer->stopAll(true);
            }

            endSignal.set_value();
        }).detach();

        // Wait for stopAll() 10s, User can change the timeout value
        auto timeout = WAIT_FOR_TERMINATING_TIMER; /* pgr0745 */

        if (std::future_status::ready != endResult.wait_for(std::chrono::milliseconds(timeout)))
        {
            // stopAll hangs for more than 10s, execute error handler here we left it and exit main.
            mdb_apl_log_warn("Failed to stop all, don't wait more");
            auto processName = mProcessName.c_str(); /* pgr0745 */
            mdb_sys_log_out("F000902", processName);
        }
        else
        {
            mdb_apl_log_info("HTTP server stopped normally");
            ret = Result::OK;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief get encrypted time saved in file.
     *
     * @param None.
     * @retval Result::OK Success
     * @retval Result::NG Fail
     */
    int MDBHttpApplicationServer::readEncryptedFile(void)
    {
        mdb_apl_log_start();
        auto retCode = Result::OK; /* pgr0745 */
        static std::string encryptedFilePath = "/opt/FJSVmapdb/etc/duration_limit";
        std::string encryptedTxt;
        std::ifstream encryptedFile(encryptedFilePath);

        if (encryptedFile.is_open())
        {
            while (getline(encryptedFile, encryptedTxt))
            {
                break;
            }

            encryptedFile.close();
        }
        else
        {
            mdb_apl_log_critical("fail to open encrypted file");
            mdb_apl_log_end();
            return Result::NG;
        }

        auto encryptedTimeStr = encryptedTxt.c_str(); /* pgr0745 */
        mdb_apl_log_debug("encrypted text: %s", encryptedTimeStr);
        std::string decryptedText;
        retCode = MDBCryptoUtility::decryptAES256(encryptedTxt, decryptedText);
        auto decryptedTextStr = decryptedText.c_str(); /* pgr0745 */
        mdb_apl_log_debug("decrypted text: %s", decryptedTextStr);

        if (Result::OK == retCode)
        {
            /* parse decryptd time to timestamp*/
            try
            {
                int tzd = -1; // different timezone in seconds
                Poco::DateTime dt;
                Poco::DateTimeParser::parse(Poco::DateTimeFormat::ISO8601_FRAC_FORMAT, decryptedText, dt, tzd);
                Poco::Timestamp ts = dt.timestamp();
                MDBHTTPServerUtility::encryptedTime = static_cast<long long>(ts.epochMicroseconds() / 1000) - 9 * 60 * 60 * 1000; /*pgr0745*/
            }
            catch (std::exception &e)
            {
                mdb_apl_log_critical("invalid time string, exception [%s]", e.what());
                retCode = Result::NG;
            }
            catch (...)
            {
                mdb_apl_log_critical("invalid time string");
                retCode = Result::NG;
            }

            /**/
        }

        mdb_apl_log_end();
        return retCode;
    }

    /*!
     * @brief The method perform processing logic of process
     * @param None.
     * @retval ExitCode::OK Success
     * @retval ExitCode::NG Fail
     */
    int MDBHttpApplicationServer::execute(const std::vector<std::string> &args __attribute__((unused)))
    {
        mdb_apl_log_start();

        auto webServerConfig = static_cast<const MDBHTTPServerConfig *>(nullptr); /*pgr0745*/

        try
        {
            webServerConfig = &getWebServerConfig();
        }
        catch (const MDBException &ex)
        {
            auto errMsg = ex.what(); /*pgr0745*/
            mdb_apl_log_critical("Error getting configuration for server [%s]", errMsg);
            mdb_apl_log_end();
            return mdbcom::ExitCode::NG;
        }
        catch (...)
        {
            mdb_apl_log_critical("Error getting configuration for server [unknown]");
            mdb_apl_log_end();
            return mdbcom::ExitCode::NG;
        }

        auto ret = readEncryptedFile();

        if (Result::OK != ret)
        {
            mdb_apl_log_critical("Cannot get content of encrypted file");
        }
        else
        {
            mdb_apl_log_info("encrypted timestamp JST: %lld", MDBHTTPServerUtility::encryptedTime);
        }

        auto maxThreads = webServerConfig->getMaxThreads(); /* pgr0745 */
        auto maxQueueds = webServerConfig->getMaxQueueds(); /* pgr0745 */
        auto stackSize = webServerConfig->getStackSize(); /* pgr0745 */

        // set-up a server socket
        auto svs = Poco::Net::ServerSocket { webServerConfig->getServerPort() }; /* pgr0745 */
        // Create ThreadPool with mix_size = max_size = number of HTTP Thread
        Poco::ThreadPool httpPool { maxThreads, maxThreads, stackSize};

        // set-up a HTTPServer instance
        auto factory = mHandlerFactory.get(); /* pgr0745 */
        auto resourceMapper = mResources.get(); /* pgr0745 */

        try
        {
            auto pParams = Poco::AutoPtr<Poco::Net::HTTPServerParams>   /* pgr0745 */
            {
                new Poco::Net::HTTPServerParams { }
            };
            pParams->setMaxQueued(maxQueueds);
            pParams->setMaxThreads(maxThreads);
            pParams->setKeepAlive(false);

            auto requestFactory = MDBMemoryUtility::make_unique<MDBRequestHandlerFactory>(factory, resourceMapper); /*pgr0745*/
            auto pFactory = requestFactory.get(); /* pgr0745 */
            auto pDupParams = pParams.duplicate(); /* pgr0745 */

            pServer = MDBMemoryUtility::make_unique<Poco::Net::HTTPServer>(pFactory, httpPool, svs,
                      pDupParams);

            // let the server manage life time of request factory
            static_cast<void>(requestFactory.release());
        }
        catch (const std::bad_alloc &ex)
        {
            auto exMsg = ex.what(); /*pgr0745*/
            mdb_apl_log_critical("Insufficient memory: %s", exMsg);
            mdb_apl_log_end();
            return mdbcom::ExitCode::NG;
        }
        catch (...)
        {
            mdb_apl_log_critical("Unknown exeption while allocating memory");
            mdb_apl_log_end();
            return mdbcom::ExitCode::NG;
        }

        // start the HTTPServer
        pServer->start();

        // Wait ending of SignalHandler
        auto retcode = waitForSignalHandler(); /* pgr0745 */

        if (Signal::OK != retcode)
        {
            if (Result::OK != stopServer())
            {
                mdb_apl_log_critical("Server end with error");
            }
            else
            {
                mdb_apl_log_info("Server end normally");
            }
        }

        mdb_apl_log_end();
        return mdbcom::ExitCode::OK;
    }
}

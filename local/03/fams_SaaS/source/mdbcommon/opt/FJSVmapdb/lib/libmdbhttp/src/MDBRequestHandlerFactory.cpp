/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBRequestHandlerFactory.cpp
 * @brief       Source file for implementing MDBRequestHandlerFactory information.
 * @author      trangvp: create it for support MDBRequestHandlerFactory code.
 * @date        2016/09/24
 */

#include <iostream>
#include <mutex>

#include <Poco/Thread.h>

#include "MDBRequestHandlerFactory.h"
#include "MDBLogger.h"
#include "MDBResourceMapper.h"
#include "MDBConnectionManager.h"

namespace mdbcom {
    //! Flag for checking if thread is initialized
    thread_local bool MDBRequestHandlerFactory::isInitialized { false };
    //! Thread name for http handler thread
    const std::string MDBRequestHandlerFactory::HANDLER_THREAD_NAME
    {
        "httpworker"
    };

    /*!
     * @brief The Constructor.
     *
     * @param [in] factory Actual factory class
     * @param [in] resourceMapper Object to manage db connections.
     *
     * @return None.
     */
    MDBRequestHandlerFactory::MDBRequestHandlerFactory(Poco::Net::HTTPRequestHandlerFactory *impl,
            MDBResourceMapper *resourceMapper) :
        pFactory { impl }, resources { resourceMapper }
    {
        if ((nullptr == pFactory) || (nullptr == resources))
        {
            mdb_apl_log_critical("Invalid parameter [factory:%p][resourceMapper:%p]", pFactory,
                                 resources);
            mdb_sys_log_out("F000601", "HTTP Thread");
            throw MDBException { "Invalid parameter [no factory or resourceMapper]" };
        }
    }

    /*!
     * @brief Init resources for handler thread then call handler
     *
     * @param [in] request the HTTP request
     * @return a pointer to request handler
     */
    Poco::Net::HTTPRequestHandler *MDBRequestHandlerFactory::createRequestHandler(
        const Poco::Net::HTTPServerRequest &request)
    {
        static std::mutex setConnectionMutex { };

        if (true == isInitialized)
        {
            mdb_apl_log_start();
        }
        else
        {
            // Open log debug for current thread
            // If already open, it'll do nothing and return OK
            if (Result::OK != MDBLogger::openDebugLog(HANDLER_THREAD_NAME))
            {
                mdb_sys_log_out("F000601", "HTTP Thread");
                return nullptr;
            }

            mdb_apl_log_start();

            if ((true == resources->isUsingDynamicDB())
                    && (false == MDBConnectionManager::hasDynamicDB()))
            {
                // To prevent critical section when set/get connection
                std::lock_guard<std::mutex> guard(setConnectionMutex); /* pgr0305 */

                // if current handler is using db conenction and hasn't init yet,
                // assign db connection frorm resource list
                if (Result::OK != MDBConnectionManager::setDynamicDBConnection(
                            resources->getDynamicDBConn()))
                {
                    mdb_apl_log_critical("Can't set dynamic connection");
                    mdb_sys_log_out("F000601", "HTTP Thread");
                    mdb_apl_log_end();
                    return nullptr;
                }
            }

            isInitialized = true;
        }

        mdb_apl_log_end();
        // actual get and return http request handler
        return pFactory->createRequestHandler(request);
    }
}

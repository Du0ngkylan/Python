/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRWaterTankRequestHandlerFactory.cpp
 * @brief       Source file for implementing FAMSDRWaterTankRequestHandlerFactory information.
 * @author      anhnguyen
 * @date        2018/12/11
 */
#include <Poco/String.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/NetException.h>
#include <Poco/RegularExpression.h>
#include "MDBURIHelper.h"
#include "MDBResourceMapper.h"
#include "MDBResourceNotFoundHandler.h"
#include "FAMSDRRequestHandlerFactory.h"
#include "FAMSDRWaterTankRequestHandler.h"
#include "FAMSDRNitrificationRequestHandler.h"
#include "FAMSDROutsideRequestHandler.h"
#include "FAMSDRCisternRequestHandler.h"
#include "MDBYamlConfig.h"

namespace famsdatareception {

    //! Dynamic info (Object) Add request URI
    
//    const Poco::RegularExpression FAMSDRWaterTankRequestHandlerFactory::URI_SENSOR(URI_SENSOR_PATH);

    /*!
     * @brief The constructor.
     * @param [in] templateConfigFile Path to template configuration file
     * @return None.
     */
    FAMSDRWaterTankRequestHandlerFactory::FAMSDRWaterTankRequestHandlerFactory(mdbcom::MDBIPCShm *sharedMemory,
            const std::string &templateConfigFile) :
        mSharedMemory { sharedMemory }, mReplaceTTemplate { }, mNitrificationTemplate {}, mOutSideTemplate{}
    {
        auto templateConf = mdbcom::MDBYamlConfig { templateConfigFile }; /* pgr0745 */
        if (mdbcom::Result::OK != templateConf.getFormat(WATER_REPLACE_TANK_PATH, Poco::toLower(Poco::Net::HTTPRequest::HTTP_POST), mReplaceTTemplate))
        {
            throw mdbcom::MDBException { "Can't get format template for water replace tank" };
        } else if (mdbcom::Result::OK != templateConf.getFormat(NITRIFICATION_TANK_PATH, Poco::toLower(Poco::Net::HTTPRequest::HTTP_POST), mNitrificationTemplate))
        {
            throw mdbcom::MDBException { "Can't get format template for nitrification request" };
        } else if (mdbcom::Result::OK != templateConf.getFormat(OUTSIDE_PATH, Poco::toLower(Poco::Net::HTTPRequest::HTTP_POST), mOutSideTemplate))
        {
            throw mdbcom::MDBException { "Can't get format template for outside request" };
        }
        if (mdbcom::Result::OK != templateConf.getFormat(CISTERN_PATH, Poco::toLower(Poco::Net::HTTPRequest::HTTP_POST), sensorCisternTemplate))
        {
            throw mdbcom::MDBException { "Can't get format template for sensor cistern request" };
        }
    }

    /*!
     * @brief The destructor.
     * @param None.
     * @return None.
     */
    FAMSDRWaterTankRequestHandlerFactory::~FAMSDRWaterTankRequestHandlerFactory()
    {
    }

    /*!
     * @brief Create handler thread then call handler
     *
     * @param [in] request the HTTP request
     * @return a pointer to request handler
     */
    Poco::Net::HTTPRequestHandler *FAMSDRWaterTankRequestHandlerFactory::createRequestHandler(
        const Poco::Net::HTTPServerRequest &request)
    {
        mdb_apl_log_start();

        auto handler = static_cast<Poco::Net::HTTPRequestHandler *>(nullptr);
        // the final URI to jugde REST API
        auto uriPath = std::string { };

        // Extract URI path only
        try
        {
            auto uri = mdbcom::MDBURIHelper { request.getURI() };
            uriPath = uri.getPath();
        }
        catch (...)
        {
            mdb_apl_log_error("Parse URI has error");
        }

        auto method = request.getMethod().c_str(); /* pgr0745 */
        auto tmpUri = uriPath.c_str(); /* pgr0745 */
        auto contentLength = request.getContentLength(); /* pgr0745 */

        auto resHttpStatus = Poco::Net::HTTPResponse::HTTP_FORBIDDEN;

        mdb_apl_log_info("Method(%s) URI(%s) Content-length(%ld)", method, tmpUri, contentLength);

        do
        {
            if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            {
                try
                {
                    if (uriPath.compare(WATER_REPLACE_TANK_PATH) == 0)
                    {
                        handler = new FAMSDRWaterTankRequestHandler(mSharedMemory, mReplaceTTemplate);
                        break;
                    } else if (uriPath.compare(CISTERN_PATH) == 0) {
                        handler = new FAMSDRCisternRequestHandler(mSharedMemory, sensorCisternTemplate);
                        break;
                    } else if (uriPath.compare(NITRIFICATION_TANK_PATH) == 0) {
                        handler = new FAMSDRNitrificationRequestHandler(mSharedMemory, mNitrificationTemplate);
                        break;
                    } else if (uriPath.compare(OUTSIDE_PATH) == 0) {
                        handler = new FAMSDROutsideRequestHandler(mSharedMemory, mOutSideTemplate);
                        break;
                    }
                    else
                    {
                        mdb_apl_log_error("Not match uri[%s]", tmpUri);
                        resHttpStatus = Poco::Net::HTTPResponse::HTTP_NOT_FOUND;
                    }

                }
                catch (const std::bad_alloc &ex)
                {
                    auto msg = ex.what(); /* pgr0745 */
                    mdb_apl_log_critical("Can't allocate new handler [%s] for [%s]", msg, tmpUri);
                    break;
                }
                catch (...)
                {
                    mdb_apl_log_critical("Can't allocate new handler [unknown error]");
                    break;
                }

            }
            else
            {
                mdb_apl_log_error("Not match method [POST]");
            }

            try
            {
                handler = new mdbcom::MDBResourceNotFoundHandler(resHttpStatus);
                break;
            }
            catch (const std::bad_alloc &ex)
            {
                auto msg = ex.what(); /* pgr0745 */
                mdb_apl_log_critical("Can't allocate new handler [%s] for [%s]", msg, tmpUri);
                break;
            }
            catch (...)
            {
                mdb_apl_log_critical("Can't allocate new handler [unknown error]");
                break;
            }
        }
        while (0);

        mdb_apl_log_end();
        return handler;
    }
}

/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURequestHandlerFactory.cpp
 * @brief       Source file for implementing FAMSDURequestHandlerFactory information.
 * @author      anhnguyen
 * @date        2018/12/10
 */
#include <Poco/String.h>
#include <Poco/Net/HTTPBasicCredentials.h>
#include <Poco/Net/NetException.h>
#include <Poco/RegularExpression.h>
#include <Poco/String.h>
#include "MDBURIHelper.h"
#include "MDBResourceMapper.h"
#include "MDBResourceNotFoundHandler.h"
#include "FAMSDURequestHandlerFactory.h"
#include "FAMSDUUserHandler.h"
#include "FAMSDUObserversHandler.h"
#include "FAMSDUCisternHandler.h"
#include "FAMSDUVersionHandler.h"
#include "FAMSDUTaskHandler.h"
#include "MDBYamlConfig.h"
#include "FAMSCommon.h"
#include "FAMSDUOrganismHandler.h"


#define newJsonFormat std::vector<mdbcom::MDBContentDecodingFormat>()

namespace famsdatautil {

    std::map<std::string, std::map<std::string, std::vector<mdbcom::MDBContentDecodingFormat>>> FAMSDURequestHandlerFactory::requestTemplates {
        {URI_LOGIN_PATH, {{Poco::Net::HTTPRequest::HTTP_POST, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_GET_USERS_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_GET_CISTERNS_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_GET_OBSERVERS_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()}
                                ,{Poco::Net::HTTPRequest::HTTP_POST, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_VERSION_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_OBSERVERS_ITEM_PATH, {{Poco::Net::HTTPRequest::HTTP_PUT, std::vector<mdbcom::MDBContentDecodingFormat>()},
                                      {Poco::Net::HTTPRequest::HTTP_DELETE, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_ORGANISM_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()},
                            {Poco::Net::HTTPRequest::HTTP_PUT, std::vector<mdbcom::MDBContentDecodingFormat>()},
                            {Poco::Net::HTTPRequest::HTTP_DELETE, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_ORGANISM_LIST_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()},
                                {Poco::Net::HTTPRequest::HTTP_POST, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
        {URI_TASKS_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()},
                                {Poco::Net::HTTPRequest::HTTP_POST, std::vector<mdbcom::MDBContentDecodingFormat>()}}}, 
        {URI_TASK_PATH, {{Poco::Net::HTTPRequest::HTTP_DELETE, std::vector<mdbcom::MDBContentDecodingFormat>()},
                        {Poco::Net::HTTPRequest::HTTP_PUT, std::vector<mdbcom::MDBContentDecodingFormat>()},
                        {Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()}}},
		{URI_ALL_TASKS_PATH, {{Poco::Net::HTTPRequest::HTTP_GET, std::vector<mdbcom::MDBContentDecodingFormat>()}}}
    };

    /*!
     * @brief The constructor.
     * @param [in] templateConfigFile Path to template configuration file
     * @return None.
     */
    FAMSDURequestHandlerFactory::FAMSDURequestHandlerFactory(const std::string &templateConfigFile)
    {
        auto templateConf = mdbcom::MDBYamlConfig { templateConfigFile }; /* pgr0745 */

        auto itKey = requestTemplates.begin();
        while(itKey != requestTemplates.end())
        {
            auto itVal = itKey->second.begin();
            while(itVal != itKey->second.end())
            {
                if (templateConf.getFormat(itKey->first, Poco::toLower(itVal->first), itVal->second) != mdbcom::Result::OK) {
                    throw mdbcom::MDBException { "Can't get format template for request " + itKey->first + " - method " + itVal->first };
                }
                itVal++;
            }
            itKey++;
        }
    }

    /*!
     * @brief The destructor.
     * @param None.
     * @return None.
     */
    FAMSDURequestHandlerFactory::~FAMSDURequestHandlerFactory()
    {
    }

    /*!
     * @brief Create handler thread then call handler
     *
     * @param [in] request the HTTP request
     * @return a pointer to request handler
     */
    Poco::Net::HTTPRequestHandler *FAMSDURequestHandlerFactory::createRequestHandler(
        const Poco::Net::HTTPServerRequest &request)
    {
        mdb_apl_log_start();

        auto handler = static_cast<Poco::Net::HTTPRequestHandler *>(nullptr); /* pgr0745 */

        // the final URI to jugde REST API
        auto uriPath = std::string { }; /* pgr0745 */
        auto resHttpStatus = Poco::Net::HTTPResponse::HTTP_FORBIDDEN; /* pgr0745 */

        do
        {
            try {
                // Extract URI path only
                auto uri = mdbcom::MDBURIHelper { request.getURI() }; /* pgr0745 */
                uriPath = uri.getPath();
            } catch (...) {
                mdb_apl_log_error("Parse URI has error");
                break;
            }

            // Get request information
            auto method = request.getMethod().c_str(); /* pgr0745 */
            auto tmpUri = uriPath.c_str(); /* pgr0745 */
            auto contentLength = request.getContentLength(); /* pgr0745 */

            mdb_apl_log_info("Method(%s) URI(%s) Content-length(%ld)", method, tmpUri, contentLength);

            // Authenticate request
            const bool &hasCredentialsFlg = request.hasCredentials();
            if(famscommon::checkExistence(skipVerifyAuthLst, uriPath) == false)
            {
                if (!hasCredentialsFlg) {
                    mdb_apl_log_error("User doesn't have credential value");
                    // TODO: In PoC, we just write error messages, don't throw exception
                } else {
                    try {
                        Poco::Net::HTTPBasicCredentials cred(request);
                        const std::string user = cred.getUsername();
                        const std::string pwd = cred.getPassword();
                        FAMSDUUserData userData;
                        if (authenticate(user, pwd, userData) == mdbcom::Result::NG) {
                            mdb_apl_log_error("Invalid credential value");
                            break;
                        } else {
                            mdb_apl_log_info("Authenticated successful");
                        }
                    } catch (const Poco::Net::NotAuthenticatedException &ex) {
                        mdb_apl_log_error("Validating Authentication got failed:(%s)", ex.what());
                    }
                }
            }

            try {
                if (Poco::RegularExpression(famscommon::replaceParentheses(URI_LOGIN_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_LOGIN_PATH)->second;
                    if (urlDetail.find(request.getMethod()) == urlDetail.end()) {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                        // TODO: Return error
                        break;
                    }
                    handler = new FAMSDUUserHandler(urlDetail.find(request.getMethod())->second, RequestType::LOGIN_POST);
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_GET_USERS_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_GET_USERS_PATH)->second;
                    if (urlDetail.find(request.getMethod()) == urlDetail.end()) {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                        break;
                    }
                    handler = new FAMSDUUserHandler(urlDetail.find(request.getMethod())->second, RequestType::USERS_GET);
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_OBSERVERS_ITEM_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_OBSERVERS_ITEM_PATH)->second;
                    if (request.getMethod() == "PUT") {
                        handler = new FAMSDUObserversHandler(urlDetail.find(request.getMethod())->second, RequestType::OBSERVERS_PUT);
                    }
                    else if (request.getMethod() == "DELETE")
                    {
                        handler = new FAMSDUObserversHandler(urlDetail.find(request.getMethod())->second, RequestType::OBSERVERS_DELETE);
                    }
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_GET_OBSERVERS_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_GET_OBSERVERS_PATH)->second;
                    if (request.getMethod() == "GET") {
                        handler = new FAMSDUObserversHandler(urlDetail.find(request.getMethod())->second, RequestType::OBSERVERS_GET);
                    } else if (request.getMethod() == "POST") {
                        handler = new FAMSDUObserversHandler(urlDetail.find(request.getMethod())->second, RequestType::OBSERVERS_POST);
                    } else if (request.getMethod() == "DELETE") {
                        handler = new FAMSDUObserversHandler(urlDetail.find(request.getMethod())->second, RequestType::OBSERVERS_DELETE);
                    }
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_ORGANISM_LIST_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_ORGANISM_LIST_PATH)->second;
                    if (urlDetail.find(request.getMethod()) == urlDetail.end()) {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                        // TODO: Return error
                        break;
                    }
                    if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
                        mdb_apl_log_trace("HTTP_POST method");
                        handler = new FAMSDUOrganismHandler(urlDetail.find(request.getMethod())->second, RequestType::ORGANISM_LIST_POST);
                    }
                    else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
                    {
                        mdb_apl_log_trace("HTTP_GET method");
                        handler = new FAMSDUOrganismHandler(urlDetail.find(request.getMethod())->second, RequestType::ORGANISM_LIST_GET);
                    }
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_GET_CISTERNS_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_GET_CISTERNS_PATH)->second;
                    if (urlDetail.find(request.getMethod()) == urlDetail.end()) {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                        break;
                    }
                    handler = new FAMSDUCisternHandler(urlDetail.find(request.getMethod())->second, RequestType::CISTERNS_GET);
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_ORGANISM_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_ORGANISM_PATH)->second;
                    if (urlDetail.find(request.getMethod()) == urlDetail.end()) {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                        // TODO: Return error
                        break;
                    }
                    if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_DELETE) {
                        mdb_apl_log_trace("HTTP_DELETE method");
                        handler = new FAMSDUOrganismHandler(urlDetail.find(request.getMethod())->second, RequestType::ORGANISM_DELETE);
                    }
                    else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                        mdb_apl_log_trace("HTTP_GET method");
                        handler = new FAMSDUOrganismHandler(urlDetail.find(request.getMethod())->second, RequestType::ORGANISM_GET);
                    }
                    else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT) {
                        mdb_apl_log_trace("HTTP_PUT method");
                        handler = new FAMSDUOrganismHandler(urlDetail.find(request.getMethod())->second, RequestType::ORGANISM_PUT);
                    }
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_TASKS_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_TASKS_PATH)->second;
                    if (urlDetail.find(request.getMethod()) == urlDetail.end()) {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                        break;
                    }
                    if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                        mdb_apl_log_trace("HTTP_DELETE method");
                        handler = new FAMSDUTaskHandler(urlDetail.find(request.getMethod())->second, RequestType::TASKS_GET);
                    } else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
                        mdb_apl_log_trace("HTTP_POST method");
                        handler = new FAMSDUTaskHandler(urlDetail.find(request.getMethod())->second, RequestType::TASKS_POST);
                    }
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_TASK_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_TASK_PATH)->second;
                    if (urlDetail.find(request.getMethod()) == urlDetail.end()) {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                        break;
                    }

                    if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_DELETE) {
                        handler = new FAMSDUTaskHandler(urlDetail.find(request.getMethod())->second, RequestType::TASK_DELETE);
                    } else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                        handler = new FAMSDUTaskHandler(urlDetail.find(request.getMethod())->second, RequestType::TASK_GET);
                    } else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT) {
                        handler = new FAMSDUTaskHandler(urlDetail.find(request.getMethod())->second, RequestType::TASK_PUT);
                    }
                }
                else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_VERSION_PATH)).match(uriPath))
                {
                    auto urlDetail = requestTemplates.find(URI_VERSION_PATH)->second;
                    if (request.getMethod() == "GET") {
                        handler = new FAMSDUVersionHandler(urlDetail.find(request.getMethod())->second, RequestType::VERSION_GET);
                    } else {
                        mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
                    }
                }
				else if (Poco::RegularExpression(famscommon::replaceParentheses(URI_ALL_TASKS_PATH)).match(uriPath))
				{
					auto urlDetail = requestTemplates.find(URI_ALL_TASKS_PATH)->second;
					if (urlDetail.find(request.getMethod()) == urlDetail.end()) 
					{
						mdb_apl_log_error("Method " + request.getMethod() + " isn't supported");
						break;
					}
					if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
					{
						handler = new FAMSDUTaskHandler(urlDetail.find(request.getMethod())->second, RequestType::ALL_TASKS_GET);
					}

				}
                else
                {
                    mdb_apl_log_error("Not match uri");
                    handler = new mdbcom::MDBResourceNotFoundHandler(resHttpStatus);
                }
            } catch (const std::bad_alloc &ex) {
                mdb_apl_log_critical("Can't allocate new handler [%s] for [%s]", ex.what(), tmpUri);
                break;
            } catch (...) {
                mdb_apl_log_critical("Can't allocate new handler [unknown error]");
                break;
            }
        } while (0);

        mdb_apl_log_end();
        return handler;
    }
}

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBResourceNotFoundHandler.cpp
 * @brief       Source file for implementing MDBResourceNotFoundHandler information.
 * @author      trangvp: create it for support MDBResourceNotFoundHandler code.
 * @date        2016/12/09
 */

#include "MDBResourceNotFoundHandler.h"
#include "MDBConstants.h"

namespace mdbcom {

    // Constant
    const std::string MDBResourceNotFoundHandler::HTTP_CONTENT_TYPE_TEXT
    {
        "text/html"
    };
    const std::string MDBResourceNotFoundHandler::HTTP_NOT_FOUND { "404 not found" };
    const std::string MDBResourceNotFoundHandler::HTTP_FORBIDDEN { "403 forbidden" };
    //Common BODY
    const std::string MDBResourceNotFoundHandler::HTTP_BODY_COMMON =
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
        "<html><head>\n"
        "<title> ";

    //NOT_FOUND BODY1
    const std::string MDBResourceNotFoundHandler::HTTP_BODY_NOT_FOUND1 =
        " </title>\n"
        "</head><body>\n"
        "<h1>Not Found</h1>\n"
        "<p>The requested URL ";
    //NOT_FOUND BODY2
    const std::string MDBResourceNotFoundHandler::HTTP_BODY_NOT_FOUND2 =
        " was not found on this server.</p>\n"
        "</body></html>\n";

    //FORBIDDEN BODY
    const std::string MDBResourceNotFoundHandler::HTTP_BODY_FORBIDDEN =
        " </title>\n"
        "</head><body>\n"
        "<h1>Forbidden</h1>\n"
        "<p>The requested method was not found on this server.</p>\n"
        "</body></html>\n";

    /*!
     * @brief The constructor.
     * @param [in] HTTP Response Status.
     * @return None.
     */
    MDBResourceNotFoundHandler::MDBResourceNotFoundHandler(const int status):
        mResHttpStatus {status}
    {
    }

    /*!
     * @brief The destructor.
     * @param None.
     * @return None.
     */
    MDBResourceNotFoundHandler::~MDBResourceNotFoundHandler(void)
    {

    }

    /*!
     *  @brief The main method for process resource not found error.
     *
     *  @param [in] request the HTTP request data
     *  @param [out] response the HTTP response data
     *
     *  @return None.
     */
    void MDBResourceNotFoundHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        mdb_sys_log_out("F001001");
        // By pass request
        request.setContentLength(0);

        auto res = HTTP_BODY_COMMON; /* pgr0745 */
        //Select HTTP Respons Status
        auto resStatus = std::string { }; /* pgr0745 */

        switch (mResHttpStatus)
        {
            case Poco::Net::HTTPResponse::HTTP_FORBIDDEN:
                {
                    res += HTTP_FORBIDDEN;
                    res += HTTP_BODY_FORBIDDEN;
                    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_FORBIDDEN);
                    break;
                }

            case Poco::Net::HTTPResponse::HTTP_NOT_FOUND:
                {
                    res += HTTP_NOT_FOUND;
                    res += HTTP_BODY_NOT_FOUND1;
                    //Get URI
                    auto uriPath = std::string { }; /* pgr0745 */

                    try
                    {
                        auto uri = Poco::URI { request.getURI() }; /* pgr0745 */
                        uriPath = uri.getPath();
                        res += uriPath.c_str();
                    }
                    catch (...)
                    {
                        mdb_apl_log_error("Parse URI has error");
                        mdb_sys_log_out("F001002");
                    }

                    res += HTTP_BODY_NOT_FOUND2;
                    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    break;
                }

            default:
                {
                    mdb_apl_log_critical("Failed HTTP Reaponse status (%d)", mResHttpStatus);
                    res += HTTP_NOT_FOUND;
                    res += HTTP_BODY_NOT_FOUND1;
                    res += HTTP_BODY_NOT_FOUND2;
                    response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    break;
                }
        }

        response.setContentType(HTTP_CONTENT_TYPE_TEXT);
        response.setContentLength(res.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /* pgr0745 */
        ostr << res;
        mdb_apl_log_end();
    }
}

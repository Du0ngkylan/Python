/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBResourceNotFoundHandler.h
 * @brief       Header file for define MDBResourceNotFoundHandler class.
 * @author      trangvp: create it for supporting define MDBResourceNotFoundHandler class.
 * @date        2016/12/09
 */

#ifndef LIBMDBHTTP_RESOURCENOTFOUNDHANDLER_H_
#define LIBMDBHTTP_RESOURCENOTFOUNDHANDLER_H_

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/URI.h>

#include "MDBLogger.h"

namespace mdbcom {
    /*!
     * @class MDBResourceNotFoundHandler
     * @brief The class handle invalid URI - 404 error
     */
    class MDBResourceNotFoundHandler: public Poco::Net::HTTPRequestHandler { /* pgr2252 */
    public:
        /*!
         * @brief The constructor.
         * @param [in] Http status
         * @return None.
         */
        explicit MDBResourceNotFoundHandler(const int status);

        /*!
         * @brief The destructor.
         * @param None.
         * @return None.
         */
        ~MDBResourceNotFoundHandler(void);

        /*!
         * @brief The constructor.
         * @param None.
         * @return None.
         */
        MDBResourceNotFoundHandler(const MDBResourceNotFoundHandler &) = delete; /*pgr0571*/ /*pgr2222*/

        /*!
         * @brief The operator=.
         * @param None.
         * @return None.
         */
        MDBResourceNotFoundHandler &operator=(const MDBResourceNotFoundHandler &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         *  @brief The main method for process resource not found error.
         *
         *  @param [in] request the HTTP request data
         *  @param [out] response the HTTP response data
         *
         *  @return None.
         */
        void handleRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response);
    private:
        //! Constant for HTTP content type
        static const std::string HTTP_CONTENT_TYPE_TEXT;

        //! Constant for HTTP not found
        static const std::string HTTP_NOT_FOUND;

        static const std::string HTTP_FORBIDDEN;
        static const std::string HTTP_BODY_COMMON;
        static const std::string HTTP_BODY_NOT_FOUND1;
        static const std::string HTTP_BODY_NOT_FOUND2;
        static const std::string HTTP_BODY_FORBIDDEN;
        int mResHttpStatus;

    };
}

#endif /* LIBMDBHTTP_RESOURCENOTFOUNDHANDLER_H_ */

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBRequestHandlerFactory.h
 * @brief       Header file for define MDBRequestHandlerFactory class.
 * @author      trangvp: create it for supporting define MDBRequestHandlerFactory class.
 * @date        2016/10/04
 */

#ifndef LIBMDBHTTP_REQUESTHANDLERFACTORY_H_
#define LIBMDBHTTP_REQUESTHANDLERFACTORY_H_

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>

#include "MDBResourceMapper.h"

namespace mdbcom {
    /*! @class MDBRequestHandlerFactory
     *   @brief This class support for preparing debug logger, DB connection
     */
    class MDBRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory { /* pgr2252 */
    public:
        /*!
         * @brief The Constructor.
         *
         * @param [in] factory Actual factory class
         * @param [in] resourceMapper Object to manage db connections.
         *
         * @return None.
         */
        MDBRequestHandlerFactory(Poco::Net::HTTPRequestHandlerFactory *factory, MDBResourceMapper *resourceMapper =
                                     nullptr);

        /*!
         * @brief The constructor.
         * @param None.
         * @return None.
         */
        MDBRequestHandlerFactory(const MDBRequestHandlerFactory &) = delete; /*pgr0571*/ /*pgr2222*/

        /*!
         * @brief The operator=.
         * @param None.
         * @return None.
         */
        MDBRequestHandlerFactory &operator=(const MDBRequestHandlerFactory &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief The destructor.
         *
         * @param None.
         * @return None.
         */
        ~MDBRequestHandlerFactory(void) = default;

        /*!
         * @brief Init resources for handler thread then call handler
         *
         * @param [in] request the HTTP request
         * @return a pointer to request handler
         */
        Poco::Net::HTTPRequestHandler *createRequestHandler(
            const Poco::Net::HTTPServerRequest &request);

    private:
        //! Thread name for http handler thread
        static const std::string HANDLER_THREAD_NAME;

        //! Actual Request Handler factory
        Poco::Net::HTTPRequestHandlerFactory *pFactory;

        //! Object to manage db connections
        MDBResourceMapper *resources;

        //! Flag for checking if thread is initialized
        static thread_local bool isInitialized;
    };
}

#endif /* LIBMDBHTTP_REQUESTHANDLERFACTORY_H_ */

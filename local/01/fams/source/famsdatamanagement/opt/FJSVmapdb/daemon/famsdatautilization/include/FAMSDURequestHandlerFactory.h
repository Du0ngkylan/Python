/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURequestHandlerFactory.h
 * @brief       handler factory file for create ApIF request handlers
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_REQUEST_HANDLER_FACTORY_H_
#define FAMS_DATA_UTIL_REQUEST_HANDLER_FACTORY_H_
#include "FAMSDUBaseHandler.h"
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/RegularExpression.h>
#include "MDBContentDecodingFormat.h"

namespace famsdatautil {
    /*! @class FAMSDURequestHandlerFactory
     *  @brief This class support for URI mapping
     */
    class FAMSDURequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory { /*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] templateConfigFile Path to template configuration file
         * @return None.
         */
        explicit FAMSDURequestHandlerFactory(const std::string &templateConfigFile);

        /*!
         * @brief The destructor.
         * @param None.
         * @return None.
         */
        ~FAMSDURequestHandlerFactory(void);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDURequestHandlerFactory(const FAMSDURequestHandlerFactory &) = delete; /*pgr2222*//*pgr0571*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDURequestHandlerFactory &operator=(const FAMSDURequestHandlerFactory &) = delete; /*pgr0055*//*pgr2223*//*pgr0309*/

        /*!
         * @brief Parse URI, mapping and return a specified handler.
         *
         * @param [in] request the HTTP request
         * @return a pointer to request handler
         */
        Poco::Net::HTTPRequestHandler *createRequestHandler(
            const Poco::Net::HTTPServerRequest &request);
    private:
        //! decode template for get user info request
        static std::map<std::string, std::map<std::string, std::vector<mdbcom::MDBContentDecodingFormat>>> requestTemplates;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_HANDLER_FACTORY_H_ */

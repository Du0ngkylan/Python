/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRRequestHandlerFactory.h
 * @brief       Header file for define FAMSDRRequestHandlerFactory class.
 * @author      anhnguyen
 * @date        2018/12/11
 */

#ifndef FAMS_DATA_RECEPTION_REQUEST_HANDLER_FACTORY_H_
#define FAMS_DATA_RECEPTION_REQUEST_HANDLER_FACTORY_H_
#include "FAMSDRBaseHandler.h"
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/RegularExpression.h>
#include "MDBContentDecodingFormat.h"
#include "MDBYamlConfig.h"

namespace famsdatareception {
    /*! @class FAMSDRRequestHandlerFactory
     *  @brief This class support for URI mapping
     */
    class FAMSDRRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory {
    public:
        /*!
         * @brief The Constructor.
         * @param [in] templateConfigFile Path to template configuration file
         * @return None.
         */
        explicit FAMSDRRequestHandlerFactory(mdbcom::MDBIPCShm *sharedMemory, const std::string &templateFile);

        /*!
         * @brief The destructor.
         * @param None.
         * @return None.
         */
        ~FAMSDRRequestHandlerFactory(void);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRRequestHandlerFactory(const FAMSDRRequestHandlerFactory &) = delete; /*pgr0571*/ /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRRequestHandlerFactory &operator=(const FAMSDRRequestHandlerFactory &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief Parse URI, mapping and return a specified handler.
         *
         * @param [in] request the HTTP request
         * @return a pointer to request handler
         */
        Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request);
    private:

        mdbcom::MDBIPCShm *mSharedMemory;
        //! sensor tempalate
		std::vector<mdbcom::MDBContentDecodingFormat> mSensornAddTemplate;
    };
}

#endif /* DYNAMICOBJECTREQUESTHANDLERFACTORY_H_ */

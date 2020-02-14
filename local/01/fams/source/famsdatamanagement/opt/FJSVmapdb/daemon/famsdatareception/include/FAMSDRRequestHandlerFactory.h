/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRWaterTankRequestHandlerFactory.h
 * @brief       Header file for define FAMSDRWaterTankRequestHandlerFactory class.
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
    /*! @class FAMSDRWaterTankRequestHandlerFactory
     *  @brief This class support for URI mapping
     */
    class FAMSDRWaterTankRequestHandlerFactory: public Poco::Net::HTTPRequestHandlerFactory {
    public:
        /*!
         * @brief The Constructor.
         * @param [in] templateConfigFile Path to template configuration file
         * @return None.
         */
        explicit FAMSDRWaterTankRequestHandlerFactory(mdbcom::MDBIPCShm *sharedMemory, const std::string &templateFile);

        /*!
         * @brief The destructor.
         * @param None.
         * @return None.
         */
        ~FAMSDRWaterTankRequestHandlerFactory(void);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRWaterTankRequestHandlerFactory(const FAMSDRWaterTankRequestHandlerFactory &) = delete; /*pgr0571*/ /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRWaterTankRequestHandlerFactory &operator=(const FAMSDRWaterTankRequestHandlerFactory &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief Parse URI, mapping and return a specified handler.
         *
         * @param [in] request the HTTP request
         * @return a pointer to request handler
         */
        Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request);
    private:

        mdbcom::MDBIPCShm *mSharedMemory;
        //cuongph change to map
        //item 1: url request
        //item2 mReplaceTTemplate
        //! sensor tempalate
        std::vector<mdbcom::MDBContentDecodingFormat> mReplaceTTemplate;
        std::vector<mdbcom::MDBContentDecodingFormat> mNitrificationTemplate;
        std::vector<mdbcom::MDBContentDecodingFormat> mOutSideTemplate;
        std::vector<mdbcom::MDBContentDecodingFormat> sensorCisternTemplate;
    };
}

#endif /* DYNAMICOBJECTREQUESTHANDLERFACTORY_H_ */

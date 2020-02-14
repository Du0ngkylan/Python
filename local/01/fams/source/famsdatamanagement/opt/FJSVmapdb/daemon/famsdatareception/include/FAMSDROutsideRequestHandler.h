/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*
 * @file		FAMSDROutsideRequestHandler.h
 * @brief		Header for class FAMSDROutsideRequestHandler
 * @author		anhnguyen
 * @date        2018/12/11
 */

#ifndef FAMS_DATA_RECEIVER_OUTSIDE_REQUEST_HANDLER_H_
#define FAMS_DATA_RECEIVER_OUTSIDE_REQUEST_HANDLER_H_

#include <future>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "MDBJsonParser.h"
#include "FAMSOutsideData.h"
#include "FAMSDRBaseHandler.h"

namespace famsdatareception {
    /*!
     * @class FAMSDROutsideRequestHandler
     * @brief The class supports Mod MessageType(Object) info at MAPDB.
     */
    class FAMSDROutsideRequestHandler: public FAMSDRBaseHandler { /*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDROutsideRequestHandler(mdbcom::MDBIPCShm *sharedMemory,
                const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDROutsideRequestHandler(
            const FAMSDROutsideRequestHandler &) = delete; /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDROutsideRequestHandler &operator=(
            const FAMSDROutsideRequestHandler &) = delete; /*pgr0055*//*pgr0309*//*pgr2223*/

        int validateJSONFormat(const std::string &jsonString,
                                       mdbcom::MDBContentParser::KeyMapType &jsonMap,
                                       std::vector<std::string> &errKeys);

        /*!
         * @brief The destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDROutsideRequestHandler(void) = default;
        /*!
         * @brief Register an event processing
         *
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        virtual int registerEvent(void);

    private:

        //! the request data
        famscommon::FAMSOutsideData mRequestData;

        /*!
         * @brief Get Obstacle buffer and sise
         * @param [in] eventType event type to send to queue
         * @param [out] dataLength Length of data
         * @retval pointer to ipc buffer prepared to be sent
         */
        virtual std::unique_ptr<famscommon::FAMSIpcHeader>
        getIpcInfoData(const famscommon::MessageType eventType, std::size_t &dataLength) override;

    };

}

#endif /* FAMS_DATA_RECEIVER_OUTSIDE_REQUEST_HANDLER_H_ */

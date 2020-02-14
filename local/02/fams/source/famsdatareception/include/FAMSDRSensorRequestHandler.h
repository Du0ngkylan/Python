/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*
 * @file		FAMSDRSensorRequestHandler.h
 * @brief		Header for class FAMSDRSensorRequestHandler 
 * @author		thanglq
 * @date        2019/10/14
 */

#ifndef FAMS_DATA_RECEIVER_SENSOR_REQUEST_HANDLER_H_
#define FAMS_DATA_RECEIVER_SENSOR_REQUEST_HANDLER_H_

#include <future>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "MDBJsonParser.h"
#include "FAMSSensorData.h"
#include "FAMSDRBaseHandler.h"

namespace famsdatareception {
    /*!
     * @class FAMSDRSensorRequestHandler 
     * @brief The class supports Mod MessageType(Object) info at MAPDB.
     */
    class FAMSDRSensorRequestHandler : public FAMSDRBaseHandler { /*pgr2252*/
    public:
        /*!
         * @brief The Constructor.
         * @param [in] inputTemplate Template for request input format
         * @return None.
         */
        explicit FAMSDRSensorRequestHandler (mdbcom::MDBIPCShm *sharedMemory,
                const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate);

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRSensorRequestHandler (
            const FAMSDRSensorRequestHandler  &) = delete; /*pgr2222*/

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSDRSensorRequestHandler  &operator=(
            const FAMSDRSensorRequestHandler  &) = delete; /*pgr0055*//*pgr0309*//*pgr2223*/

        int validateJSONFormat(const std::string &jsonString,
                                       mdbcom::MDBContentParser::KeyMapType &jsonMap,
                                       std::vector<std::string> &errKeys);

        /*!
         * @brief The destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSDRSensorRequestHandler (void) = default;

        /*!
         * @brief Register an event processing
         *
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        virtual int registerEvent(void);

    private:

        //! the request data
        famscommon::FAMSSensorData mRequestData;

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

#endif /* FAMS_DATA_RECEIVER_SENSOR_REQUEST_HANDLER_H_ */

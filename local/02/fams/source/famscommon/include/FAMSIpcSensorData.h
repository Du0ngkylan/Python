/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcSensorData.h
 * @brief       Header file of class FAMSIpcSensorData.
 * @author      thanglq
 * @date        2019/10/14
 */

#include <array>
#include <map>
#include <string>
#include <memory>

#include "MDBLogger.h"
#include "FAMSIpcHeader.h"
#include "FAMSSensorData.h"

#ifndef FAMS_IPC_SENSOR_DATA_H_
#define FAMS_IPC_SENSOR_DATA_H_

namespace famscommon {

    /*!
     * @class FAMSIpcSensorData
     * @brief Data structure to transfer information via IPC
     */
    class FAMSIpcSensorData {
    public:
        /*!
         * @brief Constructor.
         * @param [in] eventType event type to send to queue
         * @param [in] request Request data
         * @return None.
         */
        FAMSIpcSensorData(famscommon::MessageType eventType, const famscommon::FAMSSensorData &request);

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~FAMSIpcSensorData(void) = default;

        /*!
         * @brief Copy constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcSensorData(const FAMSIpcSensorData &) = default;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return This FAMSIpcSensorData.
         */
        FAMSIpcSensorData &operator=(const FAMSIpcSensorData &) = default;

        /*!
         * @brief Move constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcSensorData(FAMSIpcSensorData &&) = default;

        /*!
         * @brief move assignment.
         * @param None.
         * @return This FAMSIpcNitrificationData.
         */
        FAMSIpcSensorData &operator=(FAMSIpcSensorData &&) = default;

        /*!
         * @brief Override default operator new.
         * This operator can allocated more memory than actual class size
         * Actual memory size allocated = size + extendSize
         *
         * @param [in] size Size of class (bytes)
         * @param [in] extendSize Extended size (bytes)
         * @return Pointer to new memory location.
         */
        static void *operator new (std::size_t size, std::size_t extendSize = 0);

        /*!
         * @brief Override default operator delete.
         * Override to keep the type consistency of delete behavior.
         *
         * @param [in] pointer Memory location to be deleted
         * @return None.
         */
        static void operator delete (void *pointer);

        std::size_t fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime);
        const char *getAccumulatedTime(std::size_t &size) const;

        std::size_t fillCisternCode(std::size_t baseOffset,const std::vector<std::string> &cisternCode);
        const char *getCisternCode(std::size_t &size) const;

        std::size_t fillType(std::size_t baseOffset,const std::vector<int> &type);
        const int *getType(std::size_t &size) const;

        std::size_t fillValue(std::size_t baseOffset,const std::vector<double> &value);
        const double *getValue(std::size_t &size) const;

        std::size_t fillUnit(std::size_t baseOffset,const std::vector<std::string> &unit);
        const char *getUnit(std::size_t &size) const;
        // DATA definition
        // with raw fields (int, long): declare normally
        // with string: declare in char array with fixed length
        // with list, binary data: declare in from [offset and size]
        //! The base data
        FAMSIpcHeader ipcHeader;

        std::size_t mAccumulatedTimeOffset;
        std::size_t mAccumulatedTimeSize;
        std::size_t mCisternCodeOffset;
        std::size_t mCisternCodeSize;
        std::size_t mTypeOffset;
        std::size_t mTypeSize;
        std::size_t mValueOffset;
        std::size_t mValueSize;
        std::size_t mUnitOffset;
        std::size_t mUnitSize;
    private:

    };
}
#endif /* FAMS_IPC_SENSOR_DATA_H_ */

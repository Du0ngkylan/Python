/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcNitrificationData.h
 * @brief       Header file of class FAMSIpcNitrificationData.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include <array>
#include <map>
#include <string>
#include <memory>

#include "MDBLogger.h"
#include "FAMSIpcHeader.h"
#include "FAMSCisternCensorData.h"

#ifndef FAMS_IPC_CISTERN_SENSOR_DATA_H_
#define FAMS_IPC_CISTERN_SENSOR_DATA_H_

namespace famscommon {

    /*!
     * @class FAMSIpcCisternSensorData
     * @brief Data structure to transfer information via IPC
     */
    class FAMSIpcCisternSensorData {
    public:
        /*!
         * @brief Constructor.
         * @param [in] eventType event type to send to queue
         * @param [in] request Request data
         * @return None.
         */
        FAMSIpcCisternSensorData(famscommon::MessageType eventType, const famscommon::FAMSCisternSenSorData &request);

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~FAMSIpcCisternSensorData(void) = default;

        /*!
         * @brief Copy constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcCisternSensorData(const FAMSIpcCisternSensorData &) = default;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return This FAMSIpcNitrificationData.
         */
        FAMSIpcCisternSensorData &operator=(const FAMSIpcCisternSensorData &) = default;

        /*!
         * @brief Move constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcCisternSensorData(FAMSIpcCisternSensorData &&) = default;

        /*!
         * @brief move assignment.
         * @param None.
         * @return This FAMSIpcNitrificationData.
         */
        FAMSIpcCisternSensorData &operator=(FAMSIpcCisternSensorData &&) = default;

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

        std::size_t fillSalt(std::size_t baseOffset,const std::vector<double> &salt);
        const double *getSalt(std::size_t &size) const;
        std::size_t fillPH(std::size_t baseOffset,const std::vector<double> &pH);
        const double *getPH(std::size_t &size) const;
        std::size_t fillInflowTemp(std::size_t baseOffset,const std::vector<double> &inflowTemp);
        const double *getInflowTemp(std::size_t &size) const;
        std::size_t fillOutflowTemp(std::size_t baseOffset,const std::vector<double> &outflowTemp);
        const double *getOutflowTemp(std::size_t &size) const;

        std::size_t fillUpperCentralIll(std::size_t baseOffset,const std::vector<int> &upperCentralIll);
        const int *getUpperCentralIll(std::size_t &size) const;
        std::size_t fillLowerCentralIll(std::size_t baseOffset,const std::vector<int> &LowerCentralIll);
        const int *getLowerCentralIll(std::size_t &size) const;

        std::size_t fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime);
        const char *getAccumulatedTime(std::size_t &size) const;
        std::size_t fillCisternCode(std::size_t baseOffset,const std::vector<std::string> &cisternCode);
        const char *getCisternCode(std::size_t &size) const;
        // DATA definition
        // with raw fields (int, long): declare normally
        // with string: declare in char array with fixed length
        // with list, binary data: declare in from [offset and size]
        //! The base data
        FAMSIpcHeader ipcHeader;

        std::size_t mAccumulatedTimeListOffset;
        std::size_t mAccumulatedTimeListSize;
        std::size_t mCisternCodeOffset;
        std::size_t mCisternCodeSize;
        std::size_t mInflowTempOffset;
        std::size_t mInflowTempSize;
        std::size_t mOutflowTempOffset;
        std::size_t mOutflowTempSize;
        std::size_t mSaltOffset;
        std::size_t mSaltSize;
        std::size_t mPHOffset;
        std::size_t mPHSize;
        std::size_t mLowerCentralIllOffset;
        std::size_t mLowerCentralIllSize;
        std::size_t mUpperCentralIllOffset;
        std::size_t mUpperCentralIllSize;
    private:

    };
}
#endif /* FAMS_IPC_NITRIFICATION_INFO_DATA_H_ */

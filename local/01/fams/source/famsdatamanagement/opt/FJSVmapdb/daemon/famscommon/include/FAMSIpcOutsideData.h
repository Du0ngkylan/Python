/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcOutsideData.h
 * @brief       Header file of class FAMSIpcOutsideData.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include <array>
#include <map>
#include <string>
#include <memory>

#include "MDBLogger.h"
#include "FAMSIpcHeader.h"
#include "FAMSOutsideData.h"

#ifndef FAMS_IPC_OUTSIDE_INFO_DATA_H_
#define FAMS_IPC_OUTSIDE_INFO_DATA_H_

namespace famscommon {

    /*!
     * @class FAMSIpcOutsideData
     * @brief Data structure to transfer vehicle information via IPC
     */
    class FAMSIpcOutsideData {
    public:
        /*!
         * @brief Constructor.
         * @param [in] eventType event type to send to queue
         * @param [in] request Request data
         * @return None.
         */
        FAMSIpcOutsideData(famscommon::MessageType eventType, const famscommon::FAMSOutsideData &request);

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~FAMSIpcOutsideData(void) = default;

        /*!
         * @brief Copy constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcOutsideData(const FAMSIpcOutsideData &) = default;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return This FAMSIpcOutsideData.
         */
        FAMSIpcOutsideData &operator=(const FAMSIpcOutsideData &) = default;

        /*!
         * @brief Move constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcOutsideData(FAMSIpcOutsideData &&) = default;

        /*!
         * @brief move assignment.
         * @param None.
         * @return This FAMSIpcOutsideData.
         */
        FAMSIpcOutsideData &operator=(FAMSIpcOutsideData &&) = default;

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

        std::size_t fillRoomTemp(std::size_t baseOffset,const std::vector<double> &room_temp);
        const double *getRoomTemp(std::size_t &size) const;
        std::size_t fillHumidity(std::size_t baseOffset,const std::vector<double> &humidity);
        const double *getHumidity(std::size_t &size) const;
        std::size_t fillAtmosphericPressure(std::size_t baseOffset,const std::vector<double> &atmospheric_pressure);
        const double *getAtmosphericPressure(std::size_t &size) const;
        std::size_t fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime);
        const char *getAccumulatedTime(std::size_t &size) const;
        // DATA definition
        // with raw fields (int, long): declare normally
        // with string: declare in char array with fixed length
        // with list, binary data: declare in from [offset and size]
        //! The base data
        FAMSIpcHeader ipcHeader;

        std::size_t mRoomTempListOffset;
        std::size_t mRoomTempListSize;
        std::size_t mAccumulatedTimeListOffset;
        std::size_t mAccumulatedTimeListSize;
        std::size_t mHumidityListOffset;
        std::size_t mHumidityListSize;
        std::size_t mAtmosphericPressureListSize;
        std::size_t mAtmosphericPressureListOffset;
    private:

    };
}
#endif /* FAMS_IPC_OUTSIDE_INFO_DATA_H_ */

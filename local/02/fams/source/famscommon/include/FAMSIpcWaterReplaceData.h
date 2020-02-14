/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcWaterReplaceData.h
 * @brief       Header file of class FAMSIpcWaterReplaceData.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include <array>
#include <map>
#include <string>
#include <memory>

#include "MDBLogger.h"
#include "FAMSIpcHeader.h"
#include "FAMSWaterReplaceData.h"

#ifndef FAMS_IPC_WATER_REPLACE_INFO_DATA_H_
#define FAMS_IPC_WATER_REPLACE_INFO_DATA_H_

namespace famscommon {

    /*!
     * @class FAMSIpcWaterReplaceData
     * @brief Data structure to transfer vehicle information via IPC
     */
    class FAMSIpcWaterReplaceData {
    public:
        /*!
         * @brief Constructor.
         * @param [in] eventType event type to send to queue
         * @param [in] request Request data
         * @return None.
         */
        FAMSIpcWaterReplaceData(famscommon::MessageType eventType, const famscommon::FAMSWaterReplaceData &request);

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~FAMSIpcWaterReplaceData(void) = default;

        /*!
         * @brief Copy constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcWaterReplaceData(const FAMSIpcWaterReplaceData &) = default;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return This FAMSIpcWaterReplaceData.
         */
        FAMSIpcWaterReplaceData &operator=(const FAMSIpcWaterReplaceData &) = default;

        /*!
         * @brief Move constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcWaterReplaceData(FAMSIpcWaterReplaceData &&) = default;

        /*!
         * @brief move assignment.
         * @param None.
         * @return This FAMSIpcWaterReplaceData.
         */
        FAMSIpcWaterReplaceData &operator=(FAMSIpcWaterReplaceData &&) = default;

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

        std::size_t fillWaterLevel(std::size_t baseOffset,const std::vector<double> &water_level);
        const double *getWaterLevel(std::size_t &size) const;
        std::size_t fillWaterTemp(std::size_t baseOffset,const std::vector<double> &water_temp);
        const double *getWaterTemp(std::size_t &size) const;
        std::size_t fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime);
        const char *getAccumulatedTime(std::size_t &size) const;
        // DATA definition
        // with raw fields (int, long): declare normally
        // with string: declare in char array with fixed length
        // with list, binary data: declare in from [offset and size]
        //! The base data
        FAMSIpcHeader ipcHeader;

        std::size_t mWaterLevelListOffset;
        std::size_t mWaterLevelListSize;
        std::size_t mAccumulatedTimeListOffset;
        std::size_t mAccumulatedTimeListSize;
        std::size_t mWaterTempListOffset;
        std::size_t mWaterTempListSize;
    private:

    };
}
#endif /* FAMS_IPC_WATER_REPLACE_INFO_DATA_H_ */

/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcNitrificationData.cpp
 * @brief       Source file of FAMSIpcNitrificationData class
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include "FAMSIpcNitrificationData.h"

namespace famscommon {

    /*!
     * @brief Constructor.
     * @param [in] eventType event type to send to queue
     * @param [in] request Request data
     * @return None.
     */
    FAMSIpcNitrificationData::FAMSIpcNitrificationData(famscommon::MessageType eventType,
            const famscommon::FAMSNitrificationData &request) :
        ipcHeader { eventType},
        mWaterTempListOffset {0},
        mWaterTempListSize {0}
    {
        mdb_apl_log_start();
        auto baseOffSet = sizeof(FAMSIpcNitrificationData);
        auto extendSize = std::size_t {0};
        extendSize += fillWaterTemp(baseOffSet + extendSize, request.getWaterTemp());
        extendSize += fillAccumulatedTime(baseOffSet + extendSize, request.getAccumulatedTime());

        mdb_apl_log_trace("Total extended size %lu bytes. ", extendSize);
        mdb_apl_log_end();
    }

    /*!
     * @brief Override default operator new.
     * This operator can allocated more memory than actual class size
     * Actual memory size allocated = size + extendSize
     *
     * @param [in] size Size of class (bytes)
     * @param [in] extendSize Extended size (bytes)
     * @return Pointer to new memory location.
     */
    void *FAMSIpcNitrificationData::operator new (std::size_t size,
            std::size_t extendSize)
    {
        auto result = new char[size + extendSize]; /* pgr0745 *//* pgr1163 */
        memset(result, 0, sizeof(char) * (size + extendSize)); /* pgr0579 */
        mdb_apl_log_trace("Total allocation size %lu (extended %lu)",
                          size + extendSize, extendSize);
        mdb_apl_log_trace("[mem] allocate %lu bytes at %p", size + extendSize,
                          result);
        return result;
    }

    /*!
     * @brief Override default operator delete.
     * Override to keep the type consistency of delete behavior.
     *
     * @param [in] pointer Memory location to be deleted
     * @return None.
     */
    void FAMSIpcNitrificationData::operator delete (void *pointer)
    {
        mdb_apl_log_trace("[mem] free memory at %p", pointer);
        delete[] static_cast<char *>(pointer); /* pgr2215 */
    }

    std::size_t FAMSIpcNitrificationData::fillWaterTemp(std::size_t baseOffset,const std::vector<double> &water_temp)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, water_temp.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        mWaterTempListOffset = baseOffset;
        mWaterTempListSize = water_temp.size();

        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mWaterTempListOffset);

        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mWaterTempListSize);

        for (auto i = 0u; i < mWaterTempListSize; i++)
        {
            mdb_apl_log_trace("water_temp %f", water_temp[i]);
            tempLinkIDList[i] = water_temp[i];
        }
        ret = mWaterTempListSize * sizeof(double);
        // mdb_apl_log_trace("Extend %lu bytes of water_temp", ret);
        mdb_apl_log_end();
        return ret;
    }
    const double *FAMSIpcNitrificationData::getWaterTemp(std::size_t &size) const {
        if (sizeof(FAMSIpcNitrificationData) > mWaterTempListOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mWaterTempListSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mWaterTempListOffset;
        return (reinterpret_cast<const double *>(address));
    }

    const char *FAMSIpcNitrificationData::getAccumulatedTime(std::size_t &size) const {
        if (sizeof(FAMSIpcNitrificationData) > mAccumulatedTimeListOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mAccumulatedTimeListSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mAccumulatedTimeListOffset;
        return (reinterpret_cast<const char *>(address));
    }
    std::size_t FAMSIpcNitrificationData::fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, accumulatedTime.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList
        mAccumulatedTimeListOffset = baseOffset;
        mAccumulatedTimeListSize = accumulatedTime.size();

        auto tempLinkIDList = reinterpret_cast<char *>(baseAddress + mAccumulatedTimeListOffset);

        mdb_apl_log_trace("Array address [accumulatedTime: %p], array size = %d", tempLinkIDList, mAccumulatedTimeListSize);

        for (auto i = 0u; i < mAccumulatedTimeListSize; i++)
        {
            std::strncpy(tempLinkIDList, accumulatedTime[i].c_str(), ACCUMULATED_TIME_LEN - 1);
            tempLinkIDList[ACCUMULATED_TIME_LEN - 1] = 0;
            tempLinkIDList += ACCUMULATED_TIME_LEN;
        }
        ret += mAccumulatedTimeListSize * ACCUMULATED_TIME_LEN;
        // mdb_apl_log_trace("Extend %lu bytes of accumulatedTime", ret);
        mdb_apl_log_end();
        return ret;
    }
}


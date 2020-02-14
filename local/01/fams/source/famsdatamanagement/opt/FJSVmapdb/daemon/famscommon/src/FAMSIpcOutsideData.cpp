/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcOutsideData.cpp
 * @brief       Source file of FAMSIpcOutsideData class
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include "FAMSIpcOutsideData.h"

namespace famscommon {

    /*!
     * @brief Constructor.
     * @param [in] eventType event type to send to queue
     * @param [in] request Request data
     * @return None.
     */
    FAMSIpcOutsideData::FAMSIpcOutsideData(famscommon::MessageType eventType,
            const famscommon::FAMSOutsideData &request) :
        ipcHeader { eventType},
        mRoomTempListOffset {0},
        mRoomTempListSize {0},
        mHumidityListOffset {0},
        mHumidityListSize {0},
        mAtmosphericPressureListOffset {0},
        mAtmosphericPressureListSize {0}
    {
        mdb_apl_log_start();
        auto baseOffSet = sizeof(FAMSIpcOutsideData);
        auto extendSize = std::size_t {0};
        extendSize += fillRoomTemp(baseOffSet + extendSize, request.getRoomTemp());
        extendSize += fillHumidity(baseOffSet + extendSize, request.getHumidity());
        extendSize += fillAtmosphericPressure(baseOffSet + extendSize, request.getAtmosphericPressure());
        extendSize += fillAccumulatedTime(baseOffSet + extendSize, request.getAccumulatedTime());

//        mdb_apl_log_trace("Total extended size %lu bytes. ", extendSize);
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
    void *FAMSIpcOutsideData::operator new (std::size_t size,
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
    void FAMSIpcOutsideData::operator delete (void *pointer)
    {
        mdb_apl_log_trace("[mem] free memory at %p", pointer);
        delete[] static_cast<char *>(pointer); /* pgr2215 */
    }

    std::size_t FAMSIpcOutsideData::fillRoomTemp(std::size_t baseOffset,const std::vector<double> &room_temp)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, room_temp.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mRoomTempListOffset = baseOffset;
        mRoomTempListSize = room_temp.size();

        mRoomTempListOffset = baseOffset;
        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mRoomTempListOffset);
        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mRoomTempListSize);

        for (auto i = 0u; i < mRoomTempListSize; i++)
        {
            mdb_apl_log_trace("room_temp %f", room_temp[i]);
            tempLinkIDList[i] = room_temp[i];
        }
        ret = mRoomTempListSize * sizeof(double);
        mdb_apl_log_end();
        return ret;
    }
    std::size_t FAMSIpcOutsideData::fillHumidity(std::size_t baseOffset,const std::vector<double> &water_temp)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, water_temp.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        mHumidityListOffset = baseOffset;
        mHumidityListSize = water_temp.size();

        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mHumidityListOffset);

        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mHumidityListSize);

        for (auto i = 0u; i < mHumidityListSize; i++)
        {
            mdb_apl_log_trace("water_temp %f", water_temp[i]);
            tempLinkIDList[i] = water_temp[i];
        }
        ret = mHumidityListSize * sizeof(double);
        mdb_apl_log_end();
        return ret;
    }
    std::size_t FAMSIpcOutsideData::fillAtmosphericPressure(std::size_t baseOffset,const std::vector<double> &atmospheric_pressure)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, atmospheric_pressure.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        mAtmosphericPressureListOffset = baseOffset;
        mAtmosphericPressureListSize = atmospheric_pressure.size();

        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mAtmosphericPressureListOffset);

        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mAtmosphericPressureListSize);

        for (auto i = 0u; i < mAtmosphericPressureListSize; i++)
        {
            mdb_apl_log_trace("atmospheric_pressure %f", atmospheric_pressure[i]);
            tempLinkIDList[i] = atmospheric_pressure[i];
        }
        ret = mAtmosphericPressureListSize * sizeof(double);
        mdb_apl_log_end();
        return ret;
    }
    const double *FAMSIpcOutsideData::getRoomTemp(std::size_t &size) const {
        if (sizeof(FAMSIpcOutsideData) > mRoomTempListOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mRoomTempListSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mRoomTempListOffset;
        return (reinterpret_cast<const double *>(address));
    }
    const double *FAMSIpcOutsideData::getHumidity(std::size_t &size) const {
        if (sizeof(FAMSIpcOutsideData) > mHumidityListOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mHumidityListSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mHumidityListOffset;
        return (reinterpret_cast<const double *>(address));
    }
    const double *FAMSIpcOutsideData::getAtmosphericPressure(std::size_t &size) const {
        if (sizeof(FAMSIpcOutsideData) > mAtmosphericPressureListOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mAtmosphericPressureListSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mAtmosphericPressureListOffset;
        return (reinterpret_cast<const double *>(address));
    }

    const char *FAMSIpcOutsideData::getAccumulatedTime(std::size_t &size) const {
        if (sizeof(FAMSIpcOutsideData) > mAccumulatedTimeListOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mAccumulatedTimeListSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mAccumulatedTimeListOffset;
        return (reinterpret_cast<const char *>(address));
    }
    std::size_t FAMSIpcOutsideData::fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime)
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
        mdb_apl_log_end();
        return ret;
    }
}


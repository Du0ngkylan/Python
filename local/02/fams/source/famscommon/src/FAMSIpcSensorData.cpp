/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcSensorData.cpp
 * @brief       Source file of FAMSIpcSensorData class
 * @author      thanglq
 * @date        2019/10/14
 */

#include "FAMSIpcSensorData.h"

namespace famscommon {

    /*!
     * @brief Constructor.
     * @param [in] eventType event type to send to queue
     * @param [in] request Request data
     * @return None.
     */
FAMSIpcSensorData::FAMSIpcSensorData(famscommon::MessageType eventType,
            const famscommon::FAMSSensorData &request) :
        ipcHeader { eventType},
        mAccumulatedTimeOffset {0},
        mAccumulatedTimeSize {0},
        mCisternCodeOffset {0},
        mCisternCodeSize {0},
        mTypeOffset {0},
        mTypeSize {0},
        mValueOffset {0},
        mValueSize {0},
        mUnitOffset {0},
        mUnitSize {0}
    {
        mdb_apl_log_start();
        auto baseOffSet = sizeof(FAMSIpcSensorData);
        auto extendSize = std::size_t {0};
        extendSize += fillAccumulatedTime(baseOffSet + extendSize, request.getAccumulatedTime());
        extendSize += fillCisternCode(baseOffSet + extendSize, request.getCisternCode());
        extendSize += fillType(baseOffSet + extendSize, request.getType());
        extendSize += fillValue(baseOffSet + extendSize, request.getValue());
        extendSize += fillUnit(baseOffSet + extendSize, request.getUnit());

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
    void *FAMSIpcSensorData::operator new (std::size_t size,
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
    void FAMSIpcSensorData::operator delete (void *pointer)
    {
        mdb_apl_log_trace("[mem] free memory at %p", pointer);
        delete[] static_cast<char *>(pointer); /* pgr2215 */
    }

    const char *FAMSIpcSensorData::getAccumulatedTime(std::size_t &size) const 
    {
        if (sizeof(FAMSIpcSensorData) > mAccumulatedTimeOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mAccumulatedTimeSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mAccumulatedTimeOffset;
        return (reinterpret_cast<const char *>(address));
    }

    std::size_t FAMSIpcSensorData::fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, accumulatedTime.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList
        mAccumulatedTimeOffset = baseOffset;
        mAccumulatedTimeSize = accumulatedTime.size();

        auto tempLinkIDList = reinterpret_cast<char *>(baseAddress + mAccumulatedTimeOffset);

        mdb_apl_log_trace("Array address [accumulatedTime: %p], array size = %d", tempLinkIDList, mAccumulatedTimeSize);

        for (auto i = 0u; i < mAccumulatedTimeSize; i++)
        {
            std::strncpy(tempLinkIDList, accumulatedTime[i].c_str(), ACCUMULATED_TIME_LEN - 1);
            tempLinkIDList[ACCUMULATED_TIME_LEN - 1] = 0;
            tempLinkIDList += ACCUMULATED_TIME_LEN;
        }
        ret += mAccumulatedTimeSize * ACCUMULATED_TIME_LEN;
        // mdb_apl_log_trace("Extend %lu bytes of accumulatedTime", ret);
        mdb_apl_log_end();
        return ret;
    }

    std::size_t FAMSIpcSensorData::fillCisternCode(std::size_t baseOffset,const std::vector<std::string> &cisternCode)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, cisternCode.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList
        mCisternCodeOffset = baseOffset;
        mCisternCodeSize = cisternCode.size();

        auto tempLinkIDList = reinterpret_cast<char *>(baseAddress + mCisternCodeOffset);

        mdb_apl_log_trace("Array address [cisternCode: %p], array size = %d", tempLinkIDList, mCisternCodeSize);

        for (auto i = 0u; i < mCisternCodeSize; i++)
        {
            std::strncpy(tempLinkIDList, cisternCode[i].c_str(), CISTERN_CODE_LEN - 1);
            tempLinkIDList[CISTERN_CODE_LEN - 1] = 0;
            tempLinkIDList += CISTERN_CODE_LEN;
        }
        ret += mCisternCodeSize * CISTERN_CODE_LEN;
        // mdb_apl_log_trace("Extend %lu bytes of cisternCode", ret);
        mdb_apl_log_end();
        return ret;
    }

    const char *FAMSIpcSensorData::getCisternCode(std::size_t &size) const
    {
        if (sizeof(FAMSIpcSensorData) > mCisternCodeOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mCisternCodeSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mCisternCodeOffset;
        return (reinterpret_cast<const char *>(address));
    }

    std::size_t FAMSIpcSensorData::fillType(std::size_t baseOffset,const std::vector<int> &type)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, type.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mWaterLevelListOffset = baseOffset;
        mTypeSize = type.size();
        mTypeOffset = baseOffset;

        auto tempLinkIDList = reinterpret_cast<int *>(baseAddress + mTypeOffset);
        mdb_apl_log_trace("Array address [type: %p], array size = %d", tempLinkIDList, mTypeSize);

        for (auto i = 0u; i < mTypeSize; i++)
        {
            mdb_apl_log_trace("LowerCentralIll %d", type[i]);
            tempLinkIDList[i] = type[i];
        }
        ret = mTypeSize * sizeof(int);
        mdb_apl_log_trace("Extend %lu bytes of LowerCentralIll", ret);
        mdb_apl_log_end();
        return ret;
    }

    const int *FAMSIpcSensorData::getType(std::size_t &size) const
    {
        if (sizeof(FAMSIpcSensorData) > mTypeOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mTypeSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mTypeOffset;
        return (reinterpret_cast<const int *>(address));
    }

    std::size_t FAMSIpcSensorData::fillValue(std::size_t baseOffset,const std::vector<double> &value)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, value.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mWaterLevelListOffset = baseOffset;
        mValueSize = value.size();
        mValueOffset = baseOffset;

        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mValueOffset);
        mdb_apl_log_trace("Array address [value: %p], array size = %d", tempLinkIDList, mValueSize);

        for (auto i = 0u; i < mValueSize; i++)
        {
            mdb_apl_log_trace("salt %f", value[i]);
            tempLinkIDList[i] = value[i];
        }
        ret = mValueSize * sizeof(double);
        // mdb_apl_log_trace("Extend %lu bytes of salt", ret);
        mdb_apl_log_end();
        return ret;
    }

    const double *FAMSIpcSensorData::getValue(std::size_t &size) const
    {
        if (sizeof(FAMSIpcSensorData) > mValueOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mValueSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mValueOffset;
        return (reinterpret_cast<const double *>(address));
    }

    const char *FAMSIpcSensorData::getUnit(std::size_t &size) const 
    {
        if (sizeof(FAMSIpcSensorData) > mUnitOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mUnitSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mUnitOffset;
        return (reinterpret_cast<const char *>(address));
    }

    std::size_t FAMSIpcSensorData::fillUnit(std::size_t baseOffset,const std::vector<std::string> &unit)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, unit.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList
        mUnitOffset = baseOffset;
        mUnitSize = unit.size();

        auto tempLinkIDList = reinterpret_cast<char *>(baseAddress + mUnitOffset);

        mdb_apl_log_trace("Array address [unit: %p], array size = %d", tempLinkIDList, mUnitSize);

        for (auto i = 0u; i < mUnitSize; i++)
        {
            std::strncpy(tempLinkIDList, unit[i].c_str(), ACCUMULATED_TIME_LEN - 1);
            tempLinkIDList[ACCUMULATED_TIME_LEN - 1] = 0;
            tempLinkIDList += ACCUMULATED_TIME_LEN;
        }
        ret += mUnitSize * ACCUMULATED_TIME_LEN;
        mdb_apl_log_end();
        return ret;
    }
}


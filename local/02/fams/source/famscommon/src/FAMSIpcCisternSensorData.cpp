/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcCisternSensorData.cpp
 * @brief       Source file of FAMSIpcCisternSensorData class
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include "FAMSIpcCisternSensorData.h"

namespace famscommon {

    /*!
     * @brief Constructor.
     * @param [in] eventType event type to send to queue
     * @param [in] request Request data
     * @return None.
     */
FAMSIpcCisternSensorData::FAMSIpcCisternSensorData(famscommon::MessageType eventType,
            const famscommon::FAMSCisternSenSorData &request) :
        ipcHeader { eventType},
        mAccumulatedTimeListOffset {0},
        mAccumulatedTimeListSize {0},
        mCisternCodeOffset {0},
        mCisternCodeSize {0},
        mInflowTempOffset {0},
        mInflowTempSize {0},
        mOutflowTempOffset {0},
        mOutflowTempSize {0},
        mSaltOffset {0},
        mSaltSize {0},
        mPHOffset {0},
        mPHSize{0},
        mLowerCentralIllOffset {0},
        mLowerCentralIllSize {0},
        mUpperCentralIllOffset {0},
        mUpperCentralIllSize {0}
    {
        mdb_apl_log_start();
        auto baseOffSet = sizeof(FAMSIpcCisternSensorData);
        auto extendSize = std::size_t {0};
        extendSize += fillAccumulatedTime(baseOffSet + extendSize, request.getAccumulatedTime());
        extendSize += fillCisternCode(baseOffSet + extendSize, request.getCisternCode());
        extendSize += fillInflowTemp(baseOffSet + extendSize, request.getInFlowTemp());
        extendSize += fillOutflowTemp(baseOffSet + extendSize, request.getOutFlowTemp());
        extendSize += fillUpperCentralIll(baseOffSet + extendSize, request.getUpperCentralIll());
        extendSize += fillLowerCentralIll(baseOffSet + extendSize, request.getLowerCentralIll());
        extendSize += fillSalt(baseOffSet + extendSize, request.getSalt());
        extendSize += fillPH(baseOffSet + extendSize, request.getPH());

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
    void *FAMSIpcCisternSensorData::operator new (std::size_t size,
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
    void FAMSIpcCisternSensorData::operator delete (void *pointer)
    {
        mdb_apl_log_trace("[mem] free memory at %p", pointer);
        delete[] static_cast<char *>(pointer); /* pgr2215 */
    }



    const char *FAMSIpcCisternSensorData::getAccumulatedTime(std::size_t &size) const {
        if (sizeof(FAMSIpcCisternSensorData) > mAccumulatedTimeListOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mAccumulatedTimeListSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mAccumulatedTimeListOffset;
        return (reinterpret_cast<const char *>(address));
    }
    std::size_t FAMSIpcCisternSensorData::fillAccumulatedTime(std::size_t baseOffset,const std::vector<std::string> &accumulatedTime)
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
    std::size_t FAMSIpcCisternSensorData::fillCisternCode(std::size_t baseOffset,const std::vector<std::string> &cisternCode)
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
    const char *FAMSIpcCisternSensorData::getCisternCode(std::size_t &size) const
    {
        if (sizeof(FAMSIpcCisternSensorData) > mCisternCodeOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mCisternCodeSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mCisternCodeOffset;
        return (reinterpret_cast<const char *>(address));
    }
    std::size_t FAMSIpcCisternSensorData::fillSalt(std::size_t baseOffset,const std::vector<double> &salt)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, salt.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mWaterLevelListOffset = baseOffset;
        mSaltSize = salt.size();

        mSaltOffset = baseOffset;
        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mSaltOffset);
        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mSaltSize);

        for (auto i = 0u; i < mSaltSize; i++)
        {
            mdb_apl_log_trace("salt %f", salt[i]);
            tempLinkIDList[i] = salt[i];
        }
        ret = mSaltSize * sizeof(double);
        // mdb_apl_log_trace("Extend %lu bytes of salt", ret);
        mdb_apl_log_end();
        return ret;
    }
    const double *FAMSIpcCisternSensorData::getSalt(std::size_t &size) const
    {
        if (sizeof(FAMSIpcCisternSensorData) > mSaltOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mSaltSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mSaltOffset;
        return (reinterpret_cast<const double *>(address));
    }
    std::size_t FAMSIpcCisternSensorData::fillPH(std::size_t baseOffset,const std::vector<double> &pH)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, pH.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        mPHOffset = baseOffset;
        mPHSize = pH.size();

        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mPHOffset);

        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mPHSize);

        for (auto i = 0u; i < mPHSize; i++)
        {
            mdb_apl_log_trace("pH %f", pH[i]);
            tempLinkIDList[i] = pH[i];
        }
        ret = mPHSize * sizeof(double);
        // mdb_apl_log_trace("Extend %lu bytes of pH", ret);
        mdb_apl_log_end();
        return ret;
    }
    const double *FAMSIpcCisternSensorData::getPH(std::size_t &size) const
    {
        if (sizeof(FAMSIpcCisternSensorData) > mPHOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mPHSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mPHOffset;
        return (reinterpret_cast<const double *>(address));
    }
    std::size_t FAMSIpcCisternSensorData::fillInflowTemp(std::size_t baseOffset,const std::vector<double> &inflowTemp)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, inflowTemp.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mWaterLevelListOffset = baseOffset;
        mInflowTempSize = inflowTemp.size();

        mInflowTempOffset = baseOffset;
        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mInflowTempOffset);
        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mInflowTempSize);

        for (auto i = 0u; i < mInflowTempSize; i++)
        {
            mdb_apl_log_trace("inflowTemp %f", inflowTemp[i]);
            tempLinkIDList[i] = inflowTemp[i];
        }
        ret = mInflowTempSize * sizeof(double);
        mdb_apl_log_trace("Extend %lu bytes of inflowTemp", ret);
        mdb_apl_log_end();
        return ret;
    }
    const double *FAMSIpcCisternSensorData::getInflowTemp(std::size_t &size) const
    {
        if (sizeof(FAMSIpcCisternSensorData) > mInflowTempOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mInflowTempSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mInflowTempOffset;
        return (reinterpret_cast<const double *>(address));
    }
    std::size_t FAMSIpcCisternSensorData::fillOutflowTemp(std::size_t baseOffset,const std::vector<double> &outflowTemp)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, outflowTemp.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mWaterLevelListOffset = baseOffset;
        mOutflowTempSize = outflowTemp.size();

        mOutflowTempOffset = baseOffset;
        auto tempLinkIDList = reinterpret_cast<double *>(baseAddress + mOutflowTempOffset);
        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mOutflowTempSize);

        for (auto i = 0u; i < mOutflowTempSize; i++)
        {
            mdb_apl_log_trace("outflowTemp %f", outflowTemp[i]);
            tempLinkIDList[i] = outflowTemp[i];
        }
        ret = mOutflowTempSize * sizeof(double);
        mdb_apl_log_trace("Extend %lu bytes of outflowTemp", ret);
        mdb_apl_log_end();
        return ret;
    }
    const double *FAMSIpcCisternSensorData::getOutflowTemp(std::size_t &size) const
    {
        if (sizeof(FAMSIpcCisternSensorData) > mOutflowTempOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mOutflowTempSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mOutflowTempOffset;
        return (reinterpret_cast<const double *>(address));
    }

    std::size_t FAMSIpcCisternSensorData::fillUpperCentralIll(std::size_t baseOffset,const std::vector<int> &upperCentralIll)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, upperCentralIll.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mWaterLevelListOffset = baseOffset;
        mUpperCentralIllSize = upperCentralIll.size();

        mUpperCentralIllOffset = baseOffset;
        auto tempLinkIDList = reinterpret_cast<int *>(baseAddress + mUpperCentralIllOffset);
        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mUpperCentralIllSize);

        for (auto i = 0u; i < mUpperCentralIllSize; i++)
        {
            mdb_apl_log_trace("upperCentralIll %d", upperCentralIll[i]);
            tempLinkIDList[i] = upperCentralIll[i];
        }
        ret = mUpperCentralIllSize * sizeof(int);
        mdb_apl_log_trace("Extend %lu bytes of upperCentralIll", ret);
        mdb_apl_log_end();
        return ret;
    }
    const int *FAMSIpcCisternSensorData::getUpperCentralIll(std::size_t &size) const
    {
        if (sizeof(FAMSIpcCisternSensorData) > mUpperCentralIllOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mUpperCentralIllSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mUpperCentralIllOffset;
        return (reinterpret_cast<const int *>(address));
    }
    std::size_t FAMSIpcCisternSensorData::fillLowerCentralIll(std::size_t baseOffset,const std::vector<int> &LowerCentralIll)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, LowerCentralIll.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        // std::array mLinkIDList        mWaterLevelListOffset = baseOffset;
        mLowerCentralIllSize = LowerCentralIll.size();

        mLowerCentralIllOffset = baseOffset;
        auto tempLinkIDList = reinterpret_cast<int *>(baseAddress + mLowerCentralIllOffset);
        mdb_apl_log_trace("Array address [searchLinkIdList: %p], array size = %d", tempLinkIDList, mLowerCentralIllSize);

        for (auto i = 0u; i < mLowerCentralIllSize; i++)
        {
            mdb_apl_log_trace("LowerCentralIll %d", LowerCentralIll[i]);
            tempLinkIDList[i] = LowerCentralIll[i];
        }
        ret = mLowerCentralIllSize * sizeof(int);
        mdb_apl_log_trace("Extend %lu bytes of LowerCentralIll", ret);
        mdb_apl_log_end();
        return ret;
    }
    const int *FAMSIpcCisternSensorData::getLowerCentralIll(std::size_t &size) const
    {
        if (sizeof(FAMSIpcCisternSensorData) > mLowerCentralIllOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mLowerCentralIllSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mLowerCentralIllOffset;
        return (reinterpret_cast<const int *>(address));
    }
}


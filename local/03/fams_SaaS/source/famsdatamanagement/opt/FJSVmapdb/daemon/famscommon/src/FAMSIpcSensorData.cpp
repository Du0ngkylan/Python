/* Copyright© 2020 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSCisternSensorData.cpp
 * @brief       Source file of FAMSCisternSensorData class
 * @author      ThangLQ
 * @date        2020/01/20
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
        mTimeStampOffset {0},
        mTimeStampSize {0},
        mRtuIdOffset {0},
        mRtuIdSize {0},
        mRtuChannelOffset{ 0 },
        mRtuChannelSize{ {0, 0} },
		mValueOffset { 0 },
		mValueSize { {0, 0} }
    {
        mdb_apl_log_start();
        auto baseOffSet = sizeof(FAMSIpcSensorData);
        auto extendSize = std::size_t {0};
        extendSize += fillTimeStamps(baseOffSet + extendSize, request.getTimeStamp());
        extendSize += fillRtuIds(baseOffSet + extendSize, request.getRtuId());
        extendSize += fillRtuChannels(baseOffSet + extendSize, request.getRtuChannel());
        extendSize += fillValues(baseOffSet + extendSize, request.getValue());
       
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

    const char *FAMSIpcSensorData::getTimeStamps(std::size_t &size) const {

        if (sizeof(FAMSIpcSensorData) > mTimeStampOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mTimeStampSize;
        auto address = reinterpret_cast<const char *>(this);
        address += mTimeStampOffset;
        return (reinterpret_cast<const char *>(address));
    }
    std::size_t FAMSIpcSensorData::fillTimeStamps(std::size_t baseOffset,const std::vector<std::string> &timeStamps)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, timeStamps.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        mTimeStampOffset = baseOffset;
        mTimeStampSize = timeStamps.size();

        auto tempLinkTimeStampList = reinterpret_cast<char *>(baseAddress + mTimeStampOffset);

        mdb_apl_log_trace("Array address [TimeStamp: %p], array size = %d", tempLinkTimeStampList, mTimeStampSize);

        for (auto i = 0u; i < mTimeStampSize; i++)
        {
            std::strncpy(tempLinkTimeStampList, timeStamps[i].c_str(), TIME_STAMP_LEN - 1);
            tempLinkTimeStampList[TIME_STAMP_LEN - 1] = 0;
            tempLinkTimeStampList += TIME_STAMP_LEN;
        }
        ret += mTimeStampSize * TIME_STAMP_LEN;
        mdb_apl_log_end();
        return ret;
    }
    std::size_t FAMSIpcSensorData::fillRtuIds(std::size_t baseOffset, const std::vector<int> &rtuIds)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, rtuIds.size());
        auto ret = std::size_t {0};
        auto baseAddress = reinterpret_cast<char *>(this);
        mRtuIdOffset = baseOffset;
        mRtuIdSize = rtuIds.size();

        auto tempLinkRtuIdList = reinterpret_cast<int *>(baseAddress + mRtuIdOffset);

        mdb_apl_log_trace("Array address [RtuId: %p], array size = %d", tempLinkRtuIdList, mRtuIdSize);

        for (auto i = 0u; i < mRtuIdSize; i++)
        {
        	mdb_apl_log_trace("rtuId %d", rtuIds[i]);
        	tempLinkRtuIdList[i] = rtuIds[i];
        }
        ret += mRtuIdSize * sizeof(int);
        mdb_apl_log_end();
        return ret;
    }
    const int *FAMSIpcSensorData::getRtuIds(std::size_t &size) const
    {
        if (sizeof(FAMSIpcSensorData) > mRtuIdOffset) {
            mdb_apl_log_end();
            return nullptr;
        }
        size = mRtuIdSize;
        auto address = reinterpret_cast<const char*>(this);
        address += mRtuIdOffset;
        return (reinterpret_cast<const int *>(address));
    }

    std::size_t FAMSIpcSensorData::fillRtuChannels(std::size_t baseOffset, const std::vector<std::vector<int>> &rtuChannels)
    {
        mdb_apl_log_start();
        mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, rtuChannels.size());
        auto ret = std::size_t{ 0 };
        auto totalSize = std::size_t { 0 };
        auto baseAddress = reinterpret_cast<char*>(this);
        // std::array mLinkIDList
        mRtuIdOffset = baseOffset;
        std::size_t rtuChannelsSize = rtuChannels.size();


        auto temprtuChannelList = reinterpret_cast<std::vector<std::vector<int>>*>(baseAddress + mRtuChannelOffset);

        mdb_apl_log_trace("Array address [rtuChannels: %p], array size = %d", temprtuChannelList, rtuChannelsSize);

        for (auto i = 0u; i < rtuChannelsSize; i++)
        {
        	//mdb_apl_log_trace("rtuChannel %d", rtuChannels[i]);
        	temprtuChannelList[i].push_back(rtuChannels[i]);
        	mRtuChannelSize[i] = rtuChannels[i].size();
        	totalSize += rtuChannels[i].size();
        }

        ret += ((rtuChannelsSize + totalSize) * sizeof(int));
        mdb_apl_log_end();
        return ret;
    }
    const std::vector<std::vector<int>> *FAMSIpcSensorData::getRtuChannels(std::map<std::size_t, std::size_t> &size) const
    {
        if (sizeof(FAMSIpcSensorData) > mRtuChannelOffset) {
            mdb_apl_log_end();



            return nullptr;
        }
        size = mRtuChannelSize;
        auto address = reinterpret_cast<const char*>(this);
        address += mRtuChannelOffset;
        return (reinterpret_cast<const std::vector<std::vector<int>>*>(address));
    }
	std::size_t FAMSIpcSensorData::fillValues(std::size_t baseOffset, const std::vector<std::vector<double>>& values)
	{
		mdb_apl_log_start();
		mdb_apl_log_trace("Base offset: %lu, list size %lu", baseOffset, values.size());
		auto ret = std::size_t{ 0 };
		auto totalSize = std::size_t { 0 };
		auto baseAddress = reinterpret_cast<char*>(this);
		// std::array mLinkIDList
		mValueOffset = baseOffset;
		std::size_t valuesSize = values.size();

		auto tempValueList = reinterpret_cast<std::vector<std::vector<double>>*>(baseAddress + mValueOffset);

		mdb_apl_log_trace("Array address [values: %p], array size = %d", tempValueList, valuesSize);

		for (auto i = 0u; i < valuesSize; i++)
		{
			//mdb_apl_log_trace("rtuChannel %d", values[i]);
			tempValueList[i].push_back(values[i]);
			mValueSize[i] = values[i].size();
			totalSize += values[i].size();
		}

		ret += ((valuesSize + totalSize) * sizeof(double));
		mdb_apl_log_end();
		return ret;
	}
	const std::vector<std::vector<double>> *FAMSIpcSensorData::getValues(std::map<std::size_t, std::size_t>& size) const
	{
		if (sizeof(FAMSIpcSensorData) > mValueOffset) {
			mdb_apl_log_end();
			return nullptr;
		}
		size = mValueSize;
		auto address = reinterpret_cast<const char *>(this);
		address += mValueOffset;

		return (reinterpret_cast<const std::vector<std::vector<double>>*>(address));
	}

}


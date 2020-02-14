/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSSensorData.h
 * @brief       Header file of class FAMSSensorData.
 * @author      thanglq
 * @date        2019/11/14
 */

#ifndef FAMS_SENSOR_DATA_H_
#define FAMS_SENSOR_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famscommon {
    /*!
     * @class FAMSSensorData
     * @brief Store different map data request
     */
    class FAMSSensorData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSSensorData(void) :
            mTimeStamp{},
            mRtuId{},
            mRtuChannel{},
            mValue{}
        {
        }

        /*!
         * @brief Get accumulated time
         *
         * @param None
         * @return  mTimeStamp
         */
        inline const std::vector<std::string> &getTimeStamp() const {
            return mTimeStamp;
        }

        /*!
         * @brief Set mTimeStamp
         *
         * @param [in] accumulatedTime accumulated time
         * @return None
         */
        inline void setTimeStamp(std::string _item) { /*pgr2227*/
            mTimeStamp.push_back(_item);
        }

        /*!
         * @brief Get RTU ID
         *
         * @param None
         * @return  mRtuId
         */
        inline const std::vector<int> &getRtuId() const {
            return mRtuId;
        }

        /*!
         * @brief Set mRtuId
         *
         * @param [in] rtuid
         * @return None
         */
        inline void setRtuId(const std::vector<int> &_rtuId) { /*pgr2227*/
            mRtuId = _rtuId;
        }

        /*!
         * @brief Get RTU channel
         *
         * @param None
         * @return  mRtuChannel
         */
        inline const std::vector<std::vector<int>> &getRtuChannel() const {
            return mRtuChannel;
        }

        /*!
         * @brief Set RTU channel
         *
         * @param [in] rtu channel
         * @return None
         */
        inline void setRtuChannel(const std::vector<std::vector<int>> &_rtuChannel) { /*pgr2227*/
        	mRtuChannel = _rtuChannel;
        }

        /*!
         * @brief Get value
         *
         * @param None
         * @return  mValue
         */
        inline const std::vector<std::vector<double>> &getValue() const {
            return mValue;
        }

        /*!
         * @brief Set mValue
         *
         * @param [in] value
         * @return None
         */
        inline void setValue(const std::vector<std::vector<double>> &_value) { /*pgr2227*/
            mValue = _value;
        }

    private:
        std::vector<std::string> mTimeStamp;
        std::vector<int> mRtuId;
        std::vector<std::vector<int>> mRtuChannel;
        std::vector<std::vector<double>> mValue;
    };
}
#endif /* FAMS_SENSOR_DATA_H_ */

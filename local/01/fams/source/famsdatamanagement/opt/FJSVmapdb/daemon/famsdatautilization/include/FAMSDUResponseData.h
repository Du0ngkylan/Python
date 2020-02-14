/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSApResponseData.h
 * @brief       response data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_RESPONSE_DATA_H_
#define FAMS_DATA_UTIL_RESPONSE_DATA_H_

#include <vector>
#include <string>
#include <unordered_set>
#include "FAMSDUCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSApResponseData
     * @brief Store different map data response
     */
    class FAMSApResponseData {
    public:

        /*!
         * @brief Default constructor
         * @param None.
         * @return None.
         */
        FAMSApResponseData(void) : mSensorId {} {
        }

        /*!
         * @brief getter for mSensorId
         * @param None.
         * @return mSensorId.
         */
        inline long long getSensorId(void) const {
            return mSensorId;
        }

        /*!
         * @brief setter for sensorId
         * @param [in] sensorId id of sensor
         * @return None.
         */
        inline void setSensorId(double sensorId) {
            mSensorId = sensorId;
        }

    private:
        //! sensor id
        long long mSensorId;
     };
}
#endif /* FAMS_DATA_UTIL_RESPONSE_DATA_H_ */

/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSOutsideData.h
 * @brief       Header file of class FAMSOutsideData.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_OUTSIDE_INFORMATION_DATA_H_
#define FAMS_OUTSIDE_INFORMATION_DATA_H_
namespace famscommon {
    /*!
     * @class FAMSOutsideData
     * @brief Store different map data request
     */
    class FAMSOutsideData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSOutsideData(void):
            mAccumulatedTime {},
            room_temp {},
            humidity {},
            atmospheric_pressure {}
        {
        }

        /*!
         * @brief Get Sensor Id
         *
         * @param None
         * @return mSensorId
         */
        inline const std::vector<double> &getRoomTemp(void) const {
            return room_temp;
        }

        /*!
         * @brief set Sensor Id
         *
         * @param [in] Sensor Id
         * @return None
         */
        inline void setRoomTemp(const std::vector<double> &_room_temp) {
            room_temp = _room_temp;
        }
        /*!
         * @brief Get Sensor Id
         *
         * @param None
         * @return mSensorId
         */
        inline const std::vector<double> &getHumidity(void) const {
            return humidity;
        }

        /*!
         * @brief set Sensor Id
         *
         * @param [in] Sensor Id
         * @return None
         */
        inline void setAtmosphericPressure(const std::vector<double> &_atmospheric_pressure) {
            atmospheric_pressure = _atmospheric_pressure;
        }

        /*!
         * @brief Get Sensor Id
         *
         * @param None
         * @return mSensorId
         */
        inline const std::vector<double> &getAtmosphericPressure(void) const {
            return atmospheric_pressure;
        }

        /*!
         * @brief set Sensor Id
         *
         * @param [in] Sensor Id
         * @return None
         */
        inline void setHumidity(const std::vector<double> &_humidity) {
            humidity = _humidity;
        }
        /*!
         * @brief Get accumulated time
         *
         * @param None
         * @return  mAccumulatedTime
         */
        inline const std::vector<std::string> &getAccumulatedTime() const {
            return mAccumulatedTime;
        }

        /*!
         * @brief Set mAccumulatedTime
         *
         * @param [in] accumulatedTime accumulated time
         * @return None
         */
        inline void setAccumulatedTime(const std::vector<std::string> &accumulatedTime) { /*pgr2227*/
            mAccumulatedTime = accumulatedTime;
        }
    private:
        std::vector<double> room_temp;
        std::vector<double> humidity;
        std::vector<double> atmospheric_pressure;
        //! accumulated time
        std::vector<std::string> mAccumulatedTime;
    };
}
#endif /* FAMS_OUTSIDE_INFORMATION_DATA_H_ */

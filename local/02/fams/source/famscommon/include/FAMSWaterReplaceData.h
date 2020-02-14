/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSWaterReplaceData.h
 * @brief       Header file of class FAMSWaterReplaceData.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_WATER_REPLACE_INFORMATION_DATA_H_
#define FAMS_WATER_REPLACE_INFORMATION_DATA_H_
namespace famscommon {
    /*!
     * @class FAMSWaterReplaceData
     * @brief Store different map data request
     */
    class FAMSWaterReplaceData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSWaterReplaceData(void):
            mAccumulatedTime {},
            water_temp {},
            water_level {}
        {
        }

        /*!
         * @brief Get Sensor Id
         *
         * @param None
         * @return mSensorId
         */
        inline const std::vector<double> &getWaterLevel(void) const {
            return water_level;
        }

        /*!
         * @brief set Sensor Id
         *
         * @param [in] Sensor Id
         * @return None
         */
        inline void setWaterLevel(const std::vector<double> &waterLevel) {
            water_level = waterLevel;
        }
        /*!
         * @brief Get Sensor Id
         *
         * @param None
         * @return mSensorId
         */
        inline const std::vector<double> &getWaterTemp(void) const {
            return water_temp;
        }

        /*!
         * @brief set Sensor Id
         *
         * @param [in] Sensor Id
         * @return None
         */
        inline void setWaterTemp(const std::vector<double> &waterTemp) {
            water_temp = waterTemp;
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
        std::vector<double> water_level;
        std::vector<double> water_temp;
        //! accumulated time
        std::vector<std::string> mAccumulatedTime;
    };
}
#endif /* FAMS_WATER_REPLACE_INFORMATION_DATA_H_ */

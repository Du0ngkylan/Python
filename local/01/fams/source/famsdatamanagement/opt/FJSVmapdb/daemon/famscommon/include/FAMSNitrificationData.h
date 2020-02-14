/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSNitrificationData.h
 * @brief       Header file of class FAMSNitrificationData.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_NITRIFICATION_INFORMATION_DATA_H_
#define FAMS_NITRIFICATION_INFORMATION_DATA_H_
namespace famscommon {
    /*!
     * @class FAMSNitrificationData
     * @brief Store different map data request
     */
    class FAMSNitrificationData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSNitrificationData(void):
            mAccumulatedTime {},
            water_temp {}
        {
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
        std::vector<double> water_temp;
        //! accumulated time
        std::vector<std::string> mAccumulatedTime;
    };
}
#endif /* FAMS_NITRIFICATION_INFORMATION_DATA_H_ */

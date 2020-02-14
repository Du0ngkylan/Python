/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSCisternSenSorData.h
 * @brief       Header file of class FAMSCisternSenSorData.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_CISTERN_SENSOR_DATA_H_
#define FAMS_CISTERN_SENSOR_DATA_H_
namespace famscommon {
    /*!
     * @class FAMSCisternSenSorData
     * @brief Store different map data request
     */
    class FAMSCisternSenSorData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSCisternSenSorData(void):
            mAccumulatedTime {},
            mCisternCode {},
            mInFlowTemp {},
            mOutFlowTemp {},
            mUpperCentralIll {},
            mLowerCentralIll {},
            mSalt {},
            mPH {}
        {
        }

        /*!
         * @brief Get inflow temp
         *
         * @param None
         * @return mInFlowTemp
         */
        inline const std::vector<double> &getInFlowTemp(void) const {
            return mInFlowTemp;
        }

        /*!
         * @brief set inflow temp
         *
         * @param [in] inFlowTemp
         * @return None
         */
        inline void setInFlowTemp(const std::vector<double> &inFlowTemp) {
            mInFlowTemp = inFlowTemp;
        }

        /*!
         * @brief Get outflow temp
         *
         * @param None
         * @return mOutFlowTemp
         */
        inline const std::vector<double> &getOutFlowTemp(void) const {
            return mOutFlowTemp;
        }

        /*!
         * @brief set outFlow temp
         *
         * @param [in] outFlowTemp
         * @return None
         */
        inline void setOutFlowTemp(const std::vector<double> &outFlowTemp) {
            mOutFlowTemp = outFlowTemp;
        }

        /*!
         * @brief Get upper central ill
         *
         * @param None
         * @return mUpperCentralIll
         */
        inline const std::vector<int> &getUpperCentralIll(void) const {
            return mUpperCentralIll;
        }

        /*!
         * @brief set upper central ill
         *
         * @param [in] upperCentralIll
         * @return None
         */
        inline void setUpperCentralIll(const std::vector<int> &upperCentralIll) {
            mUpperCentralIll = upperCentralIll;
        }

        /*!
         * @brief Get lower central ill
         *
         * @param None
         * @return mLowerCentralIll
         */
        inline const std::vector<int> &getLowerCentralIll(void) const {
            return mLowerCentralIll;
        }

        /*!
         * @brief set lower central ill
         *
         * @param [in] lowerCentralIll
         * @return None
         */
        inline void setLowerCentralIll(const std::vector<int> &lowerCentralIll) {
            mLowerCentralIll = lowerCentralIll;
        }

        /*!
         * @brief Get salt
         *
         * @param None
         * @return mSalt
         */
        inline const std::vector<double> &getSalt(void) const {
            return mSalt;
        }

        /*!
         * @brief set salt
         *
         * @param [in] salt
         * @return None
         */
        inline void setSalt(const std::vector<double> &salt) {
            mSalt = salt;
        }

        /*!
         * @brief Get pH
         *
         * @param None
         * @return mPH
         */
        inline const std::vector<double> &getPH(void) const {
            return mPH;
        }

        /*!
         * @brief set pH
         *
         * @param [in] pH
         * @return None
         */
        inline void setPH(const std::vector<double> &pH) {
            mPH = pH;
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
         * @brief Set mCisternCode
         *
         * @param [in] cisternCode cistern code
         * @return None
         */
        inline void setCisternCode(const std::vector<std::string> &cisternCode) { /*pgr2227*/
            mCisternCode = cisternCode;
        }

        /*!
         * @brief Get cistern code
         *
         * @param None
         * @return  mCisternCode
         */
        inline const std::vector<std::string> &getCisternCode() const {
            return mCisternCode;
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
        //! accumulated time
        std::vector<std::string> mAccumulatedTime;
        std::vector<std::string> mCisternCode;
        std::vector<double> mInFlowTemp;
        std::vector<double> mOutFlowTemp;
        std::vector<int> mUpperCentralIll;
        std::vector<int> mLowerCentralIll;
        std::vector<double> mSalt;
        std::vector<double> mPH;
    };
}
#endif /* FAMS_NITRIFICATION_INFORMATION_DATA_H_ */

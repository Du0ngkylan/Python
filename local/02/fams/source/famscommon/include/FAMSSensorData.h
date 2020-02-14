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
        FAMSSensorData(void):
            mAccumulatedTime {},
            mCisternCode {},
            mType {},
            mValue {},
            mUnit {}
        {
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
        inline void setAccumulatedTime(std::string _item) { /*pgr2227*/
            mAccumulatedTime.push_back(_item);
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
         * @brief Set mCisternCode
         *
         * @param [in] cisternCode cistern code
         * @return None
         */
        inline void setCisternCode(std::string _item) { /*pgr2227*/
            mCisternCode.push_back(_item);
        }

        /*!
         * @brief Get port number
         *
         * @param None
         * @return  mType
         */
        inline const std::vector<int> &getType() const {
            return mType;
        }

        /*!
         * @brief Set mType
         *
         * @param [in] port number
         * @return None
         */
        inline void setType(const std::vector<int> &_type) { /*pgr2227*/
           mType = _type;
        }

        /*!
         * @brief Get value
         *
         * @param None
         * @return  mValue
         */
        inline const std::vector<double> &getValue() const {
            return mValue;
        }

        /*!
         * @brief Set mValue
         *
         * @param [in] value
         * @return None
         */
        inline void setValue(const std::vector<double> &_value) { /*pgr2227*/
           mValue = _value;
        }

        /*!
         * @brief Get unit
         *
         * @param None
         * @return  mUnit
         */
        inline const std::vector<std::string> &getUnit() const {
            return mUnit;
        }

        /*!
         * @brief Set mUnit
         *
         * @param [in] unit
         * @return None
         */
        inline void setUnit(const std::vector<std::string> &_unit) { /*pgr2227*/
           mUnit = _unit;
        }

    private:
        //! accumulated time
        std::vector<std::string> mAccumulatedTime;
        std::vector<std::string> mCisternCode;
        std::vector<int> mType;
        std::vector<double> mValue;
        std::vector<std::string> mUnit;
    };
}
#endif /* FAMS_SENSOR_DATA_H_ */

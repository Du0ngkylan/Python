/*
 * FAMSDUPostOrganismData.h
 *
 *  Created on: Jan 30, 2019
 *      Author: mapdb
 */

#ifndef FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUPOSTORGANISMDATA_H_
#define FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUPOSTORGANISMDATA_H_


namespace famsdatautil {
    /*!
     * @class FAMSDUPostOrganismData
     * @brief Store different map data request
     */
    class FAMSDUPostOrganismData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUPostOrganismData(void):
            mName {""},
            mType {0},
            mMax_value {},
            mMin_value {},
            mMin_unit {},
            mMax_unit {}
        {
        }

        /*!
         * @brief Get inflow temp
         *
         * @param None
         * @return mInFlowTemp
         */
        inline const std::vector<int> &getType(void) const {
            return mType;
        }

        /*!
         * @brief set inflow temp
         *
         * @param [in] inFlowTemp
         * @return None
         */
        inline void setType(const std::vector<int> &_type) {
            mType = _type;
        }

        /*!
         * @brief Get inflow temp
         *
         * @param None
         * @return mInFlowTemp
         */
        inline const std::string &getName(void) const {
            return mName;
        }

        /*!
         * @brief set inflow temp
         *
         * @param [in] inFlowTemp
         * @return None
         */
        inline void setName(const std::string _name) {
            mName = _name;
        }

        /*!
         * @brief Get inflow temp
         *
         * @param None
         * @return mInFlowTemp
         */
        inline const std::vector<std::string> &getMinValue(void) const {
            return mMin_value;
        }

        /*!
         * @brief set inflow temp
         *
         * @param [in] inFlowTemp
         * @return None
         */
        inline void setMinValue(const std::vector<std::string> &_minValue) {
            mMin_value = _minValue;
        }
        /*!
         * @brief Get inflow temp
         *
         * @param None
         * @return mInFlowTemp
         */
        inline const std::vector<std::string> &getMinUnit(void) const {
            return mMin_unit;
        }

        /*!
         * @brief set inflow temp
         *
         * @param [in] inFlowTemp
         * @return None
         */
        inline void setMinUnit(const std::vector<std::string> &_minUnit) {
            mMin_unit = _minUnit;
        }

        /*!
         * @brief Get inflow temp
         *
         * @param None
         * @return mInFlowTemp
         */
        inline const std::vector<std::string> &getMaxValue(void) const {
            return mMax_value;
        }

        /*!
         * @brief set inflow temp
         *
         * @param [in] inFlowTemp
         * @return None
         */
        inline void setMaxValue(const std::vector<std::string> &_maxValue) {
            mMax_value = _maxValue;
        }
        /*!
         * @brief Get inflow temp
         *
         * @param None
         * @return mInFlowTemp
         */
        inline const std::vector<std::string> &getMaxUnit(void) const {
            return mMax_unit;
        }

        /*!
         * @brief set inflow temp
         *
         * @param [in] inFlowTemp
         * @return None
         */
        inline void setMaxUnit(const std::vector<std::string> &_maxUnit) {
            mMax_unit = _maxUnit;
        }

    private:
        //! accumulated time
        std::string mName;
        std::vector<int> mType;
        std::vector<std::string> mMin_value;
        std::vector<std::string> mMax_value;
        std::vector<std::string> mMin_unit;
        std::vector<std::string> mMax_unit;
    };
}


#endif /* FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUPOSTORGANISMDATA_H_ */

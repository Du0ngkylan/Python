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
            mId {},
            mMax {},
            mMin {},
        {
        }

        /*!
         * @brief Get 
         *
         * @param None
         * @return 
         */
        inline const std::vector<std::string> &getId(void) const {
            return mId;
        }

        /*!
         * @brief set 
         *
         * @param [in] 
         * @return None
         */
        inline void setId(const std::vector<std::string> &_id) {
            mId = _id;
        }

        /*!
         * @brief Get 
         *
         * @param None
         * @return 
         */
        inline const std::string &getName(void) const {
            return mName;
        }

        /*!
         * @brief set
         *
         * @param [in] 
         * @return None
         */
        inline void setName(const std::string _name) {
            mName = _name;
        }

        /*!
         * @brief Get 
         *
         * @param None
         * @return 
         */
        inline const std::vector<std::string> &getMin(void) const {
            return mMin;
        }

        /*!
         * @brief set 
         *
         * @param [in] 
         * @return None
         */
        inline void setMin(const std::vector<std::string> &_min) {
            mMin = _min;
        }

        /*!
         * @brief Get
         *
         * @param None
         * @return
         */
        inline const std::vector<std::string> &getMax(void) const {
            return mMax;
        }

        /*!
         * @brief set 
         *
         * @param [in] 
         * @return None
         */
        inline void setMax(const std::vector<std::string> &_max) {
            mMax = _max;
        }

    private:
        std::string mName;
        std::vector<std::string> mId;
        std::vector<std::string> mMin;
        std::vector<std::string> mMax;
    };
}


#endif /* FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUPOSTORGANISMDATA_H_ */

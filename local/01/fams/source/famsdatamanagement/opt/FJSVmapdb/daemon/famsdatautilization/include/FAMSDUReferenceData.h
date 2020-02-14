/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUReferenceData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_REQUEST_REFERENCE_DATA_H_
#define FAMS_DATA_UTIL_REQUEST_REFERENCE_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUReferenceData
     * @brief The class support methods relating organism
     */
    class FAMSDUReferenceData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUReferenceData(void) :
            id {famscommon::JSON_DOUBLE_MISSING_VALUE} {
        }

        /*!
         * @brief getter for id
         * @param None.
         * @return id.
         */
        inline long long getId(void) const {
            return id;
        }

        /*!
         * @brief setter for id
         * @param [in] userId id.
         * @return None.
         */
        inline void setId(long long _id) {
            id = _id;
        }

        inline std::string getName(void) const {
            return name;
        }

        inline void setName(std::string _name) {
            name = _name;
        }

        inline void setValuesOneElement(std::string name, std::string value, std::string unit, std::string date) {
            std::map<std::string, std::string> hash;
            hash.insert(std::pair<std::string,std::string>("name", name));
            hash.insert(std::pair<std::string,std::string>("value", value == "0"? "": value));
            hash.insert(std::pair<std::string,std::string>("unit", unit));
            hash.insert(std::pair<std::string,std::string>("date", date));
            values.emplace_back(hash);
        }
        inline std::vector<std::map<std::string, std::string>> getValues(void) const {
            return values;
        }
    private:
        //! id
        long long id;
        std::string name;
        std::vector<std::map<std::string, std::string>> values;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_REFERENCE_DATA_H_ */

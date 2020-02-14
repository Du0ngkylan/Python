/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDULotData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_REQUEST_LOT_DATA_H_
#define FAMS_DATA_UTIL_REQUEST_LOT_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDULotData
     * @brief The class support methods relating organism
     */
    class FAMSDULotData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDULotData(void) :
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

        inline std::string getDescription(void) const {
            return description;
        }

        inline void setDescription(std::string _description) {
            description = _description;
        }
        inline std::string getTag(void) const {
            return tag;
        }

        inline void setTag(std::string _tag) {
            tag = _tag;
        }
        inline std::string getUnit(void) const {
            return unit;
        }

        inline void setUnit(std::string _unit) {
            unit = _unit;
        }
        inline void setDetailsOneElement(std::string name, std::string value) {
            std::map<std::string, std::string> hash;
            hash.insert(std::pair<std::string,std::string>("name", name));
            hash.insert(std::pair<std::string,std::string>("value", value));
            details.emplace_back(hash);
        }
        inline std::vector<std::map<std::string, std::string>> getDetails(void) const {
            return details;
        }
        inline void setErrorsOneElement(std::string error) {
            errors.emplace_back(error);
        }

        inline std::vector<std::string> getErrors(void) const {
            return errors;
        }
        inline std::string getValue(void) const {
            return value;
        }

        inline void setValue(std::string _value) {
            value = _value;
        }
        inline int getStatus(void) const {
            return status;
        }

        inline void setStatus(int _status) {
            status = _status;
        }
    private:
        //! id
        long long id;
        std::string name;
        std::string description;
        std::string tag;
        std::string unit;
        std::vector<std::map<std::string, std::string>> details;
        std::vector<std::string> errors;
        int status;
        std::string value;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_LOT_DATA_H_ */

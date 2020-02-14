/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUEquipmentData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_REQUEST_EQUIPMENT_DATA_H_
#define FAMS_DATA_UTIL_REQUEST_EQUIPMENT_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUEquipmentData
     * @brief The class support methods relating organism
     */
    class FAMSDUEquipmentData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUEquipmentData(void) :
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
        inline void setImagePath(std::string _imagePath) {
            imagePath = _imagePath;
        }

        inline std::string getImagePath(void) const {
            return imagePath;
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

    private:
        //! id
        long long id;
        std::string name;
        std::string imagePath;
        std::vector<std::map<std::string, std::string>> details;
        std::vector<std::string> errors;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_EQUIPMENT_DATA_H_ */

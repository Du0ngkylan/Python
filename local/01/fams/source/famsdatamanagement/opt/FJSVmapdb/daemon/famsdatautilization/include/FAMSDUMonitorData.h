/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUMonitorData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_REQUEST_MONITOR_DATA_H_
#define FAMS_DATA_UTIL_REQUEST_MONITOR_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUMonitorData
     * @brief The class support methods relating organism
     */
    class FAMSDUMonitorData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUMonitorData(void) :
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
        inline std::string getUnit(void) const {
            return unit;
        }

        inline void setUnit(std::string _unit) {
            unit = _unit;
        }
        inline void setHistory(std::map<std::string, std::string, std::greater<std::string>> _history) {
            history = _history;
        }
        inline std::map<std::string, std::string, std::greater<std::string>> getHistory(void) const {
            return history;
        }
        inline void setErrorsOneElement(std::string error) {
            errors.emplace_back(error);
        }

        inline std::vector<std::string> getErrors(void) const {
            return errors;
        }

        inline void clearErrors(void) {
            errors.clear();
        }

        inline std::string getValue(void) const {
            return value == "0"? "" : value;
        }

        inline void setValue(std::string _value) {
            value = _value;
        }
        inline std::string getDate(void) const {
            return date;
        }

        inline void setDate(std::string _date) {
            date = _date;
        }
    private:
        //! id
        long long id;
        std::string name;
        std::string date;
        std::string unit;
        std::string value;
        std::map<std::string, std::string, std::greater<std::string>> history;
        std::vector<std::string> errors;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_MONITOR_DATA_H_ */

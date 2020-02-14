/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskSummaryData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_TASK_SUMMARY_DATA_H_
#define FAMS_DATA_UTIL_TASK_SUMMARY_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUTaskSummaryData
     * @brief The class support methods relating user
     */
    class FAMSDUTaskSummaryData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUTaskSummaryData(void) :
            id {famscommon::JSON_DOUBLE_MISSING_VALUE} {
        }

        inline int8_t getType(void) const {
            return type;
        }

        inline void setType(int8_t _type) {
            type = _type;
        }

        long long getPicUserId() const {
            return picUserId;
        }

        void setPicUserId(long long picUserId) {
            this->picUserId = picUserId;
        }

        const std::string& getProcessTime() const {
            return processTime;
        }

        void setProcessTime(const std::string& processTime) {
            this->processTime = processTime;
        }

        const std::vector<std::string> getSummary() const {
            return summary;
        }

        void setSummary(std::string _summary) {
            this->summary.emplace_back(_summary);
        }

        long long getId() const {
            return id;
        }

        void setId(long long id) {
            this->id = id;
        }

    private:
        //! id
        long long id;
        int8_t type;
        std::string processTime;
        long long picUserId;
        std::vector<std::string> summary;
    };
}

#endif /* FAMS_DATA_UTIL_TASK_SUMMARY_DATA_H_ */

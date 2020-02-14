/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskSummaryData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_TASKS_DATA_H_
#define FAMS_DATA_UTIL_TASKS_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"
#include "FAMSDUTaskSpecificsData.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUTaskSummaryData
     * @brief The class support methods relating user
     */
    class FAMSDUTasksData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUTasksData(void) :
            id {famscommon::JSON_DOUBLE_MISSING_VALUE} {
        }

        inline int8_t getType(void) const {
            return type;
        }

        inline void setType(int8_t _type) {
            type = _type;
        }

        std::string getAssignee() const {
            return assignee;
        }

        void setAssignee(std::string _assignee) {
            this->assignee = _assignee;
        }

        const std::string& getDate() const {
            return date;
        }

        void setDate(const std::string& _date) {
            this->date = _date;
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
		std::string assignee;
        std::string date;
        long long status;
        std::vector<std::string> summary;
		FAMSDUTaskSpecificsData specifics;
		std::string note;
		std::string createUser;
		std::string createDate;
		std::string updateUser;
		std::string updateDate;
    };
}

#endif /* FAMS_DATA_UTIL_TASKS_DATA_H_ */

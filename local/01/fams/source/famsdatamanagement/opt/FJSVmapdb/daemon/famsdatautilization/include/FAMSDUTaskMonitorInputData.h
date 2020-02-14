/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskMonitorInputData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_TASK_MONITOR_INPUT_DATA_H_
#define FAMS_DATA_UTIL_TASK_MONITOR_INPUT_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUTaskMonitorInputData
     * @brief The class support methods relating user
     */
    class FAMSDUTaskMonitorInputData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUTaskMonitorInputData(void) :
            id {famscommon::JSON_DOUBLE_MISSING_VALUE} {
        }
        long long getId() const {
            return id;
        }

        void setId(long long id) {
            this->id = id;
        }
        inline std::string getProcessTime(void) const {
            return processTime;
        }

        inline void setProcessTime(std::string _processTime) {
            processTime = _processTime;
        }
        long long getPICId() const {
            return PICId;
        }

        void setPICId(long long _PICId) {
            PICId = _PICId;
        }
        long long getCisternInformationId() const {
            return cisternInformationId;
        }

        void setCisternInformationId(long long _cisternInformationId) {
            cisternInformationId = _cisternInformationId;
        }

        std::string getMonitorValue() const {
            return monitorValue;
        }

        void setMonitorValue(std::string _monitorValue) {
            monitorValue = _monitorValue;
        }

        inline int getMonitorType(void) const {
            return monitorType;
        }

        inline void setMonitorType(int _monitorType) {
            monitorType = _monitorType;
        }

        inline std::string getNote(void) const {
            return note;
        }

        inline void setNote(std::string _note) {
            note = _note;
        }

    private:
        //! id
        long long id;
        std::string processTime;
        long long PICId;
        long long cisternInformationId;
        int monitorType;
        std::string monitorValue;
        std::string note;
    };
}

#endif /* FAMS_DATA_UTIL_TASK_MONITOR_INPUT_DATA_H_ */

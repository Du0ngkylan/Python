/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskWaterInputData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_TASK_WATER_INPUT_DATA_H_
#define FAMS_DATA_UTIL_TASK_WATER_INPUT_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUTaskWaterInputData
     * @brief The class support methods relating user
     */
    class FAMSDUTaskWaterInputData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUTaskWaterInputData(void) :
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
        std::string getAmount() const {
            return amount;
        }

        void setAmount(std::string _amount) {
            amount = _amount;
        }
        inline int getOperation(void) const {
            return operation;
        }

        inline void setOperation(int _operation) {
            operation = _operation;
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
        int operation;
        std::string amount;
        std::string note;
    };
}

#endif /* FAMS_DATA_UTIL_TASK_WATER_INPUT_DATA_H_ */

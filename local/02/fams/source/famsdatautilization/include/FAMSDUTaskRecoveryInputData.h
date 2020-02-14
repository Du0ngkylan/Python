/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskRecoveryInputData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_TASK_RECOVERY_INPUT_DATA_H_
#define FAMS_DATA_UTIL_TASK_RECOVERY_INPUT_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUTaskRecoveryInputData
     * @brief The class support methods relating user
     */
    class FAMSDUTaskRecoveryInputData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUTaskRecoveryInputData(void) :
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
        inline std::string getNote(void) const {
            return note;
        }

        inline void setNote(std::string _note) {
            note = _note;
        }
        inline void setLotList(const std::vector<long long> &_lotList) {
            lotList = _lotList;
        }

        inline const std::vector<long long> &getLotList(void) const {
            return lotList;
        }
        inline void setAmountList(const std::vector<std::string> &_amountList) {
            amountList = _amountList;
        }

        inline const std::vector<std::string> &getAmountList(void) const {
            return amountList;
        }

        inline void setDetail(const long long lotId, const double amount) {
            details.insert ( std::pair<long long,double>(lotId,amount) );
        }

        inline const std::multimap<long long, double> &getDetails(void) const {
            return details;
        }
    private:
        //! id
        long long id;
        std::string processTime;
        long long PICId;
        std::string note;
        std::vector<long long> lotList;
        std::vector<std::string> amountList;
        std::multimap<long long, double> details;
    };
}

#endif /* FAMS_DATA_UTIL_TASK_RECOVERY_INPUT_DATA_H_ */

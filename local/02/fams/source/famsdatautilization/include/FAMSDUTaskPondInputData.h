/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskPondInputData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_TASK_POND_INPUT_DATA_H_
#define FAMS_DATA_UTIL_TASK_POND_INPUT_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUTaskPondInputData
     * @brief The class support methods relating user
     */
    class FAMSDUTaskPondInputData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUTaskPondInputData(void) :
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
        long long getLotInformationId() const {
            return lotInformationId;
        }

        void setLotInformationId(long long _lotInformationId) {
            lotInformationId = _lotInformationId;
        }
        long long getCisternInformationId() const {
            return cisternInformationId;
        }

        void setCisternInformationId(long long _cisternInformationId) {
            cisternInformationId = _cisternInformationId;
        }
        long long getOrganismInformationId() const {
            return organismInformationId;
        }

        void setOrganismInformationId(long long _organismInformationId) {
            organismInformationId = _organismInformationId;
        }
        std::string getOrganismTag(void) const {
            return organismTag;
        }

        void setOrganismTag(std::string _organismTag) {
            organismTag = _organismTag;
        }
        std::string getOrganismUnit(void) const {
            return organismUnit;
        }

        void setOrganismUnit(std::string _organismUnit) {
            organismUnit = _organismUnit;
        }
        int getOrganismNumber() const {
            return organismNumber;
        }

        void setOrganismNumber(int _organismNumber) {
            organismNumber = _organismNumber;
        }
        long long getOrganismInitialSize() const {
            return organismInitialSize;
        }

        void setOrganismInitialSize(long long _organismInitialSize) {
            organismInitialSize = _organismInitialSize;
        }
        inline std::string getShippableTime(void) const {
            return shippableTime;
        }

        inline void setShippableTime(std::string _shippableTime) {
            shippableTime = _shippableTime;
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
        long long lotInformationId;
        long long cisternInformationId;
        long long organismInformationId;
        std::string organismTag;
        std::string organismUnit;
        int organismNumber;
        long long organismInitialSize;
        std::string shippableTime;
        std::string note;
    };
}

#endif /* FAMS_DATA_UTIL_TASK_POND_INPUT_DATA_H_ */

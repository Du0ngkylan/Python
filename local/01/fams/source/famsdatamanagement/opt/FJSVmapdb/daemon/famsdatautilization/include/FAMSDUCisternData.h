/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUCisternData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_REQUEST_CISTERN_DATA_H_
#define FAMS_DATA_UTIL_REQUEST_CISTERN_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"
#include "FAMSDULotData.h"
#include "FAMSDUEquipmentData.h"
#include "FAMSDUMonitorData.h"
#include "FAMSDUReferenceData.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUCisternData
     * @brief The class support methods relating organism
     */
    class FAMSDUCisternData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUCisternData(void) :
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

        inline std::string getCode(void) const {
            return code;
        }

        inline void setCode(std::string _code) {
            code = _code;
        }
        inline std::vector<std::map<std::string, std::string>> getIpAddress(void) const {
            return ipAddresses;
        }

        inline void setCameraOneElement(std::string _ipAddress) {
            std::map<std::string, std::string> ip_hash;
            ip_hash.insert(std::pair<std::string,std::string>("ipAddress", _ipAddress));
            ipAddresses.emplace_back(ip_hash);
        }
        inline std::vector<FAMSDULotData> getLotData(void) const {
            return lotData;
        }

        inline void setLotData(const std::vector<FAMSDULotData> _lotData) {
            lotData = _lotData;
        }
        inline std::vector<FAMSDUEquipmentData> getEquipmentData(void) const {
            return equipmentData;
        }

        inline void setEquipmentData(std::vector<FAMSDUEquipmentData> _equipmentData) {
            equipmentData = _equipmentData;
        }
        inline std::vector<FAMSDUMonitorData> getMonitorData(void) const {
            return monitorData;
        }

        inline void setMonitorData(std::vector<FAMSDUMonitorData> _monitorData) {
            monitorData = _monitorData;
        }
        inline std::vector<FAMSDUReferenceData> getReferenceData(void) const {
            return referenceData;
        }

        inline void setReferenceData(std::vector<FAMSDUReferenceData> _referenceData) {
            referenceData = _referenceData;
        }
    private:
        //! id
        long long id;
        std::string name;
        std::string code;
        std::vector<std::map<std::string, std::string>> ipAddresses;
        std::vector<FAMSDULotData> lotData;
        std::vector<FAMSDUEquipmentData> equipmentData;
        std::vector<FAMSDUMonitorData> monitorData;
        std::vector<FAMSDUReferenceData> referenceData;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_CISTERN_DATA_H_ */

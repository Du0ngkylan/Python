/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDURqTaskMaintenanceInputAnalyzer.h
 * @brief       suport JSON analysis file
 * @author      anhnguyen
 * @date        2018/12/10
 */


#ifndef FAMS_DATA_UTILIZATION_TASK_MAINTENANCE_INPUT_ANALYZER_H_
#define FAMS_DATA_UTILIZATION_TASK_MAINTENANCE_INPUT_ANALYZER_H_

#include "MDBContentAnalyzer.h"

namespace famsdatautil {

    /*!
     * @class FAMSDURqTaskMaintenanceInputAnalyzer
     * @brief Perform analyzing for input of sensor info search request
     */
    class FAMSDURqTaskMaintenanceInputAnalyzer : public mdbcom::MDBContentAnalyzer {
    public:
        /*!
         * @brief Constructor for analyze object for sensor search request
         * @param [in] parser Input decoding object.
         * @return None.
         */
        FAMSDURqTaskMaintenanceInputAnalyzer(mdbcom::MDBContentParser *parser);
        inline void setRequestType(int _requestType) {
            requestType = _requestType;
        }

    protected:
        /*!
         * @brief Searching method, override from MDBContentAnalyzer
         * @param None.
         * @return Pointer to rule table
         */
        const std::vector<mdbcom::MDBContentRuleEntry> *getRuleTable(void) override;

    private:
        //! Input Rule of request
        static std::map<int, std::vector<mdbcom::MDBContentRuleEntry>> inputRqRules;
        int requestType;
    };
}

#endif /* FAMS_DATA_UTILIZATION_TASK_MAINTENANCE_INPUT_ANALYZER_H_ */

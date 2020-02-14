/*
 * FAMSDURqOrganismAnalyzer.h
 *
 *  Created on: Jan 30, 2019
 *      Author: mapdb
 */

#ifndef FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDURQORGANISMANALYZER_H_
#define FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDURQORGANISMANALYZER_H_



#include "MDBContentAnalyzer.h"

namespace famsdatautil {

    /*!
     * @class FAMSDURqOrganismAnalyzer
     * @brief Perform analyzing for input of sensor info search request
     */
    class FAMSDURqOrganismAnalyzer : public mdbcom::MDBContentAnalyzer {
    public:
        /*!
         * @brief Constructor for analyze object for sensor search request
         * @param [in] parser Input decoding object.
         * @return None.
         */
        FAMSDURqOrganismAnalyzer(mdbcom::MDBContentParser *parser);
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



#endif /* FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDURQORGANISMANALYZER_H_ */

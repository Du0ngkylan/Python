/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRRqNitrificationAnalyzer.h
 * @brief       Header file for class FAMSDRRqNitrificationAnalyzer.
 * @author      anhnguyen
 * @date        2018/12/11
 */

#ifndef FAMS_DATA_RECEPTION_NITRIFICATION_REQUEST_ANALYZE_H_
#define FAMS_DATA_RECEPTION_NITRIFICATION_REQUEST_ANALYZE_H_

#include "MDBContentAnalyzer.h"

namespace famsdatareception {

    /*!
     * @class FAMSDRRqNitrificationAnalyzer
     * @brief Perform analyzing for input of dynamic object request
     */
    class FAMSDRRqNitrificationAnalyzer : public mdbcom::MDBContentAnalyzer {
    public:
        /*!
         * @brief Construtor for analyze object for add request
         * @param [in] parser Input decoding object.
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        FAMSDRRqNitrificationAnalyzer(mdbcom::MDBContentParser *parser);

    protected:
        /*!
         * @brief Searching method, override from MDBContentAnalyzer
         * @param None.
         * @return Pointer to rule table
         */
        const std::vector<mdbcom::MDBContentRuleEntry> *getRuleTable(void) override;

    private:
        //! Analyze table for dynamic object add request
        static std::vector<mdbcom::MDBContentRuleEntry> inputRules;

    };
}

#endif /* FAMS_DATA_RECEPTION_NITRIFICATION_REQUEST_ANALYZE_H_ */

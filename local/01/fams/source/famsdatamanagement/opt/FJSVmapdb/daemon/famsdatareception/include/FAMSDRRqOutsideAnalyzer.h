/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRRqOutsideAnalyzer.h
 * @brief       Header file for class FAMSDRRqOutsideAnalyzer.
 * @author      anhnguyen
 * @date        2018/12/11
 */

#ifndef FAMS_DATA_RECEPTION_OUTSIDE_REQUEST_ANALYZE_H_
#define FAMS_DATA_RECEPTION_OUTSIDE_REQUEST_ANALYZE_H_

#include "MDBContentAnalyzer.h"

namespace famsdatareception {

    /*!
     * @class FAMSDRRqOutsideAnalyzer
     * @brief Perform analyzing for input of dynamic object request
     */
    class FAMSDRRqOutsideAnalyzer : public mdbcom::MDBContentAnalyzer {
    public:
        /*!
         * @brief Construtor for analyze object for add request
         * @param [in] parser Input decoding object.
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        FAMSDRRqOutsideAnalyzer(mdbcom::MDBContentParser *parser);

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

#endif /* FAMS_DATA_RECEPTION_OUTSIDE_REQUEST_ANALYZE_H_ */

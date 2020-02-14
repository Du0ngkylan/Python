/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSRqBaseData.h
 * @brief       The parent class of data classes.
 *              All data classes inherited from this class are matched with Request Data
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_BASE_REQUEST_DATA_H_
#define FAMS_BASE_REQUEST_DATA_H_

#include "MDBException.h"
#include "FAMSCommon.h"

namespace famscommon {
    /*!
     * @class FAMSRqBaseData
     * @brief Base class for holding common data JSON request
     */
    class FAMSRqBaseData {
    public:
        /*!
         * @brief FAMSRqBaseData constructor
         *
         * @param None
         * @return None
         */
        FAMSRqBaseData(void)
        {
        }

        /*!
         * @brief FAMSRqBaseData default destructor
         *
         * @param None
         * @return None
         */
        virtual ~FAMSRqBaseData(void) = default;

    };
}

#endif /* FAMS_BASE_REQUEST_DATA_H_ */

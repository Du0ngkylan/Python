/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUObserversData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_OBSERVERS_REQUEST_DATA_H_
#define FAMS_DATA_UTIL_OBSERVERS_REQUEST_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUObserversData
     * @brief The class support methods relating user
     */
    class FAMSDUObserversData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUObserversData(void) :
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

        inline std::string getMail(void) const {
            return mail;
        }

        inline void setMail(std::string _mail) {
            mail = _mail;
        }

    private:
        //! id
        long long id;
        std::string mail;
    };
}

#endif /* FAMS_DATA_UTIL_OBSERVERS_REQUEST_DATA_H_ */

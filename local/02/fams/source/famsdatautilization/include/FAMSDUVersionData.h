/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUVersionData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_VERSION_REQUEST_DATA_H_
#define FAMS_DATA_UTIL_VERSION_REQUEST_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUVersionData
     * @brief The class support methods relating user
     */
    class FAMSDUVersionData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUVersionData(void)
//            : id {famscommon::JSON_DOUBLE_MISSING_VALUE}
        {
        }

        inline std::string getVersion(void) const {
            return version;
        }

        inline void setVersion(std::string _version) {
            version = _version;
        }

    private:
        //! id
        std::string version;
    };
}

#endif /* FAMS_DATA_UTIL_VERSION_REQUEST_DATA_H_ */

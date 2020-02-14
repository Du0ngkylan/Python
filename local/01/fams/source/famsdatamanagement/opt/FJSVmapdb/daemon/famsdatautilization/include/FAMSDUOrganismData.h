/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUOrganismData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_ORGANISM_DATA_H_
#define FAMS_DATA_UTIL_ORGANISM_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"
#include "FAMSDUOrganismThreshold.h"
namespace famsdatautil {
    /*!
     * @class FAMSDUOrganismData
     * @brief The class support methods relating organism
     */
    class FAMSDUOrganismData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUOrganismData(void) :
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

        inline void setThresHold(FAMSDUOrganismThreshold item) {
            thresHoldList.push_back(item);
        }

        inline std::vector<FAMSDUOrganismThreshold> getThresHoldList() const{
            return thresHoldList;
        }

        inline std::string getName(void) const {
            return name;
        }

        inline void setName(std::string _name) {
            name = _name;
        }
    private:
        //! id
        long long id;
        std::string name;
        std::vector <FAMSDUOrganismThreshold> thresHoldList;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_DATA_H_ */

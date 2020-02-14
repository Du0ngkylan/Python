/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUUserData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_REQUEST_DATA_H_
#define FAMS_DATA_UTIL_REQUEST_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUUserData
     * @brief The class support methods relating user
     */
    class FAMSDUUserData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUUserData(void) :
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

        inline std::string getUserId(void) const {
            return userId;
        }

        inline void setUserId(std::string _userId) {
            userId = _userId;
        }

        inline std::string getActivities(void) const {
            return activities;
        }

        inline void setActivities(std::string _activities) {
			activities = _activities;
        }

        inline std::string getMail(void) const {
            return mail;
        }

        inline void setMail(std::string _mail) {
            mail = _mail;
        }

        inline std::string getName(void) const {
            return name;
        }

        inline void setName(std::string _Name) {
            name = _name;
        }

		inline std::string getRole(void) const {
			return role;
		}

		inline void setRole(std::string _role) {
			role = _role;
		}
    private:
        //! id
        long long id;
        std::string userId;
        std::string name;
        std::string mail;
        std::string activities;
		std::string role;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_DATA_H_ */

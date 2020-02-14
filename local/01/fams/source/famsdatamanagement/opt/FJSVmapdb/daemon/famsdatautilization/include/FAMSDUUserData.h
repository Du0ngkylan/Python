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

        inline std::string getUsername(void) const {
            return username;
        }

        inline void setUsername(std::string _userName) {
            username = _userName;
        }

        inline std::string getPassword(void) const {
            return password;
        }

        inline void setPassword(std::string _password) {
            password = _password;
        }

        inline std::string getMail(void) const {
            return mail;
        }

        inline void setMail(std::string _mail) {
            mail = _mail;
        }

        inline std::string getFullName(void) const {
            return fullName;
        }

        inline void setFullName(std::string _fullName) {
            fullName = _fullName;
        }
    private:
        //! id
        long long id;
        std::string username;
        std::string password;
        std::string mail;
        std::string fullName;
    };
}

#endif /* FAMS_DATA_UTIL_REQUEST_DATA_H_ */

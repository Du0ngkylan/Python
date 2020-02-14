/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUSerializeObserversData.h
 * @brief       request data defination file
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_SERIALIZE_OBSERVERS_REQUEST_DATA_H_
#define FAMS_DATA_UTIL_SERIALIZE_OBSERVERS_REQUEST_DATA_H_

#include <vector>
#include <string>

namespace famsdatautil {
    /*!
     * @class FAMSDUSerializeObserversData
     * @brief The class support methods relating user
     */
    class FAMSDUSerializeObserversData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUSerializeObserversData(void) :
            mailList {}, id {0}, mail{""}
        {
        }


        inline std::vector<std::string> getMail(void) const
        {
            return mailList;
        }

        inline void setMail(const std::vector<std::string> &_mail)
        {
            mailList = _mail;
        }


        inline std::string getSingleMail(void) const
        {
            return mail;
        }

        inline void setSingleMail(std::string _mail)
        {
            mail = _mail;
        }

        inline long long getId(void) const
        {
            return id;
        }

        inline void setId(const long long &_id)
        {
            id = _id;
        }
    private:
        //! id
        std::vector<std::string> mailList;
        std::string mail;
        long long id;
    };
}

#endif /* FAMS_DATA_UTIL_SERIALIZE_OBSERVERS_REQUEST_DATA_H_ */

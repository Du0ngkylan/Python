/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBSyslogContent.h
 * @brief   Defination for syslog content
 * @author  trunghn:
 * @date    2016/10/27
 */
#ifndef LIBMDBLOG_MDBSYSLOGCONTENT_H_
#define LIBMDBLOG_MDBSYSLOGCONTENT_H_

#include <string>

namespace mdbcom {

    /*!
     * @class MDBSyslogContent
     * @brief struct to store syslog message content in setting file
     */
    class MDBSyslogContent {
    public:

        /*!
         * @brief syslog content constructor
         *
         * Init default value.
         *
         * @param None.
         * @return None.
         */
        MDBSyslogContent(void) = default; /* pgr0571 *//* pgr2266 */

        /*!
         * @brief syslog content constructor
         *
         * Init default value.
         *
         * @param [in] inPriority priority
         * @param [in] inFacility facility
         * @param [in] inContent  content
         * @return None.
         */
        MDBSyslogContent(const std::string &inPriority, const std::string &inFacility,
                         const std::string &inContent);

        /**
         * @brief Copy constructor
         *
         * @param [in] other Other object
         * @return None.
         */
        MDBSyslogContent(const MDBSyslogContent &) = default; /* pgr2222 */ /* pgr0571 */

        /**
         * @brief Copy Assignment
         *
         * @param [in] other Other object
         * @return Calling instance of MDBSyslogContent.
         */
        MDBSyslogContent &operator =(const MDBSyslogContent &) = default; /* pgr2223 *//* pgr0309 */ /* pgr0055 */

        /*!
         * @brief destructor
         *
         * @param None.
         * @return None.
         */
        ~MDBSyslogContent(void) = default;

        /*!
         * @brief Set priority for Syslog content
         *
         * @param [in] inPriority syslog priority
         * @return None.
         */
        inline void setPriority(const std::string &inPriority) {
            this->priority = inPriority;
        }

        /*!
         * @brief Get priority of Syslog content
         *
         * @param None.
         * @return syslog priority
         */
        inline const std::string &getPriority(void) const {
            return this->priority;
        }

        /*!
         * @brief Set facility for Syslog content
         *
         * @param [in] inFacility syslog facility
         * @return None.
         */
        inline void setFacility(const std::string &inFacility) {
            this->facility = inFacility;
        }

        /*!
         * @brief Get facility of Syslog content
         *
         * @param None.
         * @return syslog facility
         */
        inline const std::string &getFacility(void) const {
            return this->facility;
        }

        /*!
         * @brief Set message content for Syslog content
         *
         * @param [in] inContent syslog message content
         * @return None.
         */
        inline void setContent(const std::string &inContent) {
            this->content = inContent;
        }

        /*!
         * @brief Get syslog message content of Syslog content
         *
         * @param None.
         * @return syslog message content
         */
        inline const std::string &getContent(void) const {
            return this->content;
        }

    private:
        //! Priority of message
        std::string priority;
        //! Facility of message
        std::string facility;
        //! Content of message
        std::string content;
    };
}

#endif /*End LIBMDBLOG_MDBSYSLOGCONTENT_H_*/

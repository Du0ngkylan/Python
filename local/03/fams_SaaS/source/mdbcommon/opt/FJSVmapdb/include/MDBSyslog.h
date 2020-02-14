/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBSyslog.h
 * @brief   Defination for syslog
 * @author  trunghn:
 * @date    2016/9/12
 */

#ifndef LIBMDBLOG_MDBSYSLOG_H_
#define LIBMDBLOG_MDBSYSLOG_H_

#include <map>
#include <memory>
#include <unordered_map>

#include <Poco/AutoPtr.h>
#include <Poco/SyslogChannel.h>

#include "MDBSyslogContent.h"

namespace mdbcom {
    /*!
     * @class MDBSyslog
     *  @brief syslog class handle output syslog
     */
    class MDBSyslog { //Logging wrapper class
    public:
        /*!
         * @brief MDBSyslog constructor
         *
         * Create a syslog channel and open it.
         *
         * @param [in] processName process name
         * @param [in] syslogConfPath syslog config file path
         * @return None
         */
        MDBSyslog(const std::string &processName, const std::string &syslogConfPath);

        /**
         * @brief Copy constructor
         *
         * @param [in] other Other object
         * @return None.
         */
        MDBSyslog(const MDBSyslog &) = delete; /* pgr2222 */ /* pgr0571 */

        /**
         * @brief Copy Assignment
         *
         * @param [in] other Other object
         * @return Calling instance of MDBSyslogContent.
         */
        MDBSyslog &operator =(const MDBSyslog &) = delete; /* pgr0055 *//* pgr0309 */ /* pgr2223 */

        /*!
         * @brief MDBSyslog destructor
         *
         * close  syslog channel
         *
         * @param None
         * @return None
         */
        ~MDBSyslog(void);

        /*!
         * @brief out put syslog
         *
         * create syslog message format and call to output message
         *
         * @param [in] msgNo message no
         * @param [in] syslogContent syslog content
         * @param [in] args list of argument
         * @return None
         */
        void outputSyslog(const std::string &msgNo, const MDBSyslogContent *syslogContent,
                          va_list args);

        /*!
         * @brief find syslog message
         *
         *
         * @param [in] msgNo syslog message no
         * @return syslog message content
         */
        static const MDBSyslogContent *findMsg(const std::string &msgNo);

        /*!
         * @brief out put syslog via system call
         *
         * Output syslog in case of log system failure
         *
         * @param [in] msgNo message no
         * @param [in] message log message
         * @return None
         */
        static void systemLog(const std::string &msgNo, const std::string &message);
    private:
        //! Sys log message map
        using SyslogMessageMap = std::map<std::string, std::unique_ptr<MDBSyslogContent>>;

        //! Coresponding string for facility local0
        static const std::string STR_LOG_LOCAL0;
        //! Coresponding string for facility local1
        static const std::string STR_LOG_LOCAL1;
        //! Coresponding string for facility local2
        static const std::string STR_LOG_LOCAL2;
        //! Coresponding string for facility local3
        static const std::string STR_LOG_LOCAL3;
        //! Coresponding string for facility local4
        static const std::string STR_LOG_LOCAL4;
        //! Coresponding string for facility local5
        static const std::string STR_LOG_LOCAL5;
        //! Coresponding string for facility local6
        static const std::string STR_LOG_LOCAL6;
        //! Coresponding string for facility local7
        static const std::string STR_LOG_LOCAL7;
        //! Mapping string for Poco syslog setting
        static const std::unordered_map<std::string, std::string> LOG_FACILITY_CONF;
        //! Error message for log content
        static const std::string ERROR_LOG_CONTENT;
        //! Error message when output log
        static const std::string ERROR_OUTPUT_LOG;
        //! Error message when output log
        static const std::string ERROR_LOG_MODULE;

        // Syslog message list
        static SyslogMessageMap sysMsgList;

        //! Channel to output log to file
        Poco::AutoPtr<Poco::SyslogChannel> sysChannel;

        /*!
         * @brief load syslog message in setting file
         *
         *
         * @param [in] fileName setting file name
         *
         * @retval None
         */
        static void loadSysMessage(const std::string &syslogConfPath);
    };

}
#endif /* End LIBMDBLOG_MDBSYSLOG_H */

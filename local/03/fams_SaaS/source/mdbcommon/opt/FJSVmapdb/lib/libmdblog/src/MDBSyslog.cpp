/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBSyslog.cpp
 * @brief   Implementation of syslog
 * @author  trunghn:
 * @date    2016/9/12
 */
#include <iostream>
#include <fstream>
#include <thread>
#include <syslog.h>

#include <Poco/StringTokenizer.h>
#include <Poco/Message.h>

#include "MDBMemoryUtility.h"
#include "MDBException.h"
#include "MDBSyslog.h"
#include "MDBConstants.h"
#include "MDBConfigFile.h"

namespace mdbcom {

    //! Coresponding string for facility local0
    const std::string MDBSyslog::STR_LOG_LOCAL0
    {
        "LOG_LOCAL0"
    };

    //! Coresponding string for facility local1
    const std::string MDBSyslog::STR_LOG_LOCAL1
    {
        "LOG_LOCAL1"
    };

    //! Coresponding string for facility local2
    const std::string MDBSyslog::STR_LOG_LOCAL2
    {
        "LOG_LOCAL2"
    };

    //! Coresponding string for facility local3
    const std::string MDBSyslog::STR_LOG_LOCAL3
    {
        "LOG_LOCAL3"
    };

    //! Coresponding string for facility local4
    const std::string MDBSyslog::STR_LOG_LOCAL4
    {
        "LOG_LOCAL4"
    };

    //! Coresponding string for facility local5
    const std::string MDBSyslog::STR_LOG_LOCAL5
    {
        "LOG_LOCAL5"
    };

    //! Coresponding string for facility local6
    const std::string MDBSyslog::STR_LOG_LOCAL6
    {
        "LOG_LOCAL6"
    };

    //! Coresponding string for facility local7
    const std::string MDBSyslog::STR_LOG_LOCAL7
    {
        "LOG_LOCAL7"
    };

    //! Error message for log content
    const std::string MDBSyslog::ERROR_LOG_CONTENT
    {
        "Error in log content "
    };

    //! Error message when output log
    const std::string MDBSyslog::ERROR_OUTPUT_LOG
    {
        "Error in output syslog "
    };

    //! Mapping string for Poco syslog setting
    const std::unordered_map<std::string, std::string> MDBSyslog::LOG_FACILITY_CONF =
    {
        { "local0", MDBSyslog::STR_LOG_LOCAL0 },
        { "local1", MDBSyslog::STR_LOG_LOCAL1 },
        { "local2", MDBSyslog::STR_LOG_LOCAL2 },
        { "local3", MDBSyslog::STR_LOG_LOCAL3 },
        { "local4", MDBSyslog::STR_LOG_LOCAL4 },
        { "local5", MDBSyslog::STR_LOG_LOCAL5 },
        { "local6", MDBSyslog::STR_LOG_LOCAL6 },
        { "local7", MDBSyslog::STR_LOG_LOCAL7 }
    };

    MDBSyslog::SyslogMessageMap MDBSyslog::sysMsgList {};
    //! Error message when output log
    const std::string MDBSyslog::ERROR_LOG_MODULE
    {
        "Abnomality occurred at log function."
    };

    /*!
     * @brief MDBSyslog constructor
     *
     * Create a syslog channel and open it.
     *
     * @param [in] processName process name
     * @param [in] syslogConfPath syslog config file path
     * @return None
     */
    MDBSyslog::MDBSyslog(const std::string &processName, const std::string &syslogConfPath) :
        sysChannel(nullptr)
    {

        try
        {
            loadSysMessage(syslogConfPath);

            // Create syslog channel
            sysChannel = new Poco::SyslogChannel { processName,
                                                   Poco::SyslogChannel::SYSLOG_CONS |
                                                   Poco::SyslogChannel::SYSLOG_NDELAY |
                                                   Poco::SyslogChannel::SYSLOG_PID,
                                                   Poco::SyslogChannel::SYSLOG_LOCAL0
                                                 };

            sysChannel->open();
            // Catch exception then delete syslog content which already new successfully
        }
        catch (const std::bad_alloc &ex)
        {
            throw std::bad_alloc(ex);
        }
        catch (const MDBException &ex)
        {
            throw;
        }
        catch (...)
        {
            throw MDBException("There is a exception happen!");
        }

    } /* pgr0840 */

    /*!
     * @brief MDBSyslog destructor
     *
     * close  syslog channel
     *
     * @param None
     * @return None
     */
    MDBSyslog::~MDBSyslog()
    {

        // Close syslog channel
        if (nullptr != sysChannel)
        {
            sysChannel->close();
        }
    }

    /*!
     * @brief find syslog message
     *
     *
     * @param [in] msgNo syslog message no
     * @return syslog message content
     */
    const MDBSyslogContent *MDBSyslog::findMsg(const std::string &msgNo)
    {
        // Find syslog message by message no
        auto iter = sysMsgList.find(msgNo); /* pgr0745 */

        if (iter == sysMsgList.end())
        {
            return nullptr;
        }

        return (iter->second).get();
    }

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
    void MDBSyslog::outputSyslog(const std::string &msgNo, const MDBSyslogContent *syslogContent,
                                 va_list args)
    {

        if (nullptr == sysChannel)
        {
            return;
        }

        if (nullptr == syslogContent)
        {
            auto logMsg = std::string { ERROR_LOG_CONTENT }; /* pgr0745 */
            logMsg += msgNo;
            sysChannel->setProperty(Poco::SyslogChannel::PROP_FACILITY, STR_LOG_LOCAL7);
            auto message = Poco::Message { sysChannel->getProperty("name"), logMsg, /* pgr0745 */
                                           Poco::Message::PRIO_CRITICAL };
            sysChannel->log(message);
            return;
        }

        // auto event = std::array<char, Log::CONTENT_MAX_LEN> { '\0' }; /* pgr0745 */
        auto logStr = std::array<char, Log::MSG_MAX_LEN> { '\0' }; /* pgr0745 */
        auto sysLogContent = syslogContent->getContent().c_str(); /* pgr0745 */

        auto sysLogPrio = syslogContent->getPriority().c_str(); /* pgr0745 */
        auto sysLogMsgNo = msgNo.c_str(); /* pgr0745 */
        auto threadID = std::hash<std::thread::id>()(std::this_thread::get_id()); /* pgr0745 */

        auto wrPosition = logStr.data(); /* pgr0745 */
        auto wrSize = logStr.size(); /* pgr0745 */

        auto wrCount = std::snprintf(wrPosition, wrSize, /* pgr0579 *//* pgr1170 *//* pgr0745 */
                                     " [%lu]: [%.10s] %.10s ", threadID, sysLogPrio, sysLogMsgNo);

        if (0 > wrCount)
        {
            auto logMsg = std::string { ERROR_OUTPUT_LOG }; /* pgr0745 */
            logMsg += msgNo;
            sysChannel->setProperty(Poco::SyslogChannel::PROP_FACILITY, STR_LOG_LOCAL7);
            auto message = Poco::Message { sysChannel->getProperty("name"), logMsg, /* pgr0745 */
                                           Poco::Message::PRIO_CRITICAL };
            sysChannel->log(message);
            return;
        }

        wrPosition += wrCount;
        wrSize -= wrCount;

        // Format syslog content
        wrCount = std::vsnprintf(wrPosition, wrSize, sysLogContent, args); /* pgr0579 *//* pgr1170 */

        if (0 > wrCount)
        {
            auto logMsg = std::string { ERROR_OUTPUT_LOG }; /* pgr0745 */
            logMsg += msgNo;
            sysChannel->setProperty(Poco::SyslogChannel::PROP_FACILITY, STR_LOG_LOCAL7);
            auto message = Poco::Message { sysChannel->getProperty("name"), logMsg, /* pgr0745 */
                                           Poco::Message::PRIO_CRITICAL };
            sysChannel->log(message);
            return;
        }

        //each syslog message has a different facility
        sysChannel->setProperty(Poco::SyslogChannel::PROP_FACILITY, syslogContent->getFacility());
        // Create message log and write log
        auto message = Poco::Message { sysChannel->getProperty("name"), logStr.data(), /* pgr0745 */
                                       Poco::Message::PRIO_INFORMATION };
        sysChannel->log(message);
    }

    /*!
     * @brief out put syslog via system call
     *
     * Output syslog in case of log system failure
     *
     * @param [in] msgNo message no
     * @param [in] message log message
     * @return None
     */
    void MDBSyslog::systemLog(const std::string &msgNo, const std::string &message)
    {
        std::ifstream comm { Process::NAME_PATH };
        auto processName = std::string { }; /* pgr0745 */
        static_cast<void>(std::getline(comm, processName));
        openlog(processName.c_str(), LOG_CONS | LOG_NDELAY | LOG_PID, LOG_LOCAL7);
        auto threadID = std::hash<std::thread::id>()(std::this_thread::get_id()); /* pgr0745 */

        if ("F000702" == msgNo)
        {
            auto strMsgNo = msgNo.c_str(); /* pgr0745 */
            auto strContent = ERROR_LOG_MODULE.c_str(); /* pgr0745 */
            auto strMessage = message.c_str(); /* pgr0745 */
            syslog(LOG_LOCAL7 | LOG_CRIT, " [%lu]: [crit] %s %s(message:%s)",
                   threadID, strMsgNo, strContent, strMessage);
        }

        closelog();
    }

    /*!
     * @brief load syslog message in setting file
     *
     *
     * @param [in] fileName setting file name
     *
     * @retval None
     */
    void MDBSyslog::loadSysMessage(const std::string &syslogConfPath)
    {
        std::string msgNo;
        // Load syslog message from config file
        auto configFile = MDBConfigFile { syslogConfPath }; /* pgr0745 */
        const auto &confMap = configFile.getMapValue(); /* pgr0745 */

        for (auto &pair : confMap)
        {
            msgNo = pair.first;
            Poco::StringTokenizer msgContent { pair.second, ",",
                                               Poco::StringTokenizer::Options::TOK_TRIM };

            // avoid syslog message that contain ','
            auto &syslogConf = pair.second;
            auto content = syslogConf.substr(syslogConf.find(msgContent[2])); /* pgr0745 */

            static_cast<void>(sysMsgList.emplace(
                                  std::make_pair(msgNo,
                                                 MDBMemoryUtility::make_unique<MDBSyslogContent>(msgContent[0],
                                                         LOG_FACILITY_CONF.at(msgContent[1]), content))));
        }
    }
}

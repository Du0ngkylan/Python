/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBLogger.cpp
 * @brief   Implementation of logger
 * @author  trunghn:
 * @date    2016/9/12
 */

#include <iostream>
#include <cstdarg>
#include <fstream>

#include <Poco/SyslogChannel.h>

#include "MDBLogger.h"
#include "MDBSharedMemoryUtility.h"
#include "MDBConfigFile.h"
#include "MDBException.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {

    // Constant
    const std::string MDBLogger::DEBUG_LOG_CONF_PATH
    {
        "/opt/FJSVmapdb/etc/debug_log_rotate.conf"
    };
    const std::string MDBLogger::SYSLOG_CONF_PATH { "/opt/FJSVmapdb/etc/syslog.conf" };
    const std::string MDBLogger::DEFAULT_DEBUG_DIRECT_LOG_PATH { "/tmp/" };
    const std::string MDBLogger::DEFAULT_DEBUG_STORE_LOG_PATH { "/tmp/" };
    const std::string MDBLogger::DEBUG_LOG_DIRECT_PATH_KEY { "live_path" };
    const std::string MDBLogger::DEBUG_LOG_FILE_SIZE_KEY { "size" };
    const std::string MDBLogger::DEBUG_LOG_STORE_PATH_KEY { "store_path" };
    // Default process name
    const std::string MDBLogger::DEFAULT_PROCESS_NAME
    {
        "default_process"
    };
    // Path to debug log
    std::string MDBLogger::debugLogPath;
    // Path to storage log folder
    std::string MDBLogger::storePath;
    // Maximum size for each log file
    unsigned int MDBLogger::debugLogSize { 0 };
    // Init logger flag
    bool MDBLogger::initFlag { false };
    // Syslog instance
    std::unique_ptr<MDBSyslog> MDBLogger::syslog { nullptr };
    // Level map that point to level shared mem
    MDBDebugLevelMap *MDBLogger::levelMap { nullptr };
    // Debug log for each thread
    thread_local std::unique_ptr<MDBDebugLog> MDBLogger::debugLog { nullptr };
    //! Module name
    std::string MDBLogger::processName {};
    /*!
     * @brief init log
     *
     * Init data for log, this function must be call when start application
     *
     * @param [in]  moduleName module name
     *
     * @retval Result::OK success
     * @retval Result::NG failure
     */
    int MDBLogger::initLog(const std::string &moduleName)
    {

        if (true == initFlag)
        {
            return Result::OK;
        }

        try
        {
            auto configFile = MDBConfigFile { DEBUG_LOG_CONF_PATH }; /* pgr0745 */
            // Get debug log direct path
            auto result = std::string { }; /* pgr0745 */
            auto ret = configFile.getValue(DEBUG_LOG_DIRECT_PATH_KEY, debugLogPath); /* pgr0745 */

            if (Result::OK != ret)
            {
                debugLogPath = DEFAULT_DEBUG_DIRECT_LOG_PATH;
            }

            // Get debug log file size
            ret = configFile.getValue(DEBUG_LOG_FILE_SIZE_KEY, result);

            if (Result::OK == ret)
            {
                debugLogSize = std::stoul(result);
            }
            else
            {
                debugLogSize = DEFAULT_DEBUG_LOG_SIZE;
            }

            // Get debug log store path
            ret = configFile.getValue(DEBUG_LOG_STORE_PATH_KEY, storePath);

            if (Result::OK != ret)
            {
                storePath = DEFAULT_DEBUG_STORE_LOG_PATH;
            }
        }
        catch (const MDBException &exp)
        {
            MDBSyslog::systemLog("F000702", exp.what());
            return Result::NG_CONFIGURATION;
        }
        catch (...)
        {
            MDBSyslog::systemLog("F000702", "Error load log configuration");
            return Result::NG_CONFIGURATION;
        }

        //Init syslog
        // Get process name
        processName = moduleName;

        try
        {
            syslog = MDBMemoryUtility::make_unique<MDBSyslog>(processName, SYSLOG_CONF_PATH); /* pgr0014 *//* pgr2711 */
        }
        catch (const std::bad_alloc &)
        {
            MDBSyslog::systemLog("F000702", "Error initializing syslog: memory insufficient");
            return Result::NG_SYSLOG;
        }
        catch (...)
        {
            MDBSyslog::systemLog("F000702", "Error initializing syslog");
            return Result::NG_SYSLOG;
        }

        // Init module level map
        auto sharedName = Ipc::LOG_SHARED_NAME; /* pgr0745 */
        auto shmMode = ShmModes::READ; /* pgr0745 */
        // open setting level shared memory to get level
        levelMap = reinterpret_cast<MDBDebugLevelMap *>(/* pgr0745 */
                       MDBSharedMemoryUtility::getShm(sharedName, sizeof(MDBDebugLevelMap), shmMode));

        if (nullptr == levelMap)
        {
            // do not release
            mdb_sys_log_out("F000701", "Error getting debuglog level");
            return Result::NG_SHARED_MEM;
        }

        initFlag = true;
        return Result::OK;
    }

    /*!
     * @brief Open debug log
     *
     * Init and open a debug log with name as module name,
     * this function must be call when start each thread
     *
     * @param [in]  threadName Thread name
     * @retval Result::OK success
     * @retval Result::NG failure
     */
    int MDBLogger::openDebugLog(const std::string &threadName)
    {

        if (false == initFlag)
        {
            MDBSyslog::systemLog("F000702", "Unitialized log module.");
            return Result::NG;
        }

        if (nullptr == levelMap)
        {
            mdb_sys_log_out("F000701", "Un-initialized shared memory for log");
            return Result::NG;
        }

        if (nullptr != debugLog)
        {
            // debugLog has already been created
            return Result::OK;
        }

        // Map level index
        auto mapIndex = 0; /* pgr0745 */
        auto &map = levelMap->data; /* pgr0745 */

        // Get index of debug log level by module name
        for (mapIndex = 0; mapIndex < levelMap->count; ++mapIndex)   /* pgr0692 */
        {
            if (processName == map[mapIndex].function.data())
            {
                break;
            }
        }

        if (levelMap->count == mapIndex)
        {
            auto errMessage = std::string { "Can't find module name: " }; /* pgr0745 */
            errMessage += processName;
            errMessage += " [searched in ";
            errMessage += std::to_string(mapIndex) ;
            errMessage += " entries]";
            mdb_sys_log_out("F000701", errMessage.c_str());
            return Result::NG;
        }

        try
        {
            // Create debug log object
            debugLog = MDBMemoryUtility::make_unique<MDBDebugLog>(processName, threadName,
                       &map[mapIndex].level, debugLogPath, /* pgr0014 *//* pgr2711 */
                       storePath, debugLogSize);
        }
        catch (const std::bad_alloc &)
        {
            auto errMessage = std::string { "Can't create debuglog for thread: " }; /* pgr0745 */
            errMessage += threadName;
            mdb_sys_log_out("F000701", errMessage.c_str());
            return Result::NG;
        }
        catch (...)
        {
            auto errMessage = std::string { "Can't create debuglog for thread: " }; /* pgr0745 */
            errMessage += threadName;
            mdb_sys_log_out("F000701", errMessage.c_str());
            return Result::NG;
        }

        return Result::OK;
    }

    /*!
     * @brief out put syslog
     *
     * Output syslog to rsyslog
     *
     * @param [in]  msgNo message no
     * @param [in]  ...  list of message argument
     * @return None
     */
    void MDBLogger::outputSyslog(std::string msgNo, ...)
    {
        if (nullptr == syslog)
        {
            // Normal case, if log is used by command, no need to confuse user.
            return;
        }

        // Fine syslog by message no
        auto syslogContent = syslog->findMsg(msgNo); /* pgr0745 */

        if (nullptr == syslogContent)
        {
            Poco::AutoPtr<Poco::SyslogChannel> sysChannel { nullptr };

            try
            {
                sysChannel = new Poco::SyslogChannel { "",
                                                       Poco::SyslogChannel::SYSLOG_CONS |
                                                       Poco::SyslogChannel::SYSLOG_NDELAY |
                                                       Poco::SyslogChannel::SYSLOG_PID,
                                                       Poco::SyslogChannel::SYSLOG_LOCAL7 };
            }
            catch (const std::bad_alloc &)
            {
                return; /* pgr0840 */
            }
            catch (...)
            {
                return; /* pgr0840 */
            }

            auto logMsg = std::string{"Error findind log content for: "}; /* pgr0745 */
            logMsg += msgNo;
            auto message = Poco::Message { sysChannel->getProperty("name"), logMsg, /* pgr0745 */
                                           Poco::Message::PRIO_CRITICAL };
            sysChannel->log(message);
            return; /* pgr0840 */
        }

        // Output syslog
        va_list args;
        va_start(args, msgNo); /* pgr0039 */
        syslog->outputSyslog(msgNo, syslogContent, args);
        va_end(args);
    }

    /*!
     * @brief Out put debug log
     *
     * @param [in]  msgPrio message priority
     * @param [in]  line line of code that call out log
     * @param [in]  methodName function name that call out log
     * @param [in]  fileName file name that call write log
     * @param [in]  msgFormat message format
     * @param [in]  ... message list param
     * @return None.
     */
    void MDBLogger::outputDebugLog(Poco::Message::Priority msgPrio, int line,
                                   const std::string &methodName, const std::string &fileName,
                                   std::string msgFormat, ...)
    {
        // Validate message , priority
        if (nullptr == debugLog)
        {
            // Normal case, if log is used by command, no need to confuse user.
            return;
        }

        if (true == msgFormat.empty())
        {
            auto errorMessage = std::string { "Log format empty " };
            errorMessage += fileName;
            errorMessage += ":";
            errorMessage += std::to_string(line);
            mdb_sys_log_out("F000701", errorMessage.c_str());
            return;
        }

        if (msgPrio > MDBDebugLog::convertLevelToPriority(*(debugLog->getPriority())))
        {
            // normal case, no need to output log
            return;
        }

        // Out put debug log
        va_list args;
        va_start(args, msgFormat); /* pgr0039 */
        debugLog->outputAPLLog(msgPrio, line, methodName, fileName, msgFormat, args);
        va_end(args);
    }

    /*!
     * @brief Out put debug start/end log
     *
     * @param [in]  line line of code that call out log
     * @param [in]  methodName function name that call out log
     * @param [in]  fileName file name that call write log
     * @param [in]  logPrefix log start/end prefix
     * @return None.
     */
    void MDBLogger::outputStartEndLog(int line, const std::string &methodName,
                                      const std::string &fileName, std::string logPrefix, ...)
    {
        if (nullptr == debugLog)
        {
            // Normal case, if log is used by command, no need to confuse user.
            return;
        }

        // Validate priority
        if (Poco::Message::PRIO_DEBUG
                > MDBDebugLog::convertLevelToPriority(*(debugLog->getPriority())))
        {
            // normal case, no need to output log
            return;
        }

        va_list args;
        va_start(args, logPrefix); /* pgr0039 */
        // call to out start log
        debugLog->outputAPLLog(Poco::Message::PRIO_DEBUG, line, methodName, fileName, logPrefix,
                               args); /* pgr0039 */
        va_end(args);
    }
}

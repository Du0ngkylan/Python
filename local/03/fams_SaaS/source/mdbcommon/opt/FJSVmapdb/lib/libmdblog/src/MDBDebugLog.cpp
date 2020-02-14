/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBDebugLog.cpp
 * @brief   Implementation of debug log
 * @author  trunghn:
 * @date    2016/9/12
 */

#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <pwd.h>

#include "MDBDebugLog.h"
#include "MDBLogger.h"
#include "MDBException.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {
    using Passwd = struct passwd;

    //! Counting current index of logging thread
    std::atomic_int MDBDebugLog::threadCount { 0 };

    /*!
     * @brief Default debug log constructor
     *
     * Call default constructor of parent class MDBMLog
     *
     * @param None.
     * @return None.
     */
    MDBDebugLog::MDBDebugLog(void) :
        fileChannel { nullptr }, currentPri { nullptr }
    {

    }

    /*!
     * @brief Debug log constructor with parameter
     *
     * Call parent constructor and, create and open a file channel
     *
     * @param [in]  mdlName module name
     * @param [in]  priority priority of channel
     * @param [in]  logPath directly debug log path
     * @param [in]  storePath store debug log path
     * @param [in]  logSize log file size
     * @return None.
     */
    MDBDebugLog::MDBDebugLog(const std::string &mdlName, const std::string &threadName, const std::atomic_int *priority,
                             const std::string &logPath, const std::string &storePath, unsigned int logSize) :
        fileChannel { nullptr }, currentPri { priority }
    {
        // increase thread count to append to debug log file name
        threadCount++;
        auto debugLogPath = std::string { }; /* pgr0745 */
        debugLogPath.reserve(pathLength);
        debugLogPath = logPath;
        debugLogPath += mdlName;
        debugLogPath += "_";
        debugLogPath += std::to_string(threadCount);

        if (false == threadName.empty())
        {
            debugLogPath += "_";
            debugLogPath += threadName;
        }

        debugLogPath += ".log";

        try
        {
            // Create a file channel
            fileChannel = new MDBFileChannel { logSize, debugLogPath, storePath };
            // Open file channel
            fileChannel->open();
        }
        catch (const std::bad_alloc &exp)
        {
            throw;
        }
        catch (const Poco::FileNotFoundException &exp)
        {
            throw;
        }
        catch (const Poco::Exception &exp)
        {
            throw;
        }
        catch (...)
        {
            throw;
        }

        if (0 == getuid())
        {
            errno = 0;
            auto pwd = Passwd{ nullptr, nullptr, 0, 0, nullptr, nullptr, nullptr};
            auto result = static_cast<Passwd *>(nullptr);

            auto buf = std::array<char, bufferSize> {'\0'};
            auto nameResult = getpwnam_r("mapdb", &pwd, buf.data(), bufferSize, &result);

            if (0 != nameResult)
            {
                auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'};
                auto errBufData = errBuf.data();
                auto errBufSize = errBuf.size();
                auto errorMessage = strerror_r(errno, errBufData, errBufSize);
                throw  MDBException {errorMessage};
            }

            errno = 0;

            if (0 != chown(debugLogPath.c_str(), pwd.pw_uid, pwd.pw_gid))
            {
                auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'};
                auto errBufData = errBuf.data();
                auto errBufSize = errBuf.size();
                auto errorMessage = strerror_r(errno, errBufData, errBufSize);
                throw  MDBException {errorMessage};
            }
        }
    }

    /*!
     * @brief Debug log destructor
     *
     * Close file channel
     *
     * @param None.
     * @return None.
     */
    MDBDebugLog::~MDBDebugLog(void)
    {
        // close file channel
        if (nullptr != fileChannel)
        {
            fileChannel->close();
        }

        threadCount--;

        if (0 >= threadCount)
        {
            static_cast<void>(MDBFileChannel::stopBackupThread());
        }
    }

    /*!
     * @brief Out put debug log
     *
     * create message format and call to output debug log
     *
     * @param [in]  priority log message priority
     * @param [in]  line line of code that call out log
     * @param [in]  methodName function name that call out log
     * @param [in]  fileName file name that call write log
     * @param [in]  msgFormat message format
     * @param [in]  args message list param
     * @return None.
     */
    void MDBDebugLog::outputAPLLog(Poco::Message::Priority priority, int line,
                                   const std::string &methodName, const std::string &fileName,
                                   const std::string &msgFormat, va_list args)
    {
        // Get current time
        auto now = std::chrono::system_clock::now(); /* pgr0745 */
        auto epochDuration = now.time_since_epoch(); /* pgr0745 */
        auto timeInfo = std::chrono::system_clock::to_time_t(now); /* pgr0745 */
        auto micro = (std::chrono::duration_cast<std::chrono::microseconds>(epochDuration).count() % 1000000); /* pgr0745 */
        auto tmpTime = tm { .tm_sec = 0, .tm_min = 0, .tm_hour = 0, .tm_mday = 0, .tm_mon = 0,
                            .tm_year = 0, .tm_wday = 0, .tm_yday = 0, .tm_isdst = 0, .tm_gmtoff = 0, .tm_zone =
                                nullptr }; /* pgr0431 */

        auto logStr = std::array<char, Log::MSG_MAX_LEN> { '\0' }; /* pgr0745 */

        // Format current time
        auto wrPosition = logStr.data(); /* pgr0745 */
        auto wrSize = logStr.size(); /* pgr0745 */
        auto wrNum = std::strftime(wrPosition, wrSize, "%FT%T",
                                   localtime_r(&timeInfo, &tmpTime));

        if (0 == wrNum)
        {
            // system error
            mdb_sys_log_out("F000701", "Log time fail - strftime ");
            return;
        }

        wrPosition += wrNum;
        wrSize -= wrNum;

        auto wrResult = std::snprintf(wrPosition, wrSize, ".%06ld", micro); /* pgr0579 *//* pgr1170 */

        if (0 > wrResult)
        {
            // system error
            mdb_sys_log_out("F000701", "Log time fail - milliseconds");
            return;
        }

        wrPosition += wrResult;
        wrSize -= wrResult;

        wrResult = std::snprintf(wrPosition, wrSize, "%+03ld:%02ld ",
                                 tmpTime.tm_gmtoff / 3600, (tmpTime.tm_gmtoff % 3600) / 60);

        if (0 > wrResult)
        {
            // system error
            mdb_sys_log_out("F000701", "Log time fail - timezone ");
            return;
        }

        wrPosition += wrResult;
        wrSize -= wrResult;

        auto strPri = priorityToString(priority); /* pgr0745 */

        if (true == strPri.empty())
        {
            // could be usage error
            auto errorMessage = std::string { "Error log priority " };
            errorMessage += fileName;
            errorMessage += ":";
            errorMessage += std::to_string(line);
            mdb_sys_log_out("F000701", errorMessage.c_str());
            return;
        }

        auto debugLogPriority = strPri.c_str(); /* pgr0745 */
        auto debugLogFileName = fileName.c_str(); /* pgr0745 */
        auto debugLogMethodName = methodName.c_str(); /* pgr0745 */
        auto processId = getpid(); /* pgr0745 */
        auto threadId = std::hash<std::thread::id>()(std::this_thread::get_id()); /* pgr0745 */
        wrResult = std::snprintf(wrPosition, wrSize, /* pgr0579 *//* pgr1170 */
                                 "[%.10s](%.255s,%d,%.80s) %d [%lu] ", debugLogPriority,
                                 debugLogFileName, line, debugLogMethodName, processId, threadId);

        if (0 > wrResult)
        {
            // could be usage error
            auto errorMessage = std::string { "Log prefix fail " };
            errorMessage += fileName;
            errorMessage += ":";
            errorMessage += std::to_string(line);
            mdb_sys_log_out("F000701", errorMessage.c_str());
            return;
        }

        wrPosition += wrResult;
        wrSize -= wrResult;

        auto debugLogMsgFmt = msgFormat.c_str(); /* pgr0745 */
        // Format log message
        wrResult = std::vsnprintf(wrPosition, wrSize, debugLogMsgFmt, args); /* pgr0579 *//* pgr1170 */

        if (0 > wrResult)
        {
            // could be usage error
            auto errorMessage = std::string { "Error log arguments " };
            errorMessage += fileName;
            errorMessage += ":";
            errorMessage += std::to_string(line);
            mdb_sys_log_out("F000701", errorMessage.c_str());
            return;
        }

        // Create a log message and write log
        auto message = Poco::Message("", logStr.data(), priority); /* pgr0745 */

        try
        {
            fileChannel->log(message);
        }
        catch (const std::exception &e)
        {
            auto errorMessage = std::string { "Output log failed " };
            errorMessage += fileName;
            errorMessage += ":";
            errorMessage += std::to_string(line);
            mdb_sys_log_out("F000701", errorMessage.c_str());
            mdb_sys_log_out("F000702", logStr.data());
            return;
        }
        catch (...)
        {
            auto errorMessage = std::string { "Output log failed " };
            errorMessage += fileName;
            errorMessage += ":";
            errorMessage += std::to_string(line);
            mdb_sys_log_out("F000701", errorMessage.c_str());
            mdb_sys_log_out("F000702", logStr.data());
            return;
        }
    }

    Poco::Message::Priority MDBDebugLog::convertLevelToPriority(int level)
    {
        switch (level)
        {
            case Log::SETTING_LEVEL_0:
                return Poco::Message::PRIO_INFORMATION;

            case Log::SETTING_LEVEL_1:
                return Poco::Message::PRIO_DEBUG;

            case Log::SETTING_LEVEL_2:
                return Poco::Message::PRIO_TRACE;

            default:
                return Poco::Message::PRIO_INFORMATION;
        }
    }

    std::string MDBDebugLog::priorityToString(Poco::Message::Priority pri)
    {
        switch (pri)
        {
            case Poco::Message::PRIO_CRITICAL:
                return "crit";

            case Poco::Message::PRIO_ERROR:
                return "err";

            case Poco::Message::PRIO_WARNING:
                return "warn";

            case Poco::Message::PRIO_INFORMATION:
                return "info";

            case Poco::Message::PRIO_DEBUG:
                return "debug";

            case Poco::Message::PRIO_TRACE:
                return "trace";

            default:
                return "";
        }
    }
}

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBLogger.h
 * @brief   Defination of logger
 * @author  trunghn:
 * @date    2016/9/12
 */

#ifndef LIBMDBLOG_MDBLOGGER_H_
#define LIBMDBLOG_MDBLOGGER_H_

#include "MDBSyslog.h"
#include "MDBDebugLog.h"
#include "MDBDebugLevelMap.h"
#include "Poco/Util/ServerApplication.h"
/*!
 * @def mdb_sys_log_out
 * @brief macro to out syslog
 */
#define mdb_sys_log_out(msgNo, ...) mdbcom::MDBLogger::outputSyslog(msgNo , ##__VA_ARGS__) /* pgr0671 */ /* pgr2216 */

/*!
 * @def mdb_apl_log_critical
 * @brief macro to out put critical level of debug log
 */
#define mdb_apl_log_critical(MSG, ...) mdbcom::MDBLogger::outputDebugLog(Poco::Message::PRIO_CRITICAL, __LINE__, __func__, __MDBM_FILENAME__, MSG, ##__VA_ARGS__) /* pgr0671 */ /* pgr2216 */

/*!
 * @def mdb_apl_log_error
 * @brief macro to out put error level if debug log
 */
#define mdb_apl_log_error(MSG, ...) mdbcom::MDBLogger::outputDebugLog(Poco::Message::PRIO_ERROR, __LINE__, __func__, __MDBM_FILENAME__, MSG, ##__VA_ARGS__) /* pgr0671 */ /* pgr2216 */

/*!
 * @def mdb_apl_log_warn
 * @brief macro to out put warning level of debug log
 */
#define mdb_apl_log_warn(MSG, ...) mdbcom::MDBLogger::outputDebugLog(Poco::Message::PRIO_WARNING, __LINE__, __func__, __MDBM_FILENAME__, MSG, ##__VA_ARGS__) /* pgr0671 */ /* pgr2216 */

/*!
 * @def mdb_apl_log_info
 * @brief macro to out put information level of debug log
 */
#define mdb_apl_log_info(MSG, ...) mdbcom::MDBLogger::outputDebugLog(Poco::Message::PRIO_INFORMATION, __LINE__, __func__, __MDBM_FILENAME__, MSG, ##__VA_ARGS__) /* pgr0671 */ /* pgr2216 */

/*!
 * @def mdb_apl_log_debug
 * @brief macro to out put debug level of debug log
 */
#define mdb_apl_log_debug(MSG, ...) mdbcom::MDBLogger::outputDebugLog(Poco::Message::PRIO_DEBUG, __LINE__, __func__, __MDBM_FILENAME__, MSG, ##__VA_ARGS__) /* pgr0671 */ /* pgr2216 */

/*!
 * @def mdb_apl_log_trace
 * @brief macro to out put debug level of trace log
 */
#define mdb_apl_log_trace(MSG, ...) mdbcom::MDBLogger::outputDebugLog(Poco::Message::PRIO_TRACE, __LINE__, __func__, __MDBM_FILENAME__, MSG, ##__VA_ARGS__) /* pgr0671 */ /* pgr2216 */

/*!
 * @def mdb_apl_log_start
 * @brief macro to out put debug level of start log
 */
#define mdb_apl_log_start() mdbcom::MDBLogger::outputStartEndLog(__LINE__, __func__, __MDBM_FILENAME__, "START ") /* pgr2216 */

/*!
 * @def mdb_apl_log_end
 * @brief macro to out put debug level of end log
 */
#define mdb_apl_log_end() mdbcom::MDBLogger::outputStartEndLog(__LINE__, __func__, __MDBM_FILENAME__, "END ") /* pgr2216 */

namespace mdbcom {
	using Poco::Util::Application;
    /*!
     * @class MDBLogger
     * @brief logger class handles write debug log and syslog
     */
    class MDBLogger {
    public:

        /*!
         * @brief out put syslog
         *
         * Output syslog to rsyslog
         *
         * @param [in]  msgNo message no
         * @param [in]  ...  list of message argument
         * @return None.
         */
        static void outputSyslog(std::string msgNo, ...);

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
        static int initLog(const std::string &moduleName);

        /*!
         * @brief Open debug log
         *
         * Init and open a debug log with name as module name,
         * this function must be call when start each thread
         *
         * @param [in]  threadName Thread name
         *
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static int openDebugLog(const std::string &threadName = "");

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
        static void outputDebugLog(Poco::Message::Priority msgPrio, int line,
                                   const std::string &methodName, const std::string &fileName,
                                   std::string msgFormat, ...);

        /*!
         * @brief Out put debug start/end log
         *
         * @param [in]  line line of code that call out log
         * @param [in]  methodName function name that call out log
         * @param [in]  fileName file name that call write log
         * @param [in]  logPrefix log start/end prefix
         * @return None.
         */
        static void outputStartEndLog(int line, const std::string &methodName,
                                      const std::string &fileName, std::string logPrefix, ...);

        /*!
         * @brief Start rotation thread
         *
         * Start asynchronous thread to execute rotate to storage folder
         *
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static inline int startBackupThread(void) {
            return MDBDebugLog::startBackupThread();
        }

        /*!
         * @brief Stop rotation thread
         *
         * Stop asynchronous thread to execute rotate to storage folder
         *
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static inline int stopBackupThread(void) {
            return MDBDebugLog::stopBackupThread();
        }

        /*!
         * @brief Default constructor
         *
         * Not supported
         *
         * @param None.
         * @return None.
         */
        MDBLogger(void) = delete;

        /*!
         * @brief Copy constructor
         *
         * Not supported
         *
         * @param None.
         * @return None.
         */
        MDBLogger(const MDBLogger &) = delete;

        /*!
         * @brief Copy assignment
         *
         * Not supported
         *
         * @param None.
         * @return None.
         */
        MDBLogger &operator =(const MDBLogger &) = delete; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Default destructor
         *
         * Not supported
         *
         * @param None.
         * @return None.
         */
        ~MDBLogger(void) = delete;

    private:

        //! Constant Maximum log size (in MB)
        static constexpr unsigned int DEFAULT_DEBUG_LOG_SIZE = 4;

        //! Constant for default process name
        static const std::string DEFAULT_PROCESS_NAME;

        //! Constant for debug log configure file path
        static const std::string DEBUG_LOG_CONF_PATH;

        //! Constant for syslog configure file path
        static const std::string SYSLOG_CONF_PATH;

        //! Constant for debug log direct log path
        static const std::string DEFAULT_DEBUG_DIRECT_LOG_PATH;

        //! Constant for debug log store log path
        static const std::string DEFAULT_DEBUG_STORE_LOG_PATH;

        //! Constant for debug log direct path key
        static const std::string DEBUG_LOG_DIRECT_PATH_KEY;

        //! Constant for debug log file size key
        static const std::string DEBUG_LOG_FILE_SIZE_KEY;

        //! Constant for debug log store path key
        static const std::string DEBUG_LOG_STORE_PATH_KEY;

        //! Path to debug log
        static std::string debugLogPath;

        //! Path to storage log folder
        static std::string storePath;

        //! Maximum size for each log file
        static unsigned int debugLogSize;

        //! Init logger flag
        static bool initFlag;

        //! Level map that point to level shared mem
        static MDBDebugLevelMap *levelMap;

        //! Syslog instance
        static std::unique_ptr<MDBSyslog> syslog;

        //! Debug log for each thread
        static thread_local std::unique_ptr<MDBDebugLog> debugLog;

        //! Module name
        static std::string processName;

    };
}

#endif

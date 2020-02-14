/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBConstants.h
 * @brief Definition for constants
 * @author trunghn:
 * @date 2016/9/12
 */

#ifndef LIBMDBUTIL_MDBCONSTANTS_H_
#define LIBMDBUTIL_MDBCONSTANTS_H_

#include <cstring> /* pgr0723 */  /* cstring include in Poco/Any.h so we add this comment */
#include <string>
#include <memory>

#define __MDBM_FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

namespace mdbcom {

    class Result { /* pgr1232 */
    public:
        /** Common error ID */
        // Normal case
        static constexpr int OK = 0; /* pgr0746 */
        // Common error
        static constexpr int NG = -1; /* pgr0746 */
        // Error by external like: connection error
        static constexpr int EXT_NG = -2; /* pgr0746 */
        // Return because of User terminating
        static constexpr int TERM_BY_USER = -3; /* pgr0746 */
        /** Log error*/
        /** Error in reading log configuration file */
        static constexpr int NG_CONFIGURATION = -4;
        /** Error in creating syslog*/
        static constexpr int NG_SYSLOG = -5;
        /** Error in loading shared memory for debug log */
        static constexpr int NG_SHARED_MEM = -6;

        /** IPC error ID */
        // Queue full error
        static constexpr int QUEUE_FULL = -10; /* pgr0746 */
        // Retry IPC failed error
        static constexpr int RETRY_IPC_FAILED = -11; /* pgr0746 */

        /** Configure error ID */
        // Open configure file fail
        static constexpr int EMPTY_KEY = -20; /* pgr0746 */
        // Empty input key
        static constexpr int UNKNOWN_EXCEPTION = -21; /* pgr0746 */

        /** Init application error ID */
        // Error in init log
        static constexpr int INIT_LOG_FAILED = -30; /* pgr0746 */
        // Error in open debug log
        static constexpr int OPEN_DEBUG_LOG_FAILED = -31; /* pgr0746 */
        // Error in register signal handler
        static constexpr int START_SIGNAL_HANDLER_FAILED = -32; /* pgr0746 */
        // Error in start rotating log
        static constexpr int START_ROTATE_LOG_FAILED = -33; /* pgr0746 */
    };

    class ExitCode {  /* pgr1232 */
    public:
        /** Application exit code **/
        // Exit code normal.
        static constexpr int OK = 0; /* pgr0746 */
        // Exit code error
        static constexpr int NG = 1; /* pgr0746 */
    };

    class Json {  /* pgr1232 */
    public:
        // define for json result
        static constexpr int OK = 0; /* pgr0746 */
        static constexpr int NG = -1; /* pgr0746 */
        //! JSON key "MdbCommon"
        static constexpr const char *const COMMON_KEY = "MdbCommon";/* pgr0746 */
        //! JSON key "ver"
        static constexpr const char *const VER_KEY = "ver"; /* pgr0746 */
    };

    class Signal {  /* pgr1232 */
    public:
        // define for signal
        static constexpr int OK = 0; /* pgr0746 */
        static constexpr int IGNORE = 2000; /* pgr0746 */
        static constexpr int ERROR = 2001; /* pgr0746 */
        static constexpr int ERROR_SIGEMPTYSET = 2002; /* pgr0746 */
        static constexpr int ERROR_SIGADDSET = 2003; /* pgr0746 */
        static constexpr int ERROR_PTHREAD_SIGMASK = 2004; /* pgr0746 */
        static constexpr int OK_THREAD_STARTED = 2005; /* pgr0746 */
        static constexpr int ERROR_SIGNAL_INVALID = 2006; /* pgr0746 */
        static constexpr int ERROR_HANDLER_METHOD_INVALID = 2007; /* pgr0746 */
        static constexpr int OK_NO_SIGNAL = 2008; /* pgr0746 */
        static constexpr int ERROR_THREAD_JOIN_FAILED = 2009; /* pgr0746 */
        static constexpr int ERROR_THREAD_START_FAILED = 2010; /* pgr0746 */
    };

    class Ipc {  /* pgr1232 */
    public:
        // Maximum retry time of IPC
        static constexpr unsigned int MAX_RETRY_TIMES = 2; /* pgr0746 */
        // Wait time(1 second) between 2 times of retry
        static constexpr unsigned int RETRY_WAIT_TIME_SECOND = 1; /* pgr0746 */
        //! Log shared memory name
        static constexpr const char *const LOG_SHARED_NAME = "LOG_SETTING";/* pgr0746 */
    };

    class Log { /* pgr1232 */
    public:
        // 0: critical, error, warning
        static constexpr int SETTING_LEVEL_0 = 0; /* pgr0746 */
        // 1: critical, error, warning, infomation, debug
        static constexpr int SETTING_LEVEL_1 = 1; /* pgr0746 */
        // 2: critical, error, warning, infomation, debug, trace
        static constexpr int SETTING_LEVEL_2 = 2; /* pgr0746 */
        // Number of debug log function (module) in config file
        static constexpr size_t CONF_NUM = 256; /* pgr0746 */
        //! Max length of log content
        static constexpr size_t CONTENT_MAX_LEN = 4 * 1024; /* pgr0746 */
        //! Length of log message prefix
        static constexpr size_t MSG_PREFIX_LEN = 256; /* pgr0746 */
        //! Max length of log content
        static constexpr size_t CONF_FUNC_LENGTH = 128; /* pgr0746 */
        //! Maximum length of log message
        static constexpr size_t MSG_MAX_LEN = MSG_PREFIX_LEN + CONTENT_MAX_LEN; /* pgr0746 */
        //! Empty string log argument
        static constexpr const char *const ARG_EMPTY_STR = "-";/* pgr0746 */
    };

    class Restful {  /* pgr1232 */
    public:
        // Define RESTful API version
        static constexpr int API_VER_1 = 1; /* pgr0746 */
    };

    class Database {  /* pgr1232 */
    public:
        // Host string definition
        static constexpr const char *const LOCAL_HOST_STRING = "localhost";/* pgr0746 */
    };

    class Http {  /* pgr1232 */
    public:
        // Application type definition
        static constexpr const char *const CONTENT_TYPE_JSON = "application/json; charset=UTF-8";/* pgr0746 */

    };

    class Process {  /* pgr1232 */
    public:
        //! Path to get current process name
        static constexpr const char *const NAME_PATH = "/proc/self/comm"; /* pgr0746 */
    };

    class Buffer {  /* pgr1232 */
    public:
        //! Buffer lenght for string error
        static constexpr size_t STR_ERROR_BUFFER_LENGTH = 1024; /* pgr0746 */
    };

    /*!
     * @enum ShmModes
     * @brief define shared memory access mode
     */
    enum class ShmModes
    {
        CREATE, READ, WRITE
    };

    enum class ConnectionType
        : int
    {
        NONE = 0, DYNAMIC, MAX_VALUE
    };
}

#endif

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBDebugLog.h
 * @brief   Defination of debug log
 * @author  trunghn:
 * @date    2016/9/12
 */
#ifndef LIBMDBLOG_MDBDEBUGLOG_H
#define LIBMDBLOG_MDBDEBUGLOG_H

#include <atomic>
#include <unordered_map>

#include <Poco/AutoPtr.h>
#include <Poco/Message.h>

#include "MDBFileChannel.h"
#include "MDBConstants.h"

namespace mdbcom {
    /*! @class MDBDebugLog
     *  @brief Debug log class handles output debug log
     */
    class MDBDebugLog {
    public:
        /*!
         * @brief convertLevelToPriority
         *
         * @param [in] level Log level.
         * @return
         */
        static Poco::Message::Priority convertLevelToPriority(int level);

        /*!
         * @brief Start rotation thread
         *
         * Start asynchronous thread to execute rotate to storage folder
         *
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static inline int startBackupThread(void) {
            return MDBFileChannel::startBackupThread();
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
            return MDBFileChannel::stopBackupThread();
        }

        /*!
         * @brief Default debug log constructor
         *
         * Call default constructor of parent class MDBMLog
         *
         * @param None.
         * @return None.
         */
        MDBDebugLog(void);

        /*!
         * @brief Debug log constructor with parameter
         *
         * Call parent constructor and, create and open a file channel
         *
         * @param [in]  mdlName module name
         * @param [in]  threadName thread name
         * @param [in]  priority priority of channel
         * @param [in]  logPath directly debug log path
         * @param [in]  storePath store debug log path
         * @param [in]  logSize log file size
         * @return None.
         */
        MDBDebugLog(const std::string &mdlName, const std::string &threadName, const std::atomic_int *priority, const std::string &logPath,
                    const std::string &storePath, unsigned int logSize);

        /*!
         * @brief Copy constructor
         *
         * Not support
         *
         * @param None.
         * @return None.
         */
        MDBDebugLog(const MDBDebugLog &) = delete; /* pgr2222 */ /* pgr0571 */

        /*!
         * @brief Assignment operator
         *
         * Not support
         *
         * @param None.
         * @return None.
         */
        MDBDebugLog &operator =(const MDBDebugLog &) = delete;/* pgr0055 */ /* pgr0309 */ /* pgr2223 */

        /*!
         * @brief Debug log destructor
         *
         * Close file channel
         *
         * @param None.
         * @return None.
         */
        ~MDBDebugLog(void);

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
        void outputAPLLog(Poco::Message::Priority priority, int line, const std::string &methodName,
                          const std::string &fileName, const std::string &msgFormat, va_list args);

        /*!
         * @brief set priority
         *
         * Set priorityfor log object
         *
         * @param [in]  priority priority of log object
         * @return None.
         */
        inline void setPriority(const std::atomic_int *priority) {
            this->currentPri = priority;
        }

        /*!
         * @brief get priority
         *
         * get priority for log object
         *
         * @param [in] None.
         * @return current priority
         */
        inline const std::atomic_int *getPriority(void) const {
            return this->currentPri;
        }

        /*!
         * @brief get thread count
         *
         * @param [in] None.
         * @return current thread count
         */
        static inline int getThreadCount(void) {
            int ret = threadCount;
            return ret;
        }

    private:
        //! Buffer size
        static constexpr size_t bufferSize = 4096;

        //! path length
        static constexpr int pathLength = 256;

        //! Counting current index of logging thread
        static std::atomic_int threadCount;

        //! Channel to output log to file
        Poco::AutoPtr<MDBFileChannel> fileChannel;

        //! Current priority of log
        const std::atomic_int *currentPri;

        /*!
         * @brief Convert priority to string value
         *
         * @param [in] pri Priority value
         * @return Priority string
         */
        std::string priorityToString(Poco::Message::Priority pri);

    };
}

#endif /* End LIBMDBLOG_MDBDEBUGLOG_H */

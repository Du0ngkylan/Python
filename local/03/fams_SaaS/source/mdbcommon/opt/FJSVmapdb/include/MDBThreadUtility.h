/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBThreadUtility.h
 * @brief       Header file for define MDBThreadUtility functions.
 * @author      cuongadp: Support thread operation.
 * @date        2016/12/07
 */

#ifndef LIBMDBTHREAD_MDBTHREADUTILITY_H_
#define LIBMDBTHREAD_MDBTHREADUTILITY_H_

#include <thread>

#include "MDBThreadDatabaseConfig.h"
#include "MDBThreadStartArguments.h"

namespace mdbcom {
    class MDBThreadUtility { /* pgr2205 */
    public:
        //! Define function pointer for handle signal.
        using HandleSignalFunction = std::function<int(int, const void *)>;
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBThreadUtility(void) = delete;

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBThreadUtility(void) = delete;

        /*!
         * @brief Copy Constructor.
         * @param None.
         * @return None.
         */
        MDBThreadUtility(const MDBThreadUtility &) = delete;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return None.
         */
        MDBThreadUtility &operator=(const MDBThreadUtility &) = delete; /* pgr0309 */ /* pgr0055 */

        /*!
         * @brief Move Constructor.
         * @param None.
         * @return None.
         */
        MDBThreadUtility(MDBThreadUtility &&) = delete;

        /*!
         * @brief move assignment.
         * @param None.
         * @return None.
         */
        MDBThreadUtility &operator=(MDBThreadUtility &&) = delete; /* pgr0309 */ /* pgr0055 */

        /**
         * @brief Start DB worker without detaching that thread
         *
         * Start thread with DB connection
         *
         * @param [in] threadName  Name of module (function)
         * @param [in] threadFunction  Worker function to be executed
         * @param [in] databaseNum Number of database to be used in thread
         * @param [in] databaseInfo Database information to be used in thread
         * @param [in] arguments Arguments passing to user function
         * @param [out] threadObj Thread instance representing execution thread
         * @retval Result::NG Success
         * @retval Result::OK Failure
         */
        static inline int createDBWorkUnit(const std::string &threadName,
                                           ThreadFunction threadFunction,
                                           const std::vector<MDBThreadDatabaseConfig> &databaseInfo,
                                           void *arguments, std::thread &threadObj) {
            return startThread(threadName, threadFunction, databaseInfo, arguments, false, threadObj);
        }

        /**
         * @brief Start DB worker and detach it
         *
         * Start thread with DB connection
         *
         * @param [in] threadName  Name of module (function)
         * @param [in] threadFunction  Worker function to be executed
         * @param [in] databaseNum Number of database to be used in thread
         * @param [in] databaseInfo Database information to be used in thread
         * @param [in] arguments Arguments passing to user function
         * @param [out] threadObj Thread instance representing execution thread
         * @retval Result::NG Success
         * @retval Result::OK Failure
         */
        static inline int createDetachDBWorkUnit(const std::string &threadName,
                ThreadFunction threadFunction,
                const std::vector<MDBThreadDatabaseConfig> &databaseInfo,
                void *arguments, std::thread &threadObj) {
            return startThread(threadName, threadFunction, databaseInfo, arguments, true, threadObj);
        }

        /**
         * @brief Start thread worker without detaching that thread
         *
         * Start thread with no DB connection
         *
         * @param [in] threadName  Name of module (function)
         * @param [in] threadFunction  Worker function to be executed
         * @param [in] arguments Arguments passing to user function
         * @param [out] threadObj Thread instance representing execution thread
         * @retval Result::NG Success
         * @retval Result::OK Failure
         */
        static inline int createWorkUnit(const std::string &threadName,
                                         ThreadFunction threadFunction,
                                         void *arguments, std::thread &threadObj) {
            auto databaseInfo = std::vector<MDBThreadDatabaseConfig> {}; /* pgr0745 */
            return startThread(threadName, threadFunction, databaseInfo, arguments, false, threadObj);
        }

        /**
         * @brief Start thread worker and detach it
         *
         * Start thread with no DB connection
         *
         * @param [in] threadName  Name of module (function)
         * @param [in] threadFunction  Worker function to be executed
         * @param [in] arguments Arguments passing to user function
         * @param [out] threadObj Thread instance representing execution thread
         * @retval Result::NG Success
         * @retval Result::OK Failure
         */
        static inline int createDetachWorkUnit(const std::string &threadName,
                                               ThreadFunction threadFunction,
                                               void *arguments, std::thread &threadObj) {
            auto databaseInfo = std::vector<MDBThreadDatabaseConfig> {}; /* pgr0745 */
            return startThread(threadName, threadFunction, databaseInfo, arguments, true, threadObj);
        }
    private:
        /**
         * @brief Thread's function for startThread
         *
         * processing additional step before executing actual work:
         *  - init log
         *  - init db connections
         *
         * @param [in] args Thread start argument
         * @retval nullptr Always return nullptr
         */
        static void *threadFunctionWrapper(void *args);

        /**
         * @brief Start DB worker
         *
         * Start thread with normal DB connection
         *
         * @param [in] threadName  Name of module (function)
         * @param [in] threadFunction  Worker function to be executed
         * @param [in] databaseNum Number of database to be used in threa
         * @param [in] databaseInfo Database information to be used in thread
         * @param [in] arguments Arguments passing to user function
         * @param [in] isDetach Detach the created thread if true
         * @param [out] threadObj Thread instance representing execution thread
         * @retval Result::NG Success
         * @retval Result::OK Failure
         */
        static int startThread(const std::string &threadName,
                               ThreadFunction threadFunction,
                               const std::vector<MDBThreadDatabaseConfig> &databaseInfo,
                               void *arguments,
                               bool isDetach,
                               std::thread &threadObj);
    };
}
#endif /* LIBMDBTHREAD_MDBTHREADUTILITY_H_ */

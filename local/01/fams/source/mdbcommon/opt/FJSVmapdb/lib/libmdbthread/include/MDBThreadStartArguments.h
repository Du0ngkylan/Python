/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBThreadStartArguments.h
 * @brief       Header file for define MDBThreadStartArguments information.
 * @author      cuongadp: Support thread wrapper.
 * @date        2016/10/26
 */
#ifndef LIBMDBTHREAD_MDBTHREADSTARTARGUMENTS_H_
#define LIBMDBTHREAD_MDBTHREADSTARTARGUMENTS_H_

#include <future>

#include "MDBTypes.h"
#include "MDBThreadDatabaseConfig.h"

namespace mdbcom {

    /*! @class MDBThreadStartArguments
     *  @brief Contain Thread's start up arguments
     */
    class MDBThreadStartArguments {
    public:
        /**
         * @brief Default Constructor
         *
         * @param [in] name Module name
         * @param [in] func Function executed in thread
         * @param [in] dbNum Number of database used in thread
         * @param [in] dbInfo Array of database information used in thread
         * @param [in] arg Executing arguments for thread function
         * @return  None.
         */
        MDBThreadStartArguments(const std::string &name,
                                const ThreadFunction func,
                                const std::vector<MDBThreadDatabaseConfig> &dbInfo,
                                void *arg);

        /**
         * @brief Assignment argument
         *
         * Not support
         *
         * @param  None.
         * @return  None.
         */
        MDBThreadStartArguments(const MDBThreadStartArguments &) = delete; /* pgr2222 */ /* pgr0571 */

        /*!
         * @brief Copy assignment
         *
         * This operator is not supported.
         *
         * @param  None.
         * @return  None.
         */
        MDBThreadStartArguments &operator =(const MDBThreadStartArguments &) = delete; /* pgr0055 */ /* pgr2223 */ /* pgr0309 */ /* pgr2252 */

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~MDBThreadStartArguments(void) = default;

        /**
         * @brief Getter for module name
         * @param None
         * @return Module Name
         */
        inline const std::string &getThreadName(void) const {
            return threadName;
        }

        /**
         * @brief Getter for number of database
         * @param None
         * @return databaseNum
         */
        inline int getDatabaseNum(void) const {
            return databaseInfo.size();
        }

        /**
         * @brief Getter for database information
         * @param None
         * @return Pointer to database information
         */
        inline const std::vector<MDBThreadDatabaseConfig> &getDatabaseInfo(void) const {
            return databaseInfo;
        }

        /**
         * @brief Getter for thread function
         * @param None
         * @return thread function
         */
        inline ThreadFunction getThreadFunction(void) const {
            return function;
        }

        /**
         * @brief Getter for thread function's arguments
         * @param None
         * @return Arguments of thread function
         */
        inline const void *getArguments(void) const {
            return arguments;
        }

        /**
         * @brief Get result to know if thread is already started
         * @param None
         * @return Future object to get thread result
         */
        inline std::future<int> getThreadResult(void) const {
            return startResult.get_future();
        }

        /**
         * @brief Set result for thread
         * @param [in] code Result code
         * @return None
         */
        inline void setThreadResult(int code) {
            startResult.set_value(code);
        }

    private:
        //! function(module) name
        std::string threadName;

        //! Number of DB connection
        const std::vector<MDBThreadDatabaseConfig> databaseInfo;

        //! Executing function for thread
        ThreadFunction function;

        //! Executing arguments for thread function
        void *arguments;

        //! Starting result, ensure user's function can be called.
        mutable std::promise<int> startResult;
    };
}

#endif

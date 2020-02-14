/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBThreadStartArguments.cpp
 * @brief       Source file for implementing MDBThreadStartArguments class.
 * @author      cuongadp: create it for support thread wrapper
 * @date        2016/10/27
 */

#include "MDBThreadStartArguments.h"

namespace mdbcom {

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
    MDBThreadStartArguments::MDBThreadStartArguments(const std::string &name,
            const ThreadFunction func, const std::vector<MDBThreadDatabaseConfig> &dbInfo, void *arg) :
        threadName { name }, databaseInfo(dbInfo), function { func },  arguments
    {
        arg }, startResult { }
    {

    }
}

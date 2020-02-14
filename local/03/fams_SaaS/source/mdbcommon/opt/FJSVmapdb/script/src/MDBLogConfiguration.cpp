/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBLogConfiguration.cpp
 * @brief Implementation for Initializing level for debug log in shared memory
 * @author trunghn:
 * @date 2016/9/12
 */

#include <iostream>
#include <fstream>

#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <Poco/String.h>

#include "MDBLogConfiguration.h"
#include "MDBLogger.h"
#include "MDBConstants.h"
#include "MDBSharedMemoryUtility.h"
#include "MDBDebugLevelMap.h"

namespace mdbcom {
    /*!
     * @brief Load configuration file
     *
     * Load debug configuration file to shared memory
     *
     * @param [in] filepath Path to configuration file
     * @return result
     * @retval Result::OK success
     * @retval Result::NG failure
     */
    int MDBLogConfiguration::loadConfigFile(const std::string &filepath)
    {
        auto ret = Result::OK; /* pgr0745 */

        auto shmMode = ShmModes::CREATE; /* pgr0745 */
        auto logConfiguration = reinterpret_cast<MDBDebugLevelMap *>( /* pgr0745 */
                                    MDBSharedMemoryUtility::getShm(Ipc::LOG_SHARED_NAME,
                                            sizeof(MDBDebugLevelMap), shmMode));

        if (nullptr == logConfiguration)
        {
            ret = Result::NG;
        }
        else
        {
            auto &lock = logConfiguration->lock; /* pgr0745 */
            memset(&lock, 0x00, sizeof(MDBSharedMemoryLock)); /* pgr0579 *//* pgr1170 */

            std::ifstream file_conf {filepath};

            if (false == file_conf.is_open())
            {
                ret = Result::NG;
            }
            else
            {
                auto &map = logConfiguration->data; /* pgr0745 */

                auto line = std::string {}; /* pgr0745 */
                auto key = std::string {}; /* pgr0745 */
                auto value = std::string {}; /* pgr0745 */
                auto count = 0; /* pgr0745 */

                while (std::getline(file_conf, line))
                {
                    if (true == line.empty())
                    {
                        continue;
                    }

                    if ('#' == line[line.find_first_not_of(" \t")])
                    {
                        continue;
                    }

                    key = Poco::trim(line.substr(0, line.find("=")));
                    value = Poco::trim(line.substr(line.find("=") + 1));
                    auto functionData = map[count].function.data(); /* pgr0745 */
                    auto keyCstr = key.c_str(); /* pgr0745 */
                    auto functionSize = map[count].function.size(); /* pgr0745 */

                    strncpy(functionData, keyCstr, functionSize); /* pgr0579 *//* pgr1170 */

                    try
                    {
                        map[count].level = std::stoi(value);
                    }
                    catch (const std::out_of_range &)
                    {
                        ret = Result::NG;
                    }
                    catch (const std::invalid_argument &)
                    {
                        ret = Result::NG;
                    }
                    catch (...)
                    {
                        ret = Result::NG;
                    }

                    count++;
                }

                // init shared atribute for mutex
                if (Result::OK == ret)
                {
                    logConfiguration->count = count;

                    if (0 != pthread_mutexattr_init(&lock.sharedMtxAttr))
                    {
                        ret = Result::NG;
                    }
                    else if (0 != pthread_mutexattr_setpshared(&lock.sharedMtxAttr,
                             PTHREAD_PROCESS_SHARED))
                    {
                        static_cast<void>(pthread_mutexattr_destroy(&lock.sharedMtxAttr));
                        ret = Result::NG;
                    }
                    else if (0 != pthread_mutex_init(&lock.sharedMtx, &lock.sharedMtxAttr))
                    {
                        ret = Result::NG;
                    }
                    else
                    {
                        // success
                    }
                }
            }
        }

        return ret;
    }

    /*!
     * @brief Set log level
     *
     * Set debug log level for function
     *
     * @param [in] function Function name (module/thread name)
     * @param [in/out] value Debug log level, incase failure error code is set.
     * @return result
     * @retval Result::OK success
     * @retval Result::NG failure
     */
    int MDBLogConfiguration::setLogLevel(const std::string &function, int &value)
    {
        mdb_apl_log_start();

        auto shmMode = ShmModes::WRITE; /* pgr0745 */
        auto logConfiguration = reinterpret_cast<MDBDebugLevelMap *>( /* pgr0745 */
                                    MDBSharedMemoryUtility::getShm(Ipc::LOG_SHARED_NAME,
                                            sizeof(MDBDebugLevelMap), shmMode));

        if (nullptr == logConfiguration)
        {
            mdb_apl_log_critical("Can not get share memory");
            mdb_apl_log_end();
            value = MDBCommandResult::ERROR_EXEC;
            return Result::NG;
        }

        if ((mdbcom::Log::SETTING_LEVEL_0 > value) || (mdbcom::Log::SETTING_LEVEL_2 < value))
        {
            mdb_apl_log_end();
            value = MDBCommandResult::ERROR_LEVEL;
            return Result::NG;
        }

        auto &lock = (logConfiguration->lock).sharedMtx; /* pgr0745 */

        if (0 != pthread_mutex_trylock(&lock))
        {
            mdb_apl_log_end();
            value = MDBCommandResult::ERROR_DOUBLE_EXEC;
            return Result::NG;
        }

        auto ret = Result::NG; /* pgr0745 */
        auto &map = logConfiguration->data; /* pgr0745 */

        if (true == function.empty())
        {
            ret = Result::OK;

            for (auto i = 0; i < logConfiguration->count ; i++) /* pgr0745 */ /* pgr0692 */
            {
                map[i].level = value;
            }

            mdb_apl_log_info("New debug log level for all: %d", value);
        }
        else
        {
            for (auto i = 0; i < logConfiguration->count ; i++) /* pgr0745 */ /* pgr0692 */
            {
                if (function == map[i].function.data())
                {
                    map[i].level = value;
                    auto strFunction = map[i].function.data(); /* pgr0745 */
                    mdb_apl_log_info("New debug log level for <%s>: %d", strFunction, value);
                    ret = Result::OK;
                    break;
                }
            }

            if (Result::OK != ret)
            {
                value = MDBCommandResult::ERROR_NOTFOUND;
            }
        }

        if (0 != pthread_mutex_unlock(&lock))
        {
            mdb_apl_log_warn("Can't unlock shared memory ");
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Get log level
     *
     * Get debug log level by function
     *
     * @param [in] function Function name (module/thread name)
     * @param [out] level Debug log level
     * @param [out] errorCode Error code
     * @return result
     * @retval Result::OK success
     * @retval Result::NG failure
     */
    int MDBLogConfiguration::showLogLevel(const std::string &function, std::vector<MDBDebugLevelEntry *> &level, int &errorCode)
    {
        mdb_apl_log_start();

        auto shmMode = ShmModes::READ; /* pgr0745 */
        auto logConfiguration = reinterpret_cast<MDBDebugLevelMap *>( /* pgr0745 */
                                    MDBSharedMemoryUtility::getShm(Ipc::LOG_SHARED_NAME,
                                            sizeof(MDBDebugLevelMap), shmMode));

        if (nullptr == logConfiguration)
        {
            mdb_apl_log_critical("Can not get share memory");
            mdb_apl_log_end();
            errorCode = MDBCommandResult::ERROR_EXEC;
            return Result::NG;
        }

        auto lock = (logConfiguration->lock).sharedMtx; /* pgr0745 */

        if (0 != pthread_mutex_trylock(&lock))
        {
            mdb_apl_log_end();
            errorCode = MDBCommandResult::ERROR_DOUBLE_EXEC;
            return Result::NG;
        }

        auto &map = logConfiguration->data; /* pgr0745 */
        auto ret = Result::NG; /* pgr0745 */

        if (true == function.empty())
        {
            level.reserve(logConfiguration->count);

            for (auto i = 0; i < logConfiguration->count; i++)  /* pgr0745 */ /* pgr0692 */
            {
                level.push_back(&map[i]);
            }

            ret = Result::OK;
        }
        else
        {
            for (auto &entry : map)  /* pgr0745 */
            {
                if (function == entry.function.data())
                {
                    level.push_back(&entry);
                    ret = Result::OK;
                    break;
                }
            }

            if (Result::OK != ret)
            {
                errorCode = MDBCommandResult::ERROR_NOTFOUND;
            }
        }

        if (0 != pthread_mutex_unlock(&lock))
        {
            mdb_apl_log_warn("Can't unlock shared memory ");
        }

        mdb_apl_log_end();
        return ret;
    }
}

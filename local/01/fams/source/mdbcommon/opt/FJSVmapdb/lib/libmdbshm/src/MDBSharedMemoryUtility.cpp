/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBSharedMemoryUtility.cpp
 * @brief       Source code for defined shared memory utility functions.
 * @author      trunghn
 * @date        2016/10/04
 */

#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>

#include "MDBSharedMemoryUtility.h"
#include "MDBCommonUtility.h"
#include "MDBException.h"
#include "MDBLogger.h"

namespace mdbcom {

    /*!
     * @brief  getShmFlagFromMode
     *  This function get open shm flag and mmap flag from mode
     *
     * @param [in] mode access shared memory mode
     * @param [out] shmFlag open shared memory flag
     * @param [out] mapFlag mmap shared memory flag
     * @return None.
     */
    inline void MDBSharedMemoryUtility::getShmFlagFromMode(ShmModes mode, int &shmFlag,
            int &mapFlag)
    {

        if (ShmModes::CREATE == mode)
        {
            shmFlag = O_CREAT | O_RDWR | O_EXCL;
            mapFlag = PROT_READ | PROT_WRITE;
        }
        else if (ShmModes::WRITE == mode)
        {
            shmFlag = O_RDWR;
            mapFlag = PROT_READ | PROT_WRITE;
        }
        else
        {
            shmFlag = O_RDONLY;
            mapFlag = PROT_READ;
        }

    }

    /*!
     * @brief  truncateShm
     *  This function truncate shared memory
     *
     * @param [in] fileDescriptor file descriptor
     * @param [in] size shared memory size
     * @param [in] sharedName shared memory name
     *
     * @retval Result::OK truncate success
     * @retval Result::NG truncate failed
     */
    int MDBSharedMemoryUtility::truncateShm(int fileDescriptor, size_t size,
                                            const std::string &sharedName)
    {
        mdb_apl_log_start();

        errno = 0;

        // Truncate shared memory by specified length
        auto ret = ftruncate(fileDescriptor, size); /* pgr0745 */

        if (0 > ret)
        {
            // C++ Improvement: 1.4.1
            auto errnoTmp = errno; /* pgr0745 */
            // Error string buffer
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_critical("ftruncate ret = [%d], errno = [%d], strerror = [%s]", ret,
                                 errnoTmp, errMsg);

            errno = 0;

            if (0 > shm_unlink(sharedName.c_str()))
            {
                errnoTmp = errno;
                errMsg = strerror_r(errno, errBufData, errBufSize);
                mdb_apl_log_critical("shm_unlink() failed, errno = [%d], strerror = [%s]",
                                     errnoTmp, errMsg);
            }

            ret = Result::NG;
        }
        else
        {
            ret = Result::OK;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief  statShm
     *  This function get size of shared memory
     *
     * @param [in] fileDescriptor file descriptor
     *
     * @retval size share memory size
     * @retval Result::NG cannot get size
     */
    size_t MDBSharedMemoryUtility::statShm(int fileDescriptor)
    {
        mdb_apl_log_start();
        // Return shm size
        auto stSize = size_t {}; /* pgr0745 */
        // Shm status
        using Stat = struct stat;
        auto sb = Stat {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /* pgr0745 */
        errno = 0;
        // Get status of shared memory
        auto ret = fstat(fileDescriptor, &sb); /* pgr0745 */

        if (0 > ret)
        {
            // C++ Improvement: 1.4.1
            // Error string buffer
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errnoTmp = errno; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */
            mdb_apl_log_critical("fstat ret = [%d], errno = [%d], strerror = [%s]", ret, errnoTmp,
                                 errMsg);
            stSize = 0;
        }
        else
        {
            stSize = sb.st_size;
        }

        mdb_apl_log_debug("st_size = [%d]", stSize);
        mdb_apl_log_end();

        return stSize;
    }

    /*!
    * @brief  openShm
    *  This function open shared memory
    *
    * @param [in] sharedName shared memory name
    * @param [in] flag open flag
    * @return fileDescriptor
    */
    int MDBSharedMemoryUtility::openShm(const std::string &sharedName, int flag)
    {
        mdb_apl_log_start();

        errno = 0;
        // Open shared memory
        auto fileDescriptor = shm_open(sharedName.c_str(), flag, S_IRUSR | S_IWUSR); /* pgr0745 */ // unsafe-thread: confirmed OK.

        if (0 > fileDescriptor)
        {

            if (EEXIST == errno)
            {
                fileDescriptor = -2;
            }

            // Error string buffer
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errnoTmp = errno; /* pgr0745 */
            // Error message
            auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            if (-2 == fileDescriptor)
            {
                // File exist, normal case
                mdb_apl_log_debug("shm_open fileDescriptor = [%d], errno = [%d], strerror = [%s]",
                                  fileDescriptor, errnoTmp, errMsg);
            }
            else
            {
                mdb_apl_log_critical("shm_open fileDescriptor = [%d], errno = [%d], strerror = [%s]",
                                     fileDescriptor, errnoTmp, errMsg);
            }

        }

        mdb_apl_log_end();
        return fileDescriptor;
    }

    /*!
     * @brief  mapShm
     *  This map shared memory
     *
     * @param [in] sharedSize size shared memory size
     * @param [in] mapFlag map flag
     * @param [in] fileDescriptor file desciptor
     * @return  address of shm if normal or nullptr if abnormal
     */
    void *MDBSharedMemoryUtility::mapShm(size_t sharedSize, int mapFlag, int fileDescriptor)
    {
        mdb_apl_log_start();

        // Retry mmap counter
        auto retryCount = 0u; /* pgr0745 */
        // Shared address
        void *sharedAddress {nullptr};

        do
        {

            errno = 0;
            // Shared region start from address of mutex + (size of mutex + size of shared object)
            sharedAddress = mmap(nullptr, sharedSize, mapFlag, MAP_SHARED, fileDescriptor, 0);
            // Error No temp
            auto errorNo = errno; /* pgr0745 */

            if (MAP_FAILED == sharedAddress)   /* pgr0203 */
            {
                // Error string buffer
                auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
                auto errBufData = errBuf.data(); /* pgr0745 */
                auto errBufSize = errBuf.size(); /* pgr0745 */
                auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */
                mdb_apl_log_debug("mmap errno = [%d], strerror = [%s], retryCount = [%d]", errorNo,
                                  errMsg, retryCount);
            }

            // If the first call mmap failed then sleep and retry
            if ((MAP_FAILED == sharedAddress) && (EINVAL == errorNo))   /* pgr0203 */
            {
                if (Result::OK
                        != MDBCommonUtility::sleepNano(Ipc::RETRY_WAIT_TIME_SECOND, 0))
                {
                    // in case sleep failed then do nothing
                    mdb_apl_log_warn("sleepNano failed");
                }

                // Get shared memory size again
                sharedSize = statShm(fileDescriptor);

            }
            else
            {
                break;
            }

            ++retryCount;
            // Retry 2 times (total 3 times) if cannot mmap shared memory
        }
        while (Ipc::MAX_RETRY_TIMES >= retryCount);   /* pgr0697 */

        if (MAP_FAILED == sharedAddress)   /* pgr0203 */
        {
            sharedAddress = nullptr;
            mdb_apl_log_critical("mmap failed!");
        }

        mdb_apl_log_end();
        return sharedAddress;
    }

    /*!
     * @brief  get shared memory
     *
     * This function create shared memory if it not exist or open shared memory if
     * it already exist
     *
     * @param [in] sharedName shared name
     * @param [in] size shared memory size
     * @param [in,out] mode access mode
     * @return address of shm if normal or nullptr if abnormal
     *
     */
    void *MDBSharedMemoryUtility::getShm(const std::string &sharedName, size_t size, ShmModes &mode)
    {
        mdb_apl_log_start();
        auto sharedNameTmp = sharedName.c_str(); /* pgr0745 */
        mdb_apl_log_debug("in sharedName = [%s], size = [%d], mode = [%d]", sharedNameTmp, size,
                          mode);

        if (true == sharedName.empty())
        {
            mdb_apl_log_critical("sharedName is empty.");
            mdb_sys_log_out("F000801");
            mdb_apl_log_end();
            return nullptr;
        }

        // C++ Improvement: 2.2.1
        // Return value
        auto ret = 0; /* pgr0745 */
        // Create flag
        auto createFlag = false; /* pgr0745 */
        // Open shm flag
        auto flag = 0; /* pgr0745 */
        // Mmap flag
        auto mapFlag = 0; /* pgr0745 */
        // File descriptor
        auto fileDescriptor = -1; /* pgr0745 */
        // Shared memory size
        auto sharedSize = size_t {size}; /* pgr0745 */
        // Shared address
        void *sharedAddress {nullptr};

        // Get Share memory flag from mode
        MDBSharedMemoryUtility::getShmFlagFromMode(mode, flag, mapFlag);

        if (ShmModes::CREATE == mode)
        {
            createFlag = true;
            // Open shared memory with create mode
            fileDescriptor = openShm(sharedName, O_CREAT | O_RDWR | O_EXCL);

            // If shared memory already exists then open shm again with read/write mode
            if (0 > fileDescriptor)
            {

                if (-2 == fileDescriptor)
                {
                    fileDescriptor = openShm(sharedName, O_RDWR);

                    // If still cannot open then return nullptr
                    if (0 > fileDescriptor)
                    {
                        mdb_apl_log_critical("Try to open shared memory failed!");
                        mdb_sys_log_out("F000801");
                        mdb_apl_log_end();
                        return nullptr;
                    }

                    createFlag = false;
                    mode = ShmModes::WRITE;
                }
                else
                {
                    mdb_apl_log_critical("Cannot create shared memory!");
                    mdb_sys_log_out("F000801");
                    mdb_apl_log_end();
                    return nullptr;
                }

            }

            // If in case not create shm then only open
        }
        else
        {
            // Log already output in openShm
            fileDescriptor = openShm(sharedName, flag);

            if (0 > fileDescriptor)
            {
                mdb_apl_log_critical("Cannot open shared memory!");
                mdb_sys_log_out("F000801");
                mdb_apl_log_end();
                return nullptr;
            }
        }

        if (true == createFlag)
        {
            // Truncate shared memory
            if (Result::NG == truncateShm(fileDescriptor, size, sharedName))
            {
                mdb_apl_log_critical("Cannot truncate shared memory!");
                mdb_sys_log_out("F000801");
                mdb_apl_log_end();
                ret = Result::NG;
            }

        }
        else
        {
            // Log already output in statShm
            sharedSize = statShm(fileDescriptor);

            if (0 == sharedSize)
            {
                mdb_apl_log_critical("Cannot get size of shared memory!");
                mdb_sys_log_out("F000801");
                mdb_apl_log_end();
                ret = Result::NG;
            }

        }

        if (Result::OK == ret)
        {
            // Log already output in mapShm
            sharedAddress = mapShm(sharedSize, mapFlag, fileDescriptor);
        }

        errno = 0;

        // File descriptor is no longer needed so closes it
        if (0 > close(fileDescriptor))
        {

            // Error no temp
            auto errnoTmp = errno; /* pgr0745 */
            // Error string buffer
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */

            // Error message temp
            auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_debug("close errno = [%d], strerror = [%s]", errnoTmp, errMsg);
            mdb_apl_log_critical("Cannot close file descriptor!");
            mdb_sys_log_out("F000801");
            mdb_apl_log_end();

            return nullptr;
        }

        if ((true == createFlag) && (nullptr != sharedAddress))
        {
            memset(sharedAddress, 0x00, sharedSize); /* pgr0579 */
        }

        mdb_apl_log_debug("out sharedAddress = [%p]", sharedAddress);
        mdb_apl_log_end();

        return sharedAddress;
    }

    /*!
     * @brief  Remove shared memory
     *
     * This function unmmap or destroy shared memory
     *
     * @param [in] sharedAddress shared memory address
     * @param [in] sharedSize shared memory size
     *
     * @retval Result::NG cannot unmap or unlink shared memory
     * @retval Result::OK of shm if normal
     */
    /** cppcheck-suppress unusedFunction */
    int MDBSharedMemoryUtility::unmapShm(void *sharedAddress, size_t sharedSize)
    {
        mdb_apl_log_start();
        mdb_apl_log_debug("in sharedAddress = [%p], sharedSize = [%d]", sharedAddress, sharedSize);

        // In case shared Address is null then return NG
        if (nullptr == sharedAddress)
        {
            mdb_apl_log_warn("sharedAddress is null!");
            mdb_apl_log_end();
            return Result::NG;
        }

        errno = 0;
        // Unmap shared memory
        auto ret = munmap(sharedAddress, sharedSize); /* pgr0745 */

        if (0 > ret)
        {

            auto errnoTmp = errno; /* pgr0745 */
            // Error string buffer
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_warn("munmap ret = [%d], errno = [%d], strerror = [%s]", ret, errnoTmp,
                             errMsg);
            ret = Result::NG;

        }

        // Don't need to implement unlink_shm
        // As ticket: https://120.72.83.84/issues/633
        mdb_apl_log_debug("out ret = [%d]", ret);
        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief  Delete shared memory object
     *
     * This function call shm_unlink on shm name
     *
     * @param [in] name shared memory name
     *
     * @retval Result::NG cannot unlink shared memory
     * @retval Result::OK of unlink OK
     */
    int MDBSharedMemoryUtility::unlink(const std::string &name)
    {
        mdb_apl_log_start();
        auto nameStr = name.c_str(); /* pgr0745 */
        mdb_apl_log_debug("in shm name = [%s]", nameStr);

        // In case shared Address is null then return NG
        if (true == name.empty())
        {
            mdb_apl_log_warn("share name is empty");
            mdb_apl_log_end();
            return Result::NG;
        }

        errno = 0;
        // Unmap shared memory
        auto ret = shm_unlink(name.c_str()); /* pgr0745 */

        if (0 > ret)
        {

            auto errnoTmp = errno; /* pgr0745 */
            // Error string buffer
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_warn("shm_unlink ret = [%d], errno = [%d], strerror = [%s]", ret, errnoTmp,
                             errMsg);
            ret = Result::NG;

        }

        mdb_apl_log_debug("out ret = [%d]", ret);
        mdb_apl_log_end();
        return ret;
    }

}

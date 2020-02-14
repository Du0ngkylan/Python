/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBSharedMemoryUtility.h
 * @brief   This file define Utilities for shared memory.
 * @author  trunghn:
 * @date    2016/10/04
 */

#ifndef LIBMDBSHM_MDBSHAREDMEMORYUTILITY_H_
#define LIBMDBSHM_MDBSHAREDMEMORYUTILITY_H_

#include "MDBConstants.h"

namespace mdbcom {

    /*!
     * @class MDBSharedMemoryUtility
     * @brief Utility functions
     */
    class MDBSharedMemoryUtility {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBSharedMemoryUtility(void) = delete;

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBSharedMemoryUtility(void) = delete;

        /*!
         * @brief Copy Constructor.
         * @param None.
         * @return None.
         */
        MDBSharedMemoryUtility(const MDBSharedMemoryUtility &) = delete;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return None.
         */
        MDBSharedMemoryUtility &operator =(const MDBSharedMemoryUtility &) = delete; /* pgr0055 */ /* pgr0309 */

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
        static void *getShm(const std::string &sharedName, size_t size, ShmModes &mode);

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
        static int unmapShm(void *sharedAddress, size_t sharedSize);

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
        static int unlink(const std::string &name);

    private:
        /*!
         * @brief  getShmFlagFromMode
         *  This function get open shm flag and mmap flag from mode
         *
         * @param [in] mode access shared memory mode
         * @param [out] shmFlag open shared memory flag
         * @param [out] mapFlag mmap shared memory flag
         * @return None.
         */
        static inline void getShmFlagFromMode(ShmModes mode, int &shmFlag, int &mapFlag);

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
        static int truncateShm(int fileDescriptor, size_t size, const std::string &sharedName);

        /*!
         * @brief  statShm
         *  This function get size of shared memory
         *
         * @param [in] fileDescriptor file descriptor
         *
         * @retval size share memory size
         * @retval Result::NG cannot get size
         */
        static size_t statShm(int fileDescriptor);

        /*!
         * @brief  openShm
         *  This function open shared memory
         *
         * @param [in] sharedName shared memory name
         * @param [in] flag open flag
         * @return fileDescriptor
         */
        static int openShm(const std::string &sharedName, int flag);

        /*!
         * @brief  mapShm
         *  This map shared memory
         *
         * @param [in] sharedSize size shared memory size
         * @param [in] mapFlag map flag
         * @param [in] fileDescriptor file desciptor
         * @return fileDescriptor
         */
        static void *mapShm(size_t sharedSize, int mapFlag, int fileDescriptor);

    };
}

#endif /* LIBMDBSHM_MDBSHAREDMEMORYUTILITY_H_ */

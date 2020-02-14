/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBSharedMemoryLock.h
 * @brief   This file define shared memory lock data for synchronize shared mem
 * @author  trunghn:
 * @date    2016/9/12
 */

#ifndef LIBMDBSHM_MDBSHAREMEMORYLOCK_H_
#define LIBMDBSHM_MDBSHAREMEMORYLOCK_H_

#include <pthread.h>
#include "MDBConstants.h"

namespace mdbcom {

    /*!
     * @struct MDBSharedMemoryLock
     * @brief Define mutex and condition used to synchronize shared mem,
     *  class member is placed in share memory so they must public
     */
    class MDBSharedMemoryLock { /*pgr2205*/
        /* pgr1233 */
    public:
        //! Pthread mutex
        pthread_mutex_t sharedMtx;
        //! Pthread condition
        pthread_cond_t sharedCond;
        //! Pthread mutex attribute
        pthread_mutexattr_t sharedMtxAttr;
        //! Pthread condition attribute
        pthread_condattr_t sharedCondAttr;

        /*!
         * @brief Default shared memory lock constructor
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedMemoryLock(void);

        /*!
         * @brief Default shared memory lock Destructor
         *
         * @param None.
         * @return None.
         *
         */
        ~MDBSharedMemoryLock(void);

        /*!
          * @brief Copy MDBSharedMemoryLock constructor (not use)
          *
          * @param None.
          * @return None.
          *
          */
        MDBSharedMemoryLock(const MDBSharedMemoryLock &) = delete; /* pgr0571 */ /* pgr2222 */ /*  pgr2223 */ /*pgr2266*/

        /*!
         * @brief Assign MDBSharedMemoryLock (not use)
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedMemoryLock &operator = (const MDBSharedMemoryLock &) = delete; /*  pgr2223 */ /* pgr0055 */ /* pgr0309 */

        /*!
          * @brief Copy MDBSharedMemoryLock constructor (not use)
          *
          * @param None.
          * @return None.
          *
          */
        MDBSharedMemoryLock(MDBSharedMemoryLock &&) = delete; /* pgr0571 */ /* pgr2222 */ /*  pgr2223 */

        /*!
         * @brief Assign MDBSharedMemoryLock (not use)
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedMemoryLock &operator = (MDBSharedMemoryLock &&) = delete; /*  pgr2223 */ /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Lock until mutex is acquired
         *
         * @retval Result::NG fail
         * @retval Result::OK success
         */
        int lock(void);

        /*!
         * @brief Unlock current holding mutex
         *
         * @retval Result::NG fail
         * @retval Result::OK success
         */
        int unlock(void);

        /*!
         * @brief Wait until conditional signal is sent
         *
         * @retval Result::NG fail
         * @retval Result::OK success
         */
        int wait(void);

        /*!
         * @brief Send conditional signal
         *
         * @retval Result::NG fail
         * @retval Result::OK success
         */
        int signal(void);

        /*!
         * @brief Send conditional signal to all waiting threads
         *
         * @retval Result::NG fail
         * @retval Result::OK success
         */
        int broadcast(void);
    };
}

#endif /*LIBMDBSHM_MDBSHAREMEMORYLOCK_H_*/

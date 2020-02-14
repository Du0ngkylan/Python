/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBSharedMemoryLock.cpp
 * @brief Implementation shared memory lock data for synchronize shared mem
 * @author trunghn:
 * @date 2016/9/12
 */

#include "MDBSharedMemoryLock.h"
#include "MDBLogger.h"
#include "MDBException.h"

namespace mdbcom {

    /*!
     * @brief Default shared memory lock constructor
     *
     * @param None.
     * @return None.
     *
     */
    MDBSharedMemoryLock::MDBSharedMemoryLock(void) :
        sharedMtx { }, sharedCond { }, sharedMtxAttr { }, sharedCondAttr { }
    {
        mdb_apl_log_start();

        // Init pthread mutex and pthread condition
        if (0 != pthread_mutexattr_init(&sharedMtxAttr))
        {
            mdb_apl_log_critical("Cannot init pthead mutex attribute!");
            throw MDBException {"Cannot init pthead mutex attribute!"};
        }

        if (0 != pthread_mutexattr_setpshared(&sharedMtxAttr,
                                              PTHREAD_PROCESS_SHARED))
        {
            mdb_apl_log_critical("Cannot set mutex attribute!");
            throw MDBException {"Cannot set mutex attribute!"};
        }

        if (0 != pthread_mutex_init(&sharedMtx, &sharedMtxAttr))
        {
            mdb_apl_log_critical("Cannot init pthead mutex!");
            throw MDBException {"Cannot init pthead mutex!"};
        }

        if (0 != pthread_condattr_init(&sharedCondAttr))
        {
            mdb_apl_log_critical("Cannot init pthread condition!");
            throw MDBException {"Cannot init pthread condition!"};
        }

        if (0 != pthread_condattr_setpshared(&sharedCondAttr,
                                             PTHREAD_PROCESS_SHARED))
        {
            mdb_apl_log_critical("Cannot set condition attribute!");
            throw MDBException {"Cannot set condition attribute!"};
        }

        if (0 != pthread_cond_init(&sharedCond,
                                   &sharedCondAttr))
        {
            mdb_apl_log_critical("Cannot init pthead condition!");
            throw MDBException {"Cannot init pthead condition!"};
        }

        mdb_apl_log_end();
    }

    /*!
     * @brief Default shared memory lock destructor
     *
     * @param None.
     * @return None.
     *
     */
    MDBSharedMemoryLock::~MDBSharedMemoryLock(void)
    {
        mdb_apl_log_start();

        if (0 != pthread_cond_destroy(&sharedCond))
        {
            mdb_apl_log_critical("Cannot destroy pthead condition!");
        }

        if (0 != pthread_condattr_destroy(&sharedCondAttr))
        {
            mdb_apl_log_critical("Cannot destroy pthread condition attribute!");
        }

        if (0 != pthread_mutex_destroy(&sharedMtx))
        {
            mdb_apl_log_critical("Cannot destroy pthead mutex!");
        }

        if (0 != pthread_mutexattr_destroy(&sharedMtxAttr))
        {
            mdb_apl_log_critical("Cannot inidestroyt pthead mutex attribute!");
        }

        mdb_apl_log_end();
    }

    /*!
     * @brief Lock until mutex is acquired
     *
     * @retval Result::NG fail
     * @retval Result::OK success
     */
    int MDBSharedMemoryLock::lock(void)
    {
        mdb_apl_log_start();
        auto ret = Result::OK; /*pgr0745*/

        // Lock pthread mutex
        if (0 != pthread_mutex_lock(&sharedMtx))
        {
            mdb_apl_log_critical("Can't lock mutex");
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Unlock current holding mutex
     *
     * @retval Result::NG fail
     * @retval Result::OK success
     */
    int MDBSharedMemoryLock::unlock(void)
    {
        mdb_apl_log_start();
        auto ret = Result::OK; /*pgr0745*/

        // Unlock pthread mutex
        if (0 != pthread_mutex_unlock(&sharedMtx))
        {
            mdb_apl_log_critical("Can't unlock mutex");
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Wait until conditional signal is sent
     *
     * @retval Result::NG fail
     * @retval Result::OK success
     */
    int MDBSharedMemoryLock::wait(void)
    {
        mdb_apl_log_start();
        auto ret = Result::OK; /*pgr0745*/

        // Start pthread condition wait
        if (0 != pthread_cond_wait(&sharedCond, &sharedMtx))
        {
            mdb_apl_log_critical("Can't wait for condition");
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Send conditional signal
     *
     * @retval Result::NG fail
     * @retval Result::OK success
     */
    /** cppcheck-suppress unusedFunction */
    int MDBSharedMemoryLock::signal(void)
    {
        mdb_apl_log_start();
        auto ret = Result::OK; /*pgr0745*/

        // Signal pthread condition
        if (0 != pthread_cond_signal(&sharedCond))
        {
            mdb_apl_log_critical("Can't send signal condition");
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Send conditional signal to all waiting threads
     *
     * @retval Result::NG fail
     * @retval Result::OK success
     */
    /** cppcheck-suppress unusedFunction */
    int MDBSharedMemoryLock::broadcast(void)
    {
        mdb_apl_log_start();
        auto ret = Result::OK; /*pgr0745*/

        // Broadcast pthread condition
        if (0 != pthread_cond_broadcast(&sharedCond))
        {
            mdb_apl_log_critical("Can't broadcast condition");
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }
}

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDebugLevelMap.h
 * @brief       Header file for define MDBDebugLevelMap information.
 * @author      cuongadp: Support change log level.
 * @date        2016/10/26
 */
#ifndef LIBMDBUTIL_MDBDEBUGLEVELMAP_H_
#define LIBMDBUTIL_MDBDEBUGLEVELMAP_H_

#include "MDBSharedMemoryLock.h"
#include "MDBDebugLevelEntry.h"

namespace mdbcom {

    class MDBDebugLevelMap { /* pgr1233 */
        /* pgr2205 */

    public:
        //! Shared memory lock
        MDBSharedMemoryLock lock;

        //! Number of entry
        int count;

        //! array of debug log level
        std::array<MDBDebugLevelEntry, Log::CONF_NUM> data;

        /**
         * @brief Default Constructor
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelMap(void) = default; /* pgr2266 */

        /**
         * @brief Copy Constructor (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelMap(const MDBDebugLevelMap &) = delete; /* pgr2222 *//* pgr0571 */ /* pgr2266 */

        /**
         * @brief Copy Assignment (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelMap &operator=(const MDBDebugLevelMap &) = delete; /* pgr0055 *//* pgr2223 */ /* pgr0309 */

        /**
         * @brief Move Constructor (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelMap(MDBDebugLevelMap &&) = delete; /* pgr2222 */ /* pgr0571 */

        /**
         * @brief Move Assignment (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelMap &operator=(MDBDebugLevelMap &&) = delete; /* pgr0055 *//* pgr2223 */ /* pgr0309 */

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~MDBDebugLevelMap(void) = default;
    };
}

#endif

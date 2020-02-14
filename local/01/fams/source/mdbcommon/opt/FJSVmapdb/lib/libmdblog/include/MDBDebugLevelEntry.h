/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDebugLevelEntry.h
 * @brief       Header file for define MDBDebugLevelEntry information.
 * @author      cuongadp: Support change log level.
 * @date        2016/10/26
 */
#ifndef LIBMDBUTIL_MDBDEBUGLEVELENTRY_H_
#define LIBMDBUTIL_MDBDEBUGLEVELENTRY_H_

#include <atomic>
#include "MDBConstants.h"

namespace mdbcom {

    class MDBDebugLevelEntry { /* pgr1233 */
        /* pgr2205 */

    public:
        //! function(module) name
        std::array<char, Log::CONF_FUNC_LENGTH> function;

        //! level
        std::atomic_int level;

        /**
         * @brief Default Constructor
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelEntry(void);

        /**
         * @brief Copy Constructor (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelEntry(const MDBDebugLevelEntry &) = delete; /* pgr2222 *//* pgr0571 */ /* pgr2266 */

        /**
         * @brief Copy Assignment (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelEntry &operator=(const MDBDebugLevelEntry &) = delete; /* pgr0055 *//* pgr2223 */ /* pgr0309 */

        /**
         * @brief Move Constructor (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelEntry(MDBDebugLevelEntry &&) = delete; /* pgr2222 */ /* pgr0571 */

        /**
         * @brief Move Assignment (not use)
         *
         * @param None.
         * @return  None.
         */
        MDBDebugLevelEntry &operator=(MDBDebugLevelEntry &&) = delete; /* pgr0055 *//* pgr2223 */ /* pgr0309 */

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~MDBDebugLevelEntry(void) = default;
    };
}

#endif

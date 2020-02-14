/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBSharedHeader.h
 * @brief   This file define type structure for shared header
 * @author  trunghn:
 * @date    2016/10/28
 */

#ifndef LIBMDBIPC_MDBSHAREDHEADER_H_
#define LIBMDBIPC_MDBSHAREDHEADER_H_

#include <atomic>
#include "MDBSharedMemoryLock.h"

namespace mdbcom {
    /*!
     * @struct MDBSharedHeader
     * @brief Define mutex, condition and queue monitoring variables,
     * used to synchronize shared mem, class member is placed in share
     * memory so they must public
     */
    class MDBSharedHeader { /*pgr2205*/
        /* pgr1233 */
    public:
        //! lock for reading shm
        MDBSharedMemoryLock read;

        //! lock for writing shm
        MDBSharedMemoryLock write;

        /*!
         * @brief Default shared header constructor
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedHeader(void) : read {}, write {} {

        };

        /*!
         * @brief Default shared header destructor
         *
         * @param None.
         * @return None.
         *
         */
        ~MDBSharedHeader(void) = default;

        /*!
         * @brief Copy MDBSharedHeader constructor (not use)
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedHeader(const MDBSharedHeader &) = delete; /* pgr0571 *//* pgr2222 */ /*  pgr2223 */ /*pgr2266*/

        /*!
         * @brief Assign MDBSharedHeader (not use)
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedHeader &operator =(const MDBSharedHeader &) = delete; /*  pgr2223 *//* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Copy MDBSharedHeader constructor (not use)
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedHeader(MDBSharedHeader &&) = delete; /* pgr0571 *//* pgr2222 */ /*  pgr2223 */

        /*!
         * @brief Assign MDBSharedHeader (not use)
         *
         * @param None.
         * @return None.
         *
         */
        MDBSharedHeader &operator =(MDBSharedHeader &&) = delete; /*  pgr2223 *//* pgr0055 */ /* pgr0309 */

    };
}
#endif /* End LIBMDBIPC_MDBSHAREDHEADER_H_*/

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBMemoryUtility.h
 * @brief Definition for memory operations
 * @author cuongadp:
 * @date 2016/11/14
 */

#ifndef LIBMDBUTIL_MDBMEMORYUTILITY_H_
#define LIBMDBUTIL_MDBMEMORYUTILITY_H_

#include <memory>

namespace mdbcom {
    class MDBMemoryUtility {
    public:
        template<class T>
        struct MDBUniquePtrReturnType
        {
            typedef std::unique_ptr<T> SingleObject;
        };

        template<class T>
        struct MDBUniquePtrReturnType<T[]>   /* pgr0694 */
        {
            typedef std::unique_ptr<T[]> UnknownBound; /* pgr0694 */
        };

        template<class T, size_t N>
        struct MDBUniquePtrReturnType<T[N]>
        {
            typedef void KnownBound;
        };

        /*!
         * @brief Create unique pointer for single object
         * @param [in] params... :List of constructor's arguments
         * @return new unique_ptr
         */
        template<class T, class ... Arg>
        static typename MDBUniquePtrReturnType<T>::SingleObject
        make_unique(Arg &&... params) {
            return std::unique_ptr < T > (new T(std::forward<Arg>(params)...));
        };

        /*!
         * @brief Create unique pointer for unknown bound array
         * @param [in] n Size of array
         * @return new unique_ptr
         */
        template<class T>
        static typename MDBUniquePtrReturnType<T>::UnknownBound
        make_unique(size_t n) {
            typedef typename std::remove_extent<T>::type U;
            return std::unique_ptr<T>(new U[n]());
        };

        /*!
         * @brief Create unique pointer for known bound array
         * @param [in] params... :List of constructor's arguments
         * @return new unique_ptr
         */
        template<class T, class... Args>
        static typename MDBUniquePtrReturnType<T>::KnownBound /* pgr0055 */
        make_unique(Args &&...) = delete; /* pgr0309 */
    };
}

#endif

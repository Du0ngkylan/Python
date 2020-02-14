/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBTypes.h
 * @brief Definition for constants
 * @author trunghn:
 * @date 2016/11/14
 */

#ifndef LIBMDBUTIL_MDBTYPES_H_
#define LIBMDBUTIL_MDBTYPES_H_

#include <vector>
#include <MDBPreparedStatement.h>

namespace mdbcom {
    //! List of prepare statements type
    using PrepareStatements = std::vector<MDBPreparedStatement>;

    //! Thread function type
    using ThreadFunction = void (*)(void *);
}

#endif

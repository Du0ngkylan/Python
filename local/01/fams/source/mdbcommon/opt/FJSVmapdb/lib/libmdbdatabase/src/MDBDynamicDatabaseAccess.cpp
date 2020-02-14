/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDynamicDatabaseAccess.cpp
 * @brief       Source file for implementing MDBDynamicDatabaseAccess information.
 * @author      trangvp: create it for support MDBDynamicDatabaseAccess code.
 * @date        2016/10/26
 */

#include "MDBDynamicDatabaseAccess.h"

namespace mdbcom {

    // Constant
    const std::string MDBDynamicDatabaseAccess::DB_DYNAMIC_MNG_CONF
    {
        "/opt/FJSVmapdb/etc/dynamic_management_db.conf"
    };

    /*!
     * @brief Constructor.
     * @param None.
     * @return None.
     */
    MDBDynamicDatabaseAccess::MDBDynamicDatabaseAccess(void) :
        MDBDatabaseAccess { DB_DYNAMIC_MNG_CONF }
    {

    }

    /*!
     * @brief Destructor.
     * @param None.
     * @return None.
     */
    MDBDynamicDatabaseAccess::~MDBDynamicDatabaseAccess(void)
    {

    }
}


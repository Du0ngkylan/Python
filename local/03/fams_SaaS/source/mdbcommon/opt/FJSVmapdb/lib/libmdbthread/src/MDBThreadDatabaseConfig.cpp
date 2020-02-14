/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBThreadDatabaseConfig.cpp
 * @brief       Source file for implementing MDBThreadDatabaseConfig class.
 * @author      cuongadp: create it for support thread wrapper
 * @date        2016/11/21
 */

#include "MDBThreadDatabaseConfig.h"

namespace mdbcom {

    /**
     * @brief Default Constructor
     *
     * @param [in] statements Prepare statements for database
     * @param [in] type Type of database
     * @return  None.
     */
    MDBThreadDatabaseConfig::MDBThreadDatabaseConfig(const PrepareStatements &statements, /* pgr0304 */
            ConnectionType type) :
        // Due to gcc pre-4.9.0 bug, Universal initializer with reference will compile fail with -Weff
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=50025
        prepareStatements(statements), connType { type }
    {
    }

    /**
     * @brief Copy Constructor
     *
     * Not support
     *
     * @param  None.
     * @return  None.
     */
    MDBThreadDatabaseConfig::MDBThreadDatabaseConfig(const MDBThreadDatabaseConfig &other) :
        prepareStatements(other.prepareStatements), connType { other.connType }
    {
    }

    /**
     * @brief Move Constructor
     *
     * Not support
     *
     * @param  None.
     * @return  None.
     */
    MDBThreadDatabaseConfig::MDBThreadDatabaseConfig(MDBThreadDatabaseConfig &&other) :
        prepareStatements(other.prepareStatements), connType { other.connType }
    {
    }
}

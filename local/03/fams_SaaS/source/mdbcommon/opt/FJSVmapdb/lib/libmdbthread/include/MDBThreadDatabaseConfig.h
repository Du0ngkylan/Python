/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBThreadDatabaseConfig.h
 * @brief       Header file for define MDBThreadDatabaseConfig information.
 * @author      cuongadp: Support thread wrapper.
 * @date        2016/11/23
 */
#ifndef LIBMDBTHREAD_MDBTHREADDATABASECONFIG_H_
#define LIBMDBTHREAD_MDBTHREADDATABASECONFIG_H_

#include "MDBTypes.h"
#include "MDBConstants.h"

namespace mdbcom {

    /*! @class MDBThreadDatabaseConfig
     *  @brief Contain database information to be used in thread
     */
    class MDBThreadDatabaseConfig { /* pgr2205 */
    public:
        /**
         * @brief Default Constructor
         *
         * @param [in] statements Prepare statements for database
         * @param [in] type Type of database
         * @return  None.
         */
        MDBThreadDatabaseConfig(const PrepareStatements &statements,
                                ConnectionType type);

        /**
         * @brief Copy Constructor
         *
         * Not support
         *
         * @param  None.
         * @return  None.
         */
        MDBThreadDatabaseConfig(const MDBThreadDatabaseConfig &other);

        /*!
         * @brief Copy assignment
         *
         * Not support
         *
         * @param  None.
         * @return  None.
         */
        MDBThreadDatabaseConfig &operator =(const MDBThreadDatabaseConfig &) = delete; /* pgr0055 */ /* pgr2223 */ /* pgr0309 */ /* pgr2252 */

        /**
         * @brief Move Constructor
         *
         * Not support
         *
         * @param  None.
         * @return  None.
         */
        MDBThreadDatabaseConfig(MDBThreadDatabaseConfig &&other);

        /*!
         * @brief Move assignment
         *
         * Not support
         *
         * @param  None.
         * @return  None.
         */
        MDBThreadDatabaseConfig &operator =(MDBThreadDatabaseConfig &&) = delete; /* pgr0055 */ /* pgr2223 */ /* pgr0309 */ /* pgr2252 */

        /*!
         * @brief Default Destructor.
         * @param None.
         * @return None.
         */
        ~MDBThreadDatabaseConfig(void) = default;

        /**
         * @brief Getter for connection type
         * @param None
         * @return Type of database
         */
        inline ConnectionType getConnType(void) const {
            return connType;
        }

        /**
         * @brief Getter for prepare statements
         * @param None
         * @return Reference to prepared statements
         */
        inline const PrepareStatements &getPrepareStatements(void) const {
            return prepareStatements;
        }

    private:
        //! Ref to ist of prepare statement
        const PrepareStatements &prepareStatements;

        //! Type of database
        ConnectionType connType;
    };
}

#endif
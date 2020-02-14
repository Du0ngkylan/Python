/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBResourceMapper.h
 * @brief       Header file for define MDBResourceMapper information.
 * @author      trangvp: create it for support implement MDBResourceMapper code.
 * @date        2016/10/13
 */

#ifndef LIBMDBUTIL_MDBRESOURCEMAPPER_H_
#define LIBMDBUTIL_MDBRESOURCEMAPPER_H_

#include "MDBDatabaseAccess.h"
#include <memory>

namespace mdbcom {
    /*!
     * @class MDBResourceMapper
     * @brief A pool of resources of threads
     */
    class MDBResourceMapper {
    public:
        /**
         * @brief Data constructor.
         * Construct resource map for database connections.
         *
         * @param [in] size Size of resource map.
         * @param [in] dynamicPrepareStatements Prepare statements for dynamic Db.
         * @return None.
         */
        MDBResourceMapper(int size, const PrepareStatements *dynamicPrepareStatements);

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBResourceMapper(void);

        /**
         * @brief Assignment constructor.
         *
         * Not support.
         *
         */
        MDBResourceMapper(const MDBResourceMapper &) = delete; /*pgr0571*/ /*pgr2222*/

        /**
         * @brief Assignment operation.
         *
         * Not support.
         *
         */
        MDBResourceMapper &operator=(const MDBResourceMapper &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /**
         * @brief Get connection
         *
         * Get connection for dynamic database
         *
         * @retval nullptr  If source have no connection.
         * @retval Pointer to database connection.
         */
        std::unique_ptr<const MDBDatabaseAccess> &getDynamicDBConn(void);

        /**
         * @brief Check resource
         *
         * Check if current resource contain Dynamic DB connection
         *
         * @param None.
         * @retval true  If using Dynamic database
         * @retval false If not using Dynamic database
         */
        bool isUsingDynamicDB(void) const;

    private:
        class Impl;
        std::unique_ptr<Impl> pImpl; /*pgr0359*/
    };

}

#endif /* LIBMDBUTIL_MDBRESOURCEMAPPER_H_ */

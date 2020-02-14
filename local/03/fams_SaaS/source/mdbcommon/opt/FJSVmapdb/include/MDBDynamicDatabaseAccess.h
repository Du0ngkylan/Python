/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDynamicDatabaseAccess.h
 * @brief       Header file for define MDBDynamicDatabaseAccess class.
 * @author      trangvp: create it for support MDBDynamicDatabaseAccess code.
 * @date        2016/10/26
 */

#ifndef LIBMDBDB_MDBDYNAMICDATABASEACCESS_H_
#define LIBMDBDB_MDBDYNAMICDATABASEACCESS_H_

#include "MDBDatabaseAccess.h"

namespace mdbcom {
    /*!
     * @class MDBDynamicDatabaseAccess
     * @brief The dynamic db management connection
     */
    class MDBDynamicDatabaseAccess: public MDBDatabaseAccess { /*pgr2205*/
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBDynamicDatabaseAccess(void);

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBDynamicDatabaseAccess(void);
    private:
        //! Constant for path of configure file
        static const std::string DB_DYNAMIC_MNG_CONF;
    };
}

#endif /* LIBMDBDB_MDBDYNAMICDATABASEACCESS_H_ */

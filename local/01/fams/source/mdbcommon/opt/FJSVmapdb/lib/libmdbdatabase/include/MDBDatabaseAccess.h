/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDatabaseAccess.h
 * @brief       Header file for define MDBDatabaseAccess information.
 * @author      trangvp: create it for support MDBDatabaseAccess code.
 * @date        2016/09/24
 */

#ifndef LIBMDBDB_MDBDATABASEACCESS_H_
#define LIBMDBDB_MDBDATABASEACCESS_H_

#include <libpq-fe.h>
#include <unistd.h>
#include <memory>
#include <vector>

#include "MDBDatabaseParam.h"
#include "MDBTypes.h"
#include "MDBConnectionInfo.h"
#include "MDBStatement.h"

namespace mdbcom {
    /*! @class  MDBDatabaseAccess.
     *  @brief The class supports interacting with database.
     */
    class MDBDatabaseAccess {
    public:
        /**
         * @brief Copy constructor.
         *  This is unsupported
         * @param None.
         * @return None.
         */
        MDBDatabaseAccess(const MDBDatabaseAccess &) = delete; /*pgr2222*/ /*pgr0571*/

        /*!
         * @brief Default destructor of MDBDatabaseAccess.
         * @param None.
         * @return None.
         */
        virtual ~MDBDatabaseAccess(void);

        /**
         * @brief Copy assignment.
         *  This is unsupported
         * @param None.
         * @return This MDBDatabaseAccess.
         */
        MDBDatabaseAccess &operator=(const MDBDatabaseAccess &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief Start a transaction.
         * @param None.
         * @retval Result::NG start a transaction failed.
         * @retval Result::OK start a transaction successfully.
         */
        int startTransaction(void) const;

        /*!
         * @brief Start a transaction.
         * @param [out] errCode Postgresql error code (https://www.postgresql.org/docs/9.2/static/errcodes-appendix.html)
         * @retval Result::NG start a transaction failed.
         * @retval Result::OK start a transaction successfully.
         */
        int startTransaction(std::string &errCode) const;

        /*!
         * @brief commit a transaction.
         *
         * @return result of commit transaction.
         * @retval Result::OK commit a transaction successfully.
         * @retval Result::NG commit a transaction failed.
         */
        int commit(void) const;

        /*!
         * @brief commit a transaction.
         *
         * @param [out] errCode Postgresql error code (https://www.postgresql.org/docs/9.2/static/errcodes-appendix.html)
         *
         * @return result of commit transaction.
         * @retval Result::OK commit a transaction successfully.
         * @retval Result::NG commit a transaction failed.
         */
        int commit(std::string &errCode) const;

        /*!
         * @brief send vector of prepare statement.
         *
         * @param [in] preparedStatementVec vector of statement.
         *
         * @retval Result::OK prepare a query successfully.
         * @retval Result::NG prepare a query failed.
         */
        int prepareAll(const PrepareStatements &preparedStatementVec);

        /*!
         * @brief execute a SQL (not prepared before).
         *
         * @param [in] sql the sql statement to excute.
         * @param [out] error output error
         *
         * @return a share pointer MDBStatement statement result and data.
         */
        std::shared_ptr<MDBStatement> executeSQL(const std::string &sql,
                const std::vector<MDBDatabaseParam> &params = std::vector<MDBDatabaseParam>()) const;

        /*!
         * @brief execute a query with statement name and input params.
         *
         * @param [in] stmName statement name.
         * @param [in] params input params.
         *
         * @return a share pointer MDBStatement statement result and data.
         */
        std::shared_ptr<MDBStatement> execute(const std::string &stmName,
                                              const std::vector<MDBDatabaseParam> &params = std::vector<MDBDatabaseParam>()) const;

        /*!
         * @brief execute a query with statement name and input params at transactional mode.
         *
         * @param [in] stmName statement name.
         * @param [in] params input params.
         *
         * @return a share pointer MDBStatement statement result and data.
         */
        std::shared_ptr<MDBStatement> executeWithTransaction(const std::string &stmName,
                const std::vector<MDBDatabaseParam> &params = std::vector<MDBDatabaseParam>()) const;

        /*!
         * @brief execute a rollback.
         * @param None.
         *
         * @retval Result::OK execute a rollback query successfully.
         * @retval Result::NG execute a rollback failed.
         */
        int rollback(void) const;

        /*!
         * @brief execute a rollback.
         * @param [out] errCode Postgresql error code (https://www.postgresql.org/docs/9.2/static/errcodes-appendix.html)
         *
         * @retval Result::OK execute a rollback query successfully.
         * @retval Result::NG execute a rollback failed.
         */
        int rollback(std::string &errCode) const;

        /*!
         * @brief Get connection state
         * @param None.
         * @retval Result::NG Connection is disconnected.
         * @retval Result::OK Connection is connected.
         */
        int getConnectionState(void) const;

        /*!
         * @brief Reset MDBDatabaseAccess and re-send prepared statements.
         * @param None.
         * @retval Result::OK Reconnect and resend prepareStatements successfully.
         * @retval Result::NG Reconnect or resend prepareStatements failed.
         */
        int reset(void) const;
    protected:
        /*!
         * @brief Constructor of MDBDatabaseAccess with config path.
         * @param [in] configPath the config path.
         * @return None.
         */
        explicit MDBDatabaseAccess(const std::string &configPath);

    private:
        /*! @class Impl.
         *  @brief The class supports behaviors of MDBDatabaseAccess.
         */
        class Impl;

        //!Pointer to Impl class
        std::unique_ptr<Impl> pImpl; /*pgr0359*/
    };
}

#endif /* LIBMDBDB_MDBDATABASEACCESS_H_ */

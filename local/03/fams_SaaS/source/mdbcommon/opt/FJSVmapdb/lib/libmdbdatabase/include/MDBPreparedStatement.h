/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBPreparedStatement.h
 * @brief       Header file for define MDBPreparedStatement class.
 * @author      trangvp: create it for support MDBPreparedStatement code.
 * @date        2016/10/26
 */

#ifndef LIBMDBDB_MDBPREPAREDSTATEMENT_H_
#define LIBMDBDB_MDBPREPAREDSTATEMENT_H_

#include <string>
#include <memory>
namespace mdbcom {
    /*!
     * @class MDBPreparedStatement
     * @brief Class of prepared statement
     */
    class MDBPreparedStatement {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBPreparedStatement(void);

        /*!
         * @brief Constructor.
         * @param [in] stmName statement name.
         * @param [in] sqlQuery the SQL query.
         * @param [in] paramNumber the number of parameters
         * @return None.
         */
        MDBPreparedStatement(const std::string &stmName, const std::string &sqlQuery,
                             int paramNumber);

        /*!
         * @brief Constructor.
         * @param [in] other the other MDBPreparedStatement.
         * @return None.
         */
        MDBPreparedStatement(const MDBPreparedStatement &other);

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBPreparedStatement(void);

        /*!
         * @brief Operator=.
         * @param [in] other the other MDBPreparedStatement.
         * @return the MDBPreparedStatement.
         */
        MDBPreparedStatement &operator=(const MDBPreparedStatement &other);

        /*!
         * @brief get statement name
         * @param None.
         * @return the statement name
         */
        const std::string &getStmName(void) const;

        /*!
         * @brief get SQL query
         * @param None.
         * @return the SQL query
         */
        const std::string &getSqlQuery(void) const;

        /*!
         * @brief get parameter number
         * @param None.
         * @return the parameter number
         */
        int getParamNumber(void) const;
        /*!
         * @brief set Statement name
         * @param [in] stmName statement name
         * @return None.
         */
        void setStmName(const std::string &stmName);

        /*!
         * @brief set SQL query
         * @param [in] sqlQuery sSQL query
         * @return None.
         */
        void setSqlQuery(const std::string &sqlQuery);

        /*!
         * @brief set number of parameter
         * @param [in] paramNumber number of parameter
         * @return None.
         */
        void setParamNumber(int paramNumber);

    private:
        /*! @class Impl.
         *  @brief The class supports behaviors of MDBPreparedStatement.
         */
        class Impl;

        //!Pointer to Impl class
        std::unique_ptr<Impl> pImpl; /*pgr0359*/
    };
}

#endif /* LIBMDBDB_MDBPREPAREDSTATEMENT_H_ */

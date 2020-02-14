/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBPreparedStatement.cpp
 * @brief       Source file for implementing MDBPreparedStatement information.
 * @author      trangvp: create it for support MDBPreparedStatement code.
 * @date        2016/10/26
 */

#include "MDBPreparedStatement.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {

    /*!
     * @class Impl
     * @brief Class of prepared statement
     */
    class MDBPreparedStatement::Impl {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        Impl(void);

        /*!
         * @brief Constructor.
         * @param [in] stmName statement name.
         * @param [in] sqlQuery the SQL query.
         * @param [in] paramNumber the number of parameters
         * @return None.
         */
        Impl(const std::string &stmName, const std::string &sqlQuery, int paramNumber);

        /*!
         * @brief get statement name
         * @param None.
         * @return the statement name
         */
        inline const std::string &getStmName(void) const {
            return mStmName;
        }

        /*!
         * @brief get SQL query
         * @param None.
         * @return the SQL query
         */
        inline const std::string &getSqlQuery(void) const {
            return mSqlQuery;
        }

        /*!
         * @brief get parameter number
         * @param None.
         * @return the parameter number
         */
        inline int getParamNumber(void) const {
            return mParamNumber;
        }

        /*!
         * @brief set Statement name
         * @param [in] stmName statement name
         * @return None.
         */
        inline void setStmName(const std::string &stmName) {
            mStmName = stmName;
        }

        /*!
         * @brief set SQL query
         * @param [in] sqlQuery sSQL query
         * @return None.
         */
        inline void setSqlQuery(const std::string &sqlQuery) {
            mSqlQuery = sqlQuery;
        }

        /*!
         * @brief set number of parameter
         * @param [in] paramNumber number of parameter
         * @return None.
         */
        inline void setParamNumber(int paramNumber) {
            mParamNumber = paramNumber;
        }

    private:
        //! statement name
        std::string mStmName;

        //! SQL query
        std::string mSqlQuery;

        //! number of parameters
        int mParamNumber;
    };

    //---------------------------- Impl-----------------------------

    MDBPreparedStatement::Impl::Impl(void) :
        mStmName { }, mSqlQuery { }, mParamNumber { }
    {
    }

    MDBPreparedStatement::Impl::Impl(const std::string &stmName, const std::string &sqlQuery,
                                     int paramNumber) :
        mStmName { stmName }, mSqlQuery { sqlQuery }, mParamNumber { paramNumber }
    {
    }

    //---------------------------- MDBPreparedStatement-----------------------------

    /*!
     * @brief Constructor.
     * @param None.
     * @return None.
     */
    MDBPreparedStatement::MDBPreparedStatement(void) :
        pImpl { MDBMemoryUtility::make_unique<Impl>() }
    {

    }

    /*!
     * @brief Constructor.
     * @param [in] stmName statement name.
     * @param [in] sqlQuery the SQL query.
     * @param [in] paramNumber the number of parameters
     * @return None.
     */
    MDBPreparedStatement::MDBPreparedStatement(const std::string &stmName,
            const std::string &sqlQuery, int paramNumber) :
        pImpl { MDBMemoryUtility::make_unique<Impl>(stmName, sqlQuery, paramNumber) }
    {

    }

    /*!
     * @brief Constructor.
     * @param [in] other the other MDBPreparedStatement.
     * @return None.
     */
    MDBPreparedStatement::MDBPreparedStatement(const MDBPreparedStatement &other) :
        pImpl { MDBMemoryUtility::make_unique<Impl>(other.getStmName(), other.getSqlQuery(), other.getParamNumber()) } /*pgr0512*/
    {
    }

    MDBPreparedStatement::~MDBPreparedStatement(void) = default;

    /*!
     * @brief Operator=.
     * @param [in] other the other MDBPreparedStatement.
     * @return the MDBPreparedStatement.
     */
    MDBPreparedStatement &MDBPreparedStatement::operator=(const MDBPreparedStatement &other)
    {
        if (this != &other)
        {
            pImpl->setStmName(other.getStmName());
            pImpl->setSqlQuery(other.getSqlQuery());
            pImpl->setParamNumber(other.getParamNumber());
        }

        return *this;
    }
    /**!
     * @brief get statement name
     * @param None.
     * @return the statement name
     */
    const std::string &MDBPreparedStatement::getStmName(void) const
    {
        return pImpl->getStmName();
    }

    /**!
     * @brief get SQL query
     * @param None.
     * @return the SQL query
     */
    const std::string &MDBPreparedStatement::getSqlQuery(void) const
    {
        return pImpl->getSqlQuery();
    }

    /**!
     * @brief get parameter number
     * @param None.
     * @return the parameter number
     */
    int MDBPreparedStatement::getParamNumber(void) const
    {
        return pImpl->getParamNumber();
    }

    /**!
     * @brief set Statement name
     * @param [in] stmName statement name
     * @return None.
     */
    void MDBPreparedStatement::setStmName(const std::string &stmName)
    {
        pImpl->setStmName(stmName);
    }

    /**!
     * @brief set SQL query
     * @param [in] sqlQuery sSQL query
     * @return None.
     */
    void MDBPreparedStatement::setSqlQuery(const std::string &sqlQuery)
    {
        pImpl->setSqlQuery(sqlQuery);
    }

    /**!
     * @brief set number of parameter
     * @param [in] paramNumber number of parameter
     * @return None.
     */
    void MDBPreparedStatement::setParamNumber(int paramNumber)
    {
        pImpl->setParamNumber(paramNumber);
    }
}


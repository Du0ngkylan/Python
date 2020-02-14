/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBStatement.h
 * @brief       Header file for defining MDBStatement class.
 * @author      trangvp: create it for defining MDBStatement class.
 * @date        Sep 20, 2016
 */

#ifndef LIBMDBDB_MDBSTATEMENT_H_
#define LIBMDBDB_MDBSTATEMENT_H_

#include <libpq-fe.h>
#include <string>
#include <Poco/Dynamic/Var.h>
#include "MDBLogger.h"
#include "MDBConstants.h"

namespace mdbcom {
    /*! @class  MDBStatement.
     * @brief The class support statement to database.
     */
    class MDBStatement {
    public:

        /*!
         * @brief Default constructor of MDBStatement.
         * @param None.
         * @return None.
         */
        MDBStatement(void);

        /*!
         * @brief Copy Constructor.
         * @param None.
         * @return None.
         */
        MDBStatement(const MDBStatement &) = delete; /*pgr2222*/ /*pgr0571*/

        /*!
         * @brief Constructor.
         *
         * @param [in] result the result to set.
         * @param [in] resultCode the error code
         * @param [in] errorCode Sql state of Posgres
         *
         * @return None.
         */
        MDBStatement(PGresult *result, int resultCode, const std::string &errorCode);

        /*!
         * @brief Default destructor of MDBStatement.
         * @param None.
         * @return None.
         */
        ~MDBStatement(void);

        /*!
         * @brief Overload operator= of MDBStatement.
         * @param None.
         * @return this MDBStatement.
         */
        MDBStatement &operator=(const MDBStatement &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief get statements result.
         * @param None.
         *
         * @retval Result::OK when execute statement successfully.
         * @retval Result::NG when statement failed.
         */
        inline int getResult(void) const {
            return mResultStatus;
        }

        /*!
         * @brief get number of rows of database.
         * ONLY use this function for a query and the number of rows is smaller than MAX size of INT
         * @param None.
         *
         * @return number of rows of database.
         */
        inline int getNumRows(void) const {
            return PQntuples(mResult);
        }

        /*!
         * @brief get number of lines of database.
         *        SQL excutes result of non-SELECT.
         * @param None.
         *
         * @return number of lines of database.
         */
        inline char *getNumberLines(void) const {
            return PQcmdTuples(mResult);
        }

        /*!
         * @brief get number of columns of database.
         * @param None.
         *
         * @return number of columns of database.
         */
        inline int getNumCols(void) const {
            return PQnfields(mResult);
        }

        /*!
         * @brief get value from column and row index to an output.
         *
         * @param [in] rowIndex row index.
         * @param [in] columnIndex column index.
         * @param [out] output store the output.
         *
         * @retval Result::OK when get value successfully.
         * @retval Result::NG when get value failed.
         */
        template<typename T>
        inline int getValue(const int rowIndex, const int columnIndex, T &output) const {
            // https://www.postgresql.org/docs/9.2/static/datatype.html
            auto ret = Result::NG; /* pgr0745 */
            // get value
            auto data = PQgetvalue(mResult, rowIndex, columnIndex); /* pgr0745 */

            if (nullptr != data) {
                ret = Result::OK;
                auto stringData = std::string { data }; /* pgr0745 */
                // Convert data to output parameter
                auto converter = Poco::Dynamic::Var { stringData }; /* pgr0745 */

                try {
                    converter.convert(output);
                }
                catch (const Poco::SyntaxException &) {
                    ret = Result::NG;
                }
                catch (...) {
                    ret = Result::NG;
                }
            }

            return ret;
        }

        /*!
         * @brief get value from column name and row index to an output.
         *
         * @param [in] rowIndex row index.
         * @param [in] columnName column name.
         * @param [out] output store the output.
         *
         * @retval Result::OK when get value successfully.
         * @retval Result::NG when get value failed.
         */
        template<typename T>
        inline int getValue(const int rowIndex, const std::string &columnName, T &output) const {
            auto ret = Result::NG; /* pgr0745 */
            // get column index by name
            auto colIndex = PQfnumber(mResult, columnName.c_str()); /* pgr0745 */

            // return function if column name is mismatch
            if (colIndex < 0) {
                return ret;
            }

            // Get value
            auto data = PQgetvalue(mResult, rowIndex, colIndex); /* pgr0745 */

            if (nullptr != data) {
                ret = Result::OK;
                auto stringData = std::string { data }; /* pgr0745 */
                // Convert data to output parameter
                auto converter = Poco::Dynamic::Var { stringData }; /* pgr0745 */

                try {
                    converter.convert(output);
                }
                catch (const Poco::SyntaxException &) {
                    ret = Result::NG;
                }
                catch (...) {
                    ret = Result::NG;
                }
            }

            return ret;
        }

        /*!
         * @brief Convert column name to column index
         * @param [in] columnName column name
         * @return column index (-1 when not found)
         */
        inline int getColumnIndex(const std::string &columnName) {
            return PQfnumber(mResult, columnName.c_str());
        }

        /*!
         * @brief Get error code from database
         * @param None
         * @return postgresql error code (https://www.postgresql.org/docs/9.2/static/errcodes-appendix.html)
         */
        inline const std::string &getSqlErrorCode(void) const {
            return sqlErrorCode;
        }

    private:
        friend class MDBDatabaseAccess;

        //! store result of statement
        PGresult *mResult;

        //! result status
        int mResultStatus;

        //! postgresql error state;
        std::string sqlErrorCode;

        /*!
         * @brief Clear result resource.
         *
         * @param None.
         *
         * @return None.
         */
        void clear(void);

    };
}

#endif /* LIBMDBDB_MDBSTATEMENT_H_ */

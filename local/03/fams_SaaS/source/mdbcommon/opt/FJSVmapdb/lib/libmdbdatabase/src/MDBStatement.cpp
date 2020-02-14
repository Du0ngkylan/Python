/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBStatement.cpp
 * @brief       Source file for implementing MDBStatement information.
 * @author      trangvp: create it for support MDBStatement code.
 * @date        2016/09/24
 */

#include "MDBStatement.h"

namespace mdbcom {
    /*!
     * @brief Default constructor of MDBStatement.
     * @param None.
     * @return None.
     */
    MDBStatement::MDBStatement() :
        mResult { nullptr }, mResultStatus { Result::NG }, sqlErrorCode { }
    {
    }

    /*!
     * @brief Default destructor of MDBStatement.
     * @param None.
     * @return None.
     */
    MDBStatement::~MDBStatement(void)
    {
        clear();
    }

    /*!
     * @brief Clear result resource.
     *
     * @param None.
     *
     * @return None.
     */
    inline void MDBStatement::clear(void)
    {
        PQclear(mResult);
        mResult = nullptr;
    }

    /*!
     * @brief Constructor.
     *
     * @param [in] result the result to set.
     * @param [in] resultCode the error code
     * @param [in] state Sql state of Posgres
     *
     * @return None.
     */
    MDBStatement::MDBStatement(PGresult *result, int resultCode, const std::string &errorCode) :
        mResult { result }, mResultStatus { resultCode }, sqlErrorCode { errorCode }
    {
    }

}

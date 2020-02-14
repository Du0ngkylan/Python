/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConnectionInfo.cpp
 * @brief       Source file for implementing MDBConnectionInfo information.
 * @author      trangvp: create it for support MDBConnectionInfo code.
 * @date        2016/10/26
 */

#include "MDBConnectionInfo.h"

namespace mdbcom {
    /*!
     * @brief Constructor.
     * @param None.
     * @return None.
     */
    MDBConnectionInfo::MDBConnectionInfo(void) :
        mUsername { }, mPassword { }, mDbname { }, mHost { }, mPort { 0 }, mAccessRetryNumber
    {
        0 }, mAccessWaitingTime { 0 }, mConnectionTimeOut { 0 }
    {

    }

    /*!
     * @brief Constructor.
     * @param [in] other the other MDBConnectionInfo.
     * @return None.
     */
    MDBConnectionInfo::MDBConnectionInfo(const MDBConnectionInfo &other) :
        mUsername { other.getUsername() }, mPassword { other.getPassword() }, mDbname
    {
        other.getDbname() }, mHost { other.getHost() }, mPort { other.getPort() }, mAccessRetryNumber
    {
        other.getAccessRetryNumber() }, mAccessWaitingTime
    {
        other.getAccessWaitingTime() }, mConnectionTimeOut
    {
        other.getConnectionTimeOut() }
    {

    }

    /*!
     * @brief Operator=.
     * @param [in] other the other MDBConnectionInfo.
     * @return the MDBConnectionInfo.
     */
    MDBConnectionInfo &MDBConnectionInfo::operator=(const MDBConnectionInfo &other)
    {
        if (this != &other)
        {
            mUsername = other.getUsername();
            mPassword = other.getPassword();
            mDbname = other.getDbname();
            mHost = other.getHost();
            mPort = other.getPort();
            mAccessRetryNumber = other.getAccessRetryNumber();
            mAccessWaitingTime = other.getAccessWaitingTime();
            mConnectionTimeOut = other.getConnectionTimeOut();
        }

        return *this;
    }
}


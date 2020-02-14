/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBMContextNotification.cpp
 * @brief   Extend function of MDBMContextNotification
 * @author  trunghn:
 * @date    2016/10/10
 */

#include "MDBMContextNotification.h"

namespace mdbcom {
    /*!
     * @brief Default Constructor
     *
     * Call default constructor of MDBMContextNotification
     *
     * @param None
     * @return None
     */
    MDBMContextNotification::MDBMContextNotification() :
        context { nullptr }
    {
    }

    /*!
     * @brief Context constructor
     *
     * Call constructor with default context
     *
     * @param [in] instance Address of MDBMContextNotification
     * @return None
     */
    MDBMContextNotification::MDBMContextNotification(const std::shared_ptr<MDBBackupInfo> &instance) :
        context { instance }
    {
    }

    /*!
     * @brief Context constructor
     *
     * Call constructor with default context
     *
     * @param [in] other Other instance of MDBMContextNotification
     * @return None
     */
    MDBMContextNotification::MDBMContextNotification(const MDBMContextNotification &other) :
        context { other.context }
    {
    }

    /*!
     * @brief Get
     *
     * Call default Destructor of MDBMContextNotification
     *
     * @param None
     * @return pointer to instance of MDBFileChannel
     */
    const std::shared_ptr<MDBBackupInfo> &MDBMContextNotification::get() const
    {
        return context;
    }

} // end of namespace

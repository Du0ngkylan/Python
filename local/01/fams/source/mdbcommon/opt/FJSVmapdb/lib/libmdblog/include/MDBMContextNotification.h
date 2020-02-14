/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBMContextNotification.h
 * @brief   Extend function of MDBMContextNotification
 * @author  trunghn:
 * @date    2016/10/10
 */

#ifndef LIBMDB_MDBCONTEXTNOTIFICATION_H_
#define LIBMDB_MDBCONTEXTNOTIFICATION_H_

#include <memory>

#include <Poco/NotificationQueue.h>

#include "MDBBackupInfo.h"

namespace mdbcom {
    /*! @class MDBMContextNotification
     *  @brief Data for communication with backup thread
     */
    class MDBMContextNotification: public Poco::Notification { /* pgr2252 */
    public:
        /*!
         * @brief Default Constructor
         *
         * Call default constructor of MDBMContextNotification
         *
         * @param None
         * @return None
         */
        MDBMContextNotification(void);

        /*!
         * @brief Context constructor
         *
         * Call constructor with default context
         *
         * @param [in] instance Address of MDBMContextNotification
         * @return None
         */
        explicit MDBMContextNotification(const std::shared_ptr<MDBBackupInfo> &instance);

        /*!
         * @brief Context constructor
         *
         * Call constructor with default context
         *
         * @param [in] other Other instance of MDBMContextNotification
         * @return None
         */
        MDBMContextNotification(const MDBMContextNotification &other);

        /*!
         * @brief Default Destructor
         *
         * Call default Destructor of MDBMContextNotification
         *
         * @param None
         * @return None
         */
        ~MDBMContextNotification(void) = default;

        /*!
         * @brief Get
         *
         * Call default Destructor of MDBMContextNotification
         *
         * @param None
         * @return pointer to instance of MDBFileChannel
         */
        const std::shared_ptr<MDBBackupInfo> &get(void) const;

        /**
         * @brief Copy assignment
         *
         * @param [in] other Assign value.
         * @return calling object.
         */
        MDBMContextNotification &operator=(const MDBMContextNotification &other);

    private:
        //! MDBMFile Instance for thread
        const std::shared_ptr<MDBBackupInfo> context;
    };
}

#endif

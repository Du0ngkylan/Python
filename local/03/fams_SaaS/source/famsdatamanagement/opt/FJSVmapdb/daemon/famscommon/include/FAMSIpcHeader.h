/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSIpcHeader.h
 * @brief       Header of class
 * @author      anhnguyen
 * @date        2018/12/11
 */

#ifndef FAMS_IPC_HEADER_H_
#define FAMS_IPC_HEADER_H_
#include <array>
#include <cstring>
#include <MDBLogger.h>
#include "FAMSCommon.h"
//#include "FAMSWaterReplaceData.h"

namespace famscommon {


    /*!
     * @class FAMSIpcHeader
     * @brief Base Data structure to transfer sensor information via IPC
     *        Only based on header, we can transform data to different data types
     */
    class FAMSIpcHeader {

    public:

        /*!
         * @brief Constructor.
         * @param [in] type Data type.
         * @param [in] id string.
         * @return None.
         */
        FAMSIpcHeader(MessageType type) :
            ipcType { type }
        {
        }

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~FAMSIpcHeader(void) = default;

        /*!
         * @brief Copy constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcHeader(const FAMSIpcHeader &) = default;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return This FAMSIpcHeader.
         */
        FAMSIpcHeader &operator=(const FAMSIpcHeader &) = default;

        /*!
         * @brief Move constructor.
         * @param None.
         * @return None.
         */
        FAMSIpcHeader(FAMSIpcHeader &&) = default;

        /*!
         * @brief Override default operator delete.
         * Override to keep the type consistency of delete behavior.
         *
         * @param [in] pointer Memory location to be deleted
         * @return None.
         */
        void operator delete (void *pointer) {
            mdb_apl_log_trace("[mem] ipcHeader free memory at %p", pointer);
            delete [] static_cast<char *>(pointer); /* pgr2215 */
        }

        /*!
         * @brief Move assignment.
         * @param None.
         * @return This FAMSIpcHeader.
         */
        FAMSIpcHeader &operator=(FAMSIpcHeader &&) = default;

        MessageType ipcType;
    };
}

#endif /* FAMS_IPC_HEADER_H_ */

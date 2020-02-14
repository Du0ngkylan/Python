/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBIPCShm.h
 * @brief   This file define a template allow to create shared memory
 *          based on input type.
 *          This IPC implemetation supports for Poco thread, pthread and std::thread of C++11
 * @author  trunghn:
 * @date    2016/9/12
 */

#ifndef LIBMDBIPC_MDBIPCSHM_H_
#define LIBMDBIPC_MDBIPCSHM_H_

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <future>
#include "MDBException.h"
#include "MDBLogger.h"
#include "MDBSharedMemoryUtility.h"
#include "MDBCommonUtility.h"
#include "MDBSharedHeader.h"

namespace mdbcom {

    /*!
     * @class MDBIPCShm
     * @brief Class shared memory common
     */
    class MDBIPCShm { /*pgr2204*/
    public:
        /*!
         * @class MessagePtr
         * @brief Pointer type to message returned by MDBIPCShm class
         */
        template<class T>
        using MessagePtr = std::unique_ptr<T, void(*)(void *)>;

        /*!
         * @brief IPC Shared memory constructor
         *
         * Set default value for MDBIPCShm class member,
         * Call to open shared memory
         *
         * @param [in] name Queue name
         * @param [in] capacity Maximum size of data in queue
         * @return None.
         *
         */
        MDBIPCShm(const std::string &name, unsigned long long capacity);

        /*!
         * @brief IPC Shared memory destructor
         * Do nothing. The shared memory & queue is removed when OS end.
         *
         * @param None.
         * @return None.
         *
         */
        ~MDBIPCShm(void) = default;

        /*!
         * @brief  Write data to shared memory
         * This function is thread-safe
         * This function write data to shared memory,
         *
         * @param [in] data data to write add to queue in shared memory
         * @param [in] size length of data will be written to queue
         *
         * @retval Result::NG write still fail after retry 3 times
         * @retval Result::OK normal
         */
        template<typename T>
        int writeData(const T *data, std::size_t size = sizeof(T)) {
            return write(reinterpret_cast<const char *>(data), size);
        }

        /*!
         * @brief  Write data to shared memory
         * This function is thread-safe
         * Specialization for fowarding data within one process
         * The life time of data pointed by "data" will be transfer to receiver side
         *
         * @param [in/out] data data to write add to queue in shared memory
         * @param [in] size length of data will be written to queue
         *
         * @retval Result::NG write still fail after retry 3 times
         * @retval Result::OK normal
         */
        template<typename T>
        int writeData(MessagePtr<T> &data, std::size_t size = sizeof(T)) {
            // set header to forward
            auto ret = write(reinterpret_cast<const char *>(data.get()), size, true); /*pgr0745*/

            if (Result::OK == ret) {
                // transfer message scope to receiver
                data.release(); /*pgr0519*/
            }

            return ret;
        }

        /*!
         * @brief  Read data from shared memory
         * This function is thread-safe
         * This function read data from shared memory data and write to specified buffer
         * Not recommended for forwarding messages case (refer to function "writeData")
         *
         * @param [out] data output data
         * @param [in/out] size read data size
         *
         * @retval Result::NG read data failed
         * @retval Result::OK normal
         *
         */
        template<typename T>
        int readData(T *data, std::size_t &size) { /*pgr0304*/
            if (nullptr == read(reinterpret_cast<char *>(data), size)) {
                return Result::NG;
            }

            return Result::OK;
        }

        /*!
         * @brief  Read data from shared memory
         * This function is thread-safe
         * This function read data from shared memory data,
         * The return type is basically std::unique_ptr with custom deleter,
         * therefore the return message will be clean up when it is out of scope.
         * User can release it early using reset() method of std::unique_ptr
         *
         * @param [in/out] size read data size
         *
         * @retval Pointer to received message
         *
         */
        template<typename T>
        MessagePtr<T> readData(std::size_t &size) { /*pgr0304*/
            auto result = read(nullptr, size); /*pgr0745*/
            return MessagePtr<T> {reinterpret_cast<T *>(result), MDBIPCShm::releaseIPC};
        }

        /*!
         * @brief  End the read data in the shared memory
         *
         * This function ends the reading of shared memory data
         * All reading threads waiting for data will wake up an return.
         *
         * @param none
         * @return None.
         *
         */
        void endRead(void);

        /*!
         * @brief Check reading status of current queue
         *
         * @param none
         *
         * @retval true Reading is ended.
         * @retval false Queue is being read.
         */
        bool isEnded(void) const;

        /*!
         * @brief Copy constructor
         *
         * @param  None.
         * @return None.
         *
         */
        MDBIPCShm(const MDBIPCShm &) = default; /*  pgr2222 */ /* pgr0571 */

        /*!
         * @brief Overide operator =
         *
         * @param None.
         * @return None.
         *
         */
        MDBIPCShm &operator =(const MDBIPCShm &) = default; /* pgr0055 *//* pgr2223 */ /* pgr0309 */

        /*!
         * @brief Move constructor
         *
         * Not supported
         *
         * @param  None.
         * @return None.
         *
         */
        MDBIPCShm(MDBIPCShm &&) = delete; /*  pgr2222 */ /* pgr0571 */

        /*!
         * @brief Overide operator =
         *
         * Not supported
         *
         * @param None.
         * @return None.
         *
         */
        MDBIPCShm &operator =(MDBIPCShm &&) = delete; /* pgr0055 *//* pgr2223 */ /* pgr0309 */

        /*!
         * @brief Unlink IPC
         *
         * Unlink mqueue and shm
         *
         * @param None.
         * @return None.
         *
         */
        void unlink(void);
    private:
        // Maximum retry time of IPC
        static constexpr long THRESHOLD_NANOSECONDS = 10000000; /* pgr0746 */
        //! Posix attribute structure
        using QUEUE_ATTRIBUTE = struct mq_attr;
        //! Configuration file for max message number
        static const std::string MSG_MAX_CONFIG;
        //! Configuration file for max message size
        static const std::string MSGSIZE_MAX_CONFIG;

        /*!
         * @brief Release shm queue memory
         *
         * Release message received from ipc shm queue
         *
         * @param [in] data Memory location
         * @return None.
         *
         */
        static void releaseIPC(void *data);

        //! Error string of shm creating
        enum SHM_ERR
        {
            ERR_INVALID_INPUT = 0,
            ERR_CANT_GET_SHM_SIZE,
            ERR_CANT_OPEN_MSQUEUE
        };

        //! User terminating flag
        bool endFlag;

        //! Name of shared mem
        std::string sharedName;
        //! Address of shared mem
        MDBSharedHeader *sharedData;

        //! Message queue handler
        mqd_t queue;

        //! Queue attribute
        QUEUE_ATTRIBUTE attribute;

        //! Smallsize definition of queue
        std::size_t smallSize;

        //! Data buffer for reading from queue
        std::unique_ptr<char []> headerBuffer; /*pgr0694*/

        /*!
         * @brief  Write data to queue
         *
         * @param [in] data output data
         * @param [in] size read data size
         * @param [in] forward Using forwading mechanism for inter threads
         *
         * @retval Result::NG read data failed
         * @retval Result::OK normal
         *
         */
        int write(const char *buffer, std::size_t size, bool forward = false);

        /*!
         * @brief  Read data from shared memory
         *
         * This function read data from shared memory data,
         * If buffer is null, this function will allocate memory buffer base on
         * received message and return pointer to this new buffer.
         * If buffer is not null, the return value will point to input buffer
         *
         * @param [in/out] buffer Buffer contain read message
         * @param [in/out] size Size of read message
         *
         * @retturn Pointer to buffer contain received data
         *
         */
        char *read(char *buffer, std::size_t &size);

        /*!
         * @brief  Waiting for incoming message
         *
         * @param [in] second Expecting header
         *
         * @retturn None
         *
         */
        void waitForMessage(unsigned int second);

        /*!
         * @brief  Preparing buffer base on received header
         *
         * @param [in/out] buffer Input buffer to write data
         * @param [out] size Total size of expecting message
         *
         * @retturn Buffer to write data
         *
         */
        bool consumeHeader(char **buffer, std::size_t &size);

    };

}

#endif

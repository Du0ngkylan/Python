/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBIPCShm.cpp
 * @brief   Implementation of shared message queue
 * @author  cuongadp
 * @date    2017/07/18
 */

#include <fstream>
#include "MDBIPCShm.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {
    //! Configuration file for max message number
    const std::string MDBIPCShm::MSG_MAX_CONFIG
    {
        "/proc/sys/fs/mqueue/msg_max"
    };
    //! Configuration file for max message size
    const std::string MDBIPCShm::MSGSIZE_MAX_CONFIG
    {
        "/proc/sys/fs/mqueue/msgsize_max"
    };

    /*!
     * @class MDBMessageInfo
     * @brief Meta infomation of shared memory message
     */
    class MDBMessageInfo {
    public:
        //! Total size of message
        std::size_t totalSize;
        //! Number of physical message need to be read
        std::size_t offspring;
    };

    /*!
     * @class MDBMessageBody
     * @brief Body structure of header message can contain the message information
     *        or the message data incase of Small object.
     */
    union MDBMessageBody
    {
        //! Message information
        MDBMessageInfo info;
        //! start of actual data
        char data[1];
    };

    /*!
     * @class MDBMessageType
     * @brief Type of header message
     */
    enum class MDBMessageType : std::int8_t
    {
        //! Header contain meta information only
        INFO,
        //! Header contain small object
        DATA,
        //! Header indicate fordwarded message
        FORWARD
    };

    /*!
     * @class MDBMessageHeader
     * @brief Header message
     */
    class MDBMessageHeader {
    public:
        //! Type of current header
        MDBMessageType type;
        //! Size of data contain in header (from "body")
        std::size_t bodySize;
        //! Actual body of header message
        MDBMessageBody body;

        /*!
         * @brief Constructor of header
         *
         * @param [in] type Header type
         * @param [in] size Size of header body
         * @return None.
         *
         */
        MDBMessageHeader(MDBMessageType type, std::size_t size = sizeof(MDBMessageBody))
            : type { type }, bodySize { size }, body{ 0, 0} {

        }

        /*!
         * @brief Allocation method for shared memory header
         *
         * @param [in] size Size of header
         * @param [in] extendSize Size of trailling data (body of header)
         * @return None.
         *
         */
        static void *operator new (std::size_t size, std::size_t extendSize = 0) {
            // Allocate atleast 1 byte more (of body.data[1]) than actual needed
            auto result = new char[size + extendSize];/*pgr0745*/ /*pgr1163*/
            mdb_apl_log_trace("[mem] MDBMessageHeader allocate %lu bytes at %p", size + extendSize, result);
            return result;
        }

        /*!
         * @brief Deallocation method for shared memory header
         *
         * @param [in] pointer Pointer to shared message header
         * @return None.
         *
         */
        static void operator delete (void *pointer) {
            mdb_apl_log_trace("[mem] free memory at %p", pointer);
            delete [] static_cast<char *>(pointer);/*pgr2215*/
        }
    };

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
    MDBIPCShm::MDBIPCShm(const std::string &name, unsigned long long capacity) :
        endFlag { false },
        sharedName { "/" + name }, sharedData { nullptr },
        queue { },
        attribute { 0, 0, 0, 0, {0, 0, 0, 0}},
    smallSize { 0 }, headerBuffer { }
    {
        mdb_apl_log_start();
        // Check input validation
        auto shareName = name.c_str(); /* pgr0745 */

        if ((true == name.empty()) || (0 >= capacity))
        {
            mdb_apl_log_critical("Invalid input!");
            mdb_sys_log_out("F000302", ERR_INVALID_INPUT, shareName, capacity);
            throw MDBException {"Invalid input!"};
        }

        std::ifstream sysConfig { };
        auto sysConfigValue = std::string { }; /* pgr0745 */
        // get max message number
        sysConfig.open(MSG_MAX_CONFIG);

        if (!std::getline(sysConfig, sysConfigValue))
        {
            auto strSysConfig = MSG_MAX_CONFIG.c_str();/* pgr0745 */
            mdb_apl_log_critical("Can't load file [%s]", strSysConfig);
            throw MDBException { "Can't get maximum message number."};
        }

        auto maxMsgConfig = std::atol(sysConfigValue.c_str());/* pgr0745 *//*pgr1170*/
        sysConfig.close();
        // get max message size
        sysConfig.open(MSGSIZE_MAX_CONFIG);

        if (!std::getline(sysConfig, sysConfigValue))
        {
            auto strSysConfig = MSGSIZE_MAX_CONFIG.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Can't load file [%s]", strSysConfig);
            throw MDBException { "Can't get maximum message size." };
        }

        auto strSysConfigValue = sysConfigValue.c_str();/* pgr0745 */
        auto maxMsgSizeConfig = std::strtoull(strSysConfigValue, nullptr, 10);/* pgr0745 */
        sysConfig.close();

        if (maxMsgSizeConfig < capacity)
        {
            mdb_apl_log_critical("Request message queue size (%llu) is bigger than system (%ld bytes)",
                                 capacity, maxMsgSizeConfig);
            mdb_sys_log_out("F000302", ERR_INVALID_INPUT, shareName, capacity);
            throw MDBException { "System config can not afford requested queue size" };
        }

        // With given capacity, prefer smaller block size
        // in order to have better performace of allocation
        attribute.mq_maxmsg = maxMsgConfig;
        auto calMsgSize = std::ceil(static_cast<double>(capacity));/* pgr0745 */
        // Minimum size of message must be at least size of MDBMessageHeader
        attribute.mq_msgsize = std::max(static_cast<std::size_t>(calMsgSize), sizeof(MDBMessageHeader));

        mdb_apl_log_info("Created queue with actual capacity: %ld messages * %ld bytes",
                         attribute.mq_maxmsg, attribute.mq_msgsize);
        // Size of body incase of attribute.mq_msgsize = sizeof(MDBMessageHeader)
        smallSize = attribute.mq_msgsize - offsetof(MDBMessageHeader, body);
        mdb_apl_log_info("Small size optimization will be applied for message less than %ld bytes",
                         smallSize);

        mdb_apl_log_debug("shared name: [%s], queue size [%d]", shareName, attribute.mq_maxmsg);

        auto openMode = ShmModes::CREATE; /* pgr0745 */
        mdb_apl_log_debug("shared memory size: [%u]", sizeof(MDBSharedHeader));

        // Open and map shared memory
        auto sharedAddress = MDBSharedMemoryUtility::getShm(sharedName,/* pgr0745 */
                             sizeof(MDBSharedHeader), openMode);

        if (nullptr == sharedAddress)
        {
            mdb_apl_log_critical("Cannot get shared memory size!");
            mdb_sys_log_out("F000302", ERR_CANT_GET_SHM_SIZE, shareName, sizeof(MDBSharedHeader));
            throw MDBException {"Cannot get shared memory size!"};
        }

        if (ShmModes::CREATE == openMode)
        {
            // placement new
            sharedData = new (sharedAddress) MDBSharedHeader { };
        }
        else
        {
            sharedData = static_cast<MDBSharedHeader *>(sharedAddress);
        }

        errno = 0;
        mdb_apl_log_info("Creating queue with flag 0x%X (O_RDWR | O_CREAT | O_NONBLOCK), mode 0x%X (S_IRWXU)",
                         O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU);
        queue = mq_open(sharedName.c_str(), O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, &attribute);

        if (0 > queue)
        {
            // Error string buffer
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errMsg = strerror_r(errno, errBufData, errBufSize);/* pgr0745 */

            mdb_apl_log_critical("Can't open message queue [errno:%d][%s]", /*pgr0512*/
                                 errno, errMsg);
            mdb_sys_log_out("F000302", ERR_CANT_OPEN_MSQUEUE, shareName, capacity);
            throw MDBException {"Can't open message queue"};
        }

        headerBuffer = MDBMemoryUtility::make_unique<char []> (attribute.mq_msgsize); /*pgr0694*/
        mdb_apl_log_end();
    }

    /*!
     * @brief  Read data from shared memory
     *
     * This function read data from shared memory data,
     *
     * @param [in] data output data
     * @param [in] size read data size
     * @param [in] forward Using forwading mechanism for inter threads
     *
     * @retval Result::NG read data failed
     * @retval Result::OK normal
     *
     */
    int MDBIPCShm::write(const char *buffer, std::size_t size, bool forward)
    {
        mdb_apl_log_start();
        // Return of function
        auto ret = Result::NG; /* pgr0745 */
        // Retry counter
        auto retryCount = 0u; /* pgr0745 */
        // Error string buffer
        auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
        auto errBufData = errBuf.data(); /* pgr0745 */
        auto errBufSize = errBuf.size(); /* pgr0745 */

        std::size_t maxMsgSize = attribute.mq_msgsize;
        auto sentHeader = false;/* pgr0745 */
        auto header = std::unique_ptr<MDBMessageHeader> {} ;/* pgr0745 */
        auto headerSize = sizeof(MDBMessageHeader);/* pgr0745 */

        if (true == forward)
        {
            mdb_apl_log_trace("Forwarding exist message [%u]", size);
            header = std::unique_ptr<MDBMessageHeader> {new MDBMessageHeader {MDBMessageType::FORWARD, size}};
            // Send pointer value
            std::memcpy(header->body.data, &buffer, sizeof(void *)); /* pgr0579 */ /* pgr1170 */
            mdb_apl_log_debug("Forward size %u", sizeof(void *));
        }
        else if (smallSize >= size)
        {
            auto extendSize = std::max(static_cast<std::size_t>(0),/* pgr0745 */
                                       size - sizeof(MDBMessageBody));
            mdb_apl_log_info("Embedded small object to header [%u]", size);
            header = std::unique_ptr<MDBMessageHeader>
            {
                new (extendSize) MDBMessageHeader {MDBMessageType::DATA, size}
            };
            std::memcpy(header->body.data, buffer, size); /* pgr0579 */ /* pgr1170 */
            headerSize += extendSize;
            mdb_apl_log_info("Small object size %u", header->bodySize);
        }
        else // info part of a multi-part message will go here
        {
            header = std::unique_ptr<MDBMessageHeader> {new MDBMessageHeader {MDBMessageType::INFO}};
            header->body.info.totalSize = size;
            header->body.info.offspring = std::ceil(static_cast <double>(size) / maxMsgSize);
            mdb_apl_log_debug("Number of message to send %u, total size %u",
                              header->body.info.offspring, header->body.info.totalSize);
        }

        auto msgCount = 0u;/* pgr0745 */
        auto writePtr = reinterpret_cast<const char *>(header.get());/* pgr0745 */
        auto writeSize = headerSize;/* pgr0745 */

        do
        {
            if (false == sentHeader)
            {
                mdb_apl_log_trace("Start sending header [%d]", sentHeader);
                static_cast<void>(sharedData->write.lock());
            }

            mdb_apl_log_debug("Send [0x%p][size: %u]", writePtr, writeSize);
            // send header
            errno = 0;
            mq_attr attri;
            mq_getattr(queue, &attri);
            mdb_apl_log_info("m_queue (%s) before send [mq_msgsize: %d, mq_curmsgs: %d], init mq_msgsize %d", sharedName.c_str(), attri.mq_msgsize, attri.mq_curmsgs, attribute.mq_msgsize);
            errno = 0;
            auto mqRet = mq_send(queue, writePtr, writeSize, 0);/* pgr0745 */

            if ((0 == mqRet)
                    && ((MDBMessageType::DATA == header->type) // last part of a multi-part message
                        || (MDBMessageType::FORWARD == header->type))) // forward message
            {
                // DONE sending
                ret = Result::OK;
                // unlock for other writer
                static_cast<void>(sharedData->write.unlock());
                break;
            }

            if (0 == mqRet) // multi-part message: info part or not-last-part will go here
            {
                if (false == sentHeader) // multi-part message: info part
                {
                    sentHeader = true;
                    mdb_apl_log_trace("Start sending data messages");
                    // reset retry for data messages
                    retryCount = 0u;
                    writePtr = buffer;
                    writeSize = std::min(size, maxMsgSize);
                    // continue sending data
                    continue;
                }

                if ((msgCount + 1) == header->body.info.offspring) // multi-part message: last part
                {
                    // DONE sending
                    ret = Result::OK;
                    // unlock for other writer
                    static_cast<void>(sharedData->write.unlock());
                    break;
                }

                // done 1 part
                msgCount++;
                // reset retry count for message data
                retryCount = 0u;
                // update write pointer
                writePtr += writeSize;
                // remain size to write
                size -= writeSize;
                writeSize = std::min(size, maxMsgSize);
                mdb_apl_log_debug("Sent %u messages", msgCount);
                continue;
            }

            auto errMsg = strerror_r(errno, errBufData, errBufSize);/* pgr0745 */
            mdb_apl_log_critical("Fail to send header [errno:%d][%s]", /*pgr0512*/
                                 errno, errMsg);

            if (false == sentHeader)
            {
                // Retry sending header
                // unlock for other writer
                static_cast<void>(sharedData->write.unlock());
            }

            // Add data to queue for comunication
            mdb_apl_log_info("retryCount: [%d]", retryCount);

            if (Result::OK != MDBCommonUtility::sleepNano(Ipc::RETRY_WAIT_TIME_SECOND, 0))
            {
                mdb_apl_log_warn("sleepNano failed");
            }

            ++retryCount;
            /** If cannot enqueueData the retry 2 times to communicate(total 3 times) */
        }
        while (Ipc::MAX_RETRY_TIMES >= retryCount);  /* pgr0697 */

        if (Result::OK != ret)
        {
            // After retry 3 times to add data to queue is failed
            ret = Result::RETRY_IPC_FAILED;
            mdb_apl_log_critical("Retry to communicate failed: [%d]", retryCount);
            mdb_sys_log_out("F000301");

            if (true == sentHeader)
            {
                // Fail while sending data message
                static_cast<void>(sharedData->write.unlock());
            }
        }

        mdb_apl_log_info("out ret = [%d]", ret);
        mdb_apl_log_end();
        return ret;
    }

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
    char *MDBIPCShm::read(char *buffer, std::size_t &size) /*pgr0304*/
    {
        mdb_apl_log_start();
        // Return of function
        auto ret = static_cast<char *>(nullptr); /* pgr0745 */
        // Retry counter
        auto retryCount = 0u; /* pgr0745 */
        // Error string buffer
        auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
        auto errBufData = errBuf.data(); /* pgr0745 */
        auto errBufSize = errBuf.size(); /* pgr0745 */

        auto hasHeader = false;/* pgr0745 */
        auto hasLock = false;/* pgr0745 */
        auto header = reinterpret_cast<MDBMessageHeader *>(headerBuffer.get());/* pgr0745 */
        auto msgCount = 0u;/* pgr0745 */
        auto readPtr = headerBuffer.get();/* pgr0745 */
        auto allocBuffer = false;/* pgr0745 */

        do
        {
            if ((false == hasHeader) && (false == hasLock))
            {
                mdb_apl_log_trace("Start receive header [%d]", hasHeader);
                // Lock other reader
                static_cast<void>(sharedData->read.lock());
                hasLock = true;
            }

            if (true == endFlag)
            {
                mdb_apl_log_info("End reading by user signal");

                // incase thread wake up from read.locK() or waitForMessage()
                if (true == hasLock)
                {
                    static_cast<void>(sharedData->read.unlock());
                    hasLock = false;
                }

                // DONE reading
                break;
            }

            // read data
            errno = 0;
            mq_attr attri;
            mq_getattr(queue, &attri);
            mdb_apl_log_info("m_queue (%s) before receive [mq_msgsize: %d, mq_curmsgs: %d]", sharedName.c_str(), attri.mq_msgsize, attri.mq_curmsgs);
            errno = 0;
            auto mqRet = mq_receive(queue, readPtr, attribute.mq_msgsize, nullptr);/* pgr0745 */

            if ((0 > mqRet) && (EAGAIN == errno))
            {
                mdb_apl_log_trace("Queue is empty, wait for message ");
                // queue is empty, wait for incoming message
                waitForMessage(Ipc::RETRY_WAIT_TIME_SECOND);
                // Message arrived
                mdb_apl_log_trace("Message arrived, continue processing");
                // Not count retry times for empty case
                continue;
            }

            if (0 < mqRet)
            {
                if (false == hasHeader) // read header info (body part of a multi-part message will not go here)
                {
                    // has header and keep the lock when retry
                    hasHeader = true;
                    mdb_apl_log_trace("Start processing header");
                    // if buffer is not provided, consumeHeader() will prepare one.
                    allocBuffer = (nullptr == buffer);
                    auto done = consumeHeader(&buffer, size);/* pgr0745 */

                    if (true == done) // single-part message or body part of a multi-part message will go here
                    {
                        // message is done
                        ret = buffer;
                        static_cast<void>(sharedData->read.unlock());
                        hasLock = false;
                        break;
                    }
                    else
                    {
                        // info part of a multi-part message will go here
                        readPtr = buffer; // set readPtr to start address of buffer
                        // reset retry count
                        retryCount = 0u;
                        // continue receiving data
                        continue;
                    }
                }

                // done 1 part
                msgCount++;
                // reset retry count for message data
                retryCount = 0u;
                // update read pointer to next position (mqRet = no.of.byte)
                readPtr += mqRet;
                mdb_apl_log_debug("Received %u messages", msgCount);

                // header->body.info.offspring: number of parts of a multi-part message
                if (msgCount == header->body.info.offspring) /* pgr0360 */
                {
                    ret = buffer;
                    // unlock for other reader
                    static_cast<void>(sharedData->read.unlock());
                    hasLock = false;
                    // DONE reading
                    break;
                }

                continue;
            }

            auto errMsg = strerror_r(errno, errBufData, errBufSize);/* pgr0745 */
            mdb_apl_log_warn("Fail to receive data [errno:%d][%s]",/*pgr0512*/
                             errno, errMsg);

            // unlock and retry
            if (false == hasHeader)
            {
                // Incase reading header fail, retry without hodling the lock
                // unlock for other reader
                static_cast<void>(sharedData->read.unlock());
                hasLock = false;
            }

            mdb_apl_log_debug("retryCount: [%d]", retryCount);

            if (Result::OK != MDBCommonUtility::sleepNano(Ipc::RETRY_WAIT_TIME_SECOND, 0))
            {
                mdb_apl_log_warn("sleepNano failed");
            }

            ++retryCount;
            /** If cannot enqueueData the retry 2 times to communicate(total 3 times) */
        }
        while (Ipc::MAX_RETRY_TIMES >= retryCount);    /* pgr0697 */

        if ((nullptr == ret) && (false == endFlag))
        {
            // After retry 3 times to add data to queue is failed
            mdb_apl_log_critical("Retry to communicate failed: [%d]", retryCount);
            mdb_sys_log_out("F000301");

            if (true == hasHeader)
            {
                // Fail while reading data message
                static_cast<void>(sharedData->read.unlock());
                hasLock = false;
            }
        }

        if ((nullptr == ret) && (true == allocBuffer))
        {
            mdb_apl_log_trace("[mem] free at %p", buffer);
            delete [] buffer;
        }

        mdb_apl_log_debug("out ret = [%p]", ret);
        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief  Waiting for incoming message
     *
     * @param [in] second Expecting header
     *
     * @retturn None
     *
     */
    void MDBIPCShm::waitForMessage(unsigned int second)
    {
        mdb_apl_log_start();
        auto selectRet = 0;/* pgr0745 */
        auto timeout = timeval  {second, 0};/* pgr0745 */
        fd_set readDescSet; /*pgr1163*/

        try
        {
            while (false == endFlag)
            {
                FD_ZERO(&readDescSet); /* pgr0360 *//*pgr0517*//*pgr0650*/
                FD_SET(queue, &readDescSet); /*pgr0654*/
                selectRet = select(queue + 1, &readDescSet, nullptr, nullptr, &timeout);

                if (selectRet < 0) /*pgr0607*/
                {
                    mdb_apl_log_critical("Error selecting ");
                }
                else if (FD_ISSET(queue, &readDescSet)) /*pgr0654*/
                {
                    // has data
                    mdb_apl_log_end();
                    return;
                }

                // no data, give up CPU
                if (Result::OK != MDBCommonUtility::sleepNano(0, THRESHOLD_NANOSECONDS))
                {
                    mdb_apl_log_warn("sleepNano failed");
                    // fallback solution to avoid starvation
                    std::this_thread::yield();
                }
            }
        }
        catch (const std::exception &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("Exception: %s", exDetail);
        }
        catch (...)
        {
            mdb_apl_log_critical("Unknown exception");
        }

        mdb_apl_log_info("End waiting due to signal");
        mdb_apl_log_end();
    }

    /*!
     * @brief  Preparing buffer base on received header
     *
     * @param [in/out] buffer Input buffer to write data
     * @param [out] size Total size of expecting message
     *
     * @retturn Buffer to write data
     *
     */
    bool MDBIPCShm::consumeHeader(char **buffer, std::size_t &size)
    {
        mdb_apl_log_start();
        auto header = reinterpret_cast<MDBMessageHeader *>(headerBuffer.get()) ;/* pgr0745 */

        if (MDBMessageType::DATA == header->type)
        {
            // this message contains entire data (single-part message)
            // or this is a part of a multi-part message
            mdb_apl_log_trace("Receive header with small object");

            // small object data
            if (nullptr == *buffer)
            {
                *buffer = new char[header->bodySize]; /*pgr1163*/
                mdb_apl_log_trace("[mem] allocate %lu bytes at %p", header->bodySize, buffer);
            }

            std::memcpy(*buffer, header->body.data, header->bodySize); /* pgr0579 */ /* pgr1170 */
            size = header->bodySize;
            mdb_apl_log_debug("Small object size %u", header->bodySize);
            mdb_apl_log_end();
            return true ;
        }

        if (MDBMessageType::FORWARD == header->type) // this message will be forwarded into another queue
        {
            mdb_apl_log_trace("Receive forwarded message");
            void *message = nullptr;
            std::memcpy(&message, header->body.data, sizeof(void *)); /* pgr0579 */ /* pgr1170 *//*pgr0548*/
            size = header->bodySize;

            if (nullptr != *buffer)
            {
                // this case is same as MDBMessageType::DATA, copy entire message body to buffer
                std::memcpy(*buffer, message, header->bodySize); /* pgr0579 */ /* pgr1170 */
                mdb_apl_log_trace("[mem] free at %p", message);
                delete [] static_cast<char *>(message);/*pgr2215*/
            }
            else
            {
                // because this message will be forwarded to another queue, no need to copy, just cast pointer (pointed to headerBuffer->body.data which is allocated at IPCShm started) to *buffer
                // later mq_send() will copy from *buffer to it's internal mem
                *buffer = static_cast<char *>(message);/*pgr2215*/
                mdb_apl_log_debug("Object size %u", header->bodySize);
            }

            // DONE reading
            mdb_apl_log_end();
            return true;
        }

        if (MDBMessageType::INFO == header->type)
        {
            // data is divided into many messages (multi-part message), and this is the header only (no data -> no copy, just allocate enough space for *buffer)
            mdb_apl_log_trace("Receiving data messages");
            size = header->body.info.totalSize;
            mdb_apl_log_debug("Number of message to receive %u, total size %u",
                              header->body.info.offspring, header->body.info.totalSize);

            if (nullptr == *buffer)
            {
                *buffer = new char[header->body.info.totalSize]; /*pgr1163*/
                mdb_apl_log_trace("[mem] allocate %lu bytes at %p", header->body.info.totalSize, *buffer);
            }

            mdb_apl_log_end();
            return false; // message is not done (not all messages are received)
        }

        mdb_apl_log_critical("Should not go here !");
        mdb_apl_log_end();
        return false;
    }

    /*!
     * @brief Release shm queue memory
     *
     * Release message received from ipc shm queue
     *
     * @param [in] data Memory location
     * @return None.
     *
     */
    void MDBIPCShm::releaseIPC(void *data)
    {
        mdb_apl_log_trace("[mem] free memory at %p", data);
        delete [] static_cast<char *>(data); /*pgr2215*/
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
    void MDBIPCShm::endRead(void)
    {
        mdb_apl_log_start();
        // Set global variable to pass the Spurious Wakeup preventing
        endFlag = true;
        auto sharedNameStr = sharedName.c_str();/*pgr0745*/
        mdb_apl_log_info("Set end flag [%s][%d]", sharedNameStr, endFlag);
        mdb_apl_log_end();
    }

    void MDBIPCShm::unlink(void)
    {
        auto sharedNameStr = sharedName.c_str();/*pgr0745*/
        mdb_apl_log_info("unlink mqueue %s", sharedNameStr);
        auto ret = mq_unlink(sharedNameStr);/*pgr0745*/

        if (ret != 0)
        {
            mdb_apl_log_warn("unlink mqueue NG, errno: %d", errno); /*pgr0512*/
        }

        mdb_apl_log_info("unlink shm %s", sharedNameStr);
        ret = MDBSharedMemoryUtility::unlink(sharedName);

        if (ret != 0)
        {
            mdb_apl_log_warn("unlink shm NG, errno: %d", errno); /*pgr0512*/
        }

    }

    /*!
     * @brief Check reading status of current queue
     *
     * @param none
     *
     * @retval true Reading is ended.
     * @retval false Queue is being read.
     */
    bool MDBIPCShm::isEnded(void) const
    {
        return endFlag;
    }

}

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBHTTPServerConfig.h
 * @brief Definition for MDBHTTPServerConfig
 * @author trangvp:
 * @date 2016/12/05
 */

#ifndef LIBMDBHTTP_MDBHTTPAPPCONFIG_H_
#define LIBMDBHTTP_MDBHTTPAPPCONFIG_H_

#include "MDBConfigFile.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {
    /*!
     * @class MDBHTTPServerConfig
     * @brief Common setting for a HTTP server application
     */
    class MDBHTTPServerConfig {
    public:
        /*!
         * @brief Constructor
         * @param [in] configPath the configuration path
         * @return None.
         */
        explicit MDBHTTPServerConfig(const std::string &configPath);

        /*!
         * @brief Copy constructor.
         * @param [in] other the other config instance
         * @return None.
         */
        MDBHTTPServerConfig(const MDBHTTPServerConfig &other) :
            mConfigFile { nullptr }, mServerPort { 0 }, mMaxThreads { 0 },
            mMaxQueueds { 0 }, mStackSize{ 0 } {
            if (nullptr != other.mConfigFile) {
                mConfigFile = MDBMemoryUtility::make_unique<MDBConfigFile>(*other.mConfigFile);
            }

            mServerPort = other.mServerPort;
            mMaxThreads = other.mMaxThreads;
            mMaxQueueds = other.mMaxQueueds;
            mStackSize = other.mStackSize;
        }

        /*!
         * @brief Copy assignment.
         * @param [in] other the other config instance
         * @return This MDBHTTPServerConfig.
         */
        MDBHTTPServerConfig &operator=(const MDBHTTPServerConfig &other) {
            if (this != &other) {
                if (nullptr != other.mConfigFile) {
                    mConfigFile = MDBMemoryUtility::make_unique<MDBConfigFile>(*other.mConfigFile);
                }

                mServerPort = other.mServerPort;
                mMaxThreads = other.mMaxThreads;
                mMaxQueueds = other.mMaxQueueds;
                mStackSize = other.mStackSize;
            }

            return *this;
        }

        /*!
         * @brief Destructor
         * @param None.
         * @return None.
         */
        virtual ~MDBHTTPServerConfig(void) = default;

        /*!
         * @brief Get server port
         * @param None.
         * @return the server port
         */
        inline unsigned short getServerPort(void) const {
            return mServerPort;
        }

        /*!
         * @brief get server max thread
         * @param None.
         * @return the max thread of server
         */
        inline int getMaxThreads(void) const {
            return mMaxThreads;
        }

        /*!
         * @brief get server max queued requests
         * @param None.
         * @return the max request of server that can be queued
         */
        inline int getMaxQueueds(void) const {
            return mMaxQueueds;
        }

        /*!
         * @brief Get stack size for http server threads
         * @param None.
         * @return stackSize Minimum stack size for http server threads
         */
        inline int getStackSize(void) const {
            return mStackSize;
        }

        /*!
         * @brief Set server port
         * @param [in] port the web server port
         * @return None.
         */
        inline void setServerPort(unsigned short port) {
            mServerPort = port;
        }

        /*!
         * @brief Set max thread that server will open
         * @param [in] maxThreads the max thread that server will open
         * @return None.
         */
        inline void setMaxThreads(int maxThreads) {
            mMaxThreads = maxThreads;
        }

        /*!
         * @brief Set max queued request that server will queue
         * @param [in] maxQueueds max queued request that server will enqueue
         * @return None.
         */
        inline void setMaxQueueds(int maxQueueds) {
            mMaxQueueds = maxQueueds;
        }

        /*!
         * @brief Setting stack size for http server threads
         * @param [in] stackSize Minimum stack size for http server threads
         * @return None.
         */
        inline void setStackSize(int stackSize) {
            mStackSize = stackSize;
        }
    protected:
        //! The config pointers
        std::unique_ptr<MDBConfigFile> mConfigFile;
    private:
        //! constant for HTTP server default port
        static constexpr unsigned short HTTP_SERVER_DEFAULT_PORT = 8080;

        //! constant for HTTP server maximum thread
        static constexpr int HTTP_SERVER_MAX_THREADS = 440;

        //! stack size of http threads: 512 KB
        static constexpr int HTTP_SERVER_DEFAULT_STACK_SIZE = 512 * 1024;

        //! constant for HTTP server port string
        static const std::string HTTP_SERVER_PORT_STR;

        //! constant for HTTP server maximum thread string
        static const std::string HTTP_SERVER_MAX_THREAD_STR;

        //! constant for HTTP server maximum queue string
        static const std::string HTTP_SERVER_MAX_QUEUED_STR;

        //! constant for HTTP server maximum queue string
        static const std::string HTTP_SERVER_STACK_SIZE_STR;

        //! web server port
        unsigned short mServerPort;

        //! maximum of threads served requests
        int mMaxThreads;

        //! maximum request is queued
        int mMaxQueueds;

        //! Server thread stack size
        int mStackSize;
    };
}

#endif /* LIBMDBHTTP_MDBHTTPAPPCONFIG_H_ */

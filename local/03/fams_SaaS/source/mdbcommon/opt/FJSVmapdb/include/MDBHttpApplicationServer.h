/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBHttpApplicationServer.h
 * @brief       Header file for define MDBHttpApplicationServer class.
 * @author      hunglt: Creating for supporting define MDBHttpApplicationServer class.
 * @date        2016/12/12
 */

#ifndef LIBMDBPROC_HTTPAPPLICATIONSERVER_H_
#define LIBMDBPROC_HTTPAPPLICATIONSERVER_H_

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPServer.h>
#include "MDBApplication.h"
#include "MDBConstants.h"
#include "MDBMemoryUtility.h"
#include "MDBLogger.h"
#include "MDBException.h"
#include "MDBCommonUtility.h"

#include "MDBHTTPServerConfig.h"
#include "MDBResourceMapper.h"

namespace mdbcom {

    /*! @class MDBHttpApplicationServer
     *  @brief Web server for data delivery management
     */
    class MDBHttpApplicationServer: public MDBApplication { /*pgr2252*/
        /*pgr2222*/

    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        explicit MDBHttpApplicationServer(const std::string &processName);

        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBHttpApplicationServer(const MDBHttpApplicationServer &) = delete; /*pgr0571*/ /*pgr2222*/
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBHttpApplicationServer &operator=(const MDBHttpApplicationServer &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        virtual ~MDBHttpApplicationServer(void) = default;

        /*!
         * @brief handle Sig RealTime for MDBHttpApplicationServer.
         *
         * @param [in] signum the signum
         * @param [in] args argument list when handler is called (received SIG REALTIME)
         * @return return code of handler processing.
         */
        int handleSigRealTime(const int signum, const void *arg) override;

        /*!
         * @brief Get pointer to http server
         * @param None.
         * @return pointer to http server
         */
        inline Poco::Net::HTTPServer *getHttpServerPtr(void) {
            return pServer.get();
        }
    protected:
        /*!
         * @brief The method to get the configuration for HttpServer.
         * @param None.
         * @return Reference to configuration object
         */
        virtual const MDBHTTPServerConfig &getWebServerConfig(void) = 0;

        /*!
         * @brief The method perform processing logic of process
         * @param None.
         * @retval ExitCode::OK Success
         * @retval ExitCode::NG Fail
         */
        int execute(const std::vector<std::string> &args);

        //! Handler factory for creating handler object
        std::unique_ptr<Poco::Net::HTTPRequestHandlerFactory> mHandlerFactory;

        //! Resource objects holder
        std::unique_ptr<MDBResourceMapper> mResources;

    private:
        //! Wait for terminiating timer (in miliseconds)
        static constexpr int WAIT_FOR_TERMINATING_TIMER = 10000;
        //! pointer to server
        std::unique_ptr<Poco::Net::HTTPServer> pServer;

        /*!
         * @brief handle stopping logic for http server.
         *
         * @param None.
         * @retval Result::OK Success
         * @retval Result::NG Fail
         */
        int stopServer(void);

        /*!
         * @brief get encrypted time saved in file.
         *
         * @param None.
         * @retval Result::OK Success
         * @retval Result::NG Fail
         */
        int readEncryptedFile(void);
    };

} /* namespace MapDB */

#endif /* LIBMDBPROC_HTTPAPPLICATIONSERVER_H_ */

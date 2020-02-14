/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBApplication.h
 * @brief       Header file of class MDBApplication.
 * @author      hunglt
 * @date        2016/12/12
 */

#ifndef LIBMDBPROC_APPLICATION_H_
#define LIBMDBPROC_APPLICATION_H_
#include <fstream>
#include "MDBTypes.h"
#include "MDBConstants.h"
#include "MDBMemoryUtility.h"
#include <Poco/Util/ServerApplication.h>

namespace mdbcom {
    /*!
     * @class MDBApplication
     * @brief Common application for Mobility
     */
    class MDBApplication: public Poco::Util::ServerApplication { /* pgr2252 */
    public:
        /*!
         * @brief Constructor
         * @param None.
         * @return None.
         */
        explicit MDBApplication(const std::string &processName);

        /*!
         * @brief Destructor
         * @param None.
         * @return None.
         */
        virtual ~MDBApplication(void) = default;

        /*!
         * @brief Copy constructor
         * @param None.
         * @return None.
         */
        MDBApplication(const MDBApplication &) = default; /* pgr2222 */ /* pgr0571 */

        /*!
         * @brief default SIG REALTIME handler for MDBApplication (use SIGRTMIN + 1)
         *
         * @param [in] signum the signum
         * @param [in] args argument list when handler is called (received SIG REALTIME)
         * @return return code of handler processing.
         */
        virtual int handleSigRealTime(const int signum, const void *arg);

        /*!
         * @brief Copy assignment
         * @param None.
         * @return This MDBApplication
         */
        MDBApplication &operator=(const MDBApplication &) = default; /* pgr0055 *//* pgr0309 */ /* pgr2223 */

    protected:
        /*!
         * @brief The main method that POCO will call when application is started.
         *
         * @param [in] args argument list when run application
         * @return return code of application
         */
        int main(const std::vector<std::string> &args);

        /*!
         * @brief The method does initialization when server is started.
         * @param None.
         * @retval true Initialization process successed
         * @retval false Initialization process fail
         */
        virtual bool initializeApplication(void) = 0;

        /*!
         * @brief The method does uninitialization
         * @param None.
         * @retval true uninitialize successfully.
         * @retval false uninitialize failed.
         */
        virtual bool uninitializeApplication(void) = 0;

        /*!
         * @brief The method perform processing logic of process
         * @param None.
         * @retval ExitCode::OK Successed
         * @retval ExitCode::NG Fail
         */
        virtual int execute(const std::vector<std::string> &args) = 0;

        /*!
         * @brief The method to help daemon, cron main() thread waiting for signal
         * @param None.
         * @retval Signal::OK If success.
         * @retval Signal::_ERROR_THREAD_JOIN_FAILED If "Signal Handler Thread" can not joined.
         */
        virtual int waitForSignalHandler(void);

        //! Process name
        const std::string mProcessName;

    private:
        // To check if SignalHadler thread is started or not
        bool isSignalHandlerStarted;

        /*!
         * @brief Do common logic for initialization
         * @param None.
         * @retval true Initialization process successed
         * @retval false Initialization process fail
         */
        bool initializeInternal(void);

        /*!
         * @brief Do common logic for un-initialization
         * @param [in] isInitialized flag to check initialization is normally or not.
         * @retval true Un-initialization process successed
         * @retval false Un-initialization process fail
         */
        bool uninitializeInternal(bool isInitialized);
    };
}

#define MIOT_MAINAPP(App) /* pgr2216 */                         \
int main(int argc, char* argv[]) {                              \
    if (nullptr == argv) {                                      \
        return mdbcom::ExitCode::NG;                             \
    }                                                           \
    auto result = mdbcom::ExitCode::OK; /* pgr0745 */            \
    std::ifstream comm { mdbcom::Process::NAME_PATH };           \
    auto processName = std::string { }; /* pgr0745 */           \
    if (!std::getline(comm, processName)) {                     \
        processName = argv[0];                                  \
    }                                                           \
    Poco::AutoPtr<App> app = new App(processName); /* pgr0840 */\
    try {                                                       \
        result = app->run(argc, argv);                          \
    } catch (Poco::Exception& exc) {                            \
        result = mdbcom::ExitCode::NG;                           \
    } catch (...) {                                             \
        result = mdbcom::ExitCode::NG;                           \
    }                                                           \
    return result;                                              \
}

#endif /* LIBMDBPROC_APPLICATION_H_ */

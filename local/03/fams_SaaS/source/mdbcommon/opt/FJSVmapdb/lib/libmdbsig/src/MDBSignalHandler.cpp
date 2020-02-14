/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBSignalHandler.cpp
 * @brief       The implement of MDBSignalHandler class.
 * @author      antn: Initialize MDBSignalHandler class.
 * @date        2016/9/18
 */

#include <sys/wait.h>
#include <unistd.h>

#include <system_error>

#include "MDBSignalHandler.h"
#include "MDBLogger.h"
#include "MDBException.h"
#include "MDBThreadUtility.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {

    /*! @class  Impl.
     *  @brief The class support handle signal.
     */
    class MDBSignalHandler::Impl {
    public:

        /*!
         * @brief constructor of SignalHandler.
         *
         * The method set pthreadId = 0. <BR>
         * The method add handler default for SIGCHLD, and SIGINT.
         *
         * @param None.
         * @return None.
         */
        Impl(void);

        /*!
         * @brief destructor of SignalHandler.
         * Using default destructor.
         *
         * @param None.
         * @return None.
         */
        ~Impl(void);

        /*!
         * @brief Add a signal and signal's handle management.
         *
         * The method add a signal and handle management to signal map.<BR>
         * In the case, the signal map contains signal, it update handle. <BR>
         * In the case, the signal map does not contain signal, it add signal and signal's handle management. <BR>
         * If "Signal Handler Thread" started, this method do not anything, and return Signal::OK_THREAD_STARTED.
         *
         * @param [in] signum The signal number.
         * @param [in] handle The handle management of signal (reference HandlerSignalManagement).
         *
         * @retval Signal::OK If success.
         * @retval Signal::OK_THREAD_STARTED If Signal Handler Thread started.
         * @retval Signal::ERROR_HANDLER_METHOD_INVALID If Handler is NULL.
         * @retval Signal::ERROR_SIGNAL_INVALID If signal number invalid.
         * @retval Signal::ERROR If happen other error.
         */
        int addHandleSignal(const int signum, const MDBSignalHandlerFunctionInformation &handle);

        /*!
         * @brief delete signal and signal's handler in signal map.
         *
         * The method delete signal and signal's handle in signal map.<BR>
         * In the case, the signal map does not contain signal, it is ignore.<BR>
         * If "Signal Handler Thread" started, it do not anything, and return Signal::OK_THREAD_STARTED.
         *
         * @param [in] signum The signal number.
         *
         * @retval Signal::OK If success.
         * @retval Signal::OK_THREAD_STARTED If Signal Handler Thread started.
         * @retval Signal::ERROR If happen other error.
         */
        int deleteHandleSignal(const int signum);

        /*!
         * @brief update handle of signal.
         *
         * The method update handle a signal. This method used update handle after "Signal Handler Thread" started. <BR>
         * In the case, signal map contains signal, it update handle. <BR>
         * In the case, signal map does not signal, it ignore signal and handler. <BR>
         *
         * @param [in] signum The signal number.
         * @param [in] handle The handle management of signal (reference HandlerSignalManagement).
         *
         * @retval Signal::OK If success.
         * @retval Signal::IGNORE If signal map does not contain signum.
         * @retval Signal::ERROR_HANDLER_METHOD_INVALID If Handler is NULL.
         */
        int updateHandleSignal(const int signum, const MDBSignalHandlerFunctionInformation &handle);

        /*!
         * @brief start "Signal Handler Thread".
         *
         * The method block all signal that register in signal map, and start "Signal Handler Thread.
         * And return Signal::OK, If "Signal Handler Thread" start success.
         *
         * @param None.
         * @retval Signal::OK If success.
         * @retval Signal::OK_NO_SIGNAL If no signal handler.
         * @retval Signal::OK_THREAD_STARTED If "Signal Handler Thread" started.
         * @retval Signal::ERROR_SIGEMPTYSET If sigemptyset() happen error.
         * @retval Signal::ERROR_SIGADDSET If sigaddset() happen error.
         * @retval Signal::ERROR_PTHREAD_SIGMASK If pthread_sigmask() happen error.
         * @retval Signal::ERROR_THREAD_START_FAILED If start Signal Handler Thread failed.
         */
        int startSignalHandlerThread(void);

        /*!
         * @brief waiting exit Signal Handler Thread.
         *
         * @param None.
         * @retval Signal::OK If success.
         * @retval Signal::ERROR_THREAD_JOIN_FAILED If "Signal Handler Thread" can not joined.
         */
        int waitForTermination(void);

        /*!
         * @brief copy constructor [unused].
         *
         * @param [in] signalHandler The signal handler.
         * @return The signal handler.
         */
        Impl(const Impl &) = delete; /* pgr0571 */ /* pgr2222 */

        /*!
         * @brief copy assignment operator [unused].
         *
         * @param [in] signalHandler The signal handler.
         * @return The signal handler.
         */
        Impl &operator=(const Impl &) = delete; /* pgr0055 *//* pgr2223 */ /* pgr0309 */

    private:

        //! Module name of signal hander thread
        static const std::string MODULE_SIGNAL_HANDLER_NAME;

        /*!
         * @brief handle all signals.
         *
         * The method is used in "Signal Handler" thread. It use sigwait for waiting signal. <BR>
         * After catch 1 signal, The method execute below:<BR>
         *   + write output log.<BR>
         *   + send condition to thread waiting it (usually main thread), finish method.
         *
         * @param [in] arg The pointer of SignalHandler object.
         * @return None.
         */
        static void handleSignals(void *arg);

        /*!
         * @brief handle SIGCHLD
         *
         * The method handle SIGCHLD default.
         *
         * @param [in] signum signal number.
         * @param [in] unused The argument is not use in this function.
         * @retval Signal::OK if child process terminate. Signal Hander Thread write log and send condition to Main Thread.
         * @retval Signal::IGNORE if other case (continue, stop). Signal Handler Thread ignore this signal.
         */
        static int handleSignChld(const int signum, const void *unused);

        /*!
         * @brief default of handle signal.
         *
         * The method write to logger. The method always return 0.
         *
         * @param [in] signum signal number.
         * @param [in] unused The argument is not use in this function.
         * @retval Signal::OK
         */
        static int defaultHandlerSignal(const int signum, const void *unused);

        //! Define map that manage signum, and handler signal method.
        using SignalMap = std::map<int, MDBSignalHandlerFunctionInformation>;
        //! The signal set of application.
        sigset_t signalSet;
        //! The map store signal number and signal number 's handle. Map's key: signal number, Map's value: handle function.
        SignalMap signalHandlerMap;
        //! The signal handler thread.
        std::thread threadSignalHandler;

        /*!
         * @brief setup mask of blocked signals.
         *
         * The method block all signal that register in signal map.<BR>
         *
         * @param None.
         * @retval Signal::OK If success.
         * @retval Signal::OK_NO_SIGNAL If no signal handler.
         * @retval Signal::ERROR_SIGEMPTYSET If sigemptyset() happen error.
         * @retval Signal::ERROR_SIGADDSET If sigaddset() happen error.
         * @retval Signal::ERROR_PTHREAD_SIGMASK If pthread_sigmask() happen error.
         */
        int setupSignalSet(void);

        /*!
         * @brief get signal set.
         *
         * @param None.
         * @return The signal set.
         */
        inline sigset_t *getSignalSet(void) {
            mdb_apl_log_start();
            mdb_apl_log_end();
            return &signalSet;
        }

        /*!
         * @brief remove poco signal handler.
         *
         * Poco library used SignalBlocker (Thread_POSIX.cpp ) for block SIGPIPE.<BR>
         * In the case we un-used it, un-block SIGPIPE.
         *
         * @param None.
         * @return None.
         */
        void removePocoSignalHandler(void);
    };

    //---------------------------- Impl-----------------------------

    MDBSignalHandler::Impl::Impl(void) :
        signalSet { { 0 } }, signalHandlerMap { }, threadSignalHandler { }
    {
        mdb_apl_log_start();

        try
        {
            removePocoSignalHandler();
        }
        catch (...)
        {
            mdb_apl_log_end();
            throw MDBException {};
        }

        // SIGRTMIN + 1 will be default termination signal
        MDBSignalHandlerFunctionInformation defaultHandler {defaultHandlerSignal, nullptr, true, true};

        if (Signal::OK != addHandleSignal(SIGRTMIN + 1, defaultHandler))
        {
            mdb_apl_log_end();
            throw MDBException {};
        }

        // Disable default behavior of SIGINT
        defaultHandler.setTerminationFlag(true);

        if (Signal::OK != addHandleSignal(SIGINT, defaultHandler))
        {
            mdb_apl_log_end();
            throw MDBException {};
        }

        // add default handle SICHLD
        defaultHandler.setMethod(handleSignChld);

        if (Signal::OK != addHandleSignal(SIGCHLD, defaultHandler))
        {
            mdb_apl_log_end();
            throw MDBException {};
        }

        mdb_apl_log_end();
    }

    MDBSignalHandler::Impl::~Impl(void)
    {
        mdb_apl_log_start();

        if (false == threadSignalHandler.joinable())
        {
            mdb_apl_log_end();
            return;
        }

        // get terminal signal
        auto signumTerminal = -1; /* pgr0745 */
        auto signumAny = -1; /* pgr0745 */

        for (auto &iterator : signalHandlerMap)
        {
            signumAny = iterator.first;

            auto management = iterator.second; /* pgr0745 */

            if (true == management.getTerminationFlag())
            {
                signumTerminal = iterator.first;
                break;
            }
        }

        if (-1 == signumTerminal)
        {
            signumTerminal = signumAny;

            MDBSignalHandlerFunctionInformation handleSignDefaultManagement {defaultHandlerSignal, nullptr, true};

            mdb_apl_log_info("Update default handler [signum = %d]", signumTerminal);

            if (Signal::OK != updateHandleSignal(signumTerminal, handleSignDefaultManagement))
            {
                mdb_apl_log_info("Update default handler failed");
                mdb_apl_log_end();
                return;
            }

        }

        // getpid always successful.
        auto pid = getpid(); /* pgr0745 */

        errno = 0;

        auto ret = kill(pid, signumTerminal); /* pgr0745 */

        if (0 != ret)
        {

            auto errorNumber = errno; /* pgr0745 */
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errorMessage = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_warn("Kill failed -> [ret = %d], [errno = %d], [msg = %s]", ret, errorNumber, errorMessage);
            mdb_apl_log_end();
            return;
        }

        try
        {
            threadSignalHandler.join();
        }
        catch (const std::system_error &e)
        {
            mdb_apl_log_warn("Error waiting for Signal Handler Thread.");
        }
        catch (...)
        {
            mdb_apl_log_warn("Error waiting for Signal Handler Thread.");
        }

        mdb_apl_log_end();
    }

    int MDBSignalHandler::Impl::addHandleSignal(const int signum,
            const MDBSignalHandlerFunctionInformation &handle)
    {
        mdb_apl_log_start();

        // Signal Handler thread is joinable
        if (true == threadSignalHandler.joinable())
        {
            mdb_apl_log_warn("Signal Handler Thread is running (signum = %d)", signum);
            mdb_apl_log_end();
            return Signal::OK_THREAD_STARTED;
        }

        // signum invalid
        if ((SIGHUP > signum) || (SIGRTMAX < signum))
        {
            mdb_apl_log_warn("Invalid signal number (signum = %d)", signum);
            mdb_apl_log_end();
            return Signal::ERROR_SIGNAL_INVALID;
        }

        // method is invalid (only check nullptr)
        if (nullptr == handle.getMethod())
        {
            mdb_apl_log_warn("Handle signal function is null");
            mdb_apl_log_end();
            return Signal::ERROR_HANDLER_METHOD_INVALID;
        }

        try
        {
            signalHandlerMap[signum] = handle;
        }
        catch (...)
        {
            mdb_apl_log_warn("add handler happen exception [signum = %d]", signum);
            mdb_apl_log_end();
            return Signal::ERROR;
        }

        mdb_apl_log_end();
        return Signal::OK;
    }

    int MDBSignalHandler::Impl::deleteHandleSignal(const int signum)
    {
        mdb_apl_log_start();

        // Signal Handler thread is joinable
        if (true == threadSignalHandler.joinable())
        {
            mdb_apl_log_warn("Signal Handler Thread is running (signum = %d)", signum);
            mdb_apl_log_end();
            return Signal::OK_THREAD_STARTED;
        }

        try
        {
            // delete signum:
            //      erase(key) can happen exception (Compare object),
            //      erase(key) return number of deleted element.
            auto numItems = signalHandlerMap.erase(signum); /* pgr0745 */
            mdb_apl_log_debug("Delete [signum = %d] - [numItems = %d]", signum, numItems);
        }
        catch (...)
        {
            mdb_apl_log_warn("Delete signal (%d) failed", signum);
            mdb_apl_log_end();
            return Signal::ERROR;
        }

        mdb_apl_log_end();
        return Signal::OK;
    }

    int MDBSignalHandler::Impl::updateHandleSignal(const int signum,
            const MDBSignalHandlerFunctionInformation &handle)
    {
        mdb_apl_log_start();

        if (nullptr == handle.getMethod())
        {
            mdb_apl_log_warn("Handle signal function is null");
            mdb_apl_log_end();
            return Signal::ERROR_HANDLER_METHOD_INVALID;
        }

        auto iterator = signalHandlerMap.find(signum); /* pgr0745 */

        if (iterator == signalHandlerMap.end())
        {
            mdb_apl_log_warn("Signal number not found (%d)", signum);
            mdb_apl_log_end();
            return Signal::IGNORE;
        }
        else
        {
            iterator->second = handle;
        }

        mdb_apl_log_end();
        return Signal::OK;
    }

    int MDBSignalHandler::Impl::startSignalHandlerThread(void)
    {
        mdb_apl_log_start();

        if (true == threadSignalHandler.joinable())
        {
            mdb_apl_log_warn("Signal Handler Thread is running.");
            mdb_apl_log_end();
            return Signal::OK_THREAD_STARTED;
        }

        auto result = setupSignalSet(); /* pgr0745 */

        if (Signal::OK != result)
        {
            mdb_apl_log_critical("setupSignalSet return failed (%d)", result);
            mdb_sys_log_out("F000403");
            mdb_apl_log_end();
            return result;
        }

        auto threadRet = MDBThreadUtility::createWorkUnit(MODULE_SIGNAL_HANDLER_NAME, handleSignals, this,
                         threadSignalHandler); /* pgr0745 */

        if (Result::OK != threadRet)
        {
            mdb_sys_log_out("F000405");
            mdb_apl_log_critical("Cannot create signal handler thread");
            mdb_apl_log_end();
            return Signal::ERROR_THREAD_START_FAILED;
        }

        mdb_apl_log_end();
        return Signal::OK;
    }

    int MDBSignalHandler::Impl::waitForTermination(void)
    {
        mdb_apl_log_start();

        if (false == threadSignalHandler.joinable())
        {
            mdb_apl_log_warn("Signal Handler Thread join failed");
            mdb_apl_log_end();
            return Signal::ERROR_THREAD_JOIN_FAILED;
        }

        try
        {
            threadSignalHandler.join();
            mdb_apl_log_info("Signal Handler Thread ended");
        }
        catch (const std::system_error &e)
        {
            mdb_apl_log_warn("Error waiting for Signal Handler Thread.");
            mdb_apl_log_end();
            return Signal::OK;
        }
        catch (...)
        {
            mdb_apl_log_warn("Error waiting for Signal Handler Thread.");
            mdb_apl_log_end();
            return Signal::OK;
        }

        mdb_apl_log_end();
        return Signal::OK;
    }

    void MDBSignalHandler::Impl::handleSignals(void *arg)   /*pgr0304*/
    {
        mdb_apl_log_start();

        // signal number.
        auto signalNum = 0; /* pgr0745 */
        auto signalHandler = reinterpret_cast<Impl *>(arg); /* pgr0745 */
        auto result = Signal::ERROR; /* pgr0745 */
        auto isTerm = false; /* pgr0745 */

        while (true)
        {
            result = sigwait(signalHandler->getSignalSet(), &signalNum);
            mdb_apl_log_info("sigwait() received signal [signum = %d]", signalNum);

            if (EINVAL == result)
            {
                mdb_apl_log_critical("sigwait() happen invalid signal number [signum = %d]",
                                     signalNum);
                mdb_sys_log_out("F000404");
                break; // stop thread because sigwait always failed if retry.
            }

            try
            {

                // throw std::out_of_range.
                auto management = signalHandler->signalHandlerMap.at(signalNum); /* pgr0745 */

                isTerm = management.getTerminationFlag();// not throw exception

                // If the handler is not active, do not run it.
                if (true == management.getIsActiveFlag())
                {
                    result = management.getMethod()(signalNum, management.getArgument()); // user define exception.
                }

                switch (signalNum)
                {
                    case SIGCHLD:
                        if (Signal::OK != result)
                        {
                            mdb_sys_log_out("F000401");
                        }

                        break;

                    case SIGINT:
                        mdb_sys_log_out("F000402");
                        break;

                    default:
                        break;
                }

                mdb_apl_log_info("The handler of signal [signum = %d] return %d", signalNum, result);
            }
            catch (const std::out_of_range &ex)
            {
                mdb_apl_log_critical(
                    "The application happen signal without handler: signalNum = %d", signalNum);
                mdb_sys_log_out("F000404");
            }
            catch (...)
            {
                mdb_apl_log_critical("The application happen exception when handle signalNum = %d",
                                     signalNum);
                mdb_sys_log_out("F000404");
            }

            if (true == isTerm)
            {
                break;
            }

            std::this_thread::yield();
        }

        mdb_apl_log_end();
    }

    int MDBSignalHandler::Impl::handleSignChld(const int signum, const void *unused)
    {
        mdb_apl_log_start();

        auto childPid = pid_t { 0 }; /* pgr0745 */
        auto status = 0; /* pgr0745 */

        do
        {
            // Wait dead processes.
            // Make sure signal handler will not block if a child was cleaned up.
            childPid = waitpid(-1, &status, WNOHANG);

            if (0 < childPid)
            {

                if (WIFEXITED(status))
                {

                    // WEXITSTATUS(status) -> ((((*(int *) &(status))) & 0xff00) >> 8)
                    if (0 != WEXITSTATUS(status))   /* pgr0651 */
                    {
                        mdb_apl_log_info(
                            "The process (pid =  %d) exited ab-normal with exit num = %d, arg = %p",
                            childPid, WEXITSTATUS(status), unused); /* pgr0651 */
                        mdb_apl_log_end();
                        return Signal::ERROR;
                    }
                    else
                    {
                        mdb_apl_log_info("The process (pid =  %d) exited normal", childPid);
                    }

                }
                else if (WIFSIGNALED(status))     /* pgr0651 */
                {
                    // WTERMSIG(status)    -> (((*(int *) &(status))) & 0x7f)
                    mdb_apl_log_info(
                        "The process (pid =  %d) exited when received signal = %d, arg = %p",
                        childPid, WTERMSIG(status), unused);

                    // WCOREDUMP(status)   -> (((*(int *) &(status))) & 0x80)
                    if (0 != WCOREDUMP(status))
                    {
                        mdb_apl_log_info("The process (pid =  %d) happen core dump", childPid);
                    }

                    mdb_apl_log_end();
                    return Signal::ERROR;
                }
                else
                {
                    mdb_apl_log_info("The process (pid =  %d) stopped or continued", childPid);
                }

            }

        }
        while (0 < childPid);

        mdb_apl_log_info("The application have just catch signal = %d, , arg = %p", signum, unused);
        mdb_apl_log_end();
        return Signal::OK;
    }

    int MDBSignalHandler::Impl::defaultHandlerSignal(const int signum, const void *unused)
    {
        mdb_apl_log_start();
        mdb_apl_log_info("The application have just catch signal = %d, arg = %p", signum, unused);
        mdb_apl_log_end();
        return Signal::OK;
    }

    int MDBSignalHandler::Impl::setupSignalSet(void)
    {
        mdb_apl_log_start();
        auto ret = 0; /* pgr0745 */
        // Initialize signalSet
        errno = 0;
        ret = sigemptyset(&signalSet);

        if (0 != ret)
        {
            auto errorNumber = errno; /* pgr0745 */
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> { '\0' }; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errorMessage = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_critical("sigemptyset() failed -> [ret = %d], [errno = %d], [msg = %s]",
                                 ret, errorNumber, errorMessage);
            mdb_apl_log_end();
            return Signal::ERROR_SIGEMPTYSET;
        }

        if (true == signalHandlerMap.empty())
        {
            auto message = std::string { "signalHandlerMap is empty." }; /* pgr0745 */

            mdb_apl_log_critical(message);
            mdb_apl_log_end();
            return Signal::OK_NO_SIGNAL;
        }

        // add signal that must handle.
        for (auto &iterator : signalHandlerMap)
        {

            auto signum = iterator.first; /* pgr0745 */

            errno = 0;
            ret = sigaddset(&signalSet, signum);

            if (0 != ret)
            {

                auto errorNumber = errno; /* pgr0745 */
                auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> { '\0' }; /* pgr0745 */
                auto errBufData = errBuf.data(); /* pgr0745 */
                auto errBufSize = errBuf.size(); /* pgr0745 */
                auto errorMessage = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

                mdb_apl_log_critical(
                    "sigaddset() failed -> [signum = %d], [ret = %d], [errno = %d], [msg = %s]",
                    signum, ret, errorNumber, errorMessage);
                mdb_apl_log_end();
                return Signal::ERROR_SIGADDSET;
            }
        }

        errno = 0;
        ret = pthread_sigmask(SIG_BLOCK, &signalSet, nullptr);

        if (0 != ret)
        {

            auto errorNumber = errno; /* pgr0745 */
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> { '\0' }; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errorMessage = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_critical("pthread_sigmask() failed -> [ret = %d], [errno = %d], [msg = %s]",
                                 ret, errorNumber, errorMessage);
            mdb_apl_log_end();
            return Signal::ERROR_PTHREAD_SIGMASK;
        }

        mdb_apl_log_end();
        return Signal::OK;
    }

    void MDBSignalHandler::Impl::removePocoSignalHandler(void)
    {
        mdb_apl_log_start();

        auto ret = 0; /* pgr0745 */
        auto sset = sigset_t { { 0 } }; /* pgr0745 */

        errno = 0;
        ret = sigemptyset(&sset);

        if (0 != ret)
        {

            auto errorNumber = errno; /* pgr0745 */
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> { '\0' }; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errorMessage = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_critical("sigemptyset() failed -> [ret = %d], [errno = %d], [msg = %s]",
                                 ret, errorNumber, errorMessage);
            mdb_apl_log_end();
            throw MDBException { };
        }

        errno = 0;
        ret = sigaddset(&sset, SIGPIPE);

        if (0 != ret)
        {

            auto errorNumber = errno; /* pgr0745 */
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> { '\0' }; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errorMessage = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_critical("sigaddset() failed -> [ret = %d], [errno = %d], [msg = %s]", ret,
                                 errorNumber, errorMessage);
            mdb_apl_log_end();
            throw MDBException { };
        }

        errno = 0;
        ret = pthread_sigmask(SIG_UNBLOCK, &sset, 0);

        if (0 != ret)
        {

            auto errorNumber = errno; /* pgr0745 */
            auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> { '\0' }; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errorMessage = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */

            mdb_apl_log_critical("pthread_sigmask() failed -> [ret = %d], [errno = %d], [msg = %s]",
                                 ret, errorNumber, errorMessage);
            mdb_apl_log_end();
            throw MDBException { };
        }

        mdb_apl_log_end();
    }

    //! Module name of signal hander thread
    const std::string MDBSignalHandler::Impl::MODULE_SIGNAL_HANDLER_NAME
    {
        "signalhandler"
    };

    //---------------------------- MDBSignalHandler-----------------------------

    /*!
     * @brief constructor of SignalHandler.
     *
     * The method set pthreadId = 0. <BR>
     * The method add handler default for SIGCHLD, and SIGINT.
     *
     * @return None.
     */
    MDBSignalHandler::MDBSignalHandler(void) :
        pImpl { MDBMemoryUtility::make_unique<Impl>() }
    {
    }

    /*!
     * @brief destructor of SignalHandler.
     * Using default destructor.
     *
     * @param None.
     * @return None.
     */
    MDBSignalHandler::~MDBSignalHandler(void) = default;

    /*!
     * @brief Add a signal and signal's handle management.
     *
     * The method add a signal and handle management to signal map.<BR>
     * In the case, the signal map contains signal, it update handle. <BR>
     * In the case, the signal map does not contain signal, it add signal and signal's handle management. <BR>
     * If "Signal Handler Thread" started, this method do not anything, and return Signal::OK_THREAD_STARTED.
     *
     * @param [in] signum The signal number.
     * @param [in] handle The handle management of signal (reference MDBHandlerSignalManagement).
     *
     * @retval Signal::OK If success.
     * @retval Signal::OK_THREAD_STARTED If Signal Handler Thread started.
     * @retval Signal::ERROR_HANDLER_METHOD_INVALID If Handler is nullptr.
     * @retval Signal::ERROR_SIGNAL_INVALID If signal number invalid.
     */
    int MDBSignalHandler::addHandleSignal(const int signum,
                                          const MDBSignalHandlerFunctionInformation &handle)
    {
        return pImpl->addHandleSignal(signum, handle);
    }

    /*!
     * @brief delete signal and signal's handler in signal map.
     *
     * The method delete signal and signal's handle in signal map.<BR>
     * In the case, the signal map does not contain signal, it is ignore.<BR>
     * If "Signal Handler Thread" started, it do not anything, and return Signal::OK_THREAD_STARTED.
     *
     * @param [in] signum The signal number.
     *
     * @retval Signal::OK If success.
     * @retval Signal::OK_THREAD_STARTED If Signal Handler Thread started.
     * @retval Signal::ERROR If happen other error.
     */
    int MDBSignalHandler::deleteHandleSignal(int signum)
    {
        return pImpl->deleteHandleSignal(signum);
    }

    /*!
     * @brief update handle of signal.
     *
     * The method update handle a signal. This method used update handle after "Signal Handler Thread" started. <BR>
     * In the case, signal map contains signal, it update handle. <BR>
     * In the case, signal map does not signal, it ignore signal and handler. <BR>
     *
     * @param [in] signum The signal number.
     * @param [in] handle The handle management of signal (reference MDBHandlerSignalManagement).
     *
     * @retval Signal::OK If success.
     * @retval Signal::IGNORE If signal map does not contain signum.
     * @retval Signal::ERROR_HANDLER_METHOD_INVALID If Handler is nullptr.
     */
    int MDBSignalHandler::updateHandleSignal(const int signum,
            const MDBSignalHandlerFunctionInformation &handle)
    {
        return pImpl->updateHandleSignal(signum, handle);
    }

    /*!
     * @brief start "Signal Handler Thread".
     *
     * The method block all signal that register in signal map, and start "Signal Handler Thread.
     * And return Signal::OK, If "Signal Handler Thread" start success.
     *
     * @param None.
     * @retval Signal::OK If success.
     * @retval Signal::OK_NO_SIGNAL If no signal handler.
     * @retval Signal::OK_THREAD_STARTED If "Signal Handler Thread" started.
     * @retval Signal::ERROR_SIGEMPTYSET If sigemptyset() happen error.
     * @retval Signal::ERROR_SIGADDSET If sigaddset() happen error.
     * @retval Signal::ERROR_PTHREAD_SIGMASK If pthread_sigmask() happen error.
     * @retval Signal::ERROR_THREAD_START_FAILED If start Signal Handler Thread failed.
     */
    int MDBSignalHandler::startSignalHandlerThread(void)
    {
        return pImpl->startSignalHandlerThread();
    }

    /*!
     * @brief get instance of Signal Handler.
     *
     * The method return instance of SignalHandler object. <BR>
     * If SignalHandler object do not initialize, this method call new SignalHandler() and return it. <BR>
     * It SignalHandler initialized, the method return initialized object.<BR>
     *
     * @param None.
     * @return MDBSignalHandler The reference of SignalHandler if initialize successful.
     * @throw MDBException in the case happen error.
     */
    MDBSignalHandler &MDBSignalHandler::getInstance(void)
    {
        mdb_apl_log_start();

        try
        {
            //! The properties for singleton: deny create multi signal handler.
            static MDBSignalHandler signalHanlderInstance;
            mdb_apl_log_end();
            return signalHanlderInstance;
        }
        catch (...)
        {
            mdb_apl_log_critical("happen exception");
            mdb_sys_log_out("F000403");
            mdb_apl_log_end();
            throw;
        }

    }

    /*!
     * @brief waiting condition from "Signal Hander Thread".
     *
     * @param None.
     * @retval Signal::OK If success.
     * @retval Signal::ERROR_THREAD_JOIN_FAILED If "Signal Handler Thread" can not joined.
     */
    int MDBSignalHandler::waitForTermination(void)
    {
        return pImpl->waitForTermination();
    }
} /* namespace mdbcom */


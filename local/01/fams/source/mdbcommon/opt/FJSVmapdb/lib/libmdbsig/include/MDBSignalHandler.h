/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBSignalHandler.h
 * @brief       The header of MDBSignalHandler class.
 * @author      antn: Initialize MDBSignalHandler class.
 * @date        2016/9/18
 */
#ifndef LIBMDBSIG_MDBSIGNALHANDLER_H_
#define LIBMDBSIG_MDBSIGNALHANDLER_H_

#include <csignal>
#include <thread>
#include <map>
#include <memory>
#include <MDBSignalHandlerFunctionInformation.h>

namespace mdbcom {

    /*! @class  MDBSignalHandler.
     *  @brief The class support handle signal.
     */
    class MDBSignalHandler {
    public:

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
        static MDBSignalHandler &getInstance(void);

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
        MDBSignalHandler(const MDBSignalHandler &) = delete; /* pgr0571 */ /* pgr2222 */

        /*!
         * @brief copy assignment operator [unused].
         *
         * @param [in] signalHandler The signal handler.
         * @return The signal handler.
         */
        MDBSignalHandler &operator=(const MDBSignalHandler &) = delete; /* pgr0055 *//* pgr2223 */ /* pgr0309 */
    private:
        /*! @class Impl.
         *  @brief The class supports behaviors of MDBSignalHandler.
         */
        class Impl;

        //!Pointer to Impl class
        std::unique_ptr<Impl> pImpl; /*pgr0359*/

        /*!
         * @brief constructor of SignalHandler.
         *
         * The method set pthreadId = 0. <BR>
         * The method add handler default for SIGCHLD, and SIGINT.
         *
         * @return None.
         */
        MDBSignalHandler(void);

        /*!
         * @brief destructor of SignalHandler.
         * Using default destructor.
         *
         * @param None.
         * @return None.
         */
        ~MDBSignalHandler(void);

    };

} /* namespace mdbcom */

#endif /* LIBMDBSIG_MDBSIGNALHANDLER_H_ */

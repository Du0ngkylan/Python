/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBApplicationUtility.h
 * @brief       Header file for define MDBApplicationUtility functions.
 * @author      cuongadp: Support thread operation.
 * @date        2016/12/07
 */

#ifndef LIBMDBPROC_MDBAPPLICATIONUTILITY_H_
#define LIBMDBPROC_MDBAPPLICATIONUTILITY_H_

#include <string>
#include <functional>

namespace mdbcom {
    class MDBApplicationUtility { /* pgr2205 */
    public:
        //! Define function pointer for handle signal.
        using HandleSignalFunction = std::function<int(int, const void *)>;
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBApplicationUtility(void) = delete;

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBApplicationUtility(void) = delete;

        /*!
         * @brief Copy Constructor.
         * @param None.
         * @return None.
         */
        MDBApplicationUtility(const MDBApplicationUtility &) = delete;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return None.
         */
        MDBApplicationUtility &operator=(const MDBApplicationUtility &) = delete; /* pgr0309 */ /* pgr0055 */

        /*!
         * @brief Move Constructor.
         * @param None.
         * @return None.
         */
        MDBApplicationUtility(MDBApplicationUtility &&) = delete;

        /*!
         * @brief move assignment.
         * @param None.
         * @return None.
         */
        MDBApplicationUtility &operator=(MDBApplicationUtility &&) = delete; /* pgr0309 */ /* pgr0055 */

        /*!
         * @brief  init resources for application
         *
         * processing additional step before executing actual work:
         *  - init log
         *  - open debug log for main thread of application.
         *  - init and start "Signal handler Thread"
         *  - start rotating log
         *
         * @param [in] moduleName module name
         * @param [in] handleSigRealTime sigRealTime handler
         *
         * @retval Result::OK init successfully
         * @retval Result::INIT_LOG_FAILED init log failed
         * @retval Result::OPEN_DEBUG_LOG_FAILED open debug log failed
         * @retval Result::START_SIGNAL_HANDLER_FAILED start signal handler failed
         * @retval Result::START_ROTATE_LOG_FAILED init start rotate log failed
         *
         */
        static int initApplication(const std::string &moduleName, const HandleSignalFunction handleSigRealTime);

        /*!
         * @brief  uninit resources for application
         *
         * @param None.
         * @return None.
         *
         */
        static void unInitApplication(void);
    private:
        //! Default name for main thread
        static std::string MAIN_THREAD_NAME;
    };
}
#endif /* LIBMDBPROC_MDBAPPLICATIONUTILITY_H_ */

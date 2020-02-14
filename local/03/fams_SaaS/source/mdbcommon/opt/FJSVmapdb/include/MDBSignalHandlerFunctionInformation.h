/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBSignalHandlerFunctionInformation.h
 * @brief       The header of MDBSignalHandlerFunctionInformation class.
 * @author      antn: Initialize MDBSignalHandlerFunctionInformation class.
 * @date        2016/10/28
 */

#ifndef LIBMDBSIG_MDBSIGNALHANDLERFUNCTIONINFORMATION_H_
#define LIBMDBSIG_MDBSIGNALHANDLERFUNCTIONINFORMATION_H_

#include <functional>
#include <memory>

namespace mdbcom {

    /*! @class  MDBSignalHandlerFunctionInformation.
     *  @brief The class support storage function pointer, and argument.
     */
    class MDBSignalHandlerFunctionInformation {
    public:
        //! Define function pointer for handle signal.
        using HandleSignalFunction = std::function<int(int, const void *)>;

        /*!
         * @brief constructor of MDBSignalHandlerFunctionInformation.
         *
         * @param None.
         * @return None.
         */
        MDBSignalHandlerFunctionInformation(void) noexcept;

        /*!
         * @brief constructor of MDBSignalHandlerFunctionInformation.
         *
         * @param [in] method The handler function pointer.
         * @param [in] argument The argument of handler.
         * @param [in] isActiveFlag The flag for determinate to run the handler or not.
         * @param [in] terminationFlag The flag for determinate finish Signal Handler Thread (default = false).
         * @return None.
         */
        MDBSignalHandlerFunctionInformation(const HandleSignalFunction method, const void *argument,
                                            const bool isActiveFlag, const bool terminationFlag = false) noexcept;

        /*!
         * @brief destructor of MDBSignalHandlerFunctionInformation.
         * Using default destructor.
         *
         * @param None.
         * @return None.
         */
        ~MDBSignalHandlerFunctionInformation(void);

        /*!
         * @brief get argument of method.
         *
         * The method get argument of handler function.
         *
         * @param None.
         * @return The pointer argument (void*) of handler function.
         */
        const void *getArgument(void) const noexcept;

        /*!
         * @brief set argument of method.
         *
         * The method set argument of method.
         *
         * @param [in] argument The argument of handler.
         * @return None.
         */
        void setArgument(const void *argument) noexcept;

        /*!
         * @brief get handler function.
         *
         * The method get handler function.
         *
         * @param None.
         * @return The handler function pointer.
         */
        HandleSignalFunction getMethod(void) const noexcept;

        /*!
         * @brief set handler function.
         *
         * The method set handler function.
         *
         * @param [in] method The handler function pointer.
         * @return None.
         */
        void setMethod(const HandleSignalFunction method) noexcept;

        /*!
         * @brief get IsActive flag.
         *
         * The method get IsActive flag.
         *
         * @param None.
         * @return The IsActive flag.
         */
        bool getIsActiveFlag(void) const noexcept;

        /*!
         * @brief set IsActive flag.
         *
         * The method set IsActive flag.
         *
         * @param [in] isActiveFlag The IsActive flag. <BR>
         * terminationFlag = true, "Signal Handler" Thread 'll finish after handle the signal that received.
         * @return None.
         */
        void setIsActiveFlag(const bool isActiveFlag) noexcept ;

        /*!
         * @brief get handler function.
         *
         * The method get termination flag.
         *
         * @param None.
         * @return The Termination flag.
         */
        bool getTerminationFlag(void) const noexcept;

        /*!
         * @brief set termination flag.
         *
         * The method set termination flag.
         *
         * @param [in] terminationFlag The termination flag. <BR>
         * terminationFlag = true, "Signal Handler" Thread 'll finish after handle the signal that received.
         * @return None.
         */
        void setTerminationFlag(const bool terminationFlag) noexcept ;

        /*!
         * @brief copy constructor
         *
         * @param [in] other The signal handler function information.
         * @return The handler signal management.
         */
        MDBSignalHandlerFunctionInformation(const MDBSignalHandlerFunctionInformation &other)
        noexcept;

        /*!
         * @brief copy assignment operator.
         *
         * @param [in] other The signal handler function information.
         * @return The handler signal management.
         */
        MDBSignalHandlerFunctionInformation &operator=(
            const MDBSignalHandlerFunctionInformation &other) noexcept;
    private:
        /*! @class Impl.
         *  @brief The class supports behaviors of MDBSignalHandlerFunctionInformation.
         */
        class Impl;

        //!Pointer to Impl class
        std::unique_ptr<Impl> pImpl; /*pgr0359*/
    };

} /* namespace mdbcom */

#endif /* LIBMDBSIG_MDBSIGNALHANDLERFUNCTIONINFORMATION_H_ */

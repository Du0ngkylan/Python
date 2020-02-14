/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBSignalHandlerFunctionInformation.cpp
 * @brief       The implement of MDBSignalHandlerFunctionInformation class.
 * @author      antn: Initialize MDBSignalHandlerFunctionInformation class.
 * @date        2016/10/28
 */

#include <MDBSignalHandlerFunctionInformation.h>
#include "MDBLogger.h"
#include "MDBMemoryUtility.h"
namespace mdbcom {

    /*! @class  Impl.
     *  @brief The class support storage function pointer, and argument.
     */
    class MDBSignalHandlerFunctionInformation::Impl {
    public:
        /*!
         * @brief constructor of Impl.
         *
         * @param None.
         * @return None.
         */
        Impl(void) noexcept;

        /*!
         * @brief constructor of Impl.
         *
         * @param [in] method The handler function pointer.
         * @param [in] argument The argument of handler.
         * @param [in] isActiveFlag The flag for determinate to run the handler or not.
         * @param [in] terminationFlag The flag for determinate finish Signal Handler Thread (default = false).
         * @return None.
         */
        Impl(const MDBSignalHandlerFunctionInformation::HandleSignalFunction method, const void *argument,
             const bool isActiveFlag, const bool terminationFlag = false) noexcept;
        /*!
         * @brief copy constructor
         *
         * @param [in] other The signal handler function information.
         * @return The handler signal management.
         */
        Impl(const Impl &other) noexcept = delete; /*pgr2222*//*pgr0571*//*pgr0364*/

        /*!
         * @brief copy assignment operator.
         *
         * @param [in] other The signal handler function information.
         * @return The handler signal management.
         */
        Impl &operator=(const Impl &other) noexcept = delete; /*pgr0055*//*pgr2223*//*pgr0309*//*pgr0364*/
        /*!
         * @brief get argument of method.
         *
         * The method get argument of handler function.
         *
         * @param None.
         * @return The pointer argument (void*) of handler function.
         */
        inline const void *getArgument(void) const noexcept {
            return mArgument;
        }

        /*!
         * @brief set argument of method.
         *
         * The method set argument of method.
         *
         * @param [in] argument The argument of handler.
         * @return None.
         */
        inline void setArgument(const void *argument) noexcept {
            mArgument = argument;
        }

        /*!
         * @brief get handler function.
         *
         * The method get handler function.
         *
         * @param None.
         * @return The handler function pointer.
         */
        inline MDBSignalHandlerFunctionInformation::HandleSignalFunction getMethod(void) const
        noexcept {
            return mMethod;
        }

        /*!
         * @brief set handler function.
         *
         * The method set handler function.
         *
         * @param [in] method The handler function pointer.
         * @return None.
         */
        inline void setMethod(
            const MDBSignalHandlerFunctionInformation::HandleSignalFunction method) noexcept { /*pgr2227*/
            mMethod = method;
        }

        /*!
         * @brief set IsActive flag.
         *
         * The method set IsActive flag.
         *
         * @param [in] isActiveFlag The termination flag. <BR>
         * isActiveFlag = true, the handler will be ran when signal comes.
         * isActiveFlag = false, the handler will NOT be ran when signal comes.
         * @return None.
         */
        inline void setIsActiveFlag(const bool isActiveFlag) noexcept {
            mIsActiveFlag = isActiveFlag;
        }

        /*!
         * @brief get IsActive flag.
         *
         * The method get IsActive flag.
         *
         * @param None.
         * @return The IsActive flag.
         */
        inline bool getIsActiveFlag(void) const noexcept {
            return mIsActiveFlag;
        }

        /*!
         * @brief set termination flag.
         *
         * The method set termination flag.
         *
         * @param [in] terminationFlag The termination flag. <BR>
         * terminationFlag = true, "Signal Handler" Thread 'll finish after handle the signal that received.
         * @return None.
         */
        inline void setTerminationFlag(const bool terminationFlag) noexcept {
            mTerminationFlag = terminationFlag;
        }

        /*!
         * @brief get handler function.
         *
         * The method get termination flag.
         *
         * @param None.
         * @return The Termination flag.
         */
        inline bool getTerminationFlag(void) const noexcept {
            return mTerminationFlag;
        }

    private:
        //! The method for handler.
        MDBSignalHandlerFunctionInformation::HandleSignalFunction mMethod;

        //! the argument of handler.
        const void *mArgument;

        //! The IsActive flag for checking if the Signal is active or not.
        bool mIsActiveFlag;

        //! The termination flag for stop Signal Handler Thread.
        bool mTerminationFlag;
    };

    //---------------------------- Impl-----------------------------
    /// cppcheck-suppress uninitMemberVar
    MDBSignalHandlerFunctionInformation::Impl::Impl(void) noexcept :
        mMethod {nullptr}, mArgument {nullptr}, mIsActiveFlag(true), mTerminationFlag {false}
    {}

    /// cppcheck-suppress uninitMemberVar
    MDBSignalHandlerFunctionInformation::Impl::Impl(
        const MDBSignalHandlerFunctionInformation::HandleSignalFunction method,/*pgr2227*/
        const void *argument, const bool isActiveFlag, const bool terminationFlag) noexcept :
        mMethod {method}, mArgument {argument}, mIsActiveFlag {isActiveFlag}, mTerminationFlag {terminationFlag}
    {}

    //---------------------------- MDBSignalHandlerFunctionInformation-----------------------------

    /*!
     * @brief constructor of MDBSignalHandlerFunctionInformation.
     *
     * @param None.
     * @return None.
     */
    MDBSignalHandlerFunctionInformation::MDBSignalHandlerFunctionInformation(void) noexcept
        : pImpl
    {
        MDBMemoryUtility::make_unique <
        Impl > ()
    }
    {
        mdb_apl_log_start();
        mdb_apl_log_end();
    }

    /*!
     * @brief constructor of MDBSignalHandlerFunctionInformation.
     *
     * @param [in] method The handler function pointer.
     * @param [in] argument The argument of handler.
     * @param [in] isActiveFlag The flag for determinate to run the handler or not.
     * @param [in] terminationFlag The flag for determinate finish Signal Handler Thread (default = false).
     * @return None.
     */
    MDBSignalHandlerFunctionInformation::MDBSignalHandlerFunctionInformation(
        const MDBSignalHandlerFunctionInformation::MDBSignalHandlerFunctionInformation::HandleSignalFunction method,
        const void *argument, const bool isActiveFlag, const bool terminationFlag) noexcept
        : pImpl
    {
        MDBMemoryUtility::make_unique <
        Impl > (method, argument, isActiveFlag, terminationFlag)
    }
    {
        mdb_apl_log_start();
        mdb_apl_log_end();
    }

    /*!
     * @brief copy constructor
     *
     * @param [in] other The signal handler function information.
     * @return The handler signal management.
     */
    MDBSignalHandlerFunctionInformation::MDBSignalHandlerFunctionInformation(
        const MDBSignalHandlerFunctionInformation &other) noexcept
        : pImpl
    {
        MDBMemoryUtility::make_unique <Impl> (other.getMethod(),
                                              other.getArgument(), other.getIsActiveFlag(), other.getTerminationFlag()) /*pgr0512*/
    }
    {
        mdb_apl_log_start();
        mdb_apl_log_end();
    }

    /*!
     * @brief destructor of MDBSignalHandlerFunctionInformation.
     * Using default destructor.
     *
     * @param None.
     * @return None.
     */
    MDBSignalHandlerFunctionInformation::~MDBSignalHandlerFunctionInformation(void) = default;

    /*!
     * @brief copy assignment operator.
     *
     * @param [in] other The signal handler function information.
     * @return The handler signal management.
     */
    MDBSignalHandlerFunctionInformation &MDBSignalHandlerFunctionInformation::operator=(
        const MDBSignalHandlerFunctionInformation &other) noexcept
    {
        mdb_apl_log_start();

        if (this == &other)
        {
            return *this;
        }

        pImpl->setMethod(other.getMethod());
        pImpl->setArgument(other.getArgument());
        pImpl->setIsActiveFlag(other.getIsActiveFlag());
        pImpl->setTerminationFlag(other.getTerminationFlag());

        mdb_apl_log_end();
        return *this;
    }

    /*!
     * @brief get argument of method.
     *
     * The method get argument of handler function.
     *
     * @param None.
     * @return The pointer argument (void*) of handler function.
     */
    const void *MDBSignalHandlerFunctionInformation::getArgument(void) const noexcept
    {
        return pImpl->getArgument();
    }

    /*!
     * @brief set argument of method.
     *
     * The method set argument of method.
     *
     * @param [in] argument The argument of handler.
     * @return None.
     */
    void MDBSignalHandlerFunctionInformation::setArgument(const void *argument) noexcept
    {
        pImpl->setArgument(argument);
    }

    /*!
     * @brief get handler function.
     *
     * The method get handler function.
     *
     * @param None.
     * @return The handler function pointer.
     */
    MDBSignalHandlerFunctionInformation::HandleSignalFunction MDBSignalHandlerFunctionInformation::getMethod(
        void) const noexcept
    {
        return pImpl->getMethod();
    }

    /*!
     * @brief set handler function.
     *
     * The method set handler function.
     *
     * @param [in] method The handler function pointer.
     * @return None.
     */
    void MDBSignalHandlerFunctionInformation::setMethod(
        const MDBSignalHandlerFunctionInformation::HandleSignalFunction method) noexcept /*pgr2227*/
    {
        pImpl->setMethod(method);
    }

    /*!
     * @brief set IsActive flag.
     *
     * The method set IsActive flag.
     *
     * @param [in] isActiveFlag The IsActive flag. <BR>
     * isActiveFlag = true, the handler will be ran when signal comes.
     * isActiveFlag = false, the handler will NOT be ran when signal comes.
     * @return None.
     */
    void MDBSignalHandlerFunctionInformation::setIsActiveFlag(const bool isActiveFlag)
    noexcept
    {
        pImpl->setIsActiveFlag(isActiveFlag);
    }

    /*!
     * @brief get IsActive function.
     *
     * The method get IsActive flag.
     *
     * @param None.
     * @return The IsActive flag.
     */
    bool MDBSignalHandlerFunctionInformation::getIsActiveFlag(void) const noexcept
    {
        return pImpl->getIsActiveFlag();
    }

    /*!
     * @brief set termination flag.
     *
     * The method set termination flag.
     *
     * @param [in] terminationFlag The termination flag. <BR>
     * terminationFlag = true, "Signal Handler" Thread 'll finish after handle the signal that received.
     * @return None.
     */
    void MDBSignalHandlerFunctionInformation::setTerminationFlag(const bool terminationFlag)
    noexcept
    {
        pImpl->setTerminationFlag(terminationFlag);
    }

    /*!
     * @brief get handler function.
     *
     * The method get termination flag.
     *
     * @param None.
     * @return The Termination flag.
     */
    bool MDBSignalHandlerFunctionInformation::getTerminationFlag(void) const noexcept
    {
        return pImpl->getTerminationFlag();
    }
}
/* namespace mdbcom */

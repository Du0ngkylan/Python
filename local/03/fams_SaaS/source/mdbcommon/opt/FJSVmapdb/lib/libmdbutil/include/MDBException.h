/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBException.h
 * @brief Definition for constants
 * @author cuondadp:
 * @date 2016/11/03
 */

#ifndef LIBMDBUTIL_MDBEXCEPTION_H_
#define LIBMDBUTIL_MDBEXCEPTION_H_

#include <exception>
#include <string>

namespace mdbcom {
    /*!
     * @class MDBException
     * @brief Common exception class
     */
    class MDBException: public std::exception { /* pgr2252 */
        /* pgr2205 */

    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBException(void);

        /*!
         * @brief Constructor.
         * @param [in] msg Error message.
         * @return None.
         */
        explicit MDBException(const std::string &msg);

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        virtual ~MDBException(void) = default;

        /*!
         * @brief copy constructor.
         * @param [in] other the other MDBException.
         * @return None.
         */
        MDBException(const MDBException &) = default; /* pgr2222 */ /* pgr0571 */ /* pgr2266 */

        /*!
         * @brief Operator=.
         * @param [in] other the other MDBException.
         * @return this MDBHTTPProcessingException.
         */
        MDBException &operator=(const MDBException &) = default; /* pgr0055 */ /*pgr2223*/ /* pgr0309 */

        /*!
         * @brief Move constructor.
         * @param [in] other the other MDBException.
         * @return None.
         */
        MDBException(MDBException &&) = default; /* pgr2222 */ /* pgr0571 */

        /*!
         * @brief Operator=.
         * @param [in] other the other MDBHTTPProcessingException.
         * @return this MDBHTTPProcessingException.
         */
        MDBException &operator=(MDBException &&) = default; /* pgr0055 */ /*pgr2223*/ /* pgr0309 */

        /*!
         * @brief Default function to get error message
         * @param None.
         * @return Pointer to non-null message string.
         */
        virtual const char *what(void) const noexcept ;

        /*!
         * @brief Get message content.
         * @param None.
         * @return message content.
         */
        inline const std::string &getMessage(void) const {
            return message;
        }
    protected:
        //! message content
        std::string message;
    };
}
#endif

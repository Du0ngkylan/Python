/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBException.cpp
 * @brief Definition for constants
 * @author cuongadp:
 * @date 2016/11/03
 */
#include "MDBException.h"

namespace mdbcom {

    /*!
     * @brief Constructor.
     * @param None.
     * @return None.
     */
    MDBException::MDBException() :
        message { }
    {
    }

    /*!
     * @brief Constructor.
     * @param [in] msg content.
     * @return None.
     */
    MDBException::MDBException(const std::string &msg) :
        message { msg }
    {
    }

    /*!
     * @brief Default function to get error message
     * @param None.
     * @return Pointer to non-null message string.
     */
    const char *MDBException::what(void) const noexcept
    {
        return message.c_str();
    }
}

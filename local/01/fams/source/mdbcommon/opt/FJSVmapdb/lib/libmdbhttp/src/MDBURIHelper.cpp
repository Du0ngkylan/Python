/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBURIHelper.cpp
 * @brief Implementation for MDBURIHelper
 * @author trangvp:
 * @date 2017/2/17
 */

#include "MDBURIHelper.h"
#include "MDBMemoryUtility.h"
#include "MDBException.h"
namespace mdbcom {
    /*!
     * @brief Constructor
     * @param [in] inputURI the URI input
     * @return None.
     */
    MDBURIHelper::MDBURIHelper(const std::string &inputURI) : mURI(nullptr), newPath()
    {
        try
        {
            mURI = MDBMemoryUtility::make_unique<Poco::URI>(inputURI);
        }
        catch (const std::bad_alloc &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            throw MDBException(exDetail);
        }
        catch (...)
        {
            throw MDBException("URI has wrong format");
        }

        newPath = mURI->getPath();

        // Remove last '/' character
        if (SLASH_CHAR == newPath.back())
        {
            newPath.pop_back();
        }
    }

    /*!
     * @brief Copy constructor
     * @param [in] other The other MDBURIHelper.
     * @return None.
     */
    MDBURIHelper::MDBURIHelper(const MDBURIHelper &other) : mURI(nullptr),
        newPath(other.newPath)
    {
        if (nullptr != other.mURI)
        {
            mURI = MDBMemoryUtility::make_unique<Poco::URI>(*other.mURI);
        }
    }

    /*!
     * @brief Get parsed URI path
     * @param None.
     * @return parsed URI path
     */
    const std::string &MDBURIHelper::getPath(void) const
    {
        return newPath;
    }
}

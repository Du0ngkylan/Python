/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBURIHelper.h
 * @brief Definition for MDBURIHelper
 * @author trangvp:
 * @date 2017/2/17
 */
#ifndef LIBMDBHTTP_MDBURIHELPER_H
#define LIBMDBHTTP_MDBURIHELPER_H

#include <Poco/URI.h>
#include <memory>
namespace mdbcom {
    /*!
     * @class MDBURIHelper
     * @brief Support parsing URI
     */
    class MDBURIHelper {
    private:
        //! The '/' character
        static constexpr char SLASH_CHAR = '/';
        //! The smart pointer of URI class
        std::unique_ptr<Poco::URI> mURI;
        //! The parsed URI's path
        std::string newPath;
    public:
        /*!
         * @brief Constructor
         * @param [in] inputURI the URI input
         * @return None.
         */
        explicit MDBURIHelper(const std::string &inputURI);

        /*!
         * @brief Copy constructor
         * @param [in] other The other MDBURIHelper.
         * @return None.
         */
        MDBURIHelper(const MDBURIHelper &other);

        /*!
         * @brief Copy assignment
         * @param None.
         * @return This MDBURIHelper
         */
        MDBURIHelper &operator=(const MDBURIHelper &) = delete; /* pgr0055 */ /* pgr0309 */ /* pgr2223 */

        /*!
         * @brief Destructor
         * We use default destructor
         * @param None.
         * @return None.
         */
        ~MDBURIHelper(void) = default;

        /*!
         * @brief Get parsed URI path
         * @param None.
         * @return parsed URI path
         */
        const std::string &getPath(void) const;
    };

}

#endif

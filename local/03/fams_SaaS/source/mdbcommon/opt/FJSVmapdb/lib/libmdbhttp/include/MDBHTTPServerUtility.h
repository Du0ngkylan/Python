/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBHTTPServerUtility.h
 * @brief Definition for HTTP server utility
 * @author trangvp:
 * @date 2016/11/09
 */

#ifndef LIBMDBHTTP_MDBHTTPSERVERUTILITY_H_
#define LIBMDBHTTP_MDBHTTPSERVERUTILITY_H_

#include <string>
#include <Poco/Net/HTTPServerRequest.h>

namespace mdbcom {
    /*!
     * @class MDBHTTPServerUtility
     * @brief HTTP server utility
     */
    class MDBHTTPServerUtility {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBHTTPServerUtility(void) = default; /* pgr2222 */ /* pgr0571 */ /* pgr2266 */

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBHTTPServerUtility(void) = default;

        /*!
         * @brief Utility function to help extract JSON data from HTTP request.
         *
         * @param [in] request the HTTPServerRequest
         *
         * @return JSON string
         */
        static std::string extractJSONFromHTTPRequest(Poco::Net::HTTPServerRequest &request);

        /*!
         * @brief Support convert a binary file to base64 string.
         *
         * @param [in] filePath the file path
         * @param [out] base64String the base64 string output
         *
         * @retval Result::NG It failed
         * @retval Result::OK It is successful
         */
        static int base64EncodeFile(const std::string &filePath, std::string &base64String);

        /*!
         * @brief Support convert a string to base64 string.
         *
         * @param [in] inString the string to encode
         * @param [out] base64String the base64 string output
         *
         * @retval Result::NG It failed
         * @retval Result::OK It is successful
         */
        static int base64Encode(const std::string &inString, std::string &base64String);

        /*!
         * @brief Support decode base64 string.
         *
         * @param [in] base64String input string to decode
         * @param [out] decodedString string after decoded
         *
         * @retval Result::NG It failed
         * @retval Result::OK It is successful
         */
        static int base64Decode(const std::string &base64String, std::string &decodedString);

        /**
         * @brief Copy constructor
         *
         * @param None.
         * @return None.
         */
        MDBHTTPServerUtility(const MDBHTTPServerUtility &) = delete; /* pgr2222 */ /* pgr0571 */

        /**
         * @brief Copy Assignment
         *
         * @param None.
         * @return None.
         */
        MDBHTTPServerUtility &operator=(const MDBHTTPServerUtility &) = delete; /* pgr0055 *//* pgr2223 *//* pgr0309 */ /* pgr2252 */

        static long long encryptedTime;
    private:

        /*!
         * @brief check current time is exceed time in encrypted file or not
         *
         * @param [in] None.
         *
         * @retval Result::NG It failed or current time is over the time in encrypted file
         * @retval Result::OK It is OK to continue extract JSON
         */
        static int checkUsageTimeLimit(void);
    };
}

#endif /* LIBMDBHTTP_MDBHTTPSERVERUTILITY_H_ */

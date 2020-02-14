/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBHTTPServerUtility.cpp
 * @brief Implement for crypto utility
 * @author trangvp:
 * @date 2016/11/09
 */

#include <fstream>
#include <sstream>

#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>
#include <Poco/StreamCopier.h>

#include "MDBConstants.h"
#include "MDBHTTPServerUtility.h"
#include "MDBLogger.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {

    long long MDBHTTPServerUtility::encryptedTime {0};

    /*!
     * @brief Utility function to help extract JSON data from HTTP request.
     *
     * @param [in] request the HTTPServerRequest
     *
     * @return JSON string
     */
    std::string MDBHTTPServerUtility::extractJSONFromHTTPRequest(
        Poco::Net::HTTPServerRequest &request)
    {
        mdb_apl_log_start();
        auto output = std::string {}; /* pgr0745 */

        if (mdbcom::Result::OK != checkUsageTimeLimit())
        {
            mdb_apl_log_info("Should NOT continue because checkUsageTimeLimit NG");
            mdb_apl_log_end();
            return output;
        }

        try
        {
            // Get content length
            auto dataLength = request.getContentLength() + 1; /* pgr0745 */
            // get stream content
            auto &istr = request.stream(); /* pgr0745 */
            auto jsonString = MDBMemoryUtility::make_unique<char[]>(dataLength); /*pgr0694*//* pgr0745 */
            // read data from stream to char array
            static_cast<void>(istr.read(jsonString.get(), dataLength - 1));
            // store data to string
            output = jsonString.get();
        }
        catch (const std::bad_alloc &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical("Fail to allocate memory: %s", exDetail);
        }
        catch (...)
        {
            mdb_apl_log_critical("Unknown exception");

        }

        mdb_apl_log_end();
        return output;
    }

    /*!
     * @brief check current time is exceed time in encrypted file or not
     *
     * @param [in] None.
     *
     * @retval Result::NG It failed or current time is over the time in encrypted file
     * @retval Result::OK It is OK to continue extract JSON
     */
    int MDBHTTPServerUtility::checkUsageTimeLimit(void)
    {
        mdb_apl_log_start();
        auto retCode = Result::OK; /* pgr0745 */
        long long nowMilisec = std::chrono::duration_cast<std::chrono::milliseconds>
                               (std::chrono::system_clock::now().time_since_epoch()).count();

        if (nowMilisec < encryptedTime)
        {
            mdb_apl_log_info("Current time is OK with limit");
            retCode = mdbcom::Result::OK;
        }
        else
        {
            mdb_apl_log_critical("Current time exceed use time limit");
            retCode = mdbcom::Result::NG;
        }

        mdb_apl_log_end();
        return retCode;
    }

    /*!
     * @brief Support convert a binary file to base64 string.
     *
     * @param [in] filePath the file path
     * @param [out] base64String the base64 string output
     *
     * @retval Result::NG It failed
     * @retval Result::OK It is successful
     */
    int MDBHTTPServerUtility::base64EncodeFile(const std::string &filePath, std::string &base64String)
    {
        mdb_apl_log_start();

        auto tmpPath = filePath.c_str(); /* pgr0745 */
        mdb_apl_log_debug("in filePath = [%s]", tmpPath);
        auto retCode = Result::NG; /* pgr0745 */
        // Initialize ifstream
        std::ifstream istr {filePath};

        // Check ifstream is good
        if (true == istr.good())
        {
            std::ostringstream str {};
            // Initialize Base64Encoder
            Poco::Base64Encoder encoder {str};
            // No add line feed each 64 bytes
            encoder.rdbuf()->setLineLength(0);
            // Copy stream from ifstream to Base64Encoder to encode
            static_cast<void>(Poco::StreamCopier::copyStream(istr, encoder));

            // Close encoder
            if (0 > encoder.close())
            {
                mdb_apl_log_warn("Close encoder has error");
            }

            // close ifstream
            istr.close();
            // Get base64string from encoder
            base64String = str.str();
            retCode = Result::OK;
        }

        mdb_apl_log_debug("out ret = [%d]", retCode);
        mdb_apl_log_end();
        return retCode;
    }

    /*!
     * @brief Support convert a string to base64 string.
     *
     * @param [in] inString the string to encode
     * @param [out] base64String the base64 string output
     *
     * @retval Result::NG It failed
     * @retval Result::OK It is successful
     */
    int MDBHTTPServerUtility::base64Encode(const std::string &inString, std::string &base64String)
    {
        mdb_apl_log_start();
        std::ostringstream str;
        Poco::Base64Encoder encoder{str};
        encoder.rdbuf()->setLineLength(0);
        encoder << inString;

        // Close encoder
        if (0 > encoder.close())
        {
            mdb_apl_log_warn("Close encoder has error");
        }

        base64String = str.str();
        mdb_apl_log_end();
        return Result::OK;
    }

    // cpp check: it's never use
    /*!
     * @brief Support decode base64 string.
     *
     * @param [in] base64String input string to decode
     * @param [out] decodedString string after decoded
     *
     * @retval Result::NG It failed
     * @retval Result::OK It is successful
     */
    //    int MDBHTTPServerUtility::base64Decode(const std::string &base64String, std::string &decodedString)
    //    {
    //        mdb_apl_log_start();
    //        std::istringstream istr {base64String};
    //        std::ostringstream ostr;
    //        Poco::Base64Decoder decoder{istr};
    //        Poco::StreamCopier::copyStream(decoder, ostr);
    //        decodedString = ostr.str();
    //        mdb_apl_log_end();
    //        return Result::OK;
    //    }
}

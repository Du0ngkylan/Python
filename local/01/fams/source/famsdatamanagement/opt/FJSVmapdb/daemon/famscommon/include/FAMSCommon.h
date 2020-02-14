/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSCommon.h
 * @brief       All common methods are defined here
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_COMMON_H_
#define FAMS_COMMON_H_
#include <array>
#include <vector>
#include <set>
#include <string>
#include <sstream>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/SecureSMTPClientSession.h>
#include <Poco/Net/StringPartSource.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/KeyConsoleHandler.h>
#include <Poco/Net/ConsoleCertificateHandler.h>
#include <Poco/SharedPtr.h>
#include <Poco/Path.h>
#include <Poco/Exception.h>
#include <iostream>
#include <Poco/Base64Encoder.h>

#include "MDBPreparedStatement.h"
#include "MDBLogger.h"
#include "MDBException.h"



//using Poco::Net::MailMessage;
//using Poco::Net::MailRecipient;
//using Poco::Net::SMTPClientSession;
//using Poco::Net::SecureSMTPClientSession;
//using Poco::Net::StringPartSource;
//using Poco::Net::SSLManager;
//using Poco::Net::Context;
//using Poco::Net::KeyConsoleHandler;
//using Poco::Net::PrivateKeyPassphraseHandler;
//using Poco::Net::InvalidCertificateHandler;
//using Poco::Net::ConsoleCertificateHandler;
//using Poco::SharedPtr;
//using Poco::Path;
//using Poco::Exception;

namespace famscommon {
    /*!
     * @enum MessageType
     * @brief Message Type to classify data type on IPC
     */
    enum class MessageType: char
    {
        SENSOR_DATA,
        CISTERN_SENSOR_DATA,
        NITRIFICATION_DATA,
        OUTSIDE_DATA
    };

    //! Accumulated Time length
    static constexpr std::size_t ACCUMULATED_TIME_LEN { 25 };

    //! Cistern code length
    static constexpr std::size_t CISTERN_CODE_LEN { 6 };

    // Define datetime format
    static constexpr const char *const TIMESTRING_FORMAT = "%Y-%m-%dT%H:%M:%sZ%z"; /* pgr0746 */
    static const std::string TIME_POSTGRESQL_FORMAT = "%Y-%m-%d %H:%M:%s%z"; /* pgr0746 */
    static const std::string TIME_RFC3339_FORMAT = "%Y-%m-%dT%H:%M:%sZ"; /* pgr0746 */

    // All shared SQL Statements
    const std::vector<mdbcom::MDBPreparedStatement> &getCommonStatement(void);
    // Define version of API
    static const std::string API_VERSION = "/api/{API_VERSION}";
    static const std::string API_VERSION_V1 = "/api/v1";
    // Default timezone
    static constexpr const int VALID_TIME_ZONE {9}; /*pgr0746*/

    // Variable constants

    /* ----- Default values for fields in classes ----- */
    //! default value for integer fields in FAMSxxxData class
    static constexpr const int JSON_INTEGER_MISSING_VALUE {-1}; /*pgr0746*/
    //! default value for string fields in FAMSxxxData class
    static constexpr const char *const JSON_STRING_MISSING_VALUE {""}; /*pgr0746*/
    //! default value for double fields in FAMSxxxData class
    static constexpr const double JSON_DOUBLE_MISSING_VALUE {-900000.0}; /*pgr0746*/
    //! default value for time fields in FAMSxxxData class , 100-1-1 0:0:0
    static constexpr const double JSON_DOUBLE_TIME_MISSING_VALUE {-59011459200.0}; /*pgr0746*/
    /* ------------------------------------------------- */

    /* ----- SQL Constant Definition ----- */

    /* ------------------------------------------------- */

    /* ---------------- Common function ---------------- */
 
    /*!
    * @brief emplace back into vector if element not exist in vector
    * @param [out] inputStr string to split
    * @return None
    */
    template <typename T>
    inline void emplaceBackUnique(std::vector<T> &vect, const T element)
    {
        auto beginIt = vect.begin(); /*pgr0745*/
        auto endIt = vect.end(); /*pgr0745*/
        auto foundIt = std::find(beginIt, endIt, element); /*pgr0745*/

        if (foundIt == endIt)
        {
            vect.emplace_back(element);
        }
    }

    /*!
    * @brief find if element exists in vector
    * @param [out] inputStr string
    * @return None
    */
    template <typename T>
    inline bool checkExistence(std::vector<T> &vect, const T element)
    {
        auto beginIt = vect.begin(); /*pgr0745*/
        auto endIt = vect.end(); /*pgr0745*/
        auto foundIt = std::find(beginIt, endIt, element); /*pgr0745*/

        if (foundIt == endIt)
        {
            return false;
        } else {
            return true;
        }
    }
    /*!
    * @brief convert input string to base64 string
    * @param [out] source string
    * @return encoded string
    */
    inline std::string toBase64 (const std::string &source)
    {
        std::istringstream in(source);
        std::ostringstream out;
        Poco::Base64Encoder b64out(out);
        
        std::copy(std::istreambuf_iterator< char >(in),
                    std::istreambuf_iterator< char >(),
                    std::ostreambuf_iterator< char >(b64out));
        b64out.close(); // always call this at the end!
        
        return out.str();
    }
    /*!
    * @brief get current time in second (used for set request time when sending request)
    * @param [in] None.
    * @return current time in second with precision is milisecond
    */
    inline double getCurrentTime(void)
    {
        long long nowMilisec = std::chrono::duration_cast<std::chrono::milliseconds>
                               (std::chrono::system_clock::now().time_since_epoch()).count();

        if ((nowMilisec % 10) == 0)
        {
            nowMilisec += 1;
        }

        double nowSec = static_cast<double>(nowMilisec) / 1000;
        return nowSec;
    }

    /*!
    * @brief replace parentheses by regex pattern
    * @param [in] str string need to be replaced.
    * @return string after replaced
    */
    inline std::string replaceParentheses(std::string str)
    {
        long posStart = -1, pos = 0u;
        std::string result = str;
        std::map<long, long> startEndParentheses;
        while(pos != str.length()) {
            if (str.at(pos) == '{') {
                posStart = pos;
            } else if (str.at(pos) == '}') {
                if (posStart != -1) {
                    startEndParentheses.insert ( std::pair<long,long>(posStart,pos) );
                    posStart = -1;
                }
            }
            pos += 1;
        }
        std::map<long, long>::reverse_iterator it = startEndParentheses.rbegin();
        while (it != startEndParentheses.rend()) {
            result.replace(it->first, (it->second - it->first + 1), "\\w+");
            it++;
        }
        return result;
    }

    /*!
     * @brief split string with characters in deliter string
     * @param [in] inputStr string to split
     * @param [in] delimiter delimiter characters
     * @return a vector of tokens
     */
    std::vector<std::string> splitString(const std::string &inputStr, const std::string &delimiter);

    /*!
     * @brief convert timestamp to string
     *
     * @param [in] timeStamp Time of event occurrence (miliseconds)
     * @param [out] time Time of event occurrence in string form
     * @param [in] timeZone indicate result string should have time zone part
     *
     * @return Result::OK convert success
     * @return Result::NG convert fail
     */
    int convTimestampToString(double timeStamp, std::string &time, int timeZone = VALID_TIME_ZONE);

    /*!
     * @brief convert timestamp to string
     *
     * @param [in] time Time of event occurrence in string form
     * @param [out] timeStamp Time of event occurrence (miliseconds)
     * @param [in] checkTimeZone indicate check timezone is valid or not
     *
     * @return Result::OK convert success
     * @return Result::NG convert fail
     */
    int convStringToTimestamp(const std::string &time, double &timeStamp, bool checkTimeZone = true);
    /* ------------------------------------------------- */

    extern int convertICTToUTC(const std::string ictTime, std::string &utcTime);
    extern int convertProcessTime(const std::string processTime, std::string &processTimeConverted);
    extern int sendMail(const std::string mailhost, const std::string sender, const std::vector<std::string> recipients, const std::string subject,
            const std::string username, const std::string password, const std::string contentParams, const int port);
}
#endif /* FAMS_COMMON_H_ */


/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSCommon.cpp
 * @brief       Source file of common constant and function
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include <jansson.h>
#include <Poco/StringTokenizer.h>
#include <Poco/DateTimeParser.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/Timestamp.h>
#include <sstream>
#include "MDBConnectionManager.h"
#include "FAMSCommon.h"

using Poco::DateTimeParser;
using Poco::DateTimeFormat;
using Poco::DateTime;
namespace famscommon {

    const std::vector<mdbcom::MDBPreparedStatement> &getCommonStatement(void)
    {
        static const std::vector<mdbcom::MDBPreparedStatement> famsPreparedStatements =
        {
        };
        return famsPreparedStatements;
    }

    /*!
     * @brief split string with characters in deliter string
     * @param [in] inputStr string to split
     * @param [in] delimiter delimiter characters
     * @return a vector of tokens
     */
    std::vector<std::string> splitString(const std::string &inputStr, const std::string &delimiter)
    {
        Poco::StringTokenizer pocoTokens(inputStr, delimiter, Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        std::vector<std::string> tokens {pocoTokens.begin(), pocoTokens.end()};
        return tokens;
    }

    /*!
     * @brief convert timestamp to string
     *
     * @param [in] timeStamp Time of event occurrence (milliseconds)
     * @param [out] time Time of event occurrence in string form
     * @param [in] timeZone indicate result string should have time zone part
     *
     * @return mdbcom::Result::OK convert success
     * @return mdbcom::Result::NG convert fail
     */
    int convTimestampToString(double timeStamp, std::string &time, int timeZone)
    {
        // adjust fractional seconds to microseconds to show exactly string
        long long timeZoneSec = timeZone * 60 * 60;
        long long pocoMicroSec = (timeStamp + timeZoneSec) * 1000 * 1000; // add timezone to convert correctly
        Poco::Timestamp ts(pocoMicroSec);
        int diffTimeZone = (timeZone == 0) ? static_cast<int>(Poco::DateTimeFormatter::UTC) : static_cast<int>(timeZoneSec);
        time = Poco::DateTimeFormatter::format(ts, TIMESTRING_FORMAT, diffTimeZone);
        size_t dotPos = time.find(".");

        try
        {
            time = time.erase(dotPos + 3, 3); //erase redundant 3 decimal
        }
        catch (...)
        {
            mdb_apl_log_critical("fail to cut time string");
            return mdbcom::Result::NG;
        }

        return mdbcom::Result::OK;
    }

    /*!
     * @brief convert string to timestamp (milisecond)
     *
     * @param [in] time Time of event occurrence in string form
     * @param [out] timeStamp Time of event occurrence (milliseconds)
     * @param [in] checkTimeZone indicate check timezone is valid or not
     *
     * @return mdbcom::Result::OK convert success
     * @return mdbcom::Result::NG convert fail
     */
    int convStringToTimestamp(const std::string &time, double &timeStamp, bool checkTimeZone)
    {
        try
        {
            mdb_apl_log_debug("time string = %s", time.c_str());
            int tzd = -1; // different timezone in seconds
            DateTime dt;
            DateTimeParser::parse(DateTimeFormat::ISO8601_FRAC_FORMAT, time, dt, tzd);
            auto timeZone = tzd / 60 / 60; /*pgr0745*/ //convert different time zone to hours

            if ((VALID_TIME_ZONE != timeZone) && (true == checkTimeZone))
            {
                mdb_apl_log_critical("invalid time zone in time string");
                return mdbcom::Result::NG;
            }
            else
            {
                dt.makeLocal(-tzd); //make local to convert exactly,  otherwise same result for all time zone
                Poco::Timestamp ts = dt.timestamp();
                auto timeStampMilli = static_cast<long long>(ts.epochMicroseconds() / 1000); /*pgr0745*/
                timeStamp = static_cast<double>(timeStampMilli) / 1000;  // fractional seconds. Ex 12.003 s
                return mdbcom::Result::OK;
            }
        }
        catch (std::exception &e)
        {
            mdb_apl_log_critical("invalid time string, exception [%s]", e.what());
            return mdbcom::Result::NG;
        }
        catch (...)
        {
            mdb_apl_log_critical("invalid time string");
            return mdbcom::Result::NG;
        }
    }

    int convertProcessTime(const std::string processTime, std::string &processTimeConverted) {
        mdb_apl_log_start();

        // Declare const
        const int maxSizeOfProcessTime = 25;
        const int sizeIncludedSecond = 22;
        const int sizeNotIncludedSecond = 19;

        int ret = mdbcom::Result::OK;
        if (processTime == "") {
            processTimeConverted = processTime;
            mdb_apl_log_end();
            return ret;
        }
        char convertTime[30];
        memset(convertTime, '\0', sizeof(convertTime));
        if (processTime.size() < maxSizeOfProcessTime)
        {
            strncpy(convertTime , processTime.c_str() ,sizeNotIncludedSecond);
        }
        else
        {
            strncpy(convertTime , processTime.c_str() ,sizeIncludedSecond);
        }

        processTimeConverted = convertTime;
        mdb_apl_log_trace("processTime: %s, processTimeConverted: %s", processTime.c_str(),
                processTimeConverted.c_str());
        mdb_apl_log_end();
        return ret;
    }

    int convertICTToUTC(const std::string ictTime, std::string &utcTime) {
        mdb_apl_log_start();
        int ret = mdbcom::Result::OK;
        if (ictTime == "") {
            utcTime = ictTime;
            mdb_apl_log_end();
            return ret;
        }
        DateTime dt;
        int tzd = -1; // different timezone in seconds
        Poco::DateTimeParser::parse(TIME_POSTGRESQL_FORMAT, ictTime, dt, tzd);
        dt.makeLocal(-tzd);
        utcTime = Poco::DateTimeFormatter::format(dt, TIME_RFC3339_FORMAT, tzd);
        mdb_apl_log_trace("DateTime: %s, Parsed: %s", ictTime.c_str(),
                utcTime.c_str());
        mdb_apl_log_end();
        return ret;
    }

    int sendMail(const std::string mailhost, const std::string sender, const std::vector<std::string> recipients, const std::string subject,
            const std::string username, const std::string password, const std::string contentParams, const int port) {
        mdb_apl_log_start();

        try
        {
            // Note: we must create the passphrase handler prior Context
            Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> pCert = new Poco::Net::ConsoleCertificateHandler(false); // ask the user via console
            Poco::Net::Context::Ptr pContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
            Poco::Net::SSLManager::instance().initializeClient(0, pCert, pContext);
            Poco::Net::MailMessage message;
            message.setSender(sender);
            for (int i = 0; i < recipients.size(); i++) {
                message.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT, recipients[i]));
            }
            message.setSubject(subject);
            message.setContent(contentParams);

            Poco::Net::SecureSMTPClientSession session(mailhost, port);
            session.login();
            session.startTLS(pContext);
            if (!username.empty())
            {
                session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, username, password);
            }
            session.sendMessage(message);
            session.close();
        }
        catch (Poco::Exception& exc)
        {
            return mdbcom::Result::NG;
            std::cerr << exc.displayText() << std::endl;
        }

        mdb_apl_log_end();
        return mdbcom::Result::OK;;
    }

}



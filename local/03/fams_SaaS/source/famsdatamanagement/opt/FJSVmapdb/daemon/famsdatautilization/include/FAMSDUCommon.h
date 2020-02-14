/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUCommon.h
 * @brief       file which defines common data and method
 * @author      anhnguyen
 * @date        2018/12/10
 */

#ifndef FAMS_DATA_UTIL_COMMON_H_
#define FAMS_DATA_UTIL_COMMON_H_
#include <sstream>
#include <array>
#include <math.h>
#include "MDBConstants.h"
#include "FAMSCommon.h"
#include "FAMSDUUserData.h"
//#include "FAMSDUObserversData.h"
#include "FAMSDUTaskSummaryData.h"
//#include "FAMSDUVersionData.h"
#include "MDBTypes.h"
#include "MDBContentDecodingFormat.h"
#include "MDBJsonParser.h"
#include "MDBContentAnalyzer.h"

#define AMMONIUM_ERR_AT "4"
#define NITRITE_ERR_AT "5"
#define NITRATE_ERR_AT "6"
#define OCCURRING_ERRMSG 1
#define RESOLVED_ERRMSG 2

namespace famsdatautil {

    /* ---------------- Request Path Definition ---------------- */
    static const std::string URI_LOGIN_PATH { famscommon::API_VERSION + "/login" };
	static const std::string URI_LOGOUT_PATH{ famscommon::API_VERSION + "/logout" };
    static const std::string URI_GET_USERS_PATH { famscommon::API_VERSION + "/users" };
    static const std::string URI_TASKS_PATH { famscommon::API_VERSION + "/tasks" };
    static const std::string URI_ORGANISM_LIST_PATH { famscommon::API_VERSION + "/organisms" };

    // Not-needed verifying authentication
    static std::vector<std::string> skipVerifyAuthLst {
        URI_LOGIN_PATH
    };
    /* --------------------------------------------------------- */
    /* ---------------------- Request Type---------------------- */
    enum RequestType
    {
        LOGIN_POST = 0,
		LOGOUT_POST,
        USERS_GET,
        TASKS_GET,
        ORGANISM_LIST_GET,
        ORGANISM_LIST_POST
    };

    /* --------------------------------------------------------- */


    //! Select user and validate user
    extern const std::string SELECT_LOGIN_USER_QUERY;

    //! Select user where user ID
    extern const std::string SELECT_USERS_QUERY;

	extern const std::string SELECT_THRESHOLD_DATA;

	extern const std::string INSERT_ERROR_MESSAGE;

	extern const std::string UPDATE_STATUS_ERROR_MESSAGE;
	extern const std::string UPDATE_STATUS_VIOLATED_ERROR_MESSAGE;

    extern const std::string SELECT_ALL_TASKS_QUERY;

    extern const std::string POST_ORGANISM_QUERY_INFO;
    extern const std::string PUT_ORGANISM_QUERY_INFO;

    //! param indicating omit a key from condition in SQL statement
    extern const int OMIT_CONDITION_KEY;
    //! param indicating having to use a key from condition in SQL statement
    extern const int ADD_CONDITION_KEY;

    // Request Data
    static constexpr const char *const DATA { "data" };
	static constexpr const char* const ID{ "id" };
    static constexpr const char *const USER_ID { "userId" };
    static constexpr const char *const PASSWORD { "password" };
    static constexpr const char *const ACCESS_TOKEN { "accessToken" };
	static constexpr const char* const NAME{ "name" };
	static constexpr const char* const ROLE{ "role" };
	static constexpr const char* const MAIL{ "mail" };
	static constexpr const char* const ACTIVITIES{ "activities" };
	static constexpr const char* const TYPE{ "type" };
	static constexpr const char* const ASSIGNEE{ "assignee" };
	static constexpr const char* const DATE{ "date" };
	static constexpr const char* const STATUS{ "status" };
	static constexpr const char* const SUMMARY{ "summary" };
	static constexpr const char* const SPECIFICS{ "note" };
	static constexpr const char* const NOTE{ "specifics" };
	static constexpr const char* const CREATE_USER{ "createUser" };
	static constexpr const char* const CREATE_DATE{ "createDate" };
	static constexpr const char* const UPDATE_USER{ "updateUser" };
	static constexpr const char* const UPDATE_DATE{ "updateDate" };
	static constexpr const char* const THRESHOLDS{ "thresholds" };
	static constexpr const char* const RESULT{ "result" };
    //! define JSON keys in request and response data of Data Utilization
    class MDBApJsonKey {
    public:

        static constexpr const char *const CODE {"Code"};
        static constexpr const char *const MESSAGE {"Message"};
        static constexpr const char *const KEYS {"Keys"};
    };

    //! empty result sensor found
    static constexpr const char *const  EMPTY_ARRAY {"[]"}; /*pgr0746*/

    int authenticate(const std::string &username, const std::string &password, FAMSDUUserData &userData);
    inline std::string roundedUp2Digits(const std::string &strDouble) {
      if (strDouble == "") {
        return "";
      }
      std::stringstream ss(strDouble);
      double parsedValue;
      ss >> parsedValue;
      double roundedValue = roundf(parsedValue * 100) / 100;
      std::ostringstream returnValue;
      returnValue << roundedValue;
      return returnValue.str();
    }
}

#endif /* FAMS_DATA_UTIL_COMMON_H_ */

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
#include "FAMSDUObserversData.h"
#include "FAMSDUTaskSummaryData.h"
#include "FAMSDUVersionData.h"
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
    static const std::string URI_LOGIN_PATH { famscommon::API_VERSION + "/app/login" };
    static const std::string URI_GET_USERS_PATH { famscommon::API_VERSION + "/app/users" };
    static const std::string URI_GET_OBSERVERS_PATH { famscommon::API_VERSION + "/app/observers" };
    static const std::string URI_OBSERVERS_ITEM_PATH { famscommon::API_VERSION + "/app/observers/{OBSERVER_ID}" };
    static const std::string URI_VERSION_PATH { famscommon::API_VERSION + "/app/version" };
    static const std::string URI_TASKS_PATH { famscommon::API_VERSION + "/app/tasks" };
    static const std::string URI_TASK_PATH { famscommon::API_VERSION + "/app/tasks/{TASK_ID}" };
    static const std::string URI_GET_CISTERNS_PATH { famscommon::API_VERSION + "/app/cisterns" };
    static const std::string URI_ORGANISM_LIST_PATH { famscommon::API_VERSION + "/app/organisms" };
    static const std::string URI_ORGANISM_PATH { famscommon::API_VERSION + "/app/organisms/{ORGANISM_ID}"};
	static const std::string URI_ALL_TASKS_PATH{ famscommon::API_VERSION + "/app/alltasks" };

    // Not-needed verifying authentication
    static std::vector<std::string> skipVerifyAuthLst {
        URI_LOGIN_PATH,
        URI_GET_USERS_PATH,
        URI_GET_OBSERVERS_PATH,
        URI_VERSION_PATH,
        URI_TASKS_PATH,
        URI_TASK_PATH,
        URI_OBSERVERS_ITEM_PATH
    };
    /* --------------------------------------------------------- */
    /* ---------------------- Request Type---------------------- */
    enum RequestType
    {
        LOGIN_POST = 0,
        USERS_GET,
        OBSERVERS_GET,
        OBSERVERS_POST,
        OBSERVERS_PUT,
        OBSERVERS_DELETE,
        VERSION_GET,
        TASKS_GET,
        TASKS_POST,
        TASK_DELETE,
        TASK_GET,
        TASK_PUT,
        CISTERNS_GET,
        ORGANISM_LIST_GET,
        ORGANISM_LIST_POST,
        ORGANISM_GET,
        ORGANISM_PUT,
        ORGANISM_DELETE,
		ALL_TASKS_GET
    };

    /* --------------------------------------------------------- */


    //! Select user and validate user
    extern const std::string SELECT_LOGIN_USER_QUERY;

    //! Select user where user ID
    extern const std::string SELECT_USERS_QUERY;

    //! Select user where user ID
    extern const std::string SELECT_OBSERVERS_QUERY;
    extern const std::string SELECT_OBSERVERS_BY_EMAIL_QUERY;
    //! Select cisterns
    extern const std::string SELECT_CISTERNS_QUERY;
    //! Select camera
    extern const std::string SELECT_CAMERA_QUERY;
    //! Select lot
    extern const std::string SELECT_LOT_QUERY;
    //! Select error message
    extern const std::string SELECT_ERROR_MESSAGE_WHERE_TYPE_QUERY;
    extern const std::string INSERT_ERROR_MESSAGE;
    //! Delete error message
    extern const std::string UPDATE_STATUS_ERROR_MESSAGE;
    extern const std::string UPDATE_STATUS_VIOLATED_ERROR_MESSAGE;

    //! Select details of lot
    extern const std::string SELECT_DETAIL_LOT_QUERY;
    //! Select equipments
    extern const std::string SELECT_EQUIPMENT_QUERY;
    //! Select equipment detail
    extern const std::string SELECT_EQUIPMENT_DETAIL_QUERY;
    //! Select cistern monitor ion
    extern const std::string SELECT_CISTERN_MONITOR_ION_QUERY;
    //! Select monitor
    extern const std::string SELECT_MONITOR_QUERY;
    //! Select history in the first hour
    extern const std::string SELECT_MONITOR_HISTORY_QUERY;
    //! Select reference nitrification
    extern const std::string SELECT_REFERENCE_NITRIFICATION_QUERY;
    //! Select reference replace tank
    extern const std::string SELECT_REFERENCE_REPLACE_TANK_QUERY;
    //! Select reference outside
    extern const std::string SELECT_REFERENCE_OUTSIDE_QUERY;

    //! Select error message for monitor
    extern const std::string SELECT_ERROR_MESSAGE_MONITOR_WHERE_TYPE_QUERY;

    //! Select process time to check if inputed record is the lastest
    extern const std::string SELECT_PROCESS_TIME_TO_CHANGE_ERROR_STATUS;
	extern const std::string SELECT_ERROR_MESSAGES;
	extern const std::string SELECT_CISTERN_NAME;
	extern const std::string SELECT_TASK_MONITOR_INPUT;

    extern const std::string DELETE_OBSERVERS_QUERY;
    extern const std::string DELETE_TASK_QUERY;
    extern const std::string DELETE_TASK_POND_QUERY;
    extern const std::string DELETE_LOT_QUERY;

    extern const std::string SELECT_TASK_QUERY;
    extern const std::string SELECT_ALL_TASKS_QUERY;
    extern const std::string SELECT_TASK_SHIPPING_QUERY;

    extern const std::string POST_INSERT_LOT_QUERY;
    extern const std::string POST_LOT_ORGANISM_DETAIL_QUERY;
    extern const std::string POST_TASK_POND_INPUT_QUERY;
    extern const std::string POST_TASK_FEEDING_INPUT_QUERY;
    extern const std::string POST_TASK_RECOVERY_INPUT_QUERY;
    extern const std::string POST_TASK_DISPOSAL_INPUT_QUERY;
    extern const std::string POST_TASK_SHIPPING_INPUT_QUERY;
    extern const std::string POST_TASK_WATER_INPUT_QUERY;
    extern const std::string POST_TASK_MAINTENANCE_INPUT_QUERY;
    extern const std::string POST_TASK_WATER_MONITOR_INPUT_QUERY;
    extern const std::string POST_TASK_INPUT_DETAILS_QUERY;
    extern const std::string PUT_LOT_ORGANISM_DETAIL_QUERY;
    extern const std::string PUT_INSERT_LOT_QUERY;
    extern const std::string PUT_TASK_POND_INPUT_QUERY;
    extern const std::string PUT_TASK_FEEDING_INPUT_QUERY;
    extern const std::string PUT_TASK_RECOVERY_INPUT_QUERY;
    extern const std::string PUT_TASK_DISPOSAL_INPUT_QUERY;
    extern const std::string PUT_TASK_SHIPPING_INPUT_QUERY;
    extern const std::string PUT_TASK_WATER_INPUT_QUERY;
    extern const std::string PUT_TASK_MAINTENANCE_INPUT_QUERY;
    extern const std::string PUT_TASK_WATER_MONITOR_INPUT_QUERY;
    extern const std::string DELETE_TASK_INPUT_DETAILS_QUERY;

    extern const std::string SELECT_TASK_TYPE_QUERY;
    extern const std::string SELECT_TASK_POND_INPUT_QUERY;
    extern const std::string SELECT_TASK_FEEDING_INPUT_QUERY;
    extern const std::string SELECT_TASK_RECOVERY_INPUT_QUERY;
    extern const std::string SELECT_TASK_DISPOSAL_INPUT_QUERY;
    extern const std::string SELECT_TASK_SHIPPING_INPUT_QUERY;
    extern const std::string SELECT_TASK_WATER_INPUT_QUERY;
    extern const std::string SELECT_TASK_MAINTENANCE_INPUT_QUERY;
    extern const std::string SELECT_TASK_WATER_MONITOR_INPUT_QUERY;
    extern const std::string SELECT_TASK_INPUT_DETAILS_QUERY;

    extern const std::string SELECT_VERSION_QUERY;
    extern const std::string SELECT_THRESHOLD_DATA;

    extern const std::string POST_OBSERVERS_QUERY;
    extern const std::string PUT_OBSERVERS_QUERY;
    //! Prepare statement DB.
    extern const mdbcom::PrepareStatements dataUtilPreparedStatements;

    extern const std::string SELECT_ORGANISM_LIST_QUERY;
    extern const std::string SELECT_DETAIL_ORGANISM_QUERY;
    extern const std::string SELECT_ORGANISM_ITEM_QUERY;
    extern const std::string DELETE_ORGANISM_QUERY;
    extern const std::string POST_ORGANISM_QUERY_DETAIL;
    extern const std::string DELETE_ORGANISM_QUERY_DETAIL;
    extern const std::string POST_ORGANISM_QUERY_INFO;
    extern const std::string PUT_ORGANISM_QUERY_INFO;
	extern const std::string SELECT_TASKS_QUERY;
	extern const std::string SELECT_TASKS_INFOR_QUERY;
    //! param indicating omit a key from condition in SQL statement
    extern const int OMIT_CONDITION_KEY;
    //! param indicating having to use a key from condition in SQL statement
    extern const int ADD_CONDITION_KEY;

    // Request Data
    static constexpr const char *const DATA { "data" };
    static constexpr const char *const ID { "id" };
    static constexpr const char *const PASSWORD { "password" };
    static constexpr const char *const FULL_NAME { "fullName" };
    static constexpr const char *const USER_NAME { "userName" };
    static constexpr const char *const MAIL { "mail" };
    static constexpr const char *const VERSION { "version" };
    static constexpr const char *const RESULT { "result" };

    // Cistern Request Data
    static constexpr const char *const NAME { "name" };
    static constexpr const char *const CAMERA { "cameras" };
    static constexpr const char *const IP_ADDRESS { "ipAddress" };
    static constexpr const char *const LOTS { "lots" };
    static constexpr const char *const TAG { "tag" };
    static constexpr const char *const DESCRIPTION { "description" };
    static constexpr const char *const VALUE { "value" };
    static constexpr const char *const VALUES { "values" };
    static constexpr const char *const UNIT { "unit" };
    static constexpr const char *const ERROR_MESSAGES { "errorMessages" };
    static constexpr const char *const STATUS { "status" };
    static constexpr const char *const DETAILS { "details" };
    static constexpr const char *const EQUIPMENTS { "equipments" };
    static constexpr const char *const IMAGE { "image" };
    static constexpr const char *const MONITORS { "monitors" };
    static constexpr const char *const DATE { "date" };
    static constexpr const char *const HISTORY { "history" };
    static constexpr const char *const REFERENCES { "references" };

    //! Monitor Data
    enum SensorType
    {
        INFLOW_TEMP = 1,
        TURBIDITY,
        SALT,
        PH,
        AMMONIUM,
        NITRITE,
        NITRATE,
		DISSOLVED_OXYGEN
    };
    class TaskType {
    public:
        static const int POND = 1;
        static const int FEEDING = 2;
        static const int RECOVERY = 3;
        static const int DISPOSAL = 4;
        static const int SHIPPING = 5;
        static const int WATER = 6;
        static const int MAINTENANCE = 7;
        static const int MONITOR = 8;
    };
    static std::map<int, std::multimap<const std::string, std::string>> SensorTypeDetails {
        {
            SensorType::INFLOW_TEMP,
            {
                {
                    "name",
                    "水温"
                },
                {
                    "unit",
                    "℃"
                },
            }
        },
        {
            SensorType::TURBIDITY,
            {
                {
                    "name",
                    "濁度"
                },
                {
                    "unit",
                    "mg/L"
                },
            }
        },
        {
            SensorType::SALT,
            {
                {
                    "name",
                    "塩分濃度"
                },
                {
                    "unit",
                    "ppt"
                },
            }
        },
        {
            SensorType::PH,
            {
                {
                    "name",
                    "pH"
                },
                {
                    "unit",
                    ""
                },
            }
        },
		{
			SensorType::DISSOLVED_OXYGEN,
			{
				{
					"name",
					"溶存酸素"
				},
				{
					"unit",
					"mg/L"
				},
			}
		},
        {
            SensorType::AMMONIUM,
            {
                {
                    "name",
                    "アンモニウムイオン"
                },
                {
                    "unit",
                    "mg/l"
                },
            }
        },
        {
            SensorType::NITRITE,
            {
                {
                    "name",
                    "亜硝酸イオン "
                },
                {
                    "unit",
                    "mg/l"
                },
            }
        },
        {
            SensorType::NITRATE,
            {
                {
                    "name",
                    "硝酸イオン"
                },
                {
                    "unit",
                    "mg/l"
                },
            }
        },
        
    };

    static constexpr const char *const ERRORS { "errors" };
    static constexpr const char *const CODE { "code" };
    static constexpr const char *const MESSAGE { "message" };
    static constexpr const char *const THRESHOLDS { "thresholds" };
    static constexpr const char *const TYPE { "type" };
    static constexpr const char *const MIN_VALUE { "minValue" };
    static constexpr const char *const MAX_VALUE { "maxValue" };
    static constexpr const char *const MIN_UNIT { "minUnit" };
    static constexpr const char *const MAX_UNIT { "maxUnit" };

    static constexpr const char *const POND_INPUT { "pondInput" };
    static constexpr const char *const PROCESS_TIME { "processTime" };
    static constexpr const char *const PROCESS_USER_ID { "processUserId" };
    static constexpr const char *const LOT_INFORMATION_ID { "lotInformationId" };
    static constexpr const char *const CISTERN_INFORMATION_ID { "cisternInformationId" };
    static constexpr const char *const ORGANISM_INFORMATION_ID { "organismInformationId" };
    static constexpr const char *const ORGANISM_NUMBER { "organismNumber" };
    static constexpr const char *const ORGANISM_TAG { "organismTag" };
    static constexpr const char *const ORGANISM_UNIT { "organismUnit" };
    static constexpr const char *const ORGANISM_INITIAL_SIZE { "organismInitialSize" };
    static constexpr const char *const SHIPPABLE_TIME { "shippableTime" };
    static constexpr const char *const MEMO { "memo" };
    static constexpr const char *const FEEDING_INPUT { "feedingInput" };
    static constexpr const char *const FEEDING_NAME { "feedingName" };
    static constexpr const char *const AMOUNT { "amount" };
    static constexpr const char *const RECOVERY_INPUT { "recoveryInput" };
    static constexpr const char *const DISPOSAL_INPUT { "disposalInput" };
    static constexpr const char *const DISPOSAL_TYPE { "disposalType" };
    static constexpr const char *const SHIPPING_INPUT { "shippingInput" };
    static constexpr const char *const SHIPPING_DESTINATION_INPUT { "shippingDestination" };
    static constexpr const char *const WATER_INPUT { "waterInput" };
    static constexpr const char *const OPERATION { "operation" };
    static constexpr const char *const MAINTENANCE_INPUT { "maintenanceInput" };
    static constexpr const char *const MAINTENANCE_TYPE { "maintenanceType" };
    static constexpr const char *const WATER_MONITOR_INPUT { "waterMonitorInput" };
    static constexpr const char *const MONITOR_TYPE { "monitorType" };
    static constexpr const char *const MONITOR_VALUE { "monitorValue" };
	static constexpr const char* const PIC_USER_ID{ "picUserId" };
	static constexpr const char* const SUMMARY{ "summary" };
	static constexpr const char* const TASKS{ "tasks" };

    // Type in table error_messages
    const int ERROR_MSG_TYPE_LOT = 1;
    const int ERROR_MSG_TYPE_EQUIPMENT = 2;
    const int ERROR_MSG_TYPE_MONITOR = 3;
    const int LOT_STATUS_GROWING = 1;
    const int LOT_STATUS_SHIPPABLE = 2;

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

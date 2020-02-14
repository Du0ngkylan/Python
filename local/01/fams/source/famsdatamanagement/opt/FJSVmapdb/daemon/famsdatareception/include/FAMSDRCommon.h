/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDataReceptionCommon.h
 * @brief       Header file for define DataReception common data or macro.
 * @author      anhnguyen
 * @date        2018/12/11
 */

#ifndef FAMS_DATA_RECEPTION_COMMON_H_
#define FAMS_DATA_RECEPTION_COMMON_H_

#include <vector>
#include "MDBPreparedStatement.h"
#include "FAMSCommon.h"

namespace famsdatareception {

    // Define max length of some fields in JSON

    // Define the JSON key
    // ----------  request keys ----------------//
    //! Common JSON key REQUEST
    static constexpr const char *const DATA = "data";
    static constexpr const char *const TIMESTAMP = "timestamp";
    static constexpr const char *const WATERLEVEL = "water_level";
    static constexpr const char *const WATERTEMP = "water_temp";
    static constexpr const char *const ROOM_TEMP = "room_temp";
    static constexpr const char *const HUMIDITY = "humidity";
    static constexpr const char *const ATMOSPHERIC_PRESSURE = "atmospheric_pressure";
    static constexpr const char *const CISTERNCODE = "cistern_code";
    static constexpr const char *const INFLOWTEMP = "inflow_temp";
    static constexpr const char *const SALT = "salt";
    static constexpr const char *const PH = "pH";
    static constexpr const char *const OUTFLOWTEMP = "outflow_temp";
    static constexpr const char *const UPPERCENTRALILL = "upper_central_ill";
    static constexpr const char *const LOWERCENTRALILL = "lower_central_ill";
    static constexpr const char *const ERRORS = "errors";
    static constexpr const char *const CODE { "code" };
    static constexpr const char *const MESSAGE { "message" };


    /* ---------------- Request Path Definition ---------------- */
    static const std::string WATER_REPLACE_TANK_PATH { famscommon::API_VERSION_V1 + "/data/waterReplaceTank/update" };
    static const std::string CISTERN_PATH { famscommon::API_VERSION_V1 + "/data/cistern/update" };
    static const std::string NITRIFICATION_TANK_PATH { famscommon::API_VERSION_V1 + "/data/nitrificationTank/update" };
    static const std::string OUTSIDE_PATH { famscommon::API_VERSION_V1 + "/data/outside/update" };
    /* --------------------------------------------------------- */

    // ----------  request keys ----------------//
 
    //! JSON common key Result
    static constexpr const char *JSON_RESULT_KEY = "Result";
    static constexpr const char *JSON_ARRAY_NOTATION = "[]";
    static constexpr const char *JSON_CODE = "Code";
    static constexpr const char *JSON_MESSAGE = "Message";
    static constexpr const char *JSON_KEYS = "Keys";

    // define common constant
    static constexpr int RET_OK = 0;

    extern const std::vector<mdbcom::MDBPreparedStatement> dataReceptionPreparedStatements;

    // Obstacle : Response HTTP message
    static constexpr const char *HTTP_RESPONSE_CODE_1 = "型が正しくありません。"; /*pgr0746*/
    static constexpr const char *HTTP_RESPONSE_CODE_2 = "マッピング処理で異常が発生しました。"; /*pgr0746*/
}

#endif /* FAMS_DATA_RECEPTION_COMMON_H_ */

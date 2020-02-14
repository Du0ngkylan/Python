/* Copyright© 2020 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDataReceptionCommon.h
 * @brief       Header file for define DataReception common data or macro.
 * @author      ThangLQ
 * @date        2020/01/20
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
    static constexpr const char *const TIMESTAMP = "timeStamp";
    static constexpr const char *const RTUID = "rtuId";
    static constexpr const char *const SENSORDATA = "sensorData";
    static constexpr const char *const RTUCHANNEL = "rtuChannel";
    static constexpr const char *const VALUE = "value";
    static constexpr const char *const ERRORS = "errors";
    static constexpr const char *const CODE { "code" };
    static constexpr const char *const MESSAGE { "message" };


    /* ---------------- Request Path Definition ---------------- */
    static const std::string SENSOR_ADD_PATH { famscommon::API_VERSION_V1 + "/data/sensor/update" };
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

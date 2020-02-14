/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPCommon.cpp
 * @brief       Source file of common constants for event processing.
 * @author      anhnguyen
 * @date        2018/12/12
 *
 */

#include "FAMSEPCommon.h"
#include "MDBPreparedStatement.h"

namespace famseventprocessing {
    const std::string INSERT_SENSOR_DATA { "INSERT_SENSOR_DATA" };

    const std::vector<mdbcom::MDBPreparedStatement> sensorPreparedStatements =
    {
            // Insert replace tank data query
            {
				INSERT_SENSOR_DATA,
                "INSERT INTO sensor_data(timeStamp, rtuId, rtuChanel_1, rtuChanel_2, rtuChanel_3, rtuChanel_4, rtuChanel_5, rtuChanel_6,"
				" rtuChanel_7, rtuChanel_8) values ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10);", 10
            }
    };


}



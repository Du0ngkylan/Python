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
    const std::string SQL_SELECT_SENSOR_ID { "SQL_SELECT_SENSOR_ID" };
    const std::string INSERT_REPLACE_TANK_DATA { "INSERT_REPLACE_TANK_DATA" };
    const std::string SELECT_THRESHOLD_DATA { "SELECT_THRESHOLD_DATA" };

    const std::string INSERT_SENSOR_CISTERN_DATA { "INSERT_SENSOR_CISTERN_DATA" };
    const std::string INSERT_SENSOR_NITRIFICATION_TANK_DATA { "INSERT_SENSOR_NITRIFICATION_TANK_DATA" };
    const std::string INSERT_SENSOR_OUTSIDE_TANK_DATA { "INSERT_SENSOR_OUTSIDE_TANK_DATA" };
    const std::string INSERT_ERROR_MESSAGE_DATA {"INSERT_ERROR_MESSAGE_DATA"};
    const std::string UPDATE_STATUS_RESOLVED_ERROR_MESSAGE_DATA {"UPDATE_STATUS_RESOLVED_ERROR_MESSAGE_DATA"};
    const std::string UPDATE_STATUS_VIOLATED_ERROR_MESSAGE_DATA {"UPDATE_STATUS_VIOLATED_ERROR_MESSAGE_DATA"};

    const std::string SELECT_LOT_INFORMATION_ID_DATA {"SELECT_LOT_INFORMATION_ID_DATA"};

    const std::string SELECT_OBSERVERS_QUERY {"SELECT_OBSERVERS_QUERY"};

    const std::vector<mdbcom::MDBPreparedStatement> cisternSensorPreparedStatements =
    {
            // Insert sensor cistern data query
            {
                INSERT_SENSOR_CISTERN_DATA,
                "INSERT INTO sensor_cistern_data(accumulated_time,cistern_information_id,inflow_temp,outflow_temp,upper_central_ill,lower_central_ill,salt, ph) "
                " SELECT $1 AS accumulated_time,"
                "id AS cistern_information_id,"
                "$2 AS inflow_temp,"
                "$3 AS outflow_temp,"
                "$4 AS upper_central_ill,"
                "$5 AS lower_central_ill,"
                "$6 AS salt,"
                "$7 AS ph"
                " FROM cistern_informations"
                " WHERE code = $8 returning id", 8
            },
            {
                SELECT_THRESHOLD_DATA,
                "SELECT DISTINCT(tpi.organism_information_id), tpi.cistern_information_id, ci.name as cistern_name, ori.name as organism_name, "
                "(CASE WHEN (ord.min_value IS NOT NULL AND ord.min_value > $3) THEN '基準以下' WHEN (ord.max_value IS NOT NULL AND ord.max_value < $3) THEN '基準超過' END) as violation "
                "FROM task_pond_inputs tpi, organism_informations ori, organism_details ord, cistern_informations ci "
                "WHERE ci.code = $1 "
                    "AND tpi.organism_information_id = ori.id "
                    "AND tpi.organism_information_id = ord.organism_information_id "
                    "AND ord.type = $2 "
                    "AND ci.id = tpi.cistern_information_id "
                    "AND ((ord.min_value IS NOT NULL AND ord.min_value > $3) OR (ord.max_value IS NOT NULL AND ord.max_value < $3));", 3
            },
            {
                INSERT_ERROR_MESSAGE_DATA,
                "INSERT INTO error_messages(error_message,happened_at,resource_id, resource_kind,status,error_at) VALUES ($1,$2,$3,$4,$5,$6)", 6
            },
            {
                UPDATE_STATUS_RESOLVED_ERROR_MESSAGE_DATA,
                "UPDATE error_messages SET status = $2 WHERE error_at = $1;", 2
            },
            {
                UPDATE_STATUS_VIOLATED_ERROR_MESSAGE_DATA,
                "UPDATE error_messages SET status = $3 WHERE error_at = $1 AND resource_id = $2;", 3
            },
            {
                SELECT_LOT_INFORMATION_ID_DATA,
                "select task_pond_inputs.lot_information_id as lot_information_id, lot_informations.tag as tag from task_pond_inputs, cistern_informations, lot_informations where task_pond_inputs.organism_information_id in (select id from organism_informations where name=$1) "
                "and task_pond_inputs.cistern_information_id = (select id from cistern_informations where name=$2) and lot_informations.id = task_pond_inputs.lot_information_id", 2
            },
            {
                SELECT_OBSERVERS_QUERY,
                "SELECT * FROM observer_informations", 0
            }

    };
    const std::vector<mdbcom::MDBPreparedStatement> nitrificationSensorPreparedStatements =
    {
            // Insert sensor nitrification tank data
            {
                INSERT_SENSOR_NITRIFICATION_TANK_DATA,
                "INSERT INTO sensor_nitrification_tank_data(accumulated_time, water_temp) values ($1, $2);", 2
            }
    };
    const std::vector<mdbcom::MDBPreparedStatement> outsideSensorPreparedStatements =
    {
            // Insert sensor outside tank data
            {
                INSERT_SENSOR_OUTSIDE_TANK_DATA,
                "INSERT INTO sensor_outside_data(accumulated_time, room_temp, humidity, atmospheric_pressure) values ($1, $2, $3, $4);", 4
            }
    };
    const std::vector<mdbcom::MDBPreparedStatement> sensorPreparedStatements =
    {
            // Insert replace tank data query
            {
                INSERT_REPLACE_TANK_DATA,
                "INSERT INTO sensor_replace_tank_data(accumulated_time, water_level, water_temp) values ($1, $2, $3);", 3
            }
    };


}



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
	const std::string SELECT_CISTERN_ID_QUERY{ "SELECT_CISTERN_ID_QUERY" };
	const std::string SELECT_LAST_SALT_SENSOR_CISTERN_DATA{ "SELECT_LAST_SALT_SENSOR_CISTERN_DATA" };
	const std::string SELECT_LAST_INFLOW_TEMP_SENSOR_CISTERN_DATA{ "SELECT_LAST_INFLOW_TEMP_SENSOR_CISTERN_DATA" };

	const std::string INSERT_SENSOR_DATA_PORT_TYPE1{ "INSERT_SENSOR_DATA_PORT_TYPE1" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE2{ "INSERT_SENSOR_DATA_PORT_TYPE2" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE3{ "INSERT_SENSOR_DATA_PORT_TYPE3" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE4{ "INSERT_SENSOR_DATA_PORT_TYPE4" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE5{ "INSERT_SENSOR_DATA_PORT_TYPE5" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE6{ "INSERT_SENSOR_DATA_PORT_TYPE6" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE7{ "INSERT_SENSOR_DATA_PORT_TYPE7" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE8{ "INSERT_SENSOR_DATA_PORT_TYPE8" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE9{ "INSERT_SENSOR_DATA_PORT_TYPE9" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE10{ "INSERT_SENSOR_DATA_PORT_TYPE10" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE11{ "INSERT_SENSOR_DATA_PORT_TYPE11" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE12{ "INSERT_SENSOR_DATA_PORT_TYPE12" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE13{ "INSERT_SENSOR_DATA_PORT_TYPE13" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE14{ "INSERT_SENSOR_DATA_PORT_TYPE14" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE15{ "INSERT_SENSOR_DATA_PORT_TYPE15" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE16{ "INSERT_SENSOR_DATA_PORT_TYPE16" };
	const std::string INSERT_SENSOR_DATA_PORT_TYPE17{ "INSERT_SENSOR_DATA_PORT_TYPE17" };
	const std::string SELECT_ERROR_MESSAGES{ "SELECT_ERROR_MESSAGES" };
	const std::string SELECT_SENSOR_VALUE_DATA{ "SELECT_SENSOR_VALUE_DATA" };
	const std::string SELECT_SENSOR_ADD_VALUE_DATA{ "SELECT_SENSOR_ADD_VALUE_DATA" };
	// set default integer values for keys
	/*
		1: Water temperature(tank inflow water) - tank
		2: Water temperature(aquarium effluent) - aquarium
		3: Illuminance(upper center of water tank) - Water tank
		4: Illuminance(bottom center of the tank) - tank
		5: Electric conductivity(salinity) - tank
		6: pH - water tank
		7: Dissolved oxygen - water tank
		8: Ammonium ion - water tank
		9: Nitrite ion - water tank
		10: nitrate ion - water tank
		11: Turbidity - Aquarium
		12: Water level - Water change tank
		13: Water temperature - Water exchange tank
		14: Water temperature - Nitrification tank
		15: Room temperature - Indoor
		16: Humidity - Indoor
		17: Barometric pressure - Indoor
	*/
	std::map<int, std::pair<std::string, int>> SQLstatementsMap
	{
		{1, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE1, 3)},
		{2, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE2, 3)},
		{3, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE3, 3)},
		{4, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE4, 3)},
		{5, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE5, 3)},
		{6, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE6, 3)},
		{7, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE7, 3)},
		{8, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE8, 3)},
		{9, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE9, 3)},
		{10, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE10, 3)},
		{11, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE11, 3)},
		{12, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE12, 2)},
		{13, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE13, 2)},
		{14, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE14, 2)},
		{15, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE15, 3)},
		{16, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE16, 3)},
		{17, std::make_pair(INSERT_SENSOR_DATA_PORT_TYPE17, 3)},
	};
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
                "UPDATE error_messages SET status = $2 WHERE error_at = $1 AND id in (SELECT em.id "
				"FROM lot_informations as li, cistern_informations as ci, error_messages as em "
				"WHERE ci.code = $3 and li.cistern_information_id = ci.id and em.resource_id = li.id);", 3
            },
            {
                UPDATE_STATUS_VIOLATED_ERROR_MESSAGE_DATA,
                "UPDATE error_messages SET status = $3 WHERE error_at = $1 AND resource_id = $2;", 3
            },
            {
                SELECT_LOT_INFORMATION_ID_DATA,
                "select distinct (task_pond_inputs.lot_information_id) as lot_information_id, lot_informations.tag as tag from task_pond_inputs, cistern_informations, lot_informations where task_pond_inputs.organism_information_id in (select id from organism_informations where name=$1) "
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
	const std::vector<mdbcom::MDBPreparedStatement> sensorAddPreparedStatements =
	{
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE1,
			"with upsert as (UPDATE sensor_cistern_data SET inflow_temp = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_cistern_data(accumulated_time, inflow_temp, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE2,
			"with upsert as (UPDATE sensor_cistern_data SET outflow_temp = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_cistern_data(accumulated_time, outflow_temp, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE3,
			"with upsert as (UPDATE sensor_cistern_data SET upper_central_ill = cast(ROUND($2, 0) as int) where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_cistern_data(accumulated_time, upper_central_ill, cistern_information_id) select $1, cast(ROUND($2, 0) as int), $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE4,
			"with upsert as (UPDATE sensor_cistern_data SET lower_central_ill = cast(ROUND($2, 0) as int) where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_cistern_data(accumulated_time, lower_central_ill, cistern_information_id) select $1, cast(ROUND($2, 0) as int), $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE5,
			"with upsert as (UPDATE sensor_cistern_data SET salt = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_cistern_data(accumulated_time, salt, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE6,
			"with upsert as (UPDATE sensor_cistern_data SET ph = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_cistern_data(accumulated_time, ph, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE7,
			"with upsert as (UPDATE sensor_additional_data SET air_flow_rate = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_additional_data(accumulated_time, air_flow_rate, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE8,
			"with upsert as (UPDATE sensor_additional_data SET ammonium_ion = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_additional_data(accumulated_time, ammonium_ion, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE9,
			"with upsert as (UPDATE sensor_additional_data SET nitrite_ion = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_additional_data(accumulated_time, nitrite_ion, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE10,
			"with upsert as (UPDATE sensor_additional_data SET nitrate_ion = $2 where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_additional_data(accumulated_time, nitrate_ion, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE11,
			"with upsert as (UPDATE sensor_cistern_data SET upper_central_ill = cast(ROUND($2,0) as int) where accumulated_time = $1 and cistern_information_id = $3 returning *)"
			" INSERT INTO sensor_cistern_data(accumulated_time, upper_central_ill, cistern_information_id) select $1, cast(ROUND($2,0) as int), $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE12,
			"with upsert as (UPDATE sensor_replace_tank_data SET water_level = $2 where accumulated_time = $1 returning *)"
			" INSERT INTO sensor_replace_tank_data(accumulated_time, water_level) select $1, $2 where not exists(select * from upsert);", 2
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE13,
			"with upsert as (UPDATE sensor_replace_tank_data SET water_temp = $2 where accumulated_time = $1 returning *)"
			" INSERT INTO sensor_replace_tank_data(accumulated_time, water_temp) select $1, $2 where not exists(select * from upsert);", 2
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE14,
			"with upsert as (UPDATE sensor_nitrification_tank_data SET water_temp = $2 where accumulated_time = $1 returning *)"
			" INSERT INTO sensor_nitrification_tank_data(accumulated_time, water_temp) select $1, $2 where not exists(select * from upsert);", 2
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE15,
			"with upsert as (UPDATE sensor_additional_data SET outdoor_temp = $2 where accumulated_time = $1 returning *)"
			" INSERT INTO sensor_additional_data(accumulated_time, outdoor_temp, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE16,
			"with upsert as (UPDATE sensor_additional_data SET outdoor_humidity = $2 where accumulated_time = $1 returning *)"
			" INSERT INTO sensor_additional_data(accumulated_time, outdoor_humidity, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		},
		// Insert sensor data query
		{
			INSERT_SENSOR_DATA_PORT_TYPE17,
			"with upsert as (UPDATE sensor_additional_data SET outdoor_atmospheric_pressure = $2 where accumulated_time = $1 returning *)"
			" INSERT INTO sensor_additional_data(accumulated_time, outdoor_atmospheric_pressure, cistern_information_id) select $1, $2, $3 where not exists(select * from upsert);", 3
		}
	};
	const std::vector<mdbcom::MDBPreparedStatement> cisternInformationsPreparedStatements =
	{
		{
			SELECT_CISTERN_ID_QUERY,
			"SELECT id, name FROM cistern_informations WHERE code = $1;", 1
		},
		{
			SELECT_LAST_SALT_SENSOR_CISTERN_DATA,
			"SELECT salt from sensor_cistern_data where accumulated_time = (SELECT max(accumulated_time) FROM (SELECT * FROM sensor_cistern_data WHERE salt IS NOT NULL) as tbl)", 0
		},
		{
			SELECT_LAST_INFLOW_TEMP_SENSOR_CISTERN_DATA,
			"SELECT inflow_temp from sensor_cistern_data where accumulated_time = (SELECT max(accumulated_time) FROM (SELECT * FROM sensor_cistern_data WHERE inflow_temp IS NOT NULL) as tbl)", 0
		},
		{
			SELECT_ERROR_MESSAGES,
			"SELECT error_message, happened_at, resource_id, resource_kind, error_at FROM error_messages WHERE resource_id in (SELECT lot_information_id from task_pond_inputs"
			" WHERE cistern_information_id = $1) and status = 1", 1
		},
		{
			SELECT_SENSOR_VALUE_DATA,
			"SELECT (CASE WHEN ($3 = 0) THEN 'inflow_temp' WHEN ($3 = 1) THEN 'outflow_temp' WHEN ($3 = 2) THEN 'ph' WHEN ($3 = 3) THEN 'salt' END) as value FROM sensor_cistern_data"
			" WHERE cistern_information_id = $1 AND accumulated_time = $2;", 3
		},
		{
			SELECT_SENSOR_ADD_VALUE_DATA,
			"select inflow_temp, outflow_temp, salt, ph, air_flow_rate, ammonium_ion, nitrite_ion, nitrate_ion from (select * FROM sensor_cistern_data sc"
			" where sc.cistern_information_id = $1 AND sc.accumulated_time = $2) as sai  Full JOIN (select * from sensor_additional_data sa  WHERE"
			" sa.cistern_information_id = $1 AND sa.accumulated_time = $2) as sci on sai.cistern_information_id = sci.cistern_information_id;", 2
		}
	};
}



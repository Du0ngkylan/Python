/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUCommon.cpp
 * @brief       source file for define common data or macro.
 * @author      anhnguyen
 * @date        2018/12/10
 */

#include <cstdlib>
#include "FAMSDUCommon.h"
#include "MDBConnectionManager.h"

namespace famsdatautil {

    const std::string SELECT_LOGIN_USER_QUERY { "SELECT_LOGIN_USER_QUERY" };

    const std::string SELECT_USERS_QUERY { "SELECT_USERS_QUERY" };
    // Observers
    const std::string SELECT_OBSERVERS_QUERY { "SELECT_OBSERVERS_QUERY" };
    const std::string SELECT_OBSERVERS_BY_EMAIL_QUERY { "SELECT_OBSERVERS_BY_EMAIL_QUERY" };

    const std::string SELECT_CISTERNS_QUERY { "SELECT_CISTERNS_QUERY" };
    const std::string SELECT_LOT_QUERY { "SELECT_LOT_QUERY" };
    const std::string SELECT_CAMERA_QUERY { "SELECT_CAMERA_QUERY" };
    const std::string SELECT_ERROR_MESSAGE_WHERE_TYPE_QUERY { "SELECT_ERROR_MESSAGE_WHERE_TYPE_QUERY" };
    const std::string SELECT_ERROR_MESSAGE_MONITOR_WHERE_TYPE_QUERY { "SELECT_ERROR_MESSAGE_MONITOR_WHERE_TYPE_QUERY" };
    const std::string SELECT_PROCESS_TIME_TO_CHANGE_ERROR_STATUS { "SELECT_PROCESS_TIME_TO_CHANGE_ERROR_STATUS" };
    
    const std::string INSERT_ERROR_MESSAGE { "INSERT_ERROR_MESSAGE" };
    const std::string UPDATE_STATUS_ERROR_MESSAGE { "UPDATE_STATUS_ERROR_MESSAGE" };
    const std::string UPDATE_STATUS_VIOLATED_ERROR_MESSAGE { "UPDATE_STATUS_VIOLATED_ERROR_MESSAGE" };

    const std::string SELECT_DETAIL_LOT_QUERY { "SELECT_DETAIL_LOT_QUERY" };
    const std::string SELECT_EQUIPMENT_QUERY { "SELECT_EQUIPMENT_QUERY" };
    const std::string SELECT_EQUIPMENT_DETAIL_QUERY { "SELECT_EQUIPMENT_DETAIL_QUERY" };
    const std::string SELECT_CISTERN_MONITOR_ION_QUERY { "SELECT_CISTERN_MONITOR_ION_QUERY" };
    const std::string SELECT_MONITOR_QUERY { "SELECT_MONITOR_QUERY" };
    const std::string SELECT_MONITOR_HISTORY_QUERY { "SELECT_MONITOR_HISTORY_QUERY" };
    const std::string SELECT_REFERENCE_NITRIFICATION_QUERY { "SELECT_REFERENCE_NITRIFICATION_QUERY" };
    const std::string SELECT_REFERENCE_REPLACE_TANK_QUERY { "SELECT_REFERENCE_REPLACE_TANK_QUERY" };
    const std::string SELECT_REFERENCE_OUTSIDE_QUERY { "SELECT_REFERENCE_OUTSIDE_QUERY" };

    const std::string POST_OBSERVERS_QUERY { "POST_OBSERVERS_QUERY" };
    const std::string PUT_OBSERVERS_QUERY { "PUT_OBSERVERS_QUERY" };
    const std::string DELETE_OBSERVERS_QUERY { "DELETE_OBSERVERS_QUERY" };
    // Task
    const std::string DELETE_TASK_QUERY { "DELETE_TASK_QUERY" };
    const std::string DELETE_TASK_POND_QUERY { "DELETE_TASK_POND_QUERY" };
    const std::string DELETE_LOT_QUERY { "DELETE_LOT_QUERY" };

    const std::string SELECT_TASK_QUERY { "SELECT_TASK_QUERY" };

    const std::string POST_INSERT_LOT_QUERY { "POST_INSERT_LOT_QUERY" };
    const std::string POST_LOT_ORGANISM_DETAIL_QUERY { "POST_LOT_ORGANISM_DETAIL_QUERY" };
    const std::string POST_TASK_POND_INPUT_QUERY { "POST_TASK_POND_INPUT_QUERY" };
    const std::string POST_TASK_FEEDING_INPUT_QUERY { "POST_TASK_FEEDING_INPUT_QUERY" };
    const std::string POST_TASK_RECOVERY_INPUT_QUERY { "POST_TASK_RECOVERY_INPUT_QUERY" };
    const std::string POST_TASK_DISPOSAL_INPUT_QUERY { "POST_TASK_DISPOSAL_INPUT_QUERY" };
    const std::string POST_TASK_SHIPPING_INPUT_QUERY { "POST_TASK_SHIPPING_INPUT_QUERY" };
    const std::string POST_TASK_WATER_INPUT_QUERY { "POST_TASK_WATER_INPUT_QUERY" };
    const std::string POST_TASK_MAINTENANCE_INPUT_QUERY { "POST_TASK_MAINTENANCE_INPUT_QUERY" };
    const std::string POST_TASK_WATER_MONITOR_INPUT_QUERY { "POST_TASK_WATER_MONITOR_INPUT_QUERY" };
    const std::string POST_TASK_INPUT_DETAILS_QUERY { "POST_TASK_INPUT_DETAILS_QUERY" };

    const std::string PUT_INSERT_LOT_QUERY { "PUT_INSERT_LOT_QUERY" };
    const std::string PUT_LOT_ORGANISM_DETAIL_QUERY { "PUT_LOT_ORGANISM_DETAIL_QUERY" };
    const std::string PUT_TASK_POND_INPUT_QUERY { "PUT_TASK_POND_INPUT_QUERY" };
    const std::string PUT_TASK_FEEDING_INPUT_QUERY { "PUT_TASK_FEEDING_INPUT_QUERY" };
    const std::string PUT_TASK_RECOVERY_INPUT_QUERY { "PUT_TASK_RECOVERY_INPUT_QUERY" };
    const std::string PUT_TASK_DISPOSAL_INPUT_QUERY { "PUT_TASK_DISPOSAL_INPUT_QUERY" };
    const std::string PUT_TASK_SHIPPING_INPUT_QUERY { "PUT_TASK_SHIPPING_INPUT_QUERY" };
    const std::string PUT_TASK_WATER_INPUT_QUERY { "PUT_TASK_WATER_INPUT_QUERY" };
    const std::string PUT_TASK_MAINTENANCE_INPUT_QUERY { "PUT_TASK_MAINTENANCE_INPUT_QUERY" };
    const std::string PUT_TASK_WATER_MONITOR_INPUT_QUERY { "PUT_TASK_WATER_MONITOR_INPUT_QUERY" };
    const std::string DELETE_TASK_INPUT_DETAILS_QUERY { "DELETE_TASK_INPUT_DETAILS_QUERY" };

    const std::string SELECT_TASK_TYPE_QUERY { "SELECT_TASK_TYPE_QUERY" };
    const std::string SELECT_TASK_POND_INPUT_QUERY { "SELECT_TASK_POND_INPUT_QUERY" };
    const std::string SELECT_TASK_FEEDING_INPUT_QUERY { "SELECT_TASK_FEEDING_INPUT_QUERY" };
    const std::string SELECT_TASK_RECOVERY_INPUT_QUERY { "SELECT_TASK_RECOVERY_INPUT_QUERY" };
    const std::string SELECT_TASK_DISPOSAL_INPUT_QUERY { "SELECT_TASK_DISPOSAL_INPUT_QUERY" };
    const std::string SELECT_TASK_SHIPPING_INPUT_QUERY { "SELECT_TASK_SHIPPING_INPUT_QUERY" };
    const std::string SELECT_TASK_WATER_INPUT_QUERY { "SELECT_TASK_WATER_INPUT_QUERY" };
    const std::string SELECT_TASK_MAINTENANCE_INPUT_QUERY { "SELECT_TASK_MAINTENANCE_INPUT_QUERY" };
    const std::string SELECT_TASK_WATER_MONITOR_INPUT_QUERY { "SELECT_TASK_WATER_MONITOR_INPUT_QUERY" };
    const std::string SELECT_TASK_INPUT_DETAILS_QUERY { "SELECT_TASK_INPUT_DETAILS_QUERY" };
    // Tasks
    const std::string SELECT_ALL_TASKS_QUERY { "SELECT_ALL_TASKS_QUERY" };
    const std::string SELECT_TASK_SHIPPING_QUERY { "SELECT_TASK_SHIPPING_QUERY" };

    const std::string SELECT_VERSION_QUERY { "SELECT_VERSION_QUERY" };
    const std::string SELECT_THRESHOLD_DATA { "SELECT_THRESHOLD_DATA" };

    const std::string SELECT_ORGANISM_ITEM_QUERY { "SELECT_ORGANISM_ITEM_QUERY" };
    const std::string SELECT_DETAIL_ORGANISM_QUERY { "SELECT_DETAIL_ORGANISIM_QUERY" };
    const std::string SELECT_ORGANISM_LIST_QUERY { "SELECT_ORGANISM_LIST_QUERY" };
    const std::string DELETE_ORGANISM_QUERY { "DELETE_ORGANISM_QUERY" };
    const std::string POST_ORGANISM_QUERY_DETAIL { "POST_ORGANISM_QUERY_DETAIL" };
    const std::string DELETE_ORGANISM_QUERY_DETAIL { "DETAIL_ORGANISM_QUERY_DETAIL" };
    const std::string POST_ORGANISM_QUERY_INFO { "POST_ORGANISM_QUERY_INFO" };
    const std::string PUT_ORGANISM_QUERY_INFO { "PUT_ORGANISM_QUERY_INFO" };


    const std::vector<mdbcom::MDBPreparedStatement> dataUtilPreparedStatements =
    {
        // Insert prepared statement
        {
            SELECT_LOGIN_USER_QUERY,
            "SELECT id, username, full_name, mail FROM user_informations WHERE username = $1 AND password = $2", 2
        },
        {
            SELECT_USERS_QUERY,
            "SELECT * FROM user_informations", 0
        },
        {
            SELECT_OBSERVERS_QUERY,
            "SELECT * FROM observer_informations", 0
        },
        {
            SELECT_OBSERVERS_BY_EMAIL_QUERY,
            "SELECT * FROM observer_informations WHERE mail=$1", 1
        },
        {
            SELECT_CISTERNS_QUERY,
            "SELECT * FROM cistern_informations", 0
        },
        {
            SELECT_LOT_QUERY,
            "SELECT * FROM lot_informations WHERE cistern_information_id = $1", 1
        },
        {
            SELECT_CAMERA_QUERY,
            "SELECT * FROM cistern_camera_details WHERE cistern_information_id = $1", 1
        },
        {
            SELECT_ERROR_MESSAGE_WHERE_TYPE_QUERY,
            "SELECT DISTINCT ON (error_message) error_message, error_at FROM error_messages WHERE resource_id = $1 AND (resource_kind = $2 OR resource_kind = $3) AND status = 1 ORDER BY error_message DESC, happened_at DESC", 3
        },
        {
            SELECT_ERROR_MESSAGE_MONITOR_WHERE_TYPE_QUERY,
            "SELECT DISTINCT ON (error_message) error_message, error_at FROM error_messages WHERE resource_id IN (SELECT id from lot_informations WHERE cistern_information_id = $1 ) AND (happened_at = $2 OR happened_at = $3) and status = 1 ORDER BY error_message DESC, error_at ASC, happened_at DESC", 3
        },
        {
            SELECT_PROCESS_TIME_TO_CHANGE_ERROR_STATUS,
            "SELECT process_time FROM task_monitor_inputs WHERE cistern_information_id = $1 AND monitor_type = $2 AND process_time > $3", 3
        },
        {
            SELECT_DETAIL_LOT_QUERY,
            "WITH decrease_amount_tbl AS ("
            " SELECT COALESCE((SELECT sum(amount)"
            " FROM task_input_details"
            " WHERE lot_information_id = $1 AND (resource_kind = 3 or resource_kind = 4 or resource_kind = 5)"
            " GROUP BY  lot_information_id),0) AS decrease_amount"
            ")"
            "SELECT process_time,organism_number - decrease_amount_tbl.decrease_amount as organism_number"
            ",organism_initial_size,shippable_time,"
            " CASE WHEN shippable_time <= NOW() THEN 1 ELSE 2 END AS status"
            " FROM task_pond_inputs, decrease_amount_tbl"
            " WHERE lot_information_id = $1"
            " ORDER BY process_time DESC LIMIT 1", 1
        },
        {
            SELECT_EQUIPMENT_QUERY,
            "SELECT * FROM equipment_informations WHERE cistern_information_id = $1", 1
        },
        {
            SELECT_EQUIPMENT_DETAIL_QUERY,
            "SELECT * FROM equipment_details WHERE equipment_information_id = $1", 1
        },
        {
            SELECT_CISTERN_MONITOR_ION_QUERY,
            "WITH tmp AS (SELECT max(process_time) AS maxDate from task_monitor_inputs)"
            "SELECT ammonium.ammonium_process_time, nitrite.nitrite_process_time, nitrate.nitrate_process_time, ammonium.ammonium_ion_value, nitrite.nitrite_ion_value, nitrate.nitrate_ion_value"
            " FROM "
            " ("
            " SELECT"
            "  process_time AS ammonium_process_time,"
            "  monitor_value AS ammonium_ion_value"
            " FROM task_monitor_inputs, tmp WHERE cistern_information_id = $1 AND monitor_type = 1 AND process_time <= tmp.maxDate AND process_time >= tmp.maxDate - '30 day'::INTERVAL"
            " ) as ammonium"
            " FULL OUTER JOIN"
            "("
            " SELECT"
            "  process_time AS nitrite_process_time,"
            "  monitor_value AS nitrite_ion_value"
            " FROM task_monitor_inputs, tmp WHERE cistern_information_id = $1 AND monitor_type = 2 AND process_time <= tmp.maxDate AND process_time >= tmp.maxDate - '30 day'::INTERVAL"
            ") as nitrite"
            " ON ammonium.ammonium_process_time = nitrite.nitrite_process_time"
            " FULL OUTER JOIN"
            "("
            " SELECT"
            "  process_time AS nitrate_process_time,"
            "  monitor_value AS nitrate_ion_value"
            " FROM task_monitor_inputs, tmp WHERE cistern_information_id = $1 AND monitor_type = 3 AND process_time <= tmp.maxDate AND process_time >= tmp.maxDate - '30 day'::INTERVAL"
            ") as nitrate"
            " ON nitrite.nitrite_process_time = nitrate.nitrate_process_time"
            " ORDER BY ammonium.ammonium_process_time DESC", 1
        },
        {
            SELECT_MONITOR_QUERY,
            " SELECT 1 as id, accumulated_time, process_time,"
            " ammonium_ion, nitrite_ion, nitrate_ion, inflow_temp, outflow_temp,"
            " turbidity, salt, ph"
            " FROM (("
            " SELECT accumulated_time, inflow_temp, outflow_temp,"
            " cast(lower_central_ill AS decimal) - cast(upper_central_ill AS decimal) AS turbidity, salt, ph"
            " FROM sensor_cistern_data"
            " WHERE accumulated_time = (SELECT max(accumulated_time) FROM sensor_cistern_data)"
            " AND cistern_information_id = $1) AS foo"
            " FULL OUTER JOIN("
            " SELECT process_time, max(amoni_value) as ammonium_ion, max(nitri_value) as nitrite_ion, max(nitra_value) as nitrate_ion"
            " FROM ("
            " SELECT process_time,"
            " (CASE WHEN monitor_type = 1 THEN monitor_value ELSE NULL END) AS amoni_value,"
            " (CASE WHEN monitor_type = 2 THEN monitor_value ELSE NULL END) AS  nitri_value,"
            " (CASE WHEN monitor_type = 3 THEN monitor_value ELSE NULL END) AS nitra_value"
            " FROM task_monitor_inputs"
            " WHERE (process_time = (SELECT max(process_time) FROM task_monitor_inputs))"
            " AND cistern_information_id = $1) AS tmp_table"
            " GROUP BY process_time) AS goo"
            " ON 1=1)", 1
        },
        {
            SELECT_MONITOR_HISTORY_QUERY,
            "WITH tmp AS (SELECT max(accumulated_time) as maxDate from sensor_cistern_data)"
            "("
            " SELECT"
            "  date_part('hour', tmp.maxDate - accumulated_time) as group_condition_col,"
            "  accumulated_time AS accumulated_time_rs,"
            "  inflow_temp AS inflow_temp_rs,"
            "  outflow_temp AS outflow_temp_rs,"
            "  salt AS salt_rs,"
            "  ph AS ph_rs,"
            "  cast(lower_central_ill AS decimal) - cast(upper_central_ill AS decimal) AS turbidity_rs"
            " FROM sensor_cistern_data, tmp WHERE cistern_information_id = $1 AND accumulated_time <= tmp.maxDate AND accumulated_time >= tmp.maxDate - '1 hour'::INTERVAL"
            ")"
            "UNION"
            "("
            " SELECT"
            "  date_part('hour', tmp.maxDate - accumulated_time) as group_condition_col,"
            "  max(tmp.maxDate - date_part('hour', tmp.maxDate - accumulated_time) * INTERVAL '1 hour') as accumulated_time_rs,"
            "  avg(inflow_temp) AS inflow_temp_rs,"
            "  avg(outflow_temp) AS outflow_temp_rs,"
            "  avg(salt) AS salt_rs,"
            "  avg(ph) AS ph_rs,"
            "  avg(cast(lower_central_ill AS decimal) - cast(upper_central_ill AS decimal)) AS turbidity_rs"
            " FROM sensor_cistern_data, tmp"
            " WHERE cistern_information_id = $1 AND accumulated_time < tmp.maxDate - '1 hour'::INTERVAL AND accumulated_time >= tmp.maxDate - '24 hour'::INTERVAL"
            "  GROUP BY group_condition_col"
            ")"
            " ORDER BY accumulated_time_rs DESC", 1
        },
        {
            SELECT_REFERENCE_NITRIFICATION_QUERY,
            "SELECT * FROM sensor_nitrification_tank_data ORDER BY accumulated_time DESC LIMIT 1", 0
        },
        {
            SELECT_REFERENCE_REPLACE_TANK_QUERY,
            "SELECT * FROM sensor_replace_tank_data ORDER BY accumulated_time DESC LIMIT 1", 0
        },
        {
            SELECT_REFERENCE_OUTSIDE_QUERY,
            "SELECT * FROM sensor_outside_data ORDER BY accumulated_time DESC LIMIT 1", 0
        },
        {
             POST_OBSERVERS_QUERY,
             "INSERT INTO observer_informations(mail) values ($1) returning id;", 1
        },
        {
            PUT_OBSERVERS_QUERY,
            "UPDATE observer_informations SET mail = $1 WHERE id = $2 returning *", 2
        },
        {
            SELECT_ORGANISM_LIST_QUERY,
            "SELECT id, name FROM organism_informations", 0
        },
        {
              SELECT_ORGANISM_ITEM_QUERY,
             "SELECT name FROM organism_informations WHERE id = $1", 1
        },
        {
            SELECT_DETAIL_ORGANISM_QUERY,
            "SELECT type, min_value, min_unit, max_value, max_unit "
            "FROM organism_details "
            "WHERE organism_information_id = $1", 1
        },
        {
            DELETE_ORGANISM_QUERY,
            "DELETE FROM organism_informations WHERE id = $1 returning *", 1
        },
        {
            DELETE_OBSERVERS_QUERY,
            "DELETE FROM observer_informations WHERE id = $1 returning *", 1
        },
        {
            SELECT_ALL_TASKS_QUERY,
            "("
            " SELECT task_info_tbl.id, task_info_tbl.type, pond_tbl.process_time, pond_tbl.process_user_id, organism_tbl.name || ' ' || pond_tbl.organism_number as summary"
            " FROM task_informations task_info_tbl, task_pond_inputs pond_tbl, organism_informations organism_tbl"
            " WHERE task_info_tbl.id = pond_tbl.task_information_id"
            " AND organism_tbl.id = pond_tbl.organism_information_id"
            " )"
            " UNION"
            " ("
            " select task_info_tbl.id, task_info_tbl.type, task_feeding_tbl.process_time, task_feeding_tbl.process_user_id, task_feeding_tbl.feeding_name || ' ' || feeding_detail.sum_amount as summary"
            " from task_informations as task_info_tbl, task_feeding_inputs as task_feeding_tbl, (select sum(amount) as sum_amount, resource_id as task_feeding_input_id"
            " from task_input_details"
            " where resource_kind = 2"
            " group by resource_id) as feeding_detail"
            " where task_info_tbl.id = task_feeding_tbl.task_information_id"
            " and feeding_detail.task_feeding_input_id = task_feeding_tbl.id"
            " )"
            " UNION"
            " ("
            " select task_info_tbl.id, task_info_tbl.type, task_water_tbl.process_time, task_water_tbl.process_user_id, "
            " CASE WHEN task_water_tbl.operation=1 THEN '注水（海洋深層水）'"
            " WHEN task_water_tbl.operation=2 THEN '注水（濾過海水）'"
            " WHEN task_water_tbl.operation=3 THEN '注水（人工海水）'"
            " WHEN task_water_tbl.operation=4 THEN '注水（淡水）'"
            " WHEN task_water_tbl.operation=5 THEN '注水（その他）'"
            " ELSE '排水' END"
            " || ' ' || task_water_tbl.amount as summary"
            " from task_informations as task_info_tbl, task_water_inputs as task_water_tbl"
            " where task_info_tbl.id = task_water_tbl.task_information_id"
            " )"
            " UNION"
            " ("
            " select task_info_tbl.id, task_info_tbl.type, task_main_tbl.process_time, task_main_tbl.process_user_id, "
            " CASE WHEN task_main_tbl.maintenance_type=1 THEN '掃除'"
			" WHEN task_main_tbl.maintenance_type=2 THEN 'ろ材交換'"
			" WHEN task_main_tbl.maintenance_type=3 THEN '機器交換'"
			" ELSE 'その他' END as summary"
            " from task_informations as task_info_tbl, task_maintenance_inputs as task_main_tbl"
            " where task_info_tbl.id = task_main_tbl.task_information_id"
            " )"
            " UNION"
            " ("
            " select task_info_tbl.id, task_info_tbl.type, task_monitor_tbl.process_time, task_monitor_tbl.process_user_id, "
            " CASE WHEN task_monitor_tbl.monitor_type=1 THEN 'アンモニウムイオン'"
			" WHEN task_monitor_tbl.monitor_type=2 THEN '亜硝酸イオン'"
			" WHEN task_monitor_tbl.monitor_type=3 THEN '硝酸イオン'"
			" WHEN task_monitor_tbl.monitor_type=4 THEN 'DO'"
			" WHEN task_monitor_tbl.monitor_type=5 THEN 'モニタ項目A'"
			" WHEN task_monitor_tbl.monitor_type=6 THEN 'モニタ項目B'"
			" WHEN task_monitor_tbl.monitor_type=7 THEN 'モニタ項目C'"
			" ELSE 'その他' END"
            " || ' ' || task_monitor_tbl.monitor_value as summary"
            " from task_informations as task_info_tbl, task_monitor_inputs as task_monitor_tbl"
            " where task_info_tbl.id = task_monitor_tbl.task_information_id"
            " )"
            " UNION"
            " ("
            " select task_info_tbl.id, task_info_tbl.type, task_recovery_tbl.process_time, task_recovery_tbl.process_user_id, "
            " string_agg(org_tbl.name || ' ' || task_detail_tbl.amount, ', ') as summary"
            " from task_informations as task_info_tbl, task_recovery_inputs as task_recovery_tbl, task_input_details as task_detail_tbl,"
            " lot_organism_details as lot_org_detail_tbl, organism_informations as org_tbl"
            " where task_info_tbl.id = task_recovery_tbl.task_information_id"
            " and task_detail_tbl.resource_kind = task_info_tbl.type"
            " and task_detail_tbl.resource_id = task_recovery_tbl.id"
            " and lot_org_detail_tbl.lot_information_id = task_detail_tbl.lot_information_id"
            " and org_tbl.id = lot_org_detail_tbl.organism_information_id"
            " group by task_info_tbl.id, task_info_tbl.type, task_recovery_tbl.process_time, task_recovery_tbl.process_user_id"
            " )"
            " UNION"
            " ("
            " select task_info_tbl.id, task_info_tbl.type, task_disposal_tbl.process_time, task_disposal_tbl.process_user_id, "
            " string_agg(org_tbl.name || ' ' || task_detail_tbl.amount, ', ') as summary"
            " from task_informations as task_info_tbl, task_disposal_inputs as task_disposal_tbl, task_input_details as task_detail_tbl,"
            " lot_organism_details as lot_org_detail_tbl, organism_informations as org_tbl"
            " where task_info_tbl.id = task_disposal_tbl.task_information_id"
            " and task_detail_tbl.resource_kind = task_info_tbl.type"
            " and task_detail_tbl.resource_id = task_disposal_tbl.id"
            " and lot_org_detail_tbl.lot_information_id = task_detail_tbl.lot_information_id"
            " and org_tbl.id = lot_org_detail_tbl.organism_information_id"
            " group by task_info_tbl.id, task_info_tbl.type, task_disposal_tbl.process_time, task_disposal_tbl.process_user_id"
            " )"
            " UNION"
            " ("
            " select task_info_tbl.id, task_info_tbl.type, task_shipping_tbl.process_time, task_shipping_tbl.process_user_id, "
            " string_agg(org_tbl.name || ' ' || task_detail_tbl.amount, ', ') as summary"
            " from task_informations as task_info_tbl, task_shipping_inputs as task_shipping_tbl, task_input_details as task_detail_tbl,"
            " lot_organism_details as lot_org_detail_tbl, organism_informations as org_tbl"
            " where task_info_tbl.id = task_shipping_tbl.task_information_id"
            " and task_detail_tbl.resource_kind = task_info_tbl.type"
            " and task_detail_tbl.resource_id = task_shipping_tbl.id"
            " and lot_org_detail_tbl.lot_information_id = task_detail_tbl.lot_information_id"
            " and org_tbl.id = lot_org_detail_tbl.organism_information_id"
            " group by task_info_tbl.id, task_info_tbl.type, task_shipping_tbl.process_time, task_shipping_tbl.process_user_id"
            " )"
            " ORDER BY process_time DESC", 0
        },
        {
            SELECT_TASK_SHIPPING_QUERY,
            "select task_info_tbl.id, task_shipping_tbl.shipping_destination"
            " from task_informations as task_info_tbl, task_shipping_inputs as task_shipping_tbl"
            " where task_info_tbl.id = task_shipping_tbl.task_information_id", 0
        },
        {
            DELETE_TASK_QUERY,
            "DELETE FROM task_informations WHERE id = $1 returning *", 1
        },
        {
            DELETE_TASK_POND_QUERY,
            "DELETE FROM task_pond_inputs WHERE task_information_id = $1 returning lot_information_id", 1
        },
        {
            DELETE_LOT_QUERY,
            "DELETE FROM lot_informations WHERE id = $1 returning *", 1
        },
        {
            POST_INSERT_LOT_QUERY,
            "insert into lot_informations(name, description, tag, unit, cistern_information_id) "
            " values($1,$2,$3,$4,$5) returning id ", 5
        },
        {
            POST_LOT_ORGANISM_DETAIL_QUERY,
            "insert into lot_organism_details(organism_information_id, lot_information_id)"
            " values($1,$2) returning id ", 2
        },
        {
            POST_TASK_POND_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (1)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_pond_inputs ("
            " process_time,"
            " process_user_id,"
            " lot_information_id,"
            " cistern_information_id,"
            " organism_information_id,"
            " organism_number,"
            " organism_tag,"
            " organism_unit,"
            " organism_initial_size,"
            " shippable_time,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,$4,$5,$6,$7,$8,$9,(NULLIF($10, '')::timestamp with time zone),$11,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_pond_inputs.id ", 11
        },
        {
            POST_TASK_FEEDING_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (2)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_feeding_inputs ("
            " process_time,"
            " process_user_id,"
            " feeding_name,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,$4,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_feeding_inputs.id", 4
        },
        {
            POST_TASK_RECOVERY_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (3)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_recovery_inputs ("
            " process_time,"
            " process_user_id,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_recovery_inputs.id", 3
        },
        {
            POST_TASK_DISPOSAL_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (4)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_disposal_inputs ("
            " process_time,"
            " process_user_id,"
            " disposalType,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,$4,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_disposal_inputs.id", 4
        },
        {
            POST_TASK_SHIPPING_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (5)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_shipping_inputs ("
            " process_time,"
            " process_user_id,"
            " shipping_destination,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,$4,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_shipping_inputs.id", 4
        },
        {
            POST_TASK_WATER_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (6)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_water_inputs ("
            " process_time,"
            " process_user_id,"
            " cistern_information_id,"
            " operation,"
            " amount,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,$4,"
            "(CASE WHEN ($5 = '') THEN NULL "
            "WHEN ($5 <> '') THEN $5 "
            "END)::decimal,"
            "$6,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_water_inputs.id", 6
        },
        {
            POST_TASK_MAINTENANCE_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (7)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_maintenance_inputs ("
            " process_time,"
            " process_user_id,"
            " cistern_information_id,"
            " maintenance_type,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,$4,$5,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_maintenance_inputs.id", 5
        },
        {
            POST_TASK_WATER_MONITOR_INPUT_QUERY,
            "WITH tasks AS ("
            "    INSERT INTO task_informations(type)"
            "    VALUES (8)"
            "    RETURNING id AS task_id"
            "    )"
            " INSERT INTO task_monitor_inputs ("
            " process_time,"
            " process_user_id,"
            " cistern_information_id,"
            " monitor_type,"
            " monitor_value,"
            " memo,"
            " task_information_id"
            " )"
            " SELECT $1,$2,$3,$4,$5,$6,task_id"
            " FROM tasks"
            " RETURNING task_information_id, task_monitor_inputs.id", 6
        },
        {
            POST_TASK_INPUT_DETAILS_QUERY,
            "insert into task_input_details(resource_kind, resource_id, lot_information_id, amount) \
            SELECT $1,$2, $3, \
            (CASE WHEN ($4 = '') THEN NULL \
            WHEN ($4 <> '') THEN $4 \
            END)::decimal", 4
			//            " values($1,$2,$3,$4) ", 4
        },
        {
            PUT_INSERT_LOT_QUERY,
            "update lot_informations"
            " set "
            " name = $1,"
            " description = $2,"
            " tag = $3,"
            " unit = $4,"
            " cistern_information_id = $5"
            " where id in (select lot_information_id from task_pond_inputs where task_information_id = $6)"
            " returning id", 6
        },
        {
            PUT_LOT_ORGANISM_DETAIL_QUERY,
            "update lot_organism_details"
            " set "
            " organism_information_id = $1"
            " where lot_information_id in (select lot_information_id from task_pond_inputs where task_information_id = $2)"
            " returning id", 2
        },
        {
            PUT_TASK_POND_INPUT_QUERY,
            "update task_pond_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " lot_information_id = $3,"
            " cistern_information_id = $4,"
            " organism_information_id = $5,"
            " organism_number = $6,"
            " organism_tag = $7,"
            " organism_unit = $8,"
            " organism_initial_size = $9,"
            " shippable_time = "
            "(NULLIF($10, '')::timestamp),"
			" memo = $11"
            " where task_information_id = $12"
            " returning id", 12
        },
        {
            PUT_TASK_FEEDING_INPUT_QUERY,
            "update task_feeding_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " feeding_name = $3,"
            " memo = $4"
            " where task_information_id = $5"
            " returning id", 5
        },
        {
            PUT_TASK_RECOVERY_INPUT_QUERY,
            "update task_recovery_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " memo = $3"
            " where task_information_id = $4"
            " returning id", 4
        },
        {
            PUT_TASK_DISPOSAL_INPUT_QUERY,
            "update task_disposal_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " disposalType = $3,"
            " memo = $4"
            " where task_information_id = $5"
            " returning id", 5
        },
        {
            PUT_TASK_SHIPPING_INPUT_QUERY,
            "update task_shipping_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " shipping_destination = $3,"
            " memo = $4"
            " where task_information_id = $5"
            " returning id", 5
        },
        {
            PUT_TASK_WATER_INPUT_QUERY,
            "update task_water_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " cistern_information_id = $3,"
            " operation = $4,"
            " amount = "
            "(CASE WHEN ($5 = '') THEN NULL "
            "WHEN ($5 <> '') THEN $5 "
            "END)::decimal,"
            " memo = $6"
            " where task_information_id = $7"
            " returning id", 7
        },
        {
            PUT_TASK_MAINTENANCE_INPUT_QUERY,
            "update task_maintenance_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " cistern_information_id = $3,"
            " maintenance_type = $4,"
            " memo = $5"
            " where task_information_id = $6"
            " returning id", 6
        },
        {
            PUT_TASK_WATER_MONITOR_INPUT_QUERY,
            "update task_monitor_inputs"
            " set "
            " process_time = $1,"
            " process_user_id = $2,"
            " cistern_information_id = $3,"
            " monitor_type = $4,"
            " monitor_value = $5,"
            " memo = $6"
            " where task_information_id = $7"
            " returning id", 7
        },
        {
            DELETE_TASK_INPUT_DETAILS_QUERY,
            "delete from task_input_details where resource_kind = $1 and resource_id = $2", 2
        },
        {
            SELECT_TASK_TYPE_QUERY,
            "select type from task_informations where id = $1", 1
        },
        {
            SELECT_TASK_POND_INPUT_QUERY,
            "select * from task_pond_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_FEEDING_INPUT_QUERY,
            "select * from task_feeding_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_RECOVERY_INPUT_QUERY,
            "select * from task_recovery_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_DISPOSAL_INPUT_QUERY,
            "select * from task_disposal_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_SHIPPING_INPUT_QUERY,
            "select * from task_shipping_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_WATER_INPUT_QUERY,
            "select * from task_water_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_MAINTENANCE_INPUT_QUERY,
            "select * from task_maintenance_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_WATER_MONITOR_INPUT_QUERY,
            "select * from task_monitor_inputs where task_information_id = $1", 1
        },
        {
            SELECT_TASK_INPUT_DETAILS_QUERY,
            "select * from task_input_details where resource_kind = $1 and resource_id = $2", 2
        },
        {
            SELECT_VERSION_QUERY,
            "SELECT * FROM version_informations", 0
        },
//                         Column          |          Type          |                           Modifiers                           | Storage  | Stats target | Description
//        -------------------------+------------------------+---------------------------------------------------------------+----------+--------------+-------------
//         id                      | bigint                 | not null default nextval('organism_details_id_seq'::regclass) | plain    |              |
//         organism_information_id | bigint                 | not null                                                      | plain    |              |
//         type                    | smallint               | not null                                                      | plain    |              |
//         min_value               | double precision       |                                                               | plain    |              |
//         min_unit                | character varying(100) |                                                               | extended |              |
//         max_value               | double precision       |                                                               | plain    |              |
//         max_unit                | character varying(100) |                                                               | extended |              |
        {
            POST_ORGANISM_QUERY_DETAIL,
//            "INSERT INTO organism_details(organism_information_id,type,min_value,min_unit,max_value,max_unit) values ($1,$2,$3,$4,$5,$6);", 6
			"insert into organism_details (organism_information_id,type,min_value,min_unit,max_value,max_unit) \
			SELECT $1,$2, \
			(CASE WHEN ($3 = '') THEN NULL \
			WHEN ($3 <> '') THEN $3 \
			END)::decimal ,$4, \
			(CASE WHEN ($5 = '') THEN NULL \
			WHEN ($5 <> '') THEN $5 \
			END)::decimal,$6", 6

        },
//             Column |          Type          |                             Modifiers                              | Storage  | Stats target | Description
//            --------+------------------------+--------------------------------------------------------------------+----------+--------------+-------------
//             id     | bigint                 | not null default nextval('organism_informations_id_seq'::regclass) | plain    |              |
//             name   | character varying(100) | not null                                                           | extended |              |

        {
            POST_ORGANISM_QUERY_INFO,
            "INSERT INTO organism_informations(name) values ($1) returning id;", 1
        },
        {
            PUT_ORGANISM_QUERY_INFO,
            "UPDATE organism_informations SET name = $1 WHERE id = $2", 2
        },
        {
            DELETE_ORGANISM_QUERY_DETAIL,
            "DELETE FROM organism_details WHERE organism_information_id = $1 returning *", 1
        },
        {
            SELECT_THRESHOLD_DATA,
            "SELECT DISTINCT(tpi.organism_information_id), tpi.cistern_information_id, ci.name as cistern_name, ori.name as organism_name, tpi.lot_information_id, ord.type error_at, "
                "(CASE WHEN (ord.type = 4) THEN 'ammonium_ion' WHEN (ord.type = 5) THEN 'nitrite_ion' WHEN (ord.type = 6) THEN 'nitrate_ion' END) as monitor_name, "
                "(CASE WHEN (ord.min_value IS NOT NULL AND ord.min_value > $3) THEN '基準以下' WHEN (ord.max_value IS NOT NULL AND ord.max_value < $3) THEN '基準超過' END) as violation "
            "FROM task_pond_inputs tpi, organism_informations ori, organism_details ord, cistern_informations ci "
            "WHERE ci.id = $1 "
                "AND tpi.organism_information_id = ori.id "
                "AND tpi.organism_information_id = ord.organism_information_id "
                "AND ord.type = $2 "
                "AND ci.id = tpi.cistern_information_id "
                "AND ((ord.min_value IS NOT NULL AND ord.min_value > $3) OR (ord.max_value IS NOT NULL AND ord.max_value < $3));", 3
        }
        ,
        {
            INSERT_ERROR_MESSAGE,
            "INSERT INTO error_messages(error_message, happened_at, resource_id, resource_kind, status, error_at) "
            "VALUES($1, $2, $3, 3, $4, $5);", 5
        },
        {
            UPDATE_STATUS_ERROR_MESSAGE,
            "UPDATE error_messages SET status = $2 where error_at = $1;", 2
        },
        {
            UPDATE_STATUS_VIOLATED_ERROR_MESSAGE,
            "UPDATE error_messages SET status = $3 where error_at = $1 AND resource_id = $2;", 3
        }

    };

    //! param indicating omit a key from condition in SQL statement
    const int OMIT_CONDITION_KEY
    {
        0
    };
    //! param indicating having to use a key from condition in SQL statement
    const int ADD_CONDITION_KEY
    {
        1
    };

     // Authenticate username/password
    int authenticate(const std::string &username, const std::string &password, FAMSDUUserData &userData) {
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
        params.emplace_back(username);
        params.emplace_back(password);

        auto selectStm = dbConnection.execute(SELECT_LOGIN_USER_QUERY, params); /*pgr0745*/
        auto retExec = selectStm->getResult();
        int result = mdbcom::Result::NG;
        if (mdbcom::Result::OK == retExec)
        {
            if (selectStm->getNumRows() == 1) {
                long long id;
                std::string username;
                std::string mail;
                std::string fullName;
                selectStm->getValue(0, selectStm->getColumnIndex("id"), id);
                selectStm->getValue(0, selectStm->getColumnIndex("username"), username);
                selectStm->getValue(0, selectStm->getColumnIndex("mail"), mail);
                selectStm->getValue(0, selectStm->getColumnIndex("full_name"), fullName);
                userData.setId(id);
                userData.setUsername(username);
                userData.setMail(mail);
                userData.setFullName(fullName);

                result = mdbcom::Result::OK;
                mdb_apl_log_debug("SQL select successfully");
            }
        }
        else
        {
            mdb_apl_log_error("SQL select failed! ret=%d", retExec);
        }
        mdb_apl_log_debug("SQL select successfully");
        return result;
    }

}

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

	const std::string SELECT_THRESHOLD_DATA{ "SELECT_THRESHOLD_DATA" };

	const std::string INSERT_ERROR_MESSAGE{ "INSERT_ERROR_MESSAGE" };
	const std::string UPDATE_STATUS_ERROR_MESSAGE{ "UPDATE_STATUS_ERROR_MESSAGE" };
	const std::string UPDATE_STATUS_VIOLATED_ERROR_MESSAGE{ "UPDATE_STATUS_VIOLATED_ERROR_MESSAGE" };
    // Tasks
    const std::string SELECT_ALL_TASKS_QUERY { "SELECT_ALL_TASKS_QUERY" };

    const std::string POST_ORGANISM_QUERY_INFO { "POST_ORGANISM_QUERY_INFO" };
    const std::string PUT_ORGANISM_QUERY_INFO { "PUT_ORGANISM_QUERY_INFO" };
    const std::vector<mdbcom::MDBPreparedStatement> dataUtilPreparedStatements =
    {
        // Insert prepared statement
        {
            SELECT_LOGIN_USER_QUERY,
            "SELECT id, userId, name, mail, role, activities FROM user_informations WHERE username = $1 AND password = $2", 2
        },
        {
            SELECT_USERS_QUERY,
            "SELECT * FROM user_informations", 0
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
            POST_ORGANISM_QUERY_INFO,
            "INSERT INTO organism_informations(name) values ($1) returning id;", 1
        },
        {
            PUT_ORGANISM_QUERY_INFO,
            "UPDATE organism_informations SET name = $1 WHERE id = $2", 2
        },
		{
			SELECT_TASKS_INFOR_QUERY,
			"SELECT id, type FROM task_informations", 0
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
                std::string userId;
                std::string mail;
                std::string name;
				std::string role;
				std::string activities;
                selectStm->getValue(0, selectStm->getColumnIndex("id"), id);
                selectStm->getValue(0, selectStm->getColumnIndex("userId"), userId);
                selectStm->getValue(0, selectStm->getColumnIndex("mail"), mail);
                selectStm->getValue(0, selectStm->getColumnIndex("name"), name);
				selectStm->getValue(0, selectStm->getColumnIndex("role"), role);
				selectStm->getValue(0, selectStm->getColumnIndex("activities"), activities);
                userData.setId(id);
                userData.setUserId(userId);
                userData.setMail(mail);
                userData.setName(name);
				userData.setRole(role);
				userData.setActivities(activities);
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

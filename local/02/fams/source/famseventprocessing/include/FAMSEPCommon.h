/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved.  */
/*
 * @file        FAMSEPCommon.h
 * @brief       Header file to define common things.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_EVENT_PROCESSING_COMMON_H_
#define FAMS_EVENT_PROCESSING_COMMON_H_
#include <vector>
#include <array>
#include "MDBPreparedStatement.h"
#include "MDBJsonParser.h"

//Define marco for resource ID
#define RESOURCE_KIND_LOT 1
#define RESOURCE_KIND_EQUIPMENT 2
#define RESOURCE_KIND_MONITOR 3

//Define error message status 
#define OCCURING_ERROR_ST 1
#define RESOLVED_ERROR_ST 2

//Define error message at
#define WATER_TMP_IN_ERR 0
#define WATER_TMP_OUT_ERR 1
#define WATER_TMP_SALT_ERR 3
#define WATER_TMP_PH_ERR 2
#define WATER_TMP_TUBB_ERR 8
#define WATER_TMP_AMMONI_ERR 4
#define WATER_TMP_NITRITE_ERR 5
#define WATER_TMP_NITRATE_ERR 6
#define WATER_TMP_DISSOL_ERR 7

namespace famseventprocessing {

   

    extern const std::vector<mdbcom::MDBPreparedStatement> sensorPreparedStatements;
    extern const std::vector<mdbcom::MDBPreparedStatement> sensorThresholdStatements;
    extern const std::vector<mdbcom::MDBPreparedStatement> cisternSensorPreparedStatements;
    extern const std::vector<mdbcom::MDBPreparedStatement> nitrificationSensorPreparedStatements;
    extern const std::vector<mdbcom::MDBPreparedStatement> outsideSensorPreparedStatements;
	extern const std::vector<mdbcom::MDBPreparedStatement> sensorAddPreparedStatements;
	extern const std::vector<mdbcom::MDBPreparedStatement> cisternInformationsPreparedStatements;
    // SQL CONST for sensor
	extern const std::string INSERT_SENSOR_DATA;
    extern const std::string INSERT_REPLACE_TANK_DATA;
    extern const std::string SELECT_THRESHOLD_DATA;
    extern const std::string INSERT_SENSOR_CISTERN_DATA;
    extern const std::string INSERT_SENSOR_NITRIFICATION_TANK_DATA;
    extern const std::string INSERT_SENSOR_OUTSIDE_TANK_DATA;

    // SQL for insert information into error messages table.
    extern const std::string INSERT_ERROR_MESSAGE_DATA;
    // SQL for update status error messages table.
    extern const std::string UPDATE_STATUS_RESOLVED_ERROR_MESSAGE_DATA;
    extern const std::string UPDATE_STATUS_VIOLATED_ERROR_MESSAGE_DATA;

    //SQL select lot_information_id over threshold
    extern const std::string SELECT_LOT_INFORMATION_ID_DATA;
    extern const std::string SELECT_OBSERVERS_QUERY;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE1;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE2;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE3;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE4;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE5;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE6;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE7;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE8;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE9;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE10;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE11;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE12;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE13;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE14;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE15;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE16;
	extern const std::string INSERT_SENSOR_DATA_PORT_TYPE17;
	extern const std::string SELECT_CISTERN_ID_QUERY;
	extern const std::string SELECT_LAST_SALT_SENSOR_CISTERN_DATA;
	extern const std::string SELECT_LAST_INFLOW_TEMP_SENSOR_CISTERN_DATA;
	extern const std::string SELECT_ERROR_MESSAGES;
	extern const std::string SELECT_SENSOR_VALUE_DATA;
	extern const std::string SELECT_SENSOR_ADD_VALUE_DATA;
	//! map to hold port type and SQL statement
	extern std::map<int, std::pair<std::string, int>> SQLstatementsMap;

}

#endif /* FAMS_EVENT_PROCESSING_COMMON_H_ */

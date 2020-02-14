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
#define NUMBER_OF_CHANEL 8
namespace famseventprocessing {

    extern const std::vector<mdbcom::MDBPreparedStatement> sensorPreparedStatements;
    // SQL CONST for sensor
    extern const std::string INSERT_SENSOR_DATA;

}

#endif /* FAMS_EVENT_PROCESSING_COMMON_H_ */

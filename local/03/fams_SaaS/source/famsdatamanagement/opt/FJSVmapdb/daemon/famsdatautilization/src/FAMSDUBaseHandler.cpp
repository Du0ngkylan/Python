/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPNitrificationHandler.cpp
 * @brief       Handler Obstacle Detection
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include <string>
#include <vector>
#include <set>

#include "MDBConnectionManager.h"
#include "MDBDatabaseParam.h"
#include "MDBConstants.h"
#include "MDBCommonUtility.h"
#include "FAMSDUBaseHandler.h"


namespace famsdatautil {


    int FAMSDUBaseHandler::checkSensorValue(std::string convertTime, const double value, const int type, const long long cistern_id, const int statusOfThisRecord) {

        mdb_apl_log_start();
        // Declare variable
        int numRows = 0;
        std::vector<std::string> errKeys;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {cistern_id, type, value};

        auto selectStm = dbConnection.execute(SELECT_THRESHOLD_DATA, params); /*pgr0745*/

        auto retExec = selectStm->getResult();

        if (mdbcom::Result::OK == retExec) {
            numRows = selectStm->getNumRows();
            mdb_apl_log_debug("number of Violation: %d", numRows);

            std::string err_msg; // monitor_name + violation + organism_name
            std::string cisternName;
            std::string organismName;
            std::string monitorName;
            std::string error_at;
            std::string violation;
            long long lot_information_id;

            for (int i=0; i < numRows; i++) {
                // selectStm->getValue(i, selectStm->getColumnIndex("violation"), title);
                selectStm->getValue(i, selectStm->getColumnIndex("cistern_name"), cisternName);
                selectStm->getValue(i, selectStm->getColumnIndex("organism_name"), organismName);
                selectStm->getValue(i, selectStm->getColumnIndex("monitor_name"), monitorName);
                selectStm->getValue(i, selectStm->getColumnIndex("error_at"), error_at);
                selectStm->getValue(i, selectStm->getColumnIndex("violation"), violation);
                selectStm->getValue(i, selectStm->getColumnIndex("lot_information_id"), lot_information_id);

//                organism_information_id | cistern_information_id | min_value | max_value | cistern_name | organism_name | lot_information_id | error_at | monitor_name | violation
//               -------------------------+------------------------+-----------+-----------+--------------+---------------+--------------------+----------+--------------+-----------
//                                      3 |                      2 |         6 |         8 | Nuoi Ca      | Ca            |                 39 |        3 | XYZ          | BELOW


//                id |            error_message             |        happened_at        | resource_id | resource_kind | status | error_at
//               ----+--------------------------------------+---------------------------+-------------+---------------+--------+----------
//                 2 | ABC:BELOW(マナマコ)                  | 2015-05-14 22:30:45.25+07 |          40 |            99 |     99 |        1


                char buffer[3000];
                if(error_at == AMMONIUM_ERR_AT)
                {
                    int rtTemp = snprintf(buffer, 10000, "アンモニウムイオン: %s (%s)", violation.c_str(), organismName.c_str());
                }
                else if (error_at == NITRITE_ERR_AT)
                {
                    int rtTemp = snprintf(buffer, 10000, "亜硝酸イオン: %s (%s)", violation.c_str(), organismName.c_str());
                }
                else if(error_at == NITRATE_ERR_AT)
                {
                    int rtTemp = snprintf(buffer, 10000, "硝酸イオン: %s (%s)", violation.c_str(), organismName.c_str());
                }
                else
                {
                    //do nothing.
                }
                err_msg = buffer;

                //add error messages
                auto numMsgRecordeds = registerErrorMessages(convertTime, err_msg, (int)lot_information_id, std::stoi(error_at), statusOfThisRecord);
            }

        }
        else {
            mdb_apl_log_debug("failed to get observers info");
        }

        mdb_apl_log_end();
        return numRows;
    }

    int FAMSDUBaseHandler::changeStatusResolved(const int type){
        mdb_apl_log_start();
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {};
        params.emplace_back(type);
        params.emplace_back(RESOLVED_ERRMSG);
        auto selectStm = dbConnection.execute(UPDATE_STATUS_ERROR_MESSAGE, params); /*pgr0745*/
        auto retExec = selectStm->getResult();

        if (mdbcom::Result::OK == retExec) {

        }
        else {
            mdb_apl_log_debug("Failed to update status error message");
        }
    }

    int FAMSDUBaseHandler::changeStatusViolated(const int type, const int resourceId){
        mdb_apl_log_start();
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {};
        params.emplace_back(type);
        params.emplace_back(resourceId);
        params.emplace_back(OCCURRING_ERRMSG);
        auto selectStm = dbConnection.execute(UPDATE_STATUS_VIOLATED_ERROR_MESSAGE, params); /*pgr0745*/
        auto retExec = selectStm->getResult();

        if (mdbcom::Result::OK == retExec) {

        }
        else {
            mdb_apl_log_debug("Failed to update status error message");
        }
    }

    int FAMSDUBaseHandler::registerErrorMessages(std::string convertTime, std::string error_messages, const int lot_information_id, const int error_at, const int statusOfThisRecord) {

        mdb_apl_log_start();
        // Declare variable
        int numRows = 0;
        std::vector<std::string> errKeys;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {};

//                id |            error_message             |        happened_at        | resource_id | resource_kind | status | error_at
//               ----+--------------------------------------+---------------------------+-------------+---------------+--------+----------
//                 2 | ABC:BELOW(マナマコ)                  | 2015-05-14 22:30:45.25+07 |          40 |            99 |     99 |        1
        params.emplace_back(error_messages);
        params.emplace_back(convertTime);
        params.emplace_back(lot_information_id);
        params.emplace_back(statusOfThisRecord);
        params.emplace_back(error_at);
        auto selectStm = dbConnection.execute(INSERT_ERROR_MESSAGE, params); /*pgr0745*/
        auto retExec = selectStm->getResult();

        if (mdbcom::Result::OK == retExec) {

        }
        else {
            mdb_apl_log_debug("Failed to insert error message");
        }

        //update status = 1 for each violated lot
        //changeStatusViolated(error_at, lot_information_id);

        mdb_apl_log_end();
        return numRows;    }
}

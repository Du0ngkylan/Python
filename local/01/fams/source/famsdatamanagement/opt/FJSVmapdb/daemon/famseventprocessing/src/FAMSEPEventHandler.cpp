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
#include "FAMSEPEventHandler.h"


namespace famseventprocessing {


    int FAMSEPEventHandler::checkSensorValue(const double value, const int type, const std::string cistern_code,
            std::vector<std::string> &titles, std::vector<std::string> &cisternNames, std::string accumulatedTime, std::vector<std::string> &organishNames, std::vector<std::string> &distinguishNames) {
        mdb_apl_log_start();
        // Declare variable
        int numRows = 0;
        int addMoreNumRows = 0;
        std::vector<std::string> errKeys;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();

        //set type check for Water inflow, outflow = 1
        int typeTmp = type;
        if (typeTmp == 0){
            typeTmp = 1;
        }

        auto params = std::vector<mdbcom::MDBDatabaseParam> {cistern_code, typeTmp, value};
        auto selectStm = dbConnection.execute(SELECT_THRESHOLD_DATA, params); /*pgr0745*/

        auto retExec = selectStm->getResult();

        if (mdbcom::Result::OK == retExec) {
            numRows = selectStm->getNumRows();
            mdb_apl_log_debug("number of Violation: %d", numRows);
            titles.clear();
            cisternNames.clear();
            organishNames.clear();

            //update status = 2 for all lot which are no longer violated
            changeStatusResolved(type, RESOLVED_ERROR_ST);

            std::string title;
            std::string cisternName;
            std::string organismName;
            std::string dishtinguishName;
            std::string violation;
            int lot_information_id;
            std::string tag_lotInformation;

            for (int i=0; i < numRows; i++) {
                //Add error message
                // selectStm->getValue(i, selectStm->getColumnIndex("violation"), title);
                selectStm->getValue(i, selectStm->getColumnIndex("cistern_name"), cisternName);
                selectStm->getValue(i, selectStm->getColumnIndex("organism_name"), organismName);
                selectStm->getValue(i, selectStm->getColumnIndex("violation"), violation);
                mdb_apl_log_debug("Violation: %s", violation.c_str());
                char buffer[10000];
                if(type == WATER_TMP_PH_ERR)
                {
                    int rtTemp = snprintf(buffer, 10000, "pH: %s (%s)", violation.c_str(), organismName.c_str());
                }
                else if(type == WATER_TMP_SALT_ERR)
                {
                    int rtTemp = snprintf(buffer, 10000, "塩分: %s (%s)", violation.c_str(), organismName.c_str());
                }
                else if(type == WATER_TMP_IN_ERR)
                {
                    int rtTemp = snprintf(buffer, 10000, "水温（水槽流入水）: %s (%s)", violation.c_str(), organismName.c_str());
                }
				else if(type == WATER_TMP_OUT_ERR)
                {
                    int rtTemp = snprintf(buffer, 10000, "水温（水槽流出水）: %s (%s)", violation.c_str(), organismName.c_str());
                }
                // else if(type == WATER_TMP_TURBIDITY_ERR)
                // {
                //     int rtTemp = snprintf(buffer, 10000, "濁度: %s (%s)", violation.c_str(), organismName.c_str());
                // }
                else
                {
                    //do nothing
                    mdb_apl_log_debug(" Incorrect monitor type");
                }

                std::string errorMessageTemp(buffer);
                mdb_apl_log_debug("errorMessageTemp: %s", errorMessageTemp.c_str());
                title = errorMessageTemp;

                params.clear();
                params.emplace_back(organismName);
                params.emplace_back(cisternName);
                // Find lots which contain organisms are over their thresholds.
                auto selectStmTemp = dbConnection.execute(SELECT_LOT_INFORMATION_ID_DATA, params);
                params.clear();
                retExec = selectStmTemp->getResult();
                if(mdbcom::Result::OK == retExec)
                {
                    mdb_apl_log_debug("Number of Lots over threshold: %d", selectStmTemp->getNumRows());
                    for(int k=0; k < selectStmTemp->getNumRows(); k++)
                    {
                        selectStmTemp->getValue(k, selectStmTemp->getColumnIndex("lot_information_id"), lot_information_id);
                        selectStmTemp->getValue(k, selectStmTemp->getColumnIndex("tag"), tag_lotInformation);
                        addErrorMessage(errorMessageTemp, accumulatedTime, lot_information_id, RESOURCE_KIND_LOT, OCCURING_ERROR_ST, type);
                        distinguishNames.emplace_back(tag_lotInformation);
                        titles.emplace_back(title);
                        cisternNames.emplace_back(cisternName);
                        organishNames.emplace_back(organismName);
                        //increase number of violation when there is 2(or >2) lots that contain the same oraginsm but they have 2( or > 2) tags seperately.
                        addMoreNumRows  += (selectStmTemp->getNumRows() - 1);

                        //update status = 1 for each violated lot
                        //changeStatusViolated(type, lot_information_id, OCCURING_ERROR_ST);
                    }
                }
                // titles.emplace_back(title);
                // cisternNames.emplace_back(cisternName);
                // organishNames.emplace_back(organismName);

            }
        }
        else {
            mdb_apl_log_debug("failed to get observers info");
        }

        mdb_apl_log_end();
        numRows += addMoreNumRows;
        return numRows;
    }

    int FAMSEPEventHandler::getMailConfig(std::string &hostname, std::string &sender, std::string &subject, std::string &body,
            std::string &subjectEx, std::string &bodyEx, std::string &username, std::string &password, int &port, int &numViolationCfg) {
        //getMailconfit
        FAMSEPConfig &config = FAMSEPEventHandler::getEventProcessingConfig();
        hostname = config.getHostname();
        sender = config.getSender();
        subjectEx = config.getSujectExceed();
        bodyEx = config.getBodyExceed();
        username = config.getUsername();
        password = config.getPassword();
        port = std::stod(config.getPort());
        numViolationCfg = std::stod(config.getNumberViolation());
        subject = config.getSuject();
        body = config.getBody();

        return 0;
    }

    int FAMSEPEventHandler::getMailRecipents(std::vector<std::string> &recipients) {
        mdb_apl_log_start();
        int numRecipient = 0;
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(SELECT_OBSERVERS_QUERY);
        auto retExec = selectStm->getResult();
        std::string result;
        if (mdbcom::Result::OK == retExec)
        {
            numRecipient = selectStm->getNumRows();
            mdb_apl_log_debug(" Number of recipients = %d", numRecipient);
            for(int i = 0; i < numRecipient; i++) {
                std::string repient = "";
                selectStm->getValue(i, selectStm->getColumnIndex("mail"), repient);
                mdb_apl_log_debug(" === recipient = %s", repient.c_str());
                recipients.emplace_back(repient);
            }
        }
        else
        {
            mdb_apl_log_critical("Get recipients failed! ret=%d", retExec);
        }

        mdb_apl_log_end();
        return numRecipient;

    }

    int FAMSEPEventHandler::addMailSection(const double value, const int type, const std::string cistern_code, std::string &content, std::string accumulatedTime,
            std::vector<std::string> &titles, std::vector<std::string> &cisternNames, std::vector<std::string> &organismNames, std::vector<std::string> &distinguishNames) {

        mdb_apl_log_start();
        auto numViolations = checkSensorValue(value, type, cistern_code,  titles, cisternNames, accumulatedTime, organismNames, distinguishNames);

        // std::string times = {accumulatedTime};
        
        // for (int i = 0; i < numViolations; i++) {
        //     // add pH, inflow, outflow, turbidity, salt exceed content
        //     // content += "\n\t" + stype;
        //     content += "\n\t" + titles[i];
        //     content += "\n\t 時間: " +  times;
        //     content += "\n\t 生簀: " + cisternNames[i];
        //     content += "\n\t 生体: " + organismNames[i];
        //     content += "\n\t 識別名: " + distinguishNames[i];
        //     content += "\r\n";

        // }

        mdb_apl_log_end();
        return numViolations;
    }

    int FAMSEPEventHandler::addMailSections(const double value, const int type, const std::string cistern_code,  std::string &content, std::string accumulatedTime,
                        std::vector<std::string> &titles, std::vector<std::string> &cisternNames, std::vector<std::string> &organismNames, std::vector<std::string> &distinguishNames) {

        switch (type) {
            case 1:

                break;
            default:
                break;
        }
        return 0;
    }

    bool FAMSEPEventHandler::changeStatusResolved(const int type, const int status)
    {
        mdb_apl_log_start();
        bool ret = true;
        auto params = std::vector<mdbcom::MDBDatabaseParam> {};
        params.emplace_back(type);
        params.emplace_back(status);
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(UPDATE_STATUS_RESOLVED_ERROR_MESSAGE_DATA, params);
        params.clear();
        auto retExec = selectStm->getResult();
        if (mdbcom::Result::OK == retExec)
        {
            mdb_apl_log_debug("SQL update status resolved error message successfully");
        }
        else
        {
            mdb_apl_log_critical("SQL update status resolved error message failed! ret=%d", retExec);
            ret = false;
        }

        mdb_apl_log_end();
        return ret;
    }

    bool FAMSEPEventHandler::changeStatusViolated(const int type, const int resourceId, const int status)
    {
        mdb_apl_log_start();
        bool ret = true;
        auto params = std::vector<mdbcom::MDBDatabaseParam> {};
        params.emplace_back(type);
        params.emplace_back(resourceId);
        params.emplace_back(status);
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(UPDATE_STATUS_VIOLATED_ERROR_MESSAGE_DATA, params);
        params.clear();
        auto retExec = selectStm->getResult();
        if (mdbcom::Result::OK == retExec)
        {
            mdb_apl_log_debug("SQL update status violated error message successfully");
        }
        else
        {
            mdb_apl_log_critical("SQL update status violated error message failed! ret=%d", retExec);
            ret = false;
        }

        mdb_apl_log_end();
        return ret;
    }

    bool FAMSEPEventHandler::addErrorMessage(const std::string errorMessage, const std::string accumulated_time, const int resource_id, const int resource_type, const int status, const int error_at)
    {
        mdb_apl_log_start();
        bool ret = true;
        auto params = std::vector<mdbcom::MDBDatabaseParam> {errorMessage, accumulated_time, resource_id, resource_type, status, error_at};
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto selectStm = dbConnection.execute(INSERT_ERROR_MESSAGE_DATA, params);
        params.clear();
        auto retExec = selectStm->getResult();
        std::string result;
        if (mdbcom::Result::OK == retExec)
        {
            selectStm->getValue(0, 0, result);
            mdb_apl_log_debug("SQL insert error message successfully : newID = %s", result.c_str());
        }
        else
        {
            mdb_apl_log_critical("SQL insert error message failed! ret=%d", retExec);
            ret = false;
        }

        mdb_apl_log_end();
        return ret;
    }

}

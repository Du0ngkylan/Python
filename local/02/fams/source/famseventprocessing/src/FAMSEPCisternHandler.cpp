/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPCisternHandler.cpp
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
#include "MDBHTTPServerUtility.h"
#include "FAMSIpcCisternSensorData.h"
#include "FAMSEPCisternHandler.h"
// #include "FAMSEPSalinityCalculation.h"


using famscommon::JSON_INTEGER_MISSING_VALUE;
using famscommon::JSON_DOUBLE_MISSING_VALUE;
using famscommon::JSON_STRING_MISSING_VALUE;
using famscommon::JSON_DOUBLE_TIME_MISSING_VALUE;

namespace famseventprocessing {

    /*!
     * @brief Data constructor
     * @param [in] msg Message data to handle.
     * @return None.
     */
    FAMSEPCisternHandler::FAMSEPCisternHandler(
        const famscommon::FAMSIpcHeader *msg) :
        FAMSEPEventHandler { msg }
    {
    }

    /*!
     * @brief Handle current message
     * @param None.
     * @retval Result::OK Success.
     * @retval Result::NG Failure.
     */
    int FAMSEPCisternHandler::handleEvent(void)
    {
        mdb_apl_log_start();
        static int numViolation = 0;
        static int numNonViolation = 0;
        static bool hasSendMailViolation = false;

        double startTime = famscommon::getCurrentTime();
        std::size_t size {0};
        if (famscommon::MessageType::CISTERN_SENSOR_DATA != mShrMessage->ipcType)
        {
            mdb_apl_log_error("eventProcessing read wrong DataType of Obstacle Info");
            mdb_apl_log_end();
            return mdbcom::Result::NG;
        }

        // Received data
        int isAddPHContent = 0;
        int isAddSaltContent = 0;
        int isAddInflowTempContent = 0;
        int isAddOutTempContent = 0;
        int isAddTurbidityContent = 0;

        //getMailconfig
        std::string hostname, sender, subjectEx, bodyEx, username, password, subject, body;
        int port = 25, numViolationCfg = 3;
        // getMailConfig(hostname, sender, subject, body, subjectEx, bodyEx, username, password, port, numViolationCfg);

        // receipients get from observer_information table
        // std::vector<std::string> recipients;
        // int num = getMailRecipents(recipients);

        std::string pHContent = "";
        std::string saltContent = "";
        std::string infowTempContent = "";
        std::string outflowTempContent = "";
        std::string turbidityContent = "";
        std::vector<std::string> titles, cisternNames, organismNames, distinguishNames;

        auto recData = reinterpret_cast<const famscommon::FAMSIpcCisternSensorData *>(mShrMessage); /* pgr0745 */
        const char *accumulated_time = recData->getAccumulatedTime(size);
        const char *cistern_code = recData->getCisternCode(size);
        const double *inflowTemp = recData->getInflowTemp(size);
        const double *outflowTemp = recData->getOutflowTemp(size);
        const int *upperCentralIll = recData->getUpperCentralIll(size);
        const int *lowerCentralIll = recData->getLowerCentralIll(size);
        const double *conductivity = recData->getSalt(size);
        const double *ph = recData->getPH(size);
        double tubidityTmp;
        double salt;

        // Get DB Connection
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
        for(std::vector<double>::size_type i = 0; i != size; i++) {
            /* std::cout << v[i]; ... */
			std::string codeStr = { cistern_code + (i * famscommon::CISTERN_CODE_LEN) };
			//Get error mesages before add sensor values
			auto errorParam = std::vector<mdbcom::MDBDatabaseParam>{};
			int cisternId;
			std::string cisternName;
			errorParam.emplace_back(codeStr);
			auto selectErrorStm = dbConnection.execute(SELECT_CISTERN_ID_QUERY, errorParam);
			errorParam.clear();
			if (mdbcom::Result::OK == selectErrorStm->getResult())
			{
				if (selectErrorStm->getNumRows() > 0)
				{
					selectErrorStm->getValue(0, selectErrorStm->getColumnIndex("id"), cisternId);
					selectErrorStm->getValue(0, selectErrorStm->getColumnIndex("name"), cisternName);
				}
			}
			errorParam.emplace_back(cisternId);
			selectErrorStm = dbConnection.execute(SELECT_ERROR_MESSAGES, errorParam);

            params.emplace_back(accumulated_time + (i *famscommon::ACCUMULATED_TIME_LEN));
            params.emplace_back(inflowTemp[i]);
            params.emplace_back(outflowTemp[i]);
            params.emplace_back(upperCentralIll[i]);
            params.emplace_back(lowerCentralIll[i]);
			
			//calculate salt by conductivity
            FAMSEPConfig &config = FAMSEPEventHandler::getEventProcessingConfig();
            double conductivityCoefficient = std::stod(config.getConductivityCoEfficient());
            mdb_apl_log_debug("getConductivityCoEfficient: %d", conductivityCoefficient);
            double convertToC = conductivity[i]*conductivityCoefficient / 1000;
			salt = Salinity::getPractical((double)convertToC ,(double)(inflowTemp[i]/DIVFACTOR_TMP), (double)PRESSURE);
            params.emplace_back(salt);
            params.emplace_back(ph[i]);
            params.emplace_back(codeStr);

            // check sensor value
            if (isAddPHContent > 0 && isAddSaltContent > 0 && isAddOutTempContent > 0 && isAddInflowTempContent > 0 && isAddTurbidityContent > 0) {
                // do nothing
            }
            else {
//                1: 水温（水槽流入水）(gateway) inflowTemp
//                2: 水温（水槽流出水）(gateway) outFlowTemp
//                3: 濁度 (gateway)           turbidirty
//                4: 塩分 (gateway)           Salt
//                5: pH (gateway)            pH
//                   6: アンモニウムイオン (set trên front end - IF014)
//                7: 亜硝酸イオン
//                8: 硝酸イオン

                std::string times = {*accumulated_time};

                if(!isAddInflowTempContent){
                    isAddInflowTempContent = addMailSection(inflowTemp[i], 0, cistern_code, infowTempContent, times,  titles, cisternNames, organismNames, distinguishNames);
                }

                if(!isAddOutTempContent)
                {
                    isAddOutTempContent = addMailSection(outflowTemp[i], 1, cistern_code, outflowTempContent, times,  titles, cisternNames, organismNames, distinguishNames);
                }

                if(!isAddTurbidityContent)
                {
                    //tubidityTmp = (lowerCentralIll[i])/(upperCentralIll[i]);
                    //isAddTurbidityContent =addMailSection(tubidityTmp, 3, cistern_code, turbidityContent, times,  titles, cisternNames, organismNames, distinguishNames);
                }

                if (!isAddSaltContent)  {
                    isAddSaltContent = addMailSection(salt, 3, cistern_code, saltContent, times,  titles, cisternNames, organismNames, distinguishNames);
                }

                if (!isAddPHContent)  {
                    isAddPHContent = addMailSection(ph[i], 2, cistern_code, pHContent, times,  titles, cisternNames, organismNames, distinguishNames);
                }
            }
			// Select error messages after add sensor values
			auto selectErrorAddStm = dbConnection.execute(SELECT_ERROR_MESSAGES, errorParam);
			if (mdbcom::Result::OK == selectErrorAddStm->getResult())
			{
				mdb_apl_log_mes_info("Select SQL error successfully /n");
				//std::vector<int> resourceIdAdds;
				//std::vector<int> resourceKindAdds;
				std::vector<int> errorAtAdds;
				std::vector<std::string> happenedTimeAdds;
				std::vector<std::string> errorMessageAdds;
				//int resourceId;
				//int resourceKind;
				int errorAt;
				std::string happenedTimeAdd;
				double sensorValue;
				std::string errorMessage;
				for (int i = 0; i < selectErrorAddStm->getNumRows(); i++)
				{
					//selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("resource_id"), resourceId);
					selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("error_at"), errorAt);
					selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("happened_at"), happenedTimeAdd);
					selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("error_message"), errorMessage);
					//resourceIdAdds.emplace_back(resourceId);
					errorAtAdds.emplace_back(errorAt);
					happenedTimeAdds.emplace_back(happenedTimeAdd);
					errorMessageAdds.emplace_back(errorMessage);
				}
				if (mdbcom::Result::OK == selectErrorStm->getResult())
				{
					mdb_apl_log_mes_info("Select SQL error successfully /n");
					//std::vector<int> resourceIds;
					//std::vector<int> resourceKinds;
					std::vector<int> errorAts;
					std::vector<std::string> happenedTimes;
					std::vector<std::string> errorMessages;
					std::vector<double> sensorValues;
					std::string happendAt;
					for (int i = 0; i < selectErrorStm->getNumRows(); i++)
					{
						//selectErrorStm->getValue(i, selectErrorStm->getColumnIndex("resource_id"), resourceId);
						//selectErrorStm->getValue(i, selectErrorStm->getColumnIndex("resource_kind"), resourceKind);
						selectErrorStm->getValue(i, selectErrorStm->getColumnIndex("error_at"), errorAt);
						selectErrorStm->getValue(i, selectErrorStm->getColumnIndex("error_message"), errorMessage);
						selectErrorStm->getValue(i, selectErrorStm->getColumnIndex("happened_at"), happendAt);
						//resourceIds.emplace_back(resourceId);
						//resourceKinds.emplace_back(resourceKind);
						happenedTimes.emplace_back(happendAt);
						errorMessages.emplace_back(errorMessage);
						auto paramSensors = std::vector<mdbcom::MDBDatabaseParam>{};
						paramSensors.emplace_back(cisternId);
						paramSensors.emplace_back(happendAt);
						paramSensors.emplace_back(errorAt);
						auto selectSensorStm = dbConnection.execute(SELECT_SENSOR_VALUE_DATA, paramSensors);
						if (mdbcom::Result::OK == selectSensorStm->getResult())
						{
							if (selectSensorStm->getNumRows() > 0)
							{
								selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("value"), sensorValue);
							}
						}
						sensorValues.emplace_back(sensorValue);
					}
					//mdb_apl_log_end();
					//mdb_apl_log_error_mes_start();
					//check after add value
					for (int m = 0; m < selectErrorAddStm->getNumRows(); m++)
					{
						if (std::find(happenedTimeAdds.begin(), happenedTimeAdds.end(), happenedTimes.at(m)) == happenedTimeAdds.end())
						{
							mdb_apl_log_mes_info("Recovery error_message: %s , %s, %.3f", errorMessages.at(m).c_str(), cisternName.c_str(), sensorValues.at(m));
						}
					}
					//check before add value
					for (int n = 0; n < selectErrorStm->getNumRows(); n++)
					{
						if (std::find(happenedTimes.begin(), happenedTimes.end(), happenedTimeAdds.at(n)) == happenedTimes.end())
						{
							double monitorValue;
							switch (errorAtAdds.at(n))
							{
							case 0:
								monitorValue = inflowTemp[i];
								break;
							case 1:
								monitorValue = outflowTemp[i];
								break;
							case 2:
								monitorValue = ph[i];
								break;
							case 3:
								monitorValue = salt;
								break;
							}
							mdb_apl_log_mes_info("Adding error_message: %s , %s, %.3f", errorMessageAdds.at(n).c_str(), cisternName.c_str(), monitorValue);
						}
					}				
					//mdb_apl_log_error_mes_end();
					//mdb_apl_log_start();
				}
			}

            auto selectStm = dbConnection.execute(INSERT_SENSOR_CISTERN_DATA, params); /*pgr0745*/
            params.clear();
            auto retExec = selectStm->getResult();
            std::string result;

            if (mdbcom::Result::OK == retExec)
            {
                selectStm->getValue(0, 0, result);
                if(selectStm->getNumRows() == 1)
                {
                    long long id;
                    selectStm->getValue(0, selectStm->getColumnIndex("id"), id);
                    mdb_apl_log_debug("SQL insert successfully : newID = %s", result.c_str());
                    mdb_apl_log_debug("New ID already inserted: %d", id);
                }
                else
                {
                    mdb_apl_log_error("SQL insert fail, cistern code Not Found: %s", codeStr.c_str());
                }
            }
            else
            {
                mdb_apl_log_error("Accumulated is duplicated! ret=%d", retExec);
            }
        }
        mdb_apl_log_debug("==1======numViolation=%d=====numViolationCfg==%d=====numNonViolation=%d=======", numViolation, numViolationCfg, numNonViolation);
        // if has any violation and not sendMail yet
        if (isAddPHContent > 0 || isAddSaltContent > 0 || isAddOutTempContent> 0 || isAddInflowTempContent > 0 || isAddTurbidityContent > 0) {
            // sendMail
            if (hasSendMailViolation) {
                if (numViolation < numViolationCfg) {
                    numViolation++;
                }
                else {
                    // 1. continue send mail
                    // sendMail
                    char s[10000];
                    // std::string content = saltContent + pHContent + infowTempContent + outflowTempContent + turbidityContent;
                    // int n = sprintf(s, bodyEx.c_str(), sender.c_str(), content.c_str());
                    // std::string contentEx = {s};
                    // mdb_apl_log_debug("Continue to send mail: %s", contentEx.c_str());
                    // auto ret = famscommon::sendMail(hostname, sender, recipients, subjectEx, username, password, contentEx, port);
                    // reset all config
                    numViolation = 0;
                    numNonViolation = 0;
                    hasSendMailViolation = true;
                }

            } else {
                // sendMail
                // char s[10000];
                // std::string content = saltContent + pHContent + infowTempContent + outflowTempContent + turbidityContent;
                // int n = sprintf(s, bodyEx.c_str(), sender.c_str(), content.c_str());
                // std::string contentEx = {s};
                // mdb_apl_log_debug("Content mail: %s", contentEx.c_str());
                // auto ret = famscommon::sendMail(hostname, sender, recipients, subjectEx, username, password, contentEx, port);
                // reset all config
                numViolation = 0;
                numNonViolation = 0;
                hasSendMailViolation = true;
            }

        } else { // this receive non violation
            numNonViolation++;
            if (numNonViolation == numViolationCfg && hasSendMailViolation) { // send normal mail when non violation after numViolationCfg time
                // sendMail
                char s[10000];
                // int n = sprintf(s, body.c_str(), sender.c_str());
                // std::string content = {s};

                // auto ret = famscommon::sendMail(hostname, sender, recipients, subject, username, password, content, port);
                // reset all config
                numViolation = 0;
                numNonViolation = 0;
                hasSendMailViolation = false;
            }
            else {
//                    Do nothing;
            }
        }

        int result = 0;

        double endTime = famscommon::getCurrentTime();
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
        return result;
    }

}

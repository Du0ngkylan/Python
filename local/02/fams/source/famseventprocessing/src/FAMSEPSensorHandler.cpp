/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSEPSensorHandler.cpp
 * @brief       Handler Obstacle Detection
 * @author      thanglq
 * @date        2019/10/14
 */

#include <string>
#include <vector>
#include <set>

#include "MDBConnectionManager.h"
#include "MDBDatabaseParam.h"
#include "MDBConstants.h"
#include "MDBCommonUtility.h"
#include "MDBHTTPServerUtility.h"
#include "FAMSIpcSensorData.h"
#include "FAMSEPSensorHandler.h"


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
    FAMSEPSensorHandler::FAMSEPSensorHandler(
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
    int FAMSEPSensorHandler::handleEvent(void)
    {
        mdb_apl_log_start();
		static int numViolation = 0;
		static int numNonViolation = 0;
		static bool hasSendMailViolation = false;

        double startTime = famscommon::getCurrentTime();
        std::size_t size {0};
        if (famscommon::MessageType::SENSOR_ADD_DATA != mShrMessage->ipcType)
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
		int isAddAmmoniumContent = 0;
		int isAddNitriteContent = 0;
		int isAddNitrateContent = 0;
		int isAddDissolveContent = 0;

		std::string dissolveContent = "";
		std::string pHContent = "";
		std::string saltContent = "";
		std::string infowTempContent = "";
		std::string outflowTempContent = "";
		std::string ammoniumContent = "";
		std::string nitriteContent = "";
		std::string nitrateContent = "";
		std::vector<std::string> titles, cisternNames, organismNames, distinguishNames;

        // Received data
        auto recData = reinterpret_cast<const famscommon::FAMSIpcSensorData *>(mShrMessage); /* pgr0745 */
        const char *accumulated_time = recData->getAccumulatedTime(size);
        const char *cistern_code = recData->getCisternCode(size);
        const int *type = recData->getType(size);
        const double *value = recData->getValue(size);
        const char *unit = recData->getUnit(size);

        std::string codeStr = {cistern_code};
		int cisternId = 0;
		std::string cisternName;
		// Get DB Connection
		const auto& dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
		auto selectParams = std::vector<mdbcom::MDBDatabaseParam>{}; /*pgr0745*/
		selectParams.emplace_back(codeStr);
		auto selectStm = dbConnection.execute(SELECT_CISTERN_ID_QUERY, selectParams); /*pgr0745*/
		selectParams.clear();
		auto retExec = selectStm->getResult();

		if (mdbcom::Result::OK == retExec)
		{
			if (selectStm->getNumRows() == 1)
			{
				selectStm->getValue(0, selectStm->getColumnIndex("id"), cisternId);
				selectStm->getValue(0, selectStm->getColumnIndex("name"), cisternName);
			}
			mdb_apl_log_debug("SQL select cistern id succesfful ");
			mdb_apl_log_end();
		}
		else
		{
			mdb_apl_log_error("SQL select failed! ret=%d", retExec);
			mdb_apl_log_end();
		}
		FAMSEPConfig& config = FAMSEPEventHandler::getEventProcessingConfig();
		unsigned long numViolationCfg = std::stoul(config.getNumberViolation());
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
		std::string times = { *accumulated_time };
        for(std::vector<double>::size_type i = 0; i != size; i++) {
			//Get error mesages before add sensor values
			auto errorParam = std::vector<mdbcom::MDBDatabaseParam>{};
			mdb_apl_log_mes_info("Select cistern_id = %u", cisternId);
			errorParam.emplace_back(cisternId);
			auto selectErrorStm = dbConnection.execute(SELECT_ERROR_MESSAGES, errorParam);

            std::string portName = "port" + std::to_string(type[i]);
            //get value port config
			std::string sqlStatement = config.getSQLstatement(portName);
			if (sqlStatement.empty())
			{
				continue;
			}
			switch (config.getPortType(portName))
			{

				case  1:
					// check sensor value
					if (!isAddInflowTempContent) 
					{
						isAddInflowTempContent = addMailSection(value[i], 0, cistern_code, infowTempContent, times, titles, cisternNames, organismNames, distinguishNames);
					}
					break;
				case 5:
					if (!isAddSaltContent) 
					{
						isAddSaltContent = addMailSection(value[i], 3, cistern_code, saltContent, times, titles, cisternNames, organismNames, distinguishNames);
					}
					break;
				case 6:
					if (!isAddPHContent) 
					{
						isAddPHContent = addMailSection(value[i], 2, cistern_code, pHContent, times, titles, cisternNames, organismNames, distinguishNames);
					}
					break;
				case 7:
					if (!isAddDissolveContent) 
					{
						isAddDissolveContent = addMailSection(value[i], 7, cistern_code, dissolveContent, times, titles, cisternNames, organismNames, distinguishNames);
					}
					break;
				case 8:
					if (!isAddAmmoniumContent) 
					{
						isAddAmmoniumContent = addMailSection(value[i], 4, cistern_code, ammoniumContent, times, titles, cisternNames, organismNames, distinguishNames);
					}
					break;
				case 9:
					if (!isAddNitriteContent) 
					{
						isAddNitriteContent = addMailSection(value[i], 5, cistern_code, nitriteContent, times, titles, cisternNames, organismNames, distinguishNames);
					}
					break;
				case 10:
					if (!isAddNitrateContent) 
					{
						isAddNitrateContent = addMailSection(value[i], 6, cistern_code, nitrateContent, times, titles, cisternNames, organismNames, distinguishNames);
					}
					break;
			}
			// Select error messages after add sensor values
			auto selectErrorAddStm = dbConnection.execute(SELECT_ERROR_MESSAGES, errorParam);
			if (mdbcom::Result::OK == selectErrorAddStm->getResult() && selectErrorAddStm->getNumRows() > 0)
			{
				mdb_apl_log_mes_info("Select SQL error successfully /n");
				//std::vector<int> resourceIdAdds;
				//std::vector<int> resourceKindAdds;
				//std::vector<int> errorAtAdds;
				std::vector<std::string> happenedTimeAdds;
				std::vector<std::string> errorMessageAdds;
				//int resourceId;
				//int resourceKind;
				int errorAt;
				std::string happendTime;
				double sensorValue;
				std::string errorMessage;
				for (int i = 0; i < selectErrorAddStm->getNumRows(); i++)
				{
					//selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("resource_id"), resourceId);
					//selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("resource_kind"), resourceKind);
					selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("happened_at"), happendTime);
					selectErrorAddStm->getValue(i, selectErrorAddStm->getColumnIndex("error_message"), errorMessage);
					//resourceIdAdds.emplace_back(resourceId);
					//resourceKindAdds.emplace_back(resourceKind);
					happenedTimeAdds.emplace_back(happendTime);
					errorMessageAdds.emplace_back(errorMessage);
				}
				if (mdbcom::Result::OK == selectErrorStm->getResult())
				{
					mdb_apl_log_mes_info("Select SQL error successfully /n");
					//std::vector<int> resourceIds;
					//std::vector<int> resourceKinds;
					//std::vector<int> errorAts;
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
						auto selectSensorStm = dbConnection.execute(SELECT_SENSOR_ADD_VALUE_DATA, paramSensors);
						if (mdbcom::Result::OK == selectSensorStm->getResult())
						{
							mdb_apl_log_mes_info("Select SQL add value successfully /n");
							if (selectSensorStm->getNumRows() > 0)
							{
								switch (errorAt)
								{
								case 0:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("inflow_temp"), sensorValue);
									break;
								case 1:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("outflow_temp"), sensorValue);
									break;
								case 2:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("ph"), sensorValue);
									break;
								case 3:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("satl"), sensorValue);
									break;
								case 4:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("ammonium_ion"), sensorValue);
									break;
								case 5:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("nitrite_ion"), sensorValue);
									break;
								case 6:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("nitrate_ion"), sensorValue);
									break;
								case 7:
									selectSensorStm->getValue(0, selectSensorStm->getColumnIndex("air_flow_rate"), sensorValue);
									break;
								}
							}
						}
						sensorValues.emplace_back(sensorValue);
					}
					//mdb_apl_log_end();
					//mdb_apl_log_error_mes_start();
					//check after add value
					for (int n = 0; n < selectErrorAddStm->getNumRows(); n++)
					{
						//mdb_apl_log_mes_info("Adding error_message");
						if (std::find(happenedTimes.begin(), happenedTimes.end(), happenedTimeAdds.at(n)) == happenedTimes.end())
						{
							mdb_apl_log_mes_info("Adding error_message: %s , %s, %.3f", errorMessageAdds.at(n).c_str(), cisternName.c_str(), value[i]);
						}
					}
					//check before add value
					for (int m = 0; m < selectErrorStm->getNumRows(); m++)
					{
						//mdb_apl_log_mes_info("Recovery error_message");
						if (std::find(happenedTimeAdds.begin(), happenedTimeAdds.end(), happenedTimes.at(m)) == happenedTimeAdds.end())
							mdb_apl_log_mes_info("Recovery error_message: %s , %s, %.3f", errorMessages.at(m).c_str(), cisternName.c_str(), sensorValues.at(m));
					}
					//mdb_apl_log_error_mes_end();
					//mdb_apl_log_start();
				}
			}

			int paramNum = config.getNumberOfParameters(portName);
            params.emplace_back(accumulated_time);
            params.emplace_back(value[i]);
			if (paramNum == 3)
			{
				params.emplace_back(cisternId);
			}
            auto insertStm = dbConnection.execute(sqlStatement, params); /*pgr0745*/
            params.clear();
            retExec = insertStm->getResult();
            if (mdbcom::Result::OK == retExec)
            {
                mdb_apl_log_debug("SQL insert successfully");
                mdb_apl_log_end();
            }
            else
            {
                mdb_apl_log_critical("SQL insert failed! ret=%d", retExec);
                mdb_apl_log_end();
            }
			// if has any violation and not sendMail yet
			if (isAddDissolveContent > 0 || isAddPHContent > 0 ||
				isAddSaltContent > 0 || isAddInflowTempContent > 0 ||
				isAddOutTempContent > 0 || isAddAmmoniumContent > 0 ||
				isAddNitriteContent > 0 || isAddNitrateContent > 0 ||
				isAddSaltContent > 0)
			{
				// sendMail
				if (hasSendMailViolation)
				{
					if (numViolation < numViolationCfg)
					{
						numViolation++;
					}
					else
					{
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

				}
				else
				{
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
			}
			else
			{ // this receive non violation
				numNonViolation++;
				if (numNonViolation == numViolationCfg && hasSendMailViolation)
				{ // send normal mail when non violation after numViolationCfg time
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
				else
				{
					//                    Do nothing;
				}
			}
        }
        int result = 0;
        double endTime = famscommon::getCurrentTime();
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
        mdb_apl_log_end();
        return result;
    }

}

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
		std::size_t size{ 0 };
		if (famscommon::MessageType::CISTERN_SENSOR_DATA != mShrMessage->ipcType)
		{
			mdb_apl_log_error("eventProcessing read wrong DataType of Obstacle Info");
			mdb_apl_log_end();
			return mdbcom::Result::NG;
		}

		// Received data

		auto recData = reinterpret_cast<const famscommon::FAMSIpcSensorData*>(mShrMessage); /* pgr0745 */
		const std::vector<char*> timeStamps = recData->getTimeStamps(size);
		const int* rtuIds = recData->getRtuIds(size);
		const std::vector<int*> rtuChanels = recData->getRtuChannels(size);
		const std::vector<double*> values = recData->getValues(size);
		double paramValues[NUMBER_OF_CHANEL];
		// Get DB Connection
		const auto& dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
		auto params = std::vector<mdbcom::MDBDatabaseParam>{}; /*pgr0745*/
		for (std::size_t i = 0; i != size; i++) {
			/* std::cout << v[i]; ... */
			int rtuId = rtuIds[i];

			//convert time
			char convertTime[30];
			memset(convertTime, '\0', sizeof(convertTime));
			const char* gatewayTime = timeStamps[i];
			strncpy(convertTime, gatewayTime, 23);
			//strncat(convertTime, "+9:00", 6);

			params.emplace_back(convertTime);
			params.emplace_back(rtuId);
			for (std::size_t j = 0; j != rtuChanels[i].size(); j++)
			{
				paramValues[rtuChanels[i]] = values[i]
			}
			for (std::size_t j = 0; j != NUMBER_OF_CHANEL; j++)			
				params.emplace_back(paramValues[j]);
			auto selectStm = dbConnection.execute(INSERT_SENSOR_DATA, params); /*pgr0745*/
			params.clear();
			auto retExec = selectStm->getResult();

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
		}
		int result = 0;

		double endTime = famscommon::getCurrentTime();
		mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));
		mdb_apl_log_end();
		return result;
	}

}

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
#include "MDBHTTPServerUtility.h"
#include "FAMSIpcOutsideData.h"
#include "FAMSEPOutsideHandler.h"

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
    FAMSEPOutsideHandler::FAMSEPOutsideHandler(
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
    int FAMSEPOutsideHandler::handleEvent(void)
    {
        mdb_apl_log_start();
        double startTime = famscommon::getCurrentTime();
        std::size_t size;
        if (famscommon::MessageType::OUTSIDE_DATA != mShrMessage->ipcType)
        {
            mdb_apl_log_error("eventProcessing read wrong DataType of Obstacle Info");
            mdb_apl_log_end();
            return mdbcom::Result::NG;
        }

        // Received data
        auto recData = reinterpret_cast<const famscommon::FAMSIpcOutsideData *>(mShrMessage); /* pgr0745 */
        const double *roomTemp = recData->getRoomTemp(size);
        const double *humidity = recData->getHumidity(size);
        const double *atmoshericPressure = recData->getAtmosphericPressure(size);
        const char *accumulated_time = recData->getAccumulatedTime(size);
        // Get DB Connection
        const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
        auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
        for(std::vector<double>::size_type i = 0; i != size; i++) {
            /* std::cout << v[i]; ... */
            params.emplace_back(accumulated_time + (i *famscommon::ACCUMULATED_TIME_LEN));
            params.emplace_back(roomTemp[i]);
            params.emplace_back(humidity[i]);
            params.emplace_back(atmoshericPressure[i]);
            auto selectStm = dbConnection.execute(INSERT_SENSOR_OUTSIDE_TANK_DATA, params); /*pgr0745*/
            params.clear();
            auto retExec = selectStm->getResult();
            std::string result;
            //paramList.clear();

            if (mdbcom::Result::OK == retExec)
            {
                selectStm->getValue(0, 0, result);
                //            result++;
                mdb_apl_log_debug("SQL insert successfully : newID = %s", result.c_str());
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

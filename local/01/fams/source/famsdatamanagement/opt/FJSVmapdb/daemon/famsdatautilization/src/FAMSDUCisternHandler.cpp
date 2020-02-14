/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUCisternHandler.cpp
 * @brief       Source file for implementing  information.
 * @author      anhnguyen
 * @date        2018/12/10
 */
#include <future>
#include <sstream>
#include "MDBHTTPServerUtility.h"
#include "MDBConnectionManager.h"
#include "FAMSDUCisternHandler.h"
#include "FAMSDURqUserAnalyzer.h"
#include "MDBJsonParser.h"

using famscommon::JSON_STRING_MISSING_VALUE;
using famscommon::JSON_INTEGER_MISSING_VALUE;
using famscommon::JSON_DOUBLE_MISSING_VALUE;
using famscommon::JSON_DOUBLE_TIME_MISSING_VALUE;

namespace famsdatautil {

    /*!
     * @brief The constructor.
     * @param [in] sharedMemory the shared memory from main.
     * @return None.
     */
    FAMSDUCisternHandler::FAMSDUCisternHandler(const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate, const RequestType &requestType)
        : FAMSDUBaseHandler { inputTemplate }, mRequestData { }, mRequestType {requestType}
    {
    }

    /*!
     *  @brief The main method for building difference map.
     *
     *  @param [in] request the HTTP request data
     *  @param [out] response the HTTP response data
     *
     *  @return None.
     */
    void FAMSDUCisternHandler::handleRequest(Poco::Net::HTTPServerRequest &request,
            Poco::Net::HTTPServerResponse &response)
    {
        mdb_apl_log_start();
        auto startTime = famscommon::getCurrentTime(); /* pgr0745 */
        if (mRequestType == RequestType::CISTERNS_GET) {
            handleGetCisternsRequest(request, response);
        }
        auto endTime = famscommon::getCurrentTime(); /* pgr0745 */
        mdb_apl_log_info("Executed time: %.3f seconds", (endTime - startTime));

        mdb_apl_log_end();
    }

    void FAMSDUCisternHandler::handleGetCisternsRequest(Poco::Net::HTTPServerRequest &request,
                           Poco::Net::HTTPServerResponse &response)
    {
        // Declare variable
        auto retCode = OK; /*pgr0745*/
        std::vector<std::string> errKeys;
        auto httpRetCode = Poco::Net::HTTPResponse::HTTP_INTERNAL_SERVER_ERROR;
        std::vector<FAMSDUCisternData> cisternList;

        do
        {
            // Get cistern list
            mdb_apl_log_info("Get cistern information");
            const auto &dbConnection = mdbcom::MDBConnectionManager::getDynamicDBConnection();
            auto selectStm = dbConnection.execute(SELECT_CISTERNS_QUERY); /*pgr0745*/
            auto retExec = selectStm->getResult();
            int result = mdbcom::Result::NG;
            if (mdbcom::Result::OK == retExec)
            {
                std::vector<SensorType> sensorList {
                    SensorType::INFLOW_TEMP,
                    SensorType::OUTFLOW_TEMP,
                    SensorType::TURBIDITY,
                    SensorType::SALT,
                    SensorType::PH,
                    SensorType::AMMONIUM,
                    SensorType::NITRITE,
                    SensorType::NITRATE
                };
                mdb_apl_log_info("Get reference nitrification information");
                std::vector<FAMSDUReferenceData> referenceList;
                FAMSDUReferenceData referenceNitriRecord;
                referenceNitriRecord.setId(1);
                referenceNitriRecord.setName("硝化槽");
                auto selectReference = dbConnection.execute(SELECT_REFERENCE_NITRIFICATION_QUERY); /*pgr0745*/
                auto retExecReference = selectReference->getResult();
                if (mdbcom::Result::OK == retExecReference) {
                    for (int iRef = 0; iRef < selectReference->getNumRows(); iRef++) {
                        std::string refWaterTemp;
                        std::string refDate;
                        std::string utcRefDate;
                        selectReference->getValue(iRef, selectReference->getColumnIndex("water_temp"), refWaterTemp);
                        selectReference->getValue(iRef, selectReference->getColumnIndex("accumulated_time"), refDate);
                        auto ret = famscommon::convertICTToUTC(refDate, utcRefDate);
                        if (ret == mdbcom::Result::OK) {
                            referenceNitriRecord.setValuesOneElement("水温", refWaterTemp, "℃", utcRefDate);
                        } else {
                            referenceNitriRecord.setValuesOneElement("水温", refWaterTemp, "℃", refDate);
                        }
                    } 
                    referenceList.emplace_back(referenceNitriRecord);
                }
                mdb_apl_log_info("Get reference replace tank information");
                FAMSDUReferenceData referenceReWaRecord;
                referenceReWaRecord.setName("換水用タンク");
                referenceReWaRecord.setId(1);
                selectReference = dbConnection.execute(SELECT_REFERENCE_REPLACE_TANK_QUERY); /*pgr0745*/
                retExecReference = selectReference->getResult();
                if (mdbcom::Result::OK == retExecReference) {
                    for (int iRef = 0; iRef < selectReference->getNumRows(); iRef++) {
                        std::string refWaterTemp;
                        std::string refWaterLevel;
                        std::string refDate;
                        std::string utcRefDate;

                        selectReference->getValue(iRef, selectReference->getColumnIndex("water_temp"), refWaterTemp);
                        selectReference->getValue(iRef, selectReference->getColumnIndex("water_level"), refWaterLevel);
                        selectReference->getValue(iRef, selectReference->getColumnIndex("accumulated_time"), refDate);
                        auto ret = famscommon::convertICTToUTC(refDate, utcRefDate);
                        if (ret == mdbcom::Result::OK) {
                            referenceReWaRecord.setValuesOneElement("水温", refWaterTemp, "℃", utcRefDate);
                            referenceReWaRecord.setValuesOneElement("水位", refWaterLevel, "m", utcRefDate);
                        } else {
                            referenceReWaRecord.setValuesOneElement("水温", refWaterTemp, "℃", refDate);
                            referenceReWaRecord.setValuesOneElement("水位", refWaterLevel, "m", refDate);
                        }
                    } 
                    referenceList.emplace_back(referenceReWaRecord);
                }
                mdb_apl_log_info("Get reference outside information");
                FAMSDUReferenceData referenceOutsideRecord;
                referenceOutsideRecord.setName("外");
                referenceOutsideRecord.setId(1);
                selectReference = dbConnection.execute(SELECT_REFERENCE_OUTSIDE_QUERY); /*pgr0745*/
                retExecReference = selectReference->getResult();
                if (mdbcom::Result::OK == retExecReference) {
                    for (int iRef=0; iRef< selectReference->getNumRows();iRef++) {
                        std::string refRoomTemp;
                        std::string refHumidity;
                        std::string refAtPress;
                        std::string refDate;
                        std::string utcRefDate;

                        selectReference->getValue(iRef, selectReference->getColumnIndex("room_temp"), refRoomTemp);
                        selectReference->getValue(iRef, selectReference->getColumnIndex("humidity"), refHumidity);
                        selectReference->getValue(iRef, selectReference->getColumnIndex("atmospheric_pressure"), refAtPress);
                        selectReference->getValue(iRef, selectReference->getColumnIndex("accumulated_time"), refDate);
                        auto ret = famscommon::convertICTToUTC(refDate, utcRefDate);
                        if (ret == mdbcom::Result::OK) {
                            referenceOutsideRecord.setValuesOneElement("室温", refRoomTemp, "℃", utcRefDate);
                            referenceOutsideRecord.setValuesOneElement("湿度", refHumidity, "%", utcRefDate);
                            referenceOutsideRecord.setValuesOneElement("気圧", refAtPress, "hPa", utcRefDate);
                        } else {
                            referenceOutsideRecord.setValuesOneElement("室温", refRoomTemp, "℃", refDate);
                            referenceOutsideRecord.setValuesOneElement("湿度", refHumidity, "%", refDate);
                            referenceOutsideRecord.setValuesOneElement("気圧", refAtPress, "hPa", refDate);
                        }
                    }
                    referenceList.emplace_back(referenceOutsideRecord);
                }
                
                for (int iCistern=0; iCistern< selectStm->getNumRows();iCistern++) {
                    FAMSDUCisternData cisternRecord;
                    long long cisternId;
                    std::string cisternName;
                    std::string cisternCode;
                    selectStm->getValue(iCistern, selectStm->getColumnIndex("id"), cisternId);
                    selectStm->getValue(iCistern, selectStm->getColumnIndex("name"), cisternName);
                    selectStm->getValue(iCistern, selectStm->getColumnIndex("code"), cisternCode);
                    cisternRecord.setId(cisternId);
                    cisternRecord.setName(cisternName);
                    cisternRecord.setCode(cisternCode);
                    mdb_apl_log_info("Get cistern information %ld", cisternId);

                    auto params = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                    params.emplace_back(cisternId);
                    mdb_apl_log_info("Get camera information");
                    auto selectCamera = dbConnection.execute(SELECT_CAMERA_QUERY, params); /*pgr0745*/
                    auto retExecCamera = selectCamera->getResult();
                    if (mdbcom::Result::OK == retExecCamera)
                    {
                        for (int iCamera=0; iCamera< selectCamera->getNumRows();iCamera++) {
                            std::string ipAddress;
                            selectCamera->getValue(iCamera, selectCamera->getColumnIndex("ip_address"), ipAddress);
                            cisternRecord.setCameraOneElement(ipAddress);
                        }
                    }
                    mdb_apl_log_info("Get lot information");
                    auto selectLot = dbConnection.execute(SELECT_LOT_QUERY, params); /*pgr0745*/
                    auto retExecLot = selectLot->getResult();
                    std::vector<FAMSDULotData> lotList;
                    if (mdbcom::Result::OK == retExecLot)
                    {
                        mdb_apl_log_debug("Get lot information with number of row: %d", selectLot->getNumRows());
                        for (int iLot=0; iLot< selectLot->getNumRows();iLot++) {
                            long long lotId;
                            std::string lotName;
                            std::string lotDescription;
                            std::string lotTag;
                            std::string lotUnit;
                            selectLot->getValue(iLot, selectLot->getColumnIndex("id"), lotId);
                            selectLot->getValue(iLot, selectLot->getColumnIndex("name"), lotName);
                            selectLot->getValue(iLot, selectLot->getColumnIndex("description"), lotDescription);
                            selectLot->getValue(iLot, selectLot->getColumnIndex("tag"), lotTag);
                            selectLot->getValue(iLot, selectLot->getColumnIndex("unit"), lotUnit);
                            FAMSDULotData lotRecord;
                            lotRecord.setId(lotId);
                            lotRecord.setName(lotName);
                            lotRecord.setDescription(lotDescription);
                            lotRecord.setTag(lotTag);
                            lotRecord.setUnit(lotUnit);
                            auto lotParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                            lotParams.emplace_back(lotId);
                            lotParams.emplace_back(ERROR_MSG_TYPE_LOT);
                            lotParams.emplace_back(ERROR_MSG_TYPE_MONITOR);
                            auto selectErrMsgs = dbConnection.execute(SELECT_ERROR_MESSAGE_WHERE_TYPE_QUERY, lotParams); /*pgr0745*/
                            auto retExecErrMsgs = selectErrMsgs->getResult();
                            if (mdbcom::Result::OK == retExecErrMsgs)
                            {
                                mdb_apl_log_debug("=====================++++++++++++++++++++++++++");
                                for (int iErr=0; iErr< selectErrMsgs->getNumRows();iErr++) {
                                    static std::string currentErrorMessage;
                                    static std::string previousErrorMessage;
                                    mdb_apl_log_debug("================ previousErrorMessage: %s", previousErrorMessage.c_str());
                                    selectErrMsgs->getValue(iErr, selectErrMsgs->getColumnIndex("error_message"), currentErrorMessage);
                                    mdb_apl_log_debug("================ currentErrorMessage: %s", currentErrorMessage.c_str());
                                    // if (currentErrorMessage.compare(previousErrorMessage) != 0)
                                    // {
                                    //     mdb_apl_log_debug("================ currentErrorMessage not equal previousErrorMessage");
                                    //     lotRecord.setErrorsOneElement(currentErrorMessage);
                                    // }
                                    // else 
                                    // {
                                    //     //do nothing
                                    // }

                                    lotRecord.setErrorsOneElement(currentErrorMessage);

                                    previousErrorMessage = currentErrorMessage;
                                }
                            }
                            auto lotDetailParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                            lotDetailParams.emplace_back(lotId);
                            auto selectDetailLot = dbConnection.execute(SELECT_DETAIL_LOT_QUERY, lotDetailParams); /*pgr0745*/
                            auto retExecDetailLot = selectDetailLot->getResult();
                            if (mdbcom::Result::OK == retExecDetailLot)
                            {
                                for (int iDLot=0; iDLot< selectDetailLot->getNumRows();iDLot++) {
                                    std::string processTime;
                                    std::string number;
                                    std::string size;
                                    std::string shippableTime;
                                    selectDetailLot->getValue(iDLot, selectDetailLot->getColumnIndex("process_time"), processTime);
                                    std::string processTimeRFC3339;
                                    int lotStatus;
                                    famscommon::convertICTToUTC(processTime, processTimeRFC3339);
                                    selectDetailLot->getValue(iDLot, selectDetailLot->getColumnIndex("organism_number"), number);
                                    selectDetailLot->getValue(iDLot, selectDetailLot->getColumnIndex("organism_initial_size"), size);
                                    selectDetailLot->getValue(iDLot, selectDetailLot->getColumnIndex("shippable_time"), shippableTime);
                                    selectDetailLot->getValue(iDLot, selectDetailLot->getColumnIndex("status"), lotStatus);
                                    std::string shippableTimeRFC3339;
                                    famscommon::convertICTToUTC(shippableTime, shippableTimeRFC3339);
                                    lotRecord.setDetailsOneElement("飼育開始日", processTimeRFC3339);
                                    lotRecord.setDetailsOneElement("初期サイズ", size);
                                    lotRecord.setDetailsOneElement("出荷可能日", shippableTimeRFC3339);
                                    lotRecord.setValue(number);
                                    lotRecord.setStatus(lotStatus);
                                }
                            }
                            lotList.emplace_back(lotRecord);
                        }
                    }
                    cisternRecord.setLotData(lotList);
                    mdb_apl_log_info("Get equipment information");
                    auto selectEquipment = dbConnection.execute(SELECT_EQUIPMENT_QUERY, params); /*pgr0745*/
                    auto retExecEquipment = selectEquipment->getResult();
                    std::vector<FAMSDUEquipmentData> equipList;
                    if (mdbcom::Result::OK == retExecEquipment) {
                        for (int iEquipment=0; iEquipment< selectEquipment->getNumRows();iEquipment++) {
                            long long equipId;
                            std::string equipName;
                            std::string equipImagePath;
                            selectEquipment->getValue(iEquipment, selectEquipment->getColumnIndex("id"), equipId);
                            selectEquipment->getValue(iEquipment, selectEquipment->getColumnIndex("name"), equipName);
                            selectEquipment->getValue(iEquipment, selectEquipment->getColumnIndex("image_path"), equipImagePath);
                            FAMSDUEquipmentData equipRecord;
                            equipRecord.setId(equipId);
                            equipRecord.setName(equipName);
                            equipRecord.setImagePath(equipImagePath);
                            mdb_apl_log_debug("Get equipment information Id: %ld Name: %s - ImagePath: %s", equipId, equipName.c_str(), equipImagePath.c_str());
                            auto equipParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                            equipParams.emplace_back(equipId);
                            equipParams.emplace_back(ERROR_MSG_TYPE_EQUIPMENT);
                            auto selectErrMsgs = dbConnection.execute(SELECT_ERROR_MESSAGE_WHERE_TYPE_QUERY, equipParams); /*pgr0745*/
                            auto retExecErrMsgs = selectErrMsgs->getResult();
                            if (mdbcom::Result::OK == retExecErrMsgs)
                            {
                                for (int iErr=0; iErr< selectErrMsgs->getNumRows();iErr++) {
                                    static  std::string currentErrorMessage;
                                    static std::string previousErrorMessage;

                                    selectErrMsgs->getValue(iErr, selectErrMsgs->getColumnIndex("error_message"), currentErrorMessage);
                                    if (currentErrorMessage.compare(previousErrorMessage) != 0)
                                    {
                                        equipRecord.setErrorsOneElement(currentErrorMessage);
                                    }
                                    else 
                                    {
                                        //do nothing
                                    }
                                    previousErrorMessage = currentErrorMessage;
                                    
                                }
                            }
                            auto equipDetailParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                            equipDetailParams.emplace_back(equipId);
                            auto selectDetailEquipment = dbConnection.execute(SELECT_EQUIPMENT_DETAIL_QUERY, equipDetailParams); /*pgr0745*/
                            auto retExecDetailEquipment = selectDetailEquipment->getResult();
                            if (mdbcom::Result::OK == retExecDetailEquipment)
                            {
                                for (int iDEquip=0; iDEquip< selectDetailEquipment->getNumRows();iDEquip++) {
                                    std::string equipName;
                                    std::string equipValue;
                                    selectDetailEquipment->getValue(iDEquip, selectDetailEquipment->getColumnIndex("name"), equipName);
                                    selectDetailEquipment->getValue(iDEquip, selectDetailEquipment->getColumnIndex("value"), equipValue);
                                    equipRecord.setDetailsOneElement(equipName, equipValue);
                                }
                            }
                            equipList.emplace_back(equipRecord);
                        }
                    }
                    cisternRecord.setEquipmentData(equipList);
                    mdb_apl_log_info("Get monitor history information");
                    auto selectMHistory = dbConnection.execute(SELECT_MONITOR_HISTORY_QUERY, params); /*pgr0745*/
                    auto retExecMHistory = selectMHistory->getResult();
                    std::map<int,std::map<std::string, std::string, std::greater<std::string>>> historyList {
                        {
                            SensorType::INFLOW_TEMP,
                            {}
                        },
                        {
                            SensorType::OUTFLOW_TEMP,
                            {}
                        },
                        {
                            SensorType::TURBIDITY,
                            {}
                        },
                        {
                            SensorType::SALT,
                            {}
                        },
                        {
                            SensorType::PH,
                            {}
                        },
                        {
                            SensorType:AMMONIUM,
                            {}
                        },
                        {
                            SensorType::NITRITE,
                            {}
                        },
                        {
                            SensorType::NITRATE,
                            {}
                        },
                    };
                    if (mdbcom::Result::OK == retExecMHistory) {
                        mdb_apl_log_debug("Get history information with number of row: %d", selectMHistory->getNumRows());
                        for (int iHistory=0; iHistory< selectMHistory->getNumRows();iHistory++) {
                            std::string monitorDate, ictMonitorDate;
                            std::string monitorInFlowTemp;
                            std::string monitorOutFlowTemp;
                            std::string monitorTurbidity;
                            std::string monitorSalt;
                            std::string monitorPH;
                            selectMHistory->getValue(iHistory, selectMHistory->getColumnIndex("accumulated_time_rs"), ictMonitorDate);
                            selectMHistory->getValue(iHistory, selectMHistory->getColumnIndex("inflow_temp_rs"), monitorInFlowTemp);
                            selectMHistory->getValue(iHistory, selectMHistory->getColumnIndex("outflow_temp_rs"), monitorOutFlowTemp);
                            selectMHistory->getValue(iHistory, selectMHistory->getColumnIndex("turbidity_rs"), monitorTurbidity);
                            selectMHistory->getValue(iHistory, selectMHistory->getColumnIndex("salt_rs"), monitorSalt);
                            selectMHistory->getValue(iHistory, selectMHistory->getColumnIndex("ph_rs"), monitorPH);

                            mdb_apl_log_debug("Date of history monitoring: %s", monitorDate.c_str());
                            auto ret = famscommon::convertICTToUTC(ictMonitorDate, monitorDate);
                            if (ret == mdbcom::Result::OK) {
                                historyList.find(SensorType::INFLOW_TEMP)->second.insert(std::pair<std::string,std::string>(monitorDate, monitorInFlowTemp));
                                historyList.find(SensorType::OUTFLOW_TEMP)->second.insert(std::pair<std::string,std::string>(monitorDate, monitorOutFlowTemp));
                                historyList.find(SensorType::TURBIDITY)->second.insert(std::pair<std::string,std::string>(monitorDate, monitorTurbidity));
                                historyList.find(SensorType::SALT)->second.insert(std::pair<std::string,std::string>(monitorDate, monitorSalt));
                                historyList.find(SensorType::PH)->second.insert(std::pair<std::string,std::string>(monitorDate, monitorPH));
                            } else {
                                historyList.find(SensorType::INFLOW_TEMP)->second.insert(std::pair<std::string,std::string>(ictMonitorDate, monitorInFlowTemp));
                                historyList.find(SensorType::OUTFLOW_TEMP)->second.insert(std::pair<std::string,std::string>(ictMonitorDate, monitorOutFlowTemp));
                                historyList.find(SensorType::TURBIDITY)->second.insert(std::pair<std::string,std::string>(ictMonitorDate, monitorTurbidity));
                                historyList.find(SensorType::SALT)->second.insert(std::pair<std::string,std::string>(ictMonitorDate, monitorSalt));
                                historyList.find(SensorType::PH)->second.insert(std::pair<std::string,std::string>(ictMonitorDate, monitorPH));
                            }
                        }
                    }

                    //get ammonium_ion, nitrite_ion, nitrate_ion

                    auto paramsSelectIon = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                    paramsSelectIon.emplace_back(cisternId);
                    auto selectIon = dbConnection.execute(SELECT_CISTERN_MONITOR_ION_QUERY, paramsSelectIon); /*pgr0745*/
                    auto retExecIon = selectIon->getResult();
                    if (mdbcom::Result::OK == retExecIon) {
                        mdb_apl_log_info("Get monitor ION with number of row: %d", selectIon->getNumRows());
                        for (int iHistory=0; iHistory< selectIon->getNumRows();iHistory++) {
                            std::string ammoniumDateConverted, ammoniumDate, ictAmmoniumDate;
                            std::string nitriteDateConverted, nitriteDate, ictNitriteDate;
                            std::string nitrateDateConverted, nitrateDate, ictNitrateDate;
                            std::string ammoniumIon;
                            std::string nitriteIon;
                            std::string nitrateIon;
                            selectIon->getValue(iHistory, selectIon->getColumnIndex("ammonium_process_time"), ictAmmoniumDate);
                            selectIon->getValue(iHistory, selectIon->getColumnIndex("nitrite_process_time"), ictNitriteDate);
                            selectIon->getValue(iHistory, selectIon->getColumnIndex("nitrate_process_time"), ictNitrateDate);
                            selectIon->getValue(iHistory, selectIon->getColumnIndex("ammonium_ion_value"), ammoniumIon);
                            selectIon->getValue(iHistory, selectIon->getColumnIndex("nitrite_ion_value"), nitriteIon);
                            selectIon->getValue(iHistory, selectIon->getColumnIndex("nitrate_ion_value"), nitrateIon);

                            //convert time
                            auto convert1 = famscommon::convertProcessTime(ictAmmoniumDate, ammoniumDateConverted);
                            auto convert2 = famscommon::convertProcessTime(ictNitriteDate, nitriteDateConverted);
                            auto convert3 = famscommon::convertProcessTime(ictNitrateDate, nitrateDateConverted);

                            auto ret1 = famscommon::convertICTToUTC(ammoniumDateConverted, ammoniumDate);
                            auto ret2 = famscommon::convertICTToUTC(nitriteDateConverted, nitriteDate);
                            auto ret3 = famscommon::convertICTToUTC(nitrateDateConverted, nitrateDate);
                            if (ret1 == mdbcom::Result::OK && ret2 == mdbcom::Result::OK && ret3 == mdbcom::Result::OK ) {
                                historyList.find(SensorType::AMMONIUM)->second.insert(std::pair<std::string,std::string>(ammoniumDate, ammoniumIon));
                                historyList.find(SensorType::NITRITE)->second.insert(std::pair<std::string,std::string>(nitriteDate, nitriteIon));
                                historyList.find(SensorType::NITRATE)->second.insert(std::pair<std::string,std::string>(nitrateDate, nitrateIon));
                            } else {
                                historyList.find(SensorType::AMMONIUM)->second.insert(std::pair<std::string,std::string>(ictAmmoniumDate, ammoniumIon));
                                historyList.find(SensorType::NITRITE)->second.insert(std::pair<std::string,std::string>(ictNitriteDate, nitriteIon));
                                historyList.find(SensorType::NITRATE)->second.insert(std::pair<std::string,std::string>(ictNitrateDate, nitrateIon));
                            }
                        }
                        mdb_apl_log_debug("SQL select ion succesfful ");
                    }
                    else
                    {
                        mdb_apl_log_error("SQL select failed! ret=%d", retExec);
                    }

                    //get monitor at the last time
                    auto selectMonitor = dbConnection.execute(SELECT_MONITOR_QUERY, params); /*pgr0745*/
                    auto retExecMonitor = selectMonitor->getResult();
                    std::vector<FAMSDUMonitorData> monitorList;
                    if (mdbcom::Result::OK == retExecMonitor) {
                        mdb_apl_log_info("Get monitor history with number of row: %d", selectMonitor->getNumRows());
                        for (int iMonitor=0; iMonitor< selectMonitor->getNumRows();iMonitor++) {
                            long long monitorId;
                            std::string monitorAccumulatedDate, ictAccumulatedDate;
                            std::string monitorDateConverted, monitorDate, ictDate;
                            std::string monitorInFlowTemp;
                            std::string monitorOutFlowTemp;
                            std::string monitorTurbidity;
                            std::string monitorSalt;
                            std::string monitorPH;
                            std::string monitorAmmoniumIon;
                            std::string monitorNitriteIon;
                            std::string monitorNitrateIon;
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("id"), monitorId);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("process_time"), ictDate);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("accumulated_time"), ictAccumulatedDate);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("inflow_temp"), monitorInFlowTemp);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("outflow_temp"), monitorOutFlowTemp);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("turbidity"), monitorTurbidity);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("salt"), monitorSalt);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("ph"), monitorPH);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("ammonium_ion"), monitorAmmoniumIon);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("nitrite_ion"), monitorNitriteIon);
                            selectMonitor->getValue(iMonitor, selectMonitor->getColumnIndex("nitrate_ion"), monitorNitrateIon);
                            auto monitorParams = std::vector<mdbcom::MDBDatabaseParam> {}; /*pgr0745*/
                            monitorParams.emplace_back(cisternId);
                            monitorParams.emplace_back(ictAccumulatedDate);
                            if (ictDate.empty())
                            {
                                monitorParams.emplace_back(ictAccumulatedDate);
                            }
                            else
                            {
                                monitorParams.emplace_back(ictDate);
                            }
                            auto selectErrMsgs = dbConnection.execute(SELECT_ERROR_MESSAGE_MONITOR_WHERE_TYPE_QUERY, monitorParams); /*pgr0745*/
                            auto retExecErrMsgs = selectErrMsgs->getResult();
                            FAMSDUMonitorData monitorRecord;
                            monitorRecord.setId(monitorId);

                            //convert time
                            auto convert1 = famscommon::convertProcessTime(ictDate, monitorDateConverted);

                            auto ret1 = famscommon::convertICTToUTC(monitorDateConverted, monitorDate);
                            auto ret2 = famscommon::convertICTToUTC(ictAccumulatedDate, monitorAccumulatedDate);

                            for (auto const& sensorType: sensorList) {
                                if (sensorType == SensorType::AMMONIUM && monitorAmmoniumIon.empty() 
                                    || sensorType == SensorType::NITRITE && monitorNitriteIon.empty()
                                    || sensorType == SensorType::NITRATE && monitorNitrateIon.empty())
                                {
                                    continue;
                                }
                                else if (ictAccumulatedDate.empty())
                                    {
                                        continue;
                                    }

                                monitorRecord.setName(SensorTypeDetails.find(sensorType)->second.find("name")->second);
                                monitorRecord.setUnit(SensorTypeDetails.find(sensorType)->second.find("unit")->second);
                                monitorRecord.clearErrors();
                                if (mdbcom::Result::OK == retExecErrMsgs)
                                {
                                    for (int iErr=0; iErr< selectErrMsgs->getNumRows();iErr++) {
                                        // std::string errorMessage;
                                        int errorAt;
                                        selectErrMsgs->getValue(iErr, selectErrMsgs->getColumnIndex("error_at"), errorAt);
                                        
                                        //parse type
                                        switch( errorAt ) {
                                        case 0:
                                            errorAt = SensorType::INFLOW_TEMP;
                                            break;
                                        case 1:
                                            errorAt = SensorType::OUTFLOW_TEMP;
                                            break;    
                                        case 2:
                                            errorAt = SensorType::PH;
                                            break;
                                        case 3:
                                            errorAt = SensorType::SALT;
                                            break;
                                        case 4:
                                            errorAt = SensorType::AMMONIUM;
                                            break;
                                        case 5:
                                            errorAt = SensorType::NITRITE;
                                            break;
                                        case 6:
                                            errorAt = SensorType::NITRATE;
                                            break;        
                                        }
                                        if (sensorType == errorAt) {
                                            // selectErrMsgs->getValue(iErr, selectErrMsgs->getColumnIndex("error_message"), errorMessage);
                                            static  std::string currentErrorMessage;
                                            static std::string previousErrorMessage;

                                            mdb_apl_log_debug("================ previousErrorMessage: %s", previousErrorMessage.c_str());
                                            selectErrMsgs->getValue(iErr, selectErrMsgs->getColumnIndex("error_message"), currentErrorMessage);
                                            mdb_apl_log_debug("================ currentErrorMessage: %s", currentErrorMessage.c_str());

                                            monitorRecord.setErrorsOneElement(currentErrorMessage);
                                            previousErrorMessage = currentErrorMessage;
                                        }
                                    }
                                }
                                switch( sensorType ) {
                                case SensorType::INFLOW_TEMP:
                                    monitorRecord.setValue(monitorInFlowTemp);
                                    monitorRecord.setDate(monitorAccumulatedDate);
                                    break;
                                case SensorType::OUTFLOW_TEMP:
                                    monitorRecord.setValue(monitorOutFlowTemp);
                                    monitorRecord.setDate(monitorAccumulatedDate);
                                    break;
                                case SensorType::TURBIDITY:
                                    monitorRecord.setValue(monitorTurbidity);
                                    monitorRecord.setDate(monitorAccumulatedDate);
                                    break;
                                case SensorType::SALT:
                                    monitorRecord.setValue(monitorSalt);
                                    monitorRecord.setDate(monitorAccumulatedDate);
                                    break;
                                case SensorType::PH:
                                    monitorRecord.setValue(monitorPH);
                                    monitorRecord.setDate(monitorAccumulatedDate);
                                    break;
                                case SensorType::AMMONIUM:
                                    monitorRecord.setValue(monitorAmmoniumIon);
                                    monitorRecord.setDate(monitorDate);
                                    break;
                                case SensorType::NITRITE:
                                    monitorRecord.setValue(monitorNitriteIon);
                                    monitorRecord.setDate(monitorDate);
                                    break;
                                case SensorType::NITRATE:
                                    monitorRecord.setValue(monitorNitrateIon);
                                    monitorRecord.setDate(monitorDate);
                                    break;    
                                }
                                //Get History
                                monitorRecord.setHistory(historyList.find(sensorType)->second);
                                monitorList.emplace_back(monitorRecord);
                            }
                        }
                    }
                    cisternRecord.setMonitorData(monitorList);
                    cisternRecord.setReferenceData(referenceList);
                    cisternList.emplace_back(cisternRecord);
                }
            }
            else
            {
                mdb_apl_log_error("SQL select failed! ret=%d", retExec);
            }
            httpRetCode = Poco::Net::HTTPResponse::HTTP_OK;

        }
        while (0);

        //build response JSON string
        auto resData = std::string {};

        resData = buildGetCisternsResponseData(retCode, errKeys, cisternList);
        response.setStatusAndReason(httpRetCode);
        response.setContentType(mdbcom::Http::CONTENT_TYPE_JSON);
        response.setContentLength(resData.length());
        response.setTransferEncoding(Poco::Net::HTTPMessage::IDENTITY_TRANSFER_ENCODING);

        auto &ostr = response.send(); /*pgr0745*/

        ostr << resData;
    }

    /*!
     * @brief Validate JSON format.
     *
     * @param [in] jsonString the JSON string input.
     * @param [out] errKey first key not comply rule
     *
     * @retval OK json format  is OK.
     * @retval JSON_NG json format is NG.
     */
    int FAMSDUCisternHandler::validateJSONFormat(const std::string &jsonString, std::string &errKey)
    {
        mdb_apl_log_start();

        auto ret = OK;
        mdbcom::MDBJsonParser jsonDecoder {};
        mdbcom::MDBContentParser::KeyMapType jsonMap;

        // decode JSON string to object
        auto jsonRet = jsonDecoder.decodeKeyMapData(jsonString, mJsonTemplate, "", jsonMap, errKey); /*pgr0745*/

        if (mdbcom::Json::OK != jsonRet || false == errKey.empty())
        {
            auto errKeyString = errKey.c_str();
            mdb_apl_log_critical("JSON format is abnormal, errorKey = %s", errKeyString);
            mdb_apl_log_end();
            return JSON_NG;
        }

        auto inputAnalyzer = FAMSDURqUserAnalyzer {&jsonDecoder};
        inputAnalyzer.setRequestType(RequestType::LOGIN_POST);

        if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestData, errKey))
        {
            auto errKeyString = errKey.c_str();
            mdb_apl_log_error("Analyze result fail at %s", errKeyString);
            mdb_apl_log_end();
            return JSON_NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Build response data
     *
     * @param [in] retCode the return code
     * @param [in] message message to return when return code = 1 or 999
     * @param [in] position cause error in case return code =1 or 999
     * @param [in] sensorInfoList JSON key-value after building
     * @return the JSON response string
     */
    std::string FAMSDUCisternHandler::buildGetCisternsResponseData(int &retCode,
            const std::vector<std::string> &errorCodes,
            const std::vector<FAMSDUCisternData> &cisternList)
    {
        mdb_apl_log_start();

        auto jsonResponse = std::string {};/*pgr0745*/
        mdbcom::MDBJsonParser jsonEncoder {};
        jsonEncoder.setSerialKey(0);
        auto jsonMap = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/

        if (OK == retCode)
        {
            std::vector<mdbcom::MDBContentParser::KeyMapType> cisternListTag;
            for (auto const &cisternRecord : cisternList) {
                auto cisternRecordTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                jsonEncoder.setKeyMapDataValue(cisternRecordTag, ID, cisternRecord.getId());
                jsonEncoder.setKeyMapDataValue(cisternRecordTag, NAME, cisternRecord.getName());
                std::vector<mdbcom::MDBContentParser::KeyMapType> cameraListTag;
                for (auto const &camera : cisternRecord.getIpAddress()) {
                    auto cameraTag = mdbcom::MDBContentParser::KeyMapType {};
                    jsonEncoder.setKeyMapDataValue(cameraTag, IP_ADDRESS, camera.find("ipAddress")->second);
                    cameraListTag.emplace_back(cameraTag);
                }
                jsonEncoder.setKeyMapDataArrayValue(cisternRecordTag, CAMERA, cameraListTag);
                std::vector<mdbcom::MDBContentParser::KeyMapType> lotListTag;
                for (auto const &lotRecord : cisternRecord.getLotData()) {
                    auto lotRecordTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                    jsonEncoder.setKeyMapDataValue(lotRecordTag, ID, lotRecord.getId());
                    jsonEncoder.setKeyMapDataValue(lotRecordTag, NAME, lotRecord.getName());
                    jsonEncoder.setKeyMapDataValue(lotRecordTag, TAG, lotRecord.getTag());
                    jsonEncoder.setKeyMapDataValue(lotRecordTag, DESCRIPTION, lotRecord.getDescription());
                    jsonEncoder.setKeyMapDataValue(lotRecordTag, VALUE, lotRecord.getValue());
                    jsonEncoder.setKeyMapDataValue(lotRecordTag, UNIT, lotRecord.getUnit());
                    jsonEncoder.setKeyMapDataValue(lotRecordTag, STATUS, lotRecord.getStatus());
                    jsonEncoder.setKeyMapDataArrayValue(lotRecordTag, ERROR_MESSAGES, lotRecord.getErrors());
                    std::vector<mdbcom::MDBContentParser::KeyMapType> detailsTag;
                    for (auto const &detail : lotRecord.getDetails()) {
                        auto detailTag = mdbcom::MDBContentParser::KeyMapType {};
                        jsonEncoder.setKeyMapDataValue(detailTag, NAME, detail.find("name")->second);
                        jsonEncoder.setKeyMapDataValue(detailTag, VALUE, detail.find("value")->second);
                        detailsTag.emplace_back(detailTag);
                    }
                    jsonEncoder.setKeyMapDataArrayValue(lotRecordTag, DETAILS, detailsTag);
                    lotListTag.emplace_back(lotRecordTag);
                }
                jsonEncoder.setKeyMapDataArrayValue(cisternRecordTag, LOTS, lotListTag);
                std::vector<mdbcom::MDBContentParser::KeyMapType> equipListTag;
                for (auto const &equipRecord : cisternRecord.getEquipmentData()) {
                    auto equipRecordTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                    jsonEncoder.setKeyMapDataValue(equipRecordTag, ID, equipRecord.getId());
                    jsonEncoder.setKeyMapDataValue(equipRecordTag, NAME, equipRecord.getName());
                    jsonEncoder.setKeyMapDataValue(equipRecordTag, IMAGE, equipRecord.getImagePath());
                    jsonEncoder.setKeyMapDataArrayValue(equipRecordTag, ERROR_MESSAGES, equipRecord.getErrors());
                    std::vector<mdbcom::MDBContentParser::KeyMapType> detailsTag;
                    for (auto const &detail : equipRecord.getDetails()) {
                        auto detailTag = mdbcom::MDBContentParser::KeyMapType {};
                        jsonEncoder.setKeyMapDataValue(detailTag, NAME, detail.find("name")->second);
                        jsonEncoder.setKeyMapDataValue(detailTag, VALUE, detail.find("value")->second);
                        detailsTag.emplace_back(detailTag);
                    }
                    jsonEncoder.setKeyMapDataArrayValue(equipRecordTag, DETAILS, detailsTag);
                    equipListTag.emplace_back(equipRecordTag);
                }
                jsonEncoder.setKeyMapDataArrayValue(cisternRecordTag, EQUIPMENTS, equipListTag);
                std::vector<mdbcom::MDBContentParser::KeyMapType> monitorListTag;
                for (auto const &monitorRecord : cisternRecord.getMonitorData()) {
                    auto monitorRecordTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                    jsonEncoder.setKeyMapDataValue(monitorRecordTag, ID, monitorRecord.getId());
                    jsonEncoder.setKeyMapDataValue(monitorRecordTag, NAME, monitorRecord.getName());
                    jsonEncoder.setKeyMapDataValue(monitorRecordTag, VALUE, roundedUp2Digits(monitorRecord.getValue()));
                    jsonEncoder.setKeyMapDataValue(monitorRecordTag, UNIT, monitorRecord.getUnit());
                    jsonEncoder.setKeyMapDataValue(monitorRecordTag, DATE, monitorRecord.getDate());
                    jsonEncoder.setKeyMapDataArrayValue(monitorRecordTag, ERROR_MESSAGES, monitorRecord.getErrors());
                    std::vector<mdbcom::MDBContentParser::KeyMapType> historyLstTag;
                    for (auto const &history : monitorRecord.getHistory()) {
                        auto detailTag = mdbcom::MDBContentParser::KeyMapType {};
                        jsonEncoder.setKeyMapDataValue(detailTag, DATE, history.first == "0"? "" : history.first);
                        jsonEncoder.setKeyMapDataValue(detailTag, VALUE, roundedUp2Digits(history.second));
                        historyLstTag.emplace_back(detailTag);
                    }
                    jsonEncoder.setKeyMapDataArrayValue(monitorRecordTag, HISTORY, historyLstTag);
                    monitorListTag.emplace_back(monitorRecordTag);
                }
                jsonEncoder.setKeyMapDataArrayValue(cisternRecordTag, MONITORS, monitorListTag);
                std::vector<mdbcom::MDBContentParser::KeyMapType> refListTag;
                for (auto const &refRecord : cisternRecord.getReferenceData()) {
                    auto refRecordTag = mdbcom::MDBContentParser::KeyMapType {};/*pgr0745*/
                    jsonEncoder.setKeyMapDataValue(refRecordTag, ID, refRecord.getId());
                    jsonEncoder.setKeyMapDataValue(refRecordTag, NAME, refRecord.getName());
                    std::vector<mdbcom::MDBContentParser::KeyMapType> valuesTag;
                    for (auto const &detail : refRecord.getValues()) {
                        auto detailTag = mdbcom::MDBContentParser::KeyMapType {};
                        jsonEncoder.setKeyMapDataValue(detailTag, NAME, detail.find("name")->second);
                        jsonEncoder.setKeyMapDataValue(detailTag, VALUE, detail.find("value")->second);
                        valuesTag.emplace_back(detailTag);
                    }
                    jsonEncoder.setKeyMapDataArrayValue(refRecordTag, VALUES, valuesTag);
                    refListTag.emplace_back(refRecordTag);
                }
                jsonEncoder.setKeyMapDataArrayValue(cisternRecordTag, REFERENCES, refListTag);
                cisternListTag.emplace_back(cisternRecordTag);
            }
            jsonEncoder.setKeyMapDataArrayValue(jsonMap, DATA, cisternListTag);
        } else {
            auto errorsTag = buildErrorMessages(errorCodes);
            if (0 != errorsTag.size())
            {
                jsonEncoder.setKeyMapDataArrayValue(jsonMap, ERRORS, errorsTag);
            }
        }

        auto ret = jsonEncoder.encodeKeyMapData(jsonMap, jsonResponse);/*pgr0745*/

        if (mdbcom::Json::OK != ret)
        {
            mdb_apl_log_critical("JSON encode is abnormal, cannot build response JSON");
        }

        mdb_apl_log_end();
        return jsonResponse;
    }
}


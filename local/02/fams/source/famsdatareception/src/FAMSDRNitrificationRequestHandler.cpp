/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDRNitrificationRequestHandler.cpp
 * @brief       Handler Sensor Request
 * @author      anhnguyen
 * @date        2018/12/11
 */

#include <future>
#include <sstream>
#include "MDBHTTPServerUtility.h"
#include "MDBConnectionManager.h"
#include "FAMSDRConfig.h"
#include "FAMSDRNitrificationRequestHandler.h"
#include "FAMSIpcNitrificationData.h"
#include "FAMSDRRqNitrificationAnalyzer.h"


using famscommon::JSON_INTEGER_MISSING_VALUE;
using famscommon::JSON_DOUBLE_MISSING_VALUE;
using famscommon::JSON_STRING_MISSING_VALUE;

namespace famsdatareception {

    /*!
     * @brief The Constructor.
     * @param [in] inputTemplate Template for request input format
     * @return None.
     */
    FAMSDRNitrificationRequestHandler::FAMSDRNitrificationRequestHandler(mdbcom::MDBIPCShm *sharedMemory,
            const std::vector<mdbcom::MDBContentDecodingFormat> &inputTemplate) : FAMSDRBaseHandler {sharedMemory, inputTemplate},
                    mRequestData {}
    {

    }
    /*!
     * @brief Validate JSON format.
     *
     * @param [in] jsonstring the JSON std::string input.
     * @param [out] jsonMap the map of JSON data
     *
     * @retval REST_RET_NORMAL json format  is OK.
     * @retval REST_RET_INCORRECT_FORMAT json format is invalid
     */
    int FAMSDRNitrificationRequestHandler::validateJSONFormat(
        const std::string &jsonstring,
        mdbcom::MDBContentParser::KeyMapType &jsonMap,
        std::vector<std::string> &errKeys)
    {
        mdb_apl_log_start();

        mdbcom::MDBJsonParser jsonDecoder {};
        auto errorKey = std::string {};
        // decode JSON std::string to object
        std::string rootKey {""};
        auto jsonRet = jsonDecoder.decodeKeyMapData(jsonstring, mJsonTemplate, rootKey, jsonMap, errorKey);
        if (mdbcom::Json::OK != jsonRet || 0 != errorKey.size())
        {
            if (0 != errorKey.size())
            {
                errKeys.push_back(errorKey);
            }
            mdb_apl_log_end();
            return REST_RET_INCORRECT_FORMAT;
        }
        auto inputAnalyzer = FAMSDRRqNitrificationAnalyzer {&jsonDecoder};
        auto it = jsonMap.begin();
        int index = 20;
        for(int i = 0; i < index && it != jsonMap.end(); ++i)
            ++it;
        mdb_apl_log_info("START");
        if (mJsonTemplate.empty())
        {
            mdb_apl_log_info("OK");
        } else
        {
            mdb_apl_log_info("Not OK");
        }

        if(it != jsonMap.end())
            mdb_apl_log_info("it->first=%s", it->first.c_str());
        else
            mdb_apl_log_info("END=%s",jsonstring.c_str());
        if (mdbcom::Result::OK != inputAnalyzer.analyze(jsonMap, mRequestData, errorKey))
        {
            if (0 != errorKey.size())
            {
                errKeys.push_back(errorKey);
            }

            mdb_apl_log_end();
            return REST_RET_INCORRECT_FORMAT;
        }

        mdb_apl_log_end();
        return REST_RET_NORMAL;
    }

    /*!
     * @brief Get Sensor buffer and sise
     * @param [out] dataLength Length of data
     * @retval pointer to ipc buffer prepared to be sent
     */
    std::unique_ptr<famscommon::FAMSIpcHeader>
    FAMSDRNitrificationRequestHandler::getIpcInfoData(const famscommon::MessageType eventType,
            std::size_t &dataLength)
    {
        mdb_apl_log_start();
        auto extendedSize = 0ull;
        extendedSize += mRequestData.getWaterTemp().size() * sizeof(double);
        extendedSize += mRequestData.getAccumulatedTime().size() * famscommon::ACCUMULATED_TIME_LEN;
        auto ret = std::unique_ptr<famscommon::FAMSIpcHeader> {};
        try
        {
            auto requestInformation = new (extendedSize) famscommon::FAMSIpcNitrificationData {eventType, mRequestData};
            ret = std::unique_ptr<famscommon::FAMSIpcHeader>
            {
            	reinterpret_cast<famscommon::FAMSIpcHeader *>(requestInformation)
            };
            dataLength = sizeof(famscommon::FAMSIpcNitrificationData) + extendedSize;
            mdb_apl_log_info("dataLength=%d", dataLength);
        }
        catch (const std::bad_alloc &ex)
        {
            auto exDetail = ex.what();
            mdb_apl_log_critical("Can't allocate memory for obstalce information buffer [%s]",
                                 exDetail);
        }
        catch (const mdbcom::MDBException &ex)
        {
            auto exDetail = ex.what();
            mdb_apl_log_critical("Can't allocate memory for obstalce information buffer [%s]",
                                 exDetail);
        }
        catch (...)
        {
            mdb_apl_log_critical("Can't allocate memory for obstalce information buffer [unknown exception]");
        }

        mdb_apl_log_end();
        return ret;
    }
    /*!
     * @brief Register an event processing
     *
     * @retval Result::OK Success.
     * @retval Result::NG Failure.
     */
    int FAMSDRNitrificationRequestHandler::registerEvent(void)
    {
        mdb_apl_log_start();

        sendIpcInfoData(famscommon::MessageType::NITRIFICATION_DATA);

        mdb_apl_log_end();
        return mdbcom::Result::OK;
    }
}


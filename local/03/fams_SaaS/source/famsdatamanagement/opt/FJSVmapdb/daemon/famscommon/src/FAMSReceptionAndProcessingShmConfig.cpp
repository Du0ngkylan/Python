/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSReceptionAndProcessingShmConfig.cpp
 * @brief       Source file of class FAMSReceptionAndProcessingShmConfig.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#include <Poco/Dynamic/Var.h>
#include "FAMSReceptionAndProcessingShmConfig.h"
#include "MDBException.h"
#include "MDBLogger.h"

namespace famscommon {

    const std::string FAMSReceptionAndProcessingShmConfig::RECEPTION_AND_PROCESSING_SHM_CONF
    { "/opt/FJSVmapdb/etc/reception_processing_shm.conf" };
    const std::string FAMSReceptionAndProcessingShmConfig::RECEPTION_AND_PROCESSING_SHM_SIZE_STR
    { "receptionAndProcessingMsgSize" };

    /*!
     * @brief Constructor with config path.
     * It will throw exception when config file or config format is abnormal.
     *
     * @param None.
     * @return None.
     */
    FAMSReceptionAndProcessingShmConfig::FAMSReceptionAndProcessingShmConfig(void) :
        mConfigFile { nullptr }, mReceptionAndProcessingShmSize { 0 }
    {
        mdb_apl_log_start();

        auto isValidShmSize = false; /* pgr0745 */

        try
        {
            // Use config module
            mConfigFile = mdbcom::MDBMemoryUtility::make_unique<mdbcom::MDBConfigFile> (RECEPTION_AND_PROCESSING_SHM_CONF); /* pgr0745 */
        }
        catch (const std::bad_alloc &)
        {
            mdb_apl_log_critical("Bad alloc");
        }
        catch (const mdbcom::MDBException &ex)
        {
            auto exDetail = ex.what(); /* pgr0745 */
            mdb_apl_log_critical(exDetail);
        }
        catch (...)
        {
            mdb_apl_log_critical("Unknown exception");
        }

        if (nullptr != mConfigFile)
        {
            auto cfgVal = std::string {}; /* pgr0745 */
            // get ServerPort key
            auto ret = mConfigFile->getValue(RECEPTION_AND_PROCESSING_SHM_SIZE_STR, cfgVal); /* pgr0745 */

            auto tmpCfgVal = cfgVal.c_str(); /* pgr0745 */
            auto invalidChar = static_cast<char *>(nullptr); /* pgr0745 */

            if (mdbcom::Result::OK == ret)
            {
                mReceptionAndProcessingShmSize = std::strtoull(tmpCfgVal, &invalidChar, 10);

                if ((*invalidChar != '\0') || (0 == mReceptionAndProcessingShmSize))
                {
                    mdb_apl_log_critical("receptionAndProcessingMsgSize key is invalid: %s", tmpCfgVal);
                }
                else
                {
                    mdb_apl_log_info("receptionAndProcessingMsgSize = %llu", mReceptionAndProcessingShmSize);
                    isValidShmSize = true;
                }
            }
            else
            {
                mdb_apl_log_critical("receptionAndProcessingMsgSize key is invalid: %s", tmpCfgVal);
            }
        }

        if (false == isValidShmSize)
        {
            mdb_apl_log_end();
            throw mdbcom::MDBException("receptionAndProcessingMsgSize is invalid");
        }

        mdb_apl_log_end();
    }
}

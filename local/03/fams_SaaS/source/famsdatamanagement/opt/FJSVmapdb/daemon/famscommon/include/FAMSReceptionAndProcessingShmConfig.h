/* Copyright© 2018 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSReceptionAndProcessingShmConfig.h
 * @brief       Header file of class FAMSReceptionAndProcessingShmConfig.
 * @author      anhnguyen
 * @date        2018/12/12
 */

#ifndef FAMS_RECEPTION_AND_PROCESSING_SHARE_MEMORY_CONFIG_H
#define FAMS_RECEPTION_AND_PROCESSING_SHARE_MEMORY_CONFIG_H

#include "MDBConfigFile.h"
#include "MDBMemoryUtility.h"
namespace famscommon {
    /*!
     * @class FAMSReceptionAndProcessingShmConfig
     * @brief  Base handler class for history db message
     */

    class FAMSReceptionAndProcessingShmConfig {
    public:
        /*!
         * @brief Constructor with config path.
         * It will throw exception when config file or config format is abnormal.
         *
         * @param None.
         * @return None.
         */
        explicit FAMSReceptionAndProcessingShmConfig(void);

        /*!
         * @brief Destructor, using default
         * @param None.
         * @return None.
         */
        ~FAMSReceptionAndProcessingShmConfig(void) = default;

        /*!
         * @brief Copy constructor is not supported.
         * @param None.
         * @return None.
         */
        FAMSReceptionAndProcessingShmConfig(const FAMSReceptionAndProcessingShmConfig &other) :
            mConfigFile { nullptr }, mReceptionAndProcessingShmSize { 0 } {
            if (nullptr != other.mConfigFile) {
                mConfigFile = mdbcom::MDBMemoryUtility::make_unique<mdbcom::MDBConfigFile>(
                                  *other.mConfigFile);
            }

            mReceptionAndProcessingShmSize = other.mReceptionAndProcessingShmSize;
        }

        /*!
         * @brief Copy assignment is not supported.
         * @param None.
         * @return None.
         */
        FAMSReceptionAndProcessingShmConfig &operator=(const FAMSReceptionAndProcessingShmConfig &other) {
            if (this != &other) {
                if (nullptr != other.mConfigFile) {
                    mConfigFile = mdbcom::MDBMemoryUtility::make_unique<mdbcom::MDBConfigFile>(
                                      *other.mConfigFile);
                }

                mReceptionAndProcessingShmSize = other.mReceptionAndProcessingShmSize;
            }

            return *this;
        }

        /*!
         * @brief Get Shared memory size
         * @param None.
         * @return Shared memory size
         */
        inline unsigned long long getReceptionAndProcessingShmSize(void) const {
            return mReceptionAndProcessingShmSize;
        }

        /*!
         * @brief Set Delivery and Vehicle process Shared memory size
         * @param [in] receptionAndProcessingShmSize Data reception and event processing process Shared memory size
         * @return None.
         */
        inline void setReceptionAndProcessingShmSize(unsigned long long receptionAndProcessingShmSize) {
            mReceptionAndProcessingShmSize = receptionAndProcessingShmSize;
        }

    private:
        //! Data shared memory configuration file path
        static const std::string RECEPTION_AND_PROCESSING_SHM_CONF;
        //! Data  shared memroy queue size key
        static const std::string RECEPTION_AND_PROCESSING_SHM_SIZE_STR;
        //! Config file pointer
        std::unique_ptr<mdbcom::MDBConfigFile> mConfigFile;
        //! Shared memory size
        unsigned long long mReceptionAndProcessingShmSize;
    };
}
#endif /* FAMS_RECEPTION_AND_PROCESSING_SHARE_MEMORY_CONFIG_H */

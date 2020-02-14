/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBLogConfiguration.h
 * @brief Support change log level online
 * @author cuondadp:
 * @date 2016/11/03
 */

#ifndef LIBMDBUTIL_MDBLOGCONFIGURATION_H_
#define LIBMDBUTIL_MDBLOGCONFIGURATION_H_

#include <vector>
#include <string>
#include "MDBDebugLevelEntry.h"

namespace mdbcom {

    /*! @class MDBCommandResult
     *  @brief Result for command tools
     */
    class MDBCommandResult  { /* pgr1232 */
    public:
        static constexpr int ERROR_EXEC = Result::NG;
        static constexpr int ERROR_NOTFOUND = Result::NG - 1;
        static constexpr int ERROR_LEVEL = Result::NG - 2;
        static constexpr int ERROR_DOUBLE_EXEC = Result::NG - 3;
    };

    /*! @class MDBLogConfiguration
     *  @brief Command line to support live log level change
     */
    class MDBLogConfiguration {
    public:
        /*!
         * @brief Load configuration file
         *
         * Load debug configuration file to shared memory
         *
         * @param [in] filepath Path to configuration file
         * @return result
         * @retval Result::OK success
         * @retval Result::NG failure
        */
        static int loadConfigFile(const std::string &filepath);

        /*!
         * @brief Set log level
         *
         * Set debug log level for function
         *
         * @param [in] function Function name (module/thread name)
         * @param [in/out] value Debug log level, incase failure error code is set.
         * @return result
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static int setLogLevel(const std::string &function, int &value);

        /*!
         * @brief Get log level
         *
         * Get debug log level by function
         *
         * @param [in] function Function name (module/thread name)
         * @param [out] level Debug log level
         * @param [out] errorCode Error code
         * @return result
         * @retval Result::OK success
         * @retval Result::NG failure
         */
        static int showLogLevel(const std::string &function, std::vector<MDBDebugLevelEntry *> &level, int &errorCode);

        /*!
         * @brief Move Constructor
         *
         * Not support
         *
         * @return None.
        */
        MDBLogConfiguration(MDBLogConfiguration &&) = delete; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Move assignment
         *
         * Not support
         *
         * @return None.
        */
        MDBLogConfiguration &operator =(MDBLogConfiguration &&) = delete; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Copy Constructor
         *
         * Not support
         *
         * @return None.
        */
        MDBLogConfiguration(const MDBLogConfiguration &) = delete; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Copy assignment
         *
         * Not support
         *
         * @return None.
        */
        MDBLogConfiguration &operator =(const MDBLogConfiguration &) = delete; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Default Constructor
         *
         * Not support
         *
         * @return None.
        */
        MDBLogConfiguration(void) = delete;

        /*!
         * @brief Default Destructor
         *
         * Not support
         *
         * @return None.
        */
        ~MDBLogConfiguration(void) = delete;
    };
}

#endif /* MDBLOGCONFIGURATION_H */

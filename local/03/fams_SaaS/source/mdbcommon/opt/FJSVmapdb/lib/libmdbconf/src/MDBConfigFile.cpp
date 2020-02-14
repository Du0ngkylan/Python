/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConfigFile.cpp
 * @brief       The implement of MDBConfigFile class.
 * @author      trungnv: Update C++ improvement.
 * @date        2016/11/24
 */

#include <fstream>
#include <iostream>

#include <Poco/String.h>

#include "MDBConfigFile.h"
#include "MDBLogger.h"
#include "MDBException.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {

    class MDBConfigFile::Impl {
    public:

        /*!
         * @brief Constructor with configure file path.
         *
         * Parse and store configure key/value to mValues
         * In case can't open configure file:
         * - Print out syslog
         * - Throw message
         *
         * @param [in] cfgPath Path of configure file.
         * @return None.
         */
        explicit Impl(const std::string &cfgPath);

        /*!
         * @brief Get value string from input key.
         *
         * Get value string from input key.
         *
         * @param [in] cfgKey Configure key.
         * @param [out] cfgVal Configure value.
         * @retval Result::EMPTY_KEY Input key is empty.
         * @retval Result::UNKNOWN_EXCEPTION Unknown exception occurs.
         * @retval Result::OK Get value success.
         */
        int getValue(const std::string &cfgKey, std::string &cfgVal);

        /*!
         * @brief Get map value.
         *
         * Get value string from input key.
         *
         * @param None.
         * @return Internal map reference value.
         */
        inline const std::map<std::string, std::string> &getValues() const {
            return mValues;
        }

    private:
        //! Constant for character '#'
        static const char CHAR_HASH;

        //! Constant for character '='
        static const char CHAR_EQUAL;

        //! Error code for configure file invalid
        static constexpr int RESULT_FILE_NORMAL = -22; /* pgr0746 */

        //! Error code for configure file invalid
        static constexpr int RESULT_FILE_INVALID = -23; /* pgr0746 */

        //! Error code for configure file parsing fail
        static constexpr int RESULT_FILE_PARSING_FAIL = -24; /* pgr0746 */

        //! Map stores keys/values of configure file.
        std::map<std::string, std::string> mValues;

        //! Store path of configure file.
        std::string cfgPath;

        /*!
         * @brief Read configure file.
         *
         * This method is used to parse configure file
         * and store configured info into internal map.
         *
         * @param None.
         * @retval RESULT_FILE_NORMAL if reading configure success
         * @retval RESULT_FILE_PARSING_FAIL if parsing configure failure(Open fail, delete command fail, unknow exception)
         * @retval RESULT_FILE_INVALID if configure file is invalid(duplicated key, not contain '=', '=' at first line)
         */
        int readConfig(void);
    };

    // Constant
    const char MDBConfigFile::Impl::CHAR_HASH
    {
        '#'
    };
    const char MDBConfigFile::Impl::CHAR_EQUAL { '=' };

    //---------------------------- Impl-----------------------------

    MDBConfigFile::Impl::Impl(const std::string &cfgPath) :
        mValues { }, cfgPath { Poco::trim(cfgPath) }
    {

        mdb_apl_log_start();
        auto result = int { readConfig() }; /* pgr0745 */
        auto pCfgPath = cfgPath.c_str(); /* pgr0745 */

        switch (result)
        {
            case RESULT_FILE_PARSING_FAIL:   /* pgr2031 */
                {
                    mdb_sys_log_out("F000101", RESULT_FILE_PARSING_FAIL, pCfgPath);
                    mdb_apl_log_critical("Can't parse input configured file(path = %s)", pCfgPath);
                    mdb_apl_log_end();
                    throw MDBException { "Open configure file fail" };
                }

            case RESULT_FILE_INVALID:   /* pgr2031 */
                {
                    mdb_sys_log_out("F000101", RESULT_FILE_INVALID, pCfgPath);
                    mdb_apl_log_critical("Configured file is invalid(path = %s)", pCfgPath);
                    mdb_apl_log_end();
                    throw MDBException { "Configured file is invalid" };
                }

            case RESULT_FILE_NORMAL: /* pgr2031 */
                break;

            default:
                break;
        }

        mdb_apl_log_end();

    }

    int MDBConfigFile::Impl::getValue(const std::string &cfgKey, std::string &cfgVal)
    {
        mdb_apl_log_start();
        auto ret = Result::OK; /* pgr0745 */
        cfgVal.clear();
        auto pCfgPath = cfgPath.c_str(); /* pgr0745 */
        auto pCfgKey = cfgKey.c_str(); /* pgr0745 */
        mdb_apl_log_debug("in cfgPath = [%s], cfgKey = [%s]", pCfgPath, pCfgKey);

        if (true == cfgKey.empty())
        {
            // Check empty configure key
            mdb_sys_log_out("F000101", Result::EMPTY_KEY, pCfgPath);
            mdb_apl_log_critical("Configured key is empty(path = %s)", pCfgPath);

            ret = Result::EMPTY_KEY;
        }
        else
        {
            try
            {
                cfgVal = mValues.at(cfgKey);
            }
            catch (std::out_of_range &)
            {
                // Result not found
                // key not exist, normal case
                mdb_apl_log_debug("Configure key not exist(key = %s, path = %s)", pCfgKey, pCfgPath);

            }
            catch (...)
            {
                ret = Result::UNKNOWN_EXCEPTION;
                mdb_sys_log_out("F000101", Result::UNKNOWN_EXCEPTION, pCfgPath);
                mdb_apl_log_critical("Catch unknown exception(key = %s, path = %s)", pCfgKey,
                                     pCfgPath);

            }
        }

        auto tmpVal = cfgVal.c_str(); /* pgr0745 */
        mdb_apl_log_debug("out ret = [%d], cfgVal = [%s]", ret, tmpVal);
        mdb_apl_log_end();
        return ret;
    }

    int MDBConfigFile::Impl::readConfig(void)
    {
        mdb_apl_log_start();
        auto ret = RESULT_FILE_NORMAL; /* pgr0745 */
        // Open file
        auto pCfgPath = cfgPath.c_str(); /* pgr0745 */
        std::fstream cfgFile(pCfgPath, std::ios::in);

        if (false == cfgFile.is_open())
        {
            ret = RESULT_FILE_PARSING_FAIL;
            mdb_apl_log_critical("Can't open input configured file(path = %s)", pCfgPath);

            mdb_apl_log_end();
            return ret;
        }

        auto line = std::string { }; /* pgr0745 */
        while (getline(cfgFile, line))
        {
            // Skip empty line, comment line
            if ((std::string::npos == line.find_first_not_of(" \n\t"))
                    || (CHAR_HASH == line[line.find_first_not_of(" \t")]))
            {
                continue;
            }

            // Start with '='
            if (CHAR_EQUAL == line[line.find_first_not_of(" \t")])
            {
                ret = RESULT_FILE_INVALID;
                mdb_apl_log_critical("Invalid configure, start with '='(path = %s)", pCfgPath);

                break;
            }

            // Remove comment end of line
            auto pos = line.find(CHAR_HASH); /* pgr0745 */

            if (std::string::npos != pos)
            {
                try
                {
                    static_cast<void>(line.erase(pos, -1));
                }
                catch (std::out_of_range &)
                {
                    // Can't delete comment in same line of configure, configure key is missed
                    ret = RESULT_FILE_PARSING_FAIL;
                    mdb_apl_log_critical(
                        "Can't erase comment, can't parse configure file(path = %s)", pCfgPath);

                    break;
                }
                catch (...)
                {
                    ret = RESULT_FILE_PARSING_FAIL;
                    mdb_apl_log_critical("Catch unknown exception.");

                    break;
                }
            }

            pos = line.find(CHAR_EQUAL);

            if (std::string::npos == pos)
            {
                // Skip setting invalid with '='
                ret = RESULT_FILE_INVALID;
                mdb_apl_log_critical("Invalid configure, missing '='(path = %s)", pCfgPath);

                break;
            }

            // Split key and value
            auto cfgKey = std::string { line.substr(0, pos) }; /* pgr0745 */
            auto cfgVal = std::string { line.substr(pos + 1) }; /* pgr0745 */

            // Check duplicated
            auto configurationKey = Poco::trimInPlace(cfgKey); /* pgr0745 */
            auto configurationValue = Poco::trimInPlace(cfgVal); /* pgr0745 */
            auto tmpKey = configurationKey.c_str(); /* pgr0745 */
            auto tmpVal = configurationValue.c_str(); /* pgr0745 */
            mdb_apl_log_trace("tmpKey = %s, tmpVal = %s", tmpKey, tmpVal);

            if (false == mValues.emplace(configurationKey, configurationValue).second)
            {
                ret = RESULT_FILE_INVALID;
                auto pCfgKey = cfgKey.c_str(); /* pgr0745 */
                mdb_apl_log_critical("Configure key is duplicated(key = %s,path = %s)", pCfgKey,
                                     pCfgPath);

                break;
            }
        }

        mdb_apl_log_end();
        return ret;
    }

    //---------------------------- MDBConfigFile-----------------------------

    /*!
     * @brief Copy constructor.
     *
     * @param [in] cfgObj MDBConfigFile object.
     * @return None.
     */
    MDBConfigFile::MDBConfigFile(const MDBConfigFile &other) = delete; /*pgr2222*//*pgr0571*/

    /*!
     * @brief Operator=.
     *
     * @param [in] cfgObj ConfigFile object.
     * @return New MDBConfigFile object
     */
    MDBConfigFile &MDBConfigFile::operator=(const MDBConfigFile &other) = delete; /*pgr0055*//*pgr2223*//*pgr0309*/

    /*!
     * @brief Destructor of MDBConfigFile.
     *
     * @param None.
     * @return None.
     */
    MDBConfigFile::~MDBConfigFile(void) = default;

    /*!
     * @brief Constructor with configure file path.
     *
     * Parse and store configure key/value to mValues
     * In case can't open configure file:
     * - Print out syslog
     * - Throw message
     *
     * @param [in] cfgPath Path of configure file.
     * @return None.
     */
    MDBConfigFile::MDBConfigFile(const std::string &path) :
        pImpl { MDBMemoryUtility::make_unique<Impl>(path) }
    {
    }

    /*!
     * @brief Get value string from input key.
     *
     * Get value string from input key.
     *
     * @param [in] cfgKey Configure key.
     * @param [out] cfgVal Configure value.
     * @retval Result::EMPTY_KEY Input key is empty.
     * @retval Result::UNKNOWN_EXCEPTION Unknown exception occurs.
     * @retval Result::OK Get value success.
     */
    int MDBConfigFile::getValue(const std::string &cfgKey, std::string &cfgVal)   /*pgr2227*/
    {
        return pImpl->getValue(cfgKey, cfgVal);
    }

    /*!
     * @brief Get map value.
     *
     * Get value string from input key.
     *
     * @param None.
     * @return Internal map reference value.
     */
    const std::map<std::string, std::string> &MDBConfigFile::getMapValue(void) const
    {
        return pImpl->getValues();
    }
}

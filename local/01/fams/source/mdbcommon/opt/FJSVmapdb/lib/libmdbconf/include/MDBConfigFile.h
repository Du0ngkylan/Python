/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBConfigFile.h
 * @brief       The header of MDBConfigFile class.
 * @author      trungnv: Update common change.
 * @date        2016/10/24
 */

#ifndef LIBMDBCONF_MDBCONFIGFILE_H_
#define LIBMDBCONF_MDBCONFIGFILE_H_
#include <map>
#include <memory>

namespace mdbcom {

    /*! @class MDBConfigFile.
     *  @brief The class support parsing configured file.
     */
    class MDBConfigFile {
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
        explicit MDBConfigFile(const std::string &cfgPath);

        /*!
         * @brief Copy constructor.
         *
         * @param [in] cfgObj MDBConfigFile object.
         * @return None.
         */
        MDBConfigFile(const MDBConfigFile &other); /*pgr2222*//*pgr0571*/

        /*!
         * @brief Operator=.
         *
         * @param [in] cfgObj ConfigFile object.
         * @return New MDBConfigFile object
         */
        MDBConfigFile &operator=(const MDBConfigFile &other); /*pgr0055*//*pgr2223*//*pgr0309*/

        /*!
         * @brief Destructor of MDBConfigFile.
         *
         * @param None.
         * @return None.
         */
        ~MDBConfigFile(void);

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
        const std::map<std::string, std::string> &getMapValue(void) const;

    private:
        /*! @class Impl.
         *  @brief The class supports behaviors of MDBConfigFile.
         */
        class Impl;

        //!Pointer to Impl class
        std::unique_ptr<Impl> pImpl; /*pgr0359*/
    };
}

#endif /* LIBMDBCONF_MDBCONFIGFILE_H_ */

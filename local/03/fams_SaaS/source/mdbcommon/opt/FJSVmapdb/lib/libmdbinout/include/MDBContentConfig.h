/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBContentConfig.h
 * @brief       The header of MDBContentConfig class.
 * @author      cuongadp
 * @date        2017/04/24
 */
#ifndef LIBMDBINOUT_MDBCONTENTCONFIG_H_
#define LIBMDBINOUT_MDBCONTENTCONFIG_H_

#include <vector>
#include "MDBContentDecodingFormat.h"

namespace mdbcom {
    /*!
     * @enum MDBContentConfig
     * Configuration class for parsing template configuration
     */
    class MDBContentConfig { /* pgr2205 */
    public:
        /*!
         * @brief Construtor
         * @param [in] path Path to configuration file
         * @return None.
         */
        explicit MDBContentConfig(const std::string &path) : configFile {path} {
        };

        /*!
         * @brief Destructor
         * @param None.
         * @return None.
         */
        virtual ~MDBContentConfig(void) = default;

        /*!
         * @brief Copy Construtor
         * @param [in] other Other instance
         * @return None.
         */
        MDBContentConfig(const MDBContentConfig &) = default; /* pgr0571 */ /* pgr2266 */

        /*!
         * @brief Move Construtor
         * @param [in/out] other Other instance
         * @return None.
         */
        MDBContentConfig(MDBContentConfig &&) = default;

        /*!
         * @brief Copy Assignment
         * @param [in] other Other instance
         * @return Reference to current instance.
         */
        MDBContentConfig &operator=(const MDBContentConfig &) = default; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Copy Assignment
         * @param [in] other Other instance
         * @return Reference to current instance.
         */
        MDBContentConfig &operator=(MDBContentConfig &&) = default; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Parse the decoding template from configuration data
         * @param [in] url URL to get template.
         * @param [in] method Method of the URL
         * @param [out] formatTemplate The result template
         * @retval Result::OK Success.
         * @retval Result::NG Failure
         */
        virtual int getFormat(const std::string &url, const std::string &method, std::vector<MDBContentDecodingFormat> &formatTemplate) = 0;

    protected:
        //! key "paths"
        static const std::string KEY_PATHS;
        //! key "basePath"
        static const std::string KEY_BASEPATHS;
        //! key "parameters"
        static const std::string KEY_PARAMETERS;
        //! key "name"
        static const std::string KEY_NAME;
        //! key "schema"
        static const std::string KEY_SCHEMA;
        //! key "ENUM"
        static const std::string KEY_ENUM;
        //! key "KEY_PATTERN"
        static const std::string KEY_PATTERN;
        //! key "properties"
        static const std::string KEY_PROPERTIES;
        //! key "x-nullable"
        static const std::string KEY_NULLABLE;
        //! key "minLength"
        static const std::string KEY_MINLENGTH;
        //! key "maxLength"
        static const std::string KEY_MAXLENGTH;
        //! key "x-errorContinue"
        static const std::string KEY_ERRORCONTINUE;
        //! key "required"
        static const std::string KEY_REQUIRED;
        //! key "type"
        static const std::string KEY_TYPE;
        //! key "items"
        static const std::string KEY_ITEMS;
        //! key "format"
        static const std::string KEY_FORMAT;
        //! key "minimum"
        static const std::string KEY_MINIMUM;
        //! key "maximum"
        static const std::string KEY_MAXIMUM;
        //! key "default"
        static const std::string KEY_DEFAULT;
        //! value "Body" of key "name" of "parameters"
        static const std::string VALUE_BODY;
        //! value "array" of key "type"
        static const std::string VALUE_TYPE_ARRAY;
        //! value "integer" of key "type"
        static const std::string VALUE_TYPE_INTEGER;
        //! value "boolean" of key "type"
        static const std::string VALUE_TYPE_BOOLEAN;
        //! value "number" of key "type"
        static const std::string VALUE_TYPE_NUMBER;
        //! value "string" of key "type"
        static const std::string VALUE_TYPE_STRING;
        //! value "object" of key "type"
        static const std::string VALUE_TYPE_OBJECT;
        //! value "null" of key "type"
        static const std::string VALUE_TYPE_NULL;
        //! value "int32" of key "format"
        static const std::string VALUE_FORMAT_INT32;
        //! value "int64" of key "format"
        static const std::string VALUE_FORMAT_INT64;
        //! Path to configuration file
        std::string configFile;
    };
};

#endif
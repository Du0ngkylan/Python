/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBContentConfig.cpp
 * @brief       The implement of MDBContentConfig class.
 * @author      cuongadp
 * @date        2016/04/24
 */

#include "MDBContentConfig.h"

namespace mdbcom {
    //! key "paths"
    const std::string MDBContentConfig::KEY_PATHS = "paths";
    //! key "basePath"
    const std::string MDBContentConfig::KEY_BASEPATHS = "basePath";
    //! key "parameters"
    const std::string MDBContentConfig::KEY_PARAMETERS = "parameters";
    //! key "name"
    const std::string MDBContentConfig::KEY_NAME = "name";
    //! key "schema"
    const std::string MDBContentConfig::KEY_SCHEMA = "schema";
    //! key "enum"
    const std::string MDBContentConfig::KEY_ENUM = "enum";
    //! key pattern
    const std::string MDBContentConfig::KEY_PATTERN = "pattern";
    //! key "properties"
    const std::string MDBContentConfig::KEY_PROPERTIES = "properties";
    //! key "x-nullable"
    const std::string MDBContentConfig::KEY_NULLABLE = "x-nullable";
    //! key "minLength"
    const std::string MDBContentConfig::KEY_MINLENGTH = "minLength";
    //! key "maxLength"
    const std::string MDBContentConfig::KEY_MAXLENGTH = "maxLength";
    //! key "x-errorContinue"
    const std::string MDBContentConfig::KEY_ERRORCONTINUE = "x-errorContinue";
    //! key "required"
    const std::string MDBContentConfig::KEY_REQUIRED = "required";
    //! key "type"
    const std::string MDBContentConfig::KEY_TYPE = "type";
    //! key "items"
    const std::string MDBContentConfig::KEY_ITEMS = "items";
    //! key "format"
    const std::string MDBContentConfig::KEY_FORMAT = "format";
    //! key "minimum"
    const std::string MDBContentConfig::KEY_MINIMUM = "minimum";
    //! key "maximum"
    const std::string MDBContentConfig::KEY_MAXIMUM = "maximum";
    //! key "default"
    const std::string MDBContentConfig::KEY_DEFAULT = "default";
    //! value "Body" of key "name" of "parameters"
    const std::string MDBContentConfig::VALUE_BODY = "Body";
    //! value "array" of key "type"
    const std::string MDBContentConfig::VALUE_TYPE_ARRAY = "array";
    //! value "integer" of key "type"
    const std::string MDBContentConfig::VALUE_TYPE_INTEGER = "integer";
    //! value "boolean" of key "type"
    const std::string MDBContentConfig::VALUE_TYPE_BOOLEAN = "boolean";
    //! value "number" of key "type"
    const std::string MDBContentConfig::VALUE_TYPE_NUMBER = "number";
    //! value "string" of key "type"
    const std::string MDBContentConfig::VALUE_TYPE_STRING = "string";
    //! value "object" of key "type"
    const std::string MDBContentConfig::VALUE_TYPE_OBJECT = "object";
    //! value "null" of key "type"
    const std::string MDBContentConfig::VALUE_TYPE_NULL = "null";
    //! value "int32" of key "format"
    const std::string MDBContentConfig::VALUE_FORMAT_INT32 = "int32";
    //! value "int64" of key "format"
    const std::string MDBContentConfig::VALUE_FORMAT_INT64 = "int64";
}
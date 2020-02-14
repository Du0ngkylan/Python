/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBJsonParser.cpp
 * @brief       The implement of MDBJsonParser class.
 * @author      trungnv: Initialize MDBJsonParser class.
 * @date        2016/9/23
 */

#include <limits.h>
#include <Poco/StringTokenizer.h>
#include <Poco/Any.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeParser.h>
#include <Poco/RegularExpression.h>
#include <sstream>
#include <iomanip>
#include "MDBJsonParser.h"

namespace mdbcom {

    const std::string MDBJsonParser::COORDINATES_KEY {"coordinates"};
    /*!
     * @brief constructor of MDBJsonParser.
     * @param None.
     * @return None.
     */
    MDBJsonParser::MDBJsonParser(void) : MDBContentParser {}, mDecodingTemplate {}
    {
    }

    /*!
     * @brief Create the JSON key map data from the specified Json Object.
     *
     * This method is used to create the JSON key map data from the specified JSON Object.
     * @param [in] parseDepth the depth of the current JSON hierarchy
     * @param [in] templeteIndex the index of the validating template
     * @param [in] myParentKey the parent key
     * @param [in]
     inputJsonObject the input JSON object.
     * @param [out] outValue the output the JSON key map data.
     * @param [out] keyError the invalid info key
     * @retval  Json::OK if key is found
     * @retval  Json::NG if key is not found
     */
    int MDBJsonParser::toJsonKeyMapData(const int parseDepth, const int templeteIndex,
                                        const std::string &myParentKey, json_t *inputJsonObject, KeyMapType &outJsonMapData,
                                        std::string &keyError, Poco::Any &errorValue)
    {
        mdb_apl_log_start();
        auto outputObj = KeyMapType { }; /* pgr0745 */

        //Get size of validate template array
        auto templeteKeySize = mDecodingTemplate.size(); /* pgr0745 */

        if (static_cast<int>(templeteKeySize) <= templeteIndex)
        {
            errorValue = Poco::Any { };
            auto debugParentKey = myParentKey.c_str(); /* pgr0745 */
            mdb_apl_log_error(
                "JsonDecode Error. Template index is over. parseDepth= %d, index = %d, templeteSize = %d, parentKey = %s",
                parseDepth, templeteIndex, templeteKeySize, debugParentKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        for (auto keyCnt = templeteIndex; keyCnt < static_cast<int>(templeteKeySize); ++keyCnt) /* pgr0745 */ /* pgr0692 */
        {
            auto decodeTemplete = mDecodingTemplate[keyCnt]; /* pgr0745 */
            auto key = decodeTemplete.getKey(); /* pgr0745 */
            auto templateKeyDepth = decodeTemplete.getDepth(); /* pgr0745 */

            // The processing key to the depth of the JSON hierarchy matches
            if (parseDepth != templateKeyDepth)
            {
                if (parseDepth > templateKeyDepth)
                {
                    break;
                }

                continue;
            }

            auto retCode = jsonObjectDecode(parseDepth, keyCnt, myParentKey, inputJsonObject, /* pgr0745 */
                                            decodeTemplete, outputObj, keyError, errorValue);

            if (Json::OK != retCode)
            {
                // Validate the abnormal case processing
                auto debugParentKey = myParentKey.c_str(); /* pgr0745 */
                auto debugKey = key.c_str(); /* pgr0745 */

                if (true == decodeTemplete.getFlagErrorContinue())
                {
                    mdb_apl_log_debug("Decode is failure, but continue. key= %s, parentKey= %s",
                                      debugKey, debugParentKey);

                }
                else
                {
                    mdb_apl_log_error(
                        "Decode is failure, toJsonKeyMapData template[%d], parent = %s, Mydepth = %d, templateDepth = %d",
                        keyCnt, debugParentKey, parseDepth, templateKeyDepth);
                    // Set data for the JSON Map data
                    outJsonMapData = outputObj;
                    mdb_apl_log_end();
                    return retCode;
                }
            }
        }

        // Set data for the JSON Map data
        outJsonMapData = outputObj;
        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Create the JSON key map data from the specified JsonObject.
     *
     * This method is used to create the JSON key map data from the specified JSON Object.
     * @param [in] parseDepth the depth of the current JSON hierarchy
     * @param [in] templeteIndex the index of the validating template
     * @param [in] myParentKey the parent key
     * @param [in] inputJsonObject the input JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outValue the output the JSON key map data
     * @param [out] keyError the invalid info key
     * @retval  Json::OK if create JSON is successful
     * @retval  Json::NG if create JSON is failure
     */
    int MDBJsonParser::jsonObjectDecode(const int parseDepth, const int templeteIndex,
                                        const std::string &myParentKey, json_t *inputJsonObject,
                                        const MDBContentDecodingFormat &decodeTemplete, KeyMapType &outJsonMapData,
                                        std::string &keyError, Poco::Any &errorValue) /* pgr0304 */
    {

        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */

        // Get the JSON object from the specifying key
        auto keyJsonObject = json_object_get(inputJsonObject, key.c_str()); /* pgr0745 */

        // Create the full path key
        auto myFullPathKey = createFullPathKey(myParentKey, key); /* pgr0745 */

        if (nullptr == keyJsonObject)
        {
            errorValue = Poco::Any { };
            mdb_apl_log_end();

            if (Json::NG ==  validateOmitFlag(decodeTemplete, myParentKey, key))
            {
                keyError = myFullPathKey;
                return Json::NG;
            }

            return Json::OK;
        }

        // Validate the keyJsonObject
        if (json_is_null(keyJsonObject)) /* pgr0681 */
        {
            errorValue = Poco::Any { };
            // json_is_null(json) -> ((json) && json_typeof(json) == JSON_NULL)
            keyError = myFullPathKey;
            mdb_apl_log_end();

            if (Json::NG == validateValidNullFlag(decodeTemplete, myParentKey, key, outJsonMapData))
            {
                keyError = myFullPathKey;
                return Json::NG;
            }

            return Json::OK;
        }

        auto retCode = Json::NG; /* pgr0745 */
        auto type = decodeTemplete.getType(); /* pgr0745 */

        // Carry out the following for each type
        // Object

        if (MDBInputKind::OBJECT == type)
        {

            // Validate JSON object
            if (!json_is_object(keyJsonObject)) /* pgr0681 */
            {
                // json_is_object(json) -> ((json) && json_typeof(json) == JSON_OBJECT)
                auto debugParentKey = myParentKey.c_str(); /* pgr0745 */
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_error("JsonDecode. Object type is not object. parentKey= %s, key = %s",
                                  debugParentKey, debugKey);
                keyError = myFullPathKey;
                errorValue = Poco::Any { };
                mdb_apl_log_end();
                return Json::NG;
            }

            auto debugObjectKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_debug("OBJECT KEY= %s", debugObjectKey);
            // Get the child JSON key map data
            auto childMapData = KeyMapType { }; /* pgr0745 */
            retCode = toJsonKeyMapData(parseDepth + 1, templeteIndex + 1, myFullPathKey,
                                       keyJsonObject, childMapData, keyError, errorValue);

            if (Json::OK != retCode)
            {
                auto debugParentKey = myParentKey.c_str(); /* pgr0745 */
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_error(
                    "JsonDecode. Object child element cannot get. parentKey= %s, key= %s",
                    debugParentKey, debugKey);
                outJsonMapData[key] = childMapData;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = childMapData;
        }
        // array(object)
        else if (MDBInputKind::ARRAY_OBJECT == type)
        {
            retCode = arrObjectJsonToJsonKeyMapData(parseDepth, templeteIndex, myFullPathKey, key,
                                                    keyJsonObject, outJsonMapData, keyError, errorValue);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("arrObjectJsonToJsonKeyMapData occurs error.");
                mdb_apl_log_end();
                return retCode;
            }
        }
        // array(string/integer64/integer32/boolean/real)
        else if ((MDBInputKind::ARRAY_STRING == type) || (MDBInputKind::ARRAY_INTEGER64 == type)
                 || (MDBInputKind::ARRAY_INTEGER32 == type) || (MDBInputKind::ARRAY_BOOL == type)
                 || (MDBInputKind::ARRAY_REAL == type))
        {

            auto arrayVectorData = std::vector<Poco::Any> { }; /* pgr0745 */
            auto errIdx = 0;
            retCode = getArrayValue(keyJsonObject, decodeTemplete, arrayVectorData, errorValue, errIdx);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("getArrayValue occurs error.");
                keyError = myFullPathKey + ".[" + std::to_string(errIdx) + "]"; // will be ".[]" if root is a array
                outJsonMapData[key] = arrayVectorData;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = arrayVectorData;
        }
        // string
        else if (MDBInputKind::STRING == type)
        {
            auto stringValue = std::string { }; /* pgr0745 */
            // Get the string value
            retCode = getStringValue(keyJsonObject, decodeTemplete, stringValue, errorValue);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("getStringValue occurs error.");
                keyError = myFullPathKey;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = stringValue;
        }
        // integer32
        else if (MDBInputKind::INTEGER32 == type)
        {
            auto integerValue = 0; /* pgr0745 */
            // Get the integer 32 bit value
            retCode = getInteger32Value(keyJsonObject, decodeTemplete, integerValue, errorValue);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("getInteger32Value occurs error.");
                keyError = myFullPathKey;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = integerValue;
        }
        // integer64
        else if (MDBInputKind::INTEGER64 == type)
        {
            auto longlongValue = 0ll; /* pgr0745 */
            // Get the integer 64 bit value
            retCode = getInteger64Value(keyJsonObject, decodeTemplete, longlongValue, errorValue);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("getInteger64Value occurs error.");
                keyError = myFullPathKey;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = longlongValue;
        }
        // boolean
        else if (MDBInputKind::BOOL == type)
        {
            auto booleanValue = true; /* pgr0745 */
            // Get the boolean value
            retCode = getBooleanValue(keyJsonObject, decodeTemplete, booleanValue);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("getBooleanValue occurs error.");
                keyError = myFullPathKey;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = booleanValue;
        }
        // real
        else if (MDBInputKind::REAL == type)
        {
            auto doubleValue = 0.0; /* pgr0745 */
            // Get the real value
            retCode = getRealValue(keyJsonObject, decodeTemplete, doubleValue);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("getRealValue occurs error.");
                keyError = myFullPathKey;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = doubleValue;
        }
        else if (MDBInputKind::COORDINATES == type)
        {
            auto stringValue = std::string { }; /* pgr0745 */
            // Get the string value
            retCode = getCoordStringValue(keyJsonObject, decodeTemplete, stringValue, errorValue);

            if (Json::OK != retCode)
            {
                mdb_apl_log_error("getCoordStringValue occurs error.");
                keyError = myFullPathKey;
                mdb_apl_log_end();
                return retCode;
            }

            outJsonMapData[key] = stringValue;
        }
        // Other
        else
        {
            errorValue = Poco::Any { };
            auto debugParenKey = myParentKey.c_str(); /* pgr0745 */
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_error("JsonDecode. Unexpected type. parentKey= %s key= %s type= %d",
                              debugParenKey, debugKey, type);
            keyError = myFullPathKey;
            mdb_apl_log_end();
            return Json::NG;
        }

        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Get the Array data from the specified JSON Object.
     *
     * This method is used to get the Array data from the specified JSON Object.
     * @param [in] inputJsonObject  the pointer to JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outArrayValue the output array data
     * @retval Json::OK if success
     * @retval Json::NG if failure
     */
    int MDBJsonParser::getArrayValue(json_t *inputJsonObject,
                                     const MDBContentDecodingFormat &decodeTemplete, std::vector<Poco::Any> &outArrayValue,
                                     Poco::Any &errorValue, int &errIdx)
    {

        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */

        // Validate the array JSON object
        if (!json_is_array(inputJsonObject)) /* pgr0681 */
        {
            errorValue = Poco::Any {};
            // json_is_array(json) -> ((json) && json_typeof(json) == JSON_ARRAY)
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Object type is not array. key= %s", debugKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        auto arrayNum = json_array_size(inputJsonObject); /* pgr0745 */
        auto debugKeySize = key.c_str(); /* pgr0745 */
        mdb_apl_log_debug("ARRAY VALUE KEY= %s, arrayNum = %d", debugKeySize, arrayNum);

        auto retCode = Json::NG; /* pgr0745 */
        auto type = decodeTemplete.getType(); /* pgr0745 */

        for (auto arrayIndex = std::size_t { 0 }; arrayIndex < arrayNum; ++arrayIndex) /* pgr0745 */
        {

            errIdx = arrayIndex;
            // Get JSON element from array JSON object
            auto elementJsonObject = json_array_get(inputJsonObject, arrayIndex); /* pgr0745 */

            if (nullptr == elementJsonObject)
            {
                errorValue = Poco::Any {};
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical(
                    "Array Object wrjson_array_get is null. key= %s, arrayIndex= %d", debugKey,
                    arrayIndex);
                mdb_apl_log_end();
                return Json::NG;
            }

            if (MDBInputKind::ARRAY_STRING == type)
            {
                // String
                auto elementStringData = std::string { }; /* pgr0745 */
                retCode = getStringValue(elementJsonObject, decodeTemplete, elementStringData, errorValue);

                if (Json::OK != retCode)
                {
                    auto debugKey = key.c_str(); /* pgr0745 */
                    mdb_apl_log_critical("Array string Object get error. key= %s, arrayIndex= %d",
                                         debugKey, arrayIndex);
                    mdb_apl_log_end();
                    return retCode;
                }

                outArrayValue.push_back(elementStringData);

            }
            else if (MDBInputKind::ARRAY_INTEGER64 == type)
            {
                // Integer64
                auto elementLonglongData = 0ll; /* pgr0745 */
                retCode = getInteger64Value(elementJsonObject, decodeTemplete, elementLonglongData, errorValue);

                if (Json::OK != retCode)
                {
                    auto debugKey = key.c_str(); /* pgr0745 */
                    mdb_apl_log_critical(
                        "Array integer64 Object get error. key= %s, arrayIndex= %d", debugKey,
                        arrayIndex);
                    mdb_apl_log_end();
                    return retCode;
                }

                outArrayValue.push_back(elementLonglongData);

            }
            else if (MDBInputKind::ARRAY_INTEGER32 == type)
            {
                // Integer32
                auto elementIntegerData = 0; /* pgr0745 */
                retCode = getInteger32Value(elementJsonObject, decodeTemplete, elementIntegerData, errorValue);

                if (Json::OK != retCode)
                {
                    auto debugKey = key.c_str(); /* pgr0745 */
                    mdb_apl_log_critical(
                        "Array integer32 Object get error. key= %s, arrayIndex= %d", debugKey,
                        arrayIndex);
                    mdb_apl_log_end();
                    return retCode;
                }

                outArrayValue.push_back(elementIntegerData);

            }
            else if (MDBInputKind::ARRAY_BOOL == type)
            {
                // Boolean
                auto elementBoolData = true; /* pgr0745 */
                retCode = getBooleanValue(elementJsonObject, decodeTemplete, elementBoolData);

                if (Json::OK != retCode)
                {
                    auto debugKey = key.c_str(); /* pgr0745 */
                    mdb_apl_log_critical("Array boolean Object get error. key= %s, arrayIndex= %d",
                                         debugKey, arrayIndex);
                    mdb_apl_log_end();
                    return retCode;
                }

                outArrayValue.push_back(elementBoolData);

            }
            else if (MDBInputKind::ARRAY_REAL == type)
            {
                // Real
                auto elementDoubleData = 0.0; /* pgr0745 */
                retCode = getRealValue(elementJsonObject, decodeTemplete, elementDoubleData);

                if (Json::OK != retCode)
                {
                    auto debugKey = key.c_str(); /* pgr0745 */
                    mdb_apl_log_critical("Array real Object get error. key= %s, arrayIndex= %d",
                                         debugKey, arrayIndex);
                    mdb_apl_log_end();
                    return retCode;
                }

                outArrayValue.push_back(elementDoubleData);
            }
            else
            {
                errorValue = Poco::Any { /*empty*/ };
                // Other
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical("Unexpected Array Kind.key= %s, type= %d", debugKey, type);
                mdb_apl_log_end();
                return Json::NG;
            }
        }

        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Get the string data from the specified JSON Object.
     *
     * This method is used to get the string data from the specified JSON Object.
     * @param [in] inputJsonObject  the pointer to JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outStringValue the output string data
     * @retval Json::OK if success
     * @retval Json::NG if failure
     */
    int MDBJsonParser::getStringValue(json_t *inputJsonObject,
                                      const MDBContentDecodingFormat &decodeTemplete, std::string &outStringValue,
                                      Poco::Any &errorValue)
    {

        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */
        auto defaultValue = std::string { }; /* pgr0745 */

        if (nullptr == inputJsonObject)
        {
            mdb_apl_log_critical("The inputJsonObject is nullptr");
            mdb_apl_log_end();
            return Json::NG;
        }

        if (typeid(std::string) == decodeTemplete.getDefaultValue().type())
        {
            defaultValue = Poco::AnyCast<std::string>(decodeTemplete.getDefaultValue());
        }
        else if (MDBInputKind::ARRAY_STRING == decodeTemplete.getType())
        {
            defaultValue = std::string {};
        }
        else
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical(
                "decodeTemplete format error. default value is not string. key= %s", debugKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        // Validate the JSON string object
        if (!json_is_string(inputJsonObject)) /* pgr0681 */
        {
            // json_is_string(json) -> ((json) && json_typeof(json) == JSON_STRING)
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Object type is not string. key= %s type= %d", debugKey,
                                 json_typeof(inputJsonObject));
            mdb_apl_log_end();
            return Json::NG;
        }

        // Get string value from input JSON object
        auto stringValue = static_cast<std::string>(json_string_value(inputJsonObject)); /* pgr0745 */

        if (true == stringValue.empty())
        {
            // Validate the flag empty of validated decode template
            auto debugKey = key.c_str(); /* pgr0745 */

            if (true == decodeTemplete.getFlagValidEmpty())
            {
                auto debugDefaultValue = defaultValue.c_str(); /* pgr0745 */
                mdb_apl_log_debug("Object is empty. Set default value. key= %s, defaultValue = %s",
                                  debugKey, debugDefaultValue);
                outStringValue = defaultValue;
                mdb_apl_log_end();
                return Json::OK;
            }

            errorValue = stringValue;
            mdb_apl_log_critical("Empty is not allowed. key= %s", debugKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        auto pattern = decodeTemplete.getPattern(); /* pgr0745 */

        if (false == isMatchedPattern(pattern, stringValue))
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("key [%s] doesn't match pattern", debugKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        auto format = decodeTemplete.getDateTimeFormat(); /* pgr0745 */

        if (false == isMatchedDateTimeFormat(format, stringValue))
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("key [%s] doesn't match format", debugKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        // Validate the flag value check of validated decode template
        if (true == decodeTemplete.getFlagValueCheck())
        {
            // Validate the rank value
            long long compareValue = 0;

            for (char c : stringValue)
            {
                // a letter in UTF8 starts with 11xxxxxx or 0xxxxxxx
                // which is followed by bytes start with 10xxxxxx
                // so to check a byte whether is start byte of a letter or not
                // check it differs 10xxxxxx or not.
                // 1100 0000 & xxxx xxxx != 1000 0000
                if (0x80 != (c & 0xC0))
                {
                    ++compareValue;
                }
            }

            if ((compareValue < decodeTemplete.getCheckLower())
                    || (compareValue > decodeTemplete.getCheckHigher()))
            {
                errorValue = stringValue;
                auto debugKey = key.c_str(); /* pgr0745 */
                auto debugStringValue = stringValue.c_str(); /* pgr0745 */
                mdb_apl_log_critical(
                    "String Value is range over. key= %s, Value= %s, valueSize= %lld", debugKey,
                    debugStringValue, compareValue);
                mdb_apl_log_end();
                return Json::NG;
            }
        }

        outStringValue = stringValue;
        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Get the string data from the specified JSON Object.
     *
     * This method is used to get the string data from the specified JSON Object.
     * @param [in] inputJsonObject  the pointer to JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outStringValue the output string data
     * @retval Json::OK if success
     * @retval Json::NG if failure
     */
    int MDBJsonParser::getCoordStringValue(json_t *inputJsonObject,
                                           const MDBContentDecodingFormat &decodeTemplete, std::string &outStringValue,
                                           Poco::Any &errorValue)
    {

        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */
        auto debugKey = key.c_str(); /* pgr0745 */
        auto defaultValue = std::string { }; /* pgr0745 */

        if (nullptr == inputJsonObject)
        {
            mdb_apl_log_critical("The inputJsonObject is nullptr");
            mdb_apl_log_end();
            return Json::NG;
        }

        // Validate the JSON coord object
        if (!json_is_array(inputJsonObject)) /* pgr0681 */
        {
            mdb_apl_log_critical("coordinates is not array. key= %s type= %d", debugKey,
                                 json_typeof(inputJsonObject));
            mdb_apl_log_end();
            return Json::NG;
        }

        auto arraySize = json_array_size(inputJsonObject); /* pgr0745 */

        if (0 >= arraySize) /* pgr0681 */
        {
            mdb_apl_log_critical("size of coordinates is invalid. key= %s type= %d", debugKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        // Get string value from input JSON object
        char *coordString = json_dumps(inputJsonObject, JSON_REAL_PRECISION(MAX_PRECISION_REAL_VALUE));
        auto stringValue = std::string{coordString}; /* pgr0745 */
        delete coordString;

        auto pattern = decodeTemplete.getPattern(); /* pgr0745 */

        if (false == isMatchedCoordPattern(pattern, stringValue))
        {
            mdb_apl_log_critical("key [%s] doesn't match pattern", debugKey);
            errorValue = stringValue;
            mdb_apl_log_end();
            return Json::NG;
        }

        outStringValue = stringValue;
        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Get the integer 64 bit data from the specified JSON Object.
     *
     * This method is used to get the integer 64 bit data from the specified JSON Object.
     * @param [in] inputJsonObject  the pointer to JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outLonglongValue the output integer 64 bit data
     * @retval Json::OK if success
     * @retval Json::NG if failure
     */
    int MDBJsonParser::getInteger64Value(json_t *inputJsonObject,
                                         const MDBContentDecodingFormat &decodeTemplete, long long &outLonglongValue,
                                         Poco::Any &errorValue)
    {
        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */

        if (nullptr == inputJsonObject)
        {
            mdb_apl_log_critical("The inputJsonObject is nullptr");
            mdb_apl_log_end();
            return Json::NG;
        }

        auto debugKey = key.c_str(); /* pgr0745 */

        // Validate the JSON integer object
        if (!json_is_integer(inputJsonObject)) /* pgr0681 */
        {
            // json_is_integer(json) -> ((json) && json_typeof(json) == JSON_INTEGER)
            mdb_apl_log_critical("Object type is not integer. key= %s type = %d", debugKey,
                                 json_typeof(inputJsonObject));
            mdb_apl_log_end();
            return Json::NG;
        }

        // Get the integer 64bit value from input JSON object
        auto longlongValue = json_integer_value(inputJsonObject); /* pgr0745 */
        mdb_apl_log_debug("Integer64 Object Set. value= %lld, key= %s", longlongValue, debugKey);

        // Validate the flag value check of validated decode template
        if (true == decodeTemplete.getFlagValueCheck())
        {
            // Validate the rank value
            if ((longlongValue < decodeTemplete.getCheckLower())
                    || (longlongValue > decodeTemplete.getCheckHigher()))
            {
                errorValue = longlongValue;
                mdb_apl_log_critical("Value is range over. key= %s length=%lld", debugKey,
                                     longlongValue);
                mdb_apl_log_end();
                return Json::NG;
            }
        }

        outLonglongValue = longlongValue;
        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Get the integer 32 bit data from the specified JSON Object.
     *
     * This method is used to get the integer 32 bit data from the specified JSON Object.
     * @param [in] inputJsonObject  the pointer to JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outIntValue the output integer 32 bit data
     * @retval Json::OK if success
     * @retval Json::NG if failure
     */
    int MDBJsonParser::getInteger32Value(json_t *inputJsonObject,
                                         const MDBContentDecodingFormat &decodeTemplete, int &outIntValue,
                                         Poco::Any &errorValue)
    {

        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */

        if (nullptr == inputJsonObject)
        {
            mdb_apl_log_critical("The inputJsonObject is nullptr");
            mdb_apl_log_end();
            return Json::NG;
        }

        // Validate the JSON integer 32 bit Object
        if (true != json_is_integer32(inputJsonObject))
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Object type is not integer32. key= %s, type = %d", debugKey,
                                 json_typeof(inputJsonObject));
            mdb_apl_log_end();
            return Json::NG;
        }

        // Get the integer 32 bit value from the input JSON object
        auto integer32Value = json_integer32_value(inputJsonObject); /* pgr0745 */

        // Validate the flag value check of validated decode template
        if (true == decodeTemplete.getFlagValueCheck())
        {
            // Validate the rank value
            auto compareValue = static_cast<long long>(integer32Value); /* pgr0745 */

            if ((compareValue < decodeTemplete.getCheckLower())
                    || (compareValue > decodeTemplete.getCheckHigher()))
            {
                errorValue = integer32Value;
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical("Value is out of range. key= %s, Value = %d", debugKey,
                                     compareValue);
                mdb_apl_log_end();
                return Json::NG;
            }
        }

        if (0 < decodeTemplete.getEnumList().size())
        {

            auto compareValue = static_cast<long long>(integer32Value); /* pgr0745 */
            auto found = false;
            auto enumList = decodeTemplete.getEnumList();

            for (auto it = enumList.begin(); it != enumList.end(); it++)
            {
                if (*it == compareValue)
                {
                    found = true;
                    break;
                }
            }

            if (false == found)
            {
                errorValue = integer32Value;
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical("Value is out of enum list. key= %s, Value = %d", debugKey,
                                     compareValue);
                mdb_apl_log_end();
                return Json::NG;
            }
        }

        outIntValue = integer32Value;
        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Get the boolean data from the specified JSON Object.
     *
     * This method is used to get the boolean data from the specified JSON Object.
     * @param [in] inputJsonObject  the pointer to JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outBoolValue the boolean data
     * @retval Json::OK if success
     * @retval Json::NG if failure
     */
    int MDBJsonParser::getBooleanValue(json_t *inputJsonObject,
                                       const MDBContentDecodingFormat &decodeTemplete, bool &outBoolValue)
    {

        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */

        if (nullptr == inputJsonObject)
        {
            mdb_apl_log_critical("The inputJsonObject is nullptr");
            mdb_apl_log_end();
            return Json::NG;
        }

        // Validate the boolean JSON object
        if (!json_is_boolean(inputJsonObject)) /* pgr0681 */
        {
            // json_is_boolean(json) -> (json_is_true(json) || json_is_false(json))
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Object type is not boolean. key= %s, type = %d", debugKey,
                                 json_typeof(inputJsonObject));
            mdb_apl_log_end();
            return Json::NG;
        }

        // Get the boolean value from input JSON object
        auto booleanValue = json_boolean_value(inputJsonObject); /* pgr0681 */
        // json_is_boolean(json) -> (json_is_true(json) || json_is_false(json))
        outBoolValue = booleanValue;
        mdb_apl_log_end();
        return Json::OK;

    }

    /*!
     * @brief  Get the real data from the specified JSON Object.
     *
     * This method is used to get the real data from the specified JSON Object.
     * @param [in] inputJsonObject  the pointer to JSON object
     * @param [in] decodeTemplete the validating template
     * @param [out] outDoubleValue the real data
     * @retval Json::OK if success
     * @retval Json::NG if failure
     */
    int MDBJsonParser::getRealValue(json_t *inputJsonObject,
                                    const MDBContentDecodingFormat &decodeTemplete, double &outDoubleValue)
    {

        mdb_apl_log_start();
        auto key = decodeTemplete.getKey(); /* pgr0745 */

        if (nullptr == inputJsonObject)
        {
            mdb_apl_log_critical("The inputJsonObject is nullptr");
            mdb_apl_log_end();
            return Json::NG;
        }

        // Validate the real JSON object
        if (!json_is_real(inputJsonObject)) /* pgr0681 */
        {
            // json_is_real(json) -> ((json) && json_typeof(json) == JSON_REAL)
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Object type is not real. key= %s, type = %d", debugKey,
                                 json_typeof(inputJsonObject));
            mdb_apl_log_end();
            return Json::NG;
        }

        // Get the real value from the input JSON object
        auto doubleValue = json_real_value(inputJsonObject); /* pgr0745 */

        //validate pattern
        char buffer[30];
        memset(buffer, '\0', sizeof(char) * 30);
        snprintf(buffer, sizeof(buffer), "%.8f", doubleValue);

        std::string doubleValStr {buffer};
        auto defaultEndCutPos = doubleValStr.find('.') + 2;
        auto calEndCutPos = doubleValStr.find_last_not_of('0') + 1;
        auto endCutPos = calEndCutPos > defaultEndCutPos ? calEndCutPos : defaultEndCutPos;
        doubleValStr = doubleValStr.substr(0, endCutPos);

        auto pattern = decodeTemplete.getPattern(); /* pgr0745 */
        mdb_apl_log_debug("got pattern [%s]", pattern.c_str());

        if (false == isMatchedPattern(pattern, doubleValStr))
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_critical("key [%s] doesn't match pattern", debugKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        // Validate the flag value check of validated decode template
        if (true == decodeTemplete.getFlagValueCheck())
        {
            // Validate the rank value

            if ((doubleValue < decodeTemplete.getCheckLower())
                    || (doubleValue > decodeTemplete.getCheckHigher()))
            {
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical("Value is out of range. key= %s, Value = %f", debugKey,
                                     doubleValue);
                mdb_apl_log_end();
                return Json::NG;
            }
        }

        outDoubleValue = doubleValue;
        mdb_apl_log_end();
        return Json::OK;
    }

    // --------------------------------------------------------------------------------
    // Encode
    // --------------------------------------------------------------------------------

    /*!
     * @brief Encode from the JSON map data to the JSON string.
     *
     * This method is used to encode from the JSON map data to the JSON string.
     * @param [in] jsonKeyMapData the input JSON map data
     * @param [out] createJsonString the output JSON string
     * @retval  Json::OK if encoding is successful
     * @retval  Json::NG if encoding is failure
     */
    int MDBJsonParser::encodeKeyMapData(const KeyMapType &jsonKeyMapData, /* pgr0304 */
                                        std::string &createJsonString, bool printDebugJSON)
    {

        mdb_apl_log_start();

        // Create the JSON object from the JSON key map data
        auto createJsonObject = toJsonObject(jsonKeyMapData); /* pgr0745 */

        if (nullptr == createJsonObject)
        {
            mdb_sys_log_out("F000202");
            mdb_apl_log_critical("Create the JSON object by toJsonObject function occurs error.");
            mdb_apl_log_end();
            return Json::NG;
        }

        // Create the JSON string
        auto result = json_dumps(createJsonObject, /* pgr0745 */
                                 static_cast<std::size_t>(MDBMFlag::ENCODE_FLGS) | JSON_REAL_PRECISION(MAX_PRECISION_REAL_VALUE));

        if (nullptr == result)
        {
            mdb_sys_log_out("F000202");
            mdb_apl_log_critical("Create the JSON string by json_dumps function occurs error.");
            mdb_apl_log_end();
            return Json::NG;
        }

        createJsonString = result;

        if (true == printDebugJSON)
        {
            mdb_apl_log_debug("The output JSON string : %s", result);
        }

        // Free memory
        free(result); /* pgr1006 */
        json_decref(createJsonObject);
        mdb_apl_log_end();
        return Json::OK;
    }

    /*!
     * @brief Create the JSON string for REST respond.
     *
     * This method is used to create the JSON string for REST respond.
     * @param [in] jsonKeyMapData the input JSON map data
     * @param [out] createJsonString the output JSON string
     * @retval  Json::OK if encoding is successful
     * @retval  Json::NG if encoding is failure
     */
    json_t *MDBJsonParser::toJsonObject(const Poco::Any jsonKeyValue)
    {

        mdb_apl_log_start();

        // Validate the type of JSON key value
        if (jsonKeyValue.type() == typeid(std::vector<Poco::Any>))
        {
            mdb_apl_log_end();
            return handleToJsonArrayObject(jsonKeyValue);
        }
        else if (jsonKeyValue.type() == typeid(std::map<std::string, Poco::Any>))
        {
            mdb_apl_log_end();
            return handleToJsonObject(jsonKeyValue);
        }
        else if (jsonKeyValue.type() == typeid(std::string))
        {

            auto setString = Poco::AnyCast<std::string>(jsonKeyValue); /* pgr0745 */
            // Create new reference JSON string object
            auto stringObject = json_string(setString.c_str()); /* pgr0745 */

            if (nullptr == stringObject)
            {
                mdb_apl_log_critical("JsonEncode wrjson_string Error.");
                mdb_apl_log_end();
                return nullptr;
            }

            mdb_apl_log_end();
            return stringObject;
        }
        // int(32bit Integer)の場吁E
        else if (jsonKeyValue.type() == typeid(int))
        {

            auto setInteger = Poco::AnyCast<int>(jsonKeyValue); /* pgr0745 */
            // Create new reference JSON integer 32 bit object
            auto integerObject = json_integer((json_int_t)setInteger); /* pgr0745 */

            if (nullptr == integerObject)
            {
                mdb_apl_log_critical("JsonEncode wrjson_integer32 Error.");
                mdb_apl_log_end();
                return nullptr;
            }

            mdb_apl_log_end();
            return integerObject;
        }
        else if (jsonKeyValue.type() == typeid(long long))
        {

            auto setLonglong = Poco::AnyCast<long long>(jsonKeyValue); /* pgr0745 */
            // Create new reference JSON integer 64 bit object
            auto lonlongObject = json_integer(setLonglong); /* pgr0745 */

            if (nullptr == lonlongObject)
            {
                mdb_apl_log_critical("JsonEncode wrjson_integer Error.");
                mdb_apl_log_end();
                return nullptr;
            }

            mdb_apl_log_end();
            return lonlongObject;
        }
        else if (jsonKeyValue.type() == typeid(bool))
        {
            auto setBoolean = Poco::AnyCast<bool>(jsonKeyValue); /* pgr0745 */
            // Create new reference JSON boolean object
            auto booleanObject = json_boolean(setBoolean); /* pgr0745 */

            if (nullptr == booleanObject)
            {
                mdb_apl_log_critical("JsonEncode wrjson_boolean Error.");
                mdb_apl_log_end();
                return nullptr;
            }

            mdb_apl_log_end();
            return booleanObject;
        }
        else if (jsonKeyValue.type() == typeid(double))
        {
            auto setDouble = Poco::AnyCast<double>(jsonKeyValue); /* pgr0745 */
            // Create new reference JSON real object
            auto doubleObject = json_real(setDouble); /* pgr0745 */

            if (nullptr == doubleObject)
            {
                mdb_apl_log_critical("JsonEncode wrjson_real Error.");
                mdb_apl_log_end();
                return nullptr;
            }

            mdb_apl_log_end();
            return doubleObject;
        }
        else if (jsonKeyValue.type() == typeid(void *))
        {
            // Create new reference JSON null object
            auto nullObject = json_null(); /* pgr0745 */

            if (nullptr == nullObject)
            {
                mdb_apl_log_critical("JsonEncode wrjson_null Error.");
                mdb_apl_log_end();
                return nullptr;
            }

            mdb_apl_log_end();
            return nullObject;
        }
        else
        {
            // Other
        }

        auto debugType = jsonKeyValue.type().name(); /* pgr0745 */
        mdb_apl_log_critical("Unexpected JSON key value type. type= %s", debugType);
        mdb_apl_log_end();
        return nullptr;
    }

    /*!
     * @brief  Check the specified JSON object whether it is JSON integer 32 bit.
     *
     * This method is used to check the specified JSON object whether it is JSON integer 32 bit.
     * @param [in] json  the pointer to JSON object
     * @retval true if valid
     * @retval false if invalid
     */
    bool MDBJsonParser::json_is_integer32(const json_t *json)
    {

        mdb_apl_log_start();
        auto wrjsonInt = 0ll; /* pgr0745 */
        // Validate JSON integer
        auto boolVal = json_is_integer(json); /* pgr0681 *//* pgr0745 */

        // json_is_integer(json)  ((json) && json_typeof(json) == JSON_INTEGER)
        if (true == boolVal)
        {
            wrjsonInt = json_integer_value(json);

            // Validate rank value
            if ((wrjsonInt >= INT_MIN) && (wrjsonInt <= INT_MAX))
            {
                boolVal = true;
            }
            else
            {
                boolVal = false;
            }
        }

        mdb_apl_log_end();
        return boolVal;
    }

    /*!
     * @brief  Get the integer 32 bit value from specified JSON object.
     *
     * This method is used to get the integer 32 bit value from specified JSON object.
     * @param [in] integer  the pointer to JSON object
     * @return  Integer 32 bit value
     */
    std::int32_t MDBJsonParser::json_integer32_value(const json_t *integer)
    {

        mdb_apl_log_start();
        auto wrjsonInt32 = 0; /* pgr0745 */
        // Get the integer value from the specified JSON object
        auto wrjsonInt = json_integer_value(integer); /* pgr0745 */

        // Validate rank value
        if ((wrjsonInt >= INT_MIN) && (wrjsonInt <= INT_MAX))
        {
            wrjsonInt32 = static_cast<int>(wrjsonInt);
        }
        else
        {
            mdb_apl_log_critical("The value is out of scope of Integer 32bit");
        }

        mdb_apl_log_end();
        return wrjsonInt32;
    }

    /*!
     * @brief Convert an array object JSON to the JSON Key Map data.
     *
     * This method is used to convert an array object JSON to the JSON Key Map data.
     * @param [in] parseDepth the depth of the current JSON hierarchy
     * @param [in] templeteIndex the index of the validating template
     * @param [in] myParentKey the parent key
     * @param [in] key the specifying key
     * @param [in] inputJsonObject the input JSON object
     * @param [out] outValue the output the JSON key map data
     * @param [out] keyError the invalid info key
     * @retval  true if don't have any problem
     * @retval  false if have an problem
     */
    int MDBJsonParser::arrObjectJsonToJsonKeyMapData(const int parseDepth, const int templeteIndex,
            const std::string &myParentKey, const std::string &key, json_t *inputJsonObject,
            KeyMapType &outJsonMapData, std::string &keyError, Poco::Any &errorValue) /* pgr0304 */
    {

        mdb_apl_log_start();

        // Create the full path key
        //        auto myParentKey = createFullPathKey(myParentKey, key); /* pgr0745 */
        // Validate array JSON
        if (!json_is_array(inputJsonObject)) /* pgr0681 */
        {
            errorValue = Poco::Any { };
            //json_is_array(json) -> ((json) && json_typeof(json) == JSON_ARRAY)
            auto debugParentKey = myParentKey.c_str(); /* pgr0745 */
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_error("JsonDecode. Object type is not array. parentKey= %s, key = %s",
                              debugParentKey, debugKey);
            keyError = myParentKey;
            mdb_apl_log_end();
            return Json::NG;
        }

        auto arrayVectorData = std::vector<Poco::Any> { }; /* pgr0745 */
        auto arrayNum = json_array_size(inputJsonObject); /* pgr0745 */

        for (auto arrayIndex = std::size_t { 0 }; arrayIndex < arrayNum; ++arrayIndex) /* pgr0745 */
        {
            // Get JSON object in JSON array
            auto elementJsonObject = json_array_get(inputJsonObject, arrayIndex); /* pgr0745 */

            auto fullpathkey = myParentKey + ".[" + std::to_string(arrayIndex) + "]"; // will be ".[]" if root is a array

            if (nullptr == elementJsonObject)
            {
                errorValue = Poco::Any { };
                auto debugParentKey = fullpathkey.c_str(); /* pgr0745 */
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_error(
                    "JsonDecode. Array Object wrjson_array_get error. parentKey= %s, key = %s, arrayIndex = %d",
                    debugParentKey, debugKey, arrayIndex);
                keyError = fullpathkey;
                mdb_apl_log_end();
                return Json::NG;
            }

            // To get element Map data from the array
            auto elementMapData = KeyMapType { }; /* pgr0745 */
            auto retCode = toJsonKeyMapData(parseDepth + 1, templeteIndex + 1, fullpathkey,
                                            elementJsonObject, elementMapData, keyError, errorValue);

            if (Json::OK != retCode)
            {
                auto debugParentKey = fullpathkey.c_str(); /* pgr0745 */
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_error(
                    "JsonDecode. Array Object cannot get. parentKey= %s, key = %s, arrayIndex = %d",
                    debugParentKey, debugKey, arrayIndex);
                outJsonMapData[key] = arrayVectorData;
                mdb_apl_log_end();
                return retCode;
            }

            arrayVectorData.push_back(elementMapData);
        }

        outJsonMapData[key] = arrayVectorData;
        mdb_apl_log_end();
        return Json::OK;
    }

    /*!
     * @brief Convert from any to an JSON array object.
     *
     * This method is used to convert from any to an JSON array object.
     * @param [in] jsonKeyValue the vector any value
     * @return The JSON array object
     */
    json_t *MDBJsonParser::handleToJsonArrayObject(const Poco::Any &jsonKeyValue)
    {

        mdb_apl_log_start();
        // Create new reference JSON array object
        auto jsonArrayObject = json_array(); /* pgr0745 */

        if (nullptr == jsonArrayObject)
        {
            mdb_apl_log_critical("JsonEncode Array Object create error. type=vector");
            mdb_apl_log_end();
            return nullptr;
        }

        auto vectorValue = Poco::AnyCast<std::vector<Poco::Any> >(jsonKeyValue); /* pgr0745 */
        auto vectorValueSize = vectorValue.size(); /* pgr0745 */

        for (auto cnt = std::size_t { 0 }; cnt < vectorValueSize; cnt++) /* pgr0745 */
        {
            // Get JSON object from array
            auto jsonArrayElementObject = toJsonObject(vectorValue[cnt]); /* pgr0745 */

            if (nullptr == jsonArrayElementObject)
            {
                mdb_apl_log_critical("JsonEncode Array Element Object encode error. type=vector");
                mdb_apl_log_end();
                return nullptr;
            }

            // Append array
            auto retCode = json_array_append(jsonArrayObject, jsonArrayElementObject); /* pgr0745 */
            //Free memory
            json_decref(jsonArrayElementObject);

            if (MDB_JSON_JANSSON_RESULT_OK != retCode)
            {
                mdb_apl_log_critical(
                    "JsonEncode Array Object append error. type=vector, retCode= %d", retCode);
                mdb_apl_log_end();
                return nullptr;
            }
        }

        mdb_apl_log_end();
        return jsonArrayObject;
    }

    /*!
     * @brief Convert from any to an JSON object.
     *
     * This method is used to convert from any to an JSON object.
     * @param [in] jsonKeyValue the  any value
     * @return The JSON object
     */
    json_t *MDBJsonParser::handleToJsonObject(const Poco::Any &jsonKeyValue)
    {

        // Create new reference JSON  object
        auto jsonMapObject = json_object(); /* pgr0745 */

        if (nullptr == jsonMapObject)
        {
            mdb_apl_log_critical("JsonEncode Object create error. type=map");
            mdb_apl_log_end();
            return nullptr;
        }

        auto objectKeyMapData = Poco::AnyCast<std::map<std::string, Poco::Any> >(jsonKeyValue); /* pgr0745 */

        for (auto objItr = objectKeyMapData.begin(); /* pgr0745 */
                objItr != objectKeyMapData.end(); ++objItr)
        {
            auto serialKey = objItr->first; /* pgr0745 */
            std::string key = serialKey.substr(MAX_NUM_DIGIT_KEYS);
            auto value = objItr->second; /* pgr0745 */
            // Get JSON object in MAP
            auto jsonMapElementObject = toJsonObject(value); /* pgr0745 */

            if (nullptr == jsonMapElementObject)
            {
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical(
                    "JsonEncode Map Element Object encode error. type=map, key= %s", debugKey);
                mdb_apl_log_end();
                return nullptr;
            }

            auto retCode = json_object_set(jsonMapObject, key.c_str(), jsonMapElementObject); /* pgr0745 */
            // Free memory
            json_decref(jsonMapElementObject);

            if (MDB_JSON_JANSSON_RESULT_OK != retCode)
            {
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical("JsonEncode Object set error. type=map, key= %s retCode= %d",
                                     debugKey, retCode);
                mdb_apl_log_end();
                return nullptr;
            }

        }

        mdb_apl_log_end();
        return jsonMapObject;
    }

    /*!
     * @brief Get key error when JSON string contains invalid information.
     *
     * This method is used to get key error when JSON string contains invalid information.
     * @param [in] jsonString the  JSON string
     * @param [in] position the position of an error in JSON string
     * @return The key error
     */
    std::string MDBJsonParser::getJSonKeyError(const std::string &jsonString, int position)
    {
        mdb_apl_log_start();
        auto keyError = std::string { };

        if (true == jsonString.empty() || static_cast<size_t>(position) > jsonString.size())
        {
            mdb_apl_log_end();
            return keyError;
        }

        auto tempString = std::string { jsonString };
        static_cast<void>(tempString.erase(position - 1, tempString.size()));

        auto iColonPos = tempString.find_last_of(':');

        if (std::string::npos == iColonPos)
        {
            mdb_apl_log_end();
            return keyError;
        }

        static_cast<void>(tempString.erase(iColonPos, tempString.size()));

        auto iFirstPos = tempString.find_last_of('"');

        if (std::string::npos == iFirstPos)
        {
            mdb_apl_log_end();
            return keyError;
        }

        static_cast<void>(tempString.erase(iFirstPos, tempString.size()));

        auto iLastPos = findLastQuotationMark(tempString);

        if (std::string::npos == static_cast<size_t>(iLastPos))
        {
            mdb_apl_log_end();
            return keyError;
        }

        keyError = jsonString.substr(iLastPos + 1, iFirstPos - iLastPos - 1);
        mdb_apl_log_end();
        return keyError;
    }

    /*!
     * @brief Find the last quotation mark of key error.
     *
     * This method is used to find the last quotation mark of key error.
     * @param [in] jsonString the  JSON string
     * @return The position of last quotation mark
     */
    int MDBJsonParser::findLastQuotationMark(const std::string &jsonString)
    {

        auto iPosition = jsonString.find_last_of('"');

        if ((std::string::npos == iPosition) || ('\\' != jsonString[iPosition - 1]))
        {

            return iPosition;
        }

        auto tempString = std::string { jsonString };
        static_cast<void>(tempString.erase(iPosition, tempString.size()));

        return findLastQuotationMark(tempString);
    }

    /*!
     * @brief Decode from the JSON string to the JSON map data base on the validating template.
     *
     * This method is used to decode from the JSON string to the JSON map data base on the validating template.
     * @param [in] inputJsonString the JSON string
     * @param [in] the validating template.
     * @param [in] decodeBaseKey the root key JSON data
     * @param [out] keyMapData the output JSON map data
     * @param [out] outputFirstKeyError the key error when decoding encounters the first key which has the invalid info
     * @retval  Json::OK if decoding is successful
     * @retval  Json::NG if decoding is failure
     */

    int MDBJsonParser::decodeKeyMapData(const std::string &inputJsonString, /* pgr0746 */
                                        const std::vector<MDBContentDecodingFormat> &jsonDecodeTemplete,  /* pgr0304 */
                                        const std::string &decodeBaseKey, KeyMapType &keyMapData,
                                        std::string &outputFirstKeyError, Poco::Any &errorValue)
    {

        mdb_apl_log_start();
        auto error = json_error_t { 0, 0, 0, { '\0' }, { '\0' } }; /* pgr0745 */
        // Vector variable the JSON analysis template
        auto decodeTemplete = std::vector<MDBContentDecodingFormat> { }; /* pgr0745 */

        for (auto cnt = std::size_t { 0 }; cnt < jsonDecodeTemplete.size(); ++cnt) /* pgr0745 */
        {
            decodeTemplete.push_back(jsonDecodeTemplete[cnt]);
            auto type = jsonDecodeTemplete[cnt].getType(); /* pgr0745 */
            auto data = jsonDecodeTemplete[cnt].getDefaultValue(); /* pgr0745 */

            // validate the default value
            if (true != isValidAnyDataType(type, data))
            {
                mdb_sys_log_out("F000201");
                auto debugKey = jsonDecodeTemplete[cnt].getKey().c_str(); /* pgr0745 */
                mdb_apl_log_error(
                    "Input Decode Template Format error. Default value type is illegal. key= %s",
                    debugKey);
                mdb_apl_log_end();
                return Json::NG;
            }
        }

        // Set the JSON analysis template in the member variable
        mDecodingTemplate = decodeTemplete;

        // Load JSON object from JSON string
        auto jsonString = inputJsonString.c_str(); /* pgr0745 */
        auto jsonSize = inputJsonString.size(); /* pgr0745 */
        mdb_apl_log_info("The input JSON string : %s", jsonString);
        auto inputJsonObject = json_loadb(jsonString, jsonSize, /* pgr0745 */
                                          static_cast<std::size_t>(MDBMFlag::DECODE_FLGS), &error);

        if (nullptr == inputJsonObject)
        {
            mdb_sys_log_out("F000201");
            mdb_apl_log_error(
                "Load JSON object from JSON string by json_loadb function occurs error."
                " Text information: %s, Position: %d", error.text, error.position);
            outputFirstKeyError = getJSonKeyError(jsonString, error.position);
            errorValue = Poco::Any { };
            mdb_apl_log_end();
            return Json::NG;
        }

        // Set the JsonObject to start the validate JSON
        auto decodeJsonObject = inputJsonObject; /* pgr0745 */

        if (true != decodeBaseKey.empty())
        {
            // If the analysis start key has been set, get the object of the key

            auto baseKeyList = convertStringToList(decodeBaseKey, "/"); /* pgr0745 */

            auto decodeKeyNumber = baseKeyList.size(); /* pgr0745 */

            for (auto arrayIndex = std::size_t { 0 }; arrayIndex < decodeKeyNumber; ++arrayIndex) /* pgr0745 */
            {
                // Get JSON object
                decodeJsonObject = json_object_get(decodeJsonObject,
                                                   baseKeyList[arrayIndex].c_str());

                if (nullptr == decodeJsonObject)
                {
                    auto debugBaseKey = baseKeyList[arrayIndex].c_str(); /* pgr0745 */
                    mdb_sys_log_out("F000201");
                    mdb_apl_log_error(
                        "json_object_get occurs error. decodeBaseKey size= %u, key= %s",
                        decodeKeyNumber, debugBaseKey);
                    // Free memory
                    json_decref(inputJsonObject);
                    mdb_apl_log_end();
                    return Json::NG;
                }
            }
        }

        //Create the JSON key map data
        auto outputMapData = KeyMapType { }; /* pgr0745 */
        // Clear the key error
        outputFirstKeyError.clear();
        // (depth: 0, index: 0, parent key: None.)
        auto retCode = toJsonKeyMapData(0, 0, "", decodeJsonObject, outputMapData,
                                        outputFirstKeyError, errorValue); /* pgr0745 */
        // Free memory
        json_decref(inputJsonObject);

        if (Json::OK != retCode)
        {
            // Set the output data
            keyMapData = outputMapData;
            mdb_sys_log_out("F000201");
            mdb_apl_log_error(
                "Create the JSON key map data by toJsonKeyMapData function occurs error.");
            mdb_apl_log_end();
            return retCode;
        }

        // Set the output data
        keyMapData = outputMapData;
        mdb_apl_log_info("Decode JSON Data Complete!");
        mdb_apl_log_end();
        return Json::OK;
    }

    int MDBJsonParser::decodeKeyMapData(const std::string &inputJsonString,
                                        const std::vector<MDBContentDecodingFormat> &jsonDecodeTemplete,
                                        const std::string &decodeBaseKey, KeyMapType &keyMapData,
                                        std::string &outputFirstKeyError)
    {
        auto errorValue = Poco::Any { }; /* pgr0745 */
        return decodeKeyMapData(inputJsonString, jsonDecodeTemplete, decodeBaseKey, keyMapData,
                                outputFirstKeyError, errorValue);
    }

    bool MDBJsonParser::isMatchedCoordPattern(const std::string &pattern, const std::string &checkedCoord)
    {
        mdb_apl_log_start();
        std::string coordStr;

        for (auto charac : checkedCoord)/* pgr0745 */
        {
            if (('[' != charac) && (']' != charac))
            {
                coordStr += charac;
            }
        }

        auto endIt = coordStr.end();/* pgr0745 */
        coordStr.erase(remove_if(coordStr.begin(), endIt, isspace), endIt);
        Poco::StringTokenizer coordTokens(coordStr, ",", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);

        if (0 != (coordTokens.count() % 2))
        {
            mdb_apl_log_critical("coord string invalid lat lon pair");
            return false;
        }

        /* combined pattern of lonlat /^[-]?\d{1,3}(\.\d{1,6})?$//^[-]?\d{1,2}(\.\d{1,6})?$/ */
        Poco::StringTokenizer patternTokens(pattern, "//", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
        size_t latLonSize = patternTokens.count();

        if (0 != (latLonSize % 2))
        {
            mdb_apl_log_critical("invalid pattern combined");
            return false;
        }

        std::string longPattern = patternTokens[0];
        std::string latPattern = patternTokens[1];

        for (size_t i = 0; i < latLonSize; i = i + 2)
        {
            if (false == isMatchedPattern(longPattern, coordTokens[i]))
            {
                return false;
            }

            if (false == isMatchedPattern(latPattern, coordTokens[i + 1]))
            {
                return false;
            }
        }

        mdb_apl_log_end();
        return true;
    }

    bool MDBJsonParser::isMatchedPattern(const std::string &pattern, const std::string &checkedString)
    {
        mdb_apl_log_start();
        mdb_apl_log_debug("pattern=%s, value=%s", pattern.c_str(), checkedString.c_str());

        if (0 == pattern.size())
        {
            return true;
        }

        //build regex and check
        try
        {
            Poco::RegularExpression regex{pattern};
            auto ret = regex.match(checkedString); /* pgr0745 */

            if (true == ret)
            {
                mdb_apl_log_debug("string is match pattern");
                mdb_apl_log_end();
                return true;
            }
            else
            {
                mdb_apl_log_debug("string is NOT match pattern");
                mdb_apl_log_end();
                return false;
            }
        }
        catch (...)
        {
            mdb_apl_log_critical("pattern is invalid, please recheck yaml file");
            mdb_apl_log_end();
            return false;
        }

    }

    bool MDBJsonParser::isMatchedDateTimeFormat(const std::string &format, const std::string &checkedString)
    {
        mdb_apl_log_start();
        mdb_apl_log_debug("checking format: format=%s, value=%s", format.c_str(), checkedString.c_str());

        if (0 == format.size())
        {
            return true;
        }

        int tzd;
        Poco::DateTime dt;
        auto canParse = Poco::DateTimeParser::tryParse(format, checkedString, dt, tzd); /* pgr0745 */
        auto timeZone = tzd / 60 / 60; //convert different time zone to hours  /* pgr0745 */

        if (false == canParse || VALID_TIME_ZONE != timeZone)
        {
            mdb_apl_log_end();
            return false;
        }

        mdb_apl_log_end();
        return true;
    }
}

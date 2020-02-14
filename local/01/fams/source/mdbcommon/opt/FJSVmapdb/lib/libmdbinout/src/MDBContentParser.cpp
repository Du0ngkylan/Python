/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBContentParser.cpp
 * @brief       The implement of MDBContentParser class.
 * @author      trungnv: Initialize MDBContentParser class.
 * @date        2016/9/23
 */

#include <limits.h>
#include <cstdint>

#include <Poco/StringTokenizer.h>
#include <Poco/Any.h>

#include "MDBContentParser.h"

namespace mdbcom {

    /*!
     * @brief constructor of MDBContentParser.
     * @param None.
     * @return None.
     */
    MDBContentParser::MDBContentParser(void) : mSerialKey {0}
    {

    }

    /*!
     * @brief Set value of the JSON Map Data.
     *
     * This method is used to set the value of the JSON Map Data with the specifying key.
     * Defines the type of available templates are :
     * Integer(32bit)
     * Integer(64bit)
     * Boolean
     * Real
     * Object
     * @param [in,out] setKeyMapData the JSON Map data is used to set value
     * @param [in] key the specifying key
     * @param [in] setData the  value data is used to set into the JSON map data
     * @return None.
     */
    template<typename T_ENCODE_VALUE>
    void MDBContentParser::setKeyMapDataValue(KeyMapType &setKeyMapData, /* pgr0304 */
            const std::string &key, const T_ENCODE_VALUE &setData) /*pgr0304*/
    {
        mSerialKey++;
        char buff[MAX_NUM_DIGIT_KEYS + 1] = {0}; /*pgr2031*/
        //std::string format = "%0" + std::to_string(MAX_NUM_DIGIT_KEYS) + "d"; //using literal string to avoid warning
        snprintf(buff, MAX_NUM_DIGIT_KEYS + 1, "%03d", mSerialKey); /*pgr1170*/ /*pgr0579*//*pgr0519*/
        std::string serialKey;
        serialKey += buff;
        serialKey += key;
        setKeyMapData[serialKey] = static_cast<T_ENCODE_VALUE>(setData);
    }
    // Defines the type of templates available
    // string
    template void MDBContentParser::setKeyMapDataValue(KeyMapType &setKeyMapData,
            const std::string &key, const std::string &setData);
    // integer(32bit)
    template void MDBContentParser::setKeyMapDataValue(KeyMapType &setKeyMapData,
            const std::string &key, const int &setData);
    // integer(64bit)
    template void MDBContentParser::setKeyMapDataValue(KeyMapType &setKeyMapData,
            const std::string &key, const long long &setData);
    // boolean
    template void MDBContentParser::setKeyMapDataValue(KeyMapType &setKeyMapData,
            const std::string &key, const bool &setData);
    // real
    template void MDBContentParser::setKeyMapDataValue(KeyMapType &setKeyMapData,
            const std::string &key, const double &setData);
    // object
    template void MDBContentParser::setKeyMapDataValue(KeyMapType &setKeyMapData,
            const std::string &key, const KeyMapType &setData);

    /*!
     * @brief Set array value into the JSON Map Data.
     *
     * This method is used to set array value into the JSON Map data with the specifying key.
     * Defines the type of array available templates are :
     * Array(string)
     * Array(integer 32bit)
     * Array(integer 64bit)
     * Array(boolean)
     * Array(real)
     * array(object)
     * @param [in,out] setKeyMapData the JSON Map data is used to set value
     * @param [in] key the specifying key
     * @param [in] setDataList the value array data is used to set into the JSON map data
     * @return None.
     */
    template<typename T_ENCODE_VALUE>
    void MDBContentParser::setKeyMapDataArrayValue(KeyMapType &setKeyMapData, /* pgr0304 */
            const std::string &key, const std::vector<T_ENCODE_VALUE> &setDataList)
    {
        mSerialKey++;
        char buff[MAX_NUM_DIGIT_KEYS + 1] = {0}; /*pgr2031*/
        //std::string format = "%0" + std::to_string(MAX_NUM_DIGIT_KEYS) + "d"; //using literal string to avoid warning
        snprintf(buff, MAX_NUM_DIGIT_KEYS + 1, "%03d", mSerialKey);/*pgr1170*/ /*pgr0579*//*pgr0519*/
        std::string serialKey;
        serialKey += buff;
        serialKey += key;
        auto anyList = std::vector<Poco::Any> { }; /* pgr0745 */

        for (auto arrayIndex = std::size_t { 0 }; arrayIndex < setDataList.size(); ++arrayIndex) /* pgr0745 */ /*pgr0692*/
        {
            anyList.push_back(setDataList[arrayIndex]);
        }

        setKeyMapData[serialKey] = anyList;
    }
    // Defines the type of templates available
    // array(string)
    template void MDBContentParser::setKeyMapDataArrayValue(KeyMapType &setKeyMapData,
            const std::string &key, const std::vector<std::string> &setDataList);
    // array(integer 32bit)
    template void MDBContentParser::setKeyMapDataArrayValue(KeyMapType &setKeyMapData,
            const std::string &key, const std::vector<int> &setDataList);
    // array(integer 64bit)
    template void MDBContentParser::setKeyMapDataArrayValue(KeyMapType &setKeyMapData,
            const std::string &key, const std::vector<long long> &setDataList);
    // array(boolean)
    template void MDBContentParser::setKeyMapDataArrayValue(KeyMapType &setKeyMapData,
            const std::string &key, const std::vector<bool> &setDataList);
    // array(real)
    template void MDBContentParser::setKeyMapDataArrayValue(KeyMapType &setKeyMapData,
            const std::string &key, const std::vector<double> &setDataList);
    // array(object)
    template void MDBContentParser::setKeyMapDataArrayValue(KeyMapType &setKeyMapData,
            const std::string &key, const std::vector<KeyMapType> &setDataList);

    /*!
     * @brief Get value from the JSON Map Data.
     *
     * This method is used to get the value from the JSON Map Data with the specifying key.
     * Defines the type of available templates are :
     * Integer(32bit)
     * Integer(64bit)
     * Boolean
     * Real
     * Object
     * @param [in] decodeKeyMapData the JSON Map data is used to get value
     * @param [in] key the specifying key
     * @param [out] errFlag the flag error
     * @return  The value data type from the JSON map data
     */
    template<typename T_DECODE_VALUE>
    T_DECODE_VALUE MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData, /* pgr0304 */
            const std::string &key, bool &errFlag)
    {

        auto internalErrFlag = errFlag; /* pgr0745 */
        auto returnValue = this->getKeyMapDataValue<T_DECODE_VALUE>(decodeKeyMapData, /* pgr0745 */
                           key, "", internalErrFlag);
        errFlag = internalErrFlag;

        return returnValue;
    }
    // Defines the type of templates available
    // string
    template std::string MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, bool &errFlag);
    // integer(32bit)
    template int MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, bool &errFlag);
    // integer(64bit)
    template long long MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, bool &errFlag);
    // boolean
    template bool MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, bool &errFlag);
    // real
    template double MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, bool &errFlag);
    // object
    template MDBContentParser::KeyMapType MDBContentParser::getKeyMapDataValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag);

    /*!
     * @brief Get value from the JSON Map Data.
     *
     * This method is used to get the value from the JSON Map Data with the specifying key and specified parent key.
     * Defines the type of available templates are :
     * Integer(32bit)
     * Integer(64bit)
     * Boolean
     * Real
     * Object
     * @param [in] decodeKeyMapData the JSON Map data is used to get value
     * @param [in] key the specifying key
     * @param [in] parentKey the specified parent key
     * @param [out] errFlag the flag error
     * @return  The value data type from the JSON map data
     */
    template<typename T_DECODE_VALUE>
    T_DECODE_VALUE MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData, /* pgr0304 */
            const std::string &key, const std::string &parentKey, bool &errFlag)
    {

        mdb_apl_log_start();
        auto anyValue = Poco::Any { }; /* pgr0745 */

        // The specified search key
        auto retCode = findJsonMapDataKey(key, parentKey, decodeKeyMapData, anyValue); /* pgr0745 */

        if (Json::OK != retCode)
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_info("Not found the input key in the JSON map data. (Key: %s)", debugKey);
            errFlag = true;
            mdb_apl_log_end();
            return T_DECODE_VALUE();
        }

        // Validate type
        if (anyValue.type() != typeid(T_DECODE_VALUE))
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            auto debugType = anyValue.type().name(); /* pgr0745 */
            mdb_apl_log_critical("GetValue Error. Key type is invalid. (Key: %s, Type: %s)",
                                 debugKey, debugType);
            errFlag = true;
            mdb_apl_log_end();
            return T_DECODE_VALUE();
        }

        errFlag = false;
        mdb_apl_log_end();
        // Return output value
        return Poco::AnyCast<T_DECODE_VALUE>(anyValue);
    }
    // Defines the type of templates available
    // string
    template std::string MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, const std::string &parentKey, bool &errFlag);
    // integer(32bit)
    template int MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, const std::string &parentKey, bool &errFlag);
    // integer(64bit)
    template long long MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, const std::string &parentKey, bool &errFlag);
    // boolean
    template bool MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, const std::string &parentKey, bool &errFlag);
    // real
    template double MDBContentParser::getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
            const std::string &key, const std::string &parentKey, bool &errFlag);
    // object
    template MDBContentParser::KeyMapType MDBContentParser::getKeyMapDataValue(
        const KeyMapType &decodeKeyMapData, const std::string &key,
        const std::string &parentKey, bool &errFlag);

    /*!
     * @brief Get array value from the JSON Map Data.
     *
     * This method is used to get array value from the JSON Map data with the specifying key.
     * Defines the type of array available templates are :
     * Array(string)
     * Array(integer 32bit)
     * Array(integer 64bit)
     * Array(boolean)
     * Array(real)
     * array(object)
     * @param [in] decodeKeyMapData the JSON Map data is used to get the value data
     * @param [in] key the specifying key
     * @param [out] errFlag the flag error
     * @return  The value array data from the JSON map data
     */
    template<typename T_DECODE_VALUE>
    std::vector<T_DECODE_VALUE> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag) /* pgr0304 */
    {

        auto internalErrFlag = errFlag; /* pgr0745 */
        auto returnValue = this->getKeyMapDataArrayValue<T_DECODE_VALUE>( /* pgr0745 */
                               decodeKeyMapData, key, "", internalErrFlag);
        errFlag = internalErrFlag;
        return returnValue;
    }
    // Defines the type of templates available
    // array(string)
    template std::vector<std::string> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag);
    // array(integer 64bit)
    template std::vector<long long> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag);
    // array(integer 32bit)
    template std::vector<int> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag);
    // array(boolean)
    template std::vector<bool> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag);
    // array(real)
    template std::vector<double> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag);
    // array(object)
    template std::vector<MDBContentParser::KeyMapType> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key, bool &errFlag);

    /*!
     * @brief Get array value from the JSON Map Data
     *
     * This method is used to get array value from the JSON Map data with the specifying key and specified parent key.
     * Defines the type of array available templates are :
     * Array(string)
     * Array(integer 32bit)
     * Array(integer 64bit)
     * Array(boolean)
     * Array(real)
     * array(object)
     * @param [in] decodeKeyMapData the JSON Map data is used to get the value data
     * @param [in] key the specifying key
     * @param [in] parentKey the specified parent key
     * @param [out] errFlag the flag error
     * @return  The value array data from the JSON map data.
     */
    template<typename T_DECODE_VALUE>
    std::vector<T_DECODE_VALUE> MDBContentParser::getKeyMapDataArrayValue(
        const MDBContentParser::KeyMapType &decodeKeyMapData, const std::string &key, /* pgr0304 */
        const std::string &parentKey, bool &errFlag)
    {

        mdb_apl_log_start();
        auto anyValue = Poco::Any { }; /* pgr0745 */
        auto outVector = std::vector<T_DECODE_VALUE> { }; /* pgr0745 */

        // Search the specifying key
        auto retCode = findJsonMapDataKey(key, parentKey, decodeKeyMapData, anyValue); /* pgr0745 */

        if (Json::OK != retCode)
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_warn(
                "GetArrayValue Error. Can not found the input key in the JSON map data. (Key: %s)",
                debugKey);
            errFlag = true;
            mdb_apl_log_end();
            return outVector;
        }

        // Validate type
        if (typeid(std::vector<Poco::Any>) != anyValue.type())
        {
            auto debugKey = key.c_str(); /* pgr0745 */
            auto debugType = anyValue.type().name(); /* pgr0745 */
            mdb_apl_log_critical("GetArrayValue Error. Key type is invalid. (Key: %s, Type: %s)",
                                 debugKey, debugType);
            errFlag = true;
            mdb_apl_log_end();
            return outVector;
        }

        auto anyVector = Poco::AnyCast<std::vector<Poco::Any> >(anyValue); /* pgr0745 */

        for (auto arrayIndex = std::size_t { 0 }; arrayIndex < anyVector.size(); ++arrayIndex) /* pgr0745 */
        {

            // Validate type
            if (typeid(T_DECODE_VALUE) != anyVector[arrayIndex].type())
            {
                auto debugKey = key.c_str(); /* pgr0745 */
                auto debugType = anyValue.type().name(); /* pgr0745 */
                mdb_apl_log_critical(
                    "GetArrayValue Error. Key type is invalid. (Key: %s, Type: %s)", debugKey,
                    debugType);
                errFlag = true;
                mdb_apl_log_end();
                return outVector;
            }

            auto elementValue = Poco::AnyCast<T_DECODE_VALUE>(anyVector[arrayIndex]); /* pgr0745 */
            outVector.push_back(elementValue);
        }

        errFlag = false;
        mdb_apl_log_end();
        // Output value
        return outVector;
    }
    // Defines the type of templates available
    // array(string)
    template std::vector<std::string> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key,
        const std::string &parentKey, bool &errFlag);
    // array(integer 64bit)
    template std::vector<long long> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key,
        const std::string &parentKey, bool &errFlag);
    // array(integer 32bit)
    template std::vector<int> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key,
        const std::string &parentKey, bool &errFlag);
    // array(boolean)
    template std::vector<bool> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key,
        const std::string &parentKey, bool &errFlag);
    // array(real)
    template std::vector<double> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key,
        const std::string &parentKey, bool &errFlag);
    // array(object)
    template std::vector<MDBContentParser::KeyMapType> MDBContentParser::getKeyMapDataArrayValue(
        const KeyMapType &decodeKeyMapData, const std::string &key,
        const std::string &parentKey, bool &errFlag);

    /*!
     * @brief Search the specifying key and output the corresponding value base on the specified parent key.
     *
     * This method is used to Search the specifying key and output the corresponding value base on
     * the specified parent key.
     * @param [in] key  the specifying key
     * @param [in] parentKey  the specified parent key
     * @param [in] decodeKeyMapData the the JSON Map data
     * @param [out] outValue the output value
     * @retval  Json::OK if key exists
     * @retval  Json::NG if key does not exists
     */
    int MDBContentParser::findJsonMapDataKey(const std::string &key, const std::string &parentKey,
            const std::map<std::string, Poco::Any> &decodeKeyMapData, Poco::Any &outValue)
    {

        mdb_apl_log_start();
        auto mapItr = decodeKeyMapData.begin(); /* pgr0745 */
        auto findKeyMapData = &decodeKeyMapData; /* pgr0745 */

        // Get the parent key list
        auto parentKeyList = convertStringToList(parentKey, "/"); /* pgr0745 */
        auto parentKeyNumber = parentKeyList.size(); /* pgr0745 */
        mdb_apl_log_debug("parentKey=%s", parentKey.c_str());

        for (auto depth = 0u; depth < parentKeyNumber; ++depth) /* pgr0745 */
        {
            auto isArray = false;
            auto parrentIndex = -1;
            auto strParrentIndex = parentKeyList[depth].find('[');
            mdb_apl_log_debug("parentKeyList[%d] = %s", depth, parentKeyList[depth].c_str());

            if (std::string::npos != strParrentIndex)
            {
                isArray = true;

                if (parentKeyList[depth][strParrentIndex + 1] != ']')
                {
                    parrentIndex = std::atoi(parentKeyList[depth].c_str() + strParrentIndex + 1);
                }

                parentKeyList[depth].erase(strParrentIndex, parentKeyList[depth].size() - strParrentIndex);
            }

            mapItr = findKeyMapData->find(parentKeyList[depth]);

            if (findKeyMapData->end() == mapItr)
            {
                // Key does not exist
                auto debugParentKeyList = parentKeyList[depth].c_str(); /* pgr0745 */
                auto debugParentKey = parentKey.c_str(); /* pgr0745 */
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_debug("Not found: parent key= %s. parent= %s key= %s",
                                  debugParentKeyList, debugParentKey, debugKey);
                mdb_apl_log_end();
                return Json::NG;
            }

            // Validate type
            auto keyAnyData = &mapItr->second; /* pgr0745 */

            if (isArray == true)
            {
                if (typeid(std::vector<Poco::Any>) != keyAnyData->type())
                {
                    mdb_apl_log_critical("Parrent is not array type parent= %s key= %s",
                                         parentKeyList[depth].c_str(), key.c_str());
                    mdb_apl_log_end();
                    return Json::NG;
                }

                auto pParrentArray = Poco::AnyCast<std::vector<Poco::Any> >(keyAnyData);

                if (0 <= parrentIndex)
                {
                    keyAnyData = &((*pParrentArray)[parrentIndex]);
                }
                else if (depth == (parentKeyNumber - 1))
                {
                    // last key
                    outValue = std::vector<Poco::Any> {};
                    auto &refOutValue = Poco::RefAnyCast<std::vector<Poco::Any> >(outValue);
                    refOutValue.reserve(pParrentArray->size());

                    for (auto &element : *pParrentArray)
                    {
                        auto outValueElement = Poco::Any {};
                        findJsonMapDataKey(key, "", Poco::RefAnyCast<std::map<std::string, Poco::Any>>(element), outValueElement);
                        refOutValue.push_back(outValueElement);
                    }

                    mdb_apl_log_end();
                    return Json::OK;
                }
                else
                {
                    mdb_apl_log_critical("Parrent index is not specified parent= %s key= %s",
                                         parentKeyList[depth].c_str(), key.c_str());
                    mdb_apl_log_end();
                    return Json::NG;
                }
            }

            if (typeid(std::map<std::string, Poco::Any>) != keyAnyData->type())
            {
                auto debugParentKeyList = parentKeyList[depth].c_str(); /* pgr0745 */
                auto debugParentKey = parentKey.c_str(); /* pgr0745 */
                auto debugKey = key.c_str(); /* pgr0745 */
                mdb_apl_log_critical("parent key is Not object.key= %s. parent= %s key= %s",
                                     debugParentKeyList, debugParentKey, debugKey);
                mdb_apl_log_end();
                return Json::NG;
            }

            findKeyMapData = Poco::AnyCast<std::map<std::string, Poco::Any>>(keyAnyData);
        }

        // Search key
        mapItr = findKeyMapData->find(key);

        if (findKeyMapData->end() == mapItr)
        {
            // Key does not exist
            auto debugParentKey = parentKey.c_str(); /* pgr0745 */
            auto debugKey = key.c_str(); /* pgr0745 */
            mdb_apl_log_warn("Not found key= %s, parent= %s", debugKey, debugParentKey);
            mdb_apl_log_end();
            return Json::NG;
        }

        outValue = mapItr->second;
        mdb_apl_log_end();
        return Json::OK;
    }

    /*!
     * @brief Check whether the data type of the specified Any data and the Value type is correct.
     *
     * This method is used to check whether the data type of the specified Any data and the Value type is correct.
     * @param [in] type  the decoding value type
     * @param [in] data the any data.
     * @retval  true if normal
     * @retval  false if abnormal
     */
    bool MDBContentParser::isValidAnyDataType(MDBInputKind type, const Poco::Any data)
    {

        if (MDBInputKind::STRING == type)
        {
            return (data.type() == typeid(std::string));

        }
        else if (MDBInputKind::INTEGER64 == type)
        {
            return (data.type() == typeid(long long));

        }
        else if (MDBInputKind::INTEGER32 == type)
        {
            return (data.type() == typeid(int));

        }
        else if (MDBInputKind::BOOL == type)
        {
            return (data.type() == typeid(bool));

        }
        else if (MDBInputKind::REAL == type)
        {
            return (data.type() == typeid(double));

        }
        else
        {
            // Other case
        }

        return true;
    }

    /*!
     * @brief  Create a full path key bound in with a specified parent Key self-Key.
     *
     * This method is used to create a full path key bound in with a specified parent Key self-Key.
     * @param [in] parentKey  the specified parent key
     * @param [in] ownKey the specifying key.
     * @return The full path key string
     */
    std::string MDBContentParser::createFullPathKey(const std::string &parentKey,
            const std::string &ownKey)
    {
        mdb_apl_log_start();
        // Create full key path of own key
        auto myFullPathKey = std::string { }; /* pgr0745 */

        if (true != parentKey.empty())
        {
            myFullPathKey = parentKey + ".";
        }

        myFullPathKey += ownKey;
        mdb_apl_log_end();
        return myFullPathKey;
    }

    /*!
     * @brief Convert string to list key.
     *
     * This method is used to Convert string to list key.
     * @param [in] str  the input string data
     * @param [in] delimiter the signal delimiter
     * @return  the list key
     */
    std::vector<std::string> MDBContentParser::convertStringToList(const std::string &str,
            const std::string &delimiter)
    {
        mdb_apl_log_start();
        auto source = std::string { }; /* pgr0745 */
        auto vecResult = std::vector<std::string> { }; /* pgr0745 */

        // In case str is empty
        if ((true == delimiter.empty()) || (true == str.empty()))
        {
            mdb_apl_log_end();
            return vecResult;
        }

        // In case str has one key
        if (str.find(delimiter) == std::string::npos)
        {
            vecResult.push_back(str);
            mdb_apl_log_end();
            return vecResult;
        }

        source = std::string(str);

        // Split string
        auto ret = std::vector<std::string> { }; /* pgr0745 */
        Poco::StringTokenizer result(source, delimiter);

        for (auto i = std::size_t { 0 }; i < result.count(); i++) /* pgr0745 */
        {
            ret.push_back(result[i]);
        }

        mdb_apl_log_end();
        return ret;
    }

    /*!
     * @brief Validate the OmitFlag whether the specifying keys is allowed to omit.
     *
     * This method is used to Validate the OmitFlag whether the specifying keys is allowed to omit.
     * @param [in] decodeTemplete the validating template
     * @param [in] myParentKey the parent key
     * @param [in] key the specifying key
     * @retval  true if don't have any problem
     * @retval  false if have an problem
     */
    int MDBContentParser::validateOmitFlag(const MDBContentDecodingFormat &decodeTemplete,
                                           const std::string &myParentKey, const std::string &key) /* pgr0304 */
    {

        mdb_apl_log_start();
        // Validate the flag omit
        auto debugParentKey = myParentKey.c_str(); /* pgr0745 */
        auto debugKey = key.c_str(); /* pgr0745 */

        if (true == decodeTemplete.getFlagValidOmit())
        {

            mdb_apl_log_debug("Object key is not found, so skip. parentKey= %s, key = %s",
                              debugParentKey, debugKey);
            mdb_apl_log_end();
            return Json::OK;
        }

        mdb_apl_log_error("Object key is not found. Omit is not allow. parentKey= %s, key = %s",
                          debugParentKey, debugKey);
        mdb_apl_log_end();
        return Json::NG;
    }

    /*!
     * @brief Validate the NullFlag whether the specifying keys is allowed null value
     *
     * This method is used to Validate the NullFlag whether the specifying keys is allowed null value.
     * @param [in] decodeTemplete the validating template
     * @param [in] myParentKey the parent key
     * @param [in] key the specifying key
     * @param [out] outValue the output the JSON key map data
     * @retval  true if don't have any problem
     * @retval  false if have an problem
     */
    int MDBContentParser::validateValidNullFlag(const MDBContentDecodingFormat &decodeTemplete,
            const std::string &myParentKey, const std::string &key, KeyMapType &outJsonMapData) /* pgr0304 */
    {

        mdb_apl_log_start();
        auto type = decodeTemplete.getType(); /* pgr0745 */
        // Validate the flag valid null
        auto debugParentKey = myParentKey.c_str(); /* pgr0745 */
        auto debugKey = key.c_str(); /* pgr0745 */

        if (true == decodeTemplete.getFlagValidNull())
        {
            // Allow null
            if ((MDBInputKind::STRING == type) || (MDBInputKind::INTEGER64 == type)
                    || (MDBInputKind::INTEGER32 == type) || (MDBInputKind::BOOL == type)
                    || (MDBInputKind::REAL == type))
            {
                // Set the default value
                mdb_apl_log_debug(
                    "Object key is not found. Set default value. parentKey= %s, key = %s",
                    debugParentKey, debugKey);
                outJsonMapData[key] = decodeTemplete.getDefaultValue();
            }
            else
            {
                mdb_apl_log_debug("Object key is not found, so skip. parentKey= %s, key = %s",
                                  debugParentKey, debugKey);
            }

            mdb_apl_log_end();
            return Json::OK;
        }

        mdb_apl_log_error("Object key is null. null is not allow. parentKey= %s, key = %s",
                          debugParentKey, debugKey);
        mdb_apl_log_end();
        return Json::NG;
    }

    /*!
     * @brief Check the specified key whether it exists in JSON Map data.
     *
     * This method is used to Check the specified whether it exists in JSON Map data with the specified key.
     * @param [in] key the specified key
     * @param [in] decodeKeyMapData the JSON Map data is used to check
     * @return The validate result
     * @retval  true if the key exists
     * @retval  false if the key doesn't exist
     */
    bool MDBContentParser::isExistKey(const std::string &key,
                                      const std::map<std::string, Poco::Any> &decodeKeyMapData)
    {
        return this->isExistKey(key, "", decodeKeyMapData);
    }

    /*!
     * @brief Check the specified key with the specified parent key whether it exists in JSON Map data.
     *
     * This method is used to check the specified key with the specified parent key whether it exists <BR>
     * in JSON Map data with the specified key.
     * @param [in] key the specified key
     * @param [in] parentKey the specified parent key
     * @param [in] decodeKeyMapData the JSON Map data is used to check
     * @return The validate result
     * @retval  true if the key exists
     * @retval  false if the key doesn't exist
     */
    bool MDBContentParser::isExistKey(const std::string &key, const std::string &parentKey,
                                      const std::map<std::string, Poco::Any> &decodeKeyMapData)
    {
        mdb_apl_log_start();

        auto mapItr = decodeKeyMapData.begin(); /* pgr0745 */
        auto findKeyMapData = decodeKeyMapData; /* pgr0745 */

        // Get the parent key list
        auto parentKeyList = convertStringToList(parentKey, "/"); /* pgr0745 */
        auto parentKeyNumber = parentKeyList.size(); /* pgr0745 */

        for (auto arrayIndex = std::size_t { 0 }; arrayIndex < parentKeyNumber; ++arrayIndex) /* pgr0745 */
        {
            mapItr = findKeyMapData.find(parentKeyList[arrayIndex]);

            if (findKeyMapData.end() == mapItr)
            {
                // Key does not exist
                mdb_apl_log_end();
                return false;
            }

            // Validate type
            auto keyAnyData = mapItr->second; /* pgr0745 */

            if (typeid(std::map<std::string, Poco::Any>) != keyAnyData.type())
            {
                mdb_apl_log_end();
                return false;
            }

            findKeyMapData = Poco::AnyCast<std::map<std::string, Poco::Any> >(keyAnyData);
        }

        // Search key
        mapItr = findKeyMapData.find(key);

        if (findKeyMapData.end() == mapItr)
        {
            // Key does not exist
            mdb_apl_log_end();
            return false;
        }

        mdb_apl_log_end();
        return true;
    }
}

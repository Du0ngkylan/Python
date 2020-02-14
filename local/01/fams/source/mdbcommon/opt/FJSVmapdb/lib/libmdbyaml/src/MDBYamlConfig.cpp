/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBYamlConfig.cpp
 * @brief       The implementation of MDBYamlConfig .
 * @author      cuongadp
 * @date        2017/04/24
 */

#include <Poco/DateTimeParser.h>
#include <Poco/DateTimeFormat.h>
#include "MDBLogger.h"
#include "MDBConstants.h"
#include "MDBException.h"
#include "MDBYamlConfig.h"

namespace mdbcom {
    /*!
     * @brief Construtor
     * @param [in] path Path to configuration file
     * @return None.
     */
    MDBYamlConfig::MDBYamlConfig(const std::string &configFile) : MDBContentConfig {configFile}, propList {}
    {
        mdb_apl_log_start();
        auto configFilePath = configFile.c_str(); /* pgr0745 */
        mdb_apl_log_info("Loading yaml configuration file=: %s", configFilePath);
        auto configRoot = YAML::LoadFile(configFile); /* pgr0745 */

        if (true == configRoot.IsNull())
        {
            mdb_apl_log_critical("Cannot load configuration file: %s", configFilePath);
            throw MDBException("Invalid configuration file");
        }

        auto paths = configRoot[KEY_PATHS]; /* pgr0745 */

        if (false == paths.IsDefined())
        {
            mdb_apl_log_critical("Key [paths] is not found in file: %s", configFilePath);
            throw MDBException("Key [paths] is not found");
        }

        if (false == paths.IsMap())
        {
            mdb_apl_log_critical("Key [paths] is not object type in file: %s", configFilePath);
            throw MDBException("Key [paths] is not an object type");
        }

        auto basePath = configRoot[KEY_BASEPATHS]; /* pgr0745 */
        auto strBasePath = std::string {}; /* pgr0745 */

        if (true == basePath.IsDefined())
        {
            strBasePath = basePath.as<std::string>();
        }

        for (auto itPath = paths.begin(); itPath != paths.end(); itPath++) /* pgr0745 */
        {
            auto methodList = std::map<std::string, YAML::Node> {} ; /* pgr0745 */
            auto path = itPath->first.as<std::string>(); /* pgr0745 */
            auto &pathNode = itPath->second; /* pgr0745 */

            for (auto itMethod = pathNode.begin(); itMethod != pathNode.end(); itMethod++) /* pgr0745 */
            {
                auto params = itMethod->second[KEY_PARAMETERS]; /* pgr0745 */

                if (false == params.IsDefined())
                {
                    auto strPath = path.c_str(); /* pgr0745 */
                    mdb_apl_log_critical("Object parameters not exist in path=%s",
                                         strPath);
                    throw MDBException {"Object parameters not exist"};
                }

                auto paramBegin = params.begin(); /* pgr0745 */
                auto paramEnd = params.end(); /* pgr0745 */
                auto body = std::find_if(paramBegin, paramEnd, /* pgr0745 */
                                         [this](const YAML::Node & value)
                {
                    auto name = value[KEY_NAME]; /* pgr0745 */

                    if (false == name.IsDefined())
                    {
                        return false;
                    }

                    return (VALUE_BODY == name.as<std::string>()); /*pgr0058*/
                });

                auto method = itMethod->first.as<std::string>(); /* pgr0745 */

                if (params.end() == body)
                {
                    auto strPath = path.c_str(); /* pgr0745 */
                    auto strMethod = method.c_str(); /* pgr0745 */
                    mdb_apl_log_critical("Parameter Body not exist for path=%s method:%s",
                                         strPath, strMethod);
                    throw MDBException {"Parameter Body not exist"};
                }

                auto schema = (*body)[KEY_SCHEMA]; /* pgr0745 */

                if (false == schema.IsDefined())
                {
                    auto strPath = path.c_str(); /* pgr0745 */
                    auto strMethod = method.c_str(); /* pgr0745 */
                    mdb_apl_log_critical("Object schema not exist in Body parameter of path=%s method:%s",
                                         strPath, strMethod);
                    throw MDBException("Object schema not exist in Body parameter");
                }

                methodList.emplace(method, schema);
            }

            propList[strBasePath + path] = std::move(methodList);
        }

        mdb_apl_log_end();
    }

    /*!
     * @brief Get Node the configuration data
     * @param [in] url URL to get template.
     * @param [in] method Method of the URL
     * @return Pointer to node data
     */
    YAML::Node *MDBYamlConfig::getProperties(const std::string &url, const std::string &method)
    {
        mdb_apl_log_start();
        auto urlIt = propList.find(url); /* pgr0745 */

        if (propList.end() == urlIt)
        {
            auto strUrl = url.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Url [%s] not exist in configuration", strUrl);
            mdb_apl_log_end();
            return nullptr;
        }

        auto methodIt = urlIt->second.find(method); /* pgr0745 */

        if (urlIt->second.end() == methodIt)
        {
            auto strMethod = method.c_str(); /* pgr0745 */
            auto strUrl = url.c_str(); /* pgr0745 */
            mdb_apl_log_critical("Method [%s] not exist for Url [%s] in configuration",
                                 strMethod, strUrl);
            mdb_apl_log_end();
            return nullptr;
        }

        mdb_apl_log_end();
        return &methodIt->second;
    }

    /*!
     * @brief Parse the decoding template from configuration data
     * @param [in] url URL to get template.
     * @param [in] method Method of the URL
     * @param [out] formatTemplate The result template
     * @retval Result::OK Success.
     * @retval Result::NG Failure
     */
    int MDBYamlConfig::getFormat(const std::string &url, const std::string &method, std::vector<MDBContentDecodingFormat> &formatTemplate)
    {
        mdb_apl_log_start();
        auto properties = getProperties(url, method); /* pgr0745 */
        auto strUrl = url.c_str(); /* pgr0745 */
        mdb_apl_log_debug("URL: %s", strUrl);

        if (nullptr == properties)
        {
            mdb_apl_log_critical("Can't get property list");
            mdb_apl_log_end();
            return Result::NG;
        }

        // because this is root tag, so:
        //  depth: -1
        //  required: true
        //  isRoot: true
        if (Result::OK != parseNode(KEY_SCHEMA, -1, true, *properties, formatTemplate, true))
        {
            mdb_apl_log_end();
            return Result::NG;
        }

        mdb_apl_log_end();
        return Result::OK;
    }

    /*!
     * @brief Parse single object node
     * @param [in] key Name of current node
     * @param [in] depth Depth level of current node (root is 0)
     * @param [in] required Is the node required by parrent
     * @param [in] node Node data from yaml
     * @param [out] formatTemplate The result template
     * @retval Result::OK Success.
     * @retval Result::NG Failure
     */
    int MDBYamlConfig::parseNode(const std::string &key, int depth, bool required, const YAML::Node &node, std::vector<MDBContentDecodingFormat> &resultSet, bool root)
    {
        mdb_apl_log_start();

        auto strKey = key.c_str(); /* pgr0745 */

        if (false == node.IsMap())
        {
            auto nodeType = node.Type(); /* pgr0745 */
            mdb_apl_log_critical("Invalid object type for key [%s] [level: %d] [type:%d]", strKey, depth, nodeType);
            mdb_apl_log_end();
            return Result::NG;
        }

        mdb_apl_log_trace("Parsing [key:%s] [depth:%d] [required:%d] ", strKey, depth, required);
        auto templateKey = key; /* pgr0745 */
        auto templateDepth = depth; /* pgr0745 */
        auto templateType = MDBInputKind::OBJECT; /* pgr0745 */
        auto templateFlagValueCheck = false; /* pgr0745 */
        auto templateCheckLower = 0.0; /* pgr0745 */ /*pgr0729*/
        auto templateCheckHigher = 0.0; /* pgr0745 */ /*pgr0729*/
        auto templateFlagValidNull = false; /* pgr0745 */
        auto templateFlagValidEmpty = false; /* pgr0745 */
        auto templateFlagValidOmit = !required; /* pgr0745 */
        auto templateFlagErrorContinue = false; /* pgr0745 */
        auto templateDefaultValue = Poco::Any {} ; /* pgr0745 */
        std::vector<int> enumList;
        auto pattern = std::string {}; /* pgr0745 */
        auto dateFormat = std::string {}; /* pgr0745 */

        if (Result::OK != getType(node, templateType))
        {
            if ("coordinates" == key)
            {
                mdb_apl_log_debug("this is [coordinates] node");
                templateType = MDBInputKind::COORDINATES;
            }
            else
            {
                mdb_apl_log_critical("Can't get type of [%s] [level: %d]", strKey, depth);
                mdb_apl_log_end();
                return Result::NG;
            }
        }

        // If both maximum & minimum key is defined, set the range. otherwise, skip value check
        templateFlagValueCheck = getRange(node, templateType, templateCheckLower, templateCheckHigher);
        // If key x-nullable is defined, set value, else set false
        auto validNull = node[KEY_NULLABLE]; /* pgr0745 */

        if (true == validNull.IsDefined())
        {
            templateFlagValidNull = validNull.as<bool>();
        }

        // If minLength is 0, allow empty.
        auto validEmpty = node[KEY_MINLENGTH]; /* pgr0745 */

        if (true == validEmpty.IsDefined())
        {
            templateFlagValidEmpty = (validEmpty.as<int>() == 0);
        }

        auto errorContinue = node[KEY_ERRORCONTINUE]; /* pgr0745 */

        if (true == errorContinue.IsDefined())
        {
            templateFlagErrorContinue = errorContinue.as<bool>();
        }

        if (Result::OK != getDefaultValue(node, templateType, templateDefaultValue))
        {
            mdb_apl_log_debug("Can't get default value of [%s] [level: %d]", strKey, depth);
        }

        if (Result::OK != getEnumList(node, templateType, enumList))
        {
            mdb_apl_log_debug("Can't get enum value of [%s] [level: %d]", strKey, depth);
        }

        if (Result::OK != getPattern(node, templateType, pattern))
        {
            mdb_apl_log_debug("Can't get pattern value of [%s] [level: %d]", strKey, depth);
        }

        if (MDBInputKind::STRING == templateType)
        {
            // if there is no 'format' field, not an error (accept any string)
            // if 'format' have an empty value, not an error (accept any string)
            // if 'format' have a not empty value, but cannot convert to poco format, it's an error
            if (Result::OK != getDateTimeFormat(node, templateType, dateFormat))
            {
                mdb_apl_log_critical("Can't get date format value of [%s] [level: %d]", strKey, depth);
                mdb_apl_log_end();
                return Result::NG;
            }
        }

        if (!root)
        {
            auto strPattern = pattern.c_str();/* pgr0745 */
            auto strDateFormat = dateFormat.c_str();/* pgr0745 */
            auto enumListSize = enumList.size();/* pgr0745 */
            mdb_apl_log_info("Added new format entry [key:%s][depth:%d][type:%d]"
                             "[flagValueCheck: %d][checkLower:%.6f][checkHigher:%.6f]"
                             "[flagValidNull:%d][flagValidEmpty:%d][flagValidOmit:%d]"
                             "[flagErrorContinue:%d][enum size:%d][pattern:%s][dateFormat:%s]", strKey, templateDepth, static_cast<int>(templateType),
                             templateFlagValueCheck, templateCheckLower, templateCheckHigher,
                             templateFlagValidNull, templateFlagValidEmpty, templateFlagValidOmit,
                             templateFlagErrorContinue, enumListSize, strPattern, strDateFormat);
            resultSet.emplace_back(strKey, templateDepth, templateType,
                                   templateFlagValueCheck, templateCheckLower, templateCheckHigher,
                                   templateFlagValidNull, templateFlagValidEmpty, templateFlagValidOmit,
                                   templateFlagErrorContinue, templateDefaultValue, enumList, pattern, dateFormat);
        }

        auto childNode = YAML::Node {}; /* pgr0745 */
        auto parrentNode = node; /* pgr0745 */

        if (MDBInputKind::ARRAY_OBJECT == templateType)
        {
            // set for recursion
            parrentNode = node[KEY_ITEMS];

            if (false == parrentNode.IsDefined())
            {
                mdb_apl_log_critical("Key [items] not exist for array [%s] [level: %d]", strKey, depth);
                mdb_apl_log_end();
                return Result::NG;
            }

            childNode = parrentNode[KEY_PROPERTIES];

            if (false == childNode.IsDefined())
            {
                mdb_apl_log_critical("Key [properties] not exist for array [%s] [level: %d]", strKey, depth);
                mdb_apl_log_end();
                return Result::NG;
            }

            if (false == childNode.IsMap())
            {
                mdb_apl_log_critical("Key [properties] not a valid map for array [%s] [level: %d]", strKey, depth);
                mdb_apl_log_end();
                return Result::NG;
            }
        }

        if (MDBInputKind::OBJECT == templateType)
        {
            // set for recursion
            childNode = node[KEY_PROPERTIES];

            if (false == childNode.IsDefined())
            {
                mdb_apl_log_critical("Key [properties] not exist for object [%s] [level: %d]", strKey, depth);
                mdb_apl_log_end();
                return Result::NG;
            }
        }

        if (false == childNode.IsNull())
        {
            // Get required list from parrent node
            auto require = parrentNode[KEY_REQUIRED]; /* pgr0745 */

            if (true == require.IsDefined())
            {
                if (false == require.IsSequence())
                {
                    mdb_apl_log_critical("Invalid setting type for [required]");
                    mdb_apl_log_end();
                    return Result::NG;
                }
            }

            auto beginIt = require.begin(); /* pgr0745 */
            auto endIt = require.end(); /* pgr0745 */

            for (auto it = childNode.begin(); it != childNode.end(); it++) /* pgr0745 */
            {
                auto childKey = it->first.as<std::string>(); /* pgr0745 */

                auto requireIt = std::find_if(beginIt, endIt, /* pgr0745 */
                                              [&childKey](const YAML::Node & cmpNode)
                {
                    return (cmpNode.as<std::string>() == childKey);
                });
                auto required = (requireIt != require.end()); /* pgr0745 */

                if (Result::OK != parseNode(childKey, depth + 1, required,
                                            it->second, resultSet, false))
                {
                    auto strChildKey = childKey.c_str(); /* pgr0745 */
                    mdb_apl_log_critical("Error parsing child node [%s] of [%s] [level: %d]",
                                         strChildKey, strKey, depth + 1);
                    mdb_apl_log_end();
                    return Result::NG;
                }
            }
        }

        mdb_apl_log_end();
        return Result::OK;
    }

    int MDBYamlConfig::convToPocoFormat(const std::string &format, std::string &pocoFormat)
    {
        mdb_apl_log_start();

        if (0 == format.size())
        {
            return Result::OK;
        }

        auto ret = Result::OK; /* pgr0745 */
        static std::map<std::string, std::string> pocoFormatMap
        {
            {"yyyy-MM-ddTHH:mm:ss.SSSZ+09:00", Poco::DateTimeFormat::ISO8601_FRAC_FORMAT},
            {"yyyy-MM-ddTHH:mm:ss.SSS+09:00", Poco::DateTimeFormat::ISO8601_FRAC_FORMAT},
            {"yyyy-MM-ddTHH:mm:ss.SSSZ", Poco::DateTimeFormat::ISO8601_FRAC_FORMAT}
        };

        try
        {
            pocoFormat = pocoFormatMap.at(format);
            auto strPocoFormat = pocoFormat.c_str();/* pgr0745 */
            mdb_apl_log_debug("convert to poco date format [%s]", strPocoFormat);
        }
        catch (...)
        {
            // not error, just is not supported format
            auto strFormat = format.c_str(); /* pgr0745 */
            mdb_apl_log_debug("original format [%s] is not supported", strFormat);
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }

    int MDBYamlConfig::getDateTimeFormat(const YAML::Node &node, MDBInputKind type, std::string &format)
    {
        mdb_apl_log_start();
        auto formatValue = node[KEY_FORMAT]; /* pgr0745 */
        std::vector<std::string> ignoreFormat
        {
            VALUE_TYPE_ARRAY, VALUE_TYPE_INTEGER, VALUE_TYPE_BOOLEAN, VALUE_TYPE_NUMBER,
            VALUE_TYPE_STRING, VALUE_TYPE_OBJECT, VALUE_TYPE_NULL, VALUE_FORMAT_INT32,
            VALUE_FORMAT_INT64
        };

        if (false == formatValue.IsDefined())
        {
            mdb_apl_log_debug("Key [format] not exist");
            mdb_apl_log_end();
            return Result::OK; //Do NOT consider as an error if there is no 'format' field
        }

        auto ret = Result::OK; /* pgr0745 */

        switch (type) /*pgr0669*/
        {
            case MDBInputKind::STRING:
                {
                    auto tmpFormat = formatValue.as<std::string>(); /* pgr0745 */
                    //trim space
                    size_t first = tmpFormat.find_first_not_of(' ');

                    if (std::string::npos != first)
                    {
                        size_t last = tmpFormat.find_last_not_of(' ');
                        format = tmpFormat.substr(first, (last - first + 1));
                    }

                    //if exist in ignore list, skip
                    auto beginIt = ignoreFormat.begin(); /* pgr0745 */
                    auto endIt = ignoreFormat.end();/* pgr0745 */

                    if (endIt != std::find(beginIt, endIt, format))
                    {
                        format.clear();
                        ret = Result::OK;
                        break;
                    }

                    std::string pocoFormat;

                    if (Result::NG == convToPocoFormat(format, pocoFormat))
                    {
                        ret = Result::NG;
                    }
                    else
                    {
                        format = pocoFormat;
                    }

                    break;
                }

            default:
                mdb_apl_log_debug("Unsupport [format] for type [%d]", static_cast<int>(type));
                ret = Result::NG;
                break;
        }

        mdb_apl_log_end();
        return ret;
    }

    int MDBYamlConfig::getPattern(const YAML::Node &node, MDBInputKind type, std::string &pattern)
    {
        mdb_apl_log_start();

        if (MDBInputKind::COORDINATES == type)
        {
            auto item = node[KEY_ITEMS];  /* pgr0745 */

            if (true == item.IsDefined())
            {
                auto items = item[KEY_ITEMS];  /* pgr0745 */

                if (true == items.IsDefined())
                {
                    if (false == items.IsSequence())
                    {
                        mdb_apl_log_critical("Invalid array for [items]");
                        mdb_apl_log_end();
                        return Result::NG;
                    }

                    std::string combinePattern;

                    for (auto it = items.begin(); it != items.end(); it++) /* pgr0745 */
                    {
                        auto val = it->as<YAML::Node>(); /* pgr0745 */
                        auto pattern = val[KEY_PATTERN];  /* pgr0745 */

                        if (true == pattern.IsDefined())
                        {
                            combinePattern += pattern.as<std::string>();
                        }
                        else
                        {
                            mdb_apl_log_debug("No [pattern] found in [items] of [items] of coordinates");
                            mdb_apl_log_end();
                            return Result::NG;
                        }
                    }

                    pattern = combinePattern;
                }
                else
                {
                    mdb_apl_log_debug("No [items] found in [items] of coordinates");
                    mdb_apl_log_end();
                    return Result::NG;
                }
            }
            else
            {
                mdb_apl_log_debug("No [items] found in coordinates");
                mdb_apl_log_end();
                return Result::NG;
            }
        }

        if (MDBInputKind::ARRAY_STRING == type)
        {
            auto item = node[KEY_ITEMS];  /* pgr0745 */

            if (true == item.IsDefined())
            {
                mdb_apl_log_debug("get pattern of items");
                auto arrayPattern = item[KEY_PATTERN]; /* pgr0745 */

                if (false == arrayPattern.IsDefined())
                {
                    mdb_apl_log_debug("Key [pattern] not exist");
                    mdb_apl_log_end();
                    return Result::NG;
                }

                auto tmpPattern = arrayPattern.as<std::string>();/* pgr0745 */
                //trim "/" pattern if bounded is /.../
                size_t first = tmpPattern.find_first_not_of('/');

                if (std::string::npos != first)
                {
                    size_t last = tmpPattern.find_last_not_of('/');
                    pattern = tmpPattern.substr(first, (last - first + 1));
                }

                mdb_apl_log_end();
                return Result::OK;
            }
            else
            {
                mdb_apl_log_debug("No [items] found in array");
                mdb_apl_log_end();
                return Result::NG;
            }
        }

        auto patternValue = node[KEY_PATTERN]; /* pgr0745 */

        if (false == patternValue.IsDefined())
        {
            mdb_apl_log_debug("Key [pattern] not exist");
            mdb_apl_log_end();
            return Result::NG;
        }

        auto ret = Result::OK;/* pgr0745 */

        switch (type)
        {
            case MDBInputKind::REAL:
            case MDBInputKind::STRING:
                {
                    auto tmpPattern = patternValue.as<std::string>();/* pgr0745 */
                    //trim "/" pattern if bounded is /.../
                    size_t first = tmpPattern.find_first_not_of('/');

                    if (std::string::npos != first)
                    {
                        size_t last = tmpPattern.find_last_not_of('/');
                        pattern = tmpPattern.substr(first, (last - first + 1));
                    }

                    break;
                }

            default:
                mdb_apl_log_debug("Unsupport [pattern] for type [%d]", static_cast<int>(type));
                ret = Result::NG;
                break;
        }

        mdb_apl_log_end();
        return ret;
    }

    int MDBYamlConfig::getEnumList(const YAML::Node &node, MDBInputKind type, std::vector<int> &enumList)
    {

        mdb_apl_log_start();
        auto enum_list = node[KEY_ENUM]; /* pgr0745 */

        if (false == enum_list.IsDefined())
        {
            mdb_apl_log_debug("Key [enum] not exist");
            mdb_apl_log_end();
            return Result::NG;
        }

        auto ret = Result::OK; /* pgr0745 */

        switch (type)
        {
            case MDBInputKind::INTEGER32:
                {

                    if (true == enum_list.IsDefined())
                    {
                        if (false == enum_list.IsSequence())
                        {
                            mdb_apl_log_critical("Invalid setting type for [enum]");
                            mdb_apl_log_end();
                            return Result::NG;
                        }
                    }

                    for (auto it = enum_list.begin(); it != enum_list.end(); it++) /* pgr0745 */
                    {
                        auto val = it->as<std::string>(); /* pgr0745 */
                        enumList.push_back(stoi(val));
                    }

                    break;
                }

            case MDBInputKind::INTEGER64:
            case MDBInputKind::STRING:
            case MDBInputKind::BOOL:
            case MDBInputKind::REAL:
            case MDBInputKind::ARRAY_STRING:
            case MDBInputKind::OBJECT:
            case MDBInputKind::ARRAY_INTEGER64:
            case MDBInputKind::ARRAY_INTEGER32:
            case MDBInputKind::ARRAY_BOOL:
            case MDBInputKind::ARRAY_REAL:
            case MDBInputKind::ARRAY_OBJECT:
            default:
                {
                    mdb_apl_log_debug("Unsupport [enum] for type [%d]", static_cast<int>(type));
                    ret = Result::NG;
                    break;
                }
        }

        mdb_apl_log_end();
        return ret;

    }

    /*!
     * @brief Get type from object node
     * @param [in] node Node data from yaml
     * @param [out] type The template type
     * @retval Result::OK Success.
     * @retval Result::NG Failure
     */
    int MDBYamlConfig::getType(const YAML::Node &node, MDBInputKind &type)
    {
        mdb_apl_log_start();
        auto confType = node[KEY_TYPE]; /* pgr0745 */

        if (false == confType.IsDefined())
        {
            mdb_apl_log_critical("Key [type] not exist");
            mdb_apl_log_end();
            return Result::NG;
        }

        auto typeName = confType.as<std::string>(); /* pgr0745 */
        auto typeArray = (VALUE_TYPE_ARRAY == typeName); /* pgr0745 */

        if (true == typeArray)
        {
            auto items = node[KEY_ITEMS]; /* pgr0745 */

            if (false == items.IsDefined())
            {
                mdb_apl_log_critical("Key [items] not exist for [array] type");
                mdb_apl_log_end();
                return Result::NG;
            }

            auto arrType = items[KEY_TYPE]; /* pgr0745 */

            if (false == items.IsDefined())
            {
                mdb_apl_log_critical("Key [type] not exist for [items]");
                mdb_apl_log_end();
                return Result::NG;
            }

            typeName = arrType.as<std::string>();
        }

        if (VALUE_TYPE_BOOLEAN == typeName)
        {
            if (true == typeArray)
            {
                type = MDBInputKind::ARRAY_BOOL;
            }
            else
            {
                type = MDBInputKind::BOOL;
            }

            mdb_apl_log_end();
            return Result::OK;
        }

        if (VALUE_TYPE_INTEGER == typeName)
        {
            std::string formatName;

            if (true == typeArray)
            {
                auto items = node[KEY_ITEMS]; /* pgr0745 */
                auto format = items[KEY_FORMAT]; /* pgr0745 */

                if (false == format.IsDefined())
                {
                    mdb_apl_log_critical("Key [format] not exist");
                    mdb_apl_log_end();
                    return Result::NG;
                }

                formatName = format.as<std::string>(); /* pgr0745 */
            }
            else
            {
                auto format = node[KEY_FORMAT]; /* pgr0745 */

                if (false == format.IsDefined())
                {
                    mdb_apl_log_critical("Key [format] not exist");
                    mdb_apl_log_end();
                    return Result::NG;
                }

                formatName = format.as<std::string>(); /* pgr0745 */
            }

            if (VALUE_FORMAT_INT32 == formatName)
            {
                if (true == typeArray)
                {
                    type = MDBInputKind::ARRAY_INTEGER32;
                }
                else
                {
                    type = MDBInputKind::INTEGER32;
                }
            }
            else if (VALUE_FORMAT_INT64 == formatName)
            {
                if (true == typeArray)
                {
                    type = MDBInputKind::ARRAY_INTEGER64;
                }
                else
                {
                    type = MDBInputKind::INTEGER64;
                }
            }
            else
            {
                auto strFormat = formatName.c_str(); /* pgr0745 */
                mdb_apl_log_critical("Value of [format] is invalid [%s]", strFormat);
                mdb_apl_log_end();
                return Result::NG;
            }

            mdb_apl_log_end();
            return Result::OK;
        }

        if (VALUE_TYPE_NUMBER == typeName)
        {
            if (true == typeArray)
            {
                type = MDBInputKind::ARRAY_REAL;
            }
            else
            {
                type = MDBInputKind::REAL;
            }

            mdb_apl_log_end();
            return Result::OK;
        }

        if (VALUE_TYPE_STRING == typeName)
        {
            if (true == typeArray)
            {
                type = MDBInputKind::ARRAY_STRING;
            }
            else
            {
                type = MDBInputKind::STRING;
            }

            mdb_apl_log_end();
            return Result::OK;
        }

        if (VALUE_TYPE_OBJECT == typeName)
        {
            if (true == typeArray)
            {
                type = MDBInputKind::ARRAY_OBJECT;
            }
            else
            {
                type = MDBInputKind::OBJECT;
            }

            mdb_apl_log_end();
            return Result::OK;
        }

        if (VALUE_TYPE_ARRAY == typeName)
        {
            mdb_apl_log_critical("Not support type array of array");
        }

        if (VALUE_TYPE_NULL == typeName)
        {
            mdb_apl_log_critical("Not support null type");
        }

        mdb_apl_log_end();
        return Result::NG;
    }

    /*!
     * @brief Get data range from object node
     * @param [in] node Node data from yaml
     * @param [in] type Type of input value
     * @param [out] min Minimim value
     * @param [out] max Maximum value
     * @retval true Value range is defined.
     * @retval false Value range is not defined
     */
    bool MDBYamlConfig::getRange(const YAML::Node &node, MDBInputKind type, double &min, double &max)
    {
        mdb_apl_log_start();
        mdb_apl_log_debug("check range for type=%d", static_cast<int>(type));

        if (MDBInputKind::STRING == type) /*pgr0607*/
        {
            auto minLength = node[KEY_MINLENGTH]; /* pgr0745 */
            auto maxLength = node[KEY_MAXLENGTH]; /* pgr0745 */

            if ((true == minLength.IsDefined())
                    && (true == maxLength.IsDefined()))
            {
                min = minLength.as<double>();
                max = maxLength.as<double>();
                mdb_apl_log_end();
                return true;
            }
        }
        else if ((MDBInputKind::INTEGER32 == type)
                 || (MDBInputKind::INTEGER64 == type)
                 || (MDBInputKind::REAL == type))
        {
            auto minValue = node[KEY_MINIMUM]; /* pgr0745 */
            auto maxValue = node[KEY_MAXIMUM]; /* pgr0745 */

            if ((true == minValue.IsDefined())
                    && (true == maxValue.IsDefined()))
            {
                min = minValue.as<double>();
                max = maxValue.as<double>();
                mdb_apl_log_end();
                return true;
            }
        }
        else if ((MDBInputKind::ARRAY_INTEGER64 == type)
                 || (MDBInputKind::ARRAY_INTEGER32 == type)
                 || (MDBInputKind::ARRAY_REAL == type))
        {
            auto items = node[KEY_ITEMS]; /* pgr0745 */

            if (true == items.IsDefined())
            {
                auto minValue = items[KEY_MINIMUM]; /* pgr0745 */
                auto maxValue = items[KEY_MAXIMUM]; /* pgr0745 */

                if ((true == minValue.IsDefined())
                        && (true == maxValue.IsDefined()))
                {
                    min = minValue.as<double>();
                    max = maxValue.as<double>();
                    mdb_apl_log_end();
                    return true;
                }
            }
        }

        mdb_apl_log_end();
        return false;
    }

    /*!
     * @brief Get default value from object node
     * @param [in] node Node data from yaml
     * @param [in] type Datatype of node
     * @param [out] defaultValue The default value
     * @retval Result::OK Success.
     * @retval Result::NG Failure
     */
    int MDBYamlConfig::getDefaultValue(const YAML::Node &node, MDBInputKind type, Poco::Any &defaultValue)
    {
        mdb_apl_log_start();
        auto value = node[KEY_DEFAULT]; /* pgr0745 */

        if (false == value.IsDefined())
        {
            mdb_apl_log_debug("Key [default] not exist");
            mdb_apl_log_end();
            return Result::NG;
        }

        auto ret = Result::OK; /* pgr0745 */

        switch (type)
        {
            case MDBInputKind::STRING:
                {
                    defaultValue = value.as<std::string>();
                    break;
                }

            case MDBInputKind::INTEGER64:
                {
                    defaultValue = value.as<long long>();
                    break;
                }

            case MDBInputKind::INTEGER32:
                {
                    defaultValue = value.as<int>();
                    break;
                }

            case MDBInputKind::BOOL:
                {
                    defaultValue = value.as<bool>();
                    break;
                }

            case MDBInputKind::REAL:
                {
                    defaultValue = value.as<double>();
                    break;
                }

            case MDBInputKind::ARRAY_STRING:
                {
                    //                auto childNode = node[KEY_ITEMS];
                    //                auto tempVal = childNode[KEY_DEFAULT];
                    defaultValue = std::string {};
                    break;
                }

            case MDBInputKind::OBJECT:
            case MDBInputKind::ARRAY_INTEGER64:
            case MDBInputKind::ARRAY_INTEGER32:
            case MDBInputKind::ARRAY_BOOL:
            case MDBInputKind::ARRAY_REAL:
            case MDBInputKind::ARRAY_OBJECT:
            default:
                {
                    mdb_apl_log_debug("Unsupport default value for type [%d]", static_cast<int>(type));
                    ret = Result::NG;
                    break;
                }
        }

        mdb_apl_log_end();
        return ret;
    }
};

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBContentAnalyzer.h
 * @brief       The header of MDBContentAnalyzer class.
 * @author      cuongadp
 * @date        2017/04/17
 */

#ifndef LIBMDBINOUT_MDBCONTENTANALYZER_H_
#define LIBMDBINOUT_MDBCONTENTANALYZER_H_
#include <vector>

#include <Poco/Any.h>
#include <Poco/String.h>

#include "MDBLogger.h"
#include "MDBContentParser.h"
#include "MDBConstants.h"

namespace mdbcom {
    /*!
     * @class MDBContentRule
     * @brief The rule apply for the input value
     */
    enum class MDBContentRule
    {
        //! Accept any value (not validate)
        ANY = 0,
        //! Must exist key
        MUST_EXIST = 1,
        //! Must not exist key
        MUST_NOT_EXIST = 2,
        //! Must have value. Not apply for OBJECT and ARRAY_OBJECT
        MUST_EQUAL = 3,
        //! Must not have value. Not apply for OBJECT and ARRAY_OBJECT
        MUST_NOT_EQUAL = 4,
        //! Must be one of defined value. Not apply for OBJECT and ARRAY_XXX
        IS_ONE_OF = 5,
        //! Must not be one of defined value. Not apply for OBJECT and ARRAY_XXX
        IS_NOT_ONE_OF = 6
    };

    /*!
     * @class MDBContentRuleEntry
     * @brief The entry for table analysis.
     */
    class MDBContentRuleEntry {
    public:
        /*!
         * @brief construtor
         * @param None.
         * @return None.
         */
        MDBContentRuleEntry(const std::string &inputKey, const std::string &inputParentKey,
                            MDBInputKind inputType, MDBContentRule inputRule,
                            const Poco::Any &inputCompareValue, const Poco::Any &inputSetter) :
            key {inputKey}, parentKey {inputParentKey}, type {inputType},
            rule {inputRule}, compareValue {inputCompareValue},
            setter {inputSetter} {
        };

        /*!
         * @brief construtor
         * @param None.
         * @return None.
         */
        MDBContentRuleEntry(const std::string &inputKey, const std::initializer_list<std::string> &parentKeys,
                            MDBInputKind inputType,
                            MDBContentRule inputRule = MDBContentRule::ANY,
                            const Poco::Any &inputCompareValue = Poco::Any
        {
            /* empty */
        },
        const Poco::Any &inputSetter = Poco::Any { /* empty */ },
                         bool isSubArray = false) :
            key {inputKey},
            parentKey { Poco::cat(std::string {"/"},
                                  parentKeys.begin(), parentKeys.end())}, /*pgr0512*/
            type {inputType},
            rule {inputRule}, compareValue {inputCompareValue},
            setter {inputSetter} {

            if (isSubArray == true) {
                // add array notation
                parentKey += "[]";
            }
        };
    private:
        //! Key name
        std::string key;
        //! Parrent keys path
        std::string parentKey;
        //! Type of value
        MDBInputKind type;
        //! Rule to apply
        MDBContentRule rule;
        //! Commpare value, apply for these rules: MUST_EQUAL & MUST_NOT_EQUAL
        Poco::Any compareValue;
        //! Setter function, ignore if empty
        Poco::Any setter;

        friend class MDBContentAnalyzer;
    };

    /*!
     * @class MDBContentAnalyzer
     * @brief Analyzer interface
     */
    class MDBContentAnalyzer { /* pgr2204 */
    public:

        /*!
         * @brief Default construtor
         * @param None.
         * @return None.
         */
        MDBContentAnalyzer(void) : decoder { nullptr } {
        };

        /*!
         * @brief Construtor with decoder
         * @param None.
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        MDBContentAnalyzer(MDBContentParser *inDecoder) : decoder { inDecoder } {
        };

        /*!
         * @brief Copy construtor
         * @param None.
         * @return None.
         */
        MDBContentAnalyzer(const MDBContentAnalyzer &) = default; /*pgr0571*/ /* pgr2266 */

        /*!
         * @brief Move construtor
         * @param None.
         * @return None.
         */
        MDBContentAnalyzer(MDBContentAnalyzer &&) = default;

        /*!
         * @brief Copy operator
         * @param None.
         * @return None.
         */
        MDBContentAnalyzer &operator=(const MDBContentAnalyzer &) = default;  /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Move operator
         * @param None.
         * @return None.
         */
        MDBContentAnalyzer &operator=(MDBContentAnalyzer &&) = default;  /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Default destructor
         * @param None.
         * @return None.
         */
        virtual ~MDBContentAnalyzer(void) = default;

        /*!
         * @brief Execute analysis and set value from input map
         * @param [in] inputMap Input value
         * @param [out] containObj Object to contain result after analyzing inputMap
         * @param [out] errorKey key not comply rule
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        template<class ContainerObject>
        int analyze(const MDBContentParser::KeyMapType &inputMap, ContainerObject &containObj, std::string &errorKey) { /* pgr0304 */
            mdb_apl_log_start();

            if (nullptr == decoder) {
                mdb_apl_log_critical("Input decoder is not specified");
                mdb_apl_log_end();
                return Result::NG;
            }

            // get table of rules
            auto ruleSet = getRuleTable(); /* pgr0745 */

            if (nullptr == ruleSet) {
                mdb_apl_log_critical("Can't acquire rule table");
                mdb_sys_log_out("F001101");
                mdb_apl_log_end();
                return Result::NG;
            }

            auto ret = Result::OK; /* pgr0745 */

            // setting for each key defined in ruleset.
            // ruleSet must be container: std::vector<MDBContentRuleEntry>
            for (auto &rule : (*ruleSet)) { /* pgr0745 */
                auto key = rule.key.c_str(); /*pgr0745*/ /*pgr0039*/
                auto parent = rule.parentKey.c_str(); /*pgr0745*/
                mdb_apl_log_trace("Checking rule for [key:%s][parent:%s][type:%d][rule:%d]",
                                  key, parent, static_cast<int>(rule.type), static_cast<int>(rule.rule));

                // call checking function depend on the type
                switch (rule.type) {
                    case MDBInputKind::INTEGER32:
                        ret = analyzeValue<ContainerObject, int>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::INTEGER64:
                        ret = analyzeValue<ContainerObject, long long>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::REAL:
                        ret = analyzeValue<ContainerObject, double>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::STRING:
                        ret = analyzeValue<ContainerObject, std::string>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::BOOL:
                        ret = analyzeValue<ContainerObject, bool>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::OBJECT:
                        ret = analyzeObject(inputMap, rule);
                        break;

                    case MDBInputKind::ARRAY_INTEGER32:
                        ret = analyzeArray<ContainerObject, int>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::ARRAY_INTEGER64:
                        ret = analyzeArray<ContainerObject, long long>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::ARRAY_REAL:
                        ret = analyzeArray<ContainerObject, double>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::ARRAY_STRING:
                        ret = analyzeArray<ContainerObject, std::string>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::ARRAY_BOOL:
                        ret = analyzeArray<ContainerObject, bool>(inputMap, rule, containObj);
                        break;

                    case MDBInputKind::ARRAY_OBJECT:
                        ret = analyzeObject(inputMap, rule);
                        break;

                    default:
                        mdb_apl_log_critical("Unsupport data type");
                        ret = Result::NG;
                        break;
                }

                if (Result::OK != ret) {
                    errorKey = key;
                    mdb_apl_log_error("Input map not comply to rules ");
                    mdb_sys_log_out("F001102");
                    break;
                };
            }

            mdb_apl_log_end();
            return ret;
        }

    protected:
        /*!
         * @brief Searching method, it must be implemented by derived class
         * @param None.
         * @return Pointer to rule table
         */
        virtual const std::vector<MDBContentRuleEntry> *getRuleTable(void) = 0;

    private:
        //! Pointer to decoder object
        MDBContentParser *decoder;

        /*!
         * @brief Apply rule for array data
         * @param [in] inputMap Input map.
         * @param [in] rule Rule entry to be applied.
         * @param [out] containObj Container object
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        template<class ContainerObject, class Type>
        int analyzeArray(const MDBContentParser::KeyMapType &inputMap, const MDBContentRuleEntry &rule, ContainerObject &containObj) { /*pgr0304*/
            mdb_apl_log_start();
            auto errorFlag = false; /*pgr0745*/
            auto actualValue = decoder->getKeyMapDataArrayValue<Type>(inputMap, rule.key, rule.parentKey, errorFlag); /*pgr0745*/

            switch (rule.rule) {
                case MDBContentRule::MUST_EXIST: {
                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_EXIST [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_NOT_EXIST: {
                        if (false == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EXIST [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_EQUAL: {
                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_EQUAL: key not exist[key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        // cast compare value
                        auto compareValue = Poco::AnyCast<std::vector<Type>>(&rule.compareValue); /*pgr0745*/

                        if (nullptr == compareValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_critical("Invalid compare value setting for [key:%s] [parent:%s]",
                                                 key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        if ((*compareValue) != actualValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_EQUAL: value not equal [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_NOT_EQUAL: {
                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EQUAL: key not exist[key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        // cast compare value
                        auto compareValue = Poco::AnyCast<std::vector<Type>>(&rule.compareValue); /*pgr0745*/

                        if (nullptr == compareValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_critical("Invalid compare value setting for [key:%s] [parent:%s]",
                                                 key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        if ((*compareValue) == actualValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EQUAL: value equal [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::IS_ONE_OF:
                case MDBContentRule::IS_NOT_ONE_OF: {
                        // comparing object is not support
                        // in order to make rule for object, define specific rule for each key.
                        mdb_apl_log_critical("Invalid setting for comparing array. Enum checking rule is not supported for array");
                        return Result::NG;
                    }

                default:
                    // ANY case
                    // do nothing
                    break;
            }

            // Set value to container if have value and have setter function
            // if can get value and have setter functions
            if ((false == errorFlag) && (false == rule.setter.empty())) {
                auto setterFn = Poco::AnyCast<std::_Mem_fn<void (ContainerObject::*)(const std::vector<Type>&)>>(&rule.setter); /*pgr0745*/

                if (nullptr == setterFn) {
                    auto key = rule.key.c_str(); /*pgr0745*/
                    auto parent = rule.parentKey.c_str(); /*pgr0745*/
                    mdb_apl_log_critical("Invalid setter function for [key:%s] [parent:%s]",
                                         key, parent);
                    mdb_apl_log_end();
                    return Result::NG;
                }

                (*setterFn)(containObj, actualValue);
            }

            mdb_apl_log_end();
            return Result::OK;
        }

        /*!
         * @brief Apply rule for normal data
         * @param [in] inputMap Input map.
         * @param [in] rule Rule entry to be applied.
         * @param [out] containObj Container object
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        template<class ContainerObject, class Type>
        int analyzeValue(const MDBContentParser::KeyMapType &inputMap, const MDBContentRuleEntry &rule, ContainerObject &containObj) { /*pgr0304*/
            mdb_apl_log_start();
            auto errorFlag = false; /*pgr0745*/
            auto actualValue = decoder->getKeyMapDataValue<Type>(inputMap, rule.key, rule.parentKey, errorFlag); /*pgr0745*/

            switch (rule.rule) {
                case MDBContentRule::MUST_EXIST: {
                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_EXIST [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_NOT_EXIST: {
                        if (false == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EXIST [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_EQUAL: {
                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_EQUAL: key not exist[key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        // cast compare value
                        auto compareValue = Poco::AnyCast<Type>(&rule.compareValue);/* pgr0745 */

                        if (nullptr == compareValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_critical("Invalid compare value setting for [key:%s] [parent:%s]",
                                                 key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        if ((*compareValue) != actualValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_EQUAL: value not equal [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_NOT_EQUAL: {
                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EQUAL: key not exist[key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        // cast compare value
                        auto compareValue = Poco::AnyCast<Type>(&rule.compareValue);/* pgr0745 */

                        if (nullptr == compareValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_critical("Invalid compare value setting for [key:%s] [parent:%s]",
                                                 key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        if ((*compareValue) == actualValue) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EQUAL: value equal [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::IS_ONE_OF: {

                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EQUAL: key not exist[key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        // cast compare value
                        auto compareValues = Poco::AnyCast<std::vector<Type>>(&rule.compareValue); /*pgr0745*/

                        if (nullptr == compareValues) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_critical("Invalid compare value setting for [key:%s] [parent:%s]",
                                                 key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        auto begin = compareValues->begin(); /*pgr0745*/
                        auto end = compareValues->end(); /*pgr0745*/
                        auto findResult = std::find(begin, end, actualValue); /*pgr0745*/

                        if (end == findResult) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule IS_NOT_ONE_OF: value not found [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::IS_NOT_ONE_OF: {
                        if (true == errorFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EQUAL: key not exist[key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        // cast compare value
                        auto compareValues = Poco::AnyCast<std::vector<Type>>(&rule.compareValue); /*pgr0745*/

                        if (nullptr == compareValues) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_critical("Invalid compare value setting for [key:%s] [parent:%s]",
                                                 key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        auto begin = compareValues->begin(); /*pgr0745*/
                        auto end = compareValues->end(); /*pgr0745*/
                        auto findResult = std::find(begin, end, actualValue); /*pgr0745*/

                        if (end != findResult) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule IS_NOT_ONE_OF: value found [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                default:
                    // ANY case
                    // do nothing
                    break;
            }

            // Set value to container if have value and have setter function
            // if can get value and have setter functions
            if ((false == errorFlag) && (false == rule.setter.empty())) {
                auto setterFn = Poco::AnyCast<std::_Mem_fn<void (ContainerObject::*)(Type)>>(&rule.setter);/* pgr0745 */
                auto key = rule.key.c_str(); /*pgr0745*/
                auto parent = rule.parentKey.c_str(); /*pgr0745*/

                if (nullptr == setterFn) {
                    mdb_apl_log_critical("Invalid setter function for [key:%s] [parent:%s]",
                                         key, parent);
                    mdb_apl_log_end();
                    return Result::NG;
                }

                try {
                    (*setterFn)(containObj, actualValue);
                }
                catch (...) {
                    mdb_apl_log_critical("An exception occured when set value for [key:%s] [parent:%s]", key, parent);
                    mdb_apl_log_end();
                    return Result::NG;
                }

            }

            mdb_apl_log_end();
            return Result::OK;
        }

        /*!
         * @brief Apply rule for normal data
         * @param [in] inputMap Input map.
         * @param [in] rule Rule entry to be applied.
         * @param [out] containObj Container object
         * @retval Result::OK Success.
         * @retval Result::NG Failure.
         */
        int analyzeObject(const MDBContentParser::KeyMapType &inputMap, const MDBContentRuleEntry &rule) { /* pgr0304 */
            auto existFlag = decoder->isExistKey(rule.key, rule.parentKey, inputMap);/* pgr0745 */

            switch (rule.rule) {
                case MDBContentRule::MUST_EXIST: {
                        if (false == existFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_EXIST [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_NOT_EXIST: {
                        if (true == existFlag) {
                            auto key = rule.key.c_str(); /*pgr0745*/
                            auto parent = rule.parentKey.c_str(); /*pgr0745*/
                            mdb_apl_log_error("Mismatch rule MUST_NOT_EXIST [key:%s] [parent:%s]",
                                              key, parent);
                            mdb_apl_log_end();
                            return Result::NG;
                        }

                        break;
                    }

                case MDBContentRule::MUST_EQUAL:
                case MDBContentRule::MUST_NOT_EQUAL:
                    // comparing object is not support
                    // in order to make rule for object, define specific rule for each key.
                    mdb_apl_log_critical("Invalid setting for comparing object. Define specific rules for object's keys instead of comparing 2 object");
                    return Result::NG;
                    break;

                default:
                    // ANY case
                    // do nothing
                    break;
            }

            if (false == rule.setter.empty()) {
                mdb_apl_log_critical("Invalid setting for setting object. Define specific rules for object's keys instead of setting object");
                return Result::NG;
            }

            mdb_apl_log_end();
            return Result::OK;
        }
    };
}
#endif

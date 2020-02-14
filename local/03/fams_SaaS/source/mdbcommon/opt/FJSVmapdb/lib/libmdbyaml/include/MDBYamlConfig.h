/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBYamlConfig.h
 * @brief       The header of MDBYamlConfig class.
 * @author      cuongadp
 * @date        2017/04/24
 */

#ifndef LIBMDBYAML_MDBYAMLCONFIG_H_
#define LIBMDBYAML_MDBYAMLCONFIG_H_

#include <yaml-cpp/yaml.h>
#include "MDBContentConfig.h"

namespace mdbcom {
    /*!
     * @enum MDBYamlConfig
     * Configuration class for parsing template configuration in yaml format
     */
    class MDBYamlConfig: public MDBContentConfig { /* pgr2205 */
        /* pgr2252 */

    public:
        /*!
         * @brief Construtor
         * @param [in] path Path to configuration file
         * @return None.
         */
        explicit MDBYamlConfig(const std::string &path);

        /*!
         * @brief Destructor
         * @param None.
         * @return None.
         */
        virtual ~MDBYamlConfig(void) = default;

        /*!
         * @brief Copy Construtor
         * @param [in] other Other instance
         * @return None.
         */
        MDBYamlConfig(const MDBYamlConfig &) = default; /* pgr2266 */ /* pgr0571 */

        /*!
         * @brief Move Construtor
         * @param [in/out] other Other instance
         * @return None.
         */
        MDBYamlConfig(MDBYamlConfig &&) = default;

        /*!
         * @brief Copy Assignment
         * @param [in] other Other instance
         * @return Reference to current instance.
         */
        MDBYamlConfig &operator=(const MDBYamlConfig &) = default; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Copy Assignment
         * @param [in] other Other instance
         * @return Reference to current instance.
         */
        MDBYamlConfig &operator=(MDBYamlConfig &&) = default; /* pgr0055 */ /* pgr0309 */

        /*!
         * @brief Parse the decoding template from configuration data
         * @param [in] url URL to get template.
         * @param [in] method Method of the URL
         * @param [out] formatTemplate The result template
         * @retval Result::OK Success.
         * @retval Result::NG Failure
         */
        virtual int getFormat(const std::string &url, const std::string &methodm,
                              std::vector<MDBContentDecodingFormat> &formatTemplate) override;

    private:
        using Properties = std::map<std::string, std::map<std::string, YAML::Node>>;
        //! Map of Node by URL and method
        Properties propList;

        /*!
         * @brief Get Node the configuration data
         * @param [in] url URL to get template.
         * @param [in] method Method of the URL
         * @return Pointer to node data
         */
        YAML::Node *getProperties(const std::string &url, const std::string &method);

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
        int parseNode(const std::string &key, int depth, bool required, const YAML::Node &node,
                      std::vector<MDBContentDecodingFormat> &resultSet, bool root);

        /*!
         * @brief Get type from object node
         * @param [in] key Name of current node
         * @param [in] depth Depth level of current node (root is 0)
         * @param [in] required Is the node required by parrent
         * @param [in] node Node data from yaml
         * @param [out] formatTemplate The result template
         * @retval Result::OK Success.
         * @retval Result::NG Failure
         */
        int getType(const YAML::Node &node, MDBInputKind &type);

        /*!
         * @brief Get data range from object node
         * @param [in] node Node data from yaml
         * @param [in] type Type of input value
         * @param [out] min Minimim value
         * @param [out] max Maximum value
         * @retval true Value range is defined.
         * @retval false Value range is not defined
         */
        bool getRange(const YAML::Node &node, MDBInputKind type, double &min, double &max);

        /*!
         * @brief Get default value from object node
         * @param [in] node Node data from yaml
         * @param [in] type Datatype of node
         * @param [out] defaultValue The default value
         * @retval Result::OK Success.
         * @retval Result::NG Failure
         */
        int getDefaultValue(const YAML::Node &node, MDBInputKind type, Poco::Any &defaultValue);

        int getEnumList(const YAML::Node &node, MDBInputKind type, std::vector<int> &enumList);

        int getPattern(const YAML::Node &node, MDBInputKind type, std::string &pattern);

        int getDateTimeFormat(const YAML::Node &node, MDBInputKind type, std::string &format);

        int convToPocoFormat(const std::string &format, std::string &pocoFormat);
    };
};

#endif

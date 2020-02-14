/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBContentParser.h
 * @brief       The header of MDBContentParser class.
 * @author      cuongadp
 * @date        2017/04/17
 */

#ifndef LIBMDBINOUT_MDBCONTENTPARSER_H_
#define LIBMDBINOUT_MDBCONTENTPARSER_H_

#include <Poco/Any.h>

#include "MDBLogger.h"
#include "MDBContentDecodingFormat.h"

namespace mdbcom {

    /*! @class MDBContentParser
     *  @brief  This class provides the common parsing JSON APIs for users
     */
    class MDBContentParser {
    public:

        //! Define the MDB JSON Key Map type
        using KeyMapType = std::map<std::string, Poco::Any>;

        /*!
         * @brief constructor of MDBContentParser.
         * @param None.
         * @return None.
         */
        MDBContentParser(void);
        /*!
         * @brief Destructor of MDBContentParser.
         * @param None.
         * @return None.
         */
        virtual ~MDBContentParser(void) = default;

        /*!
         * @brief Copy constructor of MDBContentParser [unused].
         * @param other the other MDBContentParser
         * @return None.
         */
        MDBContentParser(const MDBContentParser &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        /*!
         * @brief copy assignment operator [unused].
         *
         * @param None.
         * @return None.
         */
        MDBContentParser &operator=(const MDBContentParser &) = delete; /*pgr0055*//*pgr2223*/ /*pgr0309*/

        // List KeyMapData Method

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
        void setKeyMapDataValue(KeyMapType &setKeyMapData, const std::string &key,
                                const T_ENCODE_VALUE &setData);

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
        void setKeyMapDataArrayValue(KeyMapType &setKeyMapData, const std::string &key,
                                     const std::vector<T_ENCODE_VALUE> &setDataList);

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
        T_DECODE_VALUE getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
                                          const std::string &key, bool &errFlag);

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
        T_DECODE_VALUE getKeyMapDataValue(const KeyMapType &decodeKeyMapData,
                                          const std::string &key, const std::string &parentKey, bool &errFlag);

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
        std::vector<T_DECODE_VALUE> getKeyMapDataArrayValue(const KeyMapType &decodeKeyMapData,
                const std::string &key, bool &errFlag);

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
        std::vector<T_DECODE_VALUE> getKeyMapDataArrayValue(const KeyMapType &decodeKeyMapData,
                const std::string &key, const std::string &parentKey, bool &errFlag);

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
        bool isExistKey(const std::string &key,
                        const std::map<std::string, Poco::Any> &decodeKeyMapData);
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
        bool isExistKey(const std::string &key, const std::string &parentKey,
                        const std::map<std::string, Poco::Any> &decodeKeyMapData);

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
        virtual int decodeKeyMapData(const std::string &inputJsonString,
                                     const std::vector<MDBContentDecodingFormat> &jsonDecodeTemplete,
                                     const std::string &decodeBaseKey, KeyMapType &keyMapData,
                                     std::string &outputFirstKeyError) = 0;

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
        virtual int decodeKeyMapData(const std::string &inputJsonString,
                                     const std::vector<MDBContentDecodingFormat> &jsonDecodeTemplete,
                                     const std::string &decodeBaseKey, KeyMapType &keyMapData,
                                     std::string &outputFirstKeyError, Poco::Any &errorValue) = 0;

        /*!
         * @brief Encode from the JSON map data to the JSON string.
         *
         * This method is used to encode from the JSON map data to the JSON string.
         * @param [in] jsonKeyMapData the input JSON map data
         * @param [out] createJsonString the output JSON string
         * @retval  Json::OK if encoding is successful
         * @retval  Json::NG if encoding is failure
         */
        virtual int encodeKeyMapData(const KeyMapType &jsonKeyMapData, std::string &createJsonString, bool printDebugJSON = true) = 0;

        /*!
         * @brief This method is used to set initial position of key in map
         * @param [in] order the position of key in list of keys
         * @return None
         */
        inline void setSerialKey(unsigned int order) {
            mSerialKey = order;
        }

    protected:
        //! this used for preserving order of keys
        unsigned int mSerialKey;
        //! this is max number of digits of keys to output encoded JSON to be exact order
        static unsigned constexpr int MAX_NUM_DIGIT_KEYS = 3; //max is 999 keys

        /*!
         * @brief Convert string to list key.
         *
         * This method is used to Convert string to list key.
         * @param [in] str  the input string data
         * @param [in] delimiter the signal delimiter
         * @return  the list key
         */
        std::vector<std::string> convertStringToList(const std::string &str,
                const std::string &delimiter);

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
        int findJsonMapDataKey(const std::string &key, const std::string &parentKey,
                               const std::map<std::string, Poco::Any> &decodeKeyMapData, Poco::Any &outValue);

        /*!
         * @brief Check whether the data type of the specified Any data and the Value type is correct.
         *
         * This method is used to check whether the data type of the specified Any data and the Value type is correct.
         * @param [in] type  the decoding value type
         * @param [in] data the any data.
         * @retval  true if normal
         * @retval  false if abnormal
         */
        bool isValidAnyDataType(MDBInputKind type, const Poco::Any data);

        /*!
         * @brief  Create a full path key bound in with a specified parent Key self-Key.
         *
         * This method is used to create a full path key bound in with a specified parent Key self-Key.
         * @param [in] parentKey  the specified parent key
         * @param [in] ownKey the specifying key.
         * @return The full path key string
         */
        std::string createFullPathKey(const std::string &parentKey, const std::string &ownKey);

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
        int validateOmitFlag(const MDBContentDecodingFormat &decodeTemplete,
                             const std::string &myParentKey, const std::string &key);

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
        int validateValidNullFlag(const MDBContentDecodingFormat &decodeTemplete,
                                  const std::string &myParentKey, const std::string &key, KeyMapType &outJsonMapData);
    };
}

#endif /* LIBMDBINOUT_MDBCONTENTPARSER_H_ */

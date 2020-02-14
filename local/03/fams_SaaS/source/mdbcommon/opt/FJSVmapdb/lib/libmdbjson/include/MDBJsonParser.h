/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBJsonParser.h
 * @brief       The header of MDBJsonParser class.
 * @author      NamVo: Initialize MDBJsonParser class.
 * @date        2016/9/23
 */

#ifndef LIBMDBJSON_MDBJSONPARSER_H_
#define LIBMDBJSON_MDBJSONPARSER_H_

#include "jansson.h"

#include "MDBContentParser.h"
#include "MDBLogger.h"
#include "MDBContentDecodingFormat.h"

namespace mdbcom {

    /*! @class MDBJsonParser
     *  @brief  This class provides the common parsing JSON APIs for users
     */
    class MDBJsonParser : public MDBContentParser { /*pgr2252 */
        /*pgr2205*//* pgr1233*/

    public:
        /*!
         * @brief constructor of MDBJsonParser.
         * @param None.
         * @return None.
         */
        MDBJsonParser(void);
        /*!
         * @brief Destructor of MDBJsonParser.
         * @param None.
         * @return None.
         */
        virtual ~MDBJsonParser(void) = default;

        /*!
         * @brief Copy constructor of MDBJsonParser [unused].
         * @param other the other MDBJsonParser
         * @return None.
         */
        MDBJsonParser(const MDBJsonParser &) = default; /*pgr0055*//*pgr2223*/ /*pgr0309*/ /*pgr2222*/

        /*!
         * @brief copy assignment operator [unused].
         *
         * @param None.
         * @return None.
         */
        MDBJsonParser &operator=(const MDBJsonParser &) = default; /*pgr0055*//*pgr2223*/ /*pgr0309*/

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
                                     std::string &outputFirstKeyError) override;

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
                                     std::string &outputFirstKeyError, Poco::Any &errorValue) override;

        /*!
         * @brief Encode from the JSON map data to the JSON string.
         *
         * This method is used to encode from the JSON map data to the JSON string.
         * @param [in] jsonKeyMapData the input JSON map data
         * @prarm [in] dumpJson dump json file to debug
         * @param [out] createJsonString the output JSON string
         * @retval  Json::OK if encoding is successful
         * @retval  Json::NG if encoding is failure
         */
        int encodeKeyMapData(const KeyMapType &jsonKeyMapData, std::string &createJsonString, bool printDebugJSON = true) override;

        static constexpr int VALID_TIME_ZONE = 9;

    private:
        //! validating template JSON
        std::vector<MDBContentDecodingFormat> mDecodingTemplate;
        //! Constant for JANSSON lib return
        static constexpr int MDB_JSON_JANSSON_RESULT_OK = 0;

        //! this is maximum precision to parse double to string using json_dump
        // this got from pattern tag in YAML file
        // Ex: Latitude: 12.123456 and Height: 123456789.12
        // => maximum is precision of time = 10 (second part) + 3(milisecond)
        static constexpr int MAX_PRECISION_REAL_VALUE = 13;

        //! coordinates key
        static const std::string COORDINATES_KEY;

        /*!
         * @enum MDBMFlag
         * @brief define encode, decode flag
         */
        enum class MDBMFlag
        {
            //! description element
            ENCODE_FLGS,
            DECODE_FLGS
        };

        /*!
         * @brief Create the JSON key map data from the specified Json Object.
         *
         * This method is used to create the JSON key map data from the specified JSON Object.
         * @param [in] parseDepth the depth of the current JSON hierarchy
         * @param [in] templeteIndex the index of the validating template
         * @param [in] myParentKey the parent key
         * @param [in] inputJsonObject the input JSON object.
         * @param [out] outValue the output the JSON key map data.
         * @param [out] keyError the invalid info key
         * @retval  Json::OK if key is found
         * @retval  Json::NG if key is not found
         */
        int toJsonKeyMapData(const int parseDepth, const int templeteIndex,
                             const std::string &myParentKey, json_t *inputJsonObject, KeyMapType &outJsonMapData,
                             std::string &keyError, Poco::Any &errorValue);

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
        int jsonObjectDecode(const int parseDepth, const int templeteIndex,
                             const std::string &myParentKey, json_t *inputJsonObject,
                             const MDBContentDecodingFormat &decodeTemplete, KeyMapType &outJsonMapData,
                             std::string &keyError, Poco::Any &errorValue);

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
        int getArrayValue(json_t *inputJsonObject, const MDBContentDecodingFormat &decodeTemplete,
                          std::vector<Poco::Any> &outArrayValue, Poco::Any &errorValue, int &errIdx);

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
        int getStringValue(json_t *inputJsonObject, const MDBContentDecodingFormat &decodeTemplete,
                           std::string &outStringValue, Poco::Any &errorValue);

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
        int getCoordStringValue(json_t *inputJsonObject, const MDBContentDecodingFormat &decodeTemplete,
                                std::string &outStringValue, Poco::Any &errorValue);

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
        int getInteger64Value(json_t *inputJsonObject, const MDBContentDecodingFormat &decodeTemplete,
                              long long &outLonglongValue, Poco::Any &errorValue);

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
        int getInteger32Value(json_t *inputJsonObject, const MDBContentDecodingFormat &decodeTemplete,
                              int &outIntValue, Poco::Any &errorValue);

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
        int getBooleanValue(json_t *inputJsonObject, const MDBContentDecodingFormat &decodeTemplete,
                            bool &outBoolValue);

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
        int getRealValue(json_t *inputJsonObject, const MDBContentDecodingFormat &decodeTemplete,
                         double &outDoubleValue);

        /*!
         * @brief  Create a JsonObject from JSON key map data.
         *
         * This method is used to create a JsonObject from JSON key map data.
         * @param [in] jsonKeyValue  the specifying key
         * @retval Json object  if success
         * @retval nullptr if failure
         */
        json_t *toJsonObject(const Poco::Any jsonKeyValue);

        /*!
         * @brief  Check the specified JSON object whether it is JSON integer 32 bit.
         *
         * This method is used to check the specified JSON object whether it is JSON integer 32 bit.
         * @param [in] json  the pointer to JSON object
         * @retval true if valid
         * @retval false if invalid
         */
        bool json_is_integer32(const json_t *json);

        /*!
         * @brief  Get the integer 32 bit value from specified JSON object.
         *
         * This method is used to get the integer 32 bit value from specified JSON object.
         * @param [in] integer  the pointer to JSON object
         * @return  Integer 32 bit value
         */
        std::int32_t json_integer32_value(const json_t *integer);

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
        int arrObjectJsonToJsonKeyMapData(const int parseDepth, const int templeteIndex,
                                          const std::string &myParentKey, const std::string &key, json_t *inputJsonObject,
                                          KeyMapType &outJsonMapData, std::string &keyError, Poco::Any &errorValue);

        /*!
         * @brief Convert from any to an JSON array object.
         *
         * This method is used to convert from any to an JSON array object.
         * @param [in] jsonKeyValue the vector any value
         * @return The JSON array object
         */
        json_t *handleToJsonArrayObject(const Poco::Any &jsonKeyValue);

        /*!
         * @brief Convert from any to an JSON object.
         *
         * This method is used to convert from any to an JSON object.
         * @param [in] jsonKeyValue the  any value
         * @return The JSON object
         */
        json_t *handleToJsonObject(const Poco::Any &jsonKeyValue);

        /*!
         * @brief Find the last quotation mark of key error.
         *
         * This method is used to find the last quotation mark of key error.
         * @param [in] jsonString the  JSON string
         * @return The position of last quotation mark
         */
        int findLastQuotationMark(const std::string &jsonString);

        /*!
         * @brief Get key error when JSON string contains invalid information.
         *
         * This method is used to get key error when JSON string contains invalid information.
         * @param [in] jsonString the  JSON string
         * @param [in] position the position of an error in JSON string
         * @return The key error
         */
        std::string getJSonKeyError(const std::string &jsonString, int position);

        /*!
         * @brief check a string whether match pattern or not
         *
         * @param [in] pattern the regex
         * @param [in] checkedString the string to check match pattern
         * @retval true if string match pattern
         * @retval false if string doesn't match pattern
         */
        bool isMatchedPattern(const std::string &pattern, const std::string &checkedString);

        /*!
         * @brief check a string whether match pattern or not
         *
         * @param [in] pattern the regex
         * @param [in] checkedCoord the json coordinate object to verify
         * @retval true if string match pattern
         * @retval false if string doesn't match pattern
         */
        bool isMatchedCoordPattern(const std::string &pattern, const std::string &checkedCoord);

        /*!
         * @brief check a date string whether match format or not
         *
         * @param [in] format the date time format
         * @param [in] checkedString the string to check match format
         * @retval true if string match format
         * @retval false if string doesn't match format
         */
        bool isMatchedDateTimeFormat(const std::string &format, const std::string &checkedString);

    };
}

#endif /* LIBMDBJSON_MDBJSONPARSER_H_ */

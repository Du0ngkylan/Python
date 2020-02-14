/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBContentDecodingFormat.h
 * @brief       The header of MDBContentDecodingFormat class.
 * @author      NamVo: Initialize MDBContentDecodingFormat class.
 * @date        2017/04/17
 */

#ifndef LIBMDBINOUT_MDBCONTENTDECODINGFORMAT_H_
#define LIBMDBINOUT_MDBCONTENTDECODINGFORMAT_H_

#include <string>
#include <Poco/Any.h>

namespace mdbcom {

    /*!
     * @enum MDBInputKind
     * The JSON Decoding value type
     */
    enum class MDBInputKind
    {
        //! string type
        STRING,
        //! integer (64bit) type
        INTEGER64,
        //! integer (32bit) type
        INTEGER32,
        //! boolean type
        BOOL,
        //! real type
#pragma push_macro("REAL")
#undef REAL /*pgr0763*/
        REAL,
#pragma pop_macro("REAL")
        //! object type
        OBJECT,
        //! array(string) type
        ARRAY_STRING,
        //! array(integer 64bit) type
        ARRAY_INTEGER64,
        //! array(integer 32bit) type
        ARRAY_INTEGER32,
        //! array(boolean) type
        ARRAY_BOOL,
        //! array(real) type
        ARRAY_REAL,
        //! array(object) type
        ARRAY_OBJECT,
        //! coordinates type, a flexible array
        COORDINATES
    };

    /*!
     * @class MDBContentDecodingFormat
     * @brief  the validating decoding template
     */
    class MDBContentDecodingFormat {
    public:
        /*!
         * @brief constructor of MDBContentDecodingFormat.
         * @param None.
         * @return None.
         */
        MDBContentDecodingFormat(void);

        /*!
         * @brief The copy constructor of MDBContentDecodingFormat.
         * @param [in] other The other MDBContentDecodingFormat.
         * @return None.
         */
        MDBContentDecodingFormat(const MDBContentDecodingFormat &other);

        /*!
         * @brief The copy constructor of MDBContentDecodingFormat.
         * @param [in] strKey the specifying key.
         * @param [in] iDepth the hierarchy.
         * @param [in] iType the JSON Decoding value type.
         * @param [in] bflagValueCheck the flag check value.
         * @param [in] bCheckLower the minimum value check.
         * @param [in] bCheckHigher the maximum value check.
         * @param [in] bflagValidNull the flag check null.
         * @param [in] bflagValidEmpty the flag check empty.
         * @param [in] bflagValidOmit the flag check omit.
         * @param [in] bflagErrorContinue the flag check abnormal case processing.
         * @param [in] anyDefaultValue the default value.
         * @param [in] bpattern the regex to validate value
         * @return None.
         */
        MDBContentDecodingFormat(const std::string &strKey, int iDepth, MDBInputKind iType,
                                 bool bflagValueCheck, double bCheckLower, double bCheckHigher,
                                 bool bflagValidNull, bool bflagValidEmpty, bool bflagValidOmit,
                                 bool bflagErrorContinue, const Poco::Any &anyDefaultValue,
                                 const std::vector<int> &enumList,
                                 const std::string &bpattern, const std::string &bdateFormat
                                );

        /*!
         * @brief Destructor of MDBContentDecodingFormat.
         * @param None.
         * @return None.
         */
        virtual ~MDBContentDecodingFormat(void) = default;

        /*!
         * @brief copy assignment operator.
         *
         * @param [in] other The MDBContentDecodingFormat parser.
         * @return The MDBContentDecodingFormat.
         */
        MDBContentDecodingFormat &operator=(const MDBContentDecodingFormat &other);

        /*!
         * @brief get the checkHigher value
         *
         * This method is used to get the checkHigher value.
         * @param None.
         * @return  The checkHigher value
         */
        inline double getCheckHigher() const {
            return checkHigher;
        }

        /*!
         * @brief get the checkLower value
         *
         * This method is used to get the checkLower value.
         * @param None.
         * @return  The checkLower value
         */
        inline double getCheckLower() const {
            return checkLower;
        }

        /*!
         * @brief get the defaultValue value
         *
         * This method is used to get the defaultValue value.
         * @param None.
         * @return  The defaultValue value
         */
        inline const Poco::Any &getDefaultValue() const {
            return defaultValue;
        }

        /*!
         * @brief get the depth value
         *
         * This method is used to get the depth value.
         * @param None.
         * @return  The depth value
         */
        inline int getDepth() const {
            return depth;
        }

        /*!
         * @brief get the flagErrorContinue value
         *
         * This method is used to get the flagErrorContinue value.
         * @param None.
         * @return  The flagErrorContinue value
         */
        inline bool getFlagErrorContinue() const {
            return flagErrorContinue;
        }

        /*!
         * @brief get the flagValidEmpty value
         *
         * This method is used to get the flagValidEmpty value.
         * @param None.
         * @return  The flagValidEmpty value
         */
        inline bool getFlagValidEmpty() const {
            return flagValidEmpty;
        }

        /*!
         * @brief get the flagValidNull value
         *
         * This method is used to get the flagValidNull value.
         * @param None.
         * @return  The flagValidNull value
         */
        inline bool getFlagValidNull() const {
            return flagValidNull;
        }

        /*!
         * @brief get the flagValidOmit value
         *
         * This method is used to get the flagValidOmit value.
         * @param None.
         * @return  The flagValidOmit value
         */
        inline bool getFlagValidOmit() const {
            return flagValidOmit;
        }

        /*!
         * @brief get the flagValueCheck value
         *
         * This method is used to get the flagValueCheck value.
         * @param None.
         * @return  The getFlagValueCheck value
         */
        inline bool getFlagValueCheck() const {
            return flagValueCheck;
        }

        /*!
         * @brief get the key value
         *
         * This method is used to get the key value.
         * @param None.
         * @return  The key value
         */
        inline const std::string &getKey() const {
            return key;
        }

        /*!
         * @brief get the type value
         *
         * This method is used to get the type value.
         * @param None.
         * @return  The type value
         */
        inline MDBInputKind getType() const {
            return type;
        }

        /*!
         * @brief get the enum value list
         *
         * This method is used to get the enum list value.
         * @param None.
         * @return  The enum value list
         */
        inline const std::vector<int> &getEnumList() const {
            return enumList;
        }

        /*!
         * @brief get the pattern value
         *
         * This method is used to get the pattern value.
         * @param None.
         * @return  The pattern value
         */
        inline const std::string &getPattern() const {
            return pattern;
        }

        /*!
         * @brief get the date format value
         *
         * This method is used to get the date format value.
         * @param None.
         * @return  The pattern value
         */
        inline const std::string &getDateTimeFormat() const {
            return dateFormat;
        }

    private:
        //! specifying key
        std::string key;
        //! the hierarchy, 1st level is 0, child of 1st level(the 2nd) is 1, ...
        int depth;
        //! JSON Decoding value type
        MDBInputKind type;
        //! flag check value, False: Don't validate the range, True: Validate the range
        bool flagValueCheck;
        //! the minimum value check
        double checkLower;
        //! the maximum value check
        double checkHigher;
        //! flag check null, False: Can't be NULL, True: Can be NULL
        bool flagValidNull;
        //! flag check empty, False: Can't be empty, True: Can be empty
        bool flagValidEmpty;
        //! flag check Omit, False: Can't be omitted, True: Can be omitted
        bool flagValidOmit;
        //! flag check abnormal case processing, False: Stop when has error, True: Continue when has error
        bool flagErrorContinue;
        //! the default value
        Poco::Any defaultValue;
        //! list of enumeric value
        std::vector<int> enumList;
        //! the pattern
        std::string pattern;
        //! the format
        std::string dateFormat;
    };

} /* namespace mdbcom */

#endif /* LIBMDBINOUT_MDBCONTENTDECODINGFORMAT_H_ */

/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBContentDecodingFormat.cpp
 * @brief       The implement of MDBContentDecodingFormat class.
 * @author      NamVo: Initialize MDBContentDecodingFormat class.
 * @date        2016/11/01
 */

#include <MDBContentDecodingFormat.h>

namespace mdbcom {
    /*!
     * @brief constructor of MDBContentDecodingFormat.
     * @param None.
     * @return None.
     */
    MDBContentDecodingFormat::MDBContentDecodingFormat(void) :
        key { }, depth { }, type { }, flagValueCheck { }, checkLower { }, checkHigher { },
        flagValidNull { }, flagValidEmpty { }, flagValidOmit { }, flagErrorContinue { },
        defaultValue { }, enumList { }, pattern {}, dateFormat {}
    {

    }

    /*!
     * @brief The copy constructor of MDBContentDecodingFormat.
     * @param [in] other The other MDBContentDecodingFormat.
     * @return None.
     */
    MDBContentDecodingFormat::MDBContentDecodingFormat(const MDBContentDecodingFormat &other) :
        key { other.key }, depth { other.depth }, type { other.type },
        flagValueCheck { other.flagValueCheck }, checkLower { other.checkLower },
        checkHigher { other.checkHigher }, flagValidNull { other.flagValidNull },
        flagValidEmpty { other.flagValidEmpty }, flagValidOmit { other.flagValidOmit },
        flagErrorContinue { other.flagErrorContinue }, defaultValue { other.defaultValue },
        enumList { other.enumList }, pattern {other.pattern}, dateFormat {other.dateFormat}
    {
    }

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
     * @param [in] enumListVal the enum list
     * @param [in] bpattern the regex to parse node
     * @param [in] bdateFormat the date time format
     * @return None.
     */
    MDBContentDecodingFormat::MDBContentDecodingFormat(const std::string &strKey, int iDepth,
            MDBInputKind iType, bool bflagValueCheck, double bCheckLower, double bCheckHigher,
            bool bflagValidNull, bool bflagValidEmpty, bool bflagValidOmit, bool bflagErrorContinue,
            const Poco::Any &anyDefaultValue, const std::vector<int> &enumListVal, const std::string &bpattern,
            const std::string &bdateFormat):
        key { strKey }, depth { iDepth }, type { iType }, flagValueCheck { bflagValueCheck },
        checkLower { bCheckLower }, checkHigher { bCheckHigher }, flagValidNull { bflagValidNull },
        flagValidEmpty { bflagValidEmpty }, flagValidOmit { bflagValidOmit },
        flagErrorContinue { bflagErrorContinue }, defaultValue { anyDefaultValue }, enumList { enumListVal },
        pattern { bpattern }, dateFormat {bdateFormat}
    {

    }

    /*!
     * @brief copy assignment operator.
     *
     * @param [in] other The MDBContentDecodingFormat parser.
     * @return The MDBContentDecodingFormat.
     */
    MDBContentDecodingFormat &MDBContentDecodingFormat::operator=(const MDBContentDecodingFormat &other)
    {
        if (this != &other)
        {
            this->checkHigher = other.checkHigher;
            this->checkLower = other.checkLower;
            this->defaultValue = other.defaultValue;
            this->depth = other.depth;
            this->flagErrorContinue = other.flagErrorContinue;
            this->flagValidEmpty = other.flagValidEmpty;
            this->flagValidNull = other.flagValidNull;
            this->flagValidOmit = other.flagValidOmit;
            this->flagValueCheck = other.flagValueCheck;
            this->key = other.key;
            this->type = other.type;
            this->enumList = other.enumList;
            this->pattern = other.pattern;
            this->dateFormat = other.dateFormat;
        }

        return *this;
    }

} /* namespace mdbcom */

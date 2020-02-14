/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDatabaseParam.cpp
 * @brief       Implement database parameter wrapper type
 * @author      cuongadp
 * @date        2017/04/10
 */

#include "MDBDatabaseParam.h"

namespace mdbcom {

    /*!
     * @brief Default constructor
     *
     * @param None.
     *
     * @return None.
     */
    MDBDatabaseParam::MDBDatabaseParam(void) : /* pgr0571 */ /* pgr2266 */
        paramValue { nullptr },
        paramLength { 0 },
        paramFormat { FORMAT_TEXT }
    {
        // null parameter
    };

    /*!
     * @brief Default constructor
     *
     * @param [in] param Text value of parameter
     *
     * @return None.
     */
    MDBDatabaseParam::MDBDatabaseParam(const std::string &param) : /* pgr0571 */ /* pgr2266 */
        MDBDatabaseParam { param.c_str(), param.length() + 1, FORMAT_TEXT }
    {
    };

    /*!
     * @brief Copy constructor
     *
     * @param [in] other Other param value
     *
     * @return None.
     */
    MDBDatabaseParam::MDBDatabaseParam(const MDBDatabaseParam &other) :  /* pgr0571 */ /* pgr2266 */
        MDBDatabaseParam { other.paramValue.get(), other.paramLength, other.paramFormat }
    {
    };

    /*!
     * @brief Move constructor
     *
     * @param [in/out] other Other param value
     *
     * @return None.
     */
    MDBDatabaseParam::MDBDatabaseParam(MDBDatabaseParam &&other) :
        paramValue { std::move(other.paramValue) },
        paramLength { other.paramLength },
        paramFormat { other.paramFormat }
    {
    };

    /*!
     * @brief data constructor
     *
     * @param [in] data Binary Data
     * @param [in] length Length of binary data
     * @param [in] format Format type of parameter
     *
     * @return None.
     */
    MDBDatabaseParam::MDBDatabaseParam(const char *data, unsigned long length, int format) : /* pgr0571 */
        paramValue { nullptr },
        paramLength { length },
        paramFormat { format }
    {
        if (Result::OK != constructFrom(data, length, format))
        {
            throw MDBException {"Invalid input for binary data."};
        }
    };

    /*!
     * @brief Move constructor
     *
     * @param [in] data Text Data
     *
     * @return None.
     */
    MDBDatabaseParam::MDBDatabaseParam(char *data) :  /* pgr0571 */
        paramValue { nullptr },
        paramLength { 0 },
        paramFormat { FORMAT_TEXT }
    {
        if (nullptr == data)
        {
            throw MDBException {"Invalid input for text data. [data=null]"};
        }

        auto length = std::strlen(data); /* pgr0745 */ /* pgr0541 */

        if (Result::OK != constructFrom(data, length + 1, FORMAT_TEXT))
        {
            throw MDBException {"Can't not construct from c string."};
        }
    };

    /*!
     * @brief Move constructor
     *
     * @param [in] data Text Data
     *
     * @return None.
     */
    MDBDatabaseParam::MDBDatabaseParam(const char *data) :  /* pgr0571 */
        MDBDatabaseParam(const_cast<char *>(data))
    {
    }

    /*!
     * @brief Destructor
     *
     * @param None.
     *
     * @return None.
     */
    MDBDatabaseParam::~MDBDatabaseParam(void) = default;

    /*!
    * @brief Move constructor
    *
    * @param [in] other Other param value
    *
    * @return Reference to current object
    */
    MDBDatabaseParam &MDBDatabaseParam::operator = (const MDBDatabaseParam &other)
    {
        if (this != &other)
        {
            paramFormat = other.paramFormat;
            paramLength = other.paramLength;
            paramValue = nullptr;

            if (Result::OK != constructFrom(other.paramValue.get(), other.paramLength, other.paramFormat))
            {
                throw MDBException {"Error copying object"};
            }
        }

        return *this;
    }

    /*!
    * @brief Move constructor
    *
    * @param [in/out] other Other param value
    *
    * @return Reference to current object
    */
    MDBDatabaseParam &MDBDatabaseParam::operator = (MDBDatabaseParam &&other)
    {
        paramValue = std::move(other.paramValue);
        paramLength = other.paramLength;
        paramFormat = other.paramFormat;
        return *this;
    }

    /*!
      * @brief Construct parameter from binary
      *
      * @param [in] data Binary Data
      * @param [in] length Length of binary data
      * @param [in] format Format type of parameter
      *
      * @retval Result::OK success
      * @retval Result::NG Error
      */
    int MDBDatabaseParam::constructFrom(const char *data, unsigned long length, int format)
    {

        if ((0 == length)
                && (nullptr != data))
        {
            mdb_apl_log_critical("Invalid param data=%p length=%u", data, length);
            return Result::NG;
        }

        paramFormat = format;
        paramLength = length;
        paramValue = nullptr;

        if ((0 != length)
                && (nullptr != data))
        {
            try
            {
                paramValue = MDBMemoryUtility::make_unique<char []> (paramLength); /* pgr0694 */
            }
            catch (std::bad_alloc &)
            {
                mdb_apl_log_critical("Can't allocate due to insufficient memory");
                return Result::NG;
            }
            catch (...)
            {
                mdb_apl_log_critical("Can't allocate due to unknown error");
                return Result::NG;
            }

            static_cast<void>(std::memcpy(paramValue.get(), data, paramLength)); /* pgr0579 */ /* pgr2215 */ /* pgr1170 */ /*pgr0541*/
        }

        return Result::OK;
    }

    /*!
     * @brief Get param value
     *
     * @param None.
     *
     * @return pointer to param's value
     */
    const char *MDBDatabaseParam::getParamValue(void) const
    {
        return paramValue.get();
    };

    /*!
     * @brief Get param length
     *
     * @param None.
     *
     * @return length of parameter
     */
    int MDBDatabaseParam::getParamLength(void) const
    {
        return paramLength;
    }

    /*!
     * @brief Get param format
     *
     * @param None.
     *
     * @return Format of parameter
     */
    int MDBDatabaseParam::getParamFormat(void) const
    {
        return paramFormat;
    }
}

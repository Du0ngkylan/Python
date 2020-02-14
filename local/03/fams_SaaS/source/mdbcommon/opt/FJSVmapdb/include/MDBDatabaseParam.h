/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBDatabaseParam.h
 * @brief       Header file for SQL parameter type
 * @author      cuongadp
 * @date        2017/04/10
 */

#ifndef LIBMDBDB_MDBDATABASEPARAM_H_
#define LIBMDBDB_MDBDATABASEPARAM_H_
#include <type_traits>
#include <iomanip>
#include <sstream>
#include "MDBException.h"
#include "MDBLogger.h"
#include "MDBMemoryUtility.h"

namespace mdbcom {
    /*! @class  MDBDatabaseParam.
     *  @brief The class supports setting SQL parameters
     */
    class MDBDatabaseParam { /* pgr1233 */
        /* pgr2205 */

    public:

        /*!
         * @brief Default constructor
         *
         * @param None.
         *
         * @return None.
         */
        MDBDatabaseParam(void);

        /*!
         * @brief Default constructor
         * Allow directly declare
         *  MDBDatabaseParam p = 1string { "test" };
         *
         * @param [in] param Text value of parameter
         *
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        MDBDatabaseParam(const std::string &param);

        /*!
         * @brief array constructor
         * Allow directly declare
         *  MDBDatabaseParam p = array;
         *
         * @param [in] dataList a list to convert to array type
         * @param [in] addQuote flag indicate add quote to string after converted
         *
         * @return None.
         */
        template<typename T>
        MDBDatabaseParam(const std::vector<T> &dataList) : /* pgr0309*/ /*pgr0745*/
            paramValue {nullptr},
            paramLength {0},
            paramFormat {FORMAT_TEXT} {
            if (0 == dataList.size()) {
                mdb_apl_log_debug("No element in list");
            }
            else {
                std::string arrayString; //convert list to '1,2,3,4'
                std::stringstream ss;

                for (auto &data : dataList) { /*pgr0745*/
                    ss << std::fixed << std::setprecision(6) << data; /*pgr0672*/
                    ss << ',';
                }

                arrayString = ss.str();
                arrayString.pop_back(); // remove last comma

                if (Result::OK != constructFrom(arrayString.c_str(), arrayString.length() + 1, FORMAT_TEXT)) { /*pgr0512*/
                    throw MDBException {"Invalid input for binary data."};
                }
            }
        }

        /*!
         * @brief Data constructor
         * Allow directly declare
         *  MDBDatabaseParam p = 1;
         *  MDBDatabaseParam p = 1f;
         *
         * @param [in] param Numeric value of parameter
         *
         * @return None.
         */
        template<class NumberType>
        // cppcheck-suppress noExplicitConstructor
        MDBDatabaseParam(NumberType param) : /* pgr0309 */ /* pgr0745 */
            paramValue { nullptr },
            paramLength { 0 },
            paramFormat { FORMAT_TEXT } {
            if ((true == std::is_integral<NumberType>::value)
                    || (true == std::is_floating_point<NumberType>::value)) {
                auto value = std::to_string(param); /* pgr0745 */
                auto str = value.c_str(); /* pgr0745 */
                auto len = value.length(); /* pgr0745 */

                if (Result::OK != constructFrom(str, len + 1, FORMAT_TEXT)) {
                    throw MDBException {"Error creating object from number"};
                }
            }
            else {
                throw MDBException {"Input type is not number."};
            }
        };

        /*!
         * @brief Copy constructor
         *
         * @param [in] other Other param value
         *
         * @return None.
         */
        MDBDatabaseParam(const MDBDatabaseParam &other);

        /*!
         * @brief Move constructor
         *
         * @param [in/out] other Other param value
         *
         * @return None.
         */
        MDBDatabaseParam(MDBDatabaseParam &&other);

        /*!
         * @brief data constructor
         *
         * @param [in] data Binary Data
         * @param [in] length Length of binary data
         * @param [in] format Format type of parameter
         *
         * @return None.
         */
        MDBDatabaseParam(const char *data, unsigned long length, int format = FORMAT_BINARY);

        /*!
         * @brief data constructor
         * Allow directly declare
         *  MDBDatabaseParam p = "string";
         *
         * @param [in] data Binary Data
         *
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        MDBDatabaseParam(char *data);

        /*!
         * @brief data constructor
         * Allow directly declare
         *  MDBDatabaseParam p = "string";
         *
         * @param [in] data Binary Data
         *
         * @return None.
         */
        // cppcheck-suppress noExplicitConstructor
        MDBDatabaseParam(const char *data);

        /*!
         * @brief Destructor
         *
         * @param None.
         *
         * @return None.
         */
        ~MDBDatabaseParam(void);

        /*!
         * @brief Move constructor
         *
         * @param [in] other Other param value
         *
         * @return Reference to current object
         */
        MDBDatabaseParam &operator = (const MDBDatabaseParam &other);

        /*!
         * @brief Move constructor
         *
         * @param [in/out] other Other param value
         *
         * @return Reference to current object
         */
        MDBDatabaseParam &operator = (MDBDatabaseParam &&other);

        /*!
         * @brief Get param value
         *
         * @param None.
         *
         * @return pointer to param's value
         */
        const char *getParamValue(void) const ;

        /*!
         * @brief Get param length
         *
         * @param None.
         *
         * @return length of parameter
         */
        int getParamLength(void) const;

        /*!
         * @brief Get param format
         *
         * @param None.
         *
         * @return Format of parameter
         */
        int getParamFormat(void) const;

        //! Format value of prosgres text
        static constexpr int FORMAT_TEXT = 0;
        //! Format value of prosgres text
        static constexpr int FORMAT_BINARY = 1;
    private:

        //! pointer to value
        std::unique_ptr<char []> paramValue; /* pgr0694 */
        //! Param length
        unsigned long paramLength;
        //! Posgre param format, 0 for text and 1 for binary (not supported)
        int paramFormat;

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
        int constructFrom(const char *data, unsigned long length, int format);
    };
}

#endif /* LIBMDBDB_MDBDATABASEPARAM_H_ */

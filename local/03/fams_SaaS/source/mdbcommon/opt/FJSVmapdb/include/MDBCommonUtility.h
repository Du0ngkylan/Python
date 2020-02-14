/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBCommonUtility.h
 * @brief       Header file for define MDBCommonUtility functions.
 * @author      cuongadp: Support common operation.
 * @date        2016/12/07
 */

#ifndef LIBMDBUTIL_MDBCOMMONUTILITY_H_
#define LIBMDBUTIL_MDBCOMMONUTILITY_H_

#include <vector>

namespace mdbcom {
    class MDBCommonUtility { /* pgr2205 */
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        MDBCommonUtility(void) = delete;

        /*!
         * @brief Destructor.
         * @param None.
         * @return None.
         */
        ~MDBCommonUtility(void) = delete;

        /*!
         * @brief Copy Constructor.
         * @param None.
         * @return None.
         */
        MDBCommonUtility(const MDBCommonUtility &) = delete;

        /*!
         * @brief Copy assignment.
         * @param None.
         * @return None.
         */
        MDBCommonUtility &operator=(const MDBCommonUtility &) = delete; /* pgr0309 */ /* pgr0055 */

        /*!
         * @brief Move Constructor.
         * @param None.
         * @return None.
         */
        MDBCommonUtility(MDBCommonUtility &&) = delete;

        /*!
         * @brief move assignment.
         * @param None.
         * @return None.
         */
        MDBCommonUtility &operator=(MDBCommonUtility &&) = delete; /* pgr0309 */ /* pgr0055 */

        /*!
         * @brief  sleep thread in nano second
         *
         *
         * @param [in] timeSecond time in seconde
         * @param [in] timeNanoSecond time in nanosecond (must be in the range 0 to 999999999)
         *
         * @retval Result::NG sleep ok
         * @retval Result::OK sleep failed
         *
         */
        static int sleepNano(int timeSecond, long timeNanoSecond);

        /*!
         * @brief  Compress data
         *
         *
         * @param [in] fileName Name of file to compress.
         * @param [in] data Compressed data.
         *
         * @retval Result::NG compress ok
         * @retval Result::OK compress failed
         *
         */
        static int compressData(const std::string &fileName, std::vector<char> &data);

        /*!
         * @brief  Compress data
         *
         *
         * @param [in] dataIn The data do not compress.
         * @param [in] dataOut The data is compressed.
         *
         * @retval Result::NG compress ok
         * @retval Result::OK compress failed
         *
         */
        static int compressData(const std::vector<char> &dataIn, std::vector<char> &dataOut);

        /*!
         * @brief  Decompress data
         *
         *
         * @param [in] dataIn The data is compressed.
         * @param [in] dataOut The data do not compress.
         *
         * @retval Result::NG decompress ok
         * @retval Result::OK decompress failed
         *
         */
        static int decompressData(const std::vector<char> &dataIn, std::vector<char> &dataOut);

    private:
        //! Fix size of compress buffer
        static constexpr int BUFFER_SIZE = 512;
    };
}
#endif /* LIBMDBUTIL_MDBCOMMONUTILITY_H_ */

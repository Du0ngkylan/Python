/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBCommonUtility.cpp
 * @brief       Source code for defined utility functions.
 * @author      trangvp: create it for supporting thread utility class.
 * @date        2016/10/04
 */

#include <unistd.h>

#include <iostream>
#include <fstream>
#include <array>
#include <sstream>

#include "MDBCommonUtility.h"
#include "MDBLogger.h"
#include "MDBConstants.h"
#include <zlib.h>

namespace mdbcom {
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
    int MDBCommonUtility::sleepNano(int timeSecond, long timeNanoSecond)
    {
        // Sleep function should not produce much log even in debug
        auto ret = Result::NG; /* pgr0745 */
        auto delay = timespec {timeSecond, timeNanoSecond}; /* pgr0745 */
        auto remain = timespec {0, 0}; /* pgr0745 */
        auto errBuf = std::array<char, Buffer::STR_ERROR_BUFFER_LENGTH> {'\0'}; /* pgr0745 */

        while (0 > ret)
        {
            errno = 0;
            ret = nanosleep(&delay, &remain);

            if (EINTR != errno)
            {
                break;
            }
            else
            {
                // If sleep was interupted by signal then continue sleep with remain time
                mdb_apl_log_trace("nanosleep is paused by signal");
                delay = remain;
            }
        }

        if (0 > ret)
        {
            auto errnoTmp = errno; /* pgr0745 */
            auto errBufData = errBuf.data(); /* pgr0745 */
            auto errBufSize = errBuf.size(); /* pgr0745 */
            auto errMsg = strerror_r(errno, errBufData, errBufSize); /* pgr0745 */
            mdb_apl_log_warn("nanosleep is failed, errno = [%d], strerror = [%s]", errnoTmp,
                             errMsg);
            return Result::NG;
        }

        return Result::OK;
    }

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
    int MDBCommonUtility::compressData(const std::vector<char> &dataIn, std::vector<char> &dataOut)
    {
        mdb_apl_log_start();

        auto ret = Result::OK; /* pgr0745 */
        //convert string dataIn to char array dataIn
        auto data = dataIn.data(); /* pgr0745 */
        auto lengthData = dataIn.size(); /* pgr0745 */
        char tempBuffer[BUFFER_SIZE] = {0}; /*pgr2031*/

        // Initialize compress
        z_stream defstream; /*pgr1163*/
        defstream.zalloc = Z_NULL;
        defstream.zfree = Z_NULL;
        // size of input
        defstream.avail_in = lengthData;
        // input char array
        defstream.next_in = (uint8_t *)data;
        // size of output
        defstream.avail_out = BUFFER_SIZE;
        // output char array
        defstream.next_out = (uint8_t *)tempBuffer;

        //Initializes the internal stream state for compression
        auto retcode = deflateInit(&defstream, Z_BEST_COMPRESSION); /* pgr0745 */

        if (Z_OK == retcode)
        {
            auto deflateRes = Z_OK; /* pgr0745 */

            // Compress and add package data into buffer when data in is not empty and have package data is compressed
            while (0 != defstream.avail_in)
            {
                deflateRes = deflate(&defstream, Z_NO_FLUSH);

                if (Z_OK == deflateRes)
                {
                    if (0 == defstream.avail_out)
                    {
                        //add data is compressed to vector buffer and set new buffer for package data next
                        dataOut.insert(dataOut.end(), tempBuffer, tempBuffer + BUFFER_SIZE);
                        defstream.next_out = (uint8_t *)tempBuffer;
                        defstream.avail_out = BUFFER_SIZE;
                    }
                }
            }

            //Compress and add package data into buffer when data in is empty and have package data is compressed
            do
            {
                deflateRes = deflate(&defstream, Z_FINISH);

                if (Z_OK == deflateRes)
                {
                    if (0 == defstream.avail_out)
                    {
                        //add data is compressed to vector buffer and set new buffer for package data next
                        dataOut.insert(dataOut.end(), tempBuffer, tempBuffer + BUFFER_SIZE);
                        defstream.next_out = (uint8_t *)tempBuffer;
                        defstream.avail_out = BUFFER_SIZE;
                    }
                }

            }
            while (Z_OK == deflateRes);

            dataOut.insert(dataOut.end(), tempBuffer, tempBuffer + BUFFER_SIZE - defstream.avail_out);

            if (Z_OK != deflateEnd(&defstream))
            {
                mdb_apl_log_critical(" End compress is failed");
                ret = Result::NG;
            }
        }
        else
        {
            mdb_apl_log_critical("Initialize deflate failed");
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }

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
    int MDBCommonUtility::compressData(const std::string &fileName, std::vector<char> &data)
    {
        mdb_apl_log_start();
        auto fileData = std::vector<char> { }; /*pgr0745*/
        std::ifstream file { fileName, std::ios::ate | std::ifstream::binary};

        if (!file.eof() && !file.fail())
        {
            auto fileSize = file.tellg(); /*pgr0745*/
            fileData.resize(fileSize);
            auto fileDataSize = fileData.size(); /*pgr0745*/
            mdb_apl_log_trace("File size : %llu", fileDataSize);

            file.seekg(0, std::ios_base::beg);
            file.read(&fileData[0], fileSize);
            auto result = compressData(fileData, data); /*pgr0745*/

            auto compressSize = data.size(); /*pgr0745*/
            mdb_apl_log_trace("Compressed size : %llu", compressSize);
            mdb_apl_log_end();
            return result;
        }

        mdb_apl_log_end();
        return Result::NG;
    }

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
    /** cppcheck-suppress unusedFunction */
    int MDBCommonUtility::decompressData(const std::vector<char> &dataIn, std::vector<char> &dataOut)
    {
        mdb_apl_log_start();
        //convert string dataIn to char array dataIn
        auto &buffer = dataOut; /*pgr0745*/
        const char *data = dataIn.data();
        auto lengthData = dataIn.size(); /*pgr0745*/
        char tempBuffer[BUFFER_SIZE] = {0};
        auto ret = Result::OK; /*pgr0745*/

        // Initialize compress
        z_stream infstream; /*pgr1163*/
        infstream.zalloc = Z_NULL;
        infstream.zfree = Z_NULL;
        // size of input
        infstream.avail_in = lengthData;
        // input char array
        infstream.next_in = (uint8_t *)data;
        // size of output
        infstream.avail_out = BUFFER_SIZE;
        // output char array
        infstream.next_out = (uint8_t *)tempBuffer;

        auto retcode = inflateInit(&infstream); /*pgr0745*/

        if (Z_OK == retcode)
        {
            auto inflateRes = Z_OK; /*pgr0745*/

            while (inflateRes == Z_OK)
            {
                inflateRes = inflate(&infstream, Z_FINISH);

                if (Z_OK == inflateRes)
                {
                    if (0 == infstream.avail_out)
                    {
                        buffer.insert(buffer.end(), tempBuffer, tempBuffer + BUFFER_SIZE);
                        infstream.next_out = (uint8_t *)tempBuffer;
                        infstream.avail_out = BUFFER_SIZE;
                    }
                }

            }

            buffer.insert(buffer.end(), tempBuffer, tempBuffer + BUFFER_SIZE - infstream.avail_out);

            if (Z_OK != inflateEnd(&infstream))
            {
                mdb_apl_log_critical(" End decompress is failed");
                ret = Result::NG;
            }
        }
        else
        {
            mdb_apl_log_critical("Initialize deflate failed");
            ret = Result::NG;
        }

        mdb_apl_log_end();
        return ret;
    }
}

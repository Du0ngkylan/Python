/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBFileChannel.cpp
 * @brief   Extend function of FileChannel
 * @author  trunghn:
 * @date    2016/10/10
 */

#include <Poco/File.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Path.h>

#include "MDBFileChannel.h"

namespace mdbcom {
    //! Default setting value for archive prop
    const std::string MDBFileChannel::PROP_ARCHIVE_VALUE
    {
        "timestamp"
    };
    //! Default setting value for times prop
    const std::string MDBFileChannel::PROP_TIMES_VALUE
    {
        "local"
    };

    /*!
     * @brief Constructor with file path
     *
     * Initialize FileChannel to specified path
     *
     * @param [in] size Maximum log size
     * @param [in] path Log file path
     * @param [in] storeDir Directory to store rotated files
     * @return None
     */
    MDBFileChannel::MDBFileChannel(unsigned int size, const std::string &path,
                                   const std::string &storeDir) :
        Poco::FileChannel { path }, maxSize { size }, backupStrategy { path, storeDir }
    {

        if (0 == size)
        {
            throw Poco::InvalidArgumentException { "Max file size must be greater than 0" };
        }

        auto storeDirFile = Poco::File { storeDir }; /* pgr0745 */

        if (false == storeDirFile.exists())
        {
            throw Poco::InvalidArgumentException { "Storage directory not exists" };
        }

        auto logFile = Poco::Path { path }; /* pgr0745 */
        auto logDirectory = logFile.parent().toString(); /* pgr0745 */
        auto logDirFile = Poco::File { logDirectory }; /* pgr0745 */

        if (false == logDirFile.exists())
        {
            throw Poco::InvalidArgumentException { "Log directory not exists" };
        }

        maxSize <<= 20; // 1024 * 1024 * maxSize
        auto value = Poco::NumberFormatter::format(size); /* pgr0745 */
        // specify unit is MB
        value += "M";

        Poco::FileChannel::setProperty(PROP_PATH, path);
        Poco::FileChannel::setProperty(PROP_ROTATION, value);
        Poco::FileChannel::setProperty(PROP_ARCHIVE, PROP_ARCHIVE_VALUE);
        Poco::FileChannel::setProperty(PROP_TIMES, PROP_TIMES_VALUE);
    }

    /*!
     * @brief Log message to file
     *
     * Output message to file
     *
     * @param [in] msg Message structure
     * @return None
     */
    void MDBFileChannel::log(const Poco::Message &msg)
    {
        auto move = (maxSize <= size()); /* pgr0360 *//* pgr0745 */

        Poco::FileChannel::log(msg);

        if (true == move)
        {
            backupStrategy.backupAsync();
        }
    } /* pgr0840 */

} // end of namespace

/* COPYRIGHT 2016 FUJITSU LIMITED */
/*!
 * @file MDBLogStart.cpp
 * @brief Implementation for Initializing level for debug log in shared memory
 * @author cuongadp:
 * @date 2016/11/22
 */

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#include <MDBLogConfiguration.h>

int main(void)
{
    const std::string SUCCESS {"Debug log level is loaded to shared memory"};
    const std::string ERROR {"Can't load debug log level to shared memory"};
    const std::string CONFIG_FILE {"/opt/FJSVmapdb/etc/debug_level.conf"};

    static_cast<void>(umask(0007));

    // load config file and map to sharemem
    if (mdbcom::Result::NG == mdbcom::MDBLogConfiguration::loadConfigFile(CONFIG_FILE))
    {
        std::cout << ERROR << std::endl;
        return mdbcom::ExitCode::NG;
    }

    std::cout << SUCCESS << std::endl;
    return mdbcom::ExitCode::OK;
}

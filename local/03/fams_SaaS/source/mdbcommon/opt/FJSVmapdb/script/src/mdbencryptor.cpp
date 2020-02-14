/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file mdbencryptor.cpp
 * @brief Implementation for encrypting string
 * @author trangvp:
 * @date 2016/11/02
 */

#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#include "MDBLogger.h"
#include "MDBApplicationUtility.h"
#include "MDBConstants.h"
#include "MDBCryptoUtility.h"

int main(int argc, char const *argv[])
{
    const std::string ERR_INIT {"[mdbencryptor] Can't init application"};
    const std::string MAIN_THREAD_NAME {"main"};

    static_cast<void>(umask(0007));

    std::ifstream comm { mdbcom::Process::NAME_PATH };
    auto processName = std::string { };  /* pgr0745 */
    static_cast<void>(std::getline(comm, processName));

    static_cast<void>(mdbcom::MDBLogger::initLog(processName));
    static_cast<void>(mdbcom::MDBLogger::openDebugLog(MAIN_THREAD_NAME));

    mdb_apl_log_start();
    static_cast<void>(argc);

    if (!getline(comm, processName))
    {
        processName = argv[0]; /* pgr0541 */
    }

    if (mdbcom::Result::NG == mdbcom::MDBApplicationUtility::initApplication(processName, nullptr))
    {
        std::cerr << ERR_INIT << std::endl;
        return mdbcom::ExitCode::NG;
    }

    mdb_apl_log_start();

    auto tty = termios { IGNBRK, OPOST, CSIZE, ISIG, 0, { 0, 0, 0 }, 0, 0 };  /* pgr0745 */

    if (0 != tcgetattr(STDIN_FILENO, &tty))
    {
        mdb_apl_log_critical("Failure");
        mdb_apl_log_end();
        mdbcom::MDBApplicationUtility::unInitApplication();
        return mdbcom::ExitCode::NG;
    }

    auto newtty = tty; /* pgr0745 */

    newtty.c_lflag &= ~ECHO;
    newtty.c_lflag &= ~ICANON;

    if (0 != tcsetattr(STDIN_FILENO, TCSANOW, &newtty))
    {
        mdb_apl_log_critical("Failure");
        mdb_apl_log_end();
        mdbcom::MDBApplicationUtility::unInitApplication();
        return mdbcom::ExitCode::NG;
    }

    auto ret = mdbcom::ExitCode::OK; /* pgr0745 */
    // get user input' password
    std::cerr << "Enter the password: ";
    auto input = std::string { };  /* pgr0745 */
    // return value is same with input std::cin
    /// cppcheck-suppress variableScope
    char inputChar = '\0';

    while (true)
    {
        inputChar = std::cin.get();

        if (std::iscntrl(inputChar))
        {
            if (inputChar != '\n')
            {
                mdb_apl_log_critical("Input user is control character but NOT ENTER key");
                ret = mdbcom::ExitCode::NG;
                std::cerr << std::endl;
                std::cerr << "Invalid user input." << std::flush; /*pgr0672*/
            }

            break;
        }
        else
        {
            input += inputChar;
        }
    }

    std::cerr << std::endl;
    std::string encryptedText { };

    if (mdbcom::Result::OK == ret)
    {
        // Call Encrypt Util
        if (mdbcom::Result::OK != mdbcom::MDBCryptoUtility::encryptAES256(input, encryptedText))
        {
            mdb_apl_log_critical("encryptAES256 wrong");
            ret = mdbcom::ExitCode::NG;
        }
        else
        {
            std::cerr << "Success: " << std::flush; /*pgr0672*/
            std::cout << encryptedText << std::flush; /*pgr0672*/
            std::cerr << std::endl;
        }

    }

    // Reset show character of console
    if (0 != tcsetattr(STDIN_FILENO, TCSANOW, &tty))
    {
        mdb_apl_log_critical("Reset show character wrong");
        ret = mdbcom::ExitCode::NG;
    }

    mdbcom::MDBApplicationUtility::unInitApplication();
    mdb_apl_log_end();
    return ret;
}


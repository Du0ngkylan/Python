/* COPYRIGHT 2016 FUJITSU LIMITED */
/*!
 * @file mdblogconfig get.cpp
 * @brief Get log level
 * @author cuongadp:
 * @date 2016/11/22
 */
#include <iomanip>
#include <atomic>
#include <iostream>
#include <fstream>
#include <string>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <Poco/Util/Application.h>

#include "MDBDebugLevelMap.h"
#include "MDBLogConfiguration.h"
#include "MDBLogger.h"
#include "MDBConstants.h"
#include "MDBApplicationUtility.h"
#include "MDBApplication.h"

const std::string ALL_FUNCTION /*pgr1003*/ /*pgr0687*/ /*pgr2218*/
{
    "全機能"
};
//! command
const std::string CMD_HELP /*pgr1003*/ /*pgr0687*/
{
    "help"
};
const std::string CMD_SHOW {"show"}; /*pgr1003*/ /*pgr0687*/
const std::string CMD_SET {"set"}; /*pgr1003*/ /*pgr0687*/
//! message
const std::string HELP_MESSAGE /*pgr1003*/ /*pgr0687*/
{
    "\
使用方法: mdbdbglog  <サブコマンド> [<オプション>] [<引数>]\n\
\n\
利用可能なサブコマンド:\n\
  help: ヘルプを表示します。\n\
    使用方法: mdbdbglog  help\n\
\n\
  set: デバッグログレベルを変更します。\n\
    使用方法: mdbdbglog  set <オプション> [<引数>]\n\
\n\
    有効なオプション:\n\
      -l <ARG>         : デバッグログレベルを <ARG> に指定します\n\
                         0～2を指定可能です\n\
                         数字が大きくなるほど出力されるログの量が多くなります\n\
                         必ず指定が必要なオプションとなります\n\
      -f <ARG>         : デバッグログレベルを変更する機能名を <ARG> に指定します\n\
                         省略時は全機能のデバッグログレベルを変更します\n\
\n\
  show: デバッグログレベルを参照します。\n\
    使用方法: mdbdbglog  show <オプション> [<引数>]\n\
\n\
    有効なオプション:\n\
      -f <ARG>         : デバッグログレベルを参照する機能名を <ARG> に指定します\n\
                         省略時は全機能のデバッグログレベルを参照します\n"
};

const std::string SHOW_MESSAGE_FUNCTION {"機能名"}; /*pgr1003*/ /*pgr0687*/
const std::string SHOW_MESSAGE_LEVEL {"デバッグログレベル"}; /*pgr1003*/ /*pgr0687*/

const std::string ERR_LEVEL {"指定されたデバッグログレベルが誤っています。"}; /*pgr1003*/ /*pgr0687*/
const std::string ERR_NAME {"指定された機能名が誤っています。"}; /*pgr1003*/ /*pgr0687*/
const std::string ERR_REEXECUTE {"実行したコマンドが二重起動です。"}; /*pgr1003*/ /*pgr0687*/

const std::string SET_ERR {"デバッグログ設定変更コマンドが失敗しました。"}; /*pgr1003*/ /*pgr0687*/
const std::string SHOW_ERR {"デバッグログ設定表示コマンドが失敗しました。"}; /*pgr1003*/ /*pgr0687*/
const std::string MAIN_THREAD_NAME {"main"}; /*pgr0687*/ /*pgr1003*/

int main(int argc, char *const argv[])
{
    static_cast<void>(umask(0007));

    if (1 == argc)
    {
        std::cout << HELP_MESSAGE << std::endl;
        return mdbcom::Result::NG;
    }

    std::ifstream comm { mdbcom::Process::NAME_PATH };
    auto processName = std::string { }; /*pgr0745*/
    static_cast<void>(std::getline(comm, processName));

    static_cast<void>(mdbcom::MDBLogger::initLog(processName));
    static_cast<void>(mdbcom::MDBLogger::openDebugLog(MAIN_THREAD_NAME));

    mdb_apl_log_start();
    auto ret = mdbcom::Result::NG; /*pgr0745*/
    auto cmd = argv[1]; /*pgr0745*/ /*pgr0541*/
    auto functionName = std::string {}; /*pgr0745*/
    auto dblogLevel = std::string {}; /*pgr0745*/
    auto opt = std::string {}; /*pgr0745*/
    auto optIndex = 2; /*pgr0745*/

    while (optIndex < argc) /*pgr0697*/
    {
        opt = argv[optIndex];

        if ((opt == "-l") && ((optIndex + 1) < argc))
        {
            // next arg is value
            dblogLevel = argv[++optIndex]; /*pgr0871*/
        }
        else if ((opt == "-f") && ((optIndex + 1) < argc))
        {
            // next arg is value
            functionName = argv[++optIndex]; /*pgr0871*/
        }
        else
        {
            //no implement for pgr0607
        }

        // next arg
        optIndex++;
    }

    // missing value for options
    if (0 != (optIndex % 2))
    {
        std::cout << HELP_MESSAGE << std::endl;
        return mdbcom::Result::NG;
    }

    if (CMD_HELP == cmd)
    {
        std::cout << HELP_MESSAGE << std::endl;
    }
    else if (CMD_SHOW == cmd)
    {
        auto levelList = std::vector<mdbcom::MDBDebugLevelEntry *> {}; /*pgr0745*/
        auto result = 0; /*pgr0745*/

        if (mdbcom::Result::OK == mdbcom::MDBLogConfiguration::showLogLevel(functionName, levelList, result))
        {
            std::cout << std::left /*pgr0672*/
                      << std::setfill(' ') /*pgr0512*/
                      << std::setw(34) /*pgr0512*/
                      << SHOW_MESSAGE_FUNCTION
                      << std::left /*pgr0672*/
                      << std::setw(19) /*pgr0512*/
                      << SHOW_MESSAGE_LEVEL
                      << std::endl;
            std::cout << std::setfill('-') << std::setw(30) /*pgr0512*/
                      << '-' /*pgr0657*/
                      << '+' /*pgr0657*/
                      << std::setfill('-') << std::setw(18) /*pgr0512*/
                      << '-' /*pgr0657*/
                      << std::endl;

            for (auto entry : levelList)
            {
                std::cout << std::setfill(' ')
                          << std::left /*pgr0672*/
                          << std::setw(30)
                          << entry->function.data()
                          << std::right /*pgr0672*/
                          << std::setw(19)
                          << entry->level
                          << std::endl;
            }

            ret = mdbcom::Result::OK;
        }
        else
        {
            switch (result)
            {
                case mdbcom::MDBCommandResult::ERROR_NOTFOUND: /*pgr2031*/
                    std::cout << ERR_NAME << std::endl;
                    break;

                case mdbcom::MDBCommandResult::ERROR_DOUBLE_EXEC: /*pgr2031*/
                    std::cout << ERR_REEXECUTE << std::endl;
                    break;

                default:
                    std::cout << SHOW_ERR << std::endl;
                    break;
            }
        }
    }
    else if (CMD_SET == cmd)
    {
        // calling set without param or without level
        if ((2 == argc) || (true == dblogLevel.empty()))
        {
            std::cout << HELP_MESSAGE << std::endl;
            return mdbcom::Result::NG;
        }

        auto level = 0; /*pgr0745*/

        try
        {
            level = std::stol(dblogLevel.c_str());
        }
        catch (std::invalid_argument &invalidEx) /*pgr0305*/
        {
            std::fprintf(stdout, "mdbdbglog invalid argument exception "); /*pgr0579*/ /*pgr1170*//*pgr0519*/
            level = -1;
        }
        catch (std::out_of_range &rangeEx) /*pgr0305*/
        {
            std::fprintf(stdout, "mdbdbglog out of range exception"); /*pgr0579*/ /*pgr1170*//*pgr0519*/
            level = -1;
        }
        catch (...)
        {
            level = -1;
        }

        if (mdbcom::Result::OK == mdbcom::MDBLogConfiguration::setLogLevel(functionName, level))
        {

            if (true == functionName.empty())
            {
                functionName = ALL_FUNCTION;
            }

            auto strFunctionName = functionName.c_str(); /*pgr0745*/
            std::fprintf(stdout, "%sのデバッグレベルを%dに変更しました。\n", strFunctionName, level); /*pgr0579*/ /*pgr1170*//*pgr0519*/
            ret = mdbcom::Result::OK;
        }
        else
        {
            switch (level)
            {
                case mdbcom::MDBCommandResult::ERROR_LEVEL: /*pgr2031*/
                    std::cout << ERR_LEVEL << std::endl;
                    break;

                case mdbcom::MDBCommandResult::ERROR_NOTFOUND: /*pgr2031*/
                    std::cout << ERR_NAME << std::endl;
                    break;

                case mdbcom::MDBCommandResult::ERROR_DOUBLE_EXEC: /*pgr2031*/
                    std::cout << ERR_REEXECUTE << std::endl;
                    break;

                default:
                    std::cout << SET_ERR << std::endl;
                    break;
            }
        }
    }
    else
    {
        std::cout << HELP_MESSAGE << std::endl;
    }

    mdb_apl_log_end();
    return ret;
};

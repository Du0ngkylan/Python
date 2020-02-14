/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file        MDBNetworkUtility.cpp
 * @brief       Source file for implementing MDBNetworkUtility information.
 * @author      trangvp: create it for support MDBNetworkUtility code.
 * @date        2016/11/22
 */

#include <Poco/Net/NetworkInterface.h>
#include <Poco/StringTokenizer.h>

#include "MDBNetworkUtility.h"
#include "MDBLogger.h"

namespace mdbcom {

    const char MDBNetworkUtility::CHAR_PERCENT { '%' };

    bool MDBNetworkUtility::isConstructedAddressList = false;

    std::vector<std::string> MDBNetworkUtility::ipAddressList = { };

    /*!
     * @brief check input IP is included in network interface list or not
     * @param [in] inputIP the input IP
     * @retval true it exists
     * @retval false it doesn't exist
     */
    bool MDBNetworkUtility::checkIPInNetworkInterfaces(const std::string &inputIP)
    {
        mdb_apl_log_start();
        auto tmpInputIP = inputIP.c_str(); /* pgr0745 */
        mdb_apl_log_debug("in inputIP = [%s]", tmpInputIP);

        if (false == isConstructedAddressList)
        {
            auto interfaceList = Poco::Net::NetworkInterface::list(false, false);  /* pgr0745 */
            // Count number of IP
            auto counter = 0u; /* pgr0745 */
            // Delimiter %
            auto delim = std::string { 1, CHAR_PERCENT }; /* pgr0745 */

            // Loop in all Interface
            for (auto it = interfaceList.begin(); it != interfaceList.end(); ++it)   /* pgr0745 */
            {
                const auto &ipList = it->addressList(); /* pgr0745 */
                auto ipIt = ipList.begin(); /* pgr0745 */
                auto ipEnd = ipList.end(); /* pgr0745 */

                for (; ipIt != ipEnd; ipIt++)
                {
                    const auto &ipAddress = /* pgr0745 */
                        ipIt->get<Poco::Net::NetworkInterface::IP_ADDRESS>().toString();
                    auto rawIpFromInterface = ipAddress.c_str(); /* pgr0745 */

                    try
                    {
                        // Tokenize % character to get IP only
                        Poco::StringTokenizer result { ipAddress, delim };
                        auto tmpIpAddress = result[0].c_str(); /* pgr0745 */
                        counter++;
                        mdb_apl_log_debug("ipAddressList[%u] = %s", counter, tmpIpAddress);
                        ipAddressList.push_back(tmpIpAddress);
                    }
                    catch (const Poco::RangeException &)
                    {
                        mdb_apl_log_warn("IP has wrong format: %s", rawIpFromInterface);
                    }
                    catch (...)
                    {
                        mdb_apl_log_warn("Unknown error with IP tokenizer: %s", rawIpFromInterface);
                    }
                }
            }

            // Turn off flag to avoid get interfaces multiple time
            isConstructedAddressList = true;
        }

        auto isExist = false; /* pgr0745 */

        for (auto i = 0u; i < ipAddressList.size(); i++)  /* pgr0692 *//* pgr0745 *//* pgr0360 */
        {
            if (inputIP == ipAddressList[i])
            {
                isExist = true;
                mdb_apl_log_debug("Found local IP: %s", tmpInputIP);
                break;
            }
        }

        mdb_apl_log_debug("out isExist = [%d]", isExist);
        mdb_apl_log_end();
        return isExist;
    }
}

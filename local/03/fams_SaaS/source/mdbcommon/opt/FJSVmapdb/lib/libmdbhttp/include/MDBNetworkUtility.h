/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file MDBNetworkUtility.h
 * @brief Definition for Network utility
 * @author trangvp:
 * @date 2016/11/22
 */

#ifndef LIBMDBHTTP_MDBNETWORKUTILITY_H_
#define LIBMDBHTTP_MDBNETWORKUTILITY_H_

#include <vector>
#include <string>

namespace mdbcom {
    /*!
     * @class MDBNetworkUtility
     * @brief support network related function
     */
    class MDBNetworkUtility {
    public:
        /*!
         * @brief check input IP is included in network interface list or not
         * @param [in] inputIP the input IP
         * @retval true it exists
         * @retval false it doesn't exist
         */
        static bool checkIPInNetworkInterfaces(const std::string &inputIP);
    private:
        //! Constant for character '%'
        static const char CHAR_PERCENT;
        //! store result of network interface list
        static std::vector<std::string> ipAddressList;
        //! Check the first load of network interface
        static bool isConstructedAddressList;
    };
}
#endif /* LIBMDBHTTP_MDBCRYPTOUTILITY_H_ */

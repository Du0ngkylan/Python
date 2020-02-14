/* COPYRIGHT 2017 FUJITSU LIMITED */
/*!
 * @file    MDBSyslogContent.cpp
 * @brief   Implementation for syslog content
 * @author  trunghn:
 * @date    2016/10/27
 */

#include "MDBSyslogContent.h"

namespace mdbcom {

    /*!
     * @brief syslog content constructor
     *
     * Init default value.
     *
     * @param [in] inPriority priority of syslog
     * @param [in] inFacility facility of syslog
     * @param [in] inContent  content of syslog
     * @return None.
     */
    MDBSyslogContent::MDBSyslogContent(const std::string &inPriority, const std::string &inFacility,
                                       const std::string &inContent) :
        priority { inPriority }, facility { inFacility }, content { inContent }
    {
    }
}

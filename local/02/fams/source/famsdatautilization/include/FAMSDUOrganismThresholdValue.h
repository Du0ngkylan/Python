/*
 * FAMSDUOrganismThresholdValue.h
 *
 *  Created on: Jan 30, 2019
 *      Author: mapdb
 */

#ifndef FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMTHRESHOLDVALUE_H_
#define FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMTHRESHOLDVALUE_H_



#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUOrganismThresholdValue
     * @brief The class support methods relating organism
     */
    class FAMSDUOrganismThresholdValue {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUOrganismThresholdValue(void) : unit {""}, value {-1}
        {
        }


        inline std::string getUnit(void) const {
            return unit;
        }

        inline void setUnit(std::string _unit) {
            unit = _unit;
        }

        inline std::string getValue(void) const {
            return value;// == "0"? "" : value;
        }

        inline void setValue(std::string _value) {
            value = _value;
        }
    private:
        int type;
        std::string value;
        std::string unit;
    };
}


#endif /* FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMTHRESHOLDVALUE_H_ */

/*
 * FAMSDUOrganismThreshold.h
 *
 *  Created on: Jan 30, 2019
 *      Author: mapdb
 */

#ifndef FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMTHRESHOLD_H_
#define FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMTHRESHOLD_H_



#include <vector>
#include <string>
#include "FAMSDUOrganismThresholdValue.h"

namespace famsdatautil {
    /*!
     * @class FAMSDUOrganismThreshold
     * @brief The class support methods relating organism
     */
    class FAMSDUOrganismThreshold {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
        FAMSDUOrganismThreshold(void) :
            minValue {},
            maxValue {},
            type {-1}
        {
        }

        inline FAMSDUOrganismThresholdValue getMinValue(void) const {
            return minValue;
        }

        inline void setMinValue(std::string _unit, std::string _value) {
            minValue.setUnit(_unit);
            minValue.setValue(_value);
        }

        inline FAMSDUOrganismThresholdValue getMaxValue(void) const {
            return maxValue;
        }



        inline void setMaxValue(std::string _unit, std::string _value) {
            maxValue.setUnit(_unit);
            maxValue.setValue(_value);
        }

        inline int getType(void) const {
            return type;
        }

        inline void setType(int _type) {
            type = _type;
        }

    private:
        int type;
        FAMSDUOrganismThresholdValue minValue;
        FAMSDUOrganismThresholdValue maxValue;
    };
}


#endif /* FAMSDATAMANAGEMENT_OPT_FJSVMAPDB_DAEMON_FAMSDATAUTILIZATION_INCLUDE_FAMSDUORGANISMTHRESHOLD_H_ */

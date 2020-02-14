/* Copyright© 2020 FUJITSU LIMITED All Rights Reserved. */
/*!
 * @file        FAMSDUTaskSpecificsData.h
 * @brief       request data defination file
 * @author      thanglq
 * @date        2020/2/4
 */

#ifndef FAMS_DATA_UTIL_TASK_SPECIFICS_DATA_H_
#define FAMS_DATA_UTIL_TASK_SPECIFICS_DATA_H_

#include <vector>
#include <string>
#include "FAMSCommon.h"

namespace famsdatautil {
	struct Amount {
		std::string lotId;
		std::string amount;
		std::string leftover;
	};

	class FAMSDUTaskSpecificsContent
	{
	public:
		FAMSDUTaskSpecificsContent(void)
		{
		}
	private:
		std::string id;
		std::string stockId;
		std::vector<Amount> amounts;
	};
	/*!
	 * @class FAMSDUTaskSpecificsData
	 * @brief The class support methods relating user
	 */
    class FAMSDUTaskSpecificsData {
    public:
        /*!
         * @brief Constructor.
         * @param None.
         * @return None.
         */
		FAMSDUTaskSpecificsData(void) :
        {
        }

    private:
        int8_t m_type;
		FAMSDUTaskSpecificsData m_specifics;
    };
}

#endif /* FAMS_DATA_UTIL_TASK_SPECIFICS_DATA_H_ */

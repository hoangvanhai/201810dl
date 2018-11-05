/** @FILE NAME:    template.c
 *  @DESCRIPTION:  This file for ...
 *
 *  Copyright (c) 2018 EES Ltd.
 *  All Rights Reserved This program is the confidential and proprietary
 *  product of EES Ltd. Any Unauthorized use, reproduction or transfer
 *  of this program is strictly prohibited.
 *
 *  @Author: HaiHoang
 *  @NOTE:   No Note at the moment
 *  @BUG:    No known bugs.
 *
 *<pre>
 *  MODIFICATION HISTORY:
 *
 *  Ver   Who       Date                Changes
 *  ----- --------- ------------------  ----------------------------------------
 *  1.00  HaiHoang  August 1, 2018      First release
 *
 *
 *</pre>
 ******************************************************************************/

/***************************** Include Files *********************************/
#include <app.h>
#include <rtc_comm.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
SApp sApp;
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void 			App_Init(SApp *pApp) {

}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void 			task_periodic(void *parg) {

	RTC_InitI2C(0);
	OSA_SleepMs(100);

	if(RTC_GetTimeDate(&g_DateTime) == 0) {
		if(g_DateTime.tm_year == 1990) {
			RTC_SetDateTime(0, 0, 1, 1, 2018);
		}

	} else {
		ASSERT(FALSE);
	}



	FM_Init(0);



	while(1) {
		OSA_SleepMs(1000);
		if(RTC_GetTimeDate(&g_DateTime) == 0) {
			/*
			LREP("sec %d min %d hour %d day: %d date: %d month: %d year: %d\r\n",
					g_DateTime.tm_sec, g_DateTime.tm_min, g_DateTime.tm_hour, g_DateTime.tm_wday,
					g_DateTime.tm_mday, g_DateTime.tm_mon, g_DateTime.tm_year);
					*/
		} else {
			ASSERT(FALSE);
		}
	}
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

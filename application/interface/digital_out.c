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
#include <digital_out.h>
#include <gpio_pins.h>
#include <definition.h>
#include <fsl_gpio_driver.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
SControlDo obj[DIGITAL_OUTPUT_NUM_CHANNEL];
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void DOCtrl_InitAll(uint16_t period, uint16_t duty, uint8_t active) {
	DOCtrl_Init(&obj[0], 	DigitalOutput0, 	FALSE,  period, 	duty, GPIO_DRV_WritePinOutput);
	DOCtrl_Init(&obj[1], 	DigitalOutput1, 	FALSE, 	period, 	duty, GPIO_DRV_WritePinOutput);
	DOCtrl_Init(&obj[2], 	DigitalOutput2, 	FALSE, 	period, 	duty, GPIO_DRV_WritePinOutput);
	DOCtrl_Init(&obj[3], 	DigitalOutput3, 	FALSE, 	period, 	duty, GPIO_DRV_WritePinOutput);
	DOCtrl_Init(&obj[4], 	DigitalOutput4, 	FALSE, 	period, 	duty, GPIO_DRV_WritePinOutput);
	DOCtrl_Init(&obj[5], 	DigitalOutput5, 	FALSE, 	period, 	duty, GPIO_DRV_WritePinOutput);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void DOCtrl_Init(SControlDo *pDo, uint32_t pinid, uint8_t active,
		uint32_t period, uint32_t high, void *func) {
	pDo->pinId = pinid;
	pDo->active = active;
	pDo->period = period;
	pDo->high = high;
	pDo->clb_func = func;
	pDo->counter = 0;
	pDo->gen_pulse = 0;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void DOCtrl_SetPeriod(uint8_t id, uint32_t period) {
	(&obj[id])->period = period;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void DOCtrl_SetHighDuty(uint8_t id, uint32_t high) {
	(&obj[id])->high = high;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void DOCtrl_SetPulse(uint8_t id, uint8_t pulse) {
	(&obj[id])->gen_pulse = pulse;
	if(pulse == 0) {
		(&obj[id])->clb_func((&obj[id])->pinId, (&obj[id])->active ? 0 : 1);
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



void DOCtrl_ActAll() {
	int i = 0;
	for(; i < DIGITAL_OUTPUT_NUM_CHANNEL; i++) {
		DOCtrl_Act(&obj[i]);
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

void DOCtrl_OffAll() {
	int i = 0;
	for(; i < DIGITAL_OUTPUT_NUM_CHANNEL; i++) {
		DOCtrl_SetPulse(i, 0);
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

void DOCtrl_OnAll() {
	int i = 0;
	for(; i < DIGITAL_OUTPUT_NUM_CHANNEL; i++) {
		DOCtrl_SetPulse(i, 1);
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
void DOCtrl_Act(SControlDo* pDo) {
	if((pDo)->gen_pulse > 0) {
		(pDo)->counter++;
		if((pDo)->counter == (pDo)->high) {
			(pDo)->clb_func((pDo)->pinId, pDo->active ? 0 : 1);
		} else if((pDo)->counter == (pDo)->period) {
			(pDo)->counter = 0;
			(pDo)->clb_func((pDo)->pinId, pDo->active ? 1 : 0);
		}
		if((pDo)->counter > (pDo)->period) {
			(pDo)->counter = 0;
			(pDo)->gen_pulse--;
		}
	}
}


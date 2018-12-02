/*
 * digital_out.h
 *
 *  Created on: Dec 2, 2018
 *      Author: PC
 */

#ifndef APPLICATION_INTERFACE_DIGITAL_OUT_H_
#define APPLICATION_INTERFACE_DIGITAL_OUT_H_

#include <typedefs.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/
typedef void(*do_func) (uint32_t, uint32_t);

typedef struct SLed_ {
	uint8_t		id;
	uint32_t	pinId;
	uint8_t 	gen_pulse;
	uint8_t		active;
	uint32_t 	high;			// high in ms
	uint32_t 	period;			// period in ms
	uint32_t	counter;
	do_func		clb_func;

}SControlDo;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
void DOCtrl_InitAll(uint16_t period, uint16_t duty, uint8_t active);
void DOCtrl_ActAll();
void DOCtrl_OffAll();
void DOCtrl_OnAll();
void DOCtrl_Act(SControlDo* pDo);
void DOCtrl_Init(SControlDo *pDo, uint32_t pinid, uint8_t blink,
		uint32_t period, uint32_t high, void *func);
void DOCtrl_SetPeriod(uint8_t id, uint32_t period);
void DOCtrl_SetHighDuty(uint8_t id, uint32_t high);
void DOCtrl_SetPulse(uint8_t id, uint8_t pulse);

/************************** Variable Definitions *****************************/

/*****************************************************************************/

#endif /* APPLICATION_INTERFACE_DIGITAL_OUT_H_ */

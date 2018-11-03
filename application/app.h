#ifndef APPLICATION_APP_H_
#define APPLICATION_APP_H_

/** @FILE NAME:    template.h
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
#include <includes.h>
#include <gpio_pins.h>
#include <Transceiver.h>
#include <definition.h>
/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/

typedef struct SApp_ {

	STrans	sTransPc;
}SApp;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
extern void 	task_shell(task_param_t );
extern void 	task_filesystem(task_param_t );
extern void 	task_modbus(task_param_t );
extern void 	task_serialcomm(task_param_t);
void 			App_Init(SApp *pApp);
void 			task_periodic(void *parg);

/************************** Variable Definitions *****************************/
extern OS_TCB 	TCB_task_shell;
extern OS_TCB 	TCB_task_filesystem;
extern OS_TCB 	TCB_task_modbus;
extern OS_TCB	TCB_task_serialcomm;
extern bool 	sdCardDetect;

extern SApp		sApp;
/*****************************************************************************/









#endif /* APPLICATION_APP_H_ */

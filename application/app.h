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
#include <master.h>
#include <rtc_comm.h>
#include <hw_profile.h>

/************************** Constant Definitions *****************************/
#define APP_TASK_DEFINE(task, stackSize)                          \
    OS_TCB TCB_##task;                                            \
    task_stack_t task##_stack[(stackSize)/sizeof(task_stack_t)];  \
    task_handler_t task##_task_handler;

/**************************** Type Definitions *******************************/

typedef struct SApp_ {
	STrans				sTransPc;
	STrans				sTransUi;
	SModbus				sModbus;
	SDateTime			sDateTime;

	/* NET */
	SDigitalInput		sDI;
	SDigitalOutput		sDO;
	SAnalogInput		sAI;

	APP_TASK_DEFINE(task_shell, 		TASK_SHELL_STACK_SIZE);
	APP_TASK_DEFINE(task_filesystem, 	TASK_FILESYSTEM_STACK_SIZE);
	APP_TASK_DEFINE(task_modbus, 		TASK_MODBUS_STACK_SIZE);
	APP_TASK_DEFINE(task_serialcomm,	TASK_SERIAL_COMM_STACK_SIZE);
	APP_TASK_DEFINE(task_periodic,		TASK_PERIODIC_STACK_SIZE);
}SApp;

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
/* Application interface */
void 			App_Init(SApp *pApp);
void			App_InitTaskHandle(SApp *pApp);
void			App_CreateAppTask(SApp *pApp);
void 			App_TaskShell(task_param_t );
void 			App_TaskFilesystem(task_param_t );
void 			App_TaskModbus(task_param_t );
void 			App_TaskSerialcomm(task_param_t);
void 			App_TaskPeriodic(task_param_t);

// Date time
int				App_InitDateTime(SApp *pApp);
SDateTime		App_GetDateTime(SApp *pApp);
int 			App_SetDateTime(SApp *pApp, SDateTime time);

// Communication
int				App_InitTransUI(SApp *pApp);
int				App_InitTransPC(SApp *pApp);
int 			App_SendUI(SApp *pApp, uint8_t *data, uint8_t len, bool ack);
int				App_SendPC(SApp *pApp, uint8_t *data, uint8_t len, bool ack);
void			App_SetTransUICallback(SApp *pApp, EL3Event evt, FClbL3Event func);
void			App_SetTransPCCallback(SApp *pApp, EL3Event evt, FClbL3Event func);
void			App_SetNetPCCallback(SApp *pApp);
void 			App_SetFTPCallback(SApp *pApp);

// Modbus
int				App_InitModbus(SApp *pApp);
int				App_DeinitModbus(SApp *pApp);
int				App_ModbusDoRead(SApp *pApp);

// DI
int				App_InitDI(SApp *pApp);
int 			App_InitDO(SApp *pApp);
int				App_InitAI(SApp *pApp);


/************************** Variable Definitions *****************************/

extern SApp		sApp;
SApp 			*pAppObj;
/*****************************************************************************/









#endif /* APPLICATION_APP_H_ */

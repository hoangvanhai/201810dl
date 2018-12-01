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
#include <master-rtu.h>
#include <rtc_comm.h>
#include <analog.h>
#include <lwip/netif.h>
#include <filesystem.h>
#include <os_app_hooks.h>

/************************** Constant Definitions *****************************/
#define CONFIG_FILE_PATH		"/conf/config.dat"

#define APP_TASK_DEFINE(task, stackSize)                          \
    OS_TCB TCB_##task;                                            \
    task_stack_t task##_stack[(stackSize)/sizeof(task_stack_t)];  \
    task_handler_t task##_task_handler;

/**************************** Type Definitions *******************************/
typedef struct SApp_ {
	ESysStatus			eStatus;
	ECtrlCode			eCtrlCode;
	STrans				sTransPc;
	STrans				sTransUi;
	SModbus				sModbus;
	SAnalogReader		sAnalogReader;
	SDateTime			sDateTime;
	SDigitalInputLog	sDI;
	SAnalogInput		sAI;
	SModbusValue		sMB;
	STagValue			sTagValue;

	APP_TASK_DEFINE(task_shell, 		TASK_SHELL_STACK_SIZE);
	APP_TASK_DEFINE(task_ui, 			TASK_UI_STACK_SIZE);
	APP_TASK_DEFINE(task_modbus, 		TASK_MODBUS_STACK_SIZE);
	APP_TASK_DEFINE(task_serialcomm,	TASK_SERIAL_COMM_STACK_SIZE);
	APP_TASK_DEFINE(task_periodic,		TASK_PERIODIC_STACK_SIZE);
	APP_TASK_DEFINE(task_startup,		TASK_STARTUP_STACK_SIZE);

	SSysCfg				sCfg;
	OS_TMR 				hCtrlTimer;

	FATFS				sFS0;
	FATFS				sFS1;

	uint8_t				currPath[256];
	uint8_t				currFileName[256];

	bool				sdhcPlugged;



}SApp;

/***************** Macros (Inline Functions) Definitions *********************/

#define APP_TASK_INIT_HANDLER(p, task) 		(p)->task##_task_handler = &((p)->TCB_##task)
#define App_SetSysStatus(pApp, state)    	(pApp)->eStatus |= (state)
#define App_ClearSysStatus(pApp, state)  	(pApp)->eStatus &= ~(state)
#define App_IsSysStatus(pApp, state)		((pApp)->eStatus & state)

#define App_GetSdcard1Error(pApp)			((pApp)->eStatus & SYS_ERR_SDCARD_1)
#define App_GetSdcard2Error(pApp)			((pApp)->eStatus & SYS_ERR_SDCARD_2)


#define App_SetCtrlCode(pApp, code)			((pApp)->eCtrlCode |= code)
#define App_ClearCtrlCode(pApp, code)		((pApp)->eCtrlCode &= ~(code))
#define App_IsCtrlCodePending(pApp, code)   ((pApp)->eCtrlCode & code)

/************************** Function Prototypes ******************************/
/* Application interface */
/* high level initialize */
void 			App_Init(SApp *pApp);
int				App_LoadConfig(SApp *pApp, const char* cfg_path);
int 			App_SaveConfig(SApp *pApp, const char* cfg_path);
int				App_GenDefaultConfig(SSysCfg *pHandle);
int				App_VerifyTagConfig(STag *pHandle, uint8_t tagIdx);
int 			App_DefaultTag(STag *pHandle, uint8_t tagIdx);
int 			App_SetConfig(SApp *pApp, const uint8_t *pData);
int				App_GetConfig(SApp *pApp, uint8_t cfg, uint8_t idx, ECfgConnType type);

/* File system */
int				App_InitFS(SApp *pApp);

/* task body */
void			App_InitTaskHandle(SApp *pApp);
int				App_CreateAppTask(SApp *pApp);
void 			App_TaskShell(task_param_t );
void 			App_TaskUserInterface(task_param_t );
void 			App_TaskModbus(task_param_t );
void 			App_TaskSerialcomm(task_param_t);
void 			App_TaskPeriodic(task_param_t);
void 			App_TaskStartup(task_param_t);

/* Date time */
int				App_InitDateTime(SApp *pApp);
SDateTime		App_GetDateTime(SApp *pApp);
int 			App_SetDateTime(SApp *pApp, SDateTime time);

// Communication
int 			App_SendUI(SApp *pApp, uint8_t *data, uint8_t len, bool ack);
int				App_SendPC(SApp *pApp, uint8_t subctrl, uint8_t *data, uint8_t len, bool ack);
void			App_SetNetPCCallback(SApp *pApp);
void 			App_SetFTPCallback(SApp *pApp);

void 			App_CommRecvHandle(const uint8_t *data);


// Modbus
int				App_InitModbus(SApp *pApp);
int				App_DeinitModbus(SApp *pApp);
int				App_ModbusDoRead(SApp *pApp);

// DI DO
int				App_InitDI(SApp *pApp);
int 			App_InitDO(SApp *pApp);
int				App_InitAI(SApp *pApp);
int 			App_UpdateTagContent(SApp *pApp);
void 			App_DiReadAllPort(SApp *pApp);
void			App_AiReadAllPort(SApp *pApp);

void			App_SetDoPinByName(SApp *pApp, const char *name, uint32_t logic);

double			App_GetAIValueByIndex(SAnalogInput *pHandle, uint16_t index);
double			App_GetMBValueByAddress(SModbusValue *pHandle, uint16_t addr);
bool			App_GetDILevelByIndex(SDigitalInputLog *pHandle, uint16_t index);

int 			App_GenerateLogFile(SApp *pApp);
int				App_GenerateFakeTime(SApp *pApp);

/* Callback section */
void Clb_TimerControl(void *p_tmr, void *p_arg);

/************************** Variable Definitions *****************************/
extern SApp		sApp;
extern SApp 	*pAppObj;
/*****************************************************************************/









#endif /* APPLICATION_APP_H_ */

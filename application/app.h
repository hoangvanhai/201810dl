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
#include <TransPC.h>
#include <TransUI.h>
#include <definition.h>
#include <master-rtu.h>
#include <rtc_comm.h>
#include <analog.h>
#include <lwip/netif.h>
#include <filesystem.h>
#include <os_app_hooks.h>
#include <Network.h>
#include <ring_file.h>
#include <dac_comm.h>
#include <dac_out.h>

/************************** Constant Definitions *****************************/
#define CONFIG_FILE_PATH		"/conf/config.dat"

#define APP_TASK_DEFINE(task, stackSize)                          \
    OS_TCB TCB_##task;                                            \
    task_handler_t task##_task_handler = &(TCB_##task)

/**************************** Type Definitions *******************************/
typedef struct SApp_ {
	SComStatus			sStatus;
	ECtrlCode			eCtrlCode;
	STransPC			sTransPc;
	semaphore_t			semTransPc;
	STransUI			sTransUi;
	semaphore_t			semTransUi;
	SModbus				sModbus;
	SAnalogReader		sAnalogReader;
	SDigitalInputLog	sDI;
	SDigitalOutputLog	sDO;
	SAnalogInput		sAI;
	SModbusValue		sMB;
	STagValue			sTagValue;
	OS_TMR 				hCtrlTimer;
	FATFS				sFS0;
	FATFS				sFS1;
	uint8_t				currFileName[256];
	SSysCfg				sCfg;
	uint32_t			pcCounter;
	uint32_t			uiCounter;
	uint16_t			currLevel;
	semaphore_t			hSem;
	uint32_t			aiReadCount;
	uint32_t			mbReadCount;
}SApp;

/***************** Macros (Inline Functions) Definitions *********************/

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
int 			App_SetConfig(SApp *pApp, const uint8_t *pData, bool serial);
int				App_GetConfig(SApp *pApp, uint8_t cfg, uint8_t idx, ECfgConnType type);

/* File system */
int				App_InitIntFS(SApp *pApp);
int 			App_InitExtFs(SApp *pApp);
int				App_DeinitExtFs(SApp *pApp);
void			App_WriteExtFs(SApp *pApp);
int				App_DoCopyIntToExt(SApp *pApp);

/* task body */
void			App_InitTaskHandle(SApp *pApp);
int				App_CreateAppTask(SApp *pApp);
int				App_CreateAppEvent(SApp *pApp);
void 			App_TaskShell(task_param_t );
void 			App_TaskUserInterface(task_param_t );
void 			App_TaskModbus(task_param_t );
void 			App_TaskAnalogIn(task_param_t );
void 			App_TaskSerialcomm(task_param_t);
void 			App_TaskPeriodic(task_param_t);
void 			App_TaskStartup(task_param_t);

/* Date time */
int				App_InitDateTime(SApp *pApp);
SDateTime		App_GetDateTime(SApp *pApp);
int 			App_SetDateTime(SApp *pApp, SDateTime time);

// Communication
int 			App_SendUI(SApp *pApp, uint8_t subctrl, uint8_t *data, uint8_t len, bool ack);
int				App_SendPC(SApp *pApp, uint8_t subctrl, uint8_t *data, uint8_t len, bool ack);
int				App_SendPCNetworkClient(uint8_t subctrl, uint8_t *data, uint8_t len);
void			App_SetNetPCCallback(SApp *pApp);
void 			App_SetFTPCallback(SApp *pApp);

void 			App_SerialComRecvHandle(const uint8_t *data);
void 			App_TcpServerRecvHandle(const uint8_t *data);
void 			App_TcpClientRecvHandle(const uint8_t *data, int len);
void			App_CommCalibAi(SApp *pApp, const uint8_t *data);
void			App_CommCalibCurrPwr(SApp *pApp, const uint8_t *data);
void 			App_CommTurnOnOffCurr(SApp *pApp, const uint8_t *data);

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

bool			App_SetDoPinByName(SApp *pApp, const char *name, uint32_t logic);
void			App_SetDoPinByIndex(SApp *pApp, uint8_t idx, uint32_t logic);

double			App_GetAIValueByIndex(SAnalogInput *pHandle, uint16_t index);
double			App_GetMBValueByIndex(SModbusValue *pHandle, uint16_t index);
bool			App_GetDILevelByIndex(SDigitalInputLog *pHandle, uint16_t index);
bool			App_CheckNameExisted(SApp *pApp, const char *name);

int 			App_GenerateLogFile(SApp *pApp, uint8_t server);
int 			App_GenerateLogFileByName(SApp *pApp, const char *name, uint8_t server);
int				App_GenerateFakeTime(SApp *pApp);

int				App_InitNetworkModule(SApp *pApp);

/* Callback section */
void Clb_TimerControl(void *p_tmr, void *p_arg);

/************************** Variable Definitions *****************************/
extern SApp		sApp;
extern SApp 	*pAppObj;

extern OS_TCB TCB_task_ai;
extern task_handler_t task_ai_task_handler;
extern task_stack_t	task_ai_stack[TASK_AI_STACK_SIZE/ sizeof(task_stack_t)];

extern OS_TCB TCB_task_shell;
extern task_handler_t task_shell_task_handler;
extern task_stack_t	task_shell_stack[TASK_SHELL_STACK_SIZE/ sizeof(task_stack_t)];

extern OS_TCB TCB_task_ui;
extern task_handler_t task_ui_task_handler;
extern task_stack_t	task_ui_stack[TASK_UI_STACK_SIZE/ sizeof(task_stack_t)];

extern OS_TCB TCB_task_modbus;
extern task_handler_t task_modbus_task_handler;
extern task_stack_t	task_modbus_stack[TASK_MODBUS_STACK_SIZE/ sizeof(task_stack_t)];

extern OS_TCB TCB_task_serialcomm;
extern task_handler_t task_serialcomm_task_handler;
extern task_stack_t	task_serialcomm_stack[TASK_SERIAL_COMM_STACK_SIZE/ sizeof(task_stack_t)];

extern OS_TCB TCB_task_periodic;
extern task_handler_t task_periodic_task_handler;
extern task_stack_t	task_periodic_stack[TASK_PERIODIC_STACK_SIZE/ sizeof(task_stack_t)];

extern OS_TCB TCB_task_startup;
extern task_handler_t task_startup_task_handler;
extern task_stack_t	task_startup_stack[TASK_STARTUP_STACK_SIZE/ sizeof(task_stack_t)];

/*****************************************************************************/









#endif /* APPLICATION_APP_H_ */

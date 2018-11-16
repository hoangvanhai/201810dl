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

#include <lwip/netif.h>
#include <task_filesystem.h>
/************************** Constant Definitions *****************************/
#define CONFIG_FILE_PATH		"/conf/config.dat"
#define	CFG_SET						0x80
#define CFG_GET						0x00
#define CFG_COMMON					0x01
#define CFG_TAG						0x02
#define CFG_COM_TAG					0x03		// indicate this is terminate frame
												// logger should write config to sdcard

#define SERVER_FTP_IP_IDX			0
#define SERVER_FTP_IP_WIDTH			4
#define SERVER_FTP_PORT_IDX			(SERVER_FTP_IP_IDX + SERVER_FTP_IP_WIDTH)
#define SERVER_FTP_PORT_WIDTH		2
#define SERVER_CTRL_IP_IDX			(SERVER_FTP_PORT_IDX + SERVER_FTP_PORT_WIDTH)
#define SERVER_CTRL_IP_WIDTH		4
#define SERVER_CTRL_PORT_IDX		(SERVER_CTRL_IP_IDX + SERVER_CTRL_IP_WIDTH)
#define SERVER_CTRL_PORT_WIDTH		2
#define DEVICE_IP_IDX				(SERVER_CTRL_PORT_IDX + SERVER_CTRL_PORT_WIDTH)
#define DEVICE_IP_WIDTH				4
#define DEVICE_TINH_IDX				(DEVICE_IP_IDX + DEVICE_IP_WIDTH)
#define DEVICE_TINH_WIDTH			6
#define DEVICE_COSO_IDX				(DEVICE_TINH_IDX + DEVICE_TINH_WIDTH)
#define DEVICE_COSO_WIDTH			6
#define DEVICE_TRAM_IDX				(DEVICE_COSO_IDX + DEVICE_COSO_WIDTH)
#define DEVICE_TRAM_WIDTH			10
#define DEVICE_SCAN_DUR_IDX			(DEVICE_TRAM_IDX + DEVICE_TRAM_WIDTH)
#define DEVICE_SCAN_DUR_WIDTH		1
#define DEVICE_LOG_DUR_IDX			(DEVICE_SCAN_DUR_IDX + DEVICE_SCAN_DUR_WIDTH)
#define DEVICE_LOG_DUR_WIDTH		1




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
	SDateTime			sDateTime;
	SDigitalInput		sDI;
	SAnalogInput		sAI;
	SModbusValue		sMB;

	APP_TASK_DEFINE(task_shell, 		TASK_SHELL_STACK_SIZE);
	APP_TASK_DEFINE(task_filesystem, 	TASK_FILESYSTEM_STACK_SIZE);
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
int 			App_SetConfig(SApp *pApp, uint8_t *pData);
int				App_GetConfig(SApp *pApp, uint8_t cfg, uint8_t idx, ECfgConnType type);

/* File system */
int				App_InitFS(SApp *pApp);

/* task body */
void			App_InitTaskHandle(SApp *pApp);
int				App_CreateAppTask(SApp *pApp);
void 			App_TaskShell(task_param_t );
void 			App_TaskFilesystem(task_param_t );
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
int				App_SendPC(SApp *pApp, uint8_t *data, uint8_t len, bool ack);
void			App_SetNetPCCallback(SApp *pApp);
void 			App_SetFTPCallback(SApp *pApp);

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
bool			App_GetDILevelByIndex(SDigitalInput *pHandle, uint16_t index);

int 			App_GenerateLogFile(SApp *pApp);
int				App_GenerateFakeTime(SApp *pApp);

/* Callback section */
void Clb_TimerControl(void *p_tmr, void *p_arg);

/************************** Variable Definitions *****************************/
extern SApp		sApp;
extern SApp 	*pAppObj;
/*****************************************************************************/









#endif /* APPLICATION_APP_H_ */

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
#include <app_shell.c>
#include <task_filesystem.h>
#include <lib_str.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
static void Clb_TransPC_RecvEvent(void *pData, uint8_t u8Type);
static void Clb_TransPC_SentEvent(void *pDatam, uint8_t u8Type);
static void Clb_TransUI_RecvEvent(void *pData, uint8_t u8Type);
static void Clb_TransUI_SentEvent(void *pDatam, uint8_t u8Type);
/************************** Variable Definitions *****************************/
SApp sApp;
SApp *pAppObj = &sApp;
extern const shell_command_t cmd_table[];
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_Init(SApp *pApp) {

	pApp->eStatus = SYS_ERR_NONE;

	int err;
	memset(pApp->currPath, 0, 256);

	err = App_InitFS(pApp);

	if(err == FR_OK) {
		LREP("app init FS successfully \r\n");
	} else {
		LREP("app init FS failed err = %d \r\n", err);
		App_SetSysStatus(pApp, SYS_ERR_SDCARD_1);
	}

	if(!App_GetSdcard1Error(pApp)) {
		err = App_LoadConfig(pApp, CONFIG_FILE_PATH);
		if(err == FR_OK) {
			LREP("app load config successfully \r\n");
		} else {
			LREP("app load config failed err = %d\r\n", err);
		}
	} else {
		ASSERT(false);
		LREP("sdcard 1 not found, init default config for dry running\r\n");
		App_GenDefaultConfig(&pApp->sCfg);
	}

	App_InitDI(pApp);
	App_InitDO(pApp);
	App_InitAI(pApp);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void	App_InitTaskHandle(SApp *pApp) {
	APP_TASK_INIT_HANDLER(pApp, task_shell);
	APP_TASK_INIT_HANDLER(pApp, task_ui);
	APP_TASK_INIT_HANDLER(pApp, task_modbus);
	APP_TASK_INIT_HANDLER(pApp, task_serialcomm);
	APP_TASK_INIT_HANDLER(pApp, task_periodic);
	APP_TASK_INIT_HANDLER(pApp, task_startup);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int App_LoadConfig(SApp *pApp, const char *cfg_path) {

	FIL fil;        /* File object */
	FRESULT fr;     /* FatFs return code */
	uint32_t read;
	int retVal = -1;

	if(check_obj_existed(cfg_path)) {
		/* Register work area to the default drive */
		fr = f_open(&fil, cfg_path, FA_OPEN_EXISTING | FA_READ);
		if (fr) {
			LREP("open file error: %d\r\n", fr);
			return retVal;
		}

		fr = f_read(&fil, (void*)&pApp->sCfg, sizeof(SSysCfg), (UINT*)&read);

		if(!fr) {
			if(read > 0) {
				LREP("load config content ok\r\n");
				retVal = 0;
			} else {
				retVal = -2;
			}
		} else {
			retVal = -3;
		}

		/* Close the file */
		f_close(&fil);
	} else {
		LREP("gen def config \r\n");
		App_GenDefaultConfig(&pApp->sCfg);
		retVal = App_SaveConfig(pApp, cfg_path);
	}

	return retVal;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int App_SaveConfig(SApp *pApp, const char* cfg_path) {
	FIL fil;        /* File object */
	uint32_t written;
	int retVal;

	retVal = f_open(&fil, cfg_path, FA_CREATE_ALWAYS | FA_WRITE);
	if (retVal != FR_OK) {
		LREP("create file error: %d\r\n", retVal);
		return retVal;
	}

	retVal = f_write(&fil, (void*)&pApp->sCfg, sizeof(SSysCfg), (UINT*)&written);

	if(retVal == FR_OK) {
		if(written > 0) {
			LREP("save config content ok\r\n");
		} else {
			LREP("write length %d less than expected %d\r\n", written, sizeof(SSysCfg));
			retVal = -1;
		}
	} else {
		LREP("f_write return error: %d\r\n", retVal);
	}

	/* Close the file */
	f_close(&fil);
	return retVal;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

int	App_GenDefaultConfig(SSysCfg *pHandle) {

	memset(pHandle, 0, sizeof(SSysCfg));

	pHandle->sCom.coso[0] = 'C';
	pHandle->sCom.coso[1] = 'O';
	pHandle->sCom.coso[2] = 'S';
	pHandle->sCom.coso[3] = 'O';

	pHandle->sCom.tinh[0] = 'T';
	pHandle->sCom.tinh[1] = 'I';
	pHandle->sCom.tinh[2] = 'N';
	pHandle->sCom.tinh[3] = 'H';

	pHandle->sCom.tram[0] = 'T';
	pHandle->sCom.tram[1] = 'R';
	pHandle->sCom.tram[2] = 'A';
	pHandle->sCom.tram[3] = 'M';

	pHandle->sCom.scan_dur 	= 1;
	pHandle->sCom.log_dur 	= 1;	//5;
	pHandle->sCom.modbus_brate = 9600;

	IP4_ADDR(&pHandle->sCom.dev_ip, 192,168,1,2);
	IP4_ADDR(&pHandle->sCom.server_ftp_ip, 192,168,1,12);
	pHandle->sCom.server_ftp_port = 21;
	IP4_ADDR(&pHandle->sCom.server_ctrl_ip, 192,168,1,22);
	pHandle->sCom.server_ctrl_port = 1186;

	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		App_DefaultTag(&pHandle->sTag[i], i);
	}

	return 0;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_VerifyTagConfig(STag *pHandle, uint8_t tagIdx) {

	pHandle->id = tagIdx;
	pHandle->input_id = tagIdx;
	pHandle->input_id = MIN(pHandle->input_id, SYSTEM_NUM_TAG - 1);
	pHandle->pin_calib = MIN(pHandle->pin_calib, DIGITAL_INPUT_NUM_CHANNEL - 1);
	pHandle->pin_error = MIN(pHandle->pin_error, DIGITAL_INPUT_NUM_CHANNEL - 1);
	pHandle->name[sizeof(pHandle->name) - 1] = 0;
	pHandle->raw_unit[sizeof(pHandle->raw_unit) - 1] = 0;
	pHandle->std_unit[sizeof(pHandle->std_unit) - 1] = 0;
	return 0;
}

int App_DefaultTag(STag *pHandle, uint8_t tagIdx) {

	pHandle->id = tagIdx;
	pHandle->input_id = tagIdx;
	pHandle->input_id = MIN(pHandle->input_id, SYSTEM_NUM_TAG - 1);
	pHandle->pin_calib = MIN(pHandle->pin_calib, DIGITAL_INPUT_NUM_CHANNEL - 1);
	pHandle->pin_error = MIN(pHandle->pin_error, DIGITAL_INPUT_NUM_CHANNEL - 1);
	pHandle->enable = true;
	pHandle->report = true;
	pHandle->has_error = false;
	pHandle->has_calib = false;
	pHandle->alarm_enable = false;
	pHandle->input_type = TIT_AI;
	pHandle->raw_min = 0;
	pHandle->raw_max = 100;
	pHandle->scratch_min = 0;
	pHandle->scratch_max = 1000;
	pHandle->coef_a = 0;
	pHandle->coef_b = 1;

	Str_Copy_N((CPU_CHAR*)pHandle->name, "DEFAULT", sizeof(pHandle->name));
	Str_Copy_N((CPU_CHAR*)pHandle->raw_unit, "RAW_UNIT", sizeof(pHandle->raw_unit));
	Str_Copy_N((CPU_CHAR*)pHandle->std_unit, "STD_UNIT", sizeof(pHandle->std_unit));

	return 0;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int App_SetConfig(SApp *pApp, uint8_t *pData) {
	switch(pData[0]) {
	case CFG_COMMON | CFG_SET:
		if(pData[1] == sizeof(SCommon)) {
			memcpy(&pApp->sCfg.sCom, &pData[2], sizeof(SCommon));
		} else {
			ASSERT_NONVOID(FALSE, -1);
		}
		break;

	case CFG_TAG | CFG_SET: {
			uint8_t idx;
			if(pData[1] != (sizeof(STag) + 1)) {
				idx = pData[2];
				memcpy(&pApp->sCfg.sTag[idx], &pData[3], sizeof(STag));
			} else {
				ASSERT_NONVOID(FALSE, -2);
			}
		}

		break;

	case CFG_COM_TAG | CFG_SET:

	default:
		ASSERT_NONVOID(FALSE, -3);
		break;
	}

	return 0;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

int App_GetConfig(SApp *pApp, uint8_t cfg, uint8_t idx, ECfgConnType type) {

	switch(cfg) {
	case CFG_COMMON | CFG_GET:
	if(type == CFG_CONN_SERIAL) {
		uint8_t *sendData = OSA_FixedMemMalloc(sizeof(SCommon) + 2);
		sendData[0] = CFG_COMMON | CFG_GET;
		sendData[1] = sizeof(SCommon);
		memcpy(&sendData[2], &pApp->sCfg.sCom, sizeof(SCommon));
		App_SendPC(pApp, sendData, sizeof(SCommon) + 2, true);
		OSA_FixedMemFree(sendData);
	}
	break;

	case CFG_TAG | CFG_GET:
	if(type == CFG_CONN_SERIAL) {
		uint8_t *sendData = OSA_FixedMemMalloc(sizeof(STag) + 2);
		sendData[0] = CFG_TAG | CFG_GET;
		sendData[1] = sizeof(STag);
		memcpy(&sendData[2], &pApp->sCfg.sTag[idx], sizeof(STag));
		App_SendPC(pApp, sendData, sizeof(STag) + 2, true);
		OSA_FixedMemFree(sendData);
	}
	break;

	default:
		break;
	}

	return 0;
}
/*****************************************************************************/
/** @brief Init fs driver, should call only one time in entired project
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_InitFS(SApp *pApp) {

	int retVal;

	memset(&pApp->sFS0, 0, sizeof(FATFS));

	retVal = f_mount(&pApp->sFS0, "", 0);

	if(retVal != FR_OK) {
		return retVal;
	}

	if(!check_obj_existed("conf")) {
		retVal = f_mkdir("conf");
		if(retVal != FR_OK) {
			LREP("mkdir err = %d\r\n", retVal);
		} else {
			LREP("mkdir successful !\r\n");
		}
	} else {
		LREP("directory conf existed \r\n");
	}

	retVal = current_directory();

	return retVal;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void App_TaskPeriodic(task_param_t parg) {

	SApp *pApp = (SApp *)parg;

	int log_min = pApp->sCfg.sCom.log_dur > 0 ? pApp->sCfg.sCom.log_dur : 1;
	int last_min = 0;
	bool logged = false;

	/*
	ASSERT(RTC_InitDateTime(&pApp->sDateTime) == 0);
	OSA_SleepMs(100);

	if(RTC_GetTimeDate(&pApp->sDateTime) == 0) {
		if(pApp->sDateTime.tm_year == 1990) {
			RTC_SetDateTime(0, 0, 1, 1, 2018);
		}

	} else {
		ASSERT(FALSE);
	}

	//FM_Init(0);
	*/


	rnga_user_config_t rngaConfig;

	// Initialize RNGA
	rngaConfig.isIntMasked         = true;
	rngaConfig.highAssuranceEnable = true;

	RNGA_DRV_Init(0, &rngaConfig);

	pApp->sDateTime.tm_mday = 1;
	pApp->sDateTime.tm_mon = 1;
	pApp->sDateTime.tm_year = 2018;

	LREP("log min = %d\r\n", log_min);

	while(1) {
		OSA_SleepMs(1000);
		if(RTC_GetTimeDate(&pApp->sDateTime) == 0) {
			 /*LREP("Current Time: %04d/%02d/%02d %02d:%02d:%02d\r\n",
					pApp->sDateTime.tm_year, pApp->sDateTime.tm_mon,
					pApp->sDateTime.tm_mday, pApp->sDateTime.tm_hour,
					pApp->sDateTime.tm_min, pApp->sDateTime.tm_sec); */
			//LREP(".");
			GPIO_DRV_TogglePinOutput(kGpioLEDGREEN);

		}

		App_DiReadAllPort(pApp);
		App_AiReadAllPort(pApp);
		App_UpdateTagContent(pApp);

        if(pApp->sDateTime.tm_min != last_min && logged) {
            logged = false;
        }


		if((logged == false) && (pApp->sDateTime.tm_min % log_min == 0)) {
			LREP("generate log file\r\n");
			ASSERT(App_GenerateLogFile(pApp) == FR_OK);
			last_min = pApp->sDateTime.tm_min;
			logged = true;
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
void App_TaskShell(task_param_t param)
{
	OS_ERR err;
	shell_init(cmd_table, my_shell_init);
	LREP(SHELL_PROMPT);
	while(1) {
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, 0, &err);
		if(err == OS_ERR_NONE) {
			shell_task(NULL);
		}
	}
}

#if 0

#endif



/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_TaskModbus(task_param_t param)
{
	SApp *pApp = (SApp *)param;
	OS_ERR err;
	void 	*p_msg;
	OS_MSG_SIZE msg_size;
	CPU_TS	ts;

	App_InitModbus(pApp);

	uint8_t rx_buf[264];
	uint16_t rx_length;

	while (1)
	{
		p_msg = OSTaskQPend(1000, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
		if(err == OS_ERR_NONE) {
			uint8_t retVal;

			LREP("modbus get msg size = %d ts = %d\r\n", msg_size, ts);

			retVal = Modbus_SendAndRecv(&pApp->sModbus,
					(uint8_t*)p_msg, 264, rx_buf, &rx_length, 100);

			if(retVal != TRANS_SUCCESS) {
				LREP("Modbus send err: %d\r\n", retVal);
			}

			OSA_FixedMemFree((uint8_t*)p_msg);
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
void App_TaskSerialcomm(task_param_t param) {

	SApp *pApp = (SApp *)param;
	OS_ERR	err;
	CPU_TS	ts;

	Trans_RegisterClbEvent(&pApp->sTransPc, TRANS_EVT_RECV_DATA, Clb_TransPC_RecvEvent);
	Trans_RegisterClbEvent(&pApp->sTransPc, TRANS_EVT_SENT_DATA, Clb_TransPC_SentEvent);

	Trans_Init(&pApp->sTransPc, BOARD_TRANSPC_UART_INSTANCE,
			BOARD_TRANSPC_UART_BAUD, &pApp->TCB_task_serialcomm);

	LREP("task serial comm init done !\r\n");
	LREP("SFrameInfo Size: %d\r\n", sizeof(SFrameInfo));
	while(1) {
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			Trans_Task(&pApp->sTransPc);
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

void App_TaskUserInterface(task_param_t param)
{
	SApp *pApp = (SApp *)param;
	OS_ERR	err;
	CPU_TS	ts;

	Trans_RegisterClbEvent(&pApp->sTransUi, TRANS_EVT_RECV_DATA, Clb_TransUI_RecvEvent);
	Trans_RegisterClbEvent(&pApp->sTransUi, TRANS_EVT_SENT_DATA, Clb_TransUI_SentEvent);

	Trans_Init(&pApp->sTransUi, BOARD_TRANSUI_UART_INSTANCE,
			BOARD_TRANSUI_UART_BAUD, &pApp->TCB_task_ui);

	LREP("task user interface init done !\r\n");
	while(1) {
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			Trans_Task(&pApp->sTransUi);
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
void App_TaskStartup(task_param_t arg) {

	OS_ERR err;
	CPU_TS	ts;
	SApp *pApp = (SApp*)arg;

	OSA_FixedMemInit();

	App_Init(pApp);

	App_CreateAppTask(pApp);

	OSTmrCreate(&pApp->hCtrlTimer,
				(CPU_CHAR *)"timer",
				(OS_TICK)0,
				(OS_TICK)100,
				(OS_OPT)OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) Clb_TimerControl,
				(void*)NULL,
				(OS_ERR*)&err);

	if (err == OS_ERR_NONE) {
		/* Timer was created but NOT started */
		LREP("timer created successful\r\n");
		OSTmrStart(&pApp->hCtrlTimer, &err);
		if (err == OS_ERR_NONE) {
			/* Timer was created but NOT started */
			LREP("timer started ok\r\n");
		} else {
			LREP("timer start failed\r\n");
		}
	} else {
		LREP("timer create failed\r\n");
	}


	while(1) {

		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			/* First, feed dog to prevent WDG reset */
			WDOG_DRV_Refresh();

			/* Check all pending command */
			if(App_IsCtrlCodePending(pApp, CTRL_INIT_SDCARD_1)) {
				LREP("recv ctrl init sdcard 1\r\n");
				int err = App_InitFS(pApp);
				if(err != FR_OK) {
					App_SetSysStatus(pApp, SYS_ERR_SDCARD_1);
				} else {
					App_ClearSysStatus(pApp, SYS_ERR_SDCARD_1);
					ASSERT(App_LoadConfig(pApp, CONFIG_FILE_PATH) == FR_OK);
				}

				App_ClearCtrlCode(pApp, CTRL_INIT_SDCARD_1);
			}

			if(App_IsCtrlCodePending(pApp, CTRL_INIT_SDCARD_2)) {
				LREP("recv ctrl init sdcard 2\r\n");

				App_ClearCtrlCode(pApp, CTRL_INIT_SDCARD_2);
			}

			if(App_IsCtrlCodePending(pApp, CTRL_INIT_MODBUS)) {
				LREP("recv ctrl init modbus\r\n");
				if(App_InitModbus(pApp) == MB_SUCCESS) {
					App_ClearSysStatus(pApp, SYS_ERR_MODBUS);
				} else {
					App_SetSysStatus(pApp, SYS_ERR_MODBUS);
				}

				App_ClearCtrlCode(pApp, CTRL_INIT_MODBUS);
			}


			WDOG_DRV_Refresh();
		} else {
			//LEP("Feed dog \r\n");
			/* Feed dog to prevent WDG reset */
			WDOG_DRV_Refresh();
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
void Clb_TransPC_RecvEvent(void *pData, uint8_t u8Type) {
	static uint32_t count = 0;
	LREP("Recv frm event: %d - ctrl = 0x%x\r\n", count++, u8Type);

	SFrameInfo *pFrameInfo = (SFrameInfo *)MEM_BODY((SMem*)pData);

	switch(u8Type & 0x3F) {
	case FRM_DATA:
		if(pFrameInfo->pu8Data[0] & CFG_SET) {
			App_SetConfig(pAppObj, pFrameInfo->pu8Data);
		} else {

			//App_GetConfig(pAppObj, pFrameInfo->pu8Data, CFG_CONN_SERIAL);
		}
		break;

	default:
		ASSERT(false);
		break;
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
void Clb_TransPC_SentEvent(void *pDatam, uint8_t u8Type) {
	LREP("Sent done event 0x%x\r\n", u8Type);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void Clb_TransUI_RecvEvent(void *pData, uint8_t u8Type) {

}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void Clb_TransUI_SentEvent(void *pDatam, uint8_t u8Type) {

}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TimerControl(void *p_tmr, void *p_arg) {
//	GPIO_DRV_TogglePinOutput(kGpioLEDGREEN);
	App_GenerateFakeTime(pAppObj);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_InitModbus(SApp *pApp){

	return Modbus_Init(&pApp->sModbus, BOARD_MODBUS_UART_INSTANCE,
			pApp->sCfg.sCom.modbus_brate, 0, 0);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_DeinitModbus(SApp *pApp){

	return 0;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_ModbusDoRead(SApp *pApp){
	int retVal;
	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		if(pApp->sCfg.sTag[i].input_type == TIT_MB) {
			uint8_t data[4];
			uint16_t rlen;
			retVal = MBMaster_Read(pApp->sCfg.sTag[i].input_id,
					pApp->sCfg.sTag[i].slave_reg_addr, 0x03, 1, data, &rlen);
			if(retVal != MB_SUCCESS) {
				pApp->sCfg.sTag[i].status = TAG_STT_MB_FAILED;
			} else {
				pApp->sCfg.sTag[i].status = TAG_STT_OK;
				uint32_t readVal;
				readVal = data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
				pApp->sCfg.sTag[i].scratch_value = (float)readVal;
			}
		}
	}

	return 0;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_InitDI(SApp *pApp) {
	for(int i = 0; i < DIGITAL_INPUT_NUM_CHANNEL; i++) {
		pApp->sDI.Node[i].id = DigitalInputPin[i].pinName;
	}
	return 0;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int App_InitDO(SApp *pApp) {

	for(int i = 0; i < DIGITAL_OUTPUT_NUM_CHANNEL; i++) {
		pApp->sCfg.sDO.Node[i].port = DigitalOutputPin[i].pinName;
	}
	return 0;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_InitAI(SApp *pApp) {

	return 0;
}

/*****************************************************************************/
/** @brief get scratch value -> calculate to raw value -> calculate to std value
 *	-> get status
 *  ->
 *  @param
 *  @return Void.
 *  @note
 */
int App_UpdateTagContent(SApp *pApp) {
	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		// get main value
		if(pApp->sCfg.sTag[i].input_type == TIT_AI) {
			pApp->sCfg.sTag[i].scratch_value =
					App_GetAIValueByIndex(&pApp->sAI, pApp->sCfg.sTag[i].input_id);
		} else {
			pApp->sCfg.sTag[i].scratch_value =
					App_GetMBValueByAddress(&pApp->sMB, pApp->sCfg.sTag[i].input_id);
		}

		/* value = scratch * (raw_max - raw_min) / (scr_max - scr_min) */
		pApp->sCfg.sTag[i].raw_value = pApp->sCfg.sTag[i].scratch_value *
				(pApp->sCfg.sTag[i].raw_max - pApp->sCfg.sTag[i].raw_min) /
				(pApp->sCfg.sTag[i].scratch_max - pApp->sCfg.sTag[i].scratch_min);

		/* value = a + b * value */
		pApp->sCfg.sTag[i].raw_value = 	pApp->sCfg.sTag[i].coef_a +
									pApp->sCfg.sTag[i].raw_value * pApp->sCfg.sTag[i].coef_b;

		// get status
		if(pApp->sCfg.sTag[i].has_calib && pApp->sCfg.sTag[i].has_error) {
			bool error, calib;
			calib = App_GetDILevelByIndex(&pApp->sDI, pApp->sCfg.sTag[i].pin_calib);
			error = App_GetDILevelByIndex(&pApp->sDI, pApp->sCfg.sTag[i].pin_error);
			if(error) {
				pApp->sCfg.sTag[i].meas_stt[0] = '0';
				pApp->sCfg.sTag[i].meas_stt[1] = '2';
				pApp->sCfg.sTag[i].meas_stt[2] = 0;
			} else {
				if(calib) {
					pApp->sCfg.sTag[i].meas_stt[0] = '0';
					pApp->sCfg.sTag[i].meas_stt[1] = '1';
					pApp->sCfg.sTag[i].meas_stt[2] = 0;
				} else {
					pApp->sCfg.sTag[i].meas_stt[0] = 0;
					pApp->sCfg.sTag[i].meas_stt[1] = 0;
					pApp->sCfg.sTag[i].meas_stt[2] = 0;
				}
			}
		}
	}

	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		double comp_o2 = 1, comp_press = 1, comp_temp= 1;
		switch(pApp->sCfg.sTag[i].comp_type) {
		case CT_NONE:
			break;
		case CT_OXY:
			/*
			 * Fo2 = (20.9 - O2(std)) / (20.9 - O2(m))
			 * */
			if((20.9 - pApp->sCfg.sTag[pApp->sCfg.sTag[i].input_o2].raw_value) != 0 &&
					(20.9 - pApp->sCfg.sTag[i].o2_comp) != 0) {
					comp_o2 = (20.9 - pApp->sCfg.sTag[i].o2_comp) /
							(20.9 - pApp->sCfg.sTag[pApp->sCfg.sTag[i].input_o2].raw_value);
			} else {
				ASSERT(FALSE);
			}

			break;
		case CT_TERMP_PRESS:
			/*
			 * Ft = (273 + T(m))/(273+ T(std))
	         * Fp = P(std)/P(m)
			 *
			 * */
			if((273 + pApp->sCfg.sTag[pApp->sCfg.sTag[i].input_temp].raw_value) != 0 &&
					(273 + pApp->sCfg.sTag[i].temp_comp) != 0) {
				comp_temp = (273 + pApp->sCfg.sTag[pApp->sCfg.sTag[i].input_temp].raw_value) /
						(273 + pApp->sCfg.sTag[i].temp_comp);
			} else {
				ASSERT(FALSE);
			}

			if(pApp->sCfg.sTag[pApp->sCfg.sTag[i].input_press].raw_value != 0 &&
					pApp->sCfg.sTag[pApp->sCfg.sTag[i].input_press].raw_value != 0) {
				comp_press = pApp->sCfg.sTag[i].press_comp /
					pApp->sCfg.sTag[pApp->sCfg.sTag[i].input_press].raw_value;
			} else {
				ASSERT(FALSE);
			}

			break;

		case CT_TEMP_PRESS_OXY:
			break;

		default:
			ASSERT(FALSE);
			break;

		}

		pApp->sCfg.sTag[i].std_value = pApp->sCfg.sTag[i].raw_value *
										comp_o2 * comp_temp * comp_press;

		//LREP("%s - %.2f\r\n", pApp->sCfg.sTag[i].name,  pApp->sCfg.sTag[i].std_value);
	}

	return 0;
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
double	App_GetAIValueByIndex(SAnalogInput *pHandle, uint16_t index) {
	return pHandle->Node[index].value;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
double	App_GetMBValueByAddress(SModbusValue *pHandle, uint16_t addr) {
	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		if(pHandle->Node[i].id == addr) {
			return pHandle->Node[i].value;
		}
	}
	return 0xFFFFFFFF;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
bool App_GetDILevelByIndex(SDigitalInput *pHandle, uint16_t index) {
	return pHandle->Node[index].lev;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline int	App_InitDateTime(SApp *pApp) {

	int retVal;

	retVal = RTC_InitDateTime(&pApp->sDateTime);
	ASSERT_NONVOID(retVal == 0, retVal);

	OSA_SleepMs(100);

	retVal = RTC_GetTimeDate(&pApp->sDateTime);

	if(retVal == 0) {
		if(pApp->sDateTime.tm_year == 1990) {
			RTC_SetDateTime(0, 0, 1, 1, 2018);
		}

	} else {
		ASSERT_NONVOID(FALSE, retVal);
	}

	return retVal;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline SDateTime	App_GetDateTime(SApp *pApp) {
	return pApp->sDateTime;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline int App_SetDateTime(SApp *pApp, SDateTime time) {
	pApp->sDateTime = time;
	return RTC_SetTimeDate(&pApp->sDateTime);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline int App_SendUI(SApp *pApp, uint8_t *data, uint8_t len, bool ack) {
	return Trans_Send(&pApp->sTransUi, len, data, ack ? 0xA0 : 0x20);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline int	App_SendPC(SApp *pApp, uint8_t *data, uint8_t len, bool ack) {
	return Trans_Send(&pApp->sTransPc, len, data, ack ? 0xA0 : 0x20);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_SetNetPCCallback(SApp *pApp) {

}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_SetFTPCallback(SApp *pApp) {

}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_SetDoPinByName(SApp *pApp, const char *name, uint32_t logic) {
	for(int i = 0; i < DIGITAL_OUTPUT_NUM_CHANNEL; i++) {
		if(Str_Cmp((CPU_CHAR*)pApp->sCfg.sDO.Node[i].name, name) == 0) {
			GPIO_DRV_WritePinOutput(pApp->sCfg.sDO.Node[i].port, logic);
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
void App_DiReadAllPort(SApp *pApp) {
	for(int i = 0; i < DIGITAL_INPUT_NUM_CHANNEL; i++) {
		pApp->sDI.Node[i].lev = GPIO_DRV_ReadPinInput(pApp->sDI.Node[i].id);
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
void App_AiReadAllPort(SApp *pApp) {
	uint32_t randout;
	float randVal;
	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		 RNGA_DRV_GetRandomData(0, &randout, sizeof(uint32_t));
		 randout = abs(randout);
		 randout = randout % 100000;
		 randVal = randout / 100.0;
		 //LREP("rand out = %.2f\r\n", randVal);
		 pApp->sAI.Node[i].value = randVal;
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
int App_GenerateLogFile(SApp *pApp) {

	int retVal = FR_OK;

	if(App_IsSysStatus(pApp, SYS_ERR_SDCARD_1))
		return -1;

	FIL file;
	/* get file name time*/
	char *filename = (char*)OSA_FixedMemMalloc(64);
	char *time = (char*)OSA_FixedMemMalloc(64);
	char *year = (char*)OSA_FixedMemMalloc(16);
	char *mon = (char*)OSA_FixedMemMalloc(32);
	char *day = (char*)OSA_FixedMemMalloc(32);

	sprintf(year, "%04d", pApp->sDateTime.tm_year);
	sprintf(mon, "%04d/%02d", pApp->sDateTime.tm_year,
			pApp->sDateTime.tm_mon);
	sprintf(day, "%04d/%02d/%02d", pApp->sDateTime.tm_year,
			pApp->sDateTime.tm_mon, pApp->sDateTime.tm_mday);

	sprintf(time, "%04d%02d%02d%02d%02d%02d", pApp->sDateTime.tm_year,
			pApp->sDateTime.tm_mon, pApp->sDateTime.tm_mday,
			pApp->sDateTime.tm_hour, pApp->sDateTime.tm_min,
			pApp->sDateTime.tm_sec);

	if(!check_obj_existed(year)) {
		retVal = f_mkdir(year);
		if(retVal == FR_OK) {
			LREP("mkdir %s err = %d\r\n", year, retVal);
		} else {
			LREP("mkdir %s successful !\r\n", year);
		}
	}

	if(!check_obj_existed(mon)) {
		retVal = f_mkdir(mon);
		if(retVal == FR_OK) {
			LREP("mkdir %s err = %d\r\n", mon, retVal);
		} else {
			LREP("mkdir %s successful !\r\n", mon);
		}
	}

	if(!check_obj_existed(day)) {
		retVal = f_mkdir(day);
		if(retVal == FR_OK) {
			LREP("mkdir %s err = %d\r\n", day, retVal);
		} else {
			LREP("mkdir %s successful !\r\n", day);
		}
	}


	if(retVal == FR_OK) {
		sprintf((char*)filename, "/%s_%s_%s_%s.txt", pApp->sCfg.sCom.tinh,
				pApp->sCfg.sCom.coso, pApp->sCfg.sCom.tram, time);

		memset(pApp->currFileName, 0, 256);
		Str_Cat((char*)pApp->currFileName, day);
		Str_Cat((char*)pApp->currFileName, filename);

		LREP("create file: %s\r\n", pApp->currFileName);

		retVal = f_open(&file, (char*)pApp->currFileName, FA_OPEN_ALWAYS | FA_WRITE);

		if(retVal == FR_OK) {
			char *row = (char*)OSA_FixedMemMalloc(256);
			UINT written;

			sprintf(row, "%-12s %12s %12s %15s %12s\r\n", "Thong so", "Gia tri", "Don vi", "Thoi gian", "Trang thai");
			retVal = f_write(&file, row, Str_Len(row), &written);
			LREP("%s", row);
			for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
				if(pApp->sCfg.sTag[i].report) {
					memset(row, 0, 256);
					sprintf(row, "%-12s %12.2f %12s %15s %12s\r\n",
							pApp->sCfg.sTag[i].name,
							pApp->sCfg.sTag[i].std_value,
							pApp->sCfg.sTag[i].std_unit,
							time,
							pApp->sCfg.sTag[i].meas_stt);

					LREP("%s", row);

					retVal = f_write(&file, row, Str_Len(row), &written);
					if(retVal != FR_OK || written <= 0)
						break;
				}
			}
			OSA_FixedMemFree((uint8_t*)row);

			if(retVal == FR_OK) {
				// TODO: send file name to Net module
			}
		}

		f_close(&file);
	}

    OSA_FixedMemFree((uint8_t*)filename);
    OSA_FixedMemFree((uint8_t*)time);
    OSA_FixedMemFree((uint8_t*)year);
    OSA_FixedMemFree((uint8_t*)mon);
    OSA_FixedMemFree((uint8_t*)day);

    return retVal;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_GenerateFakeTime(SApp *pApp) {
	pApp->sDateTime.tm_sec++;

	if(pApp->sDateTime.tm_sec > 59) {
		pApp->sDateTime.tm_sec = 0;
		pApp->sDateTime.tm_min++;
	}

	if(pApp->sDateTime.tm_min > 59) {
		pApp->sDateTime.tm_min = 0;
		pApp->sDateTime.tm_hour++;
	}

	if(pApp->sDateTime.tm_hour > 23) {
		pApp->sDateTime.tm_hour = 0;
		pApp->sDateTime.tm_mday++;
	}

	if(pApp->sDateTime.tm_mday > 30) {
		pApp->sDateTime.tm_mday = 0;
		pApp->sDateTime.tm_mon++;
	}

	return 0;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void sdhc_card_detection(void)
{
    pAppObj->sdhcPlugged = BOARD_IsSDCardDetected();
    if(!pAppObj->sdhcPlugged) {
    	NVIC_SystemReset();
    } else {
    	if(App_IsSysStatus(pAppObj, SYS_ERR_SDCARD_1)) {
			OS_ERR err;
			App_SetCtrlCode(pAppObj, CTRL_INIT_SDCARD_1);
			OSTaskSemPost(&pAppObj->TCB_task_startup, OS_OPT_NONE, &err);
			ASSERT(err == OS_ERR_NONE);
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

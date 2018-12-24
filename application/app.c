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
#include <lib_str.h>
#include <lib_mem.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
void Clb_TransPC_RecvEvent(void *pData, uint8_t u8Type);
void Clb_TransPC_SentEvent(void *pDatam, uint8_t u8Type);
void Clb_TransUI_RecvEvent(void *pData, uint8_t u8Type);
void Clb_TransUI_SentEvent(void *pDatam, uint8_t u8Type);


//void Clb_NetStatus(Network_ConnEvent event, Network_Interface interface);

void Clb_NetTcpClientReceivedData(const char* data, int length);
void Clb_NetTcpClientSentData(const char* data, int length);
void Clb_NetTcpClientError(const char* data, int length);

void Clb_NetTcpServerReceivedData(const char* data, int length);
void Clb_NetTcpServerSendDone(const char* data, int length);
void Clb_NetTcpServerError(const char* data, int length);

/************************** Variable Definitions *****************************/
SApp sApp;
SApp *pAppObj = &sApp;
APP_TASK_DEFINE(task_ai, 			TASK_AI_STACK_SIZE);
APP_TASK_DEFINE(task_shell, 		TASK_SHELL_STACK_SIZE);
APP_TASK_DEFINE(task_ui, 			TASK_UI_STACK_SIZE);
APP_TASK_DEFINE(task_modbus, 		TASK_MODBUS_STACK_SIZE);
APP_TASK_DEFINE(task_serialcomm,	TASK_SERIAL_COMM_STACK_SIZE);
APP_TASK_DEFINE(task_periodic,		TASK_PERIODIC_STACK_SIZE);
APP_TASK_DEFINE(task_startup,		TASK_STARTUP_STACK_SIZE);

task_stack_t	task_ai_stack[TASK_AI_STACK_SIZE/ sizeof(task_stack_t)];
task_stack_t	task_shell_stack[TASK_SHELL_STACK_SIZE/ sizeof(task_stack_t)];
task_stack_t	task_ui_stack[TASK_UI_STACK_SIZE/ sizeof(task_stack_t)];
task_stack_t	task_modbus_stack[TASK_MODBUS_STACK_SIZE/ sizeof(task_stack_t)];
task_stack_t	task_serialcomm_stack[TASK_SERIAL_COMM_STACK_SIZE/ sizeof(task_stack_t)];
task_stack_t	task_periodic_stack[TASK_PERIODIC_STACK_SIZE/ sizeof(task_stack_t)];
task_stack_t	task_startup_stack[TASK_STARTUP_STACK_SIZE/ sizeof(task_stack_t)];

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

	int err;
	pApp->eStatus = SYS_ERR_NONE;
	pApp->stat = false;
	pApp->reboot = false;

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

    LREP("sizeof(SCommon) %d\r\n", 		sizeof(SCommon));
    LREP("sizeof(STag) %d\r\n", 		sizeof(STag));
    LREP("sizeof(SInputPort) %d\r\n", 	sizeof(SInputPort));
    LREP("sizeof(SCtrlPort) %d\r\n",	sizeof(SCtrlPort));
    LREP("sizeof(STagValue) %d\r\n", 	sizeof(STagNode));
    LREP("sizeof(SSysCfg) %d\r\n", 		sizeof(SSysCfg));
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
			if(read == sizeof(SSysCfg)) {
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

	if(retVal == -2) {
		f_unlink(cfg_path);
		LREP("wrong config file, generate default\r\n");
		ASSERT(0);
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

	Str_Copy((CPU_CHAR*)pHandle->sCom.coso, "COSO");
	Str_Copy((CPU_CHAR*)pHandle->sCom.tinh, "TINH");
	Str_Copy((CPU_CHAR*)pHandle->sCom.tram, "TRAM");

	Str_Copy((CPU_CHAR*)pHandle->sAccount.username, "admin");
	Str_Copy((CPU_CHAR*)pHandle->sAccount.password, "admin");

	Str_Copy((CPU_CHAR*)pHandle->sAccount.rootname, "root");
	Str_Copy((CPU_CHAR*)pHandle->sAccount.rootpass, "123456a@");

	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_usrname1, "ftpuser1");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_passwd1, "ftppasswd1");

	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_usrname2, "ftpuser2");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_passwd2, "ftppasswd2");

	Str_Copy((CPU_CHAR*)pHandle->sCom.ctrl_usrname, "ctrluser");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ctrl_passwd, "ctrlpasswd");

	pHandle->sCom.scan_dur 	= 1;
	pHandle->sCom.log_dur 	= 1;	//5;
	pHandle->sCom.modbus_brate = 9600;

	pHandle->sCom.ftp_enable1 = TRUE;
	pHandle->sCom.ftp_enable2 = FALSE;

	IP4_ADDR(&pHandle->sCom.dev_ip, 192,168,1,2);
	IP4_ADDR(&pHandle->sCom.server_ftp_ip1, 192,168,1,12);
	pHandle->sCom.server_ftp_port1 = 21;
	IP4_ADDR(&pHandle->sCom.server_ftp_ip2, 192,168,1,12);
	pHandle->sCom.server_ftp_port2 = 21;
	IP4_ADDR(&pHandle->sCom.server_ctrl_ip, 192,168,1,22);
	pHandle->sCom.server_ctrl_port = 1186;

	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		App_DefaultTag(&pHandle->sTag[i], i);
	}

	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		pHandle->sAiCalib.calib[i].offset = 0;
		pHandle->sAiCalib.calib[i].coefficient = 1;
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
	pHandle->input_id = 2;	//tagIdx;
	pHandle->input_id = MIN(pHandle->input_id, SYSTEM_NUM_TAG - 1);
	pHandle->pin_calib = MIN(pHandle->pin_calib, DIGITAL_INPUT_NUM_CHANNEL - 1);
	pHandle->pin_error = MIN(pHandle->pin_error, DIGITAL_INPUT_NUM_CHANNEL - 1);
	pHandle->enable = true;
	pHandle->report = true;
	pHandle->has_error = false;
	pHandle->has_calib = false;
	pHandle->alarm_enable = false;
	pHandle->data_type = Read_Holding_Register;
	pHandle->data_format = Integer_16bits;
	pHandle->input_type = TIT_MB;
	pHandle->slave_reg_addr = tagIdx;
	pHandle->slave_reg_addr = MIN(pHandle->slave_reg_addr, 9);
	pHandle->raw_min = 0;
	pHandle->raw_max = 1000;
	pHandle->scratch_min = 0;
	pHandle->scratch_max = 1000;
	pHandle->coef_a = 0;
	pHandle->coef_b = 1;

	Str_Copy_N((CPU_CHAR*)pHandle->name, "NAME", sizeof(pHandle->name));
	Str_Copy_N((CPU_CHAR*)pHandle->raw_unit, "RUNIT", sizeof(pHandle->raw_unit));
	Str_Copy_N((CPU_CHAR*)pHandle->std_unit, "SUNIT", sizeof(pHandle->std_unit));

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
int App_SetConfig(SApp *pApp, const uint8_t *pData, bool serial) {
	switch(pData[0]) {
	case LOGGER_SET | LOGGER_COMMON:
		if(pData[1] == sizeof(SCommon)) {
			/*SCommon *comm = (SCommon*)OSA_FixedMemMalloc(sizeof(SCommon));
			memcpy(comm, &pData[2], sizeof(SCommon));
			print_comm(comm);
			OSA_FixedMemFree((uint8_t*)comm);*/
			memcpy(&pApp->sCfg.sCom, &pData[2], sizeof(SCommon));

		} else {
			ASSERT_NONVOID(FALSE, -1);
		}
		break;

	case LOGGER_SET | LOGGER_TAG: {
			uint8_t idx;
			if(pData[1] == (sizeof(STag) + 1)) {
				idx = pData[2];
				LREP("set for tag %d\r\n", idx);
				/* Note: if has some invalid in tag config content
				 * copy from data frame to config area can cause
				 * crash (trans fifo error) */
				if(idx <= SYSTEM_NUM_TAG - 1) {

					/*STag *tag = (STag*)OSA_FixedMemMalloc(sizeof(STag));
					memcpy(tag, &pData[3], sizeof(STag));
					print_tag1(tag);
					OSA_FixedMemFree((uint8_t*)tag);*/

					memcpy(&pApp->sCfg.sTag[idx],
							&pData[3], sizeof(STag));

				} else {
					ASSERT(FALSE);
				}
			} else {
				LREP("recv tag len = %d\r\n", pData[1]);
				ASSERT_NONVOID(FALSE, -2);
			}
		}
		break;

	case LOGGER_SET | LOGGER_DI:{
			uint8_t idx;
			if(pData[1] == (sizeof(SInputPort) + 1)) {
				idx = pData[2];
				LREP("set for di %d\r\n", idx);
				if(idx <= DIGITAL_INPUT_NUM_CHANNEL - 1) {
					memcpy(&pApp->sCfg.sDI[idx], &pData[3], sizeof(SInputPort));
				} else {
					ASSERT(FALSE);
				}
			} else {
				LREP("recv di len = %d\r\n", pData[1]);
				ASSERT_NONVOID(FALSE, -3);
			}
		}
		break;

	case LOGGER_SET | LOGGER_DO:{
			uint8_t idx;
			if(pData[1] == (sizeof(SCtrlPort) + 1)) {
				idx = pData[2];
				LREP("set for do %d\r\n", idx);
				if(idx <= DIGITAL_OUTPUT_NUM_CHANNEL - 1) {
					memcpy(&pApp->sCfg.sDO[idx], &pData[3], sizeof(SCtrlPort));
				} else {
					ASSERT(FALSE);
				}
			} else {
				LREP("recv do len = %d\r\n", pData[1]);
				ASSERT_NONVOID(FALSE, -4);
			}
		}
	break;

	case LOGGER_SET | LOGGER_WRITE_DONE:
		LREP("write config done\r\n");
		App_SaveConfig(pApp, CONFIG_FILE_PATH);
		pApp->reboot = true;
	break;

	default:
		ASSERT_NONVOID(FALSE, -5);
		break;
	}

	int ret;
	if(serial)
		ret = App_SendPC(pApp, LOGGER_SET | LOGGER_WRITE_SUCCESS, NULL, 0, false);
	else
		ret = App_SendPCNetworkClient(LOGGER_SET | LOGGER_WRITE_SUCCESS, NULL, 0);

	return ret;
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

//	switch(cfg) {
//	case CFG_COMMON | CFG_GET:
//	if(type == CFG_CONN_SERIAL) {
//		uint8_t *sendData = OSA_FixedMemMalloc(sizeof(SCommon) + 2);
//		sendData[0] = CFG_COMMON | CFG_GET;
//		sendData[1] = sizeof(SCommon);
//		memcpy(&sendData[2], &pApp->sCfg.sCom, sizeof(SCommon));
//		App_SendPC(pApp, sendData, sizeof(SCommon) + 2, true);
//		OSA_FixedMemFree(sendData);
//	}
//	break;
//
//	case CFG_TAG | CFG_GET:
//	if(type == CFG_CONN_SERIAL) {
//		uint8_t *sendData = OSA_FixedMemMalloc(sizeof(STag) + 2);
//		sendData[0] = CFG_TAG | CFG_GET;
//		sendData[1] = sizeof(STag);
//		memcpy(&sendData[2], &pApp->sCfg.sTag[idx], sizeof(STag));
//		App_SendPC(pApp, sendData, sizeof(STag) + 2, true);
//		OSA_FixedMemFree(sendData);
//	}
//	break;
//
//	default:
//		break;
//	}

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

	LREP("start init FS\r\n");

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



	rnga_user_config_t rngaConfig;

	// Initialize RNGA
	rngaConfig.isIntMasked         = true;
	rngaConfig.highAssuranceEnable = true;

	RNGA_DRV_Init(0, &rngaConfig);

	pApp->sDateTime.tm_mday = 1;
	pApp->sDateTime.tm_mon = 1;
	pApp->sDateTime.tm_year = 2018;

	LREP("log min = %d\r\n", log_min);

#if NETWORK_MODULE_EN > 0
	App_InitNetworkModule(pApp);
#endif

	//Network_InitModule(&pApp->sCfg.sCom);

	static uint8_t test = 0;
	while(1) {
		OSA_SleepMs(1000);
//		if(RTC_GetTimeDate(&pApp->sDateTime) == 0) {
//			 LREP("Current Time: %04d/%02d/%02d %02d:%02d:%02d\r\n",
//					pApp->sDateTime.tm_year, pApp->sDateTime.tm_mon,
//					pApp->sDateTime.tm_mday, pApp->sDateTime.tm_hour,
//					pApp->sDateTime.tm_min, pApp->sDateTime.tm_sec);
//			//LREP(".");
//			//GPIO_DRV_TogglePinOutput(kGpioLEDGREEN);
//
//		}


		App_DiReadAllPort(pApp);
		App_UpdateTagContent(pApp);

//        if(pApp->sDateTime.tm_min != last_min && logged) {
//            logged = false;
//        }
//
//		if((logged == false) && (pApp->sDateTime.tm_min % log_min == 0)) {
//			if(pApp->sCfg.sCom.ftp_enable1 ||
//					pApp->sCfg.sCom.ftp_enable2) {
//				LREP("generate log file\r\n");
//				ASSERT(App_GenerateLogFile(pApp) == FR_OK);
//			} else {
//				LREP("disabled generate log file\r\n");
//			}
//			last_min = pApp->sDateTime.tm_min;
//			logged = true;
//		}

		if(test++ % 5 == 0) {
			App_GenerateLogFile(pApp);
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
	CPU_TS	ts;
	shell_init(cmd_table, my_shell_init);
	LREP(SHELL_PROMPT);
	while(1) {
		OSSemPend(&debug_sem, 1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			shell_task(NULL);
		}
	}
}


/*****************************************************************************/
/** @brief do read all modubs channel with read rate = sCfg.sCom.scan_dur
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_TaskModbus(task_param_t param)
{
	SApp *pApp = (SApp *)param;

	App_InitModbus(pApp);

	while (1)
	{
		App_ModbusDoRead(pApp);
		WDOG_DRV_Refresh();
		OSA_SleepMs(1000);
		WDOG_DRV_Refresh();
	}

}

/*****************************************************************************/
/** @brief do read all modubs channel with read rate = sCfg.sCom.scan_dur
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_TaskAnalogIn(task_param_t param) {
	SApp *pApp = (SApp *)param;
	App_InitAI(pApp);
	while (1) {
		App_AiReadAllPort(pApp);
		//OSA_SleepMs(1000);
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

	ASSERT(OSA_SemaCreate(&pApp->semTransPc, 0) == kStatus_OSA_Success);
	TransPC_RegisterClbEvent(&pApp->sTransPc, TRANS_EVT_RECV_DATA, Clb_TransPC_RecvEvent);
	TransPC_RegisterClbEvent(&pApp->sTransPc, TRANS_EVT_SENT_DATA, Clb_TransPC_SentEvent);

	TransPC_Init(&pApp->sTransPc, BOARD_TRANSPC_UART_INSTANCE,
			BOARD_TRANSPC_UART_BAUD, &pApp->semTransPc);

	LREP("PC PORT: %d _______\r\n", BOARD_TRANSPC_UART_INSTANCE);
	//LREP("SFrameInfo Size: %d\r\n", sizeof(SFrameInfo));
	while(1) {
		OSSemPend(&pApp->semTransPc, 1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			TransPC_Task(&pApp->sTransPc);
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
	ASSERT(OSA_SemaCreate(&pApp->semTransUi, 0) == kStatus_OSA_Success);
	TransUI_RegisterClbEvent(&pApp->sTransUi, TRANS_EVT_RECV_DATA, Clb_TransUI_RecvEvent);
	TransUI_RegisterClbEvent(&pApp->sTransUi, TRANS_EVT_SENT_DATA, Clb_TransUI_SentEvent);

	TransUI_Init(&pApp->sTransUi, BOARD_TRANSUI_UART_INSTANCE,
			BOARD_TRANSUI_UART_BAUD, &pApp->semTransUi);

	LREP("UI PORT: %d _______\r\n", BOARD_TRANSUI_UART_INSTANCE);

	OSTmrStart(&pApp->hCtrlTimer, &err);
	if (err == OS_ERR_NONE) {
		/* Timer was created but NOT started */
		LREP("timer started ok\r\n");
	} else {
		LREP("timer start failed\r\n");
	}

	while(1) {
		OSSemPend(&pApp->semTransUi, 1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			TransUI_Task(&pApp->sTransUi);
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

	App_CreateAppEvent(pApp);

	App_CreateAppTask(pApp);

	OSTmrCreate(&pApp->hCtrlTimer,
				(CPU_CHAR *)"timer",
				(OS_TICK)0,
				(OS_TICK)100,
				(OS_OPT)OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) Clb_TimerControl,
				(void*)NULL,
				(OS_ERR*)&err);

	ASSERT(err == OS_ERR_NONE);

	App_OS_SetAllHooks();

#if OS_CFG_STAT_TASK_EN == DEF_ENABLED
    OSStatTaskCPUUsageInit(&err);
    OSStatReset(&err);
#endif


	while(1) {

		OSSemPend(&pApp->hSem, 1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {

			/* Check all pending command */
			if(App_IsCtrlCodePending(pApp, CTRL_INIT_SDCARD_1)) {
			    pAppObj->sdhcPlugged = BOARD_IsSDCardDetected();
			    LREP("card event = %d\r\n", pAppObj->sdhcPlugged);

			    if(!pAppObj->sdhcPlugged) {
			    	NVIC_SystemReset();
			    } else {
			    	LREP("recv ctrl init sdcard 1\r\n");
					int err = App_InitFS(pApp);
					if(err != FR_OK) {
						App_SetSysStatus(pApp, SYS_ERR_SDCARD_1);
					} else {
						App_ClearSysStatus(pApp, SYS_ERR_SDCARD_1);
						ASSERT(App_LoadConfig(pApp, CONFIG_FILE_PATH) == FR_OK);
					}
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


			//WDOG_DRV_Refresh();
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
void App_CommRecvHandle(const uint8_t *data) {
	uint8_t sendData[30];
	switch(data[0]) {

	case LOGGER_LOGIN: {
		bool normal = (Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.username,
					(CPU_CHAR*)&data[2], SYS_CFG_USER_LENGTH) == 0) &&
					(Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.password,
					(CPU_CHAR*)&data[SYS_CFG_USER_LENGTH + 2], SYS_CFG_PASSWD_LENGTH) == 0);

		bool root = (Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.rootname,
					(CPU_CHAR*)&data[2], SYS_CFG_USER_LENGTH) == 0) &&
					(Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.rootpass,
					(CPU_CHAR*)&data[SYS_CFG_USER_LENGTH + 2], SYS_CFG_PASSWD_LENGTH) == 0);

		if(normal || root) {
			sendData[0] = LOGGER_SUCCESS;
			LREP("login as %s user\r\n", root ? "root" : "normal");
		} else {
			sendData[0] = LOGGER_ERROR;
			LREP("invalid username %s password %s\r\n",
					&data[2], &data[2 + SYS_CFG_PASSWD_LENGTH]);
		}

		App_SendPC(pAppObj, LOGGER_LOGIN, sendData, 1, false);
	}
		break;
	case LOGGER_SET | LOGGER_COMMON:
	case LOGGER_SET | LOGGER_TAG:
	case LOGGER_SET | LOGGER_DI:
	case LOGGER_SET | LOGGER_DO:
	case LOGGER_SET | LOGGER_WRITE_DONE:
		App_SetConfig(pAppObj, data, true);
		break;
	case LOGGER_SET | LOGGER_TIME:

		memcpy((uint8_t*)&pAppObj->sDateTime, &data[2], sizeof(SDateTime));
		 LREP("Current Time: %04d/%02d/%02d %02d:%02d:%02d\r\n",
				 pAppObj->sDateTime.tm_year, pAppObj->sDateTime.tm_mon,
				 pAppObj->sDateTime.tm_mday, pAppObj->sDateTime.tm_hour,
				 pAppObj->sDateTime.tm_min,  pAppObj->sDateTime.tm_sec);
		 App_SendPC(pAppObj, LOGGER_SET | LOGGER_TIME, NULL, 0, false);
		break;
	case LOGGER_CHANGE_PASSWD:

		break;

	case LOGGER_GET | LOGGER_STREAM_AI:
		App_SendPC(pAppObj, LOGGER_GET | LOGGER_STREAM_AI,
				(uint8_t*)&pAppObj->sAI, sizeof(SAnalogInput), false);
	break;

	case LOGGER_GET | LOGGER_STREAM_MB:
		App_SendPC(pAppObj, LOGGER_GET | LOGGER_STREAM_MB,
				(uint8_t*)&pAppObj->sMB, sizeof(SModbusValue), false);
	break;

	case LOGGER_GET | LOGGER_STREAM_VALUE:
	{

		if(data[2] & STREAM_AI) {
			App_SendPC(pAppObj, LOGGER_GET | LOGGER_STREAM_AI,
							(uint8_t*)&pAppObj->sAI, sizeof(SAnalogInput), false);
		}

		if(data[2] & STREAM_MB) {
			App_SendPC(pAppObj, LOGGER_GET | LOGGER_STREAM_MB,
							(uint8_t*)&pAppObj->sMB, sizeof(SModbusValue), false);
		}

		if(data[2] & STREAM_VALUE) {
			STagVArray *pMem = (STagVArray*)OSA_FixedMemMalloc(sizeof(STagVArray));
			if(pMem != NULL) {
				memset(pMem, 0, sizeof(STagVArray));
				for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
					pMem->Node[i].raw_value = pAppObj->sTagValue.Node[i].raw_value;
					pMem->Node[i].std_value = pAppObj->sTagValue.Node[i].std_value;
					pMem->Node[i].meas_stt[0] = pAppObj->sTagValue.Node[i].meas_stt[0];
					pMem->Node[i].meas_stt[1] = pAppObj->sTagValue.Node[i].meas_stt[1];
					pMem->Node[i].meas_stt[2] = pAppObj->sTagValue.Node[i].meas_stt[2];
				}
				App_SendPC(pAppObj, LOGGER_GET | LOGGER_STREAM_VALUE,
						(uint8_t*)pMem, sizeof(STagVArray), false);

				OSA_FixedMemFree((uint8_t*)pMem);
			}
		}
	}
	break;

	case LOGGER_GET | LOGGER_STREAM_HEADER: {
		pAppObj->pcCounter = 0;
		App_SetCtrlCode(pAppObj, CTRL_SEND_HEADER);
	}
	break;

	case LOGGER_SET | LOGGER_CALIB_AI:
		App_CommCalibAi(pAppObj, data);
	break;

	case LOGGER_SET | LOGGER_CALIB_CURR_PWR:

	break;

	default:
		LREP("unhandled msg type: 0x%02x\r\n", data[0]);
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
void App_NetRecvHandle(const uint8_t *data) {
	uint8_t sendData[30];
	switch(data[0]) {

	case LOGGER_LOGIN: {
		bool normal = (Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.username,
					(CPU_CHAR*)&data[2], SYS_CFG_USER_LENGTH) == 0) &&
					(Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.password,
					(CPU_CHAR*)&data[SYS_CFG_USER_LENGTH + 2], SYS_CFG_PASSWD_LENGTH) == 0);

		bool root = (Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.rootname,
					(CPU_CHAR*)&data[2], SYS_CFG_USER_LENGTH) == 0) &&
					(Str_Cmp_N((CPU_CHAR*)pAppObj->sCfg.sAccount.rootpass,
					(CPU_CHAR*)&data[SYS_CFG_USER_LENGTH + 2], SYS_CFG_PASSWD_LENGTH) == 0);

		if(normal || root) {
			sendData[0] = LOGGER_SUCCESS;
			LREP("login as %s user\r\n", root ? "root" : "normal");
		} else {
			sendData[0] = LOGGER_ERROR;
			LREP("invalid username %s password %s\r\n",
					&data[2], &data[2 + SYS_CFG_PASSWD_LENGTH]);
		}

		App_SendPCNetworkClient(LOGGER_LOGIN, sendData, 1);
	}
		break;
	case LOGGER_SET | LOGGER_COMMON:
	case LOGGER_SET | LOGGER_TAG:
	case LOGGER_SET | LOGGER_DI:
	case LOGGER_SET | LOGGER_DO:
	case LOGGER_SET | LOGGER_WRITE_DONE:
		App_SetConfig(pAppObj, data, false);
		break;
	case LOGGER_SET | LOGGER_TIME:

		memcpy((uint8_t*)&pAppObj->sDateTime, &data[2], sizeof(SDateTime));
		 LREP("Current Time: %04d/%02d/%02d %02d:%02d:%02d\r\n",
				 pAppObj->sDateTime.tm_year, pAppObj->sDateTime.tm_mon,
				 pAppObj->sDateTime.tm_mday, pAppObj->sDateTime.tm_hour,
				 pAppObj->sDateTime.tm_min,  pAppObj->sDateTime.tm_sec);
		 App_SendPCNetworkClient(LOGGER_SET | LOGGER_TIME, NULL, 0);
		break;
	case LOGGER_CHANGE_PASSWD:

		break;

	case LOGGER_GET | LOGGER_STREAM_AI:
	App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_AI,
				(uint8_t*)&pAppObj->sAI, sizeof(SAnalogInput));
	break;

	case LOGGER_GET | LOGGER_STREAM_MB:
	App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_MB,
				(uint8_t*)&pAppObj->sMB, sizeof(SModbusValue));
	break;

	case LOGGER_GET | LOGGER_STREAM_VALUE:
	{

		if(data[2] & STREAM_AI) {
			App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_AI,
							(uint8_t*)&pAppObj->sAI, sizeof(SAnalogInput));
		}

		if(data[2] & STREAM_MB) {
			App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_MB,
							(uint8_t*)&pAppObj->sMB, sizeof(SModbusValue));
		}

		if(data[2] & STREAM_VALUE) {
			STagVArray *pMem = (STagVArray*)OSA_FixedMemMalloc(sizeof(STagVArray));
			if(pMem != NULL) {
				for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
					pMem->Node[i].raw_value = pAppObj->sTagValue.Node[i].raw_value;
					pMem->Node[i].std_value = pAppObj->sTagValue.Node[i].std_value;
					pMem->Node[i].meas_stt[0] = pAppObj->sTagValue.Node[i].meas_stt[0];
					pMem->Node[i].meas_stt[1] = pAppObj->sTagValue.Node[i].meas_stt[1];
					pMem->Node[i].meas_stt[2] = pAppObj->sTagValue.Node[i].meas_stt[2];
				}
				App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_VALUE,
						(uint8_t*)pMem, sizeof(STagVArray));

				OSA_FixedMemFree((uint8_t*)pMem);
			}
		}
	}
	break;

	case LOGGER_GET | LOGGER_STREAM_HEADER: {
		pAppObj->pcCounter = 0;
		App_SetCtrlCode(pAppObj, CTRL_SEND_HEADER);
	}
	break;

	case LOGGER_SET | LOGGER_CALIB_AI:
		App_CommCalibAi(pAppObj, data);
	break;

	case LOGGER_SET | LOGGER_CALIB_CURR_PWR:

	break;

	default:
		LREP("unhandled msg type: 0x%02x\r\n", data[0]);
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

void App_CommCalibAi(SApp *pApp, const uint8_t *data) {
	bool isSave = false;
	if(data[2] == 0) {
		LREP("calib 4mA\r\n");
	} else {
		LREP("calib 20mA\r\n");
	}
	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		if(pApp->sAI.Node[i].status == TAG_STT_OK) {
			if(data[2] == 0) {// calib at 4mA
				pApp->sCfg.sAiCalib.calib[i].x1 =
					pApp->sCfg.sAiCalib.calib[i].raw - 1;
			} else {// calib at 20mA
				isSave = true;
				pApp->sCfg.sAiCalib.calib[i].x2 =
					pApp->sCfg.sAiCalib.calib[i].raw + 1;
				if((pApp->sCfg.sAiCalib.calib[i].x2 -
						pApp->sCfg.sAiCalib.calib[i].x1) != 0) {
				pApp->sCfg.sAiCalib.calib[i].coefficient = 16 /
						(pApp->sCfg.sAiCalib.calib[i].x2 -
						pApp->sCfg.sAiCalib.calib[i].x1);
				pApp->sCfg.sAiCalib.calib[i].offset = 4 -
						(pApp->sCfg.sAiCalib.calib[i].coefficient *
						pApp->sCfg.sAiCalib.calib[i].x1);
				}
			}
		}
	}

	if(isSave) {
		App_SaveConfig(pApp, CONFIG_FILE_PATH);
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
void App_CommCalibCurrPwr(SApp *pApp, const uint8_t *data) {

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
void Clb_TransPC_RecvEvent(void *pData, uint8_t u8Type) {
	//static uint32_t count = 0;
	SFrameInfo *pFrameInfo = (SFrameInfo *)MEM_BODY((SMem*)pData);
	LREP("Recv frm type %x dlen: %d\r\n",
			pFrameInfo->pu8Data[0], pFrameInfo->u16DLen);

	switch(u8Type & 0x3F) {
	case FRM_DATA:
		App_CommRecvHandle(pFrameInfo->pu8Data);
		break;


	default:
		ASSERT(false);
		break;
	}
}

void Clb_TransPC_SentEvent(void *pData, uint8_t u8Type) {

	SFrameInfo *pFrame = (SFrameInfo*)MEM_BODY((SMem*)pData);


	if(App_IsCtrlCodePending(pAppObj, CTRL_SEND_HEADER)) {
		if(pAppObj->pcCounter < SYSTEM_NUM_TAG) {
			uint8_t *mem = OSA_FixedMemMalloc(sizeof(STagHeader) + 1);
			if(mem != NULL) {
				STagHeader *hdr = (STagHeader*)((uint8_t*)mem + 1);
				mem[0] = (uint8_t)pAppObj->pcCounter;
				hdr->id = pAppObj->sCfg.sTag[pAppObj->pcCounter].id;
				hdr->enable = pAppObj->sCfg.sTag[pAppObj->pcCounter].enable;
				hdr->min = pAppObj->sCfg.sTag[pAppObj->pcCounter].raw_min;
				hdr->max = pAppObj->sCfg.sTag[pAppObj->pcCounter].raw_max;
				hdr->alarm_value = pAppObj->sCfg.sTag[pAppObj->pcCounter].alarm_value;
				hdr->alarm_enable = pAppObj->sCfg.sTag[pAppObj->pcCounter].alarm_enable;
				Str_Copy_N((CPU_CHAR*)hdr->name,
						(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->pcCounter].name,
						sizeof(hdr->name));
				Str_Copy_N((CPU_CHAR*)hdr->raw_unit,
						(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->pcCounter].raw_unit,
						sizeof(hdr->raw_unit));
				Str_Copy_N((CPU_CHAR*)hdr->std_unit,
						(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->pcCounter].std_unit,
						sizeof(hdr->std_unit));
				App_SendPC(pAppObj, LOGGER_GET | LOGGER_STREAM_HEADER,
						(uint8_t*)mem, sizeof(STagHeader) + 1, false);

				LREP("send header %d\r\n", pAppObj->pcCounter);
				App_SetCtrlCode(pAppObj, CTRL_SEND_HEADER);
				pAppObj->pcCounter++;
				OSA_FixedMemFree(mem);
			}
		} else {
			App_ClearCtrlCode(pAppObj, CTRL_SEND_HEADER);
			pAppObj->pcCounter = 0;
		}
	}


	switch(u8Type & 0x3F) {
	case FRM_DATA:
		if(pFrame->pu8Data[0] == (LOGGER_SET | LOGGER_WRITE_SUCCESS)) {
			if(pAppObj->reboot) {
				NVIC_SystemReset();
			}
		}
		break;

	default:
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
void Clb_TransUI_RecvEvent(void *pData, uint8_t u8Type) {
	LREP("ui recv frame event\r\n");
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TransUI_SentEvent(void *pDatam, uint8_t u8Type) {
	//LREP("ui send done event \r\n");
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
	static uint8_t counter = 0;

	App_GenerateFakeTime(pAppObj);

	if(counter % 15 == 0) {
		pAppObj->uiCounter = 0;
		SSystemStatus *pSys = (SSystemStatus*)OSA_FixedMemMalloc(sizeof(SSystemStatus));
		if(pSys != NULL) {
			memset(pSys, 0, sizeof(SSystemStatus));
			memcpy(&pSys->time, &pAppObj->sDateTime, sizeof(SDateTime));
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_SYSTEM_STATUS,
					(uint8_t*)pSys, sizeof(SSystemStatus), false);
			OSA_FixedMemFree((uint8_t*)pSys);
		}
		//LREP("send system status\r\n");
	}

	if(pAppObj->uiCounter < SYSTEM_NUM_TAG) {
		uint8_t *mem = OSA_FixedMemMalloc(sizeof(STagHeader) + 1);
		if(mem != NULL) {
			STagHeader *hdr = (STagHeader*)((uint8_t*)mem + 1);
			mem[0] = (uint8_t)pAppObj->uiCounter;
			hdr->id = pAppObj->sCfg.sTag[pAppObj->uiCounter].id;
			hdr->enable = pAppObj->sCfg.sTag[pAppObj->uiCounter].enable;
			hdr->min = pAppObj->sCfg.sTag[pAppObj->uiCounter].raw_min;
			hdr->max = pAppObj->sCfg.sTag[pAppObj->uiCounter].raw_max;
			hdr->alarm_value = pAppObj->sCfg.sTag[pAppObj->uiCounter].alarm_value;
			hdr->alarm_enable = pAppObj->sCfg.sTag[pAppObj->uiCounter].alarm_enable;
			Str_Copy_N((CPU_CHAR*)hdr->name,
					(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->uiCounter].name,
					sizeof(hdr->name));
			Str_Copy_N((CPU_CHAR*)hdr->raw_unit,
					(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->uiCounter].raw_unit,
					sizeof(hdr->raw_unit));
			Str_Copy_N((CPU_CHAR*)hdr->std_unit,
					(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->uiCounter].std_unit,
					sizeof(hdr->std_unit));
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_HEADER,
					(uint8_t*)mem, sizeof(STagHeader) + 1, false);

			//LREP("send header %d\r\n", pAppObj->uiCounter);
			OSA_FixedMemFree(mem);
			pAppObj->uiCounter++;
		}
	} else {
		STagVArray *pMem = (STagVArray*)OSA_FixedMemMalloc(sizeof(STagVArray));
		if(pMem != NULL) {
			for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
				pMem->Node[i].raw_value = pAppObj->sTagValue.Node[i].raw_value;
				pMem->Node[i].std_value = pAppObj->sTagValue.Node[i].std_value;
				pMem->Node[i].meas_stt[0] = pAppObj->sTagValue.Node[i].meas_stt[0];
				pMem->Node[i].meas_stt[1] = pAppObj->sTagValue.Node[i].meas_stt[1];
				pMem->Node[i].meas_stt[2] = pAppObj->sTagValue.Node[i].meas_stt[2];
			}
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_VALUE,
					(uint8_t*)pMem, sizeof(STagVArray), false);

			OSA_FixedMemFree((uint8_t*)pMem);
		}
	}

	//LREP("counter = %d\r\n", counter);
	counter++;

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

	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		if(pApp->sCfg.sTag[i].input_type == TIT_MB) {
			pApp->sMB.Node[i].enable = pApp->sCfg.sTag[i].enable;
			pApp->sMB.Node[i].address = pApp->sCfg.sTag[i].input_id;
			pApp->sMB.Node[i].data_type = pApp->sCfg.sTag[i].data_type;
			pApp->sMB.Node[i].reg_address = pApp->sCfg.sTag[i].slave_reg_addr;
			pApp->sMB.Node[i].data_format = pApp->sCfg.sTag[i].data_format;
		}
	}
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
	uint8_t data[30];
	uint16_t rlen;
	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		if(pApp->sMB.Node[i].enable &&
				pApp->sCfg.sTag[i].input_type == TIT_MB) {
			retVal = MBMaster_Read(&pApp->sModbus,
					pApp->sMB.Node[i].address,
					pApp->sMB.Node[i].data_type,
					pApp->sMB.Node[i].reg_address,
					1, data, &rlen);
			//LREP("read addr: %d data %d\r\n",pApp->sMB.Node[i].address,
			//		pApp->sMB.Node[i].reg_address);

			if(retVal != MB_SUCCESS) {
				pApp->sMB.Node[i].status = TAG_STT_MB_FAILED;
				LREP("MBT ");
			} else {
				pApp->sMB.Node[i].status = TAG_STT_OK;
				switch(pApp->sMB.Node[i].data_format) {
				case Integer_8bits: {
					uint8_t readValue;
					MBMaster_Parse((const uint8_t*)data, Integer_8bits, &readValue);
					pApp->sMB.Node[i].value = (float)readValue;
				} break;
				case Integer_16bits: {
					uint16_t readValue;
					MBMaster_Parse((const uint8_t*)data, Integer_16bits, &readValue);
					//LREP("uint16_t value: %x\r\n", readValue);
					pApp->sMB.Node[i].value = (float)readValue;
				} break;
				case Integer_32bits: {
					uint32_t readValue;
					MBMaster_Parse((const uint8_t*)data, Integer_32bits, &readValue);
					pApp->sMB.Node[i].value = (float)readValue;
				} break;
				case Float_32bits: {
					float readValue;
					MBMaster_Parse((const uint8_t*)data, Float_32bits, &readValue);
					pApp->sMB.Node[i].value = (float)readValue;
				} break;
				default:
					break;
				}
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
		pApp->sCfg.sDO[i].port = DigitalOutputPin[i].pinName;
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
	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		pApp->sAI.Node[i].enable = true;
	}
	return Analog_Init(&pApp->sAnalogReader,
						BOARD_ANALOG_UART_INSTANCE,
						BOARD_ANALOG_UART_BAUD, 0, 0);
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
		uint8_t status = 0;
		// clear status first
		pApp->sTagValue.Node[i].meas_stt[0] = 0;
		// get main value
		if(pApp->sCfg.sTag[i].input_type == TIT_AI) {
			pApp->sTagValue.Node[i].scratch_value =
				App_GetAIValueByIndex(&pApp->sAI, pApp->sCfg.sTag[i].id);
			status = pApp->sAI.Node[i].status;

		} else {
			pApp->sTagValue.Node[i].scratch_value =
				App_GetMBValueByIndex(&pApp->sMB, pApp->sCfg.sTag[i].id);
			status = pApp->sMB.Node[i].status;
		}

		/* value = scratch * (raw_max - raw_min) / (scr_max - scr_min) */
		if((pApp->sCfg.sTag[i].scratch_max - pApp->sCfg.sTag[i].scratch_min) != 0) {
			pApp->sTagValue.Node[i].raw_value =
				(pApp->sTagValue.Node[i].scratch_value -  pApp->sCfg.sTag[i].scratch_min ) *
				(pApp->sCfg.sTag[i].raw_max - pApp->sCfg.sTag[i].raw_min) /
				(pApp->sCfg.sTag[i].scratch_max - pApp->sCfg.sTag[i].scratch_min) +
				pApp->sCfg.sTag[i].raw_min;
		} else {
			ASSERT(FALSE);
		}

		/* value = a + b * value */
		pApp->sTagValue.Node[i].raw_value = 	pApp->sCfg.sTag[i].coef_a +
				pApp->sTagValue.Node[i].raw_value * pApp->sCfg.sTag[i].coef_b;

		if(status == TAG_STT_OK) {
			// get status
			// if has least error pin or calib pin, set status is normal
			if(pApp->sCfg.sTag[i].has_calib || pApp->sCfg.sTag[i].has_error) {
				pApp->sTagValue.Node[i].meas_stt[0] = '0';
				pApp->sTagValue.Node[i].meas_stt[1] = '0';
				pApp->sTagValue.Node[i].meas_stt[2] = 0;
			}

			// check calib signal on digital input
			if(pApp->sCfg.sTag[i].has_calib) {
				bool  calib =
				App_GetDILevelByIndex(&pApp->sDI, pApp->sCfg.sTag[i].pin_calib);
				if(calib) {
					pApp->sTagValue.Node[i].meas_stt[0] = '0';
					pApp->sTagValue.Node[i].meas_stt[1] = '1';
					pApp->sTagValue.Node[i].meas_stt[2] = 0;
					// force zero when error condition
					pApp->sTagValue.Node[i].raw_value = 0;
				}
			}

			// check error signal in digital input
			if(pApp->sCfg.sTag[i].has_error) {
				bool error =
				App_GetDILevelByIndex(&pApp->sDI, pApp->sCfg.sTag[i].pin_error);
				if(error) {
					pApp->sTagValue.Node[i].meas_stt[0] = '0';
					pApp->sTagValue.Node[i].meas_stt[1] = '2';
					pApp->sTagValue.Node[i].meas_stt[2] = 0;
					pApp->sTagValue.Node[i].raw_value = 0;
				}
			}

			// if value is out of range, set error
			if(pApp->sTagValue.Node[i].scratch_value > pApp->sCfg.sTag[i].scratch_max ||
					pApp->sTagValue.Node[i].scratch_value < pApp->sCfg.sTag[i].scratch_min) {
				pApp->sTagValue.Node[i].meas_stt[0] = '0';
				pApp->sTagValue.Node[i].meas_stt[1] = '2';
				pApp->sTagValue.Node[i].meas_stt[2] = 0;
				pApp->sTagValue.Node[i].raw_value = 0;
			}
		} else {
			pApp->sTagValue.Node[i].meas_stt[0] = '0';
			pApp->sTagValue.Node[i].meas_stt[1] = '2';
			pApp->sTagValue.Node[i].meas_stt[2] = 0;
			pApp->sTagValue.Node[i].raw_value = 0;
			//LREP("not read type %d index %d\r\n", pApp->sCfg.sTag[i].input_type, status);
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
			if((20.9 - pApp->sTagValue.Node[pApp->sCfg.sTag[i].input_o2].raw_value) != 0 &&
					(20.9 - pApp->sCfg.sTag[i].o2_comp) != 0) {
					comp_o2 = (20.9 - pApp->sCfg.sTag[i].o2_comp) /
							(20.9 - pApp->sTagValue.Node[pApp->sCfg.sTag[i].input_o2].raw_value);
			} else {
				ASSERT(FALSE);
			}

			break;
		case CT_TEMP_PRESS:
			/*
			 * Ft = (273 + T(m))/(273+ T(std))
	         * Fp = P(std)/P(m)
			 *
			 * */
			if((273 + pApp->sTagValue.Node[pApp->sCfg.sTag[i].input_temp].raw_value) != 0 &&
					(273 + pApp->sCfg.sTag[i].temp_comp) != 0) {
				comp_temp = (273 + pApp->sTagValue.Node[pApp->sCfg.sTag[i].input_temp].raw_value) /
						(273 + pApp->sCfg.sTag[i].temp_comp);
			} else {
				ASSERT(FALSE);
			}

			if(pApp->sTagValue.Node[pApp->sCfg.sTag[i].input_press].raw_value != 0 &&
					pApp->sTagValue.Node[pApp->sCfg.sTag[i].input_press].raw_value != 0) {
				comp_press = pApp->sCfg.sTag[i].press_comp /
					pApp->sTagValue.Node[pApp->sCfg.sTag[i].input_press].raw_value;
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

		pApp->sTagValue.Node[i].std_value = pApp->sTagValue.Node[i].raw_value *
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
	//LREP("get out idx = %d = %.2f\r\n", index, pHandle->Node[index].value);
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
double App_GetMBValueByIndex(SModbusValue *pHandle, uint16_t index) {
	//LREP("index %d value: %.2f\r\n", index, pHandle->Node[index].value);
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
bool App_GetDILevelByIndex(SDigitalInputLog *pHandle, uint16_t index) {
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
inline int App_SendUI(SApp *pApp, uint8_t subctrl, uint8_t *data, uint8_t len, bool ack) {
	bool ret = false;
	uint8_t *sdata = OSA_FixedMemMalloc(len + 2);
	if(sdata != NULL) {
		sdata[0] = subctrl;
		sdata[1] = len;
		if(len > 0 && data != NULL)
			memcpy(&sdata[2], data, len);

		ret = TransUI_Send(&pApp->sTransUi, len + 2, sdata, ack ? 0xA0 : 0x20);
		OSA_FixedMemFree(sdata);
	}
	return ret;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline int	App_SendPC(SApp *pApp, uint8_t subctrl, uint8_t *data, uint8_t len, bool ack) {
	bool ret = false;
	uint8_t *sdata = OSA_FixedMemMalloc(len + 2);
	if(sdata != NULL) {
		sdata[0] = subctrl;
		sdata[1] = len;
		if(len > 0 && data != NULL)
			memcpy(&sdata[2], data, len);

		ret = TransPC_Send(&pApp->sTransPc, len + 2, sdata, ack ? 0xA0 : 0x20);
		OSA_FixedMemFree(sdata);
	}
	return ret;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline int	App_SendPCNetworkClient(uint8_t subctrl, uint8_t *data, uint8_t len) {
	bool ret = false;
	uint8_t *sdata = OSA_FixedMemMalloc(len + 2);
	if(sdata != NULL) {
		sdata[0] = subctrl;
		sdata[1] = len;
		if(len > 0 && data != NULL)
			memcpy(&sdata[2], data, len);

		//TODO send data to tcpclient
//		Net_TCPServerSendDataToAllClient(sdata, len + 2);
		OSA_FixedMemFree(sdata);
	}
	return ret;
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
		if(Str_Cmp((CPU_CHAR*)pApp->sCfg.sDO[i].name, name) == 0) {
			GPIO_DRV_WritePinOutput(pApp->sCfg.sDO[i].port, logic);
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
typedef struct AnalogChannel_ {
	float vref;
	float vsen;
	float diff;
}SAnalogChannel;

void App_AiReadAllPort(SApp *pApp) {

#if 1
	uint8_t data[15];
	SAnalogChannel adcValue;
	uint8_t recvLen;
	uint8_t dataSize = sizeof(SAnalogChannel);
	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		if(pApp->sAI.Node[i].enable)
		{
			Analog_RecvFF_Reset(&pApp->sAnalogReader);
			Analog_SelectChannel(i);
			OSA_SleepMs(1500); // wait enough for slave mcu wakup and send data
			recvLen = Analog_RecvData(&pApp->sAnalogReader, data, 15);
			if(recvLen > 0) {
				uint8_t crc8 = crc_8((const uint8_t*)data, recvLen - 1);
				if(crc8 == data[recvLen-1])
				{
					if(data[0] == 0x55) {
						memcpy(&adcValue, &data[1], dataSize);
						pApp->sCfg.sAiCalib.calib[i].raw = adcValue.diff;
						pApp->sAI.Node[i].value = pApp->sCfg.sAiCalib.calib[i].offset +
										(pApp->sCfg.sAiCalib.calib[i].raw *
										pApp->sCfg.sAiCalib.calib[i].coefficient);

						LREP("vref: %.2f - vsen: %.2f - diff %.2f\r\n",
								adcValue.vref, adcValue.vsen, adcValue.diff);

						pApp->sAI.Node[i].status = TAG_STT_OK;
					} else {
						LREP("not recv header\r\n");
						pApp->sAI.Node[i].status = TAG_STT_AI_FAILED;
					}
				} else {
					LREP("wrong crc recv %x cal %x\r\n",data[recvLen-1], crc8);
				}
			} else {
				LREP("not recv data\r\n");
				pApp->sAI.Node[i].status = TAG_STT_AI_FAILED;
			}
		}
	}
#else

	uint32_t randout;
	float randVal;
	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		 RNGA_DRV_GetRandomData(0, &randout, sizeof(uint32_t));
		 randout = abs(randout);
		 randout = randout % 100000;
		 randVal = randout / 100.0;
		 pApp->sAI.Node[i].value = randVal;
		 //LREP("rand out = %.2f\r\n", pApp->sAI.Node[i].value);
	}
#endif

}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
bool App_CheckNameExisted(SApp *pApp, const char *name) {
	bool retVal = false;
	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		if(pApp->sCfg.sTag[i].report &&
			(Str_Cmp((CPU_CHAR*)pApp->sCfg.sTag[i].name,
					(CPU_CHAR*)name) == 0)) {
			retVal = true;
		}
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
int App_GenerateLogFile(SApp *pApp) {

	int retVal = FR_OK;

	if(App_IsSysStatus(pApp, SYS_ERR_SDCARD_1))
		return -1;

	FIL file;
	/* get file name time*/
	char *filename = (char*)OSA_FixedMemMalloc(64);
	if(filename == NULL) {
		return -1;
	}
	char *time = (char*)OSA_FixedMemMalloc(64);
	if(time == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		return -2;
	}
	char *year = (char*)OSA_FixedMemMalloc(16);
	if(year == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		OSA_FixedMemFree((uint8_t*)time);
		return -3;
	}
	char *mon = (char*)OSA_FixedMemMalloc(32);
	if(mon == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		OSA_FixedMemFree((uint8_t*)time);
		OSA_FixedMemFree((uint8_t*)year);
		return -4;
	}
	char *day = (char*)OSA_FixedMemMalloc(32);
	if(day == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		OSA_FixedMemFree((uint8_t*)time);
		OSA_FixedMemFree((uint8_t*)year);
		OSA_FixedMemFree((uint8_t*)mon);
		return -5;
	}



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
			LREP("mkdir %s successful !\r\n", year);
		} else {
			LREP("mkdir %s err = %d\r\n", year, retVal);
		}
	}

	if(!check_obj_existed(mon)) {
		retVal = f_mkdir(mon);
		if(retVal == FR_OK) {
			LREP("mkdir %s successful !\r\n", mon);
		} else {
			LREP("mkdir %s err = %d\r\n", mon, retVal);
		}
	}

	if(!check_obj_existed(day)) {
		retVal = f_mkdir(day);
		if(retVal == FR_OK) {
			LREP("mkdir %s successful !\r\n", day);
		} else {
			LREP("mkdir %s err = %d\r\n", day, retVal);
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
			if(row != NULL) {
				UINT written;
				uint8_t rowCount = 0;
				//sprintf(row, "%-12s %12s %12s %15s %12s\r\n",
				//"Thong so", "Gia tri", "Don vi", "Thoi gian", "Trang thai");
				//retVal = f_write(&file, row, Str_Len(row), &written);
				//LREP("%s", row);
				for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
					if(pApp->sCfg.sTag[i].report) {
						memset(row, 0, 256);
						sprintf(row, "%-12s %12.2f %12s %15s %12s\r\n",
								pApp->sCfg.sTag[i].name,
								pApp->sTagValue.Node[i].std_value,
								pApp->sCfg.sTag[i].std_unit,
								time,
								pApp->sTagValue.Node[i].meas_stt);

						LREP("%s", row);

						retVal = f_write(&file, row, Str_Len(row), &written);
						if(retVal != FR_OK || written <= 0)
							break;
						rowCount++;
					}
				}

				OSA_FixedMemFree((uint8_t*)row);

				if(retVal == FR_OK && rowCount > 0) {
					retVal = f_close(&file);
					if(retVal == FR_OK){
						// TODO: send file name to Net module
						uint8_t* dirPath = OSA_FixedMemMalloc(128);
						if(dirPath)
						{
							sprintf(dirPath, "/%s", day);
							memset(filename, 0, sizeof(filename));
							sprintf((char*)filename, "%s_%s_%s_%s.txt", pApp->sCfg.sCom.tinh,
									pApp->sCfg.sCom.coso, pApp->sCfg.sCom.tram, time);
//							NET_DEBUG("Send file %s/%s\r\n", dirPath, filename);
#if NETWORK_MODULE_EN > 0
							Net_FTPClientSendFile(dirPath, filename);
#endif
							OSA_FixedMemFree(dirPath);
						}
					}
				}
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
int App_GenerateLogFileByName(SApp *pApp, const char *name) {
	int retVal = FR_OK;

	if(App_IsSysStatus(pApp, SYS_ERR_SDCARD_1))
		return -1;

	if(!App_CheckNameExisted(pApp, name))
		return -2;

	FIL file;
	/* get file name time*/
	char *filename = (char*)OSA_FixedMemMalloc(64);
	if(filename == NULL) {
		return -1;
	}
	char *time = (char*)OSA_FixedMemMalloc(64);
	if(time == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		return -2;
	}
	char *year = (char*)OSA_FixedMemMalloc(16);
	if(year == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		OSA_FixedMemFree((uint8_t*)time);
		return -3;
	}
	char *mon = (char*)OSA_FixedMemMalloc(32);
	if(mon == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		OSA_FixedMemFree((uint8_t*)time);
		OSA_FixedMemFree((uint8_t*)year);
		return -4;
	}
	char *day = (char*)OSA_FixedMemMalloc(32);
	if(day == NULL) {
		OSA_FixedMemFree((uint8_t*)filename);
		OSA_FixedMemFree((uint8_t*)time);
		OSA_FixedMemFree((uint8_t*)year);
		OSA_FixedMemFree((uint8_t*)mon);
		return -4;
	}

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
			if(row != NULL) {
				UINT written;
				uint8_t rowCount = 0;
				for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
					if(pApp->sCfg.sTag[i].report &&
							(Str_Cmp((CPU_CHAR*)pApp->sCfg.sTag[i].name, name) == 0)) {
						memset(row, 0, 256);
						sprintf(row, "%-12s %12.2f %12s %15s %12s\r\n",
								pApp->sCfg.sTag[i].name,
								pApp->sTagValue.Node[i].std_value,
								pApp->sCfg.sTag[i].std_unit,
								time,
								pApp->sTagValue.Node[i].meas_stt);

						LREP("%s", row);

						retVal = f_write(&file, row, Str_Len(row), &written);
						if(retVal != FR_OK || written <= 0)
							break;
						rowCount++;
					}
				}
				OSA_FixedMemFree((uint8_t*)row);

				if(retVal == FR_OK && rowCount > 0) {
					retVal = f_close(&file);
					if(retVal == FR_OK){
						// TODO: send file name to Net module
						uint8_t* dirPath = OSA_FixedMemMalloc(128);
						if(dirPath)
						{
							sprintf(dirPath, "/%s", day);
							memset(filename, 0, sizeof(filename));
							sprintf((char*)filename, "%s_%s_%s_%s.txt", pApp->sCfg.sCom.tinh,
									pApp->sCfg.sCom.coso, pApp->sCfg.sCom.tram, time);
							NET_DEBUG("Send file %s/%s\r\n", dirPath, filename);
#if NETWORK_MODULE_EN > 0
							Net_FTPClientSendFile(dirPath, filename);
#endif
							OSA_FixedMemFree(dirPath);
						}
					}
				}
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
	OS_ERR err;
	App_SetCtrlCode(pAppObj, CTRL_INIT_SDCARD_1);
    OSSemPost(&pAppObj->hSem, OS_OPT_POST_1, &err);
}

/*****************************************************************************/

//#define FTP_SERVER_IP 			"27.118.20.209"
//#define FTP_SERVER_PORT 		21
//#define FTP_USER_NAME 			"ftpuser1"
//#define FTP_PASSWORD 			"zxcvbnm@12"
//
////#define TCP_SERVER_IP 		"10.2.82.61"
////#define TCP_SERVER_IP 		"192.168.1.116"
#define TCP_SERVER_PORT 		12345
//#define TCP_CLIENT_SERVER_IP 	"192.168.0.101"
//#define TCP_CLIENT_SERVER_PORT 	12345
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
//int	App_InitNetworkModule(SApp *pApp) {
//
//	Net_ModuleInitHw();
//
//	Net_RegisterConnEvent(Clb_NetStatus);
//	Net_RegisterTcpClientDataEvent(NetData_Received, Clb_NetTcpClientReceivedData);
//	Net_RegisterTcpClientDataEvent(NetData_SendDone, Clb_NetTcpClientSentData);
//	Net_RegisterTcpClientDataEvent(NetData_Error, Clb_NetTcpClientError);
//
//	/**
//	 * Setup a TCP Server & Set example echo callback function
//	 */
//	NetStatus status =	Net_TCPServerStart(TCP_SERVER_PORT);
//	if (status == NET_ERR_NONE)
//	{
//		Net_RegisterTcpServerDataEvent(NetData_Received, 	Clb_NetTcpServerReceivedData);
//		Net_RegisterTcpServerDataEvent(NetData_SendDone, 	Clb_NetTcpServerSendDone);
//		Net_RegisterTcpServerDataEvent(NetData_Error, 		Clb_NetTcpServerError);
//	}
//
//	/**
//	 * Start TCP Client
//	 */
//	LREP("TCP client config: %x:%d\r\n", pApp->sCfg.sCom.server_ctrl_ip,
//								pApp->sCfg.sCom.server_ctrl_port);
//	status = Net_TCPClientStart(pApp->sCfg.sCom.server_ctrl_ip,
//								pApp->sCfg.sCom.server_ctrl_port);
//
//	/**
//	 * Start FTP client
//	 */
//
//
//
//	LREP("FTP client config: %x %d %s %s\r\n", pApp->sCfg.sCom.server_ftp_ip1.addr,
//						pApp->sCfg.sCom.server_ftp_port1,
//						(const char*)pApp->sCfg.sCom.ftp_usrname1,
//						(const char*)pApp->sCfg.sCom.ftp_passwd1);
//
//	Net_FTPClientStart( pApp->sCfg.sCom.server_ftp_ip1,
//						pApp->sCfg.sCom.server_ftp_port1,
//						(const char*)pApp->sCfg.sCom.ftp_usrname1,
//						(const char*)pApp->sCfg.sCom.ftp_passwd1);
//
//	return 0;
//
//
//}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
//void Clb_NetStatus(Network_ConnEvent event, Network_Interface interface) {
//	NET_DEBUG_WARNING("Event client %d, interface = %d\r\n",
//			event, interface);
//}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
//void Clb_NetTcpClientReceivedData(const char* data, int length) {
//	NET_DEBUG_WARNING("Event client received length = %d\r\n", length);
//	Net_TCPClientSendData((uint8_t*)data, length);
//}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_NetTcpClientSentData(const char* data, int length) {
	NET_DEBUG_WARNING("Event client send done\r\n");
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_NetTcpClientError(const char* data, int length) {
	NET_DEBUG_WARNING("Event client error\r\n");
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void Clb_NetTcpServerReceivedData(const char* data, int length) {

	NET_DEBUG_WARNING("Event server received\r\n");
	App_NetRecvHandle((uint8_t*)data);
}



/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void Clb_NetTcpServerSendDone(const char* data, int length) {

	NET_DEBUG_WARNING("Event server send done\r\n");
	if(App_IsCtrlCodePending(pAppObj, CTRL_SEND_HEADER)) {
		if(pAppObj->pcCounter < SYSTEM_NUM_TAG) {
			uint8_t *mem = OSA_FixedMemMalloc(sizeof(STagHeader) + 1);
			if(mem != NULL) {
				STagHeader *hdr = (STagHeader*)((uint8_t*)mem + 1);
				mem[0] = (uint8_t)pAppObj->pcCounter;
				hdr->id = pAppObj->sCfg.sTag[pAppObj->pcCounter].id;
				hdr->enable = pAppObj->sCfg.sTag[pAppObj->pcCounter].enable;
				hdr->min = pAppObj->sCfg.sTag[pAppObj->pcCounter].raw_min;
				hdr->max = pAppObj->sCfg.sTag[pAppObj->pcCounter].raw_max;
				hdr->alarm_value = pAppObj->sCfg.sTag[pAppObj->pcCounter].alarm_value;
				hdr->alarm_enable = pAppObj->sCfg.sTag[pAppObj->pcCounter].alarm_enable;
				Str_Copy_N((CPU_CHAR*)hdr->name,
						(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->pcCounter].name,
						sizeof(hdr->name));
				Str_Copy_N((CPU_CHAR*)hdr->raw_unit,
						(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->pcCounter].raw_unit,
						sizeof(hdr->raw_unit));
				Str_Copy_N((CPU_CHAR*)hdr->std_unit,
						(CPU_CHAR*)pAppObj->sCfg.sTag[pAppObj->pcCounter].std_unit,
						sizeof(hdr->std_unit));
				App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_HEADER,
						(uint8_t*)mem, sizeof(STagHeader) + 1);

				LREP("send header %d\r\n", pAppObj->pcCounter);
				App_SetCtrlCode(pAppObj, CTRL_SEND_HEADER);
				pAppObj->pcCounter++;
				OSA_FixedMemFree(mem);
			}
		} else {
			App_ClearCtrlCode(pAppObj, CTRL_SEND_HEADER);
			pAppObj->pcCounter = 0;
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

void Clb_NetTcpServerError(const char* data, int length) {

	NET_DEBUG_WARNING("Event server error\r\n");
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



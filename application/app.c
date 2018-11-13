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
/************************** Constant Definitions *****************************/
#define CONFIG_FILE_PATH		"/config.dat"
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
static void Clb_TransPC_RecvEvent(void *pData, uint8_t u8Type);
static void Clb_TransPC_SentEvent(void *pDatam, uint8_t u8Type);
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

	if(App_InitFS(pApp) == FR_OK) {
		LREP("App init FS successfully \r\n");
	}


	//App_LoadConfig(pApp, CONFIG_FILE_PATH);

	pApp->eStatus = SYS_ERR_NONE;
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
	APP_TASK_INIT_HANDLER(pApp, task_filesystem);
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

	if(check_file_existed(cfg_path)) {
		/* Register work area to the default drive */
		fr = f_open(&fil, cfg_path, FA_READ);
		if (fr) {
			LREP("open file error: %d\r\n", fr);
			return retVal;
		}

		fr = f_read(&fil, (void*)&pApp->sCfg, 4096, &read);

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
	FRESULT fr;     /* FatFs return code */
	uint32_t written;
	int retVal = -1;

	fr = f_open(&fil, cfg_path, FA_CREATE_ALWAYS);
	if (fr) {
		LREP("create file error: %d\r\n", fr);
		return retVal;
	}

	fr = f_write(&fil, (void*)&pApp->sCfg, sizeof(SSysCfg), &written);

	if(!fr) {
		if(written > 0) {
			LREP("save config content ok\r\n");
			retVal = 0;
		} else {
			retVal = -2;
		}
	} else {
		retVal = -3;
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
	pHandle->sCom.log_dur 	= 5;
	pHandle->sCom.modbus_brate = 9600;

	IP4_ADDR(&pHandle->sCom.dev_ip, 192,168,1,2);
	IP4_ADDR(&pHandle->sCom.server_ftp_ip, 192,168,1,12);
	pHandle->sCom.server_ftp_port = 21;
	IP4_ADDR(&pHandle->sCom.server_ctrl_ip, 192,168,1,22);
	pHandle->sCom.server_ctrl_port = 1186;

	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		App_VerifyTagConfig(&pHandle->sTag[i], i);
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
	FIL writer;
	int retVal;

	uint32_t length;

	memset(&pApp->sFS0, 0, sizeof(FATFS));

	retVal = f_mount(1, &pApp->sFS0);

	if(retVal != FR_OK) {
		return retVal;
	}

	retVal = f_open(&writer, "1:InitFS.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

	if(retVal != FR_OK) {
		LREP("fat fs init error: %d\r\n", retVal);
	} else {
		f_write(&writer, "test msg for init FS\r\n", 20, &length);

	}

	f_close(&writer);

	if(f_chdir("1:this_dir") != FR_OK) {
		LREP("change dir failed\r\n");
	} else {
		LREP("change dir ok\r\n");
	}

	uint8_t * path = OSA_FixedMemMalloc(100);
	memset(path, 0, 100);
	int err = f_getcwd((char*)path, 100);
	if(err == FR_OK) {
		LREP("current path = %s\r\n", path);
	} else {
		LREP("getcwd failed err = %d\r\n", err);
	}

	OSA_FixedMemFree(path);


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

//	SApp *pApp = (SApp *)parg;

//	ASSERT(RTC_InitDateTime(&pApp->sDateTime) == 0);
//	OSA_SleepMs(100);
//
//	if(RTC_GetTimeDate(&pApp->sDateTime) == 0) {
//		if(pApp->sDateTime.tm_year == 1990) {
//			RTC_SetDateTime(0, 0, 1, 1, 2018);
//		}
//
//	} else {
//		ASSERT(FALSE);
//	}

	//FM_Init(0);

	while(1) {
		OSA_SleepMs(1000);
//		LREP("APPDATA\r\n");
//		if(RTC_GetTimeDate(&pApp->sDateTime) == 0) {
//
//			LREP("Current Time: %02d/%02d/%d %02d:%02d:%02d\r\n",
//					pApp->sDateTime.tm_sec, pApp->sDateTime.tm_min,
//					pApp->sDateTime.tm_hour, pApp->sDateTime.tm_wday,
//					pApp->sDateTime.tm_mday, pApp->sDateTime.tm_mon,
//					pApp->sDateTime.tm_year);
//
//			/* LREP("[Max %.3f - Min %.3f]\r\n", max_latch, min_latch); */
//
//		} else {
//
//		}
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

			retVal = Modbus_SendAndRecv(&pApp->sModbus, (uint8_t*)p_msg, 264, rx_buf, &rx_length, 100);

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

	Trans_RegisterClbEvent((STrans*)&pApp->sTransPc, TRANS_EVT_RECV_DATA, Clb_TransPC_RecvEvent);
	Trans_RegisterClbEvent((STrans*)&pApp->sTransPc, TRANS_EVT_SENT_DATA, Clb_TransPC_SentEvent);

	Trans_Init((STrans*)&pApp->sTransPc, BOARD_TRANSPC_UART_INSTANCE, BOARD_TRANSPC_UART_BAUD, &pApp->TCB_task_serialcomm);

	LREP("task serial comm init done !\r\n");
	LREP("SFrameInfo Size: %d\r\n", sizeof(SFrameInfo));
	while(1) {
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			Trans_Task((STrans *)&pApp->sTransPc);
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

void App_TaskFilesystem(task_param_t param)
{
	bool run = false;

	while(1) {
		OSA_TimeDelay(1000);
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
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TimerControl(void *p_tmr, void *p_arg) {
	GPIO_DRV_TogglePinOutput(kGpioLEDGREEN);
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

	Modbus_Init(&pApp->sModbus, BOARD_MODBUS_UART_INSTANCE, BOARD_MODBUS_UART_BAUD, 0, 0);
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
			retVal = MBMaster_Read(pApp->sCfg.sTag[i].input_id, pApp->sCfg.sTag[i].slave_reg_addr, 0x03, 1, data, &rlen);
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
			pApp->sCfg.sTag[i].scratch_value = App_GetAIValueByIndex(&pApp->sAI, pApp->sCfg.sTag[i].input_id);
		} else {
			pApp->sCfg.sTag[i].scratch_value = App_GetMBValueByAddress(&pApp->sMB, pApp->sCfg.sTag[i].input_id);
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
				pApp->sCfg.sTag[i].stt[0] = '0';
				pApp->sCfg.sTag[i].stt[1] = '2';
				pApp->sCfg.sTag[i].stt[2] = 0;
			} else {
				if(calib) {
					pApp->sCfg.sTag[i].stt[0] = '0';
					pApp->sCfg.sTag[i].stt[1] = '1';
					pApp->sCfg.sTag[i].stt[2] = 0;
				} else {
					pApp->sCfg.sTag[i].stt[0] = 0;
					pApp->sCfg.sTag[i].stt[1] = 0;
					pApp->sCfg.sTag[i].stt[2] = 0;
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
		default:
			ASSERT(FALSE);
			break;

		}

		pApp->sCfg.sTag[i].std_value = pApp->sCfg.sTag[i].raw_value * comp_o2 * comp_temp * comp_press;
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
int	App_InitDateTime(SApp *pApp) {

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
SDateTime	App_GetDateTime(SApp *pApp) {
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
int App_SetDateTime(SApp *pApp, SDateTime time) {
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
int App_SendUI(SApp *pApp, uint8_t *data, uint8_t len, bool ack) {
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
int	App_SendPC(SApp *pApp, uint8_t *data, uint8_t len, bool ack) {
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
//void FileSystem_Init() {
//	int retVal;
//	memset(&FatFs_SDCARD, 0, sizeof(FATFS));
//
//	retVal = f_mount(1, &FatFs_SDCARD);
//
//	if(retVal != FR_OK) {
//		PRINTF("fat fs init error: %d\r\n", retVal);
//	} else {
//		PRINTF("fat fs init successful !\r\n");
//
//		retVal = f_mkdir("1:this_dir");
//		if(retVal != FR_OK) {
//			PRINTF("mkdir err = %d\r\n", retVal);
//		} else {
//			PRINTF("mkdir successful !\r\n");
//		}
//
//		retVal = f_open(&writer, "1:this_dir/FILE1.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
//
//		if(retVal != FR_OK) {
//			PRINTF("open file error: %d\r\n", retVal);
//		} else {
//			uint32_t byte_written, size;
//			size = f_size(&writer);
//			PRINTF("file size = %d\r\n", size);
//			if(size > 0) {
//				PRINTF("file size = %d bytes\r\n", size);
//			} else {
//
//				retVal = f_write(&writer, (void*)msg, strlen(msg), &byte_written);
//				if(retVal != FR_OK) {
//					PRINTF("write to byte failed err: %d\r\n", retVal);
//				} else {
//					if(byte_written != strlen(msg)) {
//						PRINTF("write to file missing data, writereq = %d - writeact: %d\r\n", strlen(msg), byte_written);
//					} else {
//						PRINTF("write to file totally successful !\r\n");
//					}
//				}
//			}
//		}
//
//		f_close(&writer);
//
//	}
//}
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

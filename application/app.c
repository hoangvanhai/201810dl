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
#include <math.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
void Clb_TransPC_RecvEvent(void *pData, uint8_t u8Type);
void Clb_TransPC_SentEvent(void *pDatam, uint8_t u8Type);
void Clb_TransUI_RecvEvent(void *pData, uint8_t u8Type);
void Clb_TransUI_SentEvent(void *pDatam, uint8_t u8Type);
void Clb_NetTcpClientConnEvent(Network_Status event,
		Network_Interface interface);
void Clb_NetTcpClientRecvData(const uint8_t* data, int length);
void Clb_NetTcpClientSentData(const uint8_t* data, int length);
void Clb_NetTcpClientError(const uint8_t* data, int length);
void Clb_NetTcpServerConnEvent(Network_Status event,
		Network_Interface interface);
void Clb_NetTcpServerRecvData(const uint8_t* data, int length);
void Clb_NetTcpServerSentData(const uint8_t* data, int length);
void Clb_NetTcpServerError(const uint8_t* data, int length);
void Clb_NetFtpClientEvent(FtpStatus status, Network_Interface interface, uint8_t server);

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
	pApp->sStatus.hwStat.all = 0;


	err = BOARD_InitI2CModule();
	ASSERT(err == kStatus_I2C_Success);

	err = App_InitIntFS(pApp);

	if(err == FR_OK) {
		pApp->sStatus.hwStat.Bits.bIntSdcardWorking = true;
		LREP("app init FS successfully \r\n");
	} else {
		LREP("app init FS failed err = %d \r\n", err);
		pApp->sStatus.hwStat.Bits.bIntSdcardWorking = false;
	}

#if CONFIG_STORE_IN == CONFIG_IN_FILE_SYSTEM
	if(pApp->sStatus.hwStat.Bits.bIntSdcardWorking) {
		err = App_LoadConfig(pApp, CONFIG_FILE_PATH);
		if(err == FR_OK) {
			LREP("app load config successfully \r\n");
		} else {
			LREP("app load config failed err = %d\r\n", err);
		}
	} else {
		ASSERT(false);
		LREP("internal sdcard not found, init default config for dry running\r\n");
		App_GenDefaultConfig(&pApp->sCfg);
	}
#elif CONFIG_STORE_IN == CONFIG_IN_EEPROM
	if(CONF_CheckWrittenApp()) {
		int rlen = 0;
		if(CONF_ReadData(EEPROM_PAGE_SIZE,
				(uint8_t*)&pApp->sCfg, sizeof(SSysCfg), &rlen) ==
				kStatus_I2C_Success) {
			if(rlen != sizeof(SSysCfg)) {
				ASSERT(false);
				App_GenDefaultConfig(&pApp->sCfg);
			}
		} else {
			App_GenDefaultConfig(&pApp->sCfg);
		}
	} else {
		LREP("key not found, write key to eeprom\r\n");
		CONF_WriteKeyApp();
		App_GenDefaultConfig(&pApp->sCfg);
		LREP("write default config to eeprom\r\n");
		ASSERT(CONF_WriteData(EEPROM_PAGE_SIZE,
				(uint8_t*)&pApp->sCfg, sizeof(SSysCfg)) ==
				kStatus_I2C_Success);
	}

	if(App_VerifyCommConfig(&pApp->sCfg) != 0) {
		CONF_WriteKeyApp();
		App_GenDefaultConfig(&pApp->sCfg);
		LREP("invalid config generate default \r\n");
		ASSERT(CONF_WriteData(EEPROM_PAGE_SIZE,
				(uint8_t*)&pApp->sCfg, sizeof(SSysCfg)) ==
				kStatus_I2C_Success);
	}
#else
#error must specify config store location
#endif

	if(App_InitExtFs(pApp) == FR_OK) {
		LREP("app mount ext sdcard successfully\r\n");
		pApp->sStatus.hwStat.Bits.bExtSdcardWorking = true;
	} else {
		pApp->sStatus.hwStat.Bits.bExtSdcardWorking = false;
	}

	App_InitDI(pApp);
	App_InitDO(pApp);
	DAC_InterfaceInit();

    LREP("sizeof(SCommon) 	%d\r\n", 		sizeof(SCommon));
    LREP("sizeof(STag) 		%d\r\n", 		sizeof(STag));
    LREP("sizeof(SInputPort) 	%d\r\n", 	sizeof(SInputPort));
    LREP("sizeof(SCtrlPort) 	%d\r\n",	sizeof(SCtrlPort));
    LREP("sizeof(STagValue) 	%d\r\n", 	sizeof(STagNode));
    LREP("sizeof(SSysCfg) 	%d\r\n", 		sizeof(SSysCfg));
    LREP("sizeof(SComStatus) 	%d\r\n", 	sizeof(SComStatus));
    LREP("sizeof(STagVArray) 	%d\r\n", 	sizeof(STagVArray));
    LREP("sizeof(SAnalogInput) 	%d\r\n", 	sizeof(SAnalogInput));
    LREP("sizeof(SDigitalOutputLog) 	%d\r\n", 	sizeof(SDigitalOutputLog));
    LREP("sizeof(SDigitalInputLog) 	%d\r\n", 	sizeof(SDigitalInputLog));
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
	//memset(&pApp, 0, sizeof(SApp));
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
#if 1
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_usrname1, "ftpuser1");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_passwd1, "zxcvbnm@12");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_prefix1, "/home/ftpuser1/");
	IP4_ADDR(&pHandle->sCom.server_ftp_ip1, 27,118,20,209);
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_usrname2, "ftpuser1");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_passwd2, "zxcvbnm@12");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_prefix2, "/home/ftpuser1/");
	IP4_ADDR(&pHandle->sCom.server_ftp_ip2, 27,118,20,209);
#else
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_usrname1, "win7");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_passwd1, "123456a@");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_prefix1, "/home/");
	IP4_ADDR(&pHandle->sCom.server_ftp_ip1, 192,168,0,100);
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_usrname2, "win7");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_passwd2, "123456a@");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ftp_prefix2, "/home/");

	IP4_ADDR(&pHandle->sCom.server_ftp_ip2, 192,168,0,100);
#endif
	pHandle->sCom.server_ftp_port1 = 21;
	pHandle->sCom.server_ftp_port2 = 21;

	Str_Copy((CPU_CHAR*)pHandle->sCom.ctrl_usrname, "ctrluser");
	Str_Copy((CPU_CHAR*)pHandle->sCom.ctrl_passwd, "ctrlpasswd");

	pHandle->sCom.scan_dur 	= 1;
	pHandle->sCom.log_dur 	= 1;	//5;
	pHandle->sCom.modbus_brate = 9600;

	pHandle->sCom.ftp_enable1 = TRUE;
	pHandle->sCom.ftp_enable2 = FALSE;

	pHandle->sCom.dev_hwaddr[0] = defaultMAC_ADDR0;
	pHandle->sCom.dev_hwaddr[1] = defaultMAC_ADDR1;
	pHandle->sCom.dev_hwaddr[2] = defaultMAC_ADDR2;
	pHandle->sCom.dev_hwaddr[3] = defaultMAC_ADDR3;
	pHandle->sCom.dev_hwaddr[4] = defaultMAC_ADDR4;
	pHandle->sCom.dev_hwaddr[5] = defaultMAC_ADDR5;

	IP4_ADDR(&pHandle->sCom.dev_ip, 192,168,0,105);
	IP4_ADDR(&pHandle->sCom.dev_netmask, 255,255,255,0);
	IP4_ADDR(&pHandle->sCom.dev_gw, 192,168,0,1);

	IP4_ADDR(&pHandle->sCom.server_ctrl_ip, 192,168,0,100);
	pHandle->sCom.server_ctrl_port = 1186;

	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		App_DefaultTag(&pHandle->sTag[i], i);
	}

	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		pHandle->sAiCalib.calib[i].offset = 0;
		pHandle->sAiCalib.calib[i].coefficient = 1;
	}

	for(int i = 0; i < 5; i++) {
		pHandle->sCurrOutCoeff[i] = (i + 1) * 400;
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
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int App_VerifyCommConfig(SSysCfg *pHandle) {
	if(Str_Len_N((CPU_CHAR*)pHandle->sCom.tinh, SYS_TINH_LENGTH + 3) >=
			(SYS_TINH_LENGTH + 2))
		return -1;
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

	pApp->sStatus.hwStat.Bits.bCritical = true;
	uint8_t retbyte = LOGGER_WRITE_SUCCESS;
	switch(pData[0]) {
	case LOGGER_SET | LOGGER_COMMON:
		if(pData[1] == sizeof(SCommon)) {
			uint8_t mac_addr[6];
			memcpy(mac_addr, pApp->sCfg.sCom.dev_hwaddr, 6);
			memcpy(&pApp->sCfg.sCom, &pData[2], sizeof(SCommon));
			memcpy(pApp->sCfg.sCom.dev_hwaddr, mac_addr, 6);
			LREP("recv common data \r\n");
			print_comm(&pApp->sCfg.sCom);
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

	case LOGGER_SET | LOGGER_MAC_ADDR :
		if(pData[1] == 6) {
			LREP("set mac address \r\n");
			for(int i = 0; i < 6; i++)
				LREP("%x ", pData[i + 2]);

			memcpy(pApp->sCfg.sCom.dev_hwaddr, &pData[2], 6);
		} else {
			break;
		}

	case LOGGER_SET | LOGGER_WRITE_DONE:

		Network_Ftpclient_ClearWaitQueue(pApp->sCfg.sCom.ftp_enable1,
				pApp->sCfg.sCom.ftp_enable2);

		LREP("config is writing ... \r\n");
#if CONFIG_STORE_IN == CONFIG_IN_FILE_SYSTEM
		App_SaveConfig(pApp, CONFIG_FILE_PATH);
#elif CONFIG_STORE_IN == CONFIG_IN_EEPROM

		while(pApp->sStatus.hwStat.Bits.bI2CBusy);
		pApp->sStatus.hwStat.Bits.bI2CBusy = true;
		if(CONF_WriteData(EEPROM_PAGE_SIZE,
						(uint8_t*)&pApp->sCfg, sizeof(SSysCfg)) !=
						kStatus_I2C_Success) {
			ERR("write config error\r\n");
			retbyte = LOGGER_WRITE_SUCCESS;
		}

		pApp->sStatus.hwStat.Bits.bI2CBusy = false;
		LREP("write config done -> reset require !\r\n\r\n");

#else
#error here
#endif
		pApp->sStatus.hwStat.Bits.bReboot = true;
	break;

	default:
		ASSERT_NONVOID(FALSE, -5);
		break;
	}

	int ret;
	if(serial)
		ret = App_SendPC(pApp, LOGGER_SET | retbyte, NULL, 0, false);
	else
		ret = App_SendPCNetworkClient(LOGGER_SET | retbyte, NULL, 0);

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
int	App_InitIntFS(SApp *pApp) {

	int retVal;

	pApp->sStatus.hwStat.Bits.bIntSdcardPlugged = BOARD_IsIntSDCardDetected();

	if(!pApp->sStatus.hwStat.Bits.bIntSdcardPlugged)
		return FR_NOT_READY;

	LREP("start init FS\r\n");

	memset(&pApp->sFS0, 0, sizeof(FATFS));

	retVal = f_mount(&pApp->sFS0, "0:", 0);

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

int App_InitExtFs(SApp *pApp) {

	pApp->sStatus.hwStat.Bits.bExtSdcardPlugged = BOARD_IsExtSDCardDetected();
	if(!pApp->sStatus.hwStat.Bits.bExtSdcardPlugged) {
		return FR_NOT_READY;
	}
	LREP("start init Ext FS \r\n");
	memset(&pApp->sFS1, 0, sizeof(FATFS));

	return f_mount(&pApp->sFS1, "1:", 1);
}


void App_WriteExtFs(SApp *pApp) {
	int retVal;
	UINT	written;
	FIL		fil;

	retVal = f_open(&fil, "1:/test_file.txt", FA_CREATE_ALWAYS | FA_WRITE);
	if (retVal != FR_OK) {
		LREP("create file error: %d\r\n", retVal);
		return;
	}

	const char* msg = "this is test string write to external sdhc card\r\n";

	retVal = f_write(&fil, (void*)msg, strlen(msg), (UINT*)&written);

	if(retVal == FR_OK) {
		LREP("write to ex sd card success\r\n");
	} else {
		LREP("f_write return error: %d\r\n", retVal);
	}

	/* Close the file */
	f_close(&fil);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_DoCopyIntToExt(SApp *pApp) {
	int err;
	// check command in cmd.txt
	if(!check_obj_existed("1:/copy.txt")) {
		LREP("file copy.txt not found \r\n");
		err = -1;
	} else {
		LREP("found copy.txt\r\n");
		show_content_recursive("/");
	}

	return err;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int	App_DeinitExtFs(SApp *pApp){
	return f_unmount("1:");
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

	//int log_min = pApp->sCfg.sCom.log_dur > 0 ? pApp->sCfg.sCom.log_dur : 1;
	int log_min = 20;
	int last_min = 0;
	bool logged = false;

	pApp->sStatus.time.tm_mday = 1;
	pApp->sStatus.time.tm_mon = 1;
	pApp->sStatus.time.tm_year = 2018;

	ASSERT(DAC_InitRefCurr() == kStatus_I2C_Success);

	ASSERT(RTC_InitDateTime(&pApp->sStatus.time) == 0);
	OSA_SleepMs(10);

	if(RTC_GetTimeDate(&pApp->sStatus.time) == 0) {
		if(pApp->sStatus.time.tm_year == 1990) {
			RTC_SetDateTime(0, 0, 1, 1, 2018);
		}
		LREP("Current Time: %04d/%02d/%02d %02d:%02d:%02d\r\n",
				pApp->sStatus.time.tm_year, pApp->sStatus.time.tm_mon,
				pApp->sStatus.time.tm_mday, pApp->sStatus.time.tm_hour,
				pApp->sStatus.time.tm_min, pApp->sStatus.time.tm_sec);
	} else {
		ASSERT(FALSE);
	}

	LREP("log min = %d\r\n", log_min);

	while(1) {
		OSA_SleepMs(1000);

		if(pApp->sStatus.hwStat.Bits.bCritical)
			continue;

		if(!pApp->sStatus.hwStat.Bits.bI2CBusy) {
			pApp->sStatus.hwStat.Bits.bI2CBusy = true;
			ASSERT(RTC_GetTimeDate(&pApp->sStatus.time) == kStatus_I2C_Success);
			pApp->sStatus.hwStat.Bits.bI2CBusy = false;
		}

		App_DiReadAllPort(pApp);
		App_UpdateTagContent(pApp);

        if(pApp->sStatus.time.tm_min != last_min && logged) {
            logged = false;
        }

		if((logged == false) && (pApp->sStatus.time.tm_min % log_min == 0))
        //if((pApp->sStatus.time.tm_sec % 20) == 0)
		{
			// To ensure has valid data before log to file
			if(pApp->aiReadCount > 0 && pApp->mbReadCount > 0)
			{
				if(pApp->sCfg.sCom.ftp_enable1 ||
						pApp->sCfg.sCom.ftp_enable2)
				{
					LREP("generate log file\r\n");
					int retVal = App_GenerateLogFile(pApp, SEND_SERVER_ALL);
					if(retVal != FR_OK) {
						LREP("err = %d\r\n", retVal);
					}

				} else {
					LREP("disabled generate log file\r\n");
				}
				last_min = pApp->sStatus.time.tm_min;
				logged = true;
			}
		}


		WDOG_DRV_Refresh();
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

	GPIO_DRV_SetPinOutput(ModbusPsuEn);

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

	//LREP("UI PORT: %d _______\r\n", BOARD_TRANSUI_UART_INSTANCE);

	OSTmrCreate(&pApp->hCtrlTimer,
				(CPU_CHAR *)"timer",
				(OS_TICK)200,
				(OS_TICK)100,
				(OS_OPT)OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) Clb_TimerControl,
				(void*)NULL,
				(OS_ERR*)&err);

	ASSERT(err == OS_ERR_NONE);

	OSTmrStart(&pApp->hCtrlTimer, &err);
	if (err == OS_ERR_NONE) {
		/* Timer was created but NOT started */
		LREP("timer started ok\r\n");
	} else {
		ASSERT(FALSE);
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
	int err_init;
	CPU_TS	ts;
	SApp *pApp = (SApp*)arg;

	OSA_FixedMemInit();

	App_Init(pApp);

	App_CreateAppEvent(pApp);

	App_CreateAppTask(pApp);

	App_OS_SetAllHooks();

#if OS_CFG_STAT_TASK_EN == DEF_ENABLED
    OSStatTaskCPUUsageInit(&err);
    OSStatReset(&err);
#endif

    BOARD_CheckPeripheralFault();

#if NETWORK_MODULE_EN > 0
    uint32_t timeNow = sys_now();
    Network_InitTcpModule(&pApp->sCfg.sCom, &pApp->sStatus);

	Network_Register_TcpClient_Notify(Clb_NetTcpClientConnEvent);
	Network_Register_TcpClient_DataEvent(Event_DataReceived, Clb_NetTcpClientRecvData);
	Network_Register_TcpClient_DataEvent(Event_DataSendDone, Clb_NetTcpClientSentData);
	Network_Register_TcpClient_DataEvent(Event_DataError, Clb_NetTcpClientError);

	Network_Register_TcpServer_Notify(Clb_NetTcpServerConnEvent);
	Network_Register_TcpServer_DataEvent(Event_DataReceived, Clb_NetTcpServerRecvData);
	Network_Register_TcpServer_DataEvent(Event_DataSendDone, Clb_NetTcpServerSentData);
	Network_Register_TcpServer_DataEvent(Event_DataError, Clb_NetTcpServerError);
	Network_Register_FtpClient_Event(Clb_NetFtpClientEvent);

	Network_InitFtpModule(&pApp->sCfg.sCom);
	WARN("FTP client wakeup spend %d ms\r\n", sys_now() - timeNow);
	WARN("FTP CLIENT INIT DONE\r\n");
#endif

	while(1) {

		OSSemPend(&pApp->hSem, 1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {

			/* Check all pending command */
			if(App_IsCtrlCodePending(pApp, CTRL_INIT_INT_SDCARD)) {

			}


			if(App_IsCtrlCodePending(pApp, CTRL_INIT_EXT_SDCARD)) {
				bool status = BOARD_IsExtSDCardDetected();
				if(pApp->sStatus.hwStat.Bits.bExtSdcardPlugged != status) {
					pAppObj->sStatus.hwStat.Bits.bExtSdcardPlugged = status;
					LREP("Ext card event = %d\r\n", pApp->sStatus.hwStat.Bits.bExtSdcardPlugged);
					if(pAppObj->sStatus.hwStat.Bits.bExtSdcardPlugged) {
						LREP("recv ctrl init ext sdcard\r\n");

						pApp->sStatus.hwStat.Bits.bCritical = true;
						err_init = App_InitExtFs(pApp);
						pApp->sStatus.hwStat.Bits.bCritical = false;

						if(err_init != FR_OK) {
							pApp->sStatus.hwStat.Bits.bExtSdcardWorking = false;
							App_SendUI(pAppObj, LOGGER_GET | LOGGER_SYSTEM_STATUS,
									(uint8_t*)&pAppObj->sStatus, sizeof(SComStatus), false);
						} else {
							pApp->sStatus.hwStat.Bits.bExtSdcardWorking = true;
							App_SendUI(pAppObj, LOGGER_GET | LOGGER_SYSTEM_STATUS,
									(uint8_t*)&pAppObj->sStatus, sizeof(SComStatus), false);
							//OSA_SleepMs(100);
							// TODO: copy data from internal sd card to external

							//App_DoCopyIntToExt(pApp);

						}
					} else {
						LREP("unmount external card\r\n");
						pApp->sStatus.hwStat.Bits.bCritical = true;
						App_DeinitExtFs(pApp);
						pApp->sStatus.hwStat.Bits.bCritical = false;
						pApp->sStatus.hwStat.Bits.bExtSdcardWorking = false;
						App_SendUI(pAppObj, LOGGER_GET | LOGGER_SYSTEM_STATUS,
								(uint8_t*)&pAppObj->sStatus, sizeof(SComStatus), false);
					}
				}

				App_ClearCtrlCode(pApp, CTRL_INIT_EXT_SDCARD);


			}

			if(App_IsCtrlCodePending(pApp, CTRL_INIT_MODBUS)) {
				LREP("recv ctrl init modbus\r\n");

				App_ClearCtrlCode(pApp, CTRL_INIT_MODBUS);
			}

			if(App_IsCtrlCodePending(pApp, CTRL_GET_WL_STT)) {
				//LREP("recv ctrl get wireless status\r\n");

				Network_GetWirelessStatus();
				App_ClearCtrlCode(pApp, CTRL_GET_WL_STT);
			}

			//WDOG_DRV_Refresh();
		} else {
			BOARD_CheckPeripheralFault();
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
void App_SerialComRecvHandle(const uint8_t *data) {
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
	case LOGGER_SET | LOGGER_MAC_ADDR:
		App_SetConfig(pAppObj, data, true);
		break;
	case LOGGER_SET | LOGGER_TIME:

		memcpy((uint8_t*)&pAppObj->sStatus.time, &data[2], sizeof(SDateTime));
		 LREP("Current Time: %04d/%02d/%02d %02d:%02d:%02d\r\n",
				 pAppObj->sStatus.time.tm_year, pAppObj->sStatus.time.tm_mon,
				 pAppObj->sStatus.time.tm_mday, pAppObj->sStatus.time.tm_hour,
				 pAppObj->sStatus.time.tm_min,  pAppObj->sStatus.time.tm_sec);

		 App_SetDateTime(pAppObj, pAppObj->sStatus.time);

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

	case LOGGER_SET | LOGGER_CALIB_CURR:
		App_CommCalibCurrPwr(pAppObj, data);
	break;

	case LOGGER_SET | LOGGER_GENERATE_CURR:
		App_CommTurnOnOffCurr(pAppObj, data);
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
void App_TcpServerRecvHandle(const uint8_t *data) {
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
	case LOGGER_SET | LOGGER_MAC_ADDR:
		App_SetConfig(pAppObj, data, false);
		break;
	case LOGGER_SET | LOGGER_TIME:

		memcpy((uint8_t*)&pAppObj->sStatus.time, &data[2], sizeof(SDateTime));
		 LREP("Current Time: %04d/%02d/%02d %02d:%02d:%02d\r\n",
				 pAppObj->sStatus.time.tm_year, pAppObj->sStatus.time.tm_mon,
				 pAppObj->sStatus.time.tm_mday, pAppObj->sStatus.time.tm_hour,
				 pAppObj->sStatus.time.tm_min,  pAppObj->sStatus.time.tm_sec);
		 App_SetDateTime(pAppObj, pAppObj->sStatus.time);

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
			int ret = App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_AI,
							(uint8_t*)&pAppObj->sAI, sizeof(SAnalogInput));
			LREP("send stream ai %d \r\n", ret);
		}

		if(data[2] & STREAM_MB) {

			int ret = App_SendPCNetworkClient(LOGGER_GET | LOGGER_STREAM_MB,
							(uint8_t*)&pAppObj->sMB, sizeof(SModbusValue));
			LREP("send stream mb %d\r\n", ret);
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
				LREP("send stream value \r\n");
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

	case LOGGER_SET | LOGGER_CALIB_CURR:
		App_CommCalibCurrPwr(pAppObj, data);
	break;

	case LOGGER_SET | LOGGER_GENERATE_CURR:
		App_CommTurnOnOffCurr(pAppObj, data);
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
void App_TcpClientRecvHandle(const uint8_t *data, int len) {
	LREP("data len: %d\r\n", len);
	if(len <= 0) {
		ERR("len = %d\r\n", len);
		return;
	}

	uint8_t *pdata = (uint8_t*)data;

//	for(int i = 0; i < len; i++) {
//		LREP("%02x ", pdata[i]);
//	}

	switch(GET_MSG_TYPE(pdata)) {
		 case SER_GET_INPUT_ALL: {
			uint8_t data[4];
			LREP("do get input all\r\n");
			if(App_GenerateLogFile(pAppObj, SEND_SERVER_0) == 0) {
				LREP("generate file ok\r\n");
				data[0] = SER_CTRL_SUCCESS;
			} else {
				LREP("generate file failed\r\n");
				data[0] = SER_CTRL_FAILED;
			}
			Network_TcpClient_SendWLength(SER_GET_INPUT_ALL, data, 1);
		 }
		break;
		 case SER_GET_INPUT_GROUP:
			LREP("do get input group\r\n");

		break;
		 case SER_GET_INPUT_CHAN: {
			uint8_t portName[PORT_NAME_LENGTH + 1];
			uint8_t data[4];
			memset(portName, 0, PORT_NAME_LENGTH + 1);
			Str_Copy_N((CPU_CHAR*)portName, (char*)&pdata[3], PORT_NAME_LENGTH);
			if(Str_Len((CPU_CHAR*)portName) > 0) {
				LREP("do get input channel %s\r\n", portName);
				if(App_GenerateLogFileByName(pAppObj, (char*)portName, SEND_SERVER_0) == 0) {
					LREP("generate file ok\r\n");
					data[0] = SER_CTRL_SUCCESS;
				} else {
					LREP("generate file failed\r\n");
					data[0] = SER_CTRL_FAILED;
				}
				Network_TcpClient_SendWLength(SER_GET_INPUT_CHAN, data, 1);
			} else {
				WARN("get invalid name \r\n");
			}
		 }

		break;
		 case SER_SET_SAMPLE_START: {
			uint8_t portName[PORT_NAME_LENGTH + 1];
			uint8_t data[4];
			memset(portName, 0, PORT_NAME_LENGTH + 1);
			Str_Copy_N((CPU_CHAR*)portName, (char*)&pdata[3], PORT_NAME_LENGTH);
			if(Str_Len((CPU_CHAR*)portName) > 0) {
				LREP("do get sample\r\n");
				if(App_SetDoPinByName(pAppObj, (const char*)portName, 1)) {
					LREP("generate file ok\r\n");
					data[0] = SER_CTRL_SUCCESS;
				} else {
					LREP("generate file failed\r\n");
					data[0] = SER_CTRL_FAILED;
				}
				Network_TcpClient_SendWLength(SER_SET_SAMPLE_START, data, 1);
			} else {
				WARN("get invalid name \r\n");
			}
		 }
		break;
		 case LOG_REQ_CALIB_START:
			LREP("do set calib start\r\n");

		break;
		 case SER_SET_CALIB_START:

		break;
		 case LOGGER_LOGGING_IN:

		break;
		case SER_LOGGING_STATUS:
		if(pdata[3] == 0x00) {
			LREP("login = true\r\n");
			pAppObj->sStatus.hwStat.Bits.bLogged = true;
		} else {
			LREP("login = false\r\n");
			pAppObj->sStatus.hwStat.Bits.bLogged = false;
		}
		break;
		 case LOGGER_LOGGING_OUT:
			LREP("loggin out\r\n");
			pAppObj->sStatus.hwStat.Bits.bLogged = false;
		break;
	default:
		WARN("unhandled command {}\n", GET_MSG_TYPE(pdata));
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
		LREP("calib 16mA\r\n");
	}
	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		if(pApp->sAI.Node[i].status == TAG_STT_OK) {
			if(data[2] == 0) {// calib at 4mA
				pApp->sCfg.sAiCalib.calib[i].x1 =
					pApp->sCfg.sAiCalib.calib[i].raw;
			} else {// calib at 16mA
				isSave = true;
				pApp->sCfg.sAiCalib.calib[i].x2 =
					pApp->sCfg.sAiCalib.calib[i].raw;
				if((pApp->sCfg.sAiCalib.calib[i].x2 -
						pApp->sCfg.sAiCalib.calib[i].x1) != 0) {
				pApp->sCfg.sAiCalib.calib[i].coefficient = 12 /
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

	uint8_t point = data[2];

	float current_out;
	float new_coeff, new_value;
	LREP("calib point = %d\r\n", point);

	ASSERT_VOID(point >= 0 && point <5);

	switch(point) {
	case 0: current_out = 4.0; break;
	case 1: current_out = 8.0; break;
	case 2: current_out = 12.0; break;
	case 3: current_out = 16.0; break;
	case 4: current_out = 20.0; break;
	default: return;
	}

	memcpy((uint8_t*)&new_value, &data[3], sizeof(float));
	new_coeff = (current_out / new_value) *
			pApp->sCfg.sCurrOutCoeff[point];

	LREP("new coeff = %f\r\n", new_coeff);
	pApp->sCfg.sCurrOutCoeff[point] = new_coeff;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void App_CommTurnOnOffCurr(SApp *pApp, const uint8_t *data) {

	uint8_t point = data[2];
	if(point > 5 || point < 0) {
		if(point == 0xFF) {
			pApp->sStatus.hwStat.Bits.bCurrOut = false;
			GPIO_DRV_SetPinOutput(RefCurrEn); // turn off
			App_SendUI(pApp, LOGGER_GET | LOGGER_SYSTEM_STATUS,
					(uint8_t*)&pApp->sStatus, sizeof(SComStatus), false);
		}
		return;
	}

	pApp->sStatus.hwStat.Bits.bCurrOut = true;
	GPIO_DRV_ClearPinOutput(RefCurrEn);
	ASSERT_VOID(pApp->sCfg.sCurrOutCoeff[point] < 3000);
	uint16_t lev = (int)(pApp->sCfg.sCurrOutCoeff[point]);
	pApp->sStatus.fwStat.curr_lev = pApp->sCfg.sCurrOutCoeff[point];
	LREP("generate lev = %d\r\n", lev);
	DAC_SetRefLevel(lev, false);

	switch(point) {
		case 0: pApp->sStatus.fwStat.curr_lev = 4.0; break;
		case 1: pApp->sStatus.fwStat.curr_lev = 8.0; break;
		case 2: pApp->sStatus.fwStat.curr_lev = 12.0; break;
		case 3: pApp->sStatus.fwStat.curr_lev = 16.0; break;
		case 4: pApp->sStatus.fwStat.curr_lev = 20.0; break;
	}

	App_SendUI(pApp, LOGGER_GET | LOGGER_SYSTEM_STATUS,
						(uint8_t*)&pApp->sStatus, sizeof(SComStatus), false);
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
	//static uint32_t count = 0;
	SFrameInfo *pFrameInfo = (SFrameInfo *)MEM_BODY((SMem*)pData);
	LREP("Recv frm type %x dlen: %d\r\n",
			pFrameInfo->pu8Data[0], pFrameInfo->u16DLen);

	switch(u8Type & 0x3F) {
	case FRM_DATA:
		App_SerialComRecvHandle(pFrameInfo->pu8Data);
		break;


	default:
		ASSERT(false);
		break;
	}
}

void Clb_TransPC_SentEvent(void *pData, uint8_t u8Type) {

	SFrameInfo *pFrame = (SFrameInfo*)MEM_BODY((SMem*)pData);

	//LREP("Sent done event 0x%x\r\n", u8Type);
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
			if(pAppObj->sStatus.hwStat.Bits.bReboot) {
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

	GPIO_DRV_TogglePinOutput(Led1);

	if(counter % 30 == 0) {
		pAppObj->uiCounter = 0;

		App_SendUI(pAppObj, LOGGER_GET | LOGGER_SYSTEM_STATUS,
						(uint8_t*)&pAppObj->sStatus, sizeof(SComStatus), false);

		App_PrintSystemStatus(pAppObj);

		OS_ERR err;
		App_SetCtrlCode(pAppObj, CTRL_GET_WL_STT);
	    OSSemPost(&pAppObj->hSem, OS_OPT_POST_1, &err);
		LREP("send system status\r\n");
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
	}

	{
		if(counter % 4 == 0) {
			STagVArray *pMem = (STagVArray*)OSA_FixedMemMalloc(sizeof(STagVArray));
			if(pMem != NULL) {
				for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
					pMem->Node[i].raw_value = pAppObj->sTagValue.Node[i].raw_value;
					pMem->Node[i].std_value = pAppObj->sTagValue.Node[i].std_value;
					pMem->Node[i].meas_stt[0] = pAppObj->sTagValue.Node[i].meas_stt[0];
					pMem->Node[i].meas_stt[1] = pAppObj->sTagValue.Node[i].meas_stt[1];
					pMem->Node[i].meas_stt[2] = pAppObj->sTagValue.Node[i].meas_stt[2];
					//LREP("status %d = %s\r\n", i, pMem->Node[i].meas_stt);
				}
				App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_VALUE,
						(uint8_t*)pMem, sizeof(STagVArray), false);

				OSA_FixedMemFree((uint8_t*)pMem);
			}
		} else if(counter % 4 == 1) {

			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_AI,
				  (uint8_t*)&pAppObj->sAI, sizeof(SAnalogInput), false);


		} else if (counter % 4 == 2) {

			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_DO,
				  (uint8_t*)&pAppObj->sDO, sizeof(SDigitalOutputLog), false);

		}

		if(counter % 4 == 3) {

			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_DI,
				  (uint8_t*)&pAppObj->sDI, sizeof(SDigitalInputLog), false);
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

	memset(&pApp->sMB, 0, sizeof(SModbusValue));
	for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
		if(pApp->sCfg.sTag[i].input_type == TIT_MB) {
			pApp->sMB.Node[i].enable = pApp->sCfg.sTag[i].enable;
			pApp->sMB.Node[i].address = pApp->sCfg.sTag[i].input_id;
			pApp->sMB.Node[i].data_type = pApp->sCfg.sTag[i].data_type;
			pApp->sMB.Node[i].reg_address = pApp->sCfg.sTag[i].slave_reg_addr;
			pApp->sMB.Node[i].data_format = pApp->sCfg.sTag[i].data_format;
			pApp->sMB.Node[i].data_order = pApp->sCfg.sTag[i].data_order;
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
				//LREP("MBT ");
				debug_putchar('M');
			} else {

					float readValue;
					if(MBMaster_Parse((const uint8_t*)data,
							pApp->sMB.Node[i].data_format,
							pApp->sMB.Node[i].data_order, &readValue) == 1) {
						pApp->sMB.Node[i].value = readValue;
					pApp->sMB.Node[i].status = TAG_STT_OK;
					} else {
						ASSERT(FALSE);
						pApp->sMB.Node[i].status = TAG_STT_MB_FAILED;
					}

			}
		}
	}
	pApp->mbReadCount++;

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
		pApp->sDO.Node[i].id = DigitalOutputPin[i].pinName;
		// Clear logic level at startup
		pApp->sDO.Node[i].lev = false;
		GPIO_DRV_WritePinOutput(pApp->sDO.Node[i].id,
				(pApp->sCfg.sDO[i].activeType == ACTIVE_HIGH) ? 0 : 1);
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

	retVal = RTC_InitDateTime(&pApp->sStatus.time);
	ASSERT_NONVOID(retVal == 0, retVal);

	OSA_SleepMs(100);

	retVal = RTC_GetTimeDate(&pApp->sStatus.time);

	if(retVal == 0) {
		if(pApp->sStatus.time.tm_year == 1990) {
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
	return pApp->sStatus.time;
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
	pApp->sStatus.time = time;
	return RTC_SetTimeDate(&pApp->sStatus.time);
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
int	App_SendPCNetworkClient(uint8_t subctrl, uint8_t *data, uint8_t len) {
	int ret = -1;
	uint8_t *sdata = OSA_FixedMemMalloc(len + 2);
	if(sdata != NULL) {
		sdata[0] = subctrl;
		sdata[1] = len;
		if(len > 0 && data != NULL)
			memcpy(&sdata[2], data, len);
		//TODO send data to tcpclient
		ret = Network_TcpServer_Send(sdata, len + 2);
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
bool App_SetDoPinByName(SApp *pApp, const char *name, uint32_t logic) {
	bool ret = false;
	for(int i = 0; i < DIGITAL_OUTPUT_NUM_CHANNEL; i++) {
		if(Str_Cmp((CPU_CHAR*)pApp->sCfg.sDO[i].name, name) == 0) {
			if(pApp->sCfg.sDO[i].ctrlType == CTRL_LEVEL) { // ctrl by level
				pApp->sDO.Node[i].lev = logic;
				GPIO_DRV_WritePinOutput(pApp->sDO.Node[i].id,
						(pApp->sCfg.sDO[i].activeType == ACTIVE_HIGH) ? logic : (!logic));
			} else { // ctrl by pulse
				GPIO_DRV_WritePinOutput(pApp->sDO.Node[i].id,
						(pApp->sCfg.sDO[i].activeType == ACTIVE_HIGH) ? logic : (!logic));
				OSA_SleepMs(100); //pApp->sCfg.sDO.duty
				GPIO_DRV_WritePinOutput(pApp->sDO.Node[i].id,
						(pApp->sCfg.sDO[i].activeType == ACTIVE_HIGH) ? !logic : (logic));
			}
			ret = true;
		}
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
void App_SetDoPinByIndex(SApp *pApp, uint8_t idx, uint32_t logic) {
	if(idx >= DIGITAL_OUTPUT_NUM_CHANNEL)
		ASSERT_VOID(FALSE);

	if(pApp->sCfg.sDO[idx].ctrlType == CTRL_LEVEL) { // ctrl by level
		pApp->sDO.Node[idx].lev = logic;
		GPIO_DRV_WritePinOutput(pApp->sDO.Node[idx].id,
			(pApp->sCfg.sDO[idx].activeType == ACTIVE_HIGH) ? logic : (!logic));
	} else { // ctrl by pulse
		GPIO_DRV_WritePinOutput(pApp->sDO.Node[idx].id,
				(pApp->sCfg.sDO[idx].activeType == ACTIVE_HIGH) ? logic : (!logic));
		OSA_SleepMs(100); //pApp->sCfg.sDO.duty
		GPIO_DRV_WritePinOutput(pApp->sDO.Node[idx].id,
				(pApp->sCfg.sDO[idx].activeType == ACTIVE_HIGH) ? !logic : (logic));
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
	bool value;
	for(int i = 0; i < DIGITAL_INPUT_NUM_CHANNEL; i++) {
		value = GPIO_DRV_ReadPinInput(pApp->sDI.Node[i].id);
		if(pApp->sCfg.sDI[i].activeType == ACTIVE_HIGH) {
			pApp->sDI.Node[i].lev = !value;
		} else {
			pApp->sDI.Node[i].lev = value;
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
typedef struct AnalogChannel_ {
	float vref;
	float vsen;
	float diff;
}SAnalogChannel;

void App_AiReadAllPort(SApp *pApp) {

#if 1
	uint8_t data[30];
	uint8_t recvLen;
	for(int i = 0; i < ANALOG_INPUT_NUM_CHANNEL; i++) {
		if(pApp->sAI.Node[i].enable)
		{
			Analog_SelectChannel(i);
			Analog_RecvFF_Reset(&pApp->sAnalogReader);
			OSA_SleepMs(1500); // wait enough for slave mcu wakup and send data
			memset(data, 30, 0);
			recvLen = Analog_RecvData(&pApp->sAnalogReader, data, 30);

			if(recvLen >= 2)
			{
				//LREP("recv: %s\r\n", data);
				float value = atof((char*)&data[1]);
				if(value > -20 && value < 500) {
					//LREP("recv value = %.2f\r\n", value);
					pApp->sCfg.sAiCalib.calib[i].raw = value;
					pApp->sAI.Node[i].value = pApp->sCfg.sAiCalib.calib[i].offset +
									(pApp->sCfg.sAiCalib.calib[i].raw *
									pApp->sCfg.sAiCalib.calib[i].coefficient);
					pApp->sAI.Node[i].status = TAG_STT_OK;
				} else {
					LREP("invalid value = %f\r\n", value);
					pApp->sAI.Node[i].status = TAG_STT_AI_FAILED;
				}
			} else {
				//LREP("A ");
				debug_putchar('A');
				pApp->sAI.Node[i].status = TAG_STT_AI_FAILED;
			}
		}
	}
	pApp->aiReadCount++;
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
int App_GenerateLogFile(SApp *pApp, uint8_t server) {

	int retVal = FR_OK;

	if(!pApp->sStatus.hwStat.Bits.bIntSdcardWorking)
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



	sprintf(year, "%04d", pApp->sStatus.time.tm_year);
	sprintf(mon, "%04d/%02d", pApp->sStatus.time.tm_year,
			pApp->sStatus.time.tm_mon);
	sprintf(day, "%04d/%02d/%02d", pApp->sStatus.time.tm_year,
			pApp->sStatus.time.tm_mon, pApp->sStatus.time.tm_mday);

	sprintf(time, "%04d%02d%02d%02d%02d%02d", pApp->sStatus.time.tm_year,
			pApp->sStatus.time.tm_mon, pApp->sStatus.time.tm_mday,
			pApp->sStatus.time.tm_hour, pApp->sStatus.time.tm_min,
			pApp->sStatus.time.tm_sec);

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
					/* NOTE: with SPI card, single close file return error FR_DISK_ERR,
					 double close file return success */
					retVal = f_close(&file);
					if(retVal != FR_OK) retVal = f_close(&file);
					if(retVal == FR_OK)
					{
						// TODO: send file name to Net module
#if NETWORK_MODULE_EN > 0 && NETWORK_FTP_CLIENT_EN > 0
						int err = Network_FtpClient_Send(
								(uint8_t*)day, (uint8_t*)filename, server);
						if(err != 0) {
							WARN("send file to network err = %d\r\n", err);
							retVal = -1;
						}
#endif
					} else {
						WARN("close file err = %d\r\n", retVal);
					}
				} else {
					WARN("create file but err = %d \r\n", retVal);
				}
			} else {
				retVal = -2;
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
int App_GenerateLogFileByName(SApp *pApp, const char *name, uint8_t server) {
	int retVal = FR_OK;

	if(!pApp->sStatus.hwStat.Bits.bIntSdcardWorking)
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

	sprintf(year, "%04d", pApp->sStatus.time.tm_year);
	sprintf(mon, "%04d/%02d", pApp->sStatus.time.tm_year,
			pApp->sStatus.time.tm_mon);
	sprintf(day, "%04d/%02d/%02d", pApp->sStatus.time.tm_year,
			pApp->sStatus.time.tm_mon, pApp->sStatus.time.tm_mday);

	sprintf(time, "%04d%02d%02d%02d%02d%02d", pApp->sStatus.time.tm_year,
			pApp->sStatus.time.tm_mon, pApp->sStatus.time.tm_mday,
			pApp->sStatus.time.tm_hour, pApp->sStatus.time.tm_min,
			pApp->sStatus.time.tm_sec);

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
					// TODO: send file name to Net module
#if NETWORK_MODULE_EN > 0 && NETWORK_FTP_CLIENT_EN > 0
						int err = Network_FtpClient_Send(
								(uint8_t*)day, (uint8_t*)filename, server);
						if(err != 0) {
							WARN("send file to network err = %d\r\n", err);
						}
#endif
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
	pApp->sStatus.time.tm_sec++;

	if(pApp->sStatus.time.tm_sec > 59) {
		pApp->sStatus.time.tm_sec = 0;
		pApp->sStatus.time.tm_min++;
	}

	if(pApp->sStatus.time.tm_min > 59) {
		pApp->sStatus.time.tm_min = 0;
		pApp->sStatus.time.tm_hour++;
	}

	if(pApp->sStatus.time.tm_hour > 23) {
		pApp->sStatus.time.tm_hour = 0;
		pApp->sStatus.time.tm_mday++;
	}

	if(pApp->sStatus.time.tm_mday > 30) {
		pApp->sStatus.time.tm_mday = 0;
		pApp->sStatus.time.tm_mon++;
	}

	return 0;
}

void App_PrintSystemStatus(SApp *pApp) {

	SComStatus *stat = &pApp->sStatus;
	LREP("system: \r\n");
	LREP("sdw: %d \r\n", stat->hwStat.Bits.bExtSdcardWorking);
	LREP("ethp: %d \r\n", stat->hwStat.Bits.bEthernetPlugged);
	LREP("ethw: %d \r\n", stat->hwStat.Bits.bEthernetWorking);
	LREP("ethip: %s\r\n", ipaddr_ntoa(&stat->fwStat.eth_ip));
	LREP("wlw: %d \r\n", stat->hwStat.Bits.bWirelessWorking);
	LREP("wlsq: %d \r\n", stat->fwStat.wl_rssi);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void external_card_detection(void)
{
	OS_ERR err;
	App_SetCtrlCode(pAppObj, CTRL_INIT_EXT_SDCARD);
    OSSemPost(&pAppObj->hSem, OS_OPT_POST_1, &err);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_NetTcpClientConnEvent(Network_Status event,
		Network_Interface interface) {
	switch(event) {
	case Status_Connected:
		WARN("Event connected\r\n");
		if(!pAppObj->sStatus.hwStat.Bits.bLogged)
		{
			uint8_t len = SYS_CTRL_USRNAME_LENGTH + SYS_CTRL_PASSWD_LENGTH;
			uint8_t *mem = OSA_FixedMemMalloc(len);
			if(mem != NULL) {
				Str_Copy_N((CPU_CHAR*)mem, (CPU_CHAR*)pAppObj->sCfg.sCom.ctrl_usrname,
						SYS_CTRL_USRNAME_LENGTH);
				Str_Copy_N((CPU_CHAR*)&mem[SYS_CTRL_USRNAME_LENGTH],
						(CPU_CHAR*)pAppObj->sCfg.sCom.ctrl_passwd,
						SYS_CTRL_PASSWD_LENGTH);
				Network_TcpClient_SendWLength(LOGGER_LOGGING_IN, mem, len);
				OSA_FixedMemFree(mem);
			} else {
				ASSERT(FALSE);
			}
		}
		break;
	case Status_Disconnected:
		pAppObj->sStatus.hwStat.Bits.bLogged = false;
		WARN("Event disconnected\r\n");
		break;
	case Status_Network_Down:
		pAppObj->sStatus.hwStat.Bits.bLogged = false;
		WARN("Event network down\r\n");
		break;
	case Status_Connecting:
		WARN("Event connecting\r\n");
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
void Clb_NetTcpClientRecvData(const uint8_t* data, int length) {

	WARN("client recv data len = %d \r\n", length);
	App_TcpClientRecvHandle(&data[4], length - 4);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_NetTcpClientSentData(const uint8_t* data, int length) {
	LREP("client send done len = %d\r\n", length);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_NetTcpClientError(const uint8_t* data, int length) {
	LREP("client send error len = %d\r\n", length);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_NetTcpServerConnEvent(Network_Status event,
		Network_Interface interface) {
	switch(event) {
	case Status_Connected:
		LREP("Event connected\r\n");
		break;
	case Status_Disconnected:
		LREP("Event disconnected\r\n");
		break;
	case Status_Network_Down:
		LREP("Event network down\r\n");
		break;
	case Status_Connecting:
		LREP("Event connecting\r\n");
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
void Clb_NetTcpServerRecvData(const uint8_t* data, int length) {
	LREP("server recv data len = %d\r\n", length);
//	for(int i = 0; i < length; i++) {
//		LREP("%x ", data[i]);
//	}
	App_TcpServerRecvHandle((uint8_t*)data);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_NetTcpServerSentData(const uint8_t* data, int length) {
	LREP("server send done len = %d\r\n", length);
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

	if(pAppObj->sStatus.hwStat.Bits.bReboot) {
		NVIC_SystemReset();
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
void Clb_NetTcpServerError(const uint8_t* data, int length) {
	ERR("tcp server send error %d\r\n", length);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
const char* msg_idle = " ";
const char* msg_sending_eth 	= "EHTHERNET SENDING";
const char* msg_sendok_eth 		= "EHTHERNET SEND OK";
const char* msg_sendfailed_eth 	= "EHTHERNET SEND FAILED";

const char* msg_sending_3G 		= "WIRELESS SENDING";
const char* msg_sendok_3G 		= "WIRELESS SEND OK";
const char* msg_sendfailed_3G 	= "WIRELESS SEND FAILED";

void Clb_NetFtpClientEvent(FtpStatus status,
		Network_Interface interface, uint8_t server) {
	LREP("ftp event: %d if: %d server %d\r\n", status, interface, server);
	switch(status) {
	case Ftp_Idle:
		App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG, msg_idle, strlen(msg_idle), false);
		break;
	case Ftp_Sending:
		if(interface == Interface_Ethernet) {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sending_eth,
					strlen(msg_sending_eth), false);
		} else {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sending_3G,
					strlen(msg_sending_eth), false);
		}
		break;
	case Ftp_SendFailed:
		if(interface == Interface_Ethernet) {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendfailed_eth,
					strlen(msg_sendfailed_eth), false);
		} else {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendfailed_3G,
					strlen(msg_sendfailed_3G), false);
		}
		break;
	case Ftp_SendSuccess:
		if(interface == Interface_Ethernet) {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendok_eth,
					strlen(msg_sendok_eth), false);
		} else {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendok_3G,
					strlen(msg_sendok_3G), false);
		}
		break;
	case Ftp_ReSending:
		if(interface == Interface_Ethernet) {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sending_eth,
					strlen(msg_sending_eth), false);
		} else {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sending_3G,
					strlen(msg_sending_eth), false);
		}
		break;
	case Ftp_ReSendFailed:
		if(interface == Interface_Ethernet) {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendfailed_eth,
					strlen(msg_sendfailed_eth), false);
		} else {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendfailed_3G,
					strlen(msg_sendfailed_3G), false);
		}
		break;
	case Ftp_ReSendSuccess:
		if(interface == Interface_Ethernet) {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendok_eth,
					strlen(msg_sendok_eth), false);
		} else {
			App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG,
					msg_sendok_3G,
					strlen(msg_sendok_3G), false);
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



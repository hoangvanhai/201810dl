/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

///////////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////////

// SDK Included Files
#include <includes.h>
#include "shell.h"
#include <app.h>
#include <TransUI.h>
#include <TransPC.h>
#include <rtc_comm.h>
#include <lib_str.h>
#include <network.h>

void clear_screen(int32_t argc, char**argv);
void send_queue(int32_t argc, char**argv);
void help_cmd(int32_t argc, char **argv);
void setdate(int32_t argc, char **argv);
void settime(int32_t argc, char **argv);
void status(int32_t argc, char **argv);
void restart(int32_t argc, char**argv);
void save_tag(int32_t argc, char**argv);
void control(int32_t argc, char**argv);
void test_log(int32_t argc, char**argv);

void list(int32_t argc, char **argv);
void mkdir(int32_t argc, char **argv);
void rmdir(int32_t argc, char **argv);
void create_file(int32_t argc, char**argv);
void remove_file(int32_t argc, char **argv);
void chdir(int32_t argc, char **argv);
void cwd(int32_t argc, char **argv);
void cat_file(int32_t argc, char **argv);
void stat(int32_t argc, char **argv);
void modbus(int32_t argc, char **argv);
void select_ai(int32_t argc, char **argv);



void my_shell_init(void);


const shell_command_t cmd_table[] =
{
	{"help", 	0u, 0u, help_cmd, 		"display this help message", ""},
	{"clear", 	0u, 0u, clear_screen, 	"clear screen", ""},
	{"send", 	1u, 1u, send_queue, 	"send queue message", "<tcb>"},
	{"reset", 	0u, 0u, restart, 		"reset system", ""},
	{"settime", 2u, 2u, settime, 		"set time", "<min> <hour>"},
	{"setdate", 3u, 3u, setdate, 		"set date", "<date> <month> <year>"},
	{"status", 	1u, 1u, status, 		"show system status", "<type>"},
	{"tag", 	1u, 1u, save_tag, 		"save tag cfg", "<tag>"},
	{"ctrl", 	1u, 2u, control, 		"control", "<param>"},
	{"ls", 		1u, 1u, list, 			"list all item in current wd", ""},
	{"mkdir", 	1u, 1u, mkdir, 			"make directory", "<dir>"},
	{"rmdir", 	1u, 1u, rmdir, 			"remove directory", "<dir>"},
	{"create", 	1u, 1u, create_file, 	"create file", "<file name>"},
	{"rm", 		1u, 1u, remove_file, 	"remove file", "<file name>"},
	{"cat", 	1u, 1u, cat_file, 		"show file content", "<file name>"},
	{"cd", 		1u, 1u, chdir, 			"change work directory", "<dir>"},
	{"pwd", 	0u, 0u, cwd, 			"print work directory", ""},
	{"stat", 	1u, 1u, stat, 			"check file status", "<file name>"},
	{"mb", 		1u, 1u, modbus, 		"read mobus", "<reg addr>"},
	{"select",	1u, 1u, select_ai, 		"select ai", "<ch>"},
	{"log",		1u, 1u, test_log, 		"create log file randomly", "<num>"},
	{0, 0u, 0u, 0, 0, 0}
};

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void my_shell_init(void)
{
	LREP("shell init done\r\n");
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void send_queue(int32_t argc, char**argv) {
	if (argc == 2)
	{
		if(strcmp(argv[1], "modbus") == 0) {
			//int i = 0;
			//for(; i < 100; i++) {
			LREP("send data to modbus task\r\n");
			uint8_t *p_msg = OSA_FixedMemMalloc(264);
			if(p_msg != NULL) {
				memset(p_msg, 0xFF, 264);
				OS_ERR err;
				OSTaskQPost(&TCB_task_modbus, p_msg, 264, OS_OPT_POST_FIFO, &err);
				if(err != OS_ERR_NONE) {
					LREP("task queue post failed \r\n");
				} else {
					LREP("task queue post ok \r\n");
				}
			} else {
				LREP("malloc failed \r\n");
			}
			//OSA_SleepMs(100);
			//}
		} else if(strcmp(argv[1], "fs") == 0) {


		} else if(strcmp(argv[1], "t") == 0) {

		} else {

			LREP("argument not supported\r\n\n");
		}
	}
}

#define SHELL_CFG_TERMINAL_HIGH         120
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void clear_screen(int32_t argc, char**argv) {
    int i;

    for(i =0 ; i< SHELL_CFG_TERMINAL_HIGH; i++) {
        LREP("\r\n\n");
    }

    for(i =0 ; i< SHELL_CFG_TERMINAL_HIGH; i++) {
        LREP("\033[F");
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
void restart(int32_t argc, char**argv) {
	NVIC_SystemReset();
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void help_cmd(int32_t argc, char **argv)
{
	(void)argc;
	(void)argv;

	shell_help();
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void setdate(int32_t argc, char **argv)
{
	if(argc == 4) {
		int day, month, year;
		day = atoi(argv[1]);
		month = atoi(argv[2]);
		year = atoi(argv[3]);
		if(RTC_SetDateTime(pAppObj->sStatus.time.tm_min,
				pAppObj->sStatus.time.tm_hour, day, month, year) == kStatus_I2C_Success) {
			LREP("set date successful year = %d\r\n", year);
		}
	} else {

		LREP("argument not supported\r\n\n");
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
void settime(int32_t argc, char **argv)
{
	if(argc == 3) {
		int min, hour;
		min = atoi(argv[1]);
		hour = atoi(argv[2]);
		RTC_SetDateTime(min, hour, pAppObj->sStatus.time.tm_mday,
						pAppObj->sStatus.time.tm_mon,
						pAppObj->sStatus.time.tm_year);
	} else {
		LREP("argument not supported\r\n\n");
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
void print_tag(STag *pHandle) {
	LREP("id: %2d - en %d - report %d - iid %d - ctyp %d - name %s - o2c %.2f - temp_c %.2f "
			"- press_c %.2f  - rmin %.2f - rmax %.2f - coef_a %.2f - coef_b %.2f \r\n",
				pHandle->id,
				pHandle->enable,
				pHandle->report,
				pHandle->input_id,
				pHandle->comp_type,
				pHandle->name,
        		pHandle->o2_comp,
        		pHandle->temp_comp,
        		pHandle->press_comp,
        		pHandle->raw_min,
        		pHandle->raw_max,
        		pHandle->coef_a,
        		pHandle->coef_b);
}

void print_tag1(STag *pHandle) {
	LREP("id  %d "
		"desc %s "
		"en %d "
		"rep %d "
		"calib %d "
		"error %d "
		"alarm %d "
		"itype %d "
		"id %d "
		"addr %d "
		"type %d "
		"format %d "
		"order %d "
		"comp %d\r\n",
		pHandle->id,
		pHandle->desc,
		pHandle->enable,
		pHandle->report,
		pHandle->has_calib,
		pHandle->has_error,
		pHandle->alarm_enable,
		pHandle->input_type,
		pHandle->input_id,
		pHandle->slave_reg_addr,
		pHandle->data_type,
		pHandle->data_format,
		pHandle->data_order,
		pHandle->comp_type);

	LREP("name %s "
		"raw %s "
		"std %s "
		"calib %d "
		"error %d "
		"o2 %d "
		"temp %d "
		"press %d "
		"o2_comp %.2f "
		"temp_comp %.2f "
		"press_comp %.2f "
		"rmin %.2f "
		"rmax %.2f "
		"scr_min %.2f "
		"scr_max %.2f "
		"coef_a %.2f "
		"coef_b %.2f "
		"alvalue %.2f\r\n",
		pHandle->name,
		pHandle->raw_unit,
		pHandle->std_unit,
		pHandle->pin_calib,
		pHandle->pin_error,
		pHandle->input_o2,
		pHandle->input_temp,
		pHandle->input_press,
		pHandle->o2_comp,
		pHandle->temp_comp,
		pHandle->press_comp,
		pHandle->raw_min,
		pHandle->raw_max,
		pHandle->scratch_min,
		pHandle->scratch_max,
		pHandle->coef_a,
		pHandle->coef_b,
		pHandle->alarm_value);

}

void print_sys(SSysCfg *pHandle) {
	LREP("noruser: %s norpw: %s rootuser: %s rootpw: %s\r\n",
			pHandle->sAccount.username,
			pHandle->sAccount.password,
			pHandle->sAccount.rootname,
			pHandle->sAccount.rootpass);

	LREP("en1: %d user1: %s pw1: %s en2: %d user2: %s pw2: %s"
			"ctrluser: %s ctrlpw: %s\r\n",
			pHandle->sCom.ftp_enable1,
			pHandle->sCom.ftp_usrname1,
			pHandle->sCom.ftp_passwd1,
			pHandle->sCom.ftp_enable2,
			pHandle->sCom.ftp_usrname2,
			pHandle->sCom.ftp_passwd2,
			pHandle->sCom.ctrl_usrname,
			pHandle->sCom.ctrl_passwd);
}

void print_comm(SCommon *pHandle) {

	LREP("ftpip1 %s\r\n", ipaddr_ntoa(&pHandle->server_ftp_ip1));
	LREP("ftpip2 %s\r\n", ipaddr_ntoa(&pHandle->server_ftp_ip2));
	LREP("ctrlip %s\r\n", ipaddr_ntoa(&pHandle->server_ctrl_ip));

	LREP("tinh %s "
		"coso %s "
		"tram %s \r\n"
		"prefix1 %s "
		"usrname1 %s "
		"passwd1 %s "
		"prefix2 %s "
		"usrname2 %s "
		"passwd2 %s "
		"ctrl %s "
		"passwd %s \r\n",
			pHandle->tinh,
			pHandle->coso,
			pHandle->tram,
			pHandle->ftp_prefix1,
			pHandle->ftp_usrname1,
			pHandle->ftp_passwd1,
			pHandle->ftp_prefix2,
			pHandle->ftp_usrname2,
			pHandle->ftp_passwd2,
			pHandle->ctrl_usrname,
			pHandle->ctrl_passwd);
//	LREP("dev_ip %d "
//			"nm %d "
//			"dhcp %d "
//			"ftpen1 %d "
//			"ftpen2 %d \r\n"
//			"ftpip1 %s "
//			"ftpp1 %d "
//			"ftpip2 %d "
//			"fptp2 %d "
//			"ctrlip %d "
//			"ctrlp %d \r\n"
//			"tinh %s "
//			"coso %s "
//			"tram %s \r\n"
//			"prefix1 %s "
//			"usrname1 %s "
//			"passwd1 %s "
//			"prefix2 %s "
//			"usrname2 %s "
//			"passwd2 %s "
//			"ctrl %s "
//			"passwd %s \r\n"
//			"scan %d "
//			"log %d "
//			"brate %d\r\n",
//				pHandle->dev_ip,
//				pHandle->dev_netmask,
//				pHandle->dev_dhcp,
//				pHandle->ftp_enable1,
//				pHandle->ftp_enable2,
//				ipaddr_ntoa(&pHandle->server_ftp_ip1),
//				pHandle->server_ftp_port1,
//				pHandle->server_ftp_ip2,
//				pHandle->server_ftp_port2,
//				pHandle->server_ctrl_ip,
//				pHandle->server_ctrl_port,
//				pHandle->tinh,
//				pHandle->coso,
//				pHandle->tram,
//				pHandle->ftp_prefix1,
//				pHandle->ftp_usrname1,
//				pHandle->ftp_passwd1,
//				pHandle->ftp_prefix2,
//				pHandle->ftp_usrname2,
//				pHandle->ftp_passwd2,
//				pHandle->ctrl_usrname,
//				pHandle->ctrl_passwd,
//				pHandle->scan_dur,
//				pHandle->log_dur,
//				pHandle->modbus_brate);
}

void status(int32_t argc, char **argv) {
	if(strcmp(argv[1], "time") == 0) {
		LREP("Current Time: %02d/%02d/%d %02d:%02d:%02d\r\n\r\n",
					pAppObj->sStatus.time.tm_mday,
					pAppObj->sStatus.time.tm_mon,
					pAppObj->sStatus.time.tm_year,
					pAppObj->sStatus.time.tm_hour,
					pAppObj->sStatus.time.tm_min,
					pAppObj->sStatus.time.tm_sec);
	} else if(strcmp(argv[1], "conf") == 0) {
		//LREP("sizeof name = %d\r\n", sizeof(pAppObj->sCfg.sTag[0].name));
		print_sys(&pAppObj->sCfg);
		print_comm(&pAppObj->sCfg.sCom);
		for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
			print_tag(&pAppObj->sCfg.sTag[i]);
		}
	} else if(strcmp(argv[1], "net") == 0) {
		WARN("hw 			%02x:%02x:%02x:%02x:%02x:%02x\r\n",
				eth0.hwaddr[0], eth0.hwaddr[1],
				eth0.hwaddr[2], eth0.hwaddr[3],
				eth0.hwaddr[4], eth0.hwaddr[5]);

		WARN("ip 			%s\r\n", ipaddr_ntoa(&eth0.ip_addr));
		WARN("nm			%s\r\n", ipaddr_ntoa(&eth0.netmask));
		WARN("gw 			%s\r\n", ipaddr_ntoa(&eth0.gw));

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


void list(int32_t argc, char **argv) {
	//LREP("list all: \r\n");
	//int retVal = show_content((char*)pAppObj->currPath);
	int retVal = show_content(argv[1]);
	if(retVal != FR_OK) {
		LREP("list failed err = %d\r\n", retVal);
	} else {
		//LREP("list done \r\n");
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
void mkdir(int32_t argc, char **argv) {

	int retVal = f_mkdir(argv[1]);
	if(retVal != FR_OK) {
		LREP("mkdir %s err = %d\r\n", argv[1], retVal);
	} else {
		LREP("mkdir %s successful !\r\n", argv[1]);
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
void rmdir(int32_t argc, char **argv) {
	//if(check_obj_existed(argv[1]))
	{
		int err = remove_directory(argv[1]);
//		FILINFO *fno = (FILINFO *)OSA_FixedMemMalloc(sizeof(FILINFO));
//		if(fno == NULL) {
//			LREP("malloc error\r\n");
//			return;
//		}
//		int err = delete_node(argv[1], strlen(argv[1]), fno);

		if(err == FR_OK) {
			LREP("remove %s successfully\r\n", argv[1]);
		} else {
			LREP("remove %s failed err = %d \r\n", argv[1], err);
		}
	}
//	else {
//		LREP("%s is not existed \r\n");
//	}
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void remove_file(int32_t argc, char** argv) {
	int retVal = f_unlink(argv[1]);
	if(retVal != FR_OK) {
		LREP("delete file %s failed err = %d\r\n", argv[1], retVal);
	} else {
		LREP("delete file %s successfully \r\n", argv[1]);
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
void create_file(int32_t argc, char** argv) {
	FIL file;
	int retVal = f_open(&file, argv[1], FA_CREATE_ALWAYS | FA_WRITE);
	if(retVal != FR_OK) {
		LREP("create file err = %d\r\n", retVal);
	} else {
		char *msg = "this is data file \r\n";
		UINT written;
		f_write(&file, msg, strlen(msg), &written);
		LREP("create %s file success write string to file len = %d\r\n", argv[1], written);
	}
	f_close(&file);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void chdir(int32_t argc, char **argv) {
	int retVal = f_chdir(argv[1]);
	if(retVal != FR_OK) {
		LREP("chdir to %s err = %d\r\n", argv[1], retVal);
	} else {
		LREP("chdir to %s successfully\r\n", argv[1]);
		current_directory();
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
void cwd(int32_t argc, char **argv) {
	current_directory();
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void cat_file(int32_t argc, char **argv) {
	cat(argv[1]);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void stat(int32_t argc, char **argv) {
	obj_stat(argv[1]);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void modbus(int32_t argc, char **argv) {

	int regAddr = atoi(argv[1]);
	uint16_t rlen = 0;
	uint8_t data[20];
	int retVal = MBMaster_Read(&pAppObj->sModbus,
			1,
			3,
			regAddr,
			1, data, &rlen);

	if(retVal != MB_SUCCESS) {
		LREP("read err = %d\r\n", retVal);
	} else {
		LREP("recv frame: ");
		for(int i = 0; i < rlen; i++) {
			LREP("%x ", data[i]);
		}
		LREP("\r\n");
	}
}
void select_ai(int32_t argc, char **argv) {
	int idx = atoi(argv[1]);
	if(idx < 0 || idx > 11) {
		LREP("invalid index\r\n");
		return;
	}


	Analog_SelectChannel(idx);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void save_tag(int32_t argc, char**argv) {
	int idx = atoi(argv[1]);
	if(idx < 0 || idx > 11) {
		LREP("invalid index\r\n");
		return;
	}
	Str_Copy((CPU_CHAR*)pAppObj->sCfg.sTag[idx].name, "SET_NAME");
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void control(int32_t argc, char**argv) {
	uint8_t data[100];
	if(strcmp(argv[1], "save") == 0) {
		App_SaveConfig(pAppObj, CONFIG_FILE_PATH);
	} else if(strcmp(argv[1], "reset") == 0) {
		int retVal = f_unlink(CONFIG_FILE_PATH);
		if(retVal == FR_OK) {
			LREP("remove file success\r\n");
		} else {
			LREP("remove file failed err = %d\r\n", retVal);
		}
	} else if(strcmp(argv[1], "pc") == 0) {

		for(int i = 0; i < 100; i++)
		{
			App_SendPC(pAppObj, 100, data, 100, false);
			OSA_SleepMs(100);
		}
	} else if(strcmp(argv[1], "ui") == 0) {

		//uint32_t time;
		for(int i = 0; i < 100; i++)
		{
			App_SendUI(pAppObj, 100, data, 100, false);
			OSA_SleepMs(100);
		}
	} else if(strcmp(argv[1], "stat") == 0) {
		pAppObj->sStatus.hwStat.Bits.bStatitics =
				!pAppObj->sStatus.hwStat.Bits.bStatitics;
		LREP("stat %d\r\n", pAppObj->sStatus.hwStat.Bits.bStatitics);
	} else if(strcmp(argv[1], "server") == 0) {
		for(int i = 0; i < 1000; i++)
		{
			App_SendPCNetworkClient(100, data, 100);
			OSA_SleepMs(100);
		}
	} else if(strcmp(argv[1], "client") == 0) {
		for(int i = 0; i < 1000; i++)
		{
			Network_TcpClient_Send(data, 100);
			OSA_SleepMs(100);
		}
	} else if(strcmp(argv[1], "log") == 0) {
		App_GenerateLogFile(pAppObj, SEND_SERVER_ALL);
	} else if(strcmp(argv[1], "don") == 0) {
		LREP("control don\r\n");
		int idx = atoi(argv[2]);
		if(idx >= 0 && idx <= 11) {
			App_SetDoPinByIndex(pAppObj, idx, 1);
		}
	} else if(strcmp(argv[1], "doff") == 0) {
		LREP("control doff\r\n");
		int idx = atoi(argv[2]);
		if(idx >= 0 && idx <= 11) {
			App_SetDoPinByIndex(pAppObj, idx, 0);
		}
	} else if(strcmp(argv[1], "del") == 0) {
		LREP("delete ring file\r\n");
		int retVal = f_unlink("/conf/retrytable0.dat");
		ASSERT(retVal == FR_OK);
		retVal = f_unlink("/conf/retrytable1.dat");
		ASSERT(retVal == FR_OK);
	} else if(strcmp(argv[1], "ref") == 0) {
		if(strcmp(argv[2], "on") == 0) {
			GPIO_DRV_ClearPinOutput(RefCurrEn);
			ERR("on ref current \r\n");
		} else {
			GPIO_DRV_SetPinOutput(RefCurrEn);
			ERR("off ref current \r\n");

		}
	} else if(strcmp(argv[1], "lev") == 0) {
		uint16_t lev = atoi(argv[2]);

		if(lev >= 0 && lev <= 4095) {
			int err = DAC_SetRefLevel(lev, false);
			ERR("set ref level = %d err = %d\r\n", lev, err);
		}
	} else if(strcmp(argv[1], "dac") == 0) {
		uint16_t lev = atoi(argv[2]);

		if(lev >= 0 && lev <= 4095) {
			DAC_InterfaceSetLevel(lev);
			ERR("set ref level = %d \r\n", lev);
		}
	} else if(strcmp(argv[1], "lr") == 0) {
		show_content_recursive("/");
	} else if(strcmp(argv[1], "wr") == 0) {
		if(!pAppObj->sStatus.hwStat.Bits.bI2CBusy) {
			pAppObj->sStatus.hwStat.Bits.bI2CBusy = true;
			uint16_t lev = atoi(argv[2]);
			LREP("write to eeprom %x\r\n", lev);
			uint8_t *data = OSA_FixedMemMalloc(512);
			if(data != NULL) {
				int rlen = 0;
				for(int i = 0; i < 512; i++) data[i] = 0x55;
				ASSERT(CONF_WriteData(lev, data, 512) == kStatus_I2C_Success);
				OSA_SleepMs(100);
				memset(data, 0, 512);
				ASSERT(CONF_ReadData(lev, data, 512, &rlen) == kStatus_I2C_Success);
				LREP("rdata: %d\r\n", rlen);
				for(int i = 0; i < rlen; i++) {
					LREP("%x ", data[i]);
				}

				OSA_FixedMemFree(data);
			}
			pAppObj->sStatus.hwStat.Bits.bI2CBusy = false;
		}
	} else if(strcmp(argv[1], "rd") == 0) {
		if(!pAppObj->sStatus.hwStat.Bits.bI2CBusy) {
			pAppObj->sStatus.hwStat.Bits.bI2CBusy = true;

			LREP("read from eeprom \r\n");
			uint16_t lev = atoi(argv[2]);
			uint8_t *data = OSA_FixedMemMalloc(128);
			if(data != NULL) {
				int rlen = 0;
				memset(data, 0, 128);
				ASSERT(CONF_ReadData(lev, data, 128, &rlen) == kStatus_I2C_Success);
				LREP("rdata: %d\r\n", rlen);
				for(int i = 0; i < rlen; i++) {
					LREP("%x ", data[i]);
				}
				LREP("\r\n");
				OSA_FixedMemFree(data);
			}
			pAppObj->sStatus.hwStat.Bits.bI2CBusy = false;
		}
	} else if(strcmp(argv[1], "erall") == 0) {
		if(!pAppObj->sStatus.hwStat.Bits.bI2CBusy) {
			pAppObj->sStatus.hwStat.Bits.bI2CBusy = true;
			ASSERT(CONF_EraseFlash() == kStatus_I2C_Success);
			pAppObj->sStatus.hwStat.Bits.bI2CBusy = false;
		} else {
			LREP("I2C is bussy\r\n");
		}
	} else if(strcmp(argv[1], "ers") == 0) {
		uint16_t lev = atoi(argv[2]);
		CONF_EraseSector(lev, lev);
	} else if(strcmp(argv[1], "ck") == 0) {
		LREP("written = %d\r\n", CONF_CheckWrittenApp());
	} else if(strcmp(argv[1], "wk") == 0) {
		CONF_WriteKeyApp();
	} else if(strcmp(argv[1], "msg") == 0) {
		App_SendUI(pAppObj, LOGGER_GET | LOGGER_STREAM_MSG, argv[2], strlen(argv[2]), false);
		LREP("send ui msg %s\r\n", argv[2]);
	}
}


void test_log(int32_t argc, char**argv) {
	int idx = atoi(argv[1]);
	uint32_t randout;
	int randVal;
	for(int i = 0; i < idx; i++) {
		RNGA_DRV_GetRandomData(0, &randout, sizeof(uint32_t));
		randout = abs(randout);
		randout = randout % 20;
		randVal = randout;
		randVal = MAX(randVal, 3);
		App_GenerateLogFile(pAppObj, SEND_SERVER_ALL);
		randVal = 1000 * randVal;
		WARN("Delay %d\r\n", randVal);
		OSA_SleepMs(randVal);
	}

}










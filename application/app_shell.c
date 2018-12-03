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
#include <Transceiver.h>
#include <rtc_comm.h>
#include <lib_str.h>

void clear_screen(int32_t argc, char**argv);
void send_queue(int32_t argc, char**argv);
void help_cmd(int32_t argc, char **argv);
void setdate(int32_t argc, char **argv);
void settime(int32_t argc, char **argv);
void status(int32_t argc, char **argv);
void restart(int32_t argc, char**argv);
void save_tag(int32_t argc, char**argv);
void control(int32_t argc, char**argv);

void list(int32_t argc, char **argv);
void mkdir(int32_t argc, char **argv);
void rmdir(int32_t argc, char **argv);
void create_file(int32_t argc, char**argv);
void remove_file(int32_t argc, char **argv);
void chdir(int32_t argc, char **argv);
void cwd(int32_t argc, char **argv);
void cat_file(int32_t argc, char **argv);
void stat(int32_t argc, char **argv);

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
	{"ctrl", 	1u, 1u, control, 		"control", "<param>"},
	{"ls", 		1u, 1u, list, 			"list all item in current wd", ""},
	{"mkdir", 	1u, 1u, mkdir, 			"make directory", "<dir>"},
	{"rmdir", 	1u, 1u, rmdir, 			"remove directory", "<dir>"},
	{"create", 	1u, 1u, create_file, 	"create file", "<file name>"},
	{"rm", 		1u, 1u, remove_file, 	"remove file", "<file name>"},
	{"cat", 	1u, 1u, cat_file, 		"show file content", "<file name>"},
	{"cd", 		1u, 1u, chdir, 			"change work directory", "<dir>"},
	{"pwd", 	0u, 0u, cwd, 			"print work directory", ""},
	{"stat", 	1u, 1u, stat, 			"check file status", "<file name>"},
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
				OSTaskQPost(&pAppObj->TCB_task_modbus, p_msg, 264, OS_OPT_POST_FIFO, &err);
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
		if(RTC_SetDateTime(pAppObj->sDateTime.tm_min,
				pAppObj->sDateTime.tm_hour, day, month, year) == kStatus_I2C_Success) {
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
		RTC_SetDateTime(min, hour, pAppObj->sDateTime.tm_mday,
						pAppObj->sDateTime.tm_mon,
						pAppObj->sDateTime.tm_year);
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
	LREP("id: %d - en %d - report %d - iid %d - ctyp %d - name %s - o2c %f - temp_c %f "
			"- press_c %f  - rmin %f - rmax %f - coef_a %f - coef_b %f \r\n",
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

void status(int32_t argc, char **argv) {
	if(strcmp(argv[1], "time") == 0) {
		LREP("Current Time: %02d/%02d/%d %02d:%02d:%02d\r\n\r\n",
					pAppObj->sDateTime.tm_mday,
					pAppObj->sDateTime.tm_mon,
					pAppObj->sDateTime.tm_year,
					pAppObj->sDateTime.tm_hour,
					pAppObj->sDateTime.tm_min,
					pAppObj->sDateTime.tm_sec);
	} else if(strcmp(argv[1], "conf") == 0) {
		//LREP("sizeof name = %d\r\n", sizeof(pAppObj->sCfg.sTag[0].name));
		print_sys(&pAppObj->sCfg);
		for(int i = 0; i < SYSTEM_NUM_TAG; i++) {
			print_tag(&pAppObj->sCfg.sTag[i]);
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
		LREP("delete file %s failed err = %d\r\n", retVal);
	} else {
		LREP("delete file %s successfully \r\n");
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
		uint8_t data[100];
		for(int i = 0; i < 100; i++)
		{
			App_SendPC(pAppObj, 100, data, 100, false);
			OSA_SleepMs(100);
		}
	} else if(strcmp(argv[1], "ui") == 0) {
		uint8_t data[100];
		//uint32_t time;
		for(int i = 0; i < 100; i++)
		{
			App_SendUI(pAppObj, 100, data, 100, false);
			OSA_SleepMs(100);
		}
	}
}










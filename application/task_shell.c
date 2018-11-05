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

void clear_screen(int32_t argc, char**argv);
void send_queue(int32_t argc, char**argv);
void help_cmd(int32_t argc, char **argv);
void setdate(int32_t argc, char **argv);
void settime(int32_t argc, char **argv);
void status(int32_t argc, char **argv);
void restart(int32_t argc, char**argv);
void my_shell_init(void);


const shell_command_t cmd_table[] =
{
	{"help", 	0u, 0u, help_cmd, 		"display this help message", ""},
	{"clear", 	0u, 0u, clear_screen, 	"clear screen", ""},
	{"send", 	1u, 1u, send_queue, 	"send queue message", "<tcb>"},
	{"reset", 	0u, 0u, restart, 		"reset system", ""},
	{"settime", 2u, 2u, settime, 		"set time", "<min> <hour>"},
	{"setdate", 3u, 3u, setdate, 		"set date", "<date> <month> <year>"},
	{"status", 	0u, 0u, status, 		"show system status", ""},
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
			int i = 0;
			for(;i < 100; i++) {
			LREP("send data to file system task\r\n");
			uint8_t *p_msg = OSA_FixedMemMalloc(100);
			if(p_msg != NULL) {
				OS_ERR err;
				OSTaskQPost(&TCB_task_filesystem, p_msg, 100, OS_OPT_POST_FIFO, &err);
				if(err != OS_ERR_NONE) {
					LREP("task queue post failed \r\n");
				} else {
					LREP("task queue post ok \r\n");
				}
			} else {
				LREP("malloc failed \r\n");
			}
			OSA_SleepMs(100);
			}
		} else if(strcmp(argv[1], "t") == 0) {
			uint8_t data[100];
			for(int i = 0; i < 100; i++)
				data[i] = i;
			Trans_Send(&sApp.sTransPc, 100, data, FRM_DATA);
			LREP("control passed\r\n");
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
		if(RTC_SetDateTime(g_DateTime.tm_min, g_DateTime.tm_hour, day, month, year) == kStatus_I2C_Success) {
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
		RTC_SetDateTime(min, hour, g_DateTime.tm_mday, g_DateTime.tm_mon, g_DateTime.tm_year);
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
void status(int32_t argc, char **argv) {
	LREP("Current Time: %02d/%02d/%d %02d:%02d:%02d\r\n\r\n",
			g_DateTime.tm_mday, g_DateTime.tm_mon, g_DateTime.tm_year,
			g_DateTime.tm_hour, g_DateTime.tm_min, g_DateTime.tm_sec);
}


void task_shell(task_param_t param)
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

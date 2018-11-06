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
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
SApp sApp;
extern float max_latch, min_latch;

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

	pAppObj = pApp;
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

	RTC_InitI2C(0);
	OSA_SleepMs(100);

	if(RTC_GetTimeDate(&g_DateTime) == 0) {
		if(g_DateTime.tm_year == 1990) {
			RTC_SetDateTime(0, 0, 1, 1, 2018);
		}

	} else {
		ASSERT(FALSE);
	}

	//FM_Init(0);

	while(1) {
		OSA_SleepMs(1000);
		if(RTC_GetTimeDate(&g_DateTime) == 0) {
			/*
			LREP("sec %d min %d hour %d day: %d date: %d month: %d year: %d\r\n",
					g_DateTime.tm_sec, g_DateTime.tm_min, g_DateTime.tm_hour, g_DateTime.tm_wday,
					g_DateTime.tm_mday, g_DateTime.tm_mon, g_DateTime.tm_year);
					*/
			/* LREP("[Max %.3f - Min %.3f]\r\n", max_latch, min_latch); */

		} else {
			ASSERT(FALSE);
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

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TimerControl(void *p_tmr, void *p_arg);
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


OS_TMR hTimer;
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
	LREP("start create timer\r\n");
	OSTmrCreate(&hTimer,
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
		OSTmrStart(&hTimer, &err);

		if (err == OS_ERR_NONE) {
			/* Timer was created but NOT started */
			LREP("timer started ok\r\n");
		} else {
			LREP("timer start failed\r\n");
		}
	} else {
		LREP("timer create failed\r\n");
	}


	Modbus_Init(&pApp->sModbus, BOARD_MODBUS_UART_INSTANCE, BOARD_MODBUS_UART_BAUD, 0, 0);

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
void App_TaskSerialcomm(task_param_t param) {

	SApp *pApp = (SApp *)param;
	OS_ERR	err;
	CPU_TS	ts;

	Trans_RegisterClbEvent((STrans*)param, TRANS_EVT_RECV_DATA, Clb_TransPC_RecvEvent);
	Trans_RegisterClbEvent((STrans*)param, TRANS_EVT_SENT_DATA, Clb_TransPC_SentEvent);

	Trans_Init((STrans*)param, BOARD_TRANSPC_UART_INSTANCE, BOARD_TRANSPC_UART_BAUD, &pApp->TCB_task_serialcomm);

	LREP("task serial comm init done !\r\n");
	LREP("SFrameInfo Size: %d\r\n", sizeof(SFrameInfo));
	while(1) {
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			Trans_Task((STrans *)param);
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
//	OS_ERR err;
//	void 	*p_msg;
//	OS_MSG_SIZE msg_size;
//	CPU_TS	ts;

	BOOL run = FALSE;



    while (1)
    {

    	OSA_SleepMs(1000);
//    	if(run == FALSE) {
//    		run = TRUE;
//    		if(init_filesystem() != FR_OK) {
//				LREP("Init FAT FS failed \r\n");
//			} else {
//				LREP("Init FAT FS successful \r\n");
//			}
//    	}

    	/*
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, 0, &err);

    	p_msg = OSTaskQPend(1000, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
		if(err == OS_ERR_NONE) {

			LREP("filesystem get msg size = %d ts = %d\r\n", msg_size, ts);

			OSA_FixedMemFree((uint8_t*)p_msg);
		}*/
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

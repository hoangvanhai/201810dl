/*
 * task_serialcomm.c
 *
 *  Created on: Oct 29, 2018
 *      Author: PC
 */

#include "fsl_debug_console.h"
#include <board.h>
#include <includes.h>
#include <app.h>



void Clb_TransPC_RecvEvent(void *pData, uint8_t u8Type) {
	static uint32_t count = 0;
	LREP("Recv frm event: %d - ctrl = 0x%x\r\n", count++, u8Type);
}

void Clb_TransPC_SentEvent(void *pDatam, uint8_t u8Type) {
	LREP("Sent done event 0x%x\r\n", u8Type);
}

void task_serialcomm(task_param_t param) {
	OS_ERR	err;
	CPU_TS	ts;

	Trans_RegisterClbEvent((STrans*)param, TRANS_EVT_RECV_DATA, Clb_TransPC_RecvEvent);
	Trans_RegisterClbEvent((STrans*)param, TRANS_EVT_SENT_DATA, Clb_TransPC_SentEvent);

	Trans_Init((STrans*)param, BOARD_TRANSPC_UART_INSTANCE, BOARD_TRANSPC_UART_BAUD, &TCB_task_serialcomm);

	LREP("task serial comm init done !\r\n");
	LREP("SFrameInfo Size: %d\r\n", sizeof(SFrameInfo));
	while(1) {
		OSTaskSemPend(1000, OS_OPT_PEND_BLOCKING, &ts, &err);
		if(err == OS_ERR_NONE) {
			Trans_Task((STrans *)param);
		}
	}
}

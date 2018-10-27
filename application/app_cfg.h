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

#ifndef APPLICATION_APP_CFG_H_
#define APPLICATION_APP_CFG_H_



/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/


#define HW_INVALID							-1

#define TICKS_PER_SECOND					2000
#define DEBUG_BAUDRATE						115200L
#define DATA_BAUDTATE						115200L

#define RS485_DEBUG_RECV_ENABLE
#define RS485_DEBUG_SEND_ENABLE

#define RS485_UI_DATA_PORT_BASE				HW_INVALID
#define RS485_UI_DATA_PIN_IDX				HW_INVALID


#define TRANS_PC_RX_PRIO						2
#define TRANS_PC_TX_PRIO						2

#define TRANSL1_V1							1
#define TRANSL1_V2							2

#define TRANSL1_VER							TRANSL1_V1


#define TRANS_UART_PORT						(UART0)
#define DEBUG_UART_PORT						(UART1)


#define TRANS_UART_ISR_PRIORIRY				1
#define ADC_ISR_PRIORITY					2
#define TIMER_ISR_PRIORITY					3
#define DEBUG_UART_ISR_PRIORITY				4


/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/




// task prio
#define TASK_SHELL_PRIO         		6U
#define TASK_MODBUS_PRIO          		4U
#define TASK_FILESYSTEM_PRIO         	5U



// task size
#define TASK_SHELL_STACK_SIZE   		1536U
#define TASK_MODBUS_STACK_SIZE    		1024U
#define TASK_FILESYSTEM_STACK_SIZE   	512U



#endif /* APPLICATION_APP_CFG_H_ */

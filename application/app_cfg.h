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


#define TRANS_UART_ISR_PRIORIRY				1
#define ADC_ISR_PRIORITY					2
#define TIMER_ISR_PRIORITY					3
#define DEBUG_UART_ISR_PRIORITY				4

#define DIGITAL_INPUT_NUM_CHANNEL			8
#define DIGITAL_OUTPUT_NUM_CHANNEL			6
#define ANALOG_INPUT_NUM_CHANNEL			12
#define SYSTEM_NUM_TAG						ANALOG_INPUT_NUM_CHANNEL

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/


// task prio
#define TASK_SHELL_PRIO         		6U
#define TASK_MODBUS_PRIO          		4U
#define TASK_FILESYSTEM_PRIO         	5U
#define TASK_SERIALCOMM_PRIO			7U
#define TASK_PERIODIC_PRIO				8U
#define TASK_STARTUP_PRIO				9U

// task size
#define TASK_STARTUP_STACK_SIZE   		2096U
#define TASK_SHELL_STACK_SIZE   		2096U
#define TASK_MODBUS_STACK_SIZE    		2048U
#define TASK_FILESYSTEM_STACK_SIZE   	2024U
#define TASK_SERIAL_COMM_STACK_SIZE		2024U
#define TASK_PERIODIC_STACK_SIZE		4096U


#endif /* APPLICATION_APP_CFG_H_ */

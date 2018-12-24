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

/************************** Constant Definitions *****************************/


// task prio
#define TASK_MODBUS_PRIO          			3U
#define TASK_PERIODIC_PRIO					4U
#define TASK_SERIALCOMM_PRIO				5U
#define TASK_UI_PRIO         				6U
#define TASK_AI_PRIO         				7U
#define TASK_STARTUP_PRIO					8U
#define TASK_SHELL_PRIO         			9U

#define TASK_FTP_CLIENT_SENDER_PRIO			10U
#define TASK_TCP_CLIENT_SENDER_PRIO			11U
#define TASK_TCP_CLIENT_LISTEN_PRIO			12U
#define TASK_TCP_SERVER_SENDER_PRIO			13U
#define TASK_TCP_SERVER_LISTEN_PRIO			14U


// task size
#define TASK_STARTUP_STACK_SIZE   			2048U
#define TASK_AI_STACK_SIZE   				2048U
#define TASK_SHELL_STACK_SIZE   			2048U
#define TASK_MODBUS_STACK_SIZE    			2048U
#define TASK_UI_STACK_SIZE   				2048U
#define TASK_SERIAL_COMM_STACK_SIZE			2048U
#define TASK_PERIODIC_STACK_SIZE			4096U

#define TASK_TCP_CLIENT_SENDER_SIZE			2048U
#define TASK_TCP_CLIENT_LISTEN_SIZE			2048U
#define TASK_TCP_SERVER_SENDER_SIZE			2048U
#define TASK_TCP_SERVER_LISTEN_SIZE			2048U
#define TASK_FTP_CLIENT_SENDER_SIZE			2048U

// COMUNICATION
#define TRANS_RX_PRIO						2
#define TRANS_TX_PRIO						2
#define TRANSL1_V1							1
#define TRANSL1_V2							2
#define TRANSL1_VER							TRANSL1_V1


#define BOARD_FRDM_K64F						0
#define BOARD_VERSION_1						1
#define BOARD_VERSION_2						2


#define BOARD_USE_VERSION					BOARD_VERSION_1

#define NETWORK_MODULE_EN					1
#define NETWORK_FTP_CLIENT_EN				1
#define NETWORK_TCP_CLIENT_EN				0
#define NETWORK_TCP_SERVER_EN				0







/**************************** Type Definitions *******************************/

#endif /* APPLICATION_APP_CFG_H_ */

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

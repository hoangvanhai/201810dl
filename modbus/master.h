
#ifndef MODBUS_MASTER_H_
#define MODBUS_MASTER_H_

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

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/


#define FRM_ID_IDX			0
#define FRM_FC_IDX			1
#define FRM_DATA_IDX		2


#define FC_READ_AI_REG					0x04
#define FC_READ_AO_H_REG				0x03

#define FC_WRITE_AO_H_REG_SING			0x04
#define FC_WRITE_AO_H_REG_MULT			0x03



/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/



#endif /* MODBUS_MASTER_H_ */

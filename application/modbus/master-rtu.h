
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
#include <stdint.h>
#include <layer1.h>
#include <checksum.h>
#include <definition.h>
/************************** Constant Definitions *****************************/


#define MB_DELAY_MS				200


/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
uint16_t MBMaster_Read(SModbus *pModbus, uint8_t slv_adrr, uint8_t fc,
						uint16_t data_addr, uint16_t num_data,
						uint8_t *read_frame, uint16_t *len);

uint16_t MBMaster_Parse(const uint8_t* data, uint8_t data_format, void *rdata);


uint16_t MBMaster_Write(uint8_t id, uint8_t fc,
						uint16_t addr, uint8_t num_reg,
						uint8_t *data, uint16_t len);

/************************** Variable Definitions *****************************/

/*****************************************************************************/



#endif /* MODBUS_MASTER_H_ */

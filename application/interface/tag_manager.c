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
#include <tag_manager.h>
#include <master.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void Tag_Modbus_ReadAll() {
//	int retVal;
//	for(int i = 0; i < TAG_MAX_CHANNEL; i++) {
//		if(g_tagArray[i].input_type == TIT_MB) {
//			uint8_t data[4];
//			uint16_t rlen;
//			retVal = MBMaster_Read(g_tagArray[i].id, g_tagArray[i].slave_reg_addr, 0x03, 1, data, &rlen);
//			if(retVal != MB_SUCCESS) {
//				g_tagArray[i].status = TAG_STT_MB_FAILED;
//			} else {
//				g_tagArray[i].status = TAG_STT_OK;
//			}
//		}
//	}
}




void Tag_AiDi_ReadAll() {

}





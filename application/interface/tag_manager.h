#ifndef APPLICATION_INTERFACE_TAG_MANAGER_H_
#define APPLICATION_INTERFACE_TAG_MANAGER_H_

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
#include <includes.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
void Tag_Modbus_ReadAll();
void Tag_AiDi_ReadAll();
/************************** Variable Definitions *****************************/

/*****************************************************************************/




#define TAG_MAX_CHANNEL		12
#define TAG_MAX_DI_NUM		8
#define TAG_MAX_DO_NUM		4


extern STag g_tagArray[TAG_MAX_CHANNEL];

#endif /* APPLICATION_INTERFACE_TAG_MANAGER_H_ */

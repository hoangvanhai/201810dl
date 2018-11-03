#ifndef APPLICATION_INTERFACE_FREQ_MEAS_H_
#define APPLICATION_INTERFACE_FREQ_MEAS_H_

/** @FILE NAME:    freq_meas.h
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
#include <fsl_ftm_driver.h>
#include <fsl_os_abstraction.h>
#include <fsl_debug_console.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
void FM_Init(uint32_t ftmInstance);
/************************** Variable Definitions *****************************/

/*****************************************************************************/




#endif /* APPLICATION_INTERFACE_FREQ_MEAS_H_ */

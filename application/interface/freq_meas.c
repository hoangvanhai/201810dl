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
#include <freq_meas.h>
#include <fsl_interrupt_manager.h>
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

void FM_Init(uint32_t ftmInstance) {
	ftm_user_config_t ftm_user_cfg = {
			.tofFrequency = 10,
			.syncMethod = FTM_SYNC_SWSYNC_MASK,
			.isWriteProtection = false,
			.BDMMode = kFtmBdmMode_11
	};

	ftm_dual_edge_capture_param_t capture_param_status = {
			.mode = kFtmContinuous,
			.currChanEdgeMode = kFtmRisingEdge,
			.nextChanEdgeMode = kFtmRisingEdge
	};

	FTM_DRV_Init(ftmInstance, &ftm_user_cfg);

	FTM_DRV_SetupChnDualEdgeCapture (ftmInstance, &capture_param_status, CHAN2_IDX,0);

	FTM_HAL_EnableChnInt(g_ftmBase[ftmInstance], CHAN2_IDX); //enable odd channel interrupt

	FTM_DRV_SetClock(ftmInstance,kClock_source_FTM_SystemClk, kFtmDividedBy1);

	FTM_HAL_SetDualEdgeCaptureCmd(g_ftmBase[ftmInstance], CHAN2_IDX, true); //set the  DECAPx in FTM_COMBINE
}



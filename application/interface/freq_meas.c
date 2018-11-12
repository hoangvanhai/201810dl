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
uint32_t totalCounter;
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void FM_Init(uint32_t instance) {

	ftm_user_config_t ftm_user_cfg = {
		.tofFrequency = 1,
		.syncMethod = kFtmUseHardwareTrig0,
		.isWriteProtection = false,
		.BDMMode = kFtmBdmMode_11
	};

	FTM_DRV_Init(BOARD_FTM_INSTANCE, &ftm_user_cfg);
	FTM_DRV_SetClock(BOARD_FTM_INSTANCE,kClock_source_FTM_SystemClk, kFtmDividedBy1);
	FTM_DRV_SetupChnInputCapture(BOARD_FTM_INSTANCE, kFtmRisingAndFalling, BOARD_FTM_CHANNEL, 0);
	FTM_DRV_SetTimeOverflowIntCmd(BOARD_FTM_INSTANCE, true);
	FTM_HAL_EnableChnInt(g_ftmBase[BOARD_FTM_INSTANCE], BOARD_FTM_CHANNEL);
	LREP("init ftm capture module done !\r\n");
}



/*******************************************************************************
 * Code
 ******************************************************************************/

#if (FTM_INSTANCE_COUNT > 0)
/*!
 * @brief Implementation of FTM0 handler named in startup code.
 *
 *  Passes instance to generic FTM IRQ handler.
 */
float max  = -32767.0, min = 32767.0;
float max_latch, min_latch;
uint32_t tmp = 0;
float duration =  0;

void FTM0_IRQHandler(void)
{
	OSIntEnter();
	if(FTM_HAL_HasTimerOverflowed(g_ftmBase[BOARD_FTM_INSTANCE])) {
		//FTM_HAL_ClearTimerOverflow(g_ftmBase[BOARD_FTM_INSTANCE]);
		FTM_BWR_SC_TOF(g_ftmBase[BOARD_FTM_INSTANCE], 0);
		//totalCounter += FTM_HAL_GetMod(g_ftmBase[BOARD_FTM_INSTANCE]);
		totalCounter += 0xFFFF;
	}

	if (FTM_HAL_HasChnEventOccurred(g_ftmBase[BOARD_FTM_INSTANCE], BOARD_FTM_CHANNEL))
	{
		//totalCounter += FTM_HAL_GetChnCountVal(g_ftmBase[BOARD_FTM_INSTANCE], BOARD_FTM_CHANNEL);
		totalCounter += FTM_RD_CnV_VAL(g_ftmBase[BOARD_FTM_INSTANCE], CHAN0_IDX);
		//FTM_HAL_SetCounter(g_ftmBase[BOARD_FTM_INSTANCE], 0);
		FTM_WR_CNT_COUNT(g_ftmBase[BOARD_FTM_INSTANCE], 0);
		duration = ((float)(totalCounter) / (float)FTM_DRV_GetClock(BOARD_FTM_INSTANCE)) * 1000 * 2; //???
		//duration = (float)(totalCounter) / (float)30000;
		totalCounter = 0;
#if 1
		if(max <  duration)
		{
			max = duration;
		}
		else if( min > duration)
		{
			min = duration;
		}
		tmp++;
		if (tmp >= 100)
		{
			//LREP("[Max %.3f - Min %.3f curr %.3f]\r\n", max, min, duration);
			max_latch = max;
			min_latch = min;
			tmp = 0;
			max = -32767;
			min = 32767;
		}
#endif

		//FTM_HAL_ClearChnEventStatus(g_ftmBase[BOARD_FTM_INSTANCE], BOARD_FTM_CHANNEL);
		FTM_CLR_STATUS(g_ftmBase[BOARD_FTM_INSTANCE], 1U << BOARD_FTM_CHANNEL);
	}
	OSIntExit();
    //FTM_DRV_IRQHandler(0U);
}
#endif

#if (FTM_INSTANCE_COUNT > 1)
/*!
 * @brief Implementation of FTM1 handler named in startup code.
 *
 * Passes instance to generic FTM IRQ handler.
 */
void FTM1_IRQHandler(void)
{
	debug_putchar('1');
    FTM_DRV_IRQHandler(1U);
}
#endif

#if (FTM_INSTANCE_COUNT > 2)
/*!
 * @brief Implementation of FTM2 handler named in startup code.
 *
 * Passes instance to generic FTM IRQ handler.
 */
void FTM2_IRQHandler(void)
{
	debug_putchar('2');
    FTM_DRV_IRQHandler(2U);
}
#endif

#if (FTM_INSTANCE_COUNT > 3)
/*!
 * @brief Implementation of FTM3 handler named in startup code.
 *
 * Passes instance to generic FTM IRQ handler.
 */
void FTM3_IRQHandler(void)
{
	debug_putchar('3');
    FTM_DRV_IRQHandler(3U);
}
#endif




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
uint64_t totalCounter;
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
		.syncMethod = FTM_SYNC_TRIG0_MASK,
		.isWriteProtection = false,
		.BDMMode = kFtmBdmMode_01
	};

	FTM_DRV_Init(FTM_PERIOD_MEASUARE_INSTANCE, &ftm_user_cfg);
	FTM_DRV_SetClock(FTM_PERIOD_MEASUARE_INSTANCE,kClock_source_FTM_SystemClk, kFtmDividedBy1);
	FTM_DRV_SetupChnInputCapture(FTM_PERIOD_MEASUARE_INSTANCE, kFtmRisingAndFalling, CHAN0_IDX, 0);
	FTM_HAL_EnableChnInt(g_ftmBase[FTM_PERIOD_MEASUARE_INSTANCE], CHAN0_IDX);
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
void FTM0_IRQHandler(void)
{
	//debug_putchar('0');

	if(FTM_HAL_HasTimerOverflowed(g_ftmBase[FTM_PERIOD_MEASUARE_INSTANCE])) {
		//LREP(" [O] ");
		FTM_HAL_ClearTimerOverflow(g_ftmBase[FTM_PERIOD_MEASUARE_INSTANCE]);
		totalCounter += 0xFFFF;
	}

	if (FTM_HAL_HasChnEventOccurred(g_ftmBase[FTM_PERIOD_MEASUARE_INSTANCE], CHAN0_IDX))
	{
		FTM_HAL_ClearChnEventStatus(g_ftmBase[FTM_PERIOD_MEASUARE_INSTANCE], CHAN0_IDX);
		totalCounter += FTM_HAL_GetChnCountVal(g_ftmBase[FTM_PERIOD_MEASUARE_INSTANCE], CHAN0_IDX);
		LREP("%ld ", totalCounter);
		totalCounter = 0;
		FTM_HAL_SetCounter(g_ftmBase[FTM_PERIOD_MEASUARE_INSTANCE], 0);
	}

	//CaptureTime_ms = (CaptureCounts * 100) / FTM_DRV_GetClock(VALVE_FTM_UNIT);

    FTM_DRV_IRQHandler(0U);
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




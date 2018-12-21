/*
 * Copyright (c) 2013-2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "board.h"
#include "pin_mux.h"
#include "fsl_clock_manager.h"
#include "fsl_debug_console.h"
#include <includes.h>

void hardware_init(void) {

	/* enable clock for PORTs */
	CLOCK_SYS_EnablePortClock(PORTA_IDX);
	CLOCK_SYS_EnablePortClock(PORTB_IDX);
	CLOCK_SYS_EnablePortClock(PORTC_IDX);
	CLOCK_SYS_EnablePortClock(PORTD_IDX);
	CLOCK_SYS_EnablePortClock(PORTE_IDX);

	//CLOCK_SYS_EnableSdhcClock(BOARD_SDHC_INSTANCE);

	CLOCK_SYS_EnableEnetClock(BOARD_ENET_INSTANCE);
	CLOCK_SYS_SetEnetTimeStampSrc(0, kClockTimeSrcOsc0erClk);
	configure_enet_pins(BOARD_ENET_INSTANCE);
//	configure_sdhc_pins(BOARD_SDHC_INSTANCE);
	configure_i2c_pins(BOARD_I2C_RTC_INSTANCE);

	/* Init board clock */
	BOARD_ClockInit();
	dbg_uart_init();

//#ifdef MPU_INSTANCE_COUNT /* File System need disabled MPU */
//    // disable MPU
//    for(int i = 0; i < MPU_INSTANCE_COUNT; i++)
//    {
//        MPU_HAL_Disable(g_mpuBase[i]);
//    }
//#endif

	// Configure the power mode protection
	SMC_HAL_SetProtection(SMC_BASE_PTR, kAllowPowerModeVlp);

//	GPIO_DRV_Init(sdhcCdPin, NULL);
//
//    GPIO_DRV_Init(DigitalInputPin, NULL);
    GPIO_DRV_Init(NULL, DigitalOutputPin);



//	BOARD_EnableAllFault();
//
//	BOARD_CreateWDG();


	GPIO_DRV_ClearPinOutput(kGpioMbRs485);
	GPIO_DRV_ClearPinOutput(DigitalOutput0);
	GPIO_DRV_ClearPinOutput(DigitalOutput1);
	GPIO_DRV_ClearPinOutput(DigitalOutput2);
	GPIO_DRV_ClearPinOutput(DigitalOutput3);
	GPIO_DRV_ClearPinOutput(DigitalOutput4);
	GPIO_DRV_ClearPinOutput(DigitalOutput5);

	GPIO_DRV_ClearPinOutput(SelectAnalog0);
	GPIO_DRV_ClearPinOutput(SelectAnalog1);
	GPIO_DRV_ClearPinOutput(SelectAnalog2);
	GPIO_DRV_ClearPinOutput(SelectAnalog3);

	GPIO_DRV_ClearPinOutput(SelectTrigger0);
	GPIO_DRV_ClearPinOutput(SelectTrigger1);
	GPIO_DRV_ClearPinOutput(SelectTrigger2);
	GPIO_DRV_ClearPinOutput(SelectTrigger3);

	GPIO_DRV_ClearPinOutput(WatchDogFeed);
	GPIO_DRV_ClearPinOutput(LcdVccOcf);
	GPIO_DRV_ClearPinOutput(LcdVccEn);
	GPIO_DRV_ClearPinOutput(LcdGpio1);
	GPIO_DRV_ClearPinOutput(LcdGpio2);
	GPIO_DRV_ClearPinOutput(LanPsuOcp);
	GPIO_DRV_ClearPinOutput(LanPsuEn);
	GPIO_DRV_ClearPinOutput(IoVccEn);
	GPIO_DRV_ClearPinOutput(IoVccOcf);
	GPIO_DRV_ClearPinOutput(ModbusPsuOcp);
	GPIO_DRV_ClearPinOutput(ModbusPsuEn);
}

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.4 [05.10]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/

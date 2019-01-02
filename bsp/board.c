/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
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
#include "fsl_clock_manager.h"
#include "fsl_smc_hal.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "fsl_wdog_driver.h"
#include "fsl_rnga_driver.h"
#include "fsl_rcm_hal.h"
#include "fsl_lpuart_driver.h"
#include "app.h"
extern void external_card_detection(void);

#if BOARD_USE_VERSION == BOARD_FRDM_K64F
/* Configuration for enter RUN mode. Core clock = 120MHz. */
const clock_manager_user_config_t g_defaultClockConfigRun =
{
    .mcgConfig =
    {
        .mcg_mode           = kMcgModePEE,   // Work in PEE mode.
        .irclkEnable        = true,  // MCGIRCLK enable.
        .irclkEnableInStop  = false, // MCGIRCLK disable in STOP mode.
        .ircs               = kMcgIrcSlow, // Select IRC32k.
        .fcrdiv             = 0U,    // FCRDIV is 0.

        .frdiv   = 7U,
        .drs     = kMcgDcoRangeSelLow,  // Low frequency range
        .dmx32   = kMcgDmx32Default,    // DCO has a default range of 25%
        .oscsel  = kMcgOscselOsc,       // Select OSC

        .pll0EnableInFllMode = false,  // PLL0 disable
        .pll0EnableInStop    = false,  // PLL0 disalbe in STOP mode
        .prdiv0              = 0x13U,
        .vdiv0               = 0x18U,
    },

    .simConfig =
    {
        .pllFllSel = kClockPllFllSelPll,    // PLLFLLSEL select PLL.
        .er32kSrc  = kClockEr32kSrcRtc,     // ERCLK32K selection, use RTC.
        .outdiv1   = 0U,
        .outdiv2   = 1U,
        .outdiv3   = 1U,
        .outdiv4   = 4U,
    },
    .oscerConfig =
    {
        .enable       = true,  // OSCERCLK enable.
        .enableInStop = false, // OSCERCLK disable in STOP mode.
    }
};

osc_user_config_t osc_user_config_t osc0Config =
{
    .freq                = OSC0_XTAL_FREQ,
    .hgo                 = MCG_HGO0,
    .range               = MCG_RANGE0,
    .erefs               = MCG_EREFS0,
    .enableCapacitor2p   = OSC0_SC2P_ENABLE_CONFIG,
    .enableCapacitor4p   = OSC0_SC4P_ENABLE_CONFIG,
    .enableCapacitor8p   = OSC0_SC8P_ENABLE_CONFIG,
    .enableCapacitor16p  = OSC0_SC16P_ENABLE_CONFIG,
};

#elif BOARD_USE_VERSION == BOARD_VERSION_1

/* Configuration for enter RUN mode. Core clock = 180MHz. */
const clock_manager_user_config_t g_defaultClockConfigRun =
{
    .mcgConfig =
    {
        .mcg_mode           = kMcgModePEE,   // Work in PEE mode.
        .irclkEnable        =  true,  		 // MCGIRCLK enable.
        .irclkEnableInStop  = false, 		 // MCGIRCLK disable in STOP mode.
        .ircs               = kMcgIrcSlow, 	 // Select IRC32k.
        .fcrdiv             = 0U,    		 // FCRDIV is 0.

        .frdiv   = 0U,
        .drs     = kMcgDcoRangeSelLow,  // Low frequency range
        .dmx32   = kMcgDmx32Default,    // DCO has a default range of 25%
        .oscsel  = kMcgOscselOsc,       // Select OSC

        .pll0EnableInFllMode = false,  // PLL0 disable
        .pll0EnableInStop    = false,  // PLL0 disalbe in STOP mode
        .prdiv0              = 0x0U,
        .vdiv0               = 0x1DU,
		.pllcs				 = kMcgPllClkSelPll0,
    },

    .simConfig =
    {
        .pllFllSel = kClockPllFllSelPll,    // PLLFLLSEL select PLL.
        .er32kSrc  = kClockEr32kSrcOsc0,        // ERCLK32K selection, use RTC.
        .outdiv1   = 0U,
        .outdiv2   = 1U,
        .outdiv3   = 1U,
        .outdiv4   = 6U,
    },
    .oscerConfig =
    {
        .enable       = true,  // OSCERCLK enable.
        .enableInStop = true, // OSCERCLK disable in STOP mode.
    }
};

osc_user_config_t osc0Config =
{
    .freq                = OSC0_XTAL_FREQ,
    .hgo                 = kOscGainLow,
    .range               = kOscRangeVeryHigh,
    .erefs               = kOscSrcOsc,
    .enableCapacitor2p   = false,
    .enableCapacitor4p   = false,
    .enableCapacitor8p   = false,
    .enableCapacitor16p  = true,
};

#else
#error MUST DEFINE VALID HW VERSSION
#endif
/* Function to initialize OSC0 base on board configuration. */
void BOARD_InitOsc0(void)
{
    CLOCK_SYS_OscInit(0U, &osc0Config);
}


static void CLOCK_SetBootConfig(clock_manager_user_config_t const* config)
{
    CLOCK_SYS_SetSimConfigration(&config->simConfig);
    CLOCK_SYS_SetOscerConfigration(0, &config->oscerConfig);
    CLOCK_SYS_BootToPee(&config->mcgConfig);
    SystemCoreClock = CORE_CLOCK_FREQ;
}

/* Initialize clock. */
void BOARD_ClockInit(void)
{
    PORT_HAL_SetMuxMode(EXTAL0_PORT, EXTAL0_PIN, EXTAL0_PINMUX);
    PORT_HAL_SetMuxMode(XTAL0_PORT, XTAL0_PIN, XTAL0_PINMUX);
    BOARD_InitOsc0();
    CLOCK_SetBootConfig(&g_defaultClockConfigRun);
}

/* The function to indicate whether a card is detected or not */
bool BOARD_IsExtSDCardDetected(void)
{
    GPIO_Type * gpioBase = g_gpioBase[GPIO_EXTRACT_PORT(kGpioExtCd)];
    uint32_t pin = GPIO_EXTRACT_PIN(kGpioExtCd);

    if(GPIO_HAL_ReadPinInput(gpioBase, pin) == false)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool BOARD_IsIntSDCardDetected(void)
{
    GPIO_Type * gpioBase = g_gpioBase[GPIO_EXTRACT_PORT(kGpioIntCd)];
    uint32_t pin = GPIO_EXTRACT_PIN(kGpioIntCd);

    if(GPIO_HAL_ReadPinInput(gpioBase, pin) == false)
    {
        return true;
    }
    else
    {
        return false;
    }
}


/* Initialize debug console. */
void dbg_uart_init(void)
{
    configure_uart_pins(BOARD_DEBUG_UART_INSTANCE);

    DbgConsole_Init(BOARD_DEBUG_UART_INSTANCE, BOARD_DEBUG_UART_BAUD, kDebugConsoleUART);
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void HardFault_Handler(void) {
	LREP("SCB->SHCSR = 0x%x SCB->CFSR = 0x%x\r\n", SCB->SHCSR, SCB->CFSR);
	LREP("HARD FAULT ADDRESS 0x%x\r\n", SCB->HFSR);
	BOARD_GetFaultType();
	for(;;);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void BusFault_Handler(void) {
	LREP("SCB->SHCSR = 0x%x SCB->CFSR = 0x%x\r\n", SCB->SHCSR, SCB->CFSR);
	LREP("BUS FAULT ADDRESS: 0x%x\r\n", SCB->BFAR);
	BOARD_GetFaultType();
	for(;;);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void UsageFault_Handler(void) {

}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void MemManage_Handler(void) {
	LREP("SCB->SHCSR = 0x%x SCB->CFSR = 0x%x\r\n", SCB->SHCSR, SCB->CFSR);
	LREP("MEMMAN FAULT ADDRESS 0x%x\r\n", SCB->MMFAR);
	BOARD_GetFaultType();
	for(;;);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void BOARD_EnableAllFault(void) {

	SCB->SHCSR |= 	SCB_SHCSR_USGFAULTENA_Msk |
					SCB_SHCSR_BUSFAULTENA_Msk |
					SCB_SHCSR_MEMFAULTENA_Msk;
}


void BOARD_DisableAllFault(void) {

	SCB->SHCSR &= 	(~SCB_SHCSR_USGFAULTENA_Msk) |
					(~SCB_SHCSR_BUSFAULTENA_Msk) |
					(~SCB_SHCSR_MEMFAULTENA_Msk);
}

int BOARD_GetFaultType(void) {

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_DIVBYZERO) {
		LREP("CPU_REG_NVIC_CFSR_DIVBYZERO\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_UNALIGNED) {
		LREP("CPU_REG_NVIC_CFSR_UNALIGNED\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_NOCP) {
		LREP("CPU_REG_NVIC_CFSR_NOCP\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_INVPC) {
		LREP("CPU_REG_NVIC_CFSR_INVPC\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_INVSTATE) {
		LREP("CPU_REG_NVIC_CFSR_INVSTATE\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_UNDEFINSTR) {
		LREP("CPU_REG_NVIC_CFSR_UNDEFINSTR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_BFARVALID) {
		LREP("CPU_REG_NVIC_CFSR_BFARVALID\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_STKERR) {
		LREP("CPU_REG_NVIC_CFSR_STKERR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_UNSTKERR) {
		LREP("CPU_REG_NVIC_CFSR_UNSTKERR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_IMPRECISERR) {
		LREP("CPU_REG_NVIC_CFSR_IMPRECISERR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_PRECISERR) {
		LREP("CPU_REG_NVIC_CFSR_PRECISERR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_IBUSERR) {
		LREP("CPU_REG_NVIC_CFSR_IBUSERR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_MMARVALID) {
		LREP("CPU_REG_NVIC_CFSR_MMARVALID\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_MSTKERR) {
		LREP("CPU_REG_NVIC_CFSR_MSTKERR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_MUNSTKERR) {
		LREP("CPU_REG_NVIC_CFSR_MUNSTKERR\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_DACCVIOL) {
		LREP("CPU_REG_NVIC_CFSR_DACCVIOL\r\n\r\n");
	}

	if(SCB->CFSR & CPU_REG_NVIC_CFSR_IACCVIOL) {
		LREP("CPU_REG_NVIC_CFSR_IACCVIOL\r\n\r\n");
	}

	return SCB->CFSR;
}


void BOARD_CreateWDG(void) {
    const wdog_config_t wdogConfig =
    {
        .wdogEnable             = true,// Watchdog mode
        .timeoutValue          	= 4096U,// Watchdog overflow time is about 4s
        .winEnable             	= false, //Disable window function
        .windowValue           	= 0,    // Watchdog window value
        .prescaler   			= kWdogClkPrescalerDivide1, // Watchdog clock prescaler
        .updateEnable  			= true, // Update register enabled
        .clkSrc           		= kWdogLpoClkSrc, // Watchdog clock source is LPO 1KHz
#if FSL_FEATURE_WDOG_HAS_WAITEN
        .workMode.kWdogEnableInWaitMode  = true, // Enable watchdog in wait mode
#endif
        .workMode.kWdogEnableInStopMode  = true, // Enable watchdog in stop mode
        .workMode.kWdogEnableInDebugMode = false,// Disable watchdog in debug mode
    };


    WDOG_DRV_Init(&wdogConfig);

}


int BOARD_GenerateRandom(int min, int max) {
	uint32_t randout;
	RNGA_DRV_GetRandomData(0, &randout, sizeof(uint32_t));
	randout = abs(randout);
	return ((randout % (max - min + 1)) + min);
}


void BOARD_CheckResetCause(void) {

	uint32_t resetCause = RCM_HAL_GetSrcStatus(RCM, kRcmSrcAll);
	if(resetCause & kRcmWakeup) {
		LREP("kRcmWakeup\r\n");
	}
	if(resetCause & kRcmLowVoltDetect) {
		LREP("kRcmLowVoltDetect\r\n");
	}
	if(resetCause & kRcmLossOfClk) {
		LREP("kRcmLossOfClk\r\n");
	}
	if(resetCause & kRcmLossOfLock) {
		LREP("kRcmLossOfLock\r\n");
	}
	if(resetCause & kRcmWatchDog) {
		LREP("kRcmWatchDog\r\n");
	}
	if(resetCause & kRcmExternalPin) {
		LREP("kRcmExternalPin\r\n");
	}
	if(resetCause & kRcmPowerOn) {
		LREP("kRcmPowerOn\r\n");
	}
	if(resetCause & kRcmJtag) {
		LREP("kRcmJtag\r\n");
	}
	if(resetCause & kRcmCoreLockup) {
		LREP("kRcmCoreLockup\r\n");
	}
	if(resetCause & kRcmSoftware) {
		LREP("kRcmSoftware\r\n");
	}
	if(resetCause & kRcmEzport) {
		LREP("kRcmEzport\r\n");
	}
	if(resetCause & kRcmStopModeAckErr) {
		LREP("kRcmStopModeAckErr\r\n");
	}
}


void BOARD_GpioWritePin(uint32_t pinName, bool value) {
	if(value) {
		GPIO_DRV_SetPinOutput(pinName);
	} else {
		GPIO_DRV_ClearPinOutput(pinName);
	}
}


void BOARD_CheckPeripheralFault() {
	if(!GPIO_DRV_ReadPinInput(LcdVccOcf)) {
		//ERR("LCD POWER FAULT \r\n");
	}

	if(!GPIO_DRV_ReadPinInput(LanPsuOcp)) {
		//ERR("ETHERNET POWER FAULT \r\n");
	}

	if(!GPIO_DRV_ReadPinInput(IoVccOcf)) {
		//ERR("IO POWER FAULT \r\n");
	}

	if(!GPIO_DRV_ReadPinInput(ModbusPsuOcp)) {
		//ERR("MODBUS POWER FAULT \r\n");
	}

	if(!GPIO_DRV_ReadPinInput(SimVccOcf)) {
		//ERR("WIRELESS POWER FAULT \r\n");
	}

	int status = BOARD_IsExtSDCardDetected();
	if(status != pAppObj->eStatus.Bits.bExtCD) {
		external_card_detection();
	}
}


void DefaultISR(void) {
	LREP("JUM TO DefaultISR \r\n");
}
/*******************************************************************************
 * EOF
 ******************************************************************************/



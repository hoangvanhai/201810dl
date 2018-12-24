/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : gpio_pins.c
**     Project     : TWR-K64F120M
**     Processor   : MK64FN1M0VMD12
**     Component   : fsl_gpio
**     Version     : Component 1.3.0, Driver 01.00, CPU db: 3.00.000
**     Repository  : KSDK 1.3.0
**     Compiler    : GNU C Compiler
**
**     Copyright : 1997 - 2015 Freescale Semiconductor, Inc.
**     All Rights Reserved.
**
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file gpio_pins.c
** @version 01.00
*/
/*!
**  @addtogroup gpio_pins_module gpio_pins module documentation
**  @{
*/

/* MODULE gpio_pins. */

#include "gpio_pins.h"
#include <stdbool.h>

#if 1

const gpio_input_pin_user_config_t sdhcCdPin[] = {
  {
    .pinName = kGpioSdhc0Cd,
    .config.isPullEnable = true,
    .config.pullSelect = kPortPullDown,
    .config.isPassiveFilterEnabled = false,
    .config.interrupt = kPortIntEitherEdge
  },
  {
    .pinName = GPIO_PINS_OUT_OF_RANGE,
  }
};



const gpio_output_pin_user_config_t DigitalOutputPin[] = {
	{
		.pinName = DigitalOutput0,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = DigitalOutput1,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = DigitalOutput2,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = DigitalOutput3,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = DigitalOutput4,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = DigitalOutput5,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
    {
		.pinName = kGpioMbRs485,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
    },
	{
		.pinName = TriggerAnalog,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = RefCurrEn,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = RefCurrEf,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = EpromWp,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = WatchDogFeed,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = LcdVccOcf,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = LcdVccEn,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = LcdGpio1,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = LcdGpio2,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = LanPsuOcp,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = IoVccEn,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = IoVccOcf,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = ModbusPsuOcp,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = ModbusPsuEn,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SimVccEn,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = GPIO_PINS_OUT_OF_RANGE,
	}
};


const gpio_output_pin_user_config_t SelectAnalogPin[] = {
	{
		.pinName = SelectAnalog0,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SelectAnalog1,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SelectAnalog2,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SelectAnalog3,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SelectTrigger0,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SelectTrigger1,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SelectTrigger2,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = SelectTrigger3,
		.config.outputLogic = 1,
		.config.slewRate = kPortSlowSlewRate,
		.config.isOpenDrainEnabled = false,
		.config.driveStrength = kPortLowDriveStrength,
	},
	{
		.pinName = GPIO_PINS_OUT_OF_RANGE,
	}
};

const gpio_input_pin_user_config_t DigitalInputPin[] = {
	{
		.pinName = DigitalInput0,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = DigitalInput1,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = DigitalInput2,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = DigitalInput3,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = DigitalInput4,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = DigitalInput5,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = DigitalInput6,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = DigitalInput7,
		.config.isPullEnable = true,
		.config.pullSelect = kPortPullUp,
		.config.isPassiveFilterEnabled = false,
		.config.interrupt = kPortIntDisabled
	},
	{
		.pinName = GPIO_PINS_OUT_OF_RANGE,
	}
};

#endif

/* END gpio_pins. */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/


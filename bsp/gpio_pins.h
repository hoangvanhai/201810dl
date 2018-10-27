/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : gpio_pins.h
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
** @file gpio_pins.h
** @version 01.00
*/
/*!
**  @addtogroup gpio_pins_module gpio_pins module documentation
**  @{
*/

#ifndef __gpio_pins_H
#define __gpio_pins_H

/* MODULE gpio_pins. */

/* Include inherited beans */
#include "fsl_gpio_driver.h"


/*! @brief Configuration structure 0 for input pins */
extern gpio_input_pin_user_config_t switchPins[];

/*! @brief Configuration structure 3 for input pins */
extern const gpio_input_pin_user_config_t sdhcCdPin[];

/*! @brief Configuration structure 0 for output pins */
extern const gpio_output_pin_user_config_t ledPins[];

extern const gpio_output_pin_user_config_t mbRs485Pin[];


///*! @brief Pin names */
//enum _gpio_pins_pinNames{
//  kGpioSW1                    = GPIO_MAKE_PIN(GPIOC_IDX, 6U),
//  kGpioSW3                    = GPIO_MAKE_PIN(GPIOA_IDX, 4U),
//  kGpioSdhc0Cd                = GPIO_MAKE_PIN(GPIOB_IDX, 20U),
//  kGpioLED1                   = GPIO_MAKE_PIN(GPIOE_IDX, 6U),
//  kGpioLED2                   = GPIO_MAKE_PIN(GPIOE_IDX, 7U),
//  kGpioLED3                   = GPIO_MAKE_PIN(GPIOE_IDX, 8U),
//  kGpioLED4                   = GPIO_MAKE_PIN(GPIOE_IDX, 9U),
//  kGpioSdcardCardDetection    = GPIO_MAKE_PIN(PORTD_IDX, 15U),  /* Connects the TWR_MEM, this's SPI Card detector pin */
//};


/*! @brief Pin names */
enum _gpio_pins_pinNames{
  kGpioSW1                    = GPIO_MAKE_PIN(GPIOC_IDX, 6U),
  kGpioSW3                    = GPIO_MAKE_PIN(GPIOA_IDX, 4U),
  kGpioSdhc0Cd                = GPIO_MAKE_PIN(GPIOE_IDX, 6U),
  kGpioLEDRED                 = GPIO_MAKE_PIN(GPIOB_IDX, 22U),
  kGpioLEDGREEN               = GPIO_MAKE_PIN(GPIOE_IDX, 26U),
  kGpioLEDBLUE                = GPIO_MAKE_PIN(GPIOB_IDX, 21U),
  kGpioSdcardCardDetection    = GPIO_MAKE_PIN(PORTD_IDX, 15U),  /* Connects the TWR_MEM, this's SPI Card detector pin */
  kGpioMbRs485                = GPIO_MAKE_PIN(GPIOE_IDX, 10U),
};

extern gpio_input_pin_user_config_t sdcardCardDectionPin[];

#endif

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



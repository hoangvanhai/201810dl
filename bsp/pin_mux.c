/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : pin_mux.c
**     Project     : TWR-K64F120M
**     Processor   : MK64FN1M0VMD12
**     Component   : PinSettings
**     Version     : Component 1.2.0, Driver 1.4, CPU db: 3.00.000
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
** @file pin_mux.c
** @version 1.4
** @brief
**
*/
/*!
**  @addtogroup pin_mux_module pin_mux module documentation
**  @{
*/

/* MODULE pin_mux. */

#include "fsl_device_registers.h"
#include "fsl_port_hal.h"
#include "fsl_sim_hal.h"
#include "pin_mux.h"

void configure_can_pins(uint32_t instance)
{
  switch(instance) {
    case CAN0_IDX:                       /* CAN0 */
      /* Affects PORTB_PCR19 register */
      PORT_HAL_SetMuxMode(PORTB,19u,kPortMuxAlt2);
      /* Affects PORTB_PCR18 register */
      PORT_HAL_SetMuxMode(PORTB,18u,kPortMuxAlt2);
      break;
    default:
      break;
  }
}

void configure_enet_pins(uint32_t instance)
{
  /* Affects PORTC_PCR16 register */
  PORT_HAL_SetMuxMode(PORTC,16u,kPortMuxAlt4);
  /* Affects PORTC_PCR17 register */
  PORT_HAL_SetMuxMode(PORTC,17u,kPortMuxAlt4);
  /* Affects PORTC_PCR18 register */
  PORT_HAL_SetMuxMode(PORTC,18u,kPortMuxAlt4);
  /* Affects PORTC_PCR19 register */
  PORT_HAL_SetMuxMode(PORTC,19u,kPortMuxAlt4);
  /* Affects PORTA_PCR29 register */
  PORT_HAL_SetMuxMode(PORTA,29u,kPortMuxAlt4);
  /* Affects PORTA_PCR27 register */
  PORT_HAL_SetMuxMode(PORTA,27u,kPortMuxAlt4);
  /* Affects PORTB_PCR1 register */
  PORT_HAL_SetMuxMode(PORTB,1u,kPortMuxAlt4);
  /* Affects PORTB_PCR0 register */
  PORT_HAL_SetMuxMode(PORTB,0u,kPortMuxAlt4);
  PORT_HAL_SetOpenDrainCmd(PORTB,0u,true);
  /* Affects PORTA_PCR11 register */
  PORT_HAL_SetMuxMode(PORTA,11u,kPortMuxAlt4);
  /* Affects PORTA_PCR13 register */
  PORT_HAL_SetMuxMode(PORTA,13u,kPortMuxAlt4);
  /* Affects PORTA_PCR12 register */
  PORT_HAL_SetMuxMode(PORTA,12u,kPortMuxAlt4);
  /* Affects PORTA_PCR10 register */
  PORT_HAL_SetMuxMode(PORTA,10u,kPortMuxAlt4);
  /* Affects PORTA_PCR9 register */
  PORT_HAL_SetMuxMode(PORTA,9u,kPortMuxAlt4);
  /* Affects PORTA_PCR14 register */
  PORT_HAL_SetMuxMode(PORTA,14u,kPortMuxAlt4);
  /* Affects PORTA_PCR5 register */
  PORT_HAL_SetMuxMode(PORTA,5u,kPortMuxAlt4);
  /* Affects PORTA_PCR25 register */
  PORT_HAL_SetMuxMode(PORTA,25u,kPortMuxAlt4);
  /* Affects PORTA_PCR16 register */
  PORT_HAL_SetMuxMode(PORTA,16u,kPortMuxAlt4);
  /* Affects PORTA_PCR17 register */
  PORT_HAL_SetMuxMode(PORTA,17u,kPortMuxAlt4);
  /* Affects PORTA_PCR24 register */
  PORT_HAL_SetMuxMode(PORTA,24u,kPortMuxAlt4);
  /* Affects PORTA_PCR26 register */
  PORT_HAL_SetMuxMode(PORTA,26u,kPortMuxAlt4);
  /* Affects PORTA_PCR15 register */
  PORT_HAL_SetMuxMode(PORTA,15u,kPortMuxAlt4);
  /* Affects PORTA_PCR28 register */
  PORT_HAL_SetMuxMode(PORTA,28u,kPortMuxAlt4);
}

void configure_ftm_pins(uint32_t instance)
{
  switch(instance) {
    case FTM3_IDX:                       /* FTM3 */
      /* Affects PORTE_PCR6 register */
      PORT_HAL_SetDriveStrengthMode(PORTE,6u,kPortLowDriveStrength);
      PORT_HAL_SetMuxMode(PORTE,6u,kPortMuxAlt6);
      PORT_HAL_SetSlewRateMode(PORTE,6u,kPortSlowSlewRate);
      break;
    default:
      break;
  }
}

void configure_gpio_pins(uint32_t instance)
{
  switch(instance) {
    case PORTA_IDX:                      /* PORTA_IDX */
      /* Affects PORTA_PCR4 register */
      PORT_HAL_SetPassiveFilterCmd(PORTA,4u,false);
      PORT_HAL_SetMuxMode(PORTA,4u,kPortMuxAsGpio);
      PORT_HAL_SetPullMode(PORTA,4u,kPortPullUp);
      PORT_HAL_SetPullCmd(PORTA,4u,true);
      /* Affects PORTA_PCR6 register */
      PORT_HAL_SetMuxMode(PORTA,6u,kPortMuxAsGpio);
      /* Affects PORTA_PCR8 register */
      PORT_HAL_SetMuxMode(PORTA,8u,kPortMuxAsGpio);
      break;
    case PORTB_IDX:                      /* PORTB_IDX */
      /* Affects PORTB_PCR6 register */
      PORT_HAL_SetMuxMode(PORTB,6u,kPortMuxAsGpio);
      /* Affects PORTB_PCR8 register */
      PORT_HAL_SetMuxMode(PORTB,8u,kPortMuxAsGpio);
      /* Affects PORTB_PCR20 register */
      PORT_HAL_SetMuxMode(PORTB,20u,kPortMuxAsGpio);
      /* Affects PORTB_PCR21 register */
      PORT_HAL_SetMuxMode(PORTB,21u,kPortMuxAsGpio);
      break;
    case PORTC_IDX:                      /* PORTC_IDX */
      /* Affects PORTC_PCR6 register */
      PORT_HAL_SetPassiveFilterCmd(PORTC,6u,false);
      PORT_HAL_SetMuxMode(PORTC,6u,kPortMuxAsGpio);
      PORT_HAL_SetPullMode(PORTC,6u,kPortPullUp);
      PORT_HAL_SetPullCmd(PORTC,6u,true);
      /* Affects PORTC_PCR8 register */
      PORT_HAL_SetMuxMode(PORTC,8u,kPortMuxAsGpio);
      /* Affects PORTC_PCR9 register */
      PORT_HAL_SetMuxMode(PORTC,9u,kPortMuxAsGpio);
      break;
    case PORTD_IDX:                      /* PORTD_IDX */
     /* Affects PORTD_PCR14 register */
      PORT_HAL_SetMuxMode(PORTD,14u,kPortMuxAsGpio);
      break;
    case PORTE_IDX:                      /* PORTE_IDX */
      /* Affects PORTE_PCR6 register */
      PORT_HAL_SetDriveStrengthMode(PORTE,6u,kPortLowDriveStrength);
      PORT_HAL_SetMuxMode(PORTE,6u,kPortMuxAsGpio);
      PORT_HAL_SetSlewRateMode(PORTE,6u,kPortSlowSlewRate);
      /* Affects PORTE_PCR7 register */
      PORT_HAL_SetDriveStrengthMode(PORTE,7u,kPortLowDriveStrength);
      PORT_HAL_SetMuxMode(PORTE,7u,kPortMuxAsGpio);
      PORT_HAL_SetSlewRateMode(PORTE,7u,kPortSlowSlewRate);
      /* Affects PORTE_PCR8 register */
      PORT_HAL_SetDriveStrengthMode(PORTE,8u,kPortLowDriveStrength);
      PORT_HAL_SetMuxMode(PORTE,8u,kPortMuxAsGpio);
      PORT_HAL_SetSlewRateMode(PORTE,8u,kPortSlowSlewRate);
      /* Affects PORTE_PCR9 register */
      PORT_HAL_SetDriveStrengthMode(PORTE,9u,kPortLowDriveStrength);
      PORT_HAL_SetMuxMode(PORTE,9u,kPortMuxAsGpio);
      PORT_HAL_SetSlewRateMode(PORTE,9u,kPortSlowSlewRate);
      /* Affects PORTE_PCR12 register */
      PORT_HAL_SetMuxMode(PORTE,12u,kPortMuxAsGpio);
      break;
    default:
      break;
  }
}

void configure_i2c_pins(uint32_t instance)
{
  switch(instance) {
    case I2C0_IDX:                       /* I2C0 */
      /* Affects PORTD_PCR8 register */
      PORT_HAL_SetMuxMode(PORTD,8u,kPortMuxAlt2);
      PORT_HAL_SetOpenDrainCmd(PORTD,8u,true);
      /* Affects PORTD_PCR9 register */
      PORT_HAL_SetMuxMode(PORTD,9u,kPortMuxAlt2);
      PORT_HAL_SetOpenDrainCmd(PORTD,9u,true);
      break;
    case I2C1_IDX:                       /* I2C1 */
      /* Affects PORTC_PCR10 register */
      PORT_HAL_SetMuxMode(PORTC,10u,kPortMuxAlt2);
      PORT_HAL_SetOpenDrainCmd(PORTC,10u,true);
      /* Affects PORTC_PCR11 register */
      PORT_HAL_SetMuxMode(PORTC,11u,kPortMuxAlt2);
      PORT_HAL_SetOpenDrainCmd(PORTC,11u,true);
      break;
    default:
      break;
  }
}

void configure_i2s_pins(uint32_t instance)
{
  /* Affects PORTC_PCR6 register */
  PORT_HAL_SetMuxMode(PORTC,6u,kPortMuxAlt6);
  /* Affects PORTC_PCR5 register */
  PORT_HAL_SetMuxMode(PORTC,5u,kPortMuxAlt4);
  /* Affects PORTB_PCR18 register */
  PORT_HAL_SetMuxMode(PORTB,18u,kPortMuxAlt4);
  /* Affects PORTB_PCR19 register */
  PORT_HAL_SetMuxMode(PORTB,19u,kPortMuxAlt4);
  /* Affects PORTC_PCR1 register */
  PORT_HAL_SetMuxMode(PORTC,1u,kPortMuxAlt6);
}

void configure_rtc_pins(uint32_t instance)
{
  /* Affects PORTE_PCR26 register */
  PORT_HAL_SetMuxMode(PORTE,26u,kPortMuxAlt6);
}

void configure_sdhc_pins(uint32_t instance)
{
  /* Affects PORTE_PCR3 register */
  PORT_HAL_SetMuxMode(PORTE,3u,kPortMuxAlt4);
  PORT_HAL_SetPullMode(PORTE,3u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTE,3u,true);
  PORT_HAL_SetDriveStrengthMode(PORTE,3u,kPortHighDriveStrength);
  /* Affects PORTE_PCR1 register */
  PORT_HAL_SetMuxMode(PORTE,1u,kPortMuxAlt4);
  PORT_HAL_SetPullMode(PORTE,1u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTE,1u,true);
  PORT_HAL_SetDriveStrengthMode(PORTE,1u,kPortHighDriveStrength);
  /* Affects PORTE_PCR0 register */
  PORT_HAL_SetMuxMode(PORTE,0u,kPortMuxAlt4);
  PORT_HAL_SetPullMode(PORTE,0u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTE,0u,true);
  PORT_HAL_SetDriveStrengthMode(PORTE,0u,kPortHighDriveStrength);
  /* Affects PORTE_PCR5 register */
  PORT_HAL_SetMuxMode(PORTE,5u,kPortMuxAlt4);
  PORT_HAL_SetPullMode(PORTE,5u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTE,5u,true);
  PORT_HAL_SetDriveStrengthMode(PORTE,5u,kPortHighDriveStrength);
  /* Affects PORTE_PCR4 register */
  PORT_HAL_SetMuxMode(PORTE,4u,kPortMuxAlt4);
  PORT_HAL_SetPullMode(PORTE,4u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTE,4u,true);
  PORT_HAL_SetDriveStrengthMode(PORTE,4u,kPortHighDriveStrength);
  /* Affects PORTE_PCR2 register */
  PORT_HAL_SetMuxMode(PORTE,2u,kPortMuxAlt4);
  PORT_HAL_SetPullMode(PORTE,2u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTE,2u,true);
  PORT_HAL_SetDriveStrengthMode(PORTE,2u,kPortHighDriveStrength);
}

void configure_spi_pins(uint32_t instance)
{
  switch(instance) {
    case SPI0_IDX:                       /* SPI0 */
      /* Affects PORTD_PCR0 register */
      PORT_HAL_SetMuxMode(PORTD,0u,kPortMuxAlt2);
      /* Affects PORTD_PCR3 register */
      PORT_HAL_SetMuxMode(PORTD,3u,kPortMuxAlt2);
      /* Affects PORTD_PCR1 register */
      PORT_HAL_SetMuxMode(PORTD,1u,kPortMuxAlt2);
      /* Affects PORTD_PCR2 register */
      PORT_HAL_SetMuxMode(PORTD,2u,kPortMuxAlt2);
      break;
    case SPI1_IDX:                       /* SPI1 */
      /* Affects PORTB_PCR10 register */
      PORT_HAL_SetMuxMode(PORTB,10u,kPortMuxAlt2);
      /* Affects PORTB_PCR17 register */
      PORT_HAL_SetMuxMode(PORTB,17u,kPortMuxAlt2);
      /* Affects PORTB_PCR11 register */
      PORT_HAL_SetMuxMode(PORTB,11u,kPortMuxAlt2);
      /* Affects PORTB_PCR16 register */
      PORT_HAL_SetMuxMode(PORTB,16u,kPortMuxAlt2);
      break;
    default:
      break;
  }
}

// Connected TWR_MEM
void configure_sdcard_spi_pins(uint32_t instance)
{
  /* SPI1_CS0 */
  PORT_HAL_SetMuxMode(PORTB,10u,kPortMuxAlt2);
  PORT_HAL_SetPullMode(PORTB,10u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTB,10u,true);

  /* SPI1_SCK */
  PORT_HAL_SetMuxMode(PORTB,11u,kPortMuxAlt2);

  /* SPI1_SOUT */
  PORT_HAL_SetMuxMode(PORTB,16u,kPortMuxAlt2);
  PORT_HAL_SetPullMode(PORTB,16u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTB,16u,true);

  /* SPI1_SIN */
  PORT_HAL_SetMuxMode(PORTB,17u,kPortMuxAlt2);
  PORT_HAL_SetPullMode(PORTB,17u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTB,17u,true);

  /* Pin_Card_detection*/
  PORT_HAL_SetMuxMode(PORTD,15u,kPortMuxAsGpio);

  /* Pin_Card_write_protection*/
  PORT_HAL_SetMuxMode(PORTC,13u,kPortMuxAsGpio);
  PORT_HAL_SetPullMode(PORTC,13u,kPortPullUp);
  PORT_HAL_SetPullCmd(PORTC,13u,true);
}

void configure_uart_pins(uint32_t instance)
{
  switch(instance) {
    case UART1_IDX:                      /* UART1 */
      /* Affects PORTC_PCR3 register */
      PORT_HAL_SetMuxMode(PORTC,3u,kPortMuxAlt3);
      /* Affects PORTC_PCR4 register */
      PORT_HAL_SetMuxMode(PORTC,4u,kPortMuxAlt3);
      break;
    case UART2_IDX:                      /* UART5 */
	  /* Affects PORTD_PCR2 register */
	  PORT_HAL_SetMuxMode(PORTD,2u,kPortMuxAlt3);
	  /* Affects PORTD_PCR3 register */
	  PORT_HAL_SetMuxMode(PORTD,3u,kPortMuxAlt3);
	  break;
    case UART5_IDX:                      /* UART5 */
      /* Affects PORTE_PCR9 register */
      PORT_HAL_SetMuxMode(PORTE,9u,kPortMuxAlt3);
      /* Affects PORTE_PCR8 register */
      PORT_HAL_SetMuxMode(PORTE,8u,kPortMuxAlt3);
      break;
    default:
      break;
  }
}

void configure_cmp_pins(uint32_t instance)
{
  switch (instance) {
    case CMP0_IDX:
      PORT_HAL_SetMuxMode(PORTC, 6u, kPortPinDisabled); /* PTC6 - CMP0_IN0. */
      break;
    default:
      break;
  }
}

void configure_flexbus_pins(uint32_t instance)
{
  switch (instance) {
    case FB_IDX:
      /* Address pins */
      PORT_HAL_SetMuxMode(PORTD,6u,kPortMuxAlt5);      /* FB_AD0 */
      PORT_HAL_SetMuxMode(PORTD,5u,kPortMuxAlt5);      /* FB_AD1 */
      PORT_HAL_SetMuxMode(PORTD,4u,kPortMuxAlt5);      /* FB_AD2 */
      PORT_HAL_SetMuxMode(PORTD,3u,kPortMuxAlt5);      /* FB_AD3 */
      PORT_HAL_SetMuxMode(PORTD,2u,kPortMuxAlt5);      /* FB_AD4 */
      PORT_HAL_SetMuxMode(PORTC,10u,kPortMuxAlt5);     /* FB_AD5 */
      PORT_HAL_SetMuxMode(PORTC,9u,kPortMuxAlt5);      /* FB_AD6 */
      PORT_HAL_SetMuxMode(PORTC,8u,kPortMuxAlt5);      /* FB_AD7 */
      PORT_HAL_SetMuxMode(PORTC,7u,kPortMuxAlt5);      /* FB_AD8 */
      PORT_HAL_SetMuxMode(PORTC,6u,kPortMuxAlt5);      /* FB_AD9 */
      PORT_HAL_SetMuxMode(PORTC,5u,kPortMuxAlt5);      /* FB_AD10 */
      /* Comment out for UART1 purpose.*/
      // PORT_HAL_SetMuxMode(PORTC,4u,kPortMuxAlt5);      /* FB_AD11 */
      PORT_HAL_SetMuxMode(PORTC,2u,kPortMuxAlt5);      /* FB_AD12 */
      PORT_HAL_SetMuxMode(PORTC,1u,kPortMuxAlt5);      /* FB_AD13 */
      PORT_HAL_SetMuxMode(PORTC,0u,kPortMuxAlt5);      /* FB_AD14 */
      PORT_HAL_SetMuxMode(PORTB,18u,kPortMuxAlt5);     /* FB_AD15 */
      PORT_HAL_SetMuxMode(PORTB,17u,kPortMuxAlt5);     /* FB_AD16 */
      PORT_HAL_SetMuxMode(PORTB,16u,kPortMuxAlt5);     /* FB_AD17 */
      PORT_HAL_SetMuxMode(PORTB,11u,kPortMuxAlt5);     /* FB_AD18 */
      PORT_HAL_SetMuxMode(PORTB,10u,kPortMuxAlt5);     /* FB_AD19 */
      PORT_HAL_SetMuxMode(PORTC,15u,kPortMuxAlt5);     /* FB_AD24 */
      PORT_HAL_SetMuxMode(PORTC,14u,kPortMuxAlt5);     /* FB_AD25 */
      PORT_HAL_SetMuxMode(PORTC,13u,kPortMuxAlt5);     /* FB_AD26 */
      PORT_HAL_SetMuxMode(PORTC,12u,kPortMuxAlt5);     /* FB_AD27 */
      PORT_HAL_SetMuxMode(PORTB,23u,kPortMuxAlt5);     /* FB_D28 */
      PORT_HAL_SetMuxMode(PORTB,22u,kPortMuxAlt5);     /* FB_D29 */
      PORT_HAL_SetMuxMode(PORTB,21u,kPortMuxAlt5);     /* FB_D30 */
      PORT_HAL_SetMuxMode(PORTB,20u,kPortMuxAlt5);     /* FB_D31 */

      /* R/W */
      PORT_HAL_SetMuxMode(PORTC,11u,kPortMuxAlt5);     /* FB_R/W_B */

      /* OE */
      PORT_HAL_SetMuxMode(PORTB,19u,kPortMuxAlt5);     /* FB_OE_B */

      /* CS */
      PORT_HAL_SetMuxMode(PORTD,1u,kPortMuxAlt5);      /* FB_CS0 */
      PORT_HAL_SetMuxMode(PORTD,0u,kPortMuxAlt5);      /* FB_CS1 */

      /* CLKOUT */
      /* Comment out for UART1 purpose.*/
      // PORT_HAL_SetMuxMode(PORTC, 3u, kPortMuxAlt5);      /* FB_CLKOUT */
      break;
    default:
      break;
  }
}

/* END pin_mux. */
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

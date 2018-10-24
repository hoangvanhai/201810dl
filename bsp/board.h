/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
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

#if !defined(__BOARD_H__)
#define __BOARD_H__

#include <stdint.h>
#include "pin_mux.h"
#include "gpio_pins.h"

/* The board name */
#define BOARD_NAME                      "TWR-K64F120M"

#define CLOCK_VLPR 1U
#define CLOCK_RUN  2U
#define CLOCK_NUMBER_OF_CONFIGURATIONS 3U

#ifndef CLOCK_INIT_CONFIG
#define CLOCK_INIT_CONFIG CLOCK_RUN
#endif

#if (CLOCK_INIT_CONFIG == CLOCK_RUN)
#define CORE_CLOCK_FREQ 120000000U
#else
#define CORE_CLOCK_FREQ 4000000U
#endif

/* OSC0 configuration. */
#define OSC0_XTAL_FREQ 50000000U
#define OSC0_SC2P_ENABLE_CONFIG  false
#define OSC0_SC4P_ENABLE_CONFIG  false
#define OSC0_SC8P_ENABLE_CONFIG  false
#define OSC0_SC16P_ENABLE_CONFIG false
#define MCG_HGO0   kOscGainLow
#define MCG_RANGE0 kOscRangeVeryHigh
#define MCG_EREFS0 kOscSrcExt

/* EXTAL0 PTA18 */
#define EXTAL0_PORT   PORTA
#define EXTAL0_PIN    18
#define EXTAL0_PINMUX kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT   PORTA
#define XTAL0_PIN    19
#define XTAL0_PINMUX kPortPinDisabled

/* RTC external clock configuration. */
#define RTC_XTAL_FREQ   32768U
#define RTC_SC2P_ENABLE_CONFIG       false
#define RTC_SC4P_ENABLE_CONFIG       false
#define RTC_SC8P_ENABLE_CONFIG       false
#define RTC_SC16P_ENABLE_CONFIG      false
#define RTC_OSC_ENABLE_CONFIG        true

#define BOARD_RTC_CLK_FREQUENCY     32768U;
/* The UART to use for debug messages. */
#ifndef BOARD_DEBUG_UART_INSTANCE
    #define BOARD_DEBUG_UART_INSTANCE   1
    #define BOARD_DEBUG_UART_BASEADDR   UART1
#endif
#ifndef BOARD_DEBUG_UART_BAUD
    #define BOARD_DEBUG_UART_BAUD       115200
#endif

/* This define to use for power manager demo */
#define BOARD_LOW_POWER_UART_BAUD       9600

#define BOARD_USE_UART
#define PM_DBG_UART_IRQ_HANDLER         UART1_RX_TX_IRQHandler
#define PM_DBG_UART_IRQn                UART1_RX_TX_IRQn

/* Define feature for the low_power_demo */
#define FSL_FEATURE_HAS_VLLS2 (1)

/* Define the port interrupt number for the board switches */
#define BOARD_SW_GPIO               kGpioSW3
#define BOARD_SW_IRQ_NUM            PORTA_IRQn
#define BOARD_SW_IRQ_HANDLER        PORTA_IRQHandler
#define BOARD_SW_NAME                "SW3"
/* Define print statement to inform user which switch to press for
 * power_manager_hal_demo and power_manager_rtos_demo
 */
#define PRINT_LLWU_SW_NUM \
  PRINTF("SW1")

#define BOARD_MAX3353_INT_PORT       (PORTD_BASE)        /* BOARD_MAX3353_INT_PORT */
#define BOARD_MAX3353_INT_PIN        (14U)               /* BOARD_MAX3353_INT_PIN */
#define BOARD_MAX3353_INT_VECTOR     (PORTD_IRQn)        /* BOARD_MAX3353_INT_VECTOR */
#define BOARD_MAX3353_GPIO_INT       GPIO_MAKE_PIN(GPIOD_IDX, BOARD_MAX3353_INT_PIN)

/* Defines the llwu pin number for board switch which is used in power_manager_demo. */
#define BOARD_SW_HAS_LLWU_PIN        1
#define BOARD_SW_LLWU_EXT_PIN        kLlwuWakeupPin10
/* Switch port base address and IRQ handler name. Used by power_manager_demo */
#define BOARD_SW_LLWU_PIN            6
#define BOARD_SW_LLWU_BASE           PORTC
#define BOARD_SW_LLWU_IRQ_HANDLER    PORTC_IRQHandler
#define BOARD_SW_LLWU_IRQ_NUM        PORTC_IRQn

#define BOARD_I2C_GPIO_SCL           GPIO_MAKE_PIN(GPIOC_IDX, 10)
#define BOARD_I2C_GPIO_SDA           GPIO_MAKE_PIN(GPIOC_IDX, 11)

#define HWADC_INSTANCE               1
#define ADC_IRQ_N                    ADC1_IRQn

/* The instances of peripherals used for dac_adc_demo */
#define BOARD_DAC_DEMO_DAC_INSTANCE     0U
#define BOARD_DAC_DEMO_ADC_INSTANCE     1U
#define BOARD_DAC_DEMO_ADC_CHANNEL      10U

/* The CAN instance used for board */
#define BOARD_CAN_INSTANCE              0

/* The i2c instance used for i2c DAC demo */
#define BOARD_DAC_I2C_INSTANCE          1

/* The i2c instance used for OTG demo */
#define BOARD_MAX3353_I2C_INSTANCE      0
#define BOARD_MAX3353_I2C_VECTOR        I2C0_IRQn

/* The i2c instance used for i2c connection by default */
#define BOARD_I2C_INSTANCE              1

/* The dspi instance used for dspi example */
#define BOARD_DSPI_INSTANCE             0

/* The Flextimer instance/channel used for board */
#define BOARD_FTM_INSTANCE              3
#define BOARD_FTM_CHANNEL               1

/* The Enet instance used for board */
#define BOARD_ENET_INSTANCE             0

/* The FlexBus instance used for board.*/
#define BOARD_FLEXBUS_INSTANCE          0

/* board led color mapping */
#define BOARD_GPIO_LED_RED              kGpioLED3
#define BOARD_GPIO_LED_GREEN            kGpioLED1
#define BOARD_GPIO_LED_BLUE             kGpioLED4
#define BOARD_GPIO_LED_YELLOW           kGpioLED2

#define TX_DEBUG_CONSOLE_DIS PORT_HAL_SetMuxMode(PORTC, 4, kPortMuxAsGpio)
#define RX_DEBUG_CONSOLE_DIS PORT_HAL_SetMuxMode(PORTC, 3, kPortMuxAsGpio)

#define SW_INT_DIS PORT_HAL_SetPinIntMode(PORTA, 4, kPortIntDisabled)
#define SW_DIS PORT_HAL_SetMuxMode(PORTA, 4, kPortPinDisabled)
#define SW_EN PORT_HAL_SetMuxMode(PORTA, 4, kPortMuxAsGpio)

#define LED1_EN  (GPIO_DRV_OutputPinInit(&ledPins[0]))  /*!< Enable target LED1 */
#define LED2_EN  (GPIO_DRV_OutputPinInit(&ledPins[1]))  /*!< Enable target LED2 */
#define LED3_EN  (GPIO_DRV_OutputPinInit(&ledPins[2]))  /*!< Enable target LED3 */
#define LED4_EN  (GPIO_DRV_OutputPinInit(&ledPins[3]))  /*!< Enable target LED4 */

#define LED1_DIS (PORT_HAL_SetMuxMode(PORTE, 6, kPortPinDisabled))   /*!< Disable target LED1 */
#define LED2_DIS (PORT_HAL_SetMuxMode(PORTE, 7, kPortPinDisabled))   /*!< Disable target LED2 */
#define LED3_DIS (PORT_HAL_SetMuxMode(PORTE, 8, kPortPinDisabled))   /*!< Disable target LED3 */
#define LED4_DIS (PORT_HAL_SetMuxMode(PORTE, 9, kPortPinDisabled))   /*!< Disable target LED4 */

#define LED1_OFF (GPIO_DRV_WritePinOutput(ledPins[0].pinName, 1))         /*!< Turn off target LED1 */
#define LED2_OFF (GPIO_DRV_WritePinOutput(ledPins[1].pinName, 1))         /*!< Turn off target LED2 */
#define LED3_OFF (GPIO_DRV_WritePinOutput(ledPins[2].pinName, 1))         /*!< Turn off target LED3 */
#define LED4_OFF (GPIO_DRV_WritePinOutput(ledPins[3].pinName, 1))         /*!< Turn off target LED4 */

#define LED1_ON (GPIO_DRV_WritePinOutput(ledPins[0].pinName, 0))          /*!< Turn on target LED1 */
#define LED2_ON (GPIO_DRV_WritePinOutput(ledPins[1].pinName, 0))          /*!< Turn on target LED2 */
#define LED3_ON (GPIO_DRV_WritePinOutput(ledPins[2].pinName, 0))          /*!< Turn on target LED3 */
#define LED4_ON (GPIO_DRV_WritePinOutput(ledPins[3].pinName, 0))          /*!< Turn on target LED4 */

#define LED1_TOGGLE (GPIO_DRV_TogglePinOutput(ledPins[0].pinName))        /*!< Toggle on target LED1 */
#define LED2_TOGGLE (GPIO_DRV_TogglePinOutput(ledPins[1].pinName))        /*!< Toggle on target LED2 */
#define LED3_TOGGLE (GPIO_DRV_TogglePinOutput(ledPins[2].pinName))        /*!< Toggle on target LED3 */
#define LED4_TOGGLE (GPIO_DRV_TogglePinOutput(ledPins[3].pinName))        /*!< Toggle on target LED4 */

#define LED_RTOS_EN        LED1_EN
#define LED_RTOS_TOGGLE    LED1_TOGGLE
#define LED_CLOCK_EN       LED2_EN
#define LED_CLOCK_TOGGLE   LED2_TOGGLE

#define OFF_ALL_LEDS  \
                           LED1_OFF;\
                           LED2_OFF;\
                           LED3_OFF;\
                           LED4_OFF;

/* The SDHC instance/channel used for board */
#define BOARD_SDHC_INSTANCE                   0
#define BOARD_SDHC_CD_GPIO_IRQ_HANDLER        PORTE_IRQHandler

/* The CMP instance used for board. */
#define BOARD_CMP_INSTANCE              0
/* The CMP channel used for board. */
#define BOARD_CMP_CHANNEL               0
/* The SW name for CMP example */
#define BOARD_CMP_SW_NAME               "SW1"

/* The i2c instance used for sai demo */
#define BOARD_SAI_DEMO_I2C_INSTANCE     0

/* The rtc instance used for rtc_func */
#define BOARD_RTC_FUNC_INSTANCE         0

/* If connected the TWR_MEM, this is spi sd card */
#define SDCARD_CARD_DETECTION_GPIO_PORT         PORTD_IDX
#define SDCARD_CARD_DETECTION_GPIO_PIN          15
#define SDCARD_CARD_WRITE_PROTECTION_GPIO_PORT  PORTC_IDX
#define SDCARD_CARD_WRITE_PROTECTION_GPIO_PIN   13
#define SDCARD_SPI_HW_INSTANCE                  1
#define SDCARD_CARD_INSERTED                    0

/* The usb use native port  */
#define USBCFG_HOST_PORT_NATIVE         (1)

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void hardware_init(void);
void dbg_uart_init(void);
/*This function to used for power manager demo*/
void disable_unused_pins(void);
void enable_unused_pins(void);
/* Function to initialize clock base on board configuration. */
void BOARD_ClockInit(void);

/* Function to initialize OSC0 base on board configuration. */
void BOARD_InitOsc0(void);

/* Function to initialize RTC external clock base on board configuration. */
void BOARD_InitRtcOsc(void);

bool BOARD_IsSDCardDetected(void);

/*Function to handle board-specified initialization*/
uint8_t usb_device_board_init(uint8_t controller_id);
/*Function to handle board-specified initialization*/
uint8_t usb_host_board_init(uint8_t controller_id);
/*Function to handle board-specified initialization*/
uint8_t usb_otg_board_init(uint8_t controller_id);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* __BOARD_H__ */

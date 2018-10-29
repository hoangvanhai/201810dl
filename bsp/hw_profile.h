/*
 * hwprofile.h
 *
 *  Created on: Oct 25, 2018
 *      Author: PC
 */

#ifndef APPLICATION_HW_PROFILE_H_
#define APPLICATION_HW_PROFILE_H_

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
#define OSC0_XTAL_FREQ 						50000000U
#define OSC0_SC2P_ENABLE_CONFIG  			false
#define OSC0_SC4P_ENABLE_CONFIG  			false
#define OSC0_SC8P_ENABLE_CONFIG  			false
#define OSC0_SC16P_ENABLE_CONFIG 			false
#define MCG_HGO0   							kOscGainLow
#define MCG_RANGE0 							kOscRangeVeryHigh
#define MCG_EREFS0 							kOscSrcExt

/* EXTAL0 PTA18 */
#define EXTAL0_PORT   						PORTA
#define EXTAL0_PIN    						18
#define EXTAL0_PINMUX 						kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT   PORTA
#define XTAL0_PIN    19
#define XTAL0_PINMUX kPortPinDisabled

/* RTC external clock configuration. */
#define RTC_XTAL_FREQ   32768U
#define RTC_SC2P_ENABLE_CONFIG       		false
#define RTC_SC4P_ENABLE_CONFIG       		false
#define RTC_SC8P_ENABLE_CONFIG       		false
#define RTC_SC16P_ENABLE_CONFIG      		false
#define RTC_OSC_ENABLE_CONFIG        		true

#define BOARD_RTC_CLK_FREQUENCY     		32768U;
/* The UART to use for debug messages. */

#define BOARD_DEBUG_UART_INSTANCE   	1
#define BOARD_DEBUG_UART_BAUD       	115200

#define BOARD_MODBUS_UART_INSTANCE   	3
#define BOARD_MODBUS_UART_BAUD       	115200

#define BOARD_TRANSPC_UART_INSTANCE   	2
#define BOARD_TRANSPC_UART_BAUD       	115200


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
  LREP("SW1")

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
//#define SDCARD_CARD_DETECTION_GPIO_PORT         PORTD_IDX
//#define SDCARD_CARD_DETECTION_GPIO_PIN          15
//#define SDCARD_CARD_WRITE_PROTECTION_GPIO_PORT  PORTC_IDX
//#define SDCARD_CARD_WRITE_PROTECTION_GPIO_PIN   13
//#define SDCARD_SPI_HW_INSTANCE                  1
//#define SDCARD_CARD_INSERTED                    0


#endif /* APPLICATION_HW_PROFILE_H_ */

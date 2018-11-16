/*
 * hwprofile.h
 *
 *  Created on: Oct 25, 2018
 *      Author: PC
 */

#ifndef APPLICATION_HW_PROFILE_H_
#define APPLICATION_HW_PROFILE_H_

#include <app_cfg.h>
#include <MK64F12_extension.h>

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

/* HARDWARE CORE CONFIG */
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
#define XTAL0_PORT   						PORTA
#define XTAL0_PIN    						19
#define XTAL0_PINMUX 						kPortPinDisabled

/* debug */
#define BOARD_DEBUG_UART_INSTANCE   		1
#define BOARD_DEBUG_UART_BAUD       		115200

/* communication */
#define BOARD_TRANSPC_UART_INSTANCE   		2
#define BOARD_TRANSPC_UART_BAUD       		115200
#define BOARD_TRANSUI_UART_INSTANCE   		3
#define BOARD_TRANSUI_UART_BAUD       		115200
#define BOARD_MODBUS_UART_INSTANCE   		4
#define BOARD_MODBUS_UART_BAUD       		115200

#define TRANS_RX_PRIO						2
#define TRANS_TX_PRIO						2
#define TRANSL1_V1							1
#define TRANSL1_V2							2
#define TRANSL1_VER							TRANSL1_V1


/* analog measurement */
/* The Flextimer instance/channel used for board */
#define BOARD_FTM_INSTANCE					FTM0_IDX
#define BOARD_FTM_CHANNEL               	CHAN0_IDX
/* spi for sdcard2 */
#define BOARD_DSPI_INSTANCE             	0
/* The Enet instance used for board */
#define BOARD_ENET_INSTANCE             	0
/* The FlexBus instance used for board.*/
#define BOARD_FLEXBUS_INSTANCE          	0
/* The SDHC instance/channel used for board */
#define BOARD_SDHC_INSTANCE             	0
#define BOARD_SDHC_CD_GPIO_IRQ_HANDLER  	PORTE_IRQHandler
/* The CMP instance used for board. */
#define BOARD_CMP_INSTANCE              	0
/* The CMP channel used for board. */
#define BOARD_CMP_CHANNEL               	0
/* The SW name for CMP example */
#define BOARD_I2C_RTC_INSTANCE				1

#endif /* APPLICATION_HW_PROFILE_H_ */

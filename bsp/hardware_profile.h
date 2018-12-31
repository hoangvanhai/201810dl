/*
 * hwprofile.h
 *
 *  Created on: Oct 25, 2018
 *      Author: PC
 */

#ifndef APPLICATION_HW_PROFILE_H_
#define APPLICATION_HW_PROFILE_H_

#include <app_cfg.h>
#ifdef CPU_MK66FN2M0VLQ18
#include <MK66F18_extension.h>
#else
#include <MK64F12_extension.h>
#endif

/* The board name */
#define BOARD_NAME                      	"DATA_LOGGER"

#if BOARD_USE_VERSION == BOARD_FRDM_K64F
#define CORE_CLOCK_FREQ 120000000U
#define OSC0_XTAL_FREQ 						50000000U
#define OSC0_SC2P_ENABLE_CONFIG  			false
#define OSC0_SC4P_ENABLE_CONFIG  			false
#define OSC0_SC8P_ENABLE_CONFIG  			false
#define OSC0_SC16P_ENABLE_CONFIG 			false
#define MCG_HGO0   							kOscGainLow
#define MCG_RANGE0 							kOscRangeVeryHigh
#define MCG_EREFS0 							kOscSrcExt
#elif BOARD_USE_VERSION == BOARD_VERSION_1
#define CORE_CLOCK_FREQ 180000000U
#define OSC0_XTAL_FREQ 						8000000U
#define OSC0_SC2P_ENABLE_CONFIG  			false
#define OSC0_SC4P_ENABLE_CONFIG  			false
#define OSC0_SC8P_ENABLE_CONFIG  			false
#define OSC0_SC16P_ENABLE_CONFIG 			true
#define MCG_HGO0   							kOscGainLow
#define MCG_RANGE0 							kOscRangeVeryHigh
#define MCG_EREFS0 							kOscSrcExt
#endif

/* EXTAL0 PTA18 */
#define EXTAL0_PORT   						PORTA
#define EXTAL0_PIN    						18
#define EXTAL0_PINMUX 						kPortPinDisabled

/* XTAL0 PTA19 */
#define XTAL0_PORT   						PORTA
#define XTAL0_PIN    						19
#define XTAL0_PINMUX 						kPortPinDisabled



#if BOARD_USE_VERSION == BOARD_FRDM_K64F
#define BOARD_ANALOG_UART_INSTANCE			0
#define BOARD_ANALOG_UART_BAUD				9600
/* debug */
#define BOARD_DEBUG_UART_INSTANCE   		1
#define BOARD_DEBUG_UART_BAUD       		115200	//921600	//115200
/* communication */
#define BOARD_TRANSPC_UART_INSTANCE   		2
#define BOARD_TRANSPC_UART_BAUD       		115200
#define BOARD_TRANSUI_UART_INSTANCE   		3
#define BOARD_TRANSUI_UART_BAUD       		115200
#define BOARD_MODBUS_UART_INSTANCE   		4
#define BOARD_MODBUS_UART_BAUD       		115200
#define BOARD_SIM_UART_INSTANCE				0
#define BOARD_SIM_UART_BAUD					115200

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
/* The ENET instance/channel used for board */
#define BOARD_ENET_INSTANCE             	0
/* The SDHC instance/channel used for board */
#define BOARD_SDHC_INSTANCE             	0
#define BOARD_SDHC_CD_GPIO_IRQ_HANDLER  	PORTD_IRQHandler
/* The CMP instance used for board. */
#define BOARD_CMP_INSTANCE              	0
/* The CMP channel used for board. */
#define BOARD_CMP_CHANNEL               	0
/* The SW name for CMP example */
#define BOARD_I2C_RTC_INSTANCE				1


#elif BOARD_USE_VERSION == BOARD_VERSION_1

// SIM 0
// MB  1
// AI  2
// PC  3
// UI  4
// DBG 5

/* debug */
#define BOARD_DEBUG_UART_INSTANCE   		5
#define BOARD_DEBUG_UART_BAUD       		1000000	//460800	//921600	//115200

/* communication */
#define BOARD_ANALOG_UART_INSTANCE			2
#define BOARD_ANALOG_UART_BAUD				9600
#define BOARD_TRANSPC_UART_INSTANCE   		3
#define BOARD_TRANSPC_UART_BAUD       		115200
#define BOARD_TRANSUI_UART_INSTANCE   		4
#define BOARD_TRANSUI_UART_BAUD       		115200
#define BOARD_MODBUS_UART_INSTANCE   		1
#define BOARD_MODBUS_UART_BAUD       		115200
#define BOARD_SIM_UART_INSTANCE				0
#define BOARD_SIM_UART_BAUD					115200


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
/* The ENET instance/channel used for board */
#define BOARD_ENET_INSTANCE             	0
/* The SDHC instance/channel used for board */
#define BOARD_SDHC_INSTANCE             	0
#define BOARD_SDHC_CD_GPIO_IRQ_HANDLER  	PORTD_IRQHandler
#define BOARD_SDSPI_INSTANCE				2
/* The CMP instance used for board. */
#define BOARD_CMP_INSTANCE              	0
/* The CMP channel used for board. */
#define BOARD_CMP_CHANNEL               	0
/* The SW name for CMP example */
#define BOARD_I2C_RTC_INSTANCE				1
#define BOARD_I2C_DAC_INSTANCE				3

#endif

#endif /* APPLICATION_HW_PROFILE_H_ */

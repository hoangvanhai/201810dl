/*
 * modem_wrapper.h
 *
 *  Created on: Oct 26, 2018
 *      Author: buiti
 */

#ifndef MODEM_WRAPPER_H_
#define MODEM_WRAPPER_H_

#define MK64 	1U
#define STM32 	2U


#define PLATFORM_NAME MK64

#if (PLATFORM_NAME == STM32)
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;	// AT port
extern UART_HandleTypeDef huart2;	// debug port

#define MODEM_UART_INSTANCE_AT		&huart1
#define MODEM_UART_INSTANCE_DBG		&huart2


#define modem_tx_data(data, len) 		{\
											HAL_UART_Transmit(MODEM_UART_INSTANCE_AT,(uint8_t*)data, len, 100);\
										}

#define modem_delay_ms(ms) 				{\
											HAL_Delay(ms);\
										}

#define modem_debug(msg) 				{\
											HAL_UART_Transmit(MODEM_UART_INSTANCE_DBG , (uint8_t*)msg, strlen(msg), 100);\
										}
#elif  (PLATFORM_NAME == MK64)
#include "board.h"
#include "fsl_uart_driver.h"
#include "fsl_os_abstraction.h"
#include "fsl_misc_utilities.h"
#include "fsl_debug_console.h"
#include <app_cfg.h>

//#define MODEM_UART_INSTANCE_AT		4
//
//#define MODEM_UART_INSTANCE_DBG		0

#if 1
#define modem_tx_data(data, len) 		{\
											UART_DRV_SendData(BOARD_MODEM_UART_INSTANCE, data, len); \
											while (UART_DRV_GetTransmitStatus(BOARD_MODEM_UART_INSTANCE, NULL) == kStatus_UART_TxBusy); \
										}
#else
#define modem_tx_data(data, len) 		{\
											UART_DRV_SendDataBlocking(BOARD_MODEM_UART_INSTANCE, data, len, 1000); \
										}
#endif

//
#define modem_debug(msg) 				{\
											PRINTF(msg); \
										}

//void modem_hw_init();
void modem_delay_ms(uint16_t ms);

/**
 * UART RX Callback registered to UART driver
 * @param instance	UART Instance number
 * @param uartState	UART state, contain rx buffer
 */
//void modem_rx_callback(uint32_t instance, void * uartState);

#endif
#endif /* MODEM_WRAPPER_H_ */

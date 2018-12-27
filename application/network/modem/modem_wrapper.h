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

#if 0
#define MODEM_ENTER_CRITIAL()		OSA_MutexLock(&g_modem_tx_mtx, 1000)
#define MODEM_EXIT_CRITIAL()		OSA_MutexUnlock(&g_modem_tx_mtx)
#else
#define MODEM_ENTER_CRITIAL()		kStatus_OSA_Success
#define MODEM_EXIT_CRITIAL()
#endif
//#define MODEM_UART_INSTANCE_AT		4
//
//#define MODEM_UART_INSTANCE_DBG		0

typedef void (*modem_uart_rx_callback)(void* data, uint16_t len);
//typedef void (*NetworkDataEvent)(const uint8_t* data, int length);
//#if 1
//#define modem_tx_data(data, len) 		{\
//											ASSERT_NONVOID(MODEM_ENTER_CRITIAL() == kStatus_OSA_Success, 1 ); \
//											UART_DRV_SendData(BOARD_SIM_UART_INSTANCE, data, len); \
//											while (UART_DRV_GetTransmitStatus(BOARD_SIM_UART_INSTANCE, NULL) == kStatus_UART_TxBusy); \
//											 MODEM_EXIT_CRITIAL(); \
//											 return 0; \
//										}
//#else
//#define modem_tx_data(data, len) 		{\
//											UART_DRV_SendDataBlocking(BOARD_MODEM_UART_INSTANCE, data, len, 1000); \
//										}
//#endif

//
#define modem_debug(msg) 				{\
											PRINTF(msg); \
										}
uint8_t modem_tx_data(data, len);

//void modem_hw_init();
void modem_delay_ms(uint16_t ms);

/**
 * UART RX Callback registered to UART driver
 * @param instance	UART Instance number
 * @param uartState	UART state, contain rx buffer
 */
//void modem_rx_callback(uint32_t instance, void * uartState);
void modem_hw_add_rx_callback(modem_uart_rx_callback cb);

#endif
#endif /* MODEM_WRAPPER_H_ */

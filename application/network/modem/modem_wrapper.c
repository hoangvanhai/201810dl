/*
 * modem_wrapper.c
 *
 *  Created on: Oct 26, 2018
 *      Author: buiti
 */

#include "modem_wrapper.h"
#define N 12000



//uint8_t modem_rx_buffer[32] = {0};
static uint8_t rxChar;
static uart_state_t uartState;
modem_uart_rx_callback modem_rx_cb = NULL;


void uart_rx_callback(uint32_t instance, void * uartState) {
	uart_state_t *state = (uart_state_t*) uartState;
	if(modem_rx_cb != NULL)
		modem_rx_cb(state->rxBuff, 1);
}

#if 0
void modem_delay_ms(uint16_t ms)

{
	uint32_t i;

	for(i=0; i<N*ms; i++)
	{
//		__asm("nop");  //in CW tools use asm("nop");
	}
//	OSA_TimeDelay(1);

}
#else
void modem_delay_ms(uint16_t ms){
	OSA_TimeDelay(ms);
}
#endif

void modem_hw_init(void)
{
	modem_rx_cb = NULL;

	configure_uart_pins(BOARD_SIM_UART_INSTANCE);

	// Fill in uart config data
	uart_user_config_t uartConfig = { .bitCountPerChar = kUart8BitsPerChar,
			.parityMode = kUartParityDisabled, .stopBitCount = kUartOneStopBit,
			.baudRate = BOARD_SIM_UART_BAUD };

	// Initialize the uart module with base address and config structure
	UART_DRV_Init(BOARD_SIM_UART_INSTANCE, &uartState, &uartConfig);

	//Initialize the callback function
	UART_DRV_InstallRxCallback(BOARD_SIM_UART_INSTANCE, uart_rx_callback,
			&rxChar, &uartState,
			true);
}

void modem_hw_add_rx_callback(modem_uart_rx_callback cb)
{
	modem_rx_cb = cb;
}





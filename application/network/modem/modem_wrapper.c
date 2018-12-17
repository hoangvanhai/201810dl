/*
 * modem_wrapper.c
 *
 *  Created on: Oct 26, 2018
 *      Author: buiti
 */

#include "modem_wrapper.h"
#define N 12000

uint8_t modem_rx_buffer[32] = {0};

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
#if 0
	// Initialize variable uartState of type uart_state_t
	    uart_state_t uartState;

	    // Fill in uart config data
	    uart_user_config_t uartConfig = {
	        .bitCountPerChar = kUart8BitsPerChar,
	        .parityMode      = kUartParityDisabled,
	        .stopBitCount    = kUartOneStopBit,
	        .baudRate        = BOARD_MODEM_UART_BAUD
	    };

	    // Initialize the uart module with base address and config structure
	    UART_DRV_Init(BOARD_MODEM_UART_INSTANCE, &uartState, &uartConfig);
//
//	    // Inform to start non blocking example
//	    byteCountBuff = sizeof(buffStart);
//	    UART_DRV_SendData(BOARD_DEBUG_UART_INSTANCE, buffStart, byteCountBuff);
//
//	    // Wait until transmission is finished
//	    while (kStatus_UART_TxBusy == UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}
//
//	    // Inform user of what to do
//	    byteCountBuff = sizeof(bufferData1);
//	    UART_DRV_SendData(BOARD_DEBUG_UART_INSTANCE, bufferData1, byteCountBuff);
//
//	    // Wait until transmission is finished
//	    while (kStatus_UART_TxBusy == UART_DRV_GetTransmitStatus(BOARD_DEBUG_UART_INSTANCE, NULL)){}
//	    //using callback function by Rong modification

	    void* pFunc = (uart_rx_callback_t *)modem_rx_callback;
	//  UART_DRV_InstallRxCallback(BOARD_DEBUG_UART_INSTANCE,(uart_rx_callback_t)pFunc,0,NULL);
	    UART_DRV_InstallRxCallback(BOARD_MODEM_UART_INSTANCE, (uart_rx_callback_t)pFunc, &rx_char, NULL, true);
#endif
}

//void modem_rx_callback(uint32_t instance, void * uartState)
//{
//	uart_state_t* pUARTState = (uart_state_t*)uartState;
//      uint8_t rxChar, txChar;
//      txChar='K';
//      rxChar = pUARTState->rxBuff[0];
//       // txChar = rxChar;
//        UART_DRV_SendData(BOARD_DEBUG_UART_INSTANCE, &rxChar, 1u);
//}





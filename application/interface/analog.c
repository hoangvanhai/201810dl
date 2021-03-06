
/***************************** Include Files *********************************/
#include <string.h>
#include "analog.h"
#include <fsl_debug_console.h>
#include <fsl_uart_driver.h>
#include <gpio_pins.h>
#include <board.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
static void 		analog_rx_handle(uint32_t instance, void * uartState);
static void 		analog_tx_handle(uint32_t instance, void * uartState);
/************************** Variable Definitions *****************************/
uart_state_t analog_uart_state;
static uint8_t rx_char, tx_char;
SAnalogReader *pThisAr;
/*****************************************************************************/
/** @brief
 *		   init UART port
 *
 *  @param
 *  @return Void.
 *  @note
 */


void Analog_Uart_Init(uint32_t uartInstance, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio)
{
	uart_user_config_t modbus_uart_cfg = {
					.baudRate = u32Baudrate,
					.parityMode = kUartParityDisabled,
					.stopBitCount = kUartOneStopBit,
					.bitCountPerChar = kUart8BitsPerChar,
				};

	configure_uart_pins(BOARD_ANALOG_UART_INSTANCE);

	UART_DRV_Init(uartInstance, &analog_uart_state, &modbus_uart_cfg);

	UART_DRV_InstallRxCallback(uartInstance, analog_rx_handle, &rx_char, NULL, true);
	UART_DRV_InstallTxCallback(uartInstance, analog_tx_handle, &tx_char, NULL);
}



/*****************************************************************************/
/** @brief
 *		   Initialize Transmission Layer 1
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t  Analog_Init(SAnalogReader *pAR, uint32_t uartInstance,
		uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio)
{
	pThisAr = pAR;
	FIFO_Create(&pAR->sRecvFIFO, pAR->arrRecvFIFO, 30);
	pAR->uartInstance	 	= uartInstance;
	pAR->u32BaudRate 		= u32BaudRate;
	LREP("init uart port analog: %d\r\n", uartInstance);
	Analog_Uart_Init(uartInstance, u32BaudRate, u8TxIntPrio, u8RxIntPrio);
	return AR_SUCCESS;
}


/*****************************************************************************/
/** @brief Modbus_Send
 *		   Try to receive a number of bytes from the receving FIFO.
 *
 *  @param
 *  @return actual number of received bytes
 *  @note
 */
int Analog_RecvData(SAnalogReader *pAR, uint8_t* pData, uint16_t u16Size)
{
	uint16_t 	u16Idx;
	SFIFO 		*pFF;
	uint8_t  	u8Data;
	
	ASSERT_NONVOID(pAR != 0, 0);

	pFF = &pAR->sRecvFIFO;


	if(u16Size > FIFO_GetCount(pFF)) {
		u16Size = FIFO_GetCount(pFF);
	}

	for(u16Idx = 0; u16Idx < u16Size; u16Idx++)  {
		FIFO_Pop(pFF, &u8Data);
		*pData++ = u8Data;
	}
	
	return u16Size;
}

/*****************************************************************************/
/** @brief Modbus_Send
 *		   Try to receive a number of bytes from the receving FIFO.
 *
 *  @param
 *  @return actual number of received bytes
 *  @note
 */
int Analog_SelectChannel (SAnalogReader *pAR, uint8_t channel) {

	GPIO_DRV_SetPinOutput(TriggerAnalog);
	OSA_SleepMs(10);
	GPIO_DRV_ClearPinOutput(TriggerAnalog);
	return 0;
}

/*****************************************************************************/
/** @brief Modbus_Send
 *		   Try to receive a number of bytes from the receving FIFO.
 *
 *  @param
 *  @return actual number of received bytes
 *  @note
 */
void Analog_RecvFF_EnProtect(SAnalogReader *pAR, BOOL bEn)
{
	ASSERT_VOID(pAR != 0);
	if(bEn) FIFO_EnableProtect(&pAR->sRecvFIFO);
	else    FIFO_DisableProtect(&pAR->sRecvFIFO);

}
/*****************************************************************************/
/** @brief 	Modbus_RecvFF_RewindHead
 *			Rewind head pointer to the protected value
 *  @param
 *  @return Void.
 *  @note
 */
void Analog_RecvFF_RewindHead(SAnalogReader *pAR)
{
	ASSERT_VOID(pAR != 0);
	FIFO_RewindHead(&pAR->sRecvFIFO);
}
/*****************************************************************************/
/** @brief 	Modbus_RecvFF_Pop
 *			Try to pop one byte from the received FIFO
 *  @param
 *  @return value.
 *  @note
 */
BYTE Analog_RecvFF_Pop(SAnalogReader *pAR)
{
	BYTE b;

	ASSERT_NONVOID(pAR != 0, 0xFF);
	FIFO_Pop(&pAR->sRecvFIFO, &b);

	return b;
}

/*****************************************************************************/
/** @brief 	Modbus_GetRecvCount
 *			Get size of the receving FIFO
 *  @param
 *  @return size of FIFO.
 *  @note
 */
int Analog_GetRecvCount(SAnalogReader *pAR)
{
	ASSERT_NONVOID(pAR != 0, 0);
	return FIFO_GetCount(&pAR->sRecvFIFO );
}
/*****************************************************************************/
/** @brief 	Modbus_GetRecvCount
 *			Get size of the receving FIFO
 *  @param
 *  @return size of FIFO.
 *  @note
 */
void Analog_RecvFF_Reset (SAnalogReader *pAR) {
	FIFO_Reset(&pAR->sRecvFIFO);
}

/*****************************************************************************/
/** @brief RX_Interrupt
 *		   function for Rx handler
 *  @param
 *  @return Void.
 *  @note
 */

static void analog_rx_handle(uint32_t instance, void * uartState) {
	uart_state_t *state = (uart_state_t*)uartState;

	//LREP("%02x ", state->rxBuff[0]);
	if(FIFO_Push(&pThisAr->sRecvFIFO, state->rxBuff[0]) == FALSE) {
		LREP("push fifo error \r\n");
		//FIFO_Reset(&pThisAr->sRecvFIFO);
	}
}

/*****************************************************************************/
/** @brief TX_Interrupt
 *		   function for Tx handler
 *  @param
 *  @return Void.
 *  @note
 */
static void analog_tx_handle(uint32_t instance, void * uartState) {
	uart_state_t *state = (uart_state_t*)uartState;
	if(state->txSize > 0) {
		state->txBuff++;
		state->txSize--;
		if(state->txSize == 0) {

		}
	}
}

/*****************************************************************************/



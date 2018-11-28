
/***************************** Include Files *********************************/
#include <string.h>
#include "layer1.h"
#include <fsl_debug_console.h>
#include <fsl_uart_driver.h>
#include <gpio_pins.h>
#include <board.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


#define UART_RX_ENTER_CRITICAL()     	//UART_HAL_SetIntMode((UART_Type*)pModbus->uartBase,kUartIntRxDataRegFull, false)
#define UART_RX_EXIT_CRITICAL()			//UART_HAL_SetIntMode((UART_Type*)pModbus->uartBase,kUartIntRxDataRegFull, true)

#define UART_TX_ENTER_CRITICAL()		//UART_HAL_SetIntMode((UART_Type*)pModbus->uartBase,kUartIntTxComplete, true)
#define UART_TX_EXIT_CRITICAL()			//UART_HAL_SetIntMode((UART_Type*)pModbus->uartBase,kUartIntTxComplete, false)


#define Modbus_IsSendReady(pModbus)  \
									((pModbus)->uFlag.Bits.bStarted == TRUE && (pModbus)->uFlag.Bits.bSending == FALSE)

#define Modbus_IsSendDone(pModbus) 	(!(pModbus)->uFlag.Bits.bSending)

/************************** Function Prototypes ******************************/
inline void 		RS485_RX(SModbus *pModbus);
inline void 		RS485_TX(SModbus *pModbus);
inline void 		RS485_Init();
static void 		modbus_rx_handle(uint32_t instance, void * uartState);
static void 		modbus_tx_handle(uint32_t instance, void * uartState);
/************************** Variable Definitions *****************************/
uart_state_t modbus_uart_state;
static uint8_t rx_char, tx_char;
SModbus 			*pThisL1;
/*****************************************************************************/
/** @brief
 *		   init UART port
 *
 *  @param
 *  @return Void.
 *  @note
 */


void Modbus_Uart_Init(uint32_t uartInstance, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio)
{
	uart_user_config_t modbus_uart_cfg = {
					.baudRate = u32Baudrate,
					.parityMode = kUartParityDisabled,
					.stopBitCount = kUartTwoStopBit,
					.bitCountPerChar = kUart8BitsPerChar,
				};

	configure_uart_pins(BOARD_MODBUS_UART_INSTANCE);

	UART_Type * g_Base[UART_INSTANCE_COUNT] = UART_BASE_PTRS;
	UART_Type * base = g_Base[uartInstance];

	UART_DRV_Init(uartInstance, &modbus_uart_state, &modbus_uart_cfg);

	UART_DRV_InstallRxCallback(uartInstance, modbus_rx_handle, &rx_char, NULL, true);
	UART_DRV_InstallTxCallback(uartInstance, modbus_tx_handle, &tx_char, NULL);

	pThisL1->uartBase = base;
}

/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
void RS485_Init()
{
	GPIO_DRV_Init(NULL, mbRs485Pin);
}
/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
void RS485_TX(SModbus *pModbus)
{
	GPIO_DRV_SetPinOutput(pModbus->rs485Pin);
}
/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
void RS485_RX(SModbus *pModbus)
{
	uint32_t remain;
	/*wait until last byte is shifted out*/
	while(UART_DRV_GetTransmitStatus(pModbus->uartInstance, &remain) == kStatus_UART_TxBusy);
	GPIO_DRV_ClearPinOutput(pModbus->rs485Pin);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Modbus_SetSending (SModbus *pModbus, BOOL send) {
	pModbus->uFlag.Bits.bSending = send;
}
/*****************************************************************************/
/** @brief
 *		   Initialize Transmission Layer 1
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t  Modbus_Init(SModbus *pModbus, uint32_t uartInstance,
		uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio)
{
	pThisL1 = pModbus;
	//FIFO Queue to store received data from UART
	FIFO_Create(&pModbus->sRecvFIFO, pModbus->arrRecvFIFO, SIZE_FIFO_RECV);
	pModbus->rs485Pin = kGpioMbRs485;
	RS485_Init();
	RS485_TX(pModbus);	// do not want to receice any thing
	//init some data of Modbus
	pModbus->uartInstance	 	= uartInstance;
	pModbus->u32BaudRate 		= u32BaudRate;
	// init UART
	LREP("init uart port: %d\r\n", uartInstance);
	Modbus_Uart_Init(uartInstance, u32BaudRate, u8TxIntPrio, u8RxIntPrio);
	RS485_RX(pModbus);

	pModbus->uFlag.all = 0;
	pModbus->uFlag.Bits.bStarted = true;

	return MB_SUCCESS;
}

uint8_t	Modbus_SendAndRecv		(SModbus *pModbus, uint8_t *psData,
									 uint16_t sSize, uint8_t *prData,
									 uint16_t *rSize, uint16_t timeout) {
	Modbus_RecvFF_Reset(pModbus);
	uint8_t retVal;
	retVal = Modbus_Send(pModbus, psData, sSize);
	if(retVal == MB_SUCCESS) {
		OSA_SleepMs(timeout);
		uint32_t recvCount = Modbus_GetRecvCount(pModbus);
		if(recvCount > 0) {
			*rSize = Modbus_Recv(pModbus, prData, recvCount);
			if(*rSize <= 0) {
				retVal = MB_ERR_FIFO;
			}
		} else {
			retVal = MB_ERR_TIMEOUT;
		}
	}
	return retVal;
}
/*****************************************************************************/
/** @brief Modbus_Send
 *		   Try to send first byte of frame
 *
 *  @param
 *  @return BOOL.
 *  @note
 */
int Modbus_Send(SModbus *pModbus, uint8_t* pData, uint16_t u16Size) 
{

	ASSERT_NONVOID(pModbus != 0, MB_ERR_INVALID_PTR);
	ASSERT_NONVOID(u16Size <= SIZE_FIFO_SEND, MB_ERR_INVALID_DATA);
	ASSERT_NONVOID(Modbus_IsSendReady(pModbus), MB_ERR_BUSY);

	int i = 0;
	// copy data
	for(; i < u16Size; i++) {
		pModbus->pSendBuff[i] = pData[i];
	}

	pModbus->u16SendSize = u16Size;

	Modbus_SetSending(pModbus, TRUE);
	RS485_TX(pModbus);

	if(UART_DRV_SendData(pModbus->uartInstance,
						 pModbus->pSendBuff,
						 pModbus->u16SendSize) == kStatus_UART_Success) {
		return MB_SUCCESS;
	}

	return MB_FAILURE;
}
/*****************************************************************************/
/** @brief Modbus_Send
 *		   Try to receive a number of bytes from the receving FIFO.
 *
 *  @param
 *  @return actual number of received bytes
 *  @note
 */
int Modbus_Recv(SModbus *pModbus, uint8_t* pData, uint16_t u16Size) 
{
	uint16_t 	u16Idx;
	SFIFO 		*pFF;
	uint8_t  	u8Data;
	
	ASSERT_NONVOID(pModbus != 0, 0);

	pFF = &pModbus->sRecvFIFO;

	UART_RX_ENTER_CRITICAL();
	if(u16Size > FIFO_GetCount(pFF))
	{
		u16Size = FIFO_GetCount(pFF);
	}	
	UART_RX_EXIT_CRITICAL();
	
	for(u16Idx = 0; u16Idx < u16Size; u16Idx++) 
	{
		UART_RX_ENTER_CRITICAL();
		FIFO_Pop(pFF, &u8Data);
		UART_RX_EXIT_CRITICAL();
		
		*pData++ = u8Data;
	}
	
	return u16Size;
}

void Modbus_RecvFF_EnProtect(SModbus *pModbus, BOOL bEn) 
{
	ASSERT_VOID(pModbus != 0);

	UART_RX_ENTER_CRITICAL();
	if(bEn) FIFO_EnableProtect(&pModbus->sRecvFIFO);
	else    FIFO_DisableProtect(&pModbus->sRecvFIFO);
	UART_RX_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief 	Modbus_RecvFF_RewindHead
 *			Rewind head pointer to the protected value
 *  @param
 *  @return Void.
 *  @note
 */
void Modbus_RecvFF_RewindHead(SModbus *pModbus) 
{
	ASSERT_VOID(pModbus != 0);

	UART_RX_ENTER_CRITICAL();
	FIFO_RewindHead(&pModbus->sRecvFIFO);
	UART_RX_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief 	Modbus_RecvFF_Pop
 *			Try to pop one byte from the received FIFO
 *  @param
 *  @return value.
 *  @note
 */
BYTE Modbus_RecvFF_Pop(SModbus *pModbus) 
{
	BYTE b;

	ASSERT_NONVOID(pModbus != 0, 0xFF);

	UART_RX_ENTER_CRITICAL();
	FIFO_Pop(&pModbus->sRecvFIFO, &b);
	UART_RX_EXIT_CRITICAL();

	return b;
}

/*****************************************************************************/
/** @brief 	Modbus_GetRecvCount
 *			Get size of the receving FIFO
 *  @param
 *  @return size of FIFO.
 *  @note
 */
int Modbus_GetRecvCount(SModbus *pModbus) 
{
	ASSERT_NONVOID(pModbus != 0, 0);
	return FIFO_GetCount( &pModbus->sRecvFIFO );
}
/*****************************************************************************/
/** @brief 	Modbus_GetRecvCount
 *			Get size of the receving FIFO
 *  @param
 *  @return size of FIFO.
 *  @note
 */
void Modbus_RecvFF_Reset (SModbus *pModbus) {
	FIFO_Reset(&pModbus->sRecvFIFO);
}

/*****************************************************************************/
/** @brief RX_Interrupt
 *		   function for Rx handler
 *  @param
 *  @return Void.
 *  @note
 */

static void modbus_rx_handle(uint32_t instance, void * uartState) {
	uart_state_t *state = (uart_state_t*)uartState;

//	LREP("%02x ", state->rxBuff[0]);
	debug_putchar(state->rxBuff[0]);
//	if(FIFO_Push(&pThisL1->sRecvFIFO, state->rxBuff[0]) == FALSE) {
//		LREP("push fifo error \r\n");
//	}
}

/*****************************************************************************/
/** @brief TX_Interrupt
 *		   function for Tx handler
 *  @param
 *  @return Void.
 *  @note
 */
static void modbus_tx_handle(uint32_t instance, void * uartState) {
	uart_state_t *state = (uart_state_t*)uartState;
	if(state->txSize > 0) {
		state->txBuff++;
		state->txSize--;
		if(state->txSize == 0) {
			LREP("tx done ! ____________________\r\n");
			Modbus_SetSending(pThisL1, FALSE);
			RS485_RX(pThisL1);
		}
	}
}

/*****************************************************************************/




/***************************** Include Files *********************************/
#include <string.h>
#include "layer1.h"
#include <fsl_debug_console.h>
#include <fsl_uart_driver.h>
#if (TRANSL1_VER	== TRANSL1_V1)
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


#define UART_RX_ENTER_CRITICAL()     	UART_HAL_SetIntMode((UART_Type*)pTransL1->uartBase,kUartIntRxDataRegFull, false)
#define UART_RX_EXIT_CRITICAL()			UART_HAL_SetIntMode((UART_Type*)pTransL1->uartBase,kUartIntRxDataRegFull, true)

#define UART_TX_ENTER_CRITICAL()		UART_HAL_SetIntMode((UART_Type*)pTransL1->uartBase,kUartIntTxComplete, true)
#define UART_TX_EXIT_CRITICAL()			UART_HAL_SetIntMode((UART_Type*)pTransL1->uartBase,kUartIntTxComplete, false)

/************************** Function Prototypes ******************************/
extern void   		UART_SetIsrL1Handle(uint32_t uartInstance, STransL1 *pTransL1);
inline void 		RS485_RX(STransL1 *pTransL1);
inline void 		RS485_TX(STransL1 *pTransL1);
inline void 		RS485_Init();
static void 		modbus_rx_handle(uint32_t instance, void * uartState);
static void 		modbus_tx_handle(uint32_t instance, void * uartState);
/************************** Variable Definitions *****************************/
uart_state_t modbus_uart_state;
static uint8_t rx_char, tx_char;
STransL1 			*pThisL1;
/*****************************************************************************/
/** @brief
 *		   init UART port
 *
 *  @param
 *  @return Void.
 *  @note
 */


void TransL1_Uart_Init(uint32_t uartInstance, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio)
{
	uart_user_config_t modbus_uart_cfg = {
					.baudRate = u32Baudrate,
					.parityMode = kUartParityDisabled,
					.stopBitCount = kUartTwoStopBit,
					.bitCountPerChar = kUart8BitsPerChar,
				};

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
//	GPIO_DRV_Init()
}
/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
void RS485_TX(STransL1 *pTransL1)
{
	GPIO_DRV_SetPinOutput(pTransL1->rs485Pin);
}
/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
void RS485_RX(STransL1 *pTransL1)
{
	uint32_t remain;
	/*wait until last byte is shifted out*/
	while(UART_DRV_GetTransmitStatus(pTransL1->uartInstance, &remain) == kStatus_UART_TxBusy);

	GPIO_DRV_ClearPinOutput(pTransL1->rs485Pin);
}
/*****************************************************************************/
/** @brief
 *		   Initialize Transmission Layer 1
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t  TransL1_Init(STransL1 *pTransL1, uint32_t uartInstance,
		uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio)
{

	ASSERT_NONVOID(pTransL1 != 0, TRANS_ERR_PARAM);

	//FIFO Queue to store received data from UART
	FIFO_Create(&pTransL1->sRecvFIFO,pTransL1->arrRecvFIFO,SIZE_FIFO_RECV);

	RS485_Init();

	RS485_TX(pTransL1);	// do not want to receice any thing

	//init some data of TransL1
	pTransL1->uartInstance	 	= uartInstance;
	pTransL1->u32BaudRate 		= u32BaudRate;

	pTransL1->fClbL1SendDone 	= NULL;
	pTransL1->fClbL1RecvData	= NULL;
	pTransL1->fClbL1Error		= NULL;
	pTransL1->pClbSendDoneParam	= NULL;
	pTransL1->pClbRecvByteParam	= NULL;
	pTransL1->pClbErrorParam	= NULL;

	pTransL1->sFlag.u8All		= 0;

	//set handle for uart isr
	UART_SetIsrL1Handle(uartInstance,pTransL1);

	// init UART
	TransL1_Uart_Init(uartInstance, u32BaudRate, u8TxIntPrio, u8RxIntPrio);

	RS485_RX(pTransL1);//RS485_RECV_ENABLE;

	pTransL1->sFlag.Bits.bStarted = TRUE;
	pThisL1 = pTransL1;
	return SUCCESS;
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return Void.
 *  @note
 */
void TransL1_RegisterClbEvent(STransL1 *pTransL1, EL1Event event, FClbL1Event pFunction, void *pParam)
{
	switch(event)
	{
		case TRANSL1_EVT_NONE: 												
			break;
		case TRANSL1_EVT_SEND_DONE: 
			pTransL1->fClbL1SendDone 	= pFunction; 
			pTransL1->pClbSendDoneParam = pParam;
			break;
		case TRANSL1_EVT_RECV_BYTE:	
			pTransL1->fClbL1RecvData 	= pFunction; 
			pTransL1->pClbRecvByteParam = pParam;
			break;
		case TRANSL1_EVT_ERROR: 	
			pTransL1->fClbL1Error		= pFunction; 
			pTransL1->pClbErrorParam 	= pParam;
			break;
		default: 					
			ASSERT(FALSE); 							
			break;
	}	
}	
/*****************************************************************************/
/** @brief TransL1_Stop
 *		   Stop the TransL1 service
 *
 *  @param
 *  @return Void.
 *  @note
 */
void TransL1_Stop(STransL1 *pTransL1) 
{
	ASSERT_VOID(pTransL1 != 0);

	pTransL1->sFlag.Bits.bStarted = FALSE;

	//UART_DisableInt(pTransL1->uartInstance,UART_INT_TX | UART_INT_RX);

	FIFO_Destroy(&pTransL1->sRecvFIFO);
}

/*****************************************************************************/
/** @brief TransL1_IsSendReady
 *		   Check if ready to send next datat or not
 *
 *  @param
 *  @return BOOL.
 *  @note
 */
BOOL TransL1_IsSendReady(STransL1 *pTransL1) 
{
	ASSERT_NONVOID(pTransL1 != 0, FALSE);

	return (pTransL1->sFlag.Bits.bStarted == TRUE && pTransL1->sFlag.Bits.bSending == FALSE);
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return BOOL.
 *  @note
 */
BOOL TransL1_IsReceiving(STransL1 *pTransL1)
{
	BOOL bRet;
	
	UART_RX_ENTER_CRITICAL();
	bRet = pTransL1->sFlag.Bits.bNewByte;
	UART_RX_EXIT_CRITICAL();
	
	return bRet;
}
/*****************************************************************************/
/** @brief 
 *		   
 *
 *  @param
 *  @return BOOL.
 *  @note
 */	
void TransL1_ClearNewByteFlag(STransL1 *pTransL1)
{
	UART_RX_ENTER_CRITICAL();
	pTransL1->sFlag.Bits.bNewByte = FALSE;
	UART_RX_EXIT_CRITICAL();
}	
/*****************************************************************************/
/** @brief TransL1_Send
 *		   Try to send first byte of frame
 *
 *  @param
 *  @return BOOL.
 *  @note
 */
int TransL1_Send(STransL1 *pTransL1, uint8_t* pData, uint16_t u16Size) 
{

	ASSERT_NONVOID(pTransL1 != 0, TRANS_ERR_INVALID_PTR);

	if(pTransL1->sFlag.Bits.bStarted == FALSE)
	{
		ASSERT(FALSE);
		return TRANS_ERR_NOT_STARTED;
	}

	if(pTransL1->sFlag.Bits.bSending == TRUE)
	{
		ASSERT(FALSE);
		return TRANS_ERR_BUSY;
	}

	if(pData == NULL || u16Size <= 0)
	{
		ASSERT(FALSE);
		return TRANS_ERR_PARAM;
	}

	UART_TX_ENTER_CRITICAL();
		pTransL1->pSendBuff   = pData;
		pTransL1->u16SendSize = u16Size;
		pTransL1->u16SendPtr  = 0;
	UART_TX_EXIT_CRITICAL();

	RS485_TX(pTransL1);

//        UART_RX_ENTER_CRITICAL();

	pTransL1->sFlag.Bits.bSending = TRUE;


	UART_TX_ENTER_CRITICAL();

//	uint8_t u8Send = pTransL1->pSendBuff[u16NumSent++];
//	pTransL1->u16SendPtr += u16NumSent;

	UART_TX_EXIT_CRITICAL();

	//wait until there is any space in Tx buffer

	UART_DRV_SendData(pTransL1->uartInstance, pTransL1->pSendBuff, u16Size);

	return SUCCESS;

}
/*****************************************************************************/
/** @brief TransL1_Send
 *		   Try to receive a number of bytes from the receving FIFO.
 *
 *  @param
 *  @return actual number of received bytes
 *  @note
 */
int TransL1_Recv(STransL1 *pTransL1, uint8_t* pData, uint16_t u16Size) 
{
	uint16_t u16Idx;
	SFIFO *pFF;
	uint8_t  u8Data;
	
	ASSERT_NONVOID(pTransL1 != 0, 0);

	pFF = &pTransL1->sRecvFIFO;

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
/*****************************************************************************/
/** @brief 	TransL1_RecvFF_EnProtect
 *			Enable/Disable protection mode for the receving FIFO.
 *  @param
 *  @return Void.
 *  @note
 */
void TransL1_RecvFF_EnProtect(STransL1 *pTransL1, BOOL bEn) 
{
	ASSERT_VOID(pTransL1 != 0);

	UART_RX_ENTER_CRITICAL();
	if(bEn) FIFO_EnableProtect(&pTransL1->sRecvFIFO);
	else    FIFO_DisableProtect(&pTransL1->sRecvFIFO);
	UART_RX_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief 	TransL1_RecvFF_RewindHead
 *			Rewind head pointer to the protected value
 *  @param
 *  @return Void.
 *  @note
 */
void TransL1_RecvFF_RewindHead(STransL1 *pTransL1) 
{
	ASSERT_VOID(pTransL1 != 0);

	UART_RX_ENTER_CRITICAL();
	FIFO_RewindHead(&pTransL1->sRecvFIFO);
	UART_RX_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief 	TransL1_RecvFF_Pop
 *			Try to pop one byte from the received FIFO
 *  @param
 *  @return value.
 *  @note
 */
BYTE TransL1_RecvFF_Pop(STransL1 *pTransL1) 
{
	BYTE b;

	ASSERT_NONVOID(pTransL1 != 0, 0xFF);

	UART_RX_ENTER_CRITICAL();
	FIFO_Pop(&pTransL1->sRecvFIFO, &b);
	UART_RX_EXIT_CRITICAL();

	return b;
}

/*****************************************************************************/
/** @brief 	TransL1_GetRecvCount
 *			Get size of the receving FIFO
 *  @param
 *  @return size of FIFO.
 *  @note
 */
int TransL1_GetRecvCount(STransL1 *pTransL1) 
{
	ASSERT_NONVOID(pTransL1 != 0, 0);

	return FIFO_GetCount( &pTransL1->sRecvFIFO );
}
/*****************************************************************************/
/** @brief 	TransL1_GetRecvCount
 *			Get size of the receving FIFO
 *  @param
 *  @return size of FIFO.
 *  @note
 */
void TransL1_RecvFF_Reset (STransL1 *pTransL1) {
	FIFO_Reset(&pTransL1->sRecvFIFO);
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

	if(FIFO_Push(&pThisL1->sRecvFIFO, state->rxBuff[0]) == FALSE) {
		LREP("push fifo error \r\n");
	}
}

/*****************************************************************************/
/** @brief TX_Interrupt
 *		   function for Tx handler
 *  @param
 *  @return Void.
 *  @note
 */
static void modbus_tx_handle(uint32_t instance, void * uartState) {

}

/*****************************************************************************/
#endif //#if (TRANSL1_VER	== TRANSL1_V1)



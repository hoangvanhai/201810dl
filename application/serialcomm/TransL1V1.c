
/***************************** Include Files *********************************/
#include <string.h>
#include "TransL1V1.h"
#include <fsl_debug_console.h>
#include <fsl_uart_driver.h>
#include <gpio_pins.h>
#include <board.h>


#if (TRANSL1_VER	== TRANSL1_V1)
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


#define UART_RX_ENTER_CRITICAL()     //UART_DisableInt(pTransL1->u32UartPort,UART_INT_RX)
#define UART_RX_EXIT_CRITICAL()		//UART_EnableInt(pTransL1->u32UartPort,UART_INT_RX)

#define UART_TX_ENTER_CRITICAL()	//UART_DisableInt(pTransL1->u32UartPort,UART_INT_TX)
#define UART_TX_EXIT_CRITICAL()		//UART_EnableInt(pTransL1->u32UartPort,UART_INT_TX)

/************************** Function Prototypes ******************************/
extern void   		UART_SetIsrL1Handle(uint32_t u32UartPort, STransL1 *pTransL1);
inline void 		RS485_RX(STransL1 *pTransL1);
inline void 		RS485_TX(STransL1 *pTransL1);
inline void 		RS485_Init();
/************************** Variable Definitions *****************************/

uart_state_t trans_uart_state;
static uint8_t rx_char, tx_char;
STransL1 *pThisTransL1;

/*****************************************************************************/
/** @brief
 *		   init UART port
 *
 *  @param
 *  @return Void.
 *  @note
 */


void TransL1_UARTInit(uint32_t u32UartPort, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio)
{
	uart_user_config_t trans_uart_cfg = {
					.baudRate = u32Baudrate,
					.parityMode = kUartParityDisabled,
					.stopBitCount = kUartTwoStopBit,
					.bitCountPerChar = kUart8BitsPerChar,
				};

	configure_uart_pins(BOARD_TRANSPC_UART_INSTANCE);

	UART_DRV_Init(u32UartPort, &trans_uart_state, &trans_uart_cfg);

	UART_DRV_InstallRxCallback(u32UartPort, TransL1_RX_Interrupt_Handle, &rx_char, NULL, true);
	UART_DRV_InstallTxCallback(u32UartPort, TransL1_TX_Interrupt_Handle, &tx_char, NULL);
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
	/*To Transceiver*/
	#if (HW_BOARD_SELECTED == HW_BOARD_UI_VRU812_V2)
//	CONFIG_RD0_AS_DIG_OUTPUT(); // DE of Transceiver RS485 driver 
	#elif (HW_BOARD_SELECTED == HW_BOARD_UI_VRU812_V1)
    CONFIG_RD0_AS_DIG_OUTPUT(); // DE of Transceiver RS485 driver 
	#endif
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
    /*Config RS485 Driver as transmiter*/

//    if(pTransL1->sRS485DE.u16Port != NULL)
//    {
//        BIT_SET(*(volatile unsigned int *)pTransL1->sRS485DE.u16Port, pTransL1->sRS485DE.u16Pin);
//    }
	
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
//	/*wait until last byte is shifted out*/
//	while(UART_IsBusy(pTransL1->u32UartPort));
//
//	/*Config RS485 Driver as receiver*/
//	if(pTransL1->sRS485DE.u16Port != NULL)
//	{
//		ASSERT(pTransL1->sRS485DE.u16Port == LATF_BASE);
//
//		BIT_CLEAR(*(volatile unsigned int *)pTransL1->sRS485DE.u16Port,	pTransL1->sRS485DE.u16Pin);
//	}
	
}
/*****************************************************************************/
/** @brief
 *		   Initialize Transmission Layer 1
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t  TransL1_Init(STransL1 *pTransL1, uint32_t u32UartPort, uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio)
{

	ASSERT_NONVOID(pTransL1 != 0, TRANS_ERR_PARAM);

	//clear all data
	#ifdef DEBUG_TRANSL1
	memset(&pTransL1->sDBG, 0, sizeof(STransL1DBG));
	#endif
	
	pThisTransL1 = pTransL1;
	//FIFO Queue to store received data from UART
	FIFO_Create(&pTransL1->sRecvFIFO,pTransL1->arrRecvFIFO,SIZE_FIFO_RECV);
	
	RS485_Init();
	
	RS485_TX(pTransL1);	// do not want to receice any thing
	
	//init some data of TransL1
	pTransL1->u32UartPort	 	= u32UartPort;
	pTransL1->u32BaudRate 		= u32BaudRate;
	
	pTransL1->fClbL1SendDone 	= NULL;
	pTransL1->fClbL1RecvData	= NULL;
	pTransL1->fClbL1Error		= NULL;
	pTransL1->pClbSendDoneParam	= NULL;
	pTransL1->pClbRecvByteParam	= NULL;
	pTransL1->pClbErrorParam	= NULL;

	pTransL1->sFlag.u8All		= 0;
	
	//set handle for uart isr

	// init UART
	TransL1_UARTInit(u32UartPort, u32BaudRate, u8TxIntPrio, u8RxIntPrio);
		
	RS485_RX(pTransL1);
	
	pTransL1->sFlag.Bits.bStarted = TRUE;
	return TRANS_SUCCESS;
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

	//UART_DisableInt(pTransL1->u32UartPort,UART_INT_TX | UART_INT_RX);

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
	ASSERT_NONVOID(pTransL1->sFlag.Bits.bStarted == TRUE, TRANS_ERR_NOT_STARTED);
	ASSERT_NONVOID(pTransL1->sFlag.Bits.bSending == FALSE, TRANS_ERR_BUSY);
	ASSERT_NONVOID(pData != NULL, TRANS_ERR_PARAM);
	
	UART_TX_ENTER_CRITICAL();
		pTransL1->pSendBuff   = pData;
		pTransL1->u16SendSize = u16Size;
		pTransL1->u16SendPtr  = 0;
	UART_TX_EXIT_CRITICAL();

	RS485_TX(pTransL1);

	pTransL1->sFlag.Bits.bSending = TRUE;
	
	for(int i = 0; i < u16Size; i++) {
		LREP("%02x ", pData[i]);
	}

	if(UART_DRV_SendData(pTransL1->u32UartPort,
			pTransL1->pSendBuff,
			pTransL1->u16SendSize) == kStatus_UART_Success) {
		return TRANS_SUCCESS;
	}

	pTransL1->sFlag.Bits.bSending = FALSE;
	return TRANS_FAILURE;
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
inline void TransL1_RX_Interrupt_Handle(uint32_t instance, void* uartState)
{
	uart_state_t *state = (uart_state_t*)uartState;

	if(FIFO_Push(&pThisTransL1->sRecvFIFO, state->rxBuff[0]) == FALSE) {
		LREP("push fifo error \r\n");
	}

	/* Callback function, signal to run the trans thread*/
	if(pThisTransL1->fClbL1RecvData && pThisTransL1->pClbRecvByteParam)
	{
		pThisTransL1->fClbL1RecvData(pThisTransL1->pClbRecvByteParam);
	}
}
/*****************************************************************************/
/** @brief TX_Interrupt
 *		   function for Tx handler
 *  @param
 *  @return Void.
 *  @note
 */
inline void TransL1_TX_Interrupt_Handle(uint32_t instance, void* uartState)
{
	uart_state_t *state = (uart_state_t*)uartState;
	if(state->txSize > 0) {
		state->txBuff++;
		state->txSize--;
		if(state->txSize == 0) {
			RS485_RX(pThisTransL1);
			pThisTransL1->sFlag.Bits.bSending = FALSE;
			if(pThisTransL1->fClbL1SendDone && pThisTransL1->pClbSendDoneParam) {
				pThisTransL1->fClbL1SendDone(pThisTransL1->pClbSendDoneParam);
			}
		}
	}
}
/*****************************************************************************/
/** @brief TransL1_PrintLog
 *		   report TransL1 statistic
 *  @param
 *  @return Void.
 *  @note
 */
#ifdef DEBUG_TRANSL1
	void TransL1_PrintLog(STransL1 *pTransL1)
	{
		LREP("[------------------------------------]\r\n");

		LREP("[L1: START]\r\n");

		//LREP("L1: u32UARTFramingError = %d\r\n", 	pTransL1->sDBG.u32UARTFramingError);
		//LREP("L1: u32UARTParityError = %d\r\n", 	pTransL1->sDBG.u32UARTParityError);
		//LREP("L1: u32UARTOverRun = %d\r\n", 		pTransL1->sDBG.u32UARTOverRun);

		LREP("L1: u32RxInterrupt = %d\r\n", 		pTransL1->sDBG.u32RxInterrupt);
		LREP("L1: u32TxInterrupt = %d\r\n", 		pTransL1->sDBG.u32TxInterrupt);

		LREP("L1: u32FIFOFull = %d\r\n", 			pTransL1->sDBG.u32FIFOFull);
		LREP("L1: u32FIFOPushOK = %d\r\n", 		pTransL1->sDBG.u32FIFOPushOK);
		LREP("L1: u32FIFOPushNotOK = %d\r\n", 		pTransL1->sDBG.u32FIFOPushNotOK);

		LREP("L1: u32SendedBytes = %d\r\n", 		pTransL1->sDBG.u32SendedBytes);
		LREP("L1: u32RecvBytes = %d\r\n", 			pTransL1->sDBG.u32RecvBytes);

	
		LREP("[L1: END]\r\n");
	}

#endif

/*****************************************************************************/
#endif //#if (TRANSL1_VER	== TRANSL1_V1)



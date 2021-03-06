
/***************************** Include Files *********************************/

#include <string.h>
#include "typedefs.h"
#include "TransL1V2.h"
#include "hardware_profile.h"
#include "fsl_debug_console.h"

#if (TRANSL1_VER	== TRANSL1_V2)

/************************** Constant Definitions *****************************/
#define DMA_TXUART_CONFIG			DMA_CH_DIS | DMA_SIZE_BYTE | DMA_DIR_WRITE_PERIPHERAL | \
                        			DMA_INTERRUPT_FULL | DMA_AMODE_REGISTER_POSTINC | DMA_MODE_ONE_SHOT
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


#define UART_RX_ENTER_CRITICAL()	//UART_DisableInt(pTransL1->u32UartPort,UART_INT_RX)
#define UART_RX_EXIT_CRITICAL()		//UART_EnableInt(pTransL1->u32UartPort,UART_INT_RX)

#define UART_TX_ENTER_CRITICAL()	//DMA_DisableIntr(pTransL1->u16TxDMABase)
#define UART_TX_EXIT_CRITICAL()		//DMA_EnableIntr(pTransL1->u16TxDMABase)

/************************** Function Prototypes ******************************/

extern void   UART_SetIsrL1Handle(uint16_t u32UartPort, STransL1 *pTransL1);

/************************** Variable Definitions *****************************/

uint8_t u8DMAUART1TxBuffer[FRM_MAX_SIZE] /*For UART1*/
uint8_t u8DMAUART2TxBuffer[FRM_MAX_SIZE] /*For UART2*/
/*****************************************************************************/
/** @brief
 *		   init UART port
 *
 *  @param
 *  @return Void.
 *  @note
 */

static void UART_Init(uint16_t u32UartPort, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio)
{
	/* Config UART mode
	 * FCY, DEFAULT_BAUDRATE, DEFAULT_UARTMODE see in app_cfg.h
	 */
	UART_SetConfigMode(u32UartPort, FCY, u32Baudrate, DEFAULT_UARTMODE);
	
	/*Set interrupt mode for TX and RX*/
	UART_SetFIFOLevel(u32UartPort,UART_INT_TX_BUF_EMPTY,UART_INT_RX_CHAR);
	
	/*Enable interrupt*/
	//UART_EnableInt(u32UartPort, UART_INT_TX | UART_INT_RX);
	UART_EnableInt(u32UartPort, UART_INT_RX);
	
	/*Set interrupt priority*/
	UART_SetIntIP(u32UartPort,UART_INT_TX_IP, u8TxPrio);
	UART_SetIntIP(u32UartPort,UART_INT_RX_IP, u8RxPrio);	
	
	//LREP(" - UART = 0x%x",(DWORD)u32UartPort);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void DMA_Init(STransL1 *pTransL1, uint16_t u32UartPort, uint8_t u8TxIntPrio)
{
	DMA_SetConfigMode(pTransL1->u16TxDMABase, DMA_TXUART_CONFIG);
				
	if(u32UartPort == UART1_BASE)
	{
		DMA_SetBufferA(pTransL1->u16TxDMABase, u8DMAUART1TxBuffer);
		DMA_SetIRQ(pTransL1->u16TxDMABase, DMA_IRQ_U1TX);
		DMA_SetPeripheral(pTransL1->u16TxDMABase, &U1TXREG);
	}	
	else if(u32UartPort == UART2_BASE)
	{
		DMA_SetBufferA(pTransL1->u16TxDMABase, u8DMAUART2TxBuffer);
		DMA_SetIRQ(pTransL1->u16TxDMABase, DMA_IRQ_U2TX);
		DMA_SetPeripheral(pTransL1->u16TxDMABase, &U2TXREG);
	}	
	
	DMA_SetTransferCount(pTransL1->u16TxDMABase, FRM_MAX_SIZE);
	
	/*Set Interrupt Priority*/
	DMA_SetIntrPrio(pTransL1->u16TxDMABase, u8TxIntPrio);
	
	/*Enable Interrupt*/
	DMA_EnableIntr(pTransL1->u16TxDMABase);
	
	/*Enable Channel, CHEN = 1*/
	DMA_EnableChannel(pTransL1->u16TxDMABase);
	
	DMA_SetInterruptMode(pTransL1->u16TxDMABase,DMA_INTERRUPT_FULL);

	/*
	LREP("\r\nDMA_Base = 0x%x",(uint32_t)pTransL1->u16TxDMABase);
	LREP("\r\nDMA - CON = 0x%x",(uint32_t)DMA_GetConfigMode(pTransL1->u16TxDMABase));
	LREP("\r\nDMA - IRQ = 0x%x",(uint32_t)DMA_GetIRQ(pTransL1->u16TxDMABase));
	LREP("\r\nDMA - PAD = 0x%x",(uint32_t)DMA_GetPeripheral(pTransL1->u16TxDMABase));
	LREP("\r\nDMA - CNT = %d",  (uint32_t)DMA_GetTransferCount(pTransL1->u16TxDMABase));
	//*/
}	
static inline void DMA_ChangeSize(STransL1 *pTransL1,uint16_t u16Size)
{
	DMA_DisableChannel(pTransL1->u16TxDMABase);
	DMA_SetTransferCount(pTransL1->u16TxDMABase, u16Size);
	DMA_EnableChannel(pTransL1->u16TxDMABase);
} 
/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline void RS485_Init()
{

}
/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline void RS485_TX(STransL1 *pTransL1)
{
	/*Config RS485 Driver as transmiter*/
	if(pTransL1->sRS485DE.u16Port != NULL)
	{
		ASSERT(pTransL1->sRS485DE.u16Port == LATF_BASE);
		
		BIT_SET(*(volatile unsigned int *)pTransL1->sRS485DE.u16Port, \
				 pTransL1->sRS485DE.u16Pin);	
	}
	
}
	
/*****************************************************************************/
/** @brief
 *		  
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline void RS485_RX(STransL1 *pTransL1)
{
	/*wait until last byte is shifted out*/
	while(UART_IsBusy(pTransL1->u32UartPort));
	
	/*Config RS485 Driver as receiver*/
	if(pTransL1->sRS485DE.u16Port != NULL)
	{
		ASSERT(pTransL1->sRS485DE.u16Port == LATF_BASE);
		
		BIT_CLEAR(*(volatile unsigned int *)pTransL1->sRS485DE.u16Port,	\
					pTransL1->sRS485DE.u16Pin);
	}
	
}
/*****************************************************************************/
/** @brief
 *		   Initialize Transmission Layer 1
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t  TransL1_Init(STransL1 *pTransL1, uint16_t u32UartPort, uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio,uint16_t u16TxDMABase)
{

	ASSERT_NONVOID(pTransL1 != 0, TRANS_ERR_PARAM);

	//clear all data
	#ifdef DEBUG_TRANSL1
	memset(&pTransL1->sDBG, 0, sizeof(STransL1DBG));
	#endif
	
	//FIFO Queue to store received data from UART
	FIFO_Create(&pTransL1->sRecvFIFO,pTransL1->arrRecvFIFO,SIZE_FIFO_RECV);
	
	RS485_Init();
	
	RS485_TX(pTransL1);	// do not want to receice any thing
	
	//init some data of TransL1
	pTransL1->u32UartPort	 	= u32UartPort;
	pTransL1->u32BaudRate 		= u32BaudRate;
	
	pTransL1->u16TxDMABase		= u16TxDMABase;
	
	
	pTransL1->fClbL1SendDone 	= NULL;
	pTransL1->fClbL1RecvData	= NULL;
	pTransL1->fClbL1Error		= NULL;
	pTransL1->pClbSendDoneParam	= NULL;
	pTransL1->pClbRecvByteParam	= NULL;
	pTransL1->pClbErrorParam	= NULL;

	pTransL1->sFlag.u8All		= 0;
	
	//set handle for uart isr
	UART_SetIsrL1Handle(u32UartPort,pTransL1);
	
	// init UART
	UART_Init(u32UartPort, u32BaudRate, u8TxIntPrio, u8RxIntPrio);
	
	//init DMA
	DMA_Init(pTransL1, u32UartPort, u8TxIntPrio);
		
	RS485_RX(pTransL1);//RS485_RECV_ENABLE;
	
	pTransL1->sFlag.Bits.bStarted = TRUE;
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

	UART_DisableInt(pTransL1->u32UartPort,UART_INT_TX | UART_INT_RX);

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
	uint8_t	 u8Idx;

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
	

	RS485_TX(pTransL1);

	pTransL1->sFlag.Bits.bSending = TRUE;

	if(pTransL1->u32UartPort == UART1_BASE)
	{
		for(u8Idx = 0; u8Idx < u16Size; u8Idx++)
		{
			u8DMAUART1TxBuffer[u8Idx] = pData[u8Idx];
		}
	}
	else
	{
		for(u8Idx = 0; u8Idx < u16Size; u8Idx++)
		{
			u8DMAUART2TxBuffer[u8Idx] = pData[u8Idx];
		}
	}	
	DMA_ChangeSize(pTransL1, u16Size);
	
	DMA_ForceSingleTransfer(pTransL1->u16TxDMABase);
	
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
/** @brief RX_Interrupt
 *		   function for Rx handler
 *  @param
 *  @return Void.
 *  @note
 */
inline void TransL1_RX_Interrupt_Handle(void* pParam) 
{
	STransL1 *pTransL1 = (STransL1 *)pParam;
	SFIFO	 *pRecvFF  = &pTransL1->sRecvFIFO;
	uint8_t    u8Recv;

	//20130315 - Test
	#ifdef DEBUG
	if(FIFO_IsEnablePush(pRecvFF) == FALSE){
		L1DBG_INC(u32FIFOFull);
	}
	#endif
	
	/* Push data into FIFO */
	while((UART_IsCharsAvail(pTransL1->u32UartPort) == TRUE) && 	\
		  (FIFO_IsEnablePush(pRecvFF) == TRUE) )
	{
		u8Recv = UART_GetChar(pTransL1->u32UartPort);

		L1DBG_INC(u32RecvBytes);

		if(FIFO_Push(pRecvFF, u8Recv) == TRUE)
			L1DBG_INC(u32FIFOPushOK);
		else
			L1DBG_INC(u32FIFOPushNotOK);
	}
	
	//pTransL1->sFlag.Bits.bNewByte = TRUE;
	
	/* Callback function */
	if(pTransL1->fClbL1RecvData) 
	{
		pTransL1->fClbL1RecvData(pTransL1->pClbRecvByteParam);
	}
}
/*****************************************************************************/
/** @brief TX_Interrupt
 *		   function for Tx handler
 *  @param
 *  @return Void.
 *  @note
 */
inline void TransL1_TX_Interrupt_Handle(void* pParam) 
{
	STransL1 *pTransL1 = (STransL1 *)pParam;

	if(pTransL1->sFlag.Bits.bSending) 
	{
		/* Finished	*/
		if(pTransL1->u16SendPtr >= pTransL1->u16SendSize) 
		{				
//			RS485_RX(pTransL1);
			
			pTransL1->sFlag.Bits.bSending = FALSE;
			
			if(pTransL1->fClbL1SendDone) 
			{
				pTransL1->fClbL1SendDone(pTransL1->pClbSendDoneParam);
			}
		}
		/* Keep sending next bytes	*/
		else {

			while( (UART_IsSpaceAvail(pTransL1->u32UartPort) == TRUE) &&	\
				   pTransL1->u16SendPtr < pTransL1->u16SendSize ) 
			{

				uint8_t u8Send = pTransL1->pSendBuff[pTransL1->u16SendPtr++];

				UART_PutChar(pTransL1->u32UartPort,u8Send);

				L1DBG_INC(u32SendedBytes);
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
#endif //#if (TRANSL1_VER	== TRANSL1_V2)



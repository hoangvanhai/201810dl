
/***************************** Include Files *********************************/

#include "TransL2S.h"
#include "checksum.h"


//#if BOARD == BOARD_RADIO_ADAPTER || BOARD == BOARD_RADIO_ADAPTER

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

//static BOOL  			StartSendFrame	(STransL2S *pTransL2);
static BOOL  			ProcessRecvData	(STransL2S *pTransL2);
static EL2RecvAction    OnRecvFrame	(STransL2S *pTransL2);
static void  			RunSendFrame	(STransL2S *pTransL2);
static void  			L2FireEvent		(STransL2S *pTransL2, EL2Event eEventCode);

static void 			InitL1TimeOutTimer	(STransL2S *pTransL2);
static void 			StartL1TimeOutTimer	(STransL2S *pTransL2, uint32_t u32TimeOut);
static void 			StopL1TimeOutTimer	(STransL2S *pTransL2);
static void				Clb_L1TimeOutTimer	(void *timer, void *pClbParam);

/************************** Variable Definitions *****************************/


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static BOOL TransL2S_MemAlloc(STransL2S* pTransL2)
{
    //LREP("\r\nL2 Malloc, Mem_Size %d",(uint32_t)Mem_GetAvailableLargeSpace());
    SFrameInfo *pFrameInfo;
    pTransL2->sMemRecv 	= (SMem *)Mem_Alloc(LARGE_MEM_SIZE);

    ASSERT(pTransL2->sMemRecv != NULL);

    if(pTransL2->sMemRecv == NULL)
    {
        pTransL2->u8ErrorCode = TRANS_ERR_MEM;
        pTransL2->frmRecv	  = NULL;

        L2FireEvent(pTransL2, TRANSL2_EVT_ERROR);

        return FALSE;
    }

    pFrameInfo				= (SFrameInfo *)MEM_BODY(pTransL2->sMemRecv);
    pTransL2->frmRecv		= (uint8_t*)pFrameInfo + sizeof(SFrameInfo);
    pFrameInfo->pFrame		= pTransL2->frmRecv;
    pFrameInfo->pu8Data		= &pTransL2->frmRecv[IDX_SFRM_DATA0];
    return TRUE;
}	
/*****************************************************************************/
/** @brief
 *			Callback function for Layer 1, it is called when Layer 1 finishes sending a frame
 *
 *  @param
 *  @return Void.
 *  @note
 */

static void Clb_L1SendDone(LPVOID pParam) 
{
	OS_ERR err;
    STransL2S *pTransL2 = (STransL2S *)pParam;
//    STransL1 *pTransL1 = &pTransL2->sTransL1;

    ASSERT_VOID(pParam);

    pTransL2->sFlag.Bits.bL1Sending = FALSE;

    L2DBG_INC(nL1SendDone);

    /*Stop L1 TimeOut Timer*/
    StopL1TimeOutTimer(pTransL2);

    /*Signal to run Trans thread*/
    OSTaskSemPost((OS_TCB*)pTransL2->hSem, OS_OPT_NONE, &err);

}

/*****************************************************************************/
/** @brief
 *			Callback function for Layer 1, it is called when Layer 1 received a byte from UART
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void Clb_L1RecvData(LPVOID pParam) 
{
	OS_ERR err;
    STransL2S *pTransL2 = (STransL2S *)pParam;

    ASSERT_VOID(pParam);

    L2DBG_INC(nL1RecvData);
       
    /*Signal to run Trans thread*/
    OSTaskSemPost((OS_TCB*)pTransL2->hSem, OS_OPT_NONE, &err);
}
	
/*****************************************************************************/
/** @brief
 *			To check Timeout of sending of Layer 1. It is called by a Timeout timer
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void Clb_L1TimeOutTimer(void *timer, void *pParam)
{
    STransL2S *pTransL2 = (STransL2S *)pParam;
    uint8_t*   pSendFrm  = pTransL2->frmSend;

    ASSERT(FALSE);

    StopL1TimeOutTimer(pTransL2);

    if(pTransL2->sFlag.Bits.bL1Sending)
    {
        pTransL2->sFlag.Bits.bL1Sending      = FALSE;
        pTransL2->sFlag.Bits.bWaitingL1Send  = FALSE;
        pTransL2->sFlag.Bits.bSending        = FALSE;
        pTransL2->u8ErrorCode                = TRANS_ERR_L1_TIMEOUT;

        L2DBG_INC(nSendTimeout1);

        /*Layer 1 timeout*/
        if(IS_DATA_FRM(pSendFrm[IDX_SFRM_CTRL]))
        {
            /* DATA.con to higher layer
             * Sended frame is Data frame (Data.req) from higher layer
             */
            L2FireEvent(pTransL2, TRANSL2_EVT_ERROR);
        }
        else
        {
            /*Sended frame is Command frame -> free buffer internally*/
            Mem_Free(pTransL2->sMemSend);

            //Do not need to indicate to higher layer
            ASSERT(FALSE);
        }
    }
    else
    {
        ASSERT(FALSE);
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
inline void InitL1TimeOutTimer(STransL2S *pTransL2)
{
	OS_ERR err;
	OSTmrCreate(&pTransL2->hL1TimeOutTimer,
				(CPU_CHAR *)"timer",
				(OS_TICK)0,
				(OS_TICK)100,
				(OS_OPT)OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) Clb_L1TimeOutTimer,
				(void*)pTransL2,
				(OS_ERR*)&err);

}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
inline void StartL1TimeOutTimer(STransL2S *pTransL2, uint32_t u32TimeOut)
{
    //Timer_SetRate(pTransL2->hL1TimeOutTimer,(uint32_t)u32TimeOut * SYSTICK_PER_SECOND/1000);
	OS_ERR err;
	OSTmrStart(&pTransL2->hL1TimeOutTimer, &err);
}	
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void StopL1TimeOutTimer(STransL2S *pTransL2)
{
	OS_ERR err;
    OSTmrStop(&pTransL2->hL1TimeOutTimer, OS_OPT_NONE, (void*)NULL, &err);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void TransL2S_SetL1Para(STransL2S *pTransL2, uint32_t u32UartPort, uint32_t u32BaudRate, int32_t u16RS485Port, int32_t u16RS485Pin)
{
    STransL1 *pTransL1 = &pTransL2->sTransL1;

    ASSERT_VOID(pTransL2);

    pTransL1->u32UartPort		= u32UartPort;
    pTransL1->u32BaudRate		= u32BaudRate;

    pTransL1->sRS485DE.u16Port 	= u16RS485Port;
    pTransL1->sRS485DE.u16Pin  	= u16RS485Pin;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
#if (TRANSL1_VER == TRANSL1_V1)
ETransErrorCode TransL2S_Init(STransL2S *pTransL2, uint8_t u8NodeID, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio, void* hSem)
#elif (TRANSL1_VER == TRANSL1_V2)
ETransErrorCode TransL2S_Init(STransL2S *pTransL2, uint8_t u8NodeID, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio, void* hSem, uint16_t u16TxDMABase)
#endif
{
    STransL1 *pTransL1 = &pTransL2->sTransL1;
    ETransErrorCode eErrorCode;

    ASSERT_NONVOID(pTransL2, FALSE);

    /*-------------------------------
     * Initialize L2
     *-------------------------------*/
    pTransL2->u8NodeID 		= u8NodeID;
    pTransL2->u16RecvCount	= 0;
    pTransL2->u8SeqRemote	= 0xFF;
    pTransL2->u8SeqLocal	= 0x00;
    pTransL2->u8ErrorCode	= TRANS_SUCCESS;
    pTransL2->sFlag.u8All   = 0;
    pTransL2->sFlag.Bits.bCheckSeqNum	= TRUE;

    pTransL2->fClbL2SendDone= NULL;
    pTransL2->fClbL2RecvData= NULL;
    pTransL2->fClbL2Error	= NULL;

    pTransL2->hSem    		= hSem;

    /*allocate a buffer for receiving a frame*/
    TransL2S_MemAlloc(pTransL2);

    /*-------------------------------
     * Initialize L1
     *-------------------------------*/

    #if (TRANSL1_VER == TRANSL1_V1)
    eErrorCode = TransL1_Init(pTransL1, pTransL1->u32UartPort, pTransL1->u32BaudRate, u8TxIntPrio, u8RxIntPrio);
    #elif (TRANSL1_VER == TRANSL1_V2)
    eErrorCode = TransL1_Init(pTransL1, pTransL1->u32UartPort, pTransL1->u32BaudRate, u8TxIntPrio, u8RxIntPrio, u16TxDMABase);
    #endif

    if(eErrorCode != TRANS_SUCCESS)
    {
		pTransL2->u8ErrorCode = eErrorCode;
		return TRANS_FAILURE;
    }

    TransL1_RegisterClbEvent(pTransL1, TRANSL1_EVT_SEND_DONE, Clb_L1SendDone, pTransL2);
    TransL1_RegisterClbEvent(pTransL1, TRANSL1_EVT_RECV_BYTE, Clb_L1RecvData, pTransL2);

    /*-------------------------------
     * Initialize L1 Timeout Timer
     *-------------------------------*/
    InitL1TimeOutTimer(pTransL2);

    /* Done	*/
    pTransL2->sFlag.Bits.bStarted = TRUE;

    return eErrorCode;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
BOOL TransL2S_Stop(STransL2S *pTransL2)
{
    ASSERT_NONVOID(pTransL2, FALSE);

    if(pTransL2->sFlag.Bits.bStarted != TRUE) {
        pTransL2->u8ErrorCode = TRANS_ERR_NOT_STARTED;
        return FALSE;
    }

    pTransL2->sFlag.Bits.bStarted = FALSE;

    return TRUE;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
ETransErrorCode TransL2S_GetLastError(STransL2S *pTransL2)
{
    ASSERT_NONVOID(pTransL2, TRANS_ERR_UNKNOWN);
    return pTransL2->u8ErrorCode;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
BOOL TransL2S_IsSendReady(STransL2S *pTransL2)
{
    ASSERT_NONVOID(pTransL2, FALSE);
    return ( pTransL2->sFlag.Bits.bStarted == TRUE  &&
             pTransL2->sFlag.Bits.bSending == FALSE);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void TransL2S_SetCheckSeqNum(STransL2S *pTransL2, BOOL bSet)
{
    ASSERT_VOID(pTransL2);
    pTransL2->sFlag.Bits.bCheckSeqNum = bSet;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t* TransL2S_GetSendData(STransL2S *pTransL2)
{
    ASSERT_NONVOID(pTransL2, NULL);
    return &pTransL2->frmSend[IDX_SFRM_DATA0];
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
BOOL TransL2S_GetRecvFrame(STransL2S *pTransL2, uint8_t *pu8From, uint8_t **ppData, uint16_t *pu16Size)
{
    uint8_t* pRecvFrm;

    ASSERT_NONVOID(pTransL2, FALSE);

    pRecvFrm 	= pTransL2->frmRecv;
    *pu8From 	= SFRAME_GET_SRCADR(pRecvFrm);
    *ppData	= &pRecvFrm[IDX_SFRM_DATA0];
    *pu16Size	= SFRAME_GET_DLEN(pRecvFrm);

    return TRUE;
}
/*****************************************************************************/
 /** @brief
  *
  *
  *  @param
  *  @return Void.
  *  @note
  */
void TransL2S_SendTask(STransL2S *pTransL2)
{
    if(TransL2S_IsSending(pTransL2))
    {
        RunSendFrame(pTransL2);
        L2DBG_INC(nRunSendFrame);
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
EL2RecvAction TransL2S_RecvTask(STransL2S *pTransL2)
{
    EL2RecvAction eRecvAction = TRANSL2_ACT_NO;
    
    if(ProcessRecvData(pTransL2))
    {
        eRecvAction = OnRecvFrame(pTransL2);
        L2DBG_INC(nL2ProcRecvData);
    }

    return eRecvAction;
}
 /*****************************************************************************/
 /** @brief
  *
  *
  *  @param
  *  @return Void.
  *  @note
  */
EL2RecvAction TransL2S_Task(STransL2S *pTransL2)
{
    return TRANSL2_ACT_NO;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

ETransErrorCode TransL2S_Send(STransL2S *pTransL2, uint8_t u8DstAdr, SMem *pMem)
{
//    SFrameInfo 	*pFrameInfo;
    uint8_t 	*pSendFrm;
    uint8_t 	*pData;
    uint16_t       u16DLen;

    LREP("transl2 send \r\n");
    /*-------------------------*/
    ASSERT_NONVOID(pTransL2, FALSE);
    ASSERT_NONVOID(pMem != NULL, FALSE);

    /*-------------------------*/
    // Check valid and parameters
    /*-------------------------*/
    if(pTransL2->sFlag.Bits.bStarted != TRUE)
    {
        
        pTransL2->u8ErrorCode = TRANS_ERR_NOT_STARTED;
        ASSERT(FALSE);
        return pTransL2->u8ErrorCode;
    }

    if(pTransL2->sFlag.Bits.bSending)
    {
        
        pTransL2->u8ErrorCode = TRANS_ERR_BUSY;
        ASSERT(FALSE);
        return pTransL2->u8ErrorCode;
    }
	
    /*-------------------------*/
//    LREP("pMem at: 0x%x\r\n", pMem);
//    pFrameInfo = (SFrameInfo *)MEM_BODY(pMem);
//
    SFrameInfo *pFrameInfo = (SFrameInfo *)MEM_BODY(pMem);
    LREP("Header: 0x%x Location: 0x%x\r\n", pMem, pFrameInfo);

    pFrameInfo->u8NumSend--;

    pTransL2->sMemSend 		= pMem; /*save this for callback function*/

    pTransL2->frmSend 		= (uint8_t*)pFrameInfo + sizeof(SFrameInfo);
    pSendFrm          		= pTransL2->frmSend;
    pData         			= &pTransL2->frmSend[IDX_SFRM_DATA0];


    LREP("ctrl: 0x%x - len = 0x%x\r\n", pFrameInfo->u8Ctrl, pFrameInfo->u16DLen);

    /*---------------------------*/
    // Building the sending frame
    /*---------------------------*/
    pSendFrm[IDX_SFRM_PREAM] = CST_PREAMBLE;
    pSendFrm[IDX_SFRM_SRCADR]= pTransL2->u8NodeID;
    pSendFrm[IDX_SFRM_DSTADR]= u8DstAdr;
    pSendFrm[IDX_SFRM_SEQNUM]= pFrameInfo->u8Seq;
    pSendFrm[IDX_SFRM_CTRL]  = pFrameInfo->u8Ctrl;

    SFRAME_SET_DLEN(pSendFrm,pFrameInfo->u16DLen);

    u16DLen = SFRAME_GET_DLEN(pSendFrm);
    
    //LREP("\r\nL2 pFrameInfo->u16DLen = %d, u16DLen = %d",(uint32_t)pFrameInfo->u16DLen,(uint32_t)u16DLen);

    ASSERT(pTransL2->frmSend[IDX_SFRM_SRCADR] != pTransL2->frmSend[IDX_SFRM_DSTADR]);

    if(u16DLen > SFRM_MAX_DLEN)
    {
        pTransL2->u8ErrorCode = TRANS_ERR_PARAM;
        ASSERT(FALSE);
        LREP("u8DLen = %d,SFRM_MAX_DLEN = %d",(int)u16DLen,(int)SFRM_MAX_DLEN);
        return pTransL2->u8ErrorCode;
    }
    //CRCD
    if(u16DLen != 0)
    {
        pSendFrm[IDX_SFRM_CRCD] = crc_8(pData, u16DLen);
        //LREP("\r\nCRCD = 0x%x",(uint32_t)pSendFrm[IDX_SFRM_CRCD]);
    }
    else
    {
        pSendFrm[IDX_SFRM_CRCD]  = 0;
    }
    //CRCH
    pSendFrm[IDX_SFRM_CRCH] 	= crc_8(&pSendFrm[IDX_SFRM_SRCADR], SFRM_HDR_SIZE - 2);

    /*---------------------------*/
    // Queue the frame to send
    /*---------------------------*/
    pTransL2->sFlag.Bits.bSending 		= TRUE;
    pTransL2->sFlag.Bits.bWaitingL1Send 	= FALSE;
    pTransL2->sFlag.Bits.bL1Sending 		= FALSE;

    pTransL2->u8ErrorCode 			= TRANS_SUCCESS;
    
    return pTransL2->u8ErrorCode;
}
/*****************************************************************************/
/** @brief
 *		   Try to send the sending frame
 *
 *  @param
 *  @return 
 *  @note
 */

static void RunSendFrame(STransL2S *pTransL2)
{
//    STransL1 *pTransL1   = &pTransL2->sTransL1;
    uint8_t*   pSendFrm    = pTransL2->frmSend;
    uint16_t   u16SendSize = SFRAME_SIZE(pSendFrm);
    ETransErrorCode      eErrorCode;

    ASSERT_VOID(pTransL2);

    L2DBG_INC(nRunSendFrm1);

    /*-------------------------*/
    if(pTransL2->sFlag.Bits.bWaitingL1Send)
    {
        if(pTransL2->sFlag.Bits.bL1Sending)
        {
//          ASSERT(FALSE);
            return;
        }
        else
        {
            /*L1 send done*/
            pTransL2->sFlag.Bits.bSending 	= FALSE;
            pTransL2->sFlag.Bits.bWaitingL1Send = FALSE;

            L2FireEvent(pTransL2, TRANSL2_EVT_SEND_DONE);
            return;
        }
    }
    /*-------------------------
     * Transfer buffer to L1 to send
     *-------------------------*/
    L2DBG_INC(nSendFrmCount);

    if(TransL1_IsSendReady(&pTransL2->sTransL1) == TRUE)
    {
        
        pTransL2->sFlag.Bits.bL1Sending      = TRUE;
        pTransL2->sFlag.Bits.bWaitingL1Send  = TRUE;

        eErrorCode = TransL1_Send(&pTransL2->sTransL1, pSendFrm, u16SendSize);

        if(eErrorCode == TRANS_SUCCESS)
        {
            
            pTransL2->u8ErrorCode = eErrorCode;

            pTransL2->u32L1SendTimeout = 40; //200L + (uint32_t)u16SendSize * 1000L * 10L / pTransL2->sTransL1.u32BaudRate;

            /*
             *Start TimeOut Timer to monitor L1 Sending
             */
            StartL1TimeOutTimer(pTransL2,pTransL2->u32L1SendTimeout);

        }
        else
        {
            /*Error - L1 is busy*/

            pTransL2->u8ErrorCode 		= eErrorCode;
            pTransL2->sFlag.Bits.bSending 	= FALSE;

            L2FireEvent(pTransL2, TRANSL2_EVT_ERROR);

            return;
        }
    }

    return;
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static BOOL ProcessRecvData(STransL2S *pTransL2)
{
    uint8_t 	bCRCH;
    uint16_t      u16Recv;
    STransL1    *pTransL1  = &pTransL2->sTransL1;
    uint8_t  	*pRecvFrm  = pTransL2->frmRecv;
    uint8_t 	*pRecvData = &pRecvFrm[IDX_SFRM_DATA0];
    static uint16_t  u16DLen;

    ASSERT_NONVOID(pTransL2, FALSE);

    if(pTransL2->frmRecv == NULL)
    {
        //////ASSERT(pTransL2->frmRecv);
        
        if(TransL2S_MemAlloc(pTransL2) == FALSE)
        {
                ASSERT(FALSE);
                
                return FALSE;
        }
    }

    *pRecvFrm = CST_PREAMBLE;	//Start byte of frame

    while(TransL1_GetRecvCount(pTransL1))
    {    	
        L2DBG_INC(nL1Data);

        /*-------------------------------
         * WAIT_HEADER - Waiting for header
         *--------------------------------*/
        if(pTransL2->u16RecvCount == 0)
        {
            /*-------------------------*/
            // Start receiving if received at least a frame header
            if(TransL1_GetRecvCount(pTransL1) < SFRM_HDR_SIZE)
                    break;

            L2DBG_INC(nRecvHeader1);

            /*-------------------------*/
            // Seek for a valid preamble
            if(TransL1_RecvFF_Pop(pTransL1) != CST_PREAMBLE)
                    continue;

            L2DBG_INC(nRecvHeader2);

            // Enter FIFO protection mode
            // This step is needed in case any collision occured
            TransL1_RecvFF_EnProtect(pTransL1, TRUE);

            /*-------------------------*/
            // Receiving header's fields
            u16Recv = TransL1_Recv(pTransL1, &pRecvFrm[1], SFRM_HDR_SIZE-1);
            ASSERT_NONVOID(u16Recv == SFRM_HDR_SIZE-1, FALSE);

            bCRCH = crc_8(&pRecvFrm[1], SFRM_HDR_SIZE-2);

            /*-------------------------*/
            // Checking CRC for header
            if(bCRCH != pRecvFrm[IDX_SFRM_CRCH])
            {
                // If CRC for header is error,
                // ... comes back and search again.
                TransL1_RecvFF_RewindHead(pTransL1);
                TransL1_RecvFF_EnProtect(pTransL1, FALSE);
                L2DBG_INC(nRecvErrCRCH);
                continue;
            }

            /*-------------------------*/
            // Exit protected mode after received a correct header
            // NOTE: Assumes that collision can only occur in the period of header
            TransL1_RecvFF_EnProtect(pTransL1, FALSE);

            L2DBG_INC(nRecvHeader3);

            /*-------------------------*/
            // If needs recv DATA
            u16DLen = SFRAME_GET_DLEN(pRecvFrm);
            if(u16DLen != 0)
            {
                pTransL2->u16RecvCount = SFRM_HDR_SIZE;
                L2DBG_INC(nRecvDataStart);
                continue;
            }

            /*-------------------------*/
            // Processes received frame
            L2DBG_INC(nRecvCmdDone);
            return TRUE;

        } // WAIT_HEADER

        /*----------------------------------------
         * WAIT_DATA - Waiting Data
         *----------------------------------------*/
        else
        {
            int  nDataCount = pTransL2->u16RecvCount - SFRM_HDR_SIZE;
            uint8_t b;

            /*-------------------------*/
            // Store received data
            b = TransL1_RecvFF_Pop(pTransL1);
            if(nDataCount < SFRM_MAX_DLEN)
            {
                    pRecvData[nDataCount] = b;
                    L2DBG_INC(nRecvDataCount);
            }

            /*-------------------------*/
            // Not complete -> continue
            pTransL2->u16RecvCount++;
            nDataCount++;
            if(nDataCount < u16DLen)
            {
               continue;
            }
            /*-------------------------*/
            L2DBG_INC(nRecvDataDone);
            pTransL2->u16RecvCount = 0;
            return TRUE;

        } // WAIT_DATA
    } // while(1)
    return FALSE;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static EL2RecvAction OnRecvFrame(STransL2S *pTransL2)
{
    uint8_t*          pRecvFrm  = pTransL2->frmRecv;
    uint8_t*          pRecvData = &pRecvFrm[IDX_SFRM_DATA0];
    BOOL            bRecvFrame = FALSE;
    uint16_t          u16DLen;
    EL2RecvAction   eRecvAction = TRANSL2_ACT_NO;
    ETransErrorCode eErrorCode = TRANS_SUCCESS;

    ASSERT_NONVOID(pTransL2, eErrorCode);

    L2DBG_INC(nRecvFrame);

    u16DLen = SFRAME_GET_DLEN(pRecvFrm);
    //LREP("\r\nDLen = %d",(uint32_t)u16DLen);
    /*-------------------------*/
    // If the frame is NOT for this node
    if( pRecvFrm[IDX_SFRM_DSTADR] != pTransL2->u8NodeID && pRecvFrm[IDX_SFRM_DSTADR] != ADDR_BROADCAST)
    {
        L2DBG_INC(nNotDestination);
        LREP("\r\nTransL2 - Not for this NODE: %d",(int)pRecvFrm[IDX_SFRM_DSTADR]);
        return eRecvAction;
    }

    /*-------------------------*/
    // Invalid data length
    if( u16DLen > SFRM_MAX_DLEN && pRecvFrm[IDX_SFRM_DSTADR] != ADDR_BROADCAST )
    {
        L2DBG_INC(nRecvInvldDLen);

        eRecvAction = TRANSL2_REQ_NACK;
        eErrorCode  = TRANS_ERR_REMOTE_DLEN;

        LREP("\r\nTransL2 - invalid DLen");
        
        return eRecvAction;
    }

    /*-------------------------*/
    // Check CRC for data

    //LREP("\r\n[CRCD 0x%x, 0x%x] ",(uint32_t)CalCRC8(pRecvData, u16DLen),(uint32_t)pRecvFrm[IDX_SFRM_CRCD]);
    if((u16DLen != 0) && (pRecvFrm[IDX_SFRM_CRCD] != crc_8(pRecvData, u16DLen)))
    {
        // Wrong CRC -> Send NACK
        if( IS_ACK_REQ(pRecvFrm[IDX_SFRM_CTRL]) && pRecvFrm[IDX_SFRM_DSTADR] != ADDR_BROADCAST)
        {
            eRecvAction = TRANSL2_REQ_NACK;
            eErrorCode  = TRANS_ERR_REMOTE_CRCD;
        }

        LREP("\r\nTransL2 - wrong CRCD");
        TransL1_RecvFF_Reset(&pTransL2->sTransL1);
        L2DBG_INC(nRecvErrCRCD);

        return eRecvAction;
    }

    /*----------------------------------------
     * DATA frame
     *----------------------------------------*/
    if( IS_DATA_FRM(pRecvFrm[IDX_SFRM_CTRL]) )
    {
        L2DBG_INC(nRecvDataFrm1);

        // Check Sequence number
        if(!pTransL2->sFlag.Bits.bCheckSeqNum || (pRecvFrm[IDX_SFRM_SEQNUM] != pTransL2->u8SeqRemote))
        {
            bRecvFrame = TRUE;

            // Update Sequence number
            pTransL2->u8SeqRemote = pRecvFrm[IDX_SFRM_SEQNUM];

            L2DBG_INC(nRecvDataFrm2);
        }
        else
        {
            LREP("\r\nTransL2 - same SEQ");
        }
    }

    /*----------------------------------------
     * COMMAND frame
     *----------------------------------------*/
    else
    {
        L2DBG_INC(nRecvCmdFrm);
        switch( GET_FRM_TYPE(pRecvFrm[IDX_SFRM_CTRL]) )
        {
            case CMD_NONE	:
                break;
            case CMD_ACK 	:
                eRecvAction = TRANSL2_RCV_ACK;
                pTransL2->u8SeqCMDFrm = pRecvFrm[IDX_SFRM_SEQNUM];
                break;
            case CMD_NACK 	:
                eRecvAction = TRANSL2_RCV_NACK;
                pTransL2->u8SeqCMDFrm = pRecvFrm[IDX_SFRM_SEQNUM];
                break;
            default:
                ASSERT(FALSE);
                break;
        }
    }

    /*-------------------------*/
    // Send ACK if required
    if( IS_ACK_REQ(pRecvFrm[IDX_SFRM_CTRL]) && (pRecvFrm[IDX_SFRM_DSTADR] != ADDR_BROADCAST))
    {
        eRecvAction = TRANSL2_REQ_ACK;
        L2DBG_INC(nSendFrmACK1);
    }

    /*-------------------------*/
    // Callback
    if(bRecvFrame)
    {
        /*DATA.ind to higher layer*/
        L2FireEvent(pTransL2, TRANSL2_EVT_RECV_DATA);
    }

    return eRecvAction;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void TransL2S_RegisterClbEvent(STransL2S *pTransL2, EL2Event event, FClbL2Event pFunction, void *pClbParam)
{
    ASSERT_VOID(pTransL2);

    switch(event)
    {
        case TRANSL2_EVT_NONE:
            break;
        case TRANSL2_EVT_RECV_DATA:
            pTransL2->fClbL2RecvData 	= pFunction;
            pTransL2->pClbRecvDataParam	= pClbParam;
            break;
        case TRANSL2_EVT_SEND_DONE:
            pTransL2->fClbL2SendDone 	= pFunction;
            pTransL2->pClbSendDoneParam	= pClbParam;
            break;
        case TRANSL2_EVT_ERROR:
            pTransL2->fClbL2Error	= pFunction;
            pTransL2->pClbErrorParam	= pClbParam;
            break;
        default:
            ASSERT(FALSE);
            break;
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
static void L2FireEvent(STransL2S *pTransL2, EL2Event event)
{
    ASSERT_VOID(pTransL2);

    switch(event)
    {
        case TRANSL2_EVT_RECV_DATA:

            ASSERT(pTransL2->fClbL2RecvData != NULL);
            if(pTransL2->fClbL2RecvData)
            {
                SFrameInfo *pFrameInfo 	= (SFrameInfo *)MEM_BODY(pTransL2->sMemRecv);
                pFrameInfo->u16DLen	= SFRAME_GET_DLEN(pFrameInfo->pFrame);
                pFrameInfo->u8Ctrl	= pFrameInfo->pFrame[IDX_SFRM_CTRL];
                pTransL2->u8ErrorCode	= TRANS_SUCCESS;

                pTransL2->fClbL2RecvData(TRANSL2_EVT_RECV_DATA,pTransL2->u8ErrorCode,pTransL2->sMemRecv, pTransL2->pClbRecvDataParam);

                /*The previous buffer is eaten up and a new buffer is not assigned yet*/
                TransL2S_MemAlloc(pTransL2);
            }
            break;
        case TRANSL2_EVT_SEND_DONE:
            ASSERT(pTransL2->fClbL2SendDone != NULL);
            if(pTransL2->fClbL2SendDone)
            {
                pTransL2->u8ErrorCode	= TRANS_SUCCESS;
                pTransL2->fClbL2SendDone(TRANSL2_EVT_SEND_DONE,pTransL2->u8ErrorCode,pTransL2->sMemSend, pTransL2->pClbSendDoneParam);
            }
            break;
        case TRANSL2_EVT_ERROR:
            ASSERT(pTransL2->fClbL2Error != NULL);
            if(pTransL2->fClbL2Error)
            {
                pTransL2->fClbL2Error(TRANSL2_EVT_ERROR,pTransL2->u8ErrorCode,pTransL2->sMemSend, pTransL2->pClbErrorParam);
            }
            break;
        default:
            ASSERT(FALSE);
            break;
    }
}

/*****************************************************************************/
#ifdef DEBUG_TRANSL2
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void TransL2S_PrintLog(STransL2S *pTransL2)
{

}

#endif

/*****************************************************************************/
//#endif //BOARD




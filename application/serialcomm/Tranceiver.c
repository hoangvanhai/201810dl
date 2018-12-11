
#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/
#include "TransDefS.h"
#include "Transceiver.h"
#include "mem.h"
#include "queue.h"
#include "hardware_profile.h"
#include <fsl_debug_console.h>

/************************** Constant Definitions *****************************/
typedef enum
{
    E_CMD_FRM   =   0,      //(CMD_ACK or CMD_NACK)
    E_DATA_FRM  =   1       //(DATA_FRM)
}EPackageType;
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

#define Trans_SendCmdFrame(p,pData,Dlen,Ctrl) Trans_send(p,pData,Dlen,Ctrl,E_CMD_FRM)

/************************** Function Prototypes ******************************/

static void  Clb_L2RecvData	(EL2Event evt, ETransStatus eStatus, SMem *pMem, void *pClbParam);
static void  Clb_L2SendDone	(EL2Event evt, ETransStatus eStatus, SMem *pMem, void *pClbParam);
static void  Clb_L2Error 	(EL2Event evt, ETransStatus eStatus, SMem *pMem, void *pClbParam);
static void  Clb_UpdateTimer        (void *timer, void *pClbParam);
static uint8_t SearchSeqFrame         (void* pMem, void *pSeq);
static BOOL  Trans_send           (STrans *pTrans, uint8_t* pu8Data, uint16_t u16Dlen, uint8_t u8Ctrl, EPackageType bFrmType);
static void  Trans_CheckTimeOutOfSendedFrame(STrans *pTrans);
/************************** Variable Definitions *****************************/

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Trans_Init(STrans *pTrans,  uint32_t u32UartPort, uint32_t u32BaudRate, void *pSemaphore)
{
	OS_ERR err;

	pTrans->sFlag.u8All = 0;
    pTrans->hSem = pSemaphore;

    Queue_Init(&pTrans->qSendingData);
    Queue_Init(&pTrans->qSendingCMD);
    Queue_Init(&pTrans->qSentQueue);

    LREP("trans port %d \r\nqSendingData %x \r\nqSendingCMD %x \r\nqSentQueue %x\r\n",
				u32UartPort, &pTrans->qSendingData,
				&pTrans->qSendingCMD, &pTrans->qSentQueue);


    /*-----------------------------------------------------------------------
     * Init Trans Layer 2
     *-----------------------------------------------------------------------*/

    TransL2S_SetL1Para((STransL2S *)&pTrans->sTransL2,
    		u32UartPort, u32BaudRate, 0, 0);

    //LREP("init transceiver protocol\r\n\n");
#if (TRANSL1_VER == TRANSL1_V1)
    TransL2S_Init(&pTrans->sTransL2, LOGGER_DEV_ID, TRANS_TX_PRIO, TRANS_RX_PRIO, &pTrans->hSem);
#elif (TRANSL1_VER == TRANSL1_V2)
    TransL2S_Init(&pTrans->sTransL2, LOGGER_DEV_ID, TRANS_TX_PRIO, TRANS_RX_PRIO, pSemaphore, DMA_TX_TRANSPC);
#endif

    TransL2S_SetCheckSeqNum(&pTrans->sTransL2,TRUE);

    /*register callback functions for TransL2*/
    TransL2S_RegisterClbEvent(&pTrans->sTransL2, TRANSL2_EVT_RECV_DATA, Clb_L2RecvData, pTrans);
    TransL2S_RegisterClbEvent(&pTrans->sTransL2, TRANSL2_EVT_SEND_DONE, Clb_L2SendDone, pTrans);
    TransL2S_RegisterClbEvent(&pTrans->sTransL2, TRANSL2_EVT_ERROR,     Clb_L2Error,    pTrans);

    /*------------------------------------------------------------------------*/
	OSTmrCreate(&pTrans->hUpdateTimer,
				(CPU_CHAR *)"timer",
				(OS_TICK)0,
				(OS_TICK)TRANS_UPDATE_TIMER_RATE_MS,
				(OS_OPT)OS_OPT_TMR_PERIODIC,
				(OS_TMR_CALLBACK_PTR) Clb_UpdateTimer,
				(void*)pTrans,
				(OS_ERR*)&err);

    OSTmrStart(&pTrans->hUpdateTimer, &err);

    /*------------------------------------------------------------------------*/
    pTrans->sFlag.Bits.bStarted = TRUE;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Trans_RecvTask(STrans *pTrans)
{
    STransL2S 		*pTransL2 = &pTrans->sTransL2;
    EL2RecvAction 	eL2Action = TRANSL2_ACT_NO;
    SSearch 		sSearch;
    SMem    		*pMem;
    /*------------------------------------------------------------------------*/
    /*Run RecvTask of Layer 2 to process receiving frame*/
    eL2Action = TransL2S_RecvTask(pTransL2);

    switch(eL2Action)
    {
        case TRANSL2_ACT_NO:
            break;
        case TRANSL2_RCV_ACK:
            //LREP("\r\nTrans_L2_RCV_ACK");
            sSearch.fCriteria = SearchSeqFrame;
            sSearch.pHandle   = &pTrans->sTransL2.u8SeqCMDFrm;
            ASSERT(Queue_GetSize(&pTrans->qSentQueue) != 0);
            pMem = Queue_Remove(&pTrans->qSentQueue,&sSearch);
            ASSERT(pMem != NULL);
            if(pMem != NULL) {
                //LREP("\r\nRemove Seq = 0x%x from sended queue",(uint32_t)pTrans->sTransL2.u8SeqCMDFrm);
                Mem_Free(pMem);
            }
            break;
        case TRANSL2_RCV_NACK:
            LREP("\r\nTrans_L2_RCV_NACK");
            sSearch.fCriteria = SearchSeqFrame;
            sSearch.pHandle   = &pTrans->sTransL2.u8SeqCMDFrm;

            ASSERT(Queue_GetSize(&pTrans->qSentQueue) != 0);

            pMem = Queue_Remove(&pTrans->qSentQueue,&sSearch);

            ASSERT(pMem != NULL);

            if(pMem != NULL)
            {
                SFrameInfo *pFrameInfo = (SFrameInfo*)MEM_BODY(pMem);
                pFrameInfo->i16TimeOut = TRANS_TIMEOUT_TICK;

                //LREP("\r\nQueue Seq = 0x%x from sended to sending queue",(uint32_t)pTrans->sTransL2.u8SeqCMDFrm);

                Queue_Push(&pTrans->qSendingData,pMem);

            }
            break;

        case TRANSL2_REQ_ACK:
//            LREP("\r\nTrans_L2_REQ_ACK");
            //L2 get a data frame requiring ACK		->	Add CMD_ACK frame to queue
             Trans_SendCmdFrame(pTrans, NULL, 0, CMD_ACK);
            break;

        case TRANSL2_REQ_NACK:
            LREP("\r\nTrans_L2_REQ_NACK");
            //L2 get a frame but DLEN or CRCD is wrong	->	Add CMD_NACK frame to queue
            Trans_SendCmdFrame(pTrans, NULL, 0, CMD_NACK);
            break;

        default:
            ASSERT(FALSE);
            break;
    }
    /*------------------------------------------------------------------------*/

}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Trans_SendTask(STrans *pTrans)
{
    STransL2S   *pTransL2         = &pTrans->sTransL2;
    SMem        *pMem             = NULL;
    /*------------------------------------------------------------------------*/
    //Update State of waiting frames

    if(pTrans->sFlag.Bits.bUpdateWaitingACKFrameState == TRUE)
    {
        pTrans->sFlag.Bits.bUpdateWaitingACKFrameState = FALSE;
        Trans_CheckTimeOutOfSendedFrame(pTrans);
    }
    /*------------------------------------------------------------------------*/



    /*------------------------------------------------------------------------*/
    /*Send voice frame to RADIO*/
    if (TransL2S_IsSendReady((STransL2S*)pTransL2) == TRUE)
    {
        
        /*First, Send ACK/NACK message*/
        if(Queue_GetSize(&pTrans->qSendingCMD) > 0)
        {
            pMem = Queue_Remove(&pTrans->qSendingCMD, NULL);
        }
        /*Secondly, Send UI Control/Status message*/
        else if (Queue_GetSize(&pTrans->qSendingData) > 0)
        {
            pMem = Queue_Remove(&pTrans->qSendingData, NULL);
            //LREP("get mem at: 0x%x - 0x%x\r\n", pMem, MEM_BODY(pMem));
        }

        if(pMem != NULL) {
            if(TransL2S_Send(pTransL2, LOGGER_PC_ID, pMem) != TRANS_SUCCESS) {
                ASSERT(FALSE);
            }
        }
    }

    /*-----------------------------------------------------------------------
     * Run TransL2 to send frame
     *-----------------------------------------------------------------------*/
     TransL2S_SendTask(pTransL2);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Trans_Task(STrans *pTrans)
{
	Trans_RecvTask(pTrans);
	Trans_SendTask(pTrans);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static BOOL Trans_send(STrans *pTrans, uint8_t* pu8Data, 
                            uint16_t u16Dlen, uint8_t u8Ctrl, EPackageType bFrmType)
{

	if(!pTrans->sFlag.Bits.bStarted)
		return FALSE;

	if(u16Dlen > SFRM_MAX_DLEN) u16Dlen = SFRM_MAX_DLEN;

    uint16_t u16Idx = 0;
    SMem        *pMem;
    

    pMem =  Mem_Alloc(CALCULATE_MEM_SIZE(u16Dlen + SFRM_HDR_SIZE));

    if(pMem == NULL) {
        ASSERT("No memory available" == 0);
        return FALSE;
    }
    

    SFrameInfo *pFrameInfo = (SFrameInfo *)MEM_BODY(pMem);

    pFrameInfo->u8Ctrl   = (uint8_t)u8Ctrl;
    pFrameInfo->u16DLen  = (uint16_t)u16Dlen;
    pFrameInfo->pFrame   = ((uint8_t*)pFrameInfo + sizeof(SFrameInfo));
    pFrameInfo->pu8Data	 = &pFrameInfo->pFrame[IDX_SFRM_DATA0];


//    LREP("ctrl: 0x%x - len = 0x%x\r\n", pFrameInfo->u8Ctrl, pFrameInfo->u16DLen);
//    LREP("Header: 0x%x Location: 0x%x\r\n", pMem, pFrameInfo);

    //SEQ
    if(bFrmType == E_CMD_FRM)
    {
        pFrameInfo->u8Seq = pTrans->sTransL2.u8SeqRemote;
    }
    else
    {
        pFrameInfo->u8Seq = pTrans->sTransL2.u8SeqLocal++;
        //LREP("\r\nL3 Seq = %d",(uint32_t)pTrans->u8SeqLocal);
    }
//

    if(u16Dlen > 0 && pu8Data != NULL)
    {
        for(u16Idx = 0; u16Idx < u16Dlen; u16Idx++)
        {
            pFrameInfo->pu8Data[u16Idx] = pu8Data[u16Idx];
        }
    }
//
    pFrameInfo->u8NumSend  = TRANS_MAX_NUM_SEND;
    pFrameInfo->i16TimeOut = TRANS_TIMEOUT_TICK;

    switch(u8Ctrl & 0x3F)
    {
        case CMD_ACK:
        case CMD_NACK:
            Queue_Push(&pTrans->qSendingCMD, pMem);
            break;
        default:
            Queue_Push(&pTrans->qSendingData, pMem);
            break;
    }

    //Signal to run Trans task
    
    OS_ERR err;
    OSTaskSemPost((OS_TCB*)pTrans->hSem, OS_OPT_NONE, &err);

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
BOOL Trans_Send(STrans *pTrans, uint16_t u16Dlen, uint8_t* pu8Data,  uint8_t u8Ctrl)
{
    return Trans_send(pTrans, pu8Data, u16Dlen, u8Ctrl, E_DATA_FRM);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
BOOL Trans_IsSendReady(STrans *pTrans, uint16_t u16DLen)
{
//    uint16_t u16Size = CALCULATE_MEM_SIZE(u16DLen + SFRM_HDR_SIZE);
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
static uint8_t SearchSeqFrame(void* pMem, void *pSeq)
{
    SFrameInfo  *pFrameInfo = (SFrameInfo*)pMem;

    //LREP("\r\nSEARCH Seq = 0x%x", (uint32_t)(*((uint8_t*)pSeq)));

    if(pFrameInfo->u8Seq == *((uint8_t*)pSeq))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
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
static void Trans_CheckTimeOutOfSendedFrame(STrans *pTrans)
{
    SMem *pMem, *pTemMem;

    SFrameInfo *pFrameInfo;
    uint8_t u8Seq;
    SSearch sSearch;    
    if(Queue_GetSize(&pTrans->qSentQueue) > 0)
    {
	//LREP(" Size = %d",(uint32_t)Queue_GetSize(&pTrans->qSentQueue));
        pMem = Queue_Peek(&pTrans->qSentQueue);

        ASSERT(pMem);

        while(pMem != 0)
        {
            pFrameInfo  = (SFrameInfo*)MEM_BODY(pMem);
            u8Seq       = pFrameInfo->u8Seq;

            sSearch.fCriteria = SearchSeqFrame;
            sSearch.pHandle   = &u8Seq;

            if(--pFrameInfo->i16TimeOut == 0)
            {
				if(pFrameInfo->u8NumSend > 0)
				{
					//if need, can set timeout by random value
					pFrameInfo->i16TimeOut = TRANS_TIMEOUT_TICK;
					//Queue frame to sending queue
					pMem = Queue_Remove(&pTrans->qSentQueue,&sSearch);
					Queue_Push(&pTrans->qSendingData, pMem);
					pMem = pMem->pNext;
				}
				else
				{
					pMem = Queue_Remove(&pTrans->qSentQueue,&sSearch);
					pTemMem = pMem->pNext;
					LREP("DOES NOT GET ACK SEQ %d\r\n",(int)pFrameInfo->u8Seq);
					//delete frame
					Mem_Free(pMem);
					pMem = pTemMem;
				}
            } else {
				//move to next frame
				pMem = pMem->pNext;
            }
        }
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

void Trans_RegisterClbEvent(STrans *pTrans, EL3Event evt, FClbL3Event pFunction)
{
    switch(evt)
    {
        case TRANS_EVT_RECV_DATA:
            pTrans->fClbRecv = pFunction;
            break;
        case TRANS_EVT_SENT_DATA:
            pTrans->fClbSendDone = pFunction;
            break;
        case TRANS_EVT_ERROR:
            pTrans->fClbError = pFunction;
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
void Clb_L2RecvData(EL2Event evt, ETransStatus eStatus, SMem *pRecvMem, void *pClbParam)
{
    STrans *pTrans = (STrans *)pClbParam;

    ASSERT(evt == TRANSL2_EVT_RECV_DATA);

    if(NULL != pRecvMem)
    {

        SFrameInfo *pFrameInfo  = (SFrameInfo*)MEM_BODY(pRecvMem);
        
        // Callback to higher layer
        if(pTrans->fClbRecv != NULL)
        {
            pTrans->fClbRecv(pRecvMem, pFrameInfo->u8Ctrl);
        }

        /*Free buffer*/
        
        Mem_Free(pRecvMem);
    }
    else
    {
        ASSERT("Null frame From TransL2" == 0);
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
void Clb_L2SendDone(EL2Event evt, ETransStatus eStatus, SMem *pSendMem, void *pClbParam)
{
    STrans   *pTrans = (STrans *)pClbParam;
    SFrameInfo  *pFrameInfo = (SFrameInfo*)MEM_BODY(pSendMem);
    uint8_t 	*pFrame     = pFrameInfo->pFrame;
    
    ASSERT(evt == TRANSL2_EVT_SEND_DONE);
    
    ASSERT_VOID(pSendMem != 0);
    
    if(pTrans->fClbSendDone != NULL)
    {
        pTrans->fClbSendDone(pSendMem,((SFrameInfo*)MEM_BODY(pSendMem))->u8Ctrl);
    }
    /*if the frame require ACK --> queue to SendedQueue*/
    if(IS_ACK_REQ(pFrame[IDX_SFRM_CTRL]))
    {
        Queue_Push(&pTrans->qSentQueue, pSendMem);
        LREP("#");
    }
    else
    {
    	LREP(">");
        Mem_Free(pSendMem);
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
void Clb_L2Error (EL2Event evt, ETransStatus eStatus, SMem *pSendMem, void *pClbParam)
{
    ASSERT(evt == TRANSL2_EVT_ERROR);

    LREP("\r\nTrans L2 Err %d.",(int)eStatus);

    //free buffer if needed
    if((TRANS_ERR_L2_TIMEOUT == eStatus) || (TRANS_ERR_L1_TIMEOUT == eStatus))
    {
        Mem_Free(pSendMem);
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
static void Clb_UpdateTimer(void *timer, void *pClbParam)
{
	OS_ERR err;
    STrans *pTrans = (STrans *)pClbParam;
    pTrans->sFlag.Bits.bUpdateWaitingACKFrameState = TRUE;
    OSTaskSemPost((OS_TCB*)pTrans->hSem, OS_OPT_POST_NONE, &err);
}


#ifdef __cplusplus
}
#endif
/**/



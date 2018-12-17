#ifndef TransL2S_H_
#define TransL2S_H_


#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/
#include "app_cfg.h"
#include "TransDefS.h"
#include "mem.h"
#include "queue.h"

#include "app_cfg.h"
#include "fsl_debug_console.h"


#if (TRANSL1_VER == TRANSL1_V1)
    #include "TransL1V1.h"
#else
    #include "TransL1V2.h"
#endif


/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/



typedef void (*FClbL2Event)(EL2Event eEventCode, ETransStatus eStatus, SMem *pMem, void *pClbParam);


#ifdef DEBUG_TRANSL2
    #define L2DBG_INC(n)	pTransL2->sDbg.n++
    typedef struct {

    } STransL2SDbg;

#else
    #define TransL2S_PrintLog(a)
    #define L2DBG_INC(n)
#endif

typedef union _STransL2SFlag
{
    uint8_t u8All;
    struct
    {
        unsigned bStarted    		:   1;
        unsigned bSending    		:   1;
        unsigned bL1Sending    		:   1;
        unsigned bWaitingL1Send    	:   1;
        unsigned bCheckSeqNum		:   1;
        unsigned b5    			:   1;
        unsigned b6    			:   1;
        unsigned b7    			:   1;
    }Bits;
    
}SL2SFlags;


typedef struct _STransL2S
{
    SL2SFlags           sFlag;

    SMem  				*sMemRecv;	//Contain Received frame
    SMem  				*sMemSend;	//Contain frame to send

    uint8_t				*frmRecv;	//point to frame position of frmRecvInfor
    uint8_t				*frmSend;	//point to frame position of frmSendInfor

    STransL1            sTransL1;

    uint8_t				u8NodeID;
    uint16_t			u16RecvCount;
    uint8_t				u8SeqRemote;
    uint8_t				u8SeqLocal;
    uint8_t				u8SeqCMDFrm;
    uint8_t				u8ErrorCode;

    uint32_t 			u32L1SendTimeout;

    /*L2 Callback function*/
    FClbL2Event         fClbL2RecvData;	//callback function for data indicate
    FClbL2Event         fClbL2SendDone;	//callback function for data confirm
    FClbL2Event         fClbL2Error;		//callback function for error event

    void		 		*pClbSendDoneParam; //Callback param for send done
    void		 		*pClbRecvDataParam; //Callback param for send done
    void		 		*pClbErrorParam; 	//Callback param for send done

    /*Semaphore to signal Trans Task*/

    semaphore_t			*hSem;			//use to signal Trans thread

    /*TimeOut Timer for L1*/
    OS_TMR				hL1TimeOutTimer;
	
} STransL2S;

/***************** Macros (Inline Functions) Definitions *********************/

#define TransL2S_IsStarted(pTransL2)		(pTransL2->sFlag.Bits.bStarted)
#define TransL2S_IsSending(pTransL2)		(pTransL2->sFlag.Bits.bSending)
#define TransL2S_IsL1Sending(pTransL2)		(pTransL2->sFlag.Bits.bL1Sending)
#define TransL2S_IsWaitL1Sending(pTransL2)	(pTransL2->sFlag.Bits.bWaitL1Sending)
#define TransL2S_IsCheckSeq(pTransL2)		(pTransL2->sFlag.Bits.bCheckSeqNum)
#define TransL2S_GetLocalSeq(pTransL2)		(pTransL2->u8SeqLocal)
#define TransL2S_GetRemoteSeq(pTransL2)		(pTransL2->u8SeqRemote)
#define TransL2S_GetRecvCMDFrmSeq(pTransL2)	(pTransL2->u8SeqCMDFrm)
#define TransL2S_UpdateLocalSeq(pTransL2)	(pTransL2->u8SeqLocal++)

/************************** Function Prototypes ******************************/

#if (TRANSL1_VER == TRANSL1_V1)
ETransErrorCode 	TransL2S_Init		(STransL2S *pTransL2, uint8_t u8NodeID, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio, void* pSemaphore);
#elif (TRANSL1_VER == TRANSL1_V2)
ETransErrorCode 	TransL2S_Init		(STransL2S *pTransL2, uint8_t u8NodeID, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio, void* pSemaphore, uint16_t u16TxDMABase);
#endif

void 				TransL2S_SetL1Para	(STransL2S *pTransL2,  uint32_t u32UartPort, uint32_t u32BaudRate, int32_t u16RS485Port, int32_t u16RS485Pin);
BOOL 				TransL2S_Stop		(STransL2S *pTransL2);
EL2RecvAction  		TransL2S_Task		(STransL2S *pTransL2);
EL2RecvAction 		TransL2S_RecvTask	(STransL2S *pTransL2);
void 				TransL2S_SendTask	(STransL2S *pTransL2);
BOOL				TransL2S_IsSendReady	(STransL2S *pTransL2);
void 				TransL2S_SetCheckSeqNum	(STransL2S *pTransL2, BOOL bSet);
uint8_t*			TransL2S_GetSendData	(STransL2S *pTransL2);
ETransErrorCode		TransL2S_Send           (STransL2S *pTransL2, uint8_t u8DstAdr, SMem *pMem);
BOOL				TransL2S_GetRecvFrame	(STransL2S *pTransL2, uint8_t *pu8From, uint8_t **ppData, uint16_t *pu16Size);
ETransErrorCode 	TransL2S_GetLastError	(STransL2S *pTransL2);
void				TransL2S_RegisterClbEvent(STransL2S *pTransL2, EL2Event event, FClbL2Event pFunction, void* pClbParam);


#ifdef DEBUG_TRANSL2
void TransL2S_PrintLog			(STransL2S *pTransL2);
#endif

/************************** Variable Definitions *****************************/

/*****************************************************************************/


#ifdef __cplusplus
}
#endif

//#endif //BOARD

#endif /* TransL2S_H_ */

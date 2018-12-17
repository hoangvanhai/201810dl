#ifndef TRANS_PC_H_
#define TRANS_PC_H_


/***************************** Include Files *********************************/
#include "TransL2S.h"
#include "mem.h"
#include "queue.h"
#include "app_cfg.h"
#include "definition.h"

/***************** Macros (Inline Functions) Definitions *********************/

typedef struct _STrans
{
    STransFlag       	sFlag;
    SQueue				qSendingData;
    SQueue 				qSendingCMD;
    SQueue 				qSentQueue;
    /*Hanlde of StransL2*/
    STransL2S           sTransL2;
    /*Handle of semaphore */
    semaphore_t			*hSem;
    /*Timer handle*/
    OS_TMR              hUpdateTimer;
    FClbL3Event        	fClbRecv;
    FClbL3Event        	fClbSendDone;
    FClbL3Event        	fClbError;
}STransPC;

/************************** Function Prototypes ******************************/

void TransPC_Init				(STransPC *pTrans,  uint32_t u32UartPort, uint32_t u32BaudRate, void *pSemaphore);

void TransPC_Task				(STransPC *pTrans);

BOOL TransPC_Send             (STransPC *pTrans,
							 uint16_t u16Dlen,
							 uint8_t* pu8Data,
							 uint8_t u8Ctrl);


BOOL TransPC_IsSendReady        (STransPC *pTrans, uint16_t u16DLen);

void TransPC_RegisterClbEvent   (STransPC *pTrans, EL3Event evt, FClbL3Event pFunction);






BOOL TransPC_IsTxReady          (STransPC *pTrans, uint16_t u16DLen);
/************************** Variable Definitions *****************************/

#endif /* TRANS_UI_H_ */

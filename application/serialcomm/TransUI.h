#ifndef TRANS_UI_H_
#define TRANS_UI_H_


/***************************** Include Files *********************************/
#include "TransL2S.h"
#include "mem.h"
#include "queue.h"
#include "app_cfg.h"
#include "definition.h"


/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

typedef struct _STransUI
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
}STransUI;
/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

void TransUI_Init				(STransUI *pTrans,  uint32_t u32UartPort, uint32_t u32BaudRate, void *pSemaphore);

void TransUI_Task				(STransUI *pTrans);

BOOL TransUI_Send             (STransUI *pTrans,
							 uint16_t u16Dlen,
							 uint8_t* pu8Data,
							 uint8_t u8Ctrl);


BOOL TransUI_IsSendReady        (STransUI *pTrans, uint16_t u16DLen);

void TransUI_RegisterClbEvent   (STransUI *pTrans, EL3Event evt, FClbL3Event pFunction);






BOOL TransUI_IsTxReady          (STransUI *pTrans, uint16_t u16DLen);
/************************** Variable Definitions *****************************/

#endif /* TRANS_UI_H_ */

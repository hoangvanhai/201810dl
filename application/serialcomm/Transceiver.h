#ifndef TRANS_UI_H_
#define TRANS_UI_H_


/***************************** Include Files *********************************/
#include "TransL2S.h"
#include "mem.h"
#include "queue.h"
#include "app_cfg.h"


/************************** Constant Definitions *****************************/
#define TRANS_UPDATE_TIMER_RATE_MS   (1)    //10ms

#define TRANS_TIMEOUT_TICK           (50L)	//in mili-ten second
#define TRANS_MAX_NUM_SEND           (3)

#define TRANS_MAX_DLEN               (240)



/**************************** Type Definitions *******************************/
typedef union _S_Trans_FLAG
{
    uint8_t	u8All;
    struct
    {
        unsigned bStarted                    :	1;
        unsigned bUpdateWaitingACKFrameState :	1;
        unsigned b2                          :	1;
        unsigned b3                          :	1;
        unsigned b4                          :	1;
        unsigned b5                          :	1;
        unsigned b6                          :	1;
        unsigned b7                          :	1;
    }Bits;

}STransFlag;

typedef enum E_Trans_EVENT_
{
    TRANS_EVT_NONE           = 0,
    TRANS_EVT_RECV_DATA      = 1,	//Data indicate
    TRANS_EVT_SENT_DATA      = 2,	//Data confirm
    TRANS_EVT_ERROR          = 3

}ETransEvent;

typedef void (*FClbUIEvent)(void *pData, uint8_t u8Type);

typedef struct _STrans
{
    STransFlag       	sFlag;
    SQueue				qSendingData;
    SQueue 				qSendingCMD;
    SQueue 				qSentQueue;
    /*Hanlde of StransL2*/
    STransL2S           sTransL2;
    /*Handle of semaphore */
    void*             	hSem;
    /*Timer handle*/
    OS_TMR              hUpdateTimer;
    FClbUIEvent        	fClbRecv;
    FClbUIEvent        	fClbSendDone;
    FClbUIEvent        	fClbError;


}STrans;
/***************** Macros (Inline Functions) Definitions *********************/

#define  TransIBC_Init(pTransIBC, u32UartPort, u32BaudRate, pSemaphore)	\
 		 Trans_Init(pTransIBC, u32UartPort, u32BaudRate, pSemaphore)
#define  TransIBC_Task(pTransIBC)		Trans_Task(pTransIBC)

#define  TransIBCSend(pData, len, ctrl)                   Trans_Send(&sApp.sTrans,len, pData, ctrl)
/************************** Function Prototypes ******************************/

void Trans_Init				(STrans *pTrans,  uint32_t u32UartPort, uint32_t u32BaudRate, void *pSemaphore);

void Trans_Task				(STrans *pTrans);

BOOL Trans_Send               (STrans *pTrans, 
                                 uint16_t u16Dlen, 
                                 uint8_t* pu8Data,  
                                 uint8_t u8Ctrl);


BOOL Trans_IsSendReady        (STrans *pTrans, uint16_t u16DLen);

void Trans_RegisterClbEvent   (STrans *pTrans, ETransEvent evt, FClbUIEvent pFunction);






BOOL Trans_IsTxReady          (STrans *pTrans, uint16_t u16DLen);
/************************** Variable Definitions *****************************/

extern STrans sTrans;



#endif /* TRANS_UI_H_ */

#ifndef TRANSL1_V1_H_
#define TRANSL1_V1_H_


#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/
#include <string.h>
#include "typedefs.h"
#include "TransFiFo.h"
#include "app_cfg.h"


#if (TRANSL1_VER == TRANSL1_V1)

/************************** Constant Definitions *****************************/
//#define DEBUG_TRANSL1

#define SIZE_FIFO_RECV		512

/**************************** Type Definitions *******************************/

/* Struct for debug layer 1*/
#ifdef DEBUG_TRANSL1

	#define L1DBG_INC(n)	pTransL1->sDBG.n++

	typedef struct _STransL1DBG {

		uint32_t  u32FIFOFull;
		uint32_t 	u32FIFOPushOK;
		uint32_t 	u32FIFOPushNotOK;

		uint32_t	u32SendedBytes;
		uint32_t	u32RecvBytes;
		
		uint32_t  u32RxInterrupt;
		uint32_t  u32TxInterrupt;

	}STransL1DBG;

#else

	#define TransL1_PrintLog(a)
	#define L1DBG_INC(n)

#endif

typedef struct _SRS485Drive
{
	uint16_t	u16Port;
	uint16_t	u16Pin;
	
}SRS485DE;

typedef enum E_TRANSL1_EVENT_
{
	TRANSL1_EVT_NONE 		= 0,
	TRANSL1_EVT_SEND_DONE 	= 1,	//L1 Send done
	TRANSL1_EVT_RECV_BYTE 	= 2,	//L1 receive a byte
	TRANSL1_EVT_ERROR		= 3

}EL1Event;

typedef struct _S_L1_FLAG
{
	uint8_t u8All;
    struct
    {
        unsigned bStarted    		:   1;
        unsigned bSending    		:   1;
        unsigned bNewByte    		:   1;	//???
        unsigned b3		    		:   1;
        unsigned b4					:   1;
        unsigned b5    				:   1;
        unsigned b6    				:   1;
        unsigned b7    				:   1;
    }Bits;
}SL1Flag;
	
typedef void (*FClbL1Event)(void *pParam);	/*format of callback function */

/* Layer 1 data struct*/
typedef struct _STransL1 
{
	SL1Flag				sFlag;
	
	uint32_t   			u32UartPort;
	uint32_t			u32BaudRate;
	
	uint8_t				*pSendBuff;			// Buffer waiting to be sent
	uint16_t			u16SendSize;		// Sending size in BYTEs
	uint16_t			u16SendPtr;			// Current sending index
	
	SFIFO				sRecvFIFO;			// Receving FIFO
	
	FClbL1Event			fClbL1SendDone;		// Callback when all data sent
	FClbL1Event			fClbL1RecvData;		// Callback when received data
	FClbL1Event			fClbL1Error;		// Callback when error
	void* 				pClbSendDoneParam;	// Parameter to pass to the callback functions
	void* 				pClbRecvByteParam;	// Parameter to pass to the callback functions
	void* 				pClbErrorParam;		// Parameter to pass to the callback functions
	
	uint8_t 			arrRecvFIFO[SIZE_FIFO_RECV];
	
	SRS485DE			sRS485DE;			// To drive RS485
		
	#ifdef DEBUG_TRANSL1
		STransL1DBG		sDBG;
	#endif

} STransL1;


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

void 		TransL1_UARTInit(uint32_t u32UartPort, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio);
uint8_t  	TransL1_Init( STransL1 *pTransL1,  uint32_t u32UartPort,
							uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio);

void 		TransL1_Stop				(STransL1 *pTransL1);
BOOL 		TransL1_IsSendReady			(STransL1 *pTransL1);
int  		TransL1_Send				(STransL1 *pTransL1, uint8_t* pData, uint16_t nSize);
int  		TransL1_Recv				(STransL1 *pTransL1, uint8_t* pData, uint16_t nSize);
int	 		TransL1_GetRecvCount		(STransL1 *pTransL1);
void 		TransL1_RecvFF_EnProtect	(STransL1 *pTransL1, BOOL bEn);
void 		TransL1_RecvFF_RewindHead	(STransL1 *pTransL1);
uint8_t   	TransL1_RecvFF_Pop			(STransL1 *pTransL1);
void 		TransL1_RecvFF_Reset		(STransL1 *pTransL1);

void		TransL1_RegisterClbEvent		(STransL1 *pTransL1, EL1Event event, FClbL1Event pFunction, void *pParam);

BOOL		TransL1_IsReceiving			(STransL1 *pTransL1);
void		TransL1_ClearNewByteFlag	(STransL1 *pTransL1);

void 		TransL1_TX_Interrupt_Handle	(void* pParam);
void 		TransL1_RX_Interrupt_Handle	(void* pParam);

#ifdef DEBUG_TRANSL1
void 	TransL1_PrintLog			(STransL1 *pTransL1);
void 	TransL1_PrintRecvBuffer			(STransL1 *pTransL1);
#endif

/************************** Variable Definitions *****************************/

/*****************************************************************************/

#endif //#if (TRANSL1_VER	== TRANSL1_V1)

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* TRANSL1_H_ */

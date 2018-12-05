
#ifndef _FIFOQUEUE_
#define _FIFOQUEUE_

/***************************** Include Files *********************************/
#include "typedefs.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


typedef struct _SFIFO {
	//BYTE 			arrBuff[SIZE_FIFO_RECV];
	uint8_t			*arrBuff;
	uint16_t 		u16MaxSize;
	uint16_t		u16Size;
	uint16_t		u16Head;
	uint16_t		u16Tail;
	BOOL			bEnProtect;
	uint16_t		u16ProtectPtr;
} SFIFO;


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

void FIFO_Create(SFIFO *pFF, uint8_t *arrBuff, uint16_t u16Size);
void FIFO_Destroy(SFIFO *pFF);
BOOL FIFO_Pop(SFIFO *pFF, BYTE *b);
BOOL FIFO_Push(SFIFO *pFF, BYTE b);
void FIFO_EnableProtect(SFIFO *pFF);
void FIFO_DisableProtect(SFIFO *pFF);
void FIFO_RewindHead(SFIFO *pFF);
BOOL FIFO_IsEnablePush(SFIFO *pFF);
BOOL FIFO_IsEmpty(SFIFO *pFF);
BOOL FIFO_IsFull(SFIFO *pFF);
BOOL FIFO_Reset(SFIFO *pFF);
uint16_t FIFO_GetCount(SFIFO *pFF);
/************************** Variable Definitions *****************************/

/*****************************************************************************/



#endif

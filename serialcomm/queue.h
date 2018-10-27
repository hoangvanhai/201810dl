#ifndef QUEUE_H_
#define QUEUE_H_

/***************************** Include Files *********************************/
#include "mem.h"
#include "TransDef.h"
#include "fsl_os_abstraction.h"
/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/
typedef enum{
	QUEUE_SUCCESS   = 0,
	QUEUE_FULL      = 1
}Queue_status_t;
/**
 * @brief Structure to search for a buffer to be removed from a queue
 */
typedef struct _SSearchag
{
    /** Pointer to search criteria function */
    uint8_t (*fCriteria)(void *pMem, void *pHandle);
    /** Handle to callbck parameter */
    void *pHandle;
} SSearch;

typedef struct _SQueue
{
    /** Pointer to head of queue */
    SMem *pHead;
    /** Pointer to tail of queue */
    SMem *pTail;
    /**
     * Number of buffers present in the current queue
     */
    uint8_t u8Size;
} SQueue;
/***************** Macros (Inline Functions) Definitions *********************/
#define Search_SetParam(sSearch, fCriteria, pHandle) (sSearch.fCriteria = fCriteria; sSearch.pHandle = pHandle;)

#define Queue_GetSize(q) ((q)->u8Size)

#define Queue_Pop(q)    Queue_Remove(q,NULL)
#define Queue_Peek(q)   Queue_Read(q,NULL)

#define Queue_Push(q,b) Queue_Append(q,b)
/************************** Function Prototypes ******************************/


#ifdef __cplusplus
extern "C"
{

#endif

void 	Queue_Init(SQueue *q);

void 	Queue_Append(SQueue *q, SMem *buf);

SMem* 	Queue_Remove(SQueue *q, SSearch *search);

SMem* 	Queue_Read(SQueue *q, SSearch *search);

void 	Queue_Flush(SQueue *q);

uint8_t Queue_GetFreeQueueSize(uint16_t u16Size);

#if (TOTAL_NUMBER_OF_SMALL_MEMS > 0)
uint8_t Queue_GetFreeSmallQueueSize(void);
#endif

uint8_t Queue_GetFreeLargeQueueSize();
        
#ifdef __cplusplus
} /* extern "C" */
#endif
/************************** Variable Definitions *****************************/

/*****************************************************************************/
#endif /* QUEUE_H_ */

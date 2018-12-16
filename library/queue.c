
/***************************** Include Files *********************************/


#include "mem.h"
#include "queue.h"
#include "app_cfg.h"
#include <fsl_debug_console.h>


/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/
typedef enum _SMode
{
    REMOVE_MODE,
    READ_MODE
} SMode;

/***************** Macros (Inline Functions) Definitions *********************/

#if 	(APP_PLATFORM == APP_RTOS)


	
#elif	(APP_PLATFORM == APP_STAND_ALONE)

	#define ENTER_CRITICAL()		
	#define EXIT_CRITICAL() 
#else
	#error Please define Platform, RTOS or Stand alone in app_cfg.h
#endif
/************************** Function Prototypes ******************************/

static SMem *Queue_Read_Or_Remove(SQueue *q, SMode mode, SSearch *search);

/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
 * @brief Initializes the queue.
 *
 * @param q The queue which should be initialized.
 * @return Void.
 * @note
 */

bool Queue_Init(SQueue *q)
{
    q->pHead = NULL;
    q->pTail = NULL;
    q->u8Size = 0;
    return OSA_MutexCreate(&q->mtx) == kStatus_OSA_Success;
}
/*****************************************************************************/

/**
 * @brief Appends a buffer into the queue.
 *
 * @param q Queue into which buffer should be appended
 * @param buf Pointer to the buffer that should be appended into the queue.
 * @return Void.
 * @note
 */

void Queue_Append(SQueue *q, SMem *buf)
{
    if(OSA_MutexLock(&q->mtx, 100) == kStatus_OSA_Success)
    {
    	ASSERT(buf != NULL);
        if (q->u8Size == 0)
        {
        	//LREP("add first queue loc = %x\r\n", q);
            q->pHead = buf;
        }
        else
        {
            q->pTail->pNext = buf;
        }
        q->pTail = buf;

        buf->pNext = NULL;

        q->u8Size++;

#if 1
        if (q->pHead == NULL)
        {
            ASSERT("Corrupted queue: Null pointer has been queued" == 0);
        }
#endif

    }
    OSA_MutexUnlock(&q->mtx);

}/* Queue_Append */

/*****************************************************************************/
/*
 * @brief Reads or removes a buffer from queue
 *
 * @param q Queue from which buffer is to be read or removed.
 * @param mode Mode of operations. If this parameter has value REMOVE_MODE,
 *             buffer will be removed from queue and returned. If this parameter is
 *             READ_MODE, buffer pointer will be returned without
 *             removing from queue.
 * @param search Search criteria structure pointer.
 * @return Buffer header pointer, if the buffer is successfully
 *         removed or read, otherwise NULL is returned.
 */
static SMem *Queue_Read_Or_Remove(SQueue *q, SMode mode, SSearch *search)
{
    SMem *pBufCurrent = NULL;
    SMem *pBufPrevious;

    if(OSA_MutexLock(&q->mtx, 100) != kStatus_OSA_Success)
    	return NULL;
   
    if (q->u8Size != 0)
    {
    	//LREP("pop queue loc = %x\r\n", q);
        pBufCurrent  = q->pHead;
        pBufPrevious = q->pHead;
  
        if (NULL != search)
        {
            uint8_t match;

            while (NULL != pBufCurrent)
            {
                match = search->fCriteria((void *)pBufCurrent->u8Body,search->pHandle);
                if (match)
                {
    
                    break;
                }

                pBufPrevious = pBufCurrent;
                pBufCurrent = pBufCurrent->pNext;
            }
        }
        /* Buffer matching */
        if (NULL != pBufCurrent)
        {
            if (REMOVE_MODE == mode)
            {
                if (pBufCurrent == q->pHead)
                {
                    q->pHead = pBufCurrent->pNext;
                }
                else
                {
                    pBufPrevious->pNext = pBufCurrent->pNext;
                }

                if (pBufCurrent == q->pTail)
                {
                    q->pTail = pBufPrevious;
                }
                q->u8Size--;

                if (NULL == q->pHead)
                {
                    q->pTail = NULL;
                }
            }
            else
            {
                /* Nothing needs done*/
            }
        } else{
        	ASSERT(FALSE);
        }
    } /* q->u8Size != 0 */

    OSA_MutexUnlock(&q->mtx);

    return (pBufCurrent);

}/* Queue_Read_Or_Remove */
/*****************************************************************************/

/**
 * @brief Removes a buffer from queue.
 *
 *
 * @param q Queue from which buffer should be removed
 * @param search Search criteria. 
 * @return Pointer to the buffer header, if the buffer is
 * successfully removed, NULL otherwise.
 */
SMem *Queue_Remove(SQueue *q, SSearch *search)
{
    return (Queue_Read_Or_Remove(q, REMOVE_MODE, search));
}



/**
 * @brief Reads a buffer from queue.
 *
 * @param q The queue from which buffer should be read.
 * @param search Search criteria. 
 * @return Pointer to the buffer header which is to be read, NULL if the buffer
 * is not available
 */
SMem *Queue_Read(SQueue *q, SSearch *search)
{
    return (Queue_Read_Or_Remove(q, READ_MODE, search));
}

/*****************************************************************************
 * @brief Internal function for flushing a specific queue
 *
 * @param q Queue to be flushed
 * @return Void.
 * @note
 */
void Queue_Flush(SQueue *q)
{
    SMem *buf_to_free;

    while (q->u8Size > 0)
    {
        /* Remove the buffer from the queue and free it */
        buf_to_free = Queue_Remove(q, NULL);

        if (NULL == buf_to_free)
        {

#ifdef DEBUG
            ASSERT("Corrupted queue" == 0);
#endif
            q->u8Size = 0;
            return;
        }
        Mem_Free(buf_to_free);
    }
}

/*****************************************************************************
 * @brief Internal function for flushing a specific queue
 *
 * @param q Queue to be flushed
 * @return Void.
 * @note
 */
uint8_t Queue_GetSize(SQueue *q) {
	uint8_t size = 0;
	OSA_MutexLock(&q->mtx, 100);
	size = q->u8Size;
	OSA_MutexUnlock(&q->mtx);
	return size;
}

/*****************************************************************************/




/***************************** Include Files *********************************/
#include "app_cfg.h"
#include "Timer.h"
#include <stdlib.h>
/************************** Constant Definitions *****************************/

#if		(APP_PLATFORM == APP_STAND_ALONE)

	#define TIMER_ENTER_CRITICAL()    //DisableIntT1
	#define TIMER_EXIT_CRITICAL()     //EnableIntT1
	
#elif 	(APP_PLATFORM	== APP_RTOS)


	#define TIMER_ENTER_CRITICAL()    //CPU_CRITICAL_ENTER()
	#define TIMER_EXIT_CRITICAL()     //CPU_CRITICAL_EXIT()
	
#else
	#error Please define Platform, RTOS or Stand alone in app_cfg.h	
#endif

/**************************** Type Definitions *******************************/

SSysTick sSysTick = {0};

typedef struct _STickNode
{
        struct STickNode    	*pNext;
        uint8_t                	bRun;
        uint32_t              	u32Rate;           // rate of call
        uint32_t        		u32CurrCnt;        // current count value
        void*					pClbParam;
        void (*ClbFunc)(SYS_TICK currTick, void *pClbParam);      // function to be called

}STickNode;     // system tick registration node

typedef struct _STimerList
{
    STickNode*   tickHead;
    STickNode*   tickTail;
}STimerList;

STimerList sTimerList;
/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Timer_Init()
{
    TIMER_ENTER_CRITICAL();

    sTimerList.tickHead = 0;
    sTimerList.tickTail = 0;

    TIMER_EXIT_CRITICAL();

}
/*****************************************************************************/
/** @brief
 *          This function creates a timer based on the System Tick.
 *          It registrates a callback handler with the SystemTick ISR
 *          The handler will be called from within this ISR at specified rate.
 *
 *  @param  tickTimerHandler - handler to be called from the tick ISR
 *  @return a valid handle if the registration succeeded, NULL otherwise
 *  @note
 *          - All the registered handlers expiring in a current tick
 *            will be called in turn, sequentially.
 *            Therefore they have to be as short as possible.
 *          - The timer handler is called from within an ISR. All the
 *            ISR code restrictions apply.
 *          - The Timer_Create() should not be called from within an ISR.
 *          - The rate of which the System Tick based timer expires is set by the
 *            Timer_SetRate();
 */

TimerHandle Timer_Create(FClbTimer fClbTimer, void *pClbParam)
{
    STickNode*  newNode=0;

    if(fClbTimer)
    {
        newNode = malloc(sizeof(*newNode));
        if(newNode)
        {
            newNode->ClbFunc = fClbTimer;
            newNode->pClbParam = pClbParam;
            newNode->u32Rate = newNode->u32CurrCnt = 0;
            newNode->pNext = 0;
            newNode->bRun = false;

            TIMER_ENTER_CRITICAL();

            // add tail
            if(sTimerList.tickTail == 0)
            {
                sTimerList.tickHead = sTimerList.tickTail= newNode;
            }
            else
            {
                sTimerList.tickTail->pNext= (struct STickNode *)newNode;
                sTimerList.tickTail= newNode;
            }

            TIMER_EXIT_CRITICAL();

        }
    }

    return (TimerHandle)newNode;
        
}
/*****************************************************************************/
/** @brief
 *          This function deletes a System Tick timer previously created by
 *          Timer_Create() and registered with the System Tick ISR.
 *
 *  @param  tmrHandle - handle to a Tick Timer to be unregistered from the tick ISR
 *                      The handle should have been obtained by Timer_Create()
 *  @return TRUE if the deletion succeeded, false otherwise
 *  @note
 *          The Timer_Delete() should not be called from within an ISR.
 */

uint8_t Timer_Delete(TimerHandle tmrHandle)
{
    STickNode   *pTick, *prev;

    if(sTimerList.tickHead == 0)
    {
        return false;   // no registered handlers
    }

    TIMER_ENTER_CRITICAL();

    if((pTick=sTimerList.tickHead) == (STickNode*)tmrHandle)
    {   // remove head
        if(sTimerList.tickHead==sTimerList.tickTail)
        {
            sTimerList.tickHead=sTimerList.tickTail=0;
        }
        else
        {
            sTimerList.tickHead=(STickNode*)pTick->pNext;
        }
    }        
    else
    {
        for(prev=(STickNode*)sTimerList.tickHead, pTick=(STickNode*)sTimerList.tickHead->pNext;
        		pTick!=0; prev=(STickNode*)pTick, pTick=(STickNode*)pTick->pNext)
        {   // search within the list
            if(pTick == (STickNode*)tmrHandle)
            {   // found it
                prev->pNext=pTick->pNext;
                if(sTimerList.tickTail==pTick)
                {   // adjust tail
                    sTimerList.tickTail=prev;
                }
                break;
            }
        }
    }
    TIMER_EXIT_CRITICAL();

    if(pTick)
    {
        free(pTick);
    }

    return pTick != 0;        
        
}

/*****************************************************************************/
/** @brief
 *          This function sets the rate of a System Tick timer previously created by
 *          Timer_Create() and registered with the System Tick ISR.
 *
 *  @param  tmrHandle - handle to a Tick Timer to update the rate for.
 *                      The handle should have been obtained by Timer_Create()
 *          u32Rate   - current timer rate, in System Tick counts
 *  @return TRUE if the update succeeded, false otherwise.
 *  @note
 *          A timer with rate == 0 is disabled
 */

uint8_t Timer_SetRate(TimerHandle tmrHandle, uint32_t u32Rate)
{
    STickNode*  tNode;
    
    tNode = (STickNode*)tmrHandle;

    TIMER_ENTER_CRITICAL();

    tNode->u32Rate = tNode->u32CurrCnt = u32Rate;

    //LREP("\r\nRate = %d, Curr = %d",(uint32_t)tNode->u32Rate,(uint32_t)tNode->u32CurrCnt);
	
    TIMER_EXIT_CRITICAL();

    return true;


}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 *              
 */
uint32_t Timer_GetRate(TimerHandle tmrHandle)
{
    return (uint32_t)(((STickNode*)tmrHandle)->u32Rate);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 *              SystemTickCount variable is updated
 */

uint32_t		Timer_GetCurrCount(TimerHandle tmrHandle) {
	return (uint32_t)(((STickNode*)tmrHandle)->u32CurrCnt);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 *              SystemTickCount variable is updated
 */

void Timer_Update(void)
{
    STickNode   *pTick;

    for(pTick=(STickNode*)sTimerList.tickHead; pTick!=0; pTick=(STickNode*)pTick->pNext)
    {
        if((pTick->u32Rate != 0) && (pTick->bRun == true))
        {
            if(--pTick->u32CurrCnt==0)
            {
                pTick->u32CurrCnt=pTick->u32Rate;
                /* Callback function is called */
                (*pTick->ClbFunc)(sSysTick.u32SystemTickCount, pTick->pClbParam);
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
void Timer_Start(TimerHandle tmrHandle)
{
    STickNode*  pTick;

    pTick = (STickNode*)tmrHandle;

    TIMER_ENTER_CRITICAL();
    pTick->bRun = true;
    pTick->u32CurrCnt = pTick->u32Rate;
    TIMER_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Timer_StartAt(TimerHandle tmrHandle, uint32_t u32CurrCnt)
{
    STickNode*  pTick;

    pTick = (STickNode*)tmrHandle;

    TIMER_ENTER_CRITICAL();
    pTick->bRun = true;
    pTick->u32CurrCnt = u32CurrCnt;
    TIMER_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Timer_Stop(TimerHandle tmrHandle)
{
    STickNode*  pTick;

    pTick = (STickNode*)tmrHandle;

    TIMER_ENTER_CRITICAL();
    pTick->bRun = false;
    TIMER_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
//void Timer_Reset(TimerHandle tmrHandle, uint32_t u32CurrCnt)
void Timer_Reset(TimerHandle tmrHandle)
{
    STickNode*  pTick;

    pTick = (STickNode*)tmrHandle;

    TIMER_ENTER_CRITICAL();
    pTick->u32CurrCnt = pTick->u32Rate;
    //pTick->u32CurrCnt = u32CurrCnt;
    TIMER_EXIT_CRITICAL();
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
uint8_t Timer_IsRunning(TimerHandle tmrHandle)
{
    STickNode*  pTick;
    uint8_t  bRunning = false;
    pTick = (STickNode*)tmrHandle;

    TIMER_ENTER_CRITICAL();
    bRunning=pTick->bRun;
    TIMER_EXIT_CRITICAL();
    
    return bRunning;
}

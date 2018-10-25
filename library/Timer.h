
#ifndef _TIMER_H_
#define _TIMER_H_

/***************************** Include Files *********************************/
#include <stdint.h>
#include <stdbool.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


typedef uint32_t SYS_TICK;

typedef struct
{
        SYS_TICK   	 u32SystemTickCount;
        uint32_t     u32SystemTickResolution;
        uint32_t     u32SystemTickUpdateRate;

}SSysTick;

typedef void*    TimerHandle;  // handle to access the System Tick functions

//typedef void (*FClbTimer)(SYS_TICK currSysTick);

typedef void (*FClbTimer)(uint32_t currSysTick, void *pClbParam);


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

void        	Timer_Init();
TimerHandle 	Timer_Create    (FClbTimer fClbTimer, void *pClbParam);
uint8_t        	Timer_Delete    (TimerHandle tmrHandle);
uint8_t        	Timer_SetRate   (TimerHandle tmrHandle, uint32_t u32Rate);
uint32_t      	Timer_GetRate   (TimerHandle tmrHandle);
uint32_t		Timer_GetCurrCount(TimerHandle tmrHandle);
void        	Timer_Update    (void);
void        	Timer_Start     (TimerHandle tmrHandle);
void        	Timer_StartAt   (TimerHandle tmrHandle, uint32_t u32CurrCnt);
void        	Timer_Stop      (TimerHandle tmrHandle);
//void 			Timer_Reset		(TimerHandle tmrHandle, uint32_t u32CurrCnt);
void        	Timer_Reset     (TimerHandle tmrHandle);
uint8_t        	Timer_IsRunning (TimerHandle tmrHandle);

/************************** Variable Definitions *****************************/

extern SSysTick sSysTick;

#endif //_TIMER_H_

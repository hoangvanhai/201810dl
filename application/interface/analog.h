#ifndef ANALOG_INPUT_H_
#define ANALOG_INPUT_H_


#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/
#include <string.h>
#include "typedefs.h"
#include "fifo.h"
#include "app_cfg.h"
#include <includes.h>


/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

typedef enum
{
    /*General return values*/
    AR_SUCCESS 				= 	0,
	AR_FAILURE				=	1,
	AR_ERR_MEM				=  	2,
	AR_ERR_BUSY			=	10,
	AR_ERR_NOT_STARTED 	= 	11,
	AR_ERR_TIMEOUT		=	12,
	AR_ERR_INVALID_PTR	=	14,
	AR_ERR_INVALID_DATA = 	15,
	AR_ERR_FIFO			=   16,
	AR_ERR_CRCD			=	17,
}EAiReturn;


typedef struct _SAnalogReader
{
	uint32_t   			uartInstance;
	void*   			uartBase;
	uint32_t			u32BaudRate;
	SFIFO				sRecvFIFO;			// Receving FIFO
	uint8_t 			arrRecvFIFO[30];
} SAnalogReader;


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

void 		Analog_Uart_Init(uint32_t uartInstance, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio);
uint8_t  	Analog_Init(SAnalogReader *pAR,  uint32_t uartInstance,
							uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio);
int 		Analog_SelectChannel	(uint8_t channel);
int  		Analog_RecvData			(SAnalogReader *pAR, uint8_t* pData, uint16_t nSize);
int	 		Analog_GetRecvCount		(SAnalogReader *pAR);
void 		Analog_RecvFF_EnProtect	(SAnalogReader *pAR, BOOL bEn);
uint8_t   	Analog_RecvFF_Pop		(SAnalogReader *pAR);
void 		Analog_RecvFF_Reset		(SAnalogReader *pAR);

/************************** Variable Definitions *****************************/

/*****************************************************************************/

#endif

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

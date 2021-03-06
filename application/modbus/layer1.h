#ifndef MODBUS_LAYER1_H_
#define MODBUS_LAYER1_H_


#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/
#include <string.h>
#include "typedefs.h"
#include "fifo.h"
#include "app_cfg.h"
#include <includes.h>


#if (TRANSL1_VER == TRANSL1_V1)

/************************** Constant Definitions *****************************/
//#define DEBUG_TRANSL1

#define SIZE_FIFO_RECV		512
#define SIZE_FIFO_SEND		512

/**************************** Type Definitions *******************************/

typedef enum
{
    /*General return values*/
    MB_SUCCESS 				= 	0,
	MB_FAILURE				=	1,

	MB_ERR_MEM				=  	2,

    /*return value and error code for Modbus & TransL2*/
	MB_ERR_BUSY			=	10,
	MB_ERR_NOT_STARTED 	= 	11,
    MB_ERR_TIMEOUT		=	12,
	MB_ERR_INVALID_PTR	=	14,
	MB_ERR_INVALID_DATA = 	15,
	MB_ERR_FIFO			=   16,
	MB_ERR_CRCD			=	17,

}EMbReturn, EMbErrorCode, EMbStatus;


typedef union UMbFlag_ {
	uint8_t all;
	struct {
		unsigned bStarted: 1;
		unsigned bSending: 1;
		unsigned b3 : 1;
		unsigned b4 : 1;
		unsigned b5 : 1;
		unsigned b6 : 1;
		unsigned b7 : 1;
	}Bits;
}UMbFlag;

/* Layer 1 data struct*/
typedef struct _SModbus 
{
	UMbFlag				uFlag;
	uint32_t   			uartInstance;
	void*   			uartBase;
	uint32_t			u32BaudRate;
	uint8_t				pSendBuff[SIZE_FIFO_SEND];			// Buffer waiting to be sent
	uint16_t			u16SendSize;		// Sending size in BYTEs
	uint16_t			u16SendPtr;			// Current sending index
	SFIFO				sRecvFIFO;			// Receving FIFO
	uint8_t 			arrRecvFIFO[SIZE_FIFO_RECV];
	uint32_t			rs485Pin;
} SModbus;


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

void 		Modbus_Uart_Init(uint32_t uartInstance, uint32_t u32Baudrate, uint8_t u8TxPrio, uint8_t u8RxPrio);
uint8_t  	Modbus_Init( SModbus *pModbus,  uint32_t uartInstance,
							uint32_t u32BaudRate, uint8_t u8TxIntPrio, uint8_t u8RxIntPrio);

int  		Modbus_Send				(SModbus *pModbus, uint8_t* pData, uint16_t nSize);
int  		Modbus_Recv				(SModbus *pModbus, uint8_t* pData, uint16_t nSize);
int	 		Modbus_GetRecvCount		(SModbus *pModbus);
void 		Modbus_RecvFF_EnProtect	(SModbus *pModbus, BOOL bEn);
uint8_t   	Modbus_RecvFF_Pop		(SModbus *pModbus);
void 		Modbus_RecvFF_Reset		(SModbus *pModbus);
void		Modbus_SetSending		(SModbus *pModbus, BOOL send);
uint8_t		Modbus_SendAndRecv		(SModbus *pModbus, uint8_t *psData,
									 uint16_t sSize, uint8_t *prData,
									 uint16_t *rSize, uint16_t timeout);

/************************** Variable Definitions *****************************/

/*****************************************************************************/

#endif

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* MODBUS_LAYER1_H_ */

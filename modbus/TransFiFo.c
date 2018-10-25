

/***************************** Include Files *********************************/
#include "TransFiFo.h"


/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/

/** @brief 
 *
 *  @param
 *  @return Void.
 *  @note
 */
void FIFO_Create(SFIFO *pFF, uint8_t *arrBuff, uint16_t u16Size) {
	
	pFF->arrBuff	= arrBuff;//malloc(u16Size*sizeof(unsigned char));
	
	pFF->u16Size = 0;
	pFF->u16Head = 0;
	pFF->u16Tail = 0;
	pFF->bEnProtect    = FALSE;
	pFF->u16ProtectPtr = 0;
	pFF->u16MaxSize    = u16Size;//sizeof(pFF->arrBuff);
}

void FIFO_Destroy(SFIFO *pFF){
	//free(pFF->arrBuff);
}	
/** @brief 
 *
 *  @param
 *  @return Void.
 *  @note
 */
BOOL FIFO_Push(SFIFO *pFF, BYTE b) {
	if(pFF->u16Size < pFF->u16MaxSize) {
		pFF->arrBuff[pFF->u16Tail] = b;
		pFF->u16Tail++;				
		if(pFF->u16Tail >= pFF->u16MaxSize) {
			pFF->u16Tail = 0;
		}
		pFF->u16Size++;

		return TRUE;
	}	
	else{
		return FALSE;
	}
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
BOOL FIFO_Pop(SFIFO *pFF, BYTE *b) {
	
	//UART_ENTER_CRITICAL();
	
	if(pFF->u16Size != 0) {
		*b = pFF->arrBuff[pFF->u16Head];
		pFF->u16Head++;
		if(pFF->u16Head >= pFF->u16MaxSize) {
			pFF->u16Head = 0;
		}
		pFF->u16Size--;

		//UART_EXIT_CRITICAL();
		
		return TRUE;
	}		

	*b = 0xFF;
	
	//UART_EXIT_CRITICAL();
	
	return FALSE;
}
/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
void FIFO_EnableProtect(SFIFO *pFF) {
	//UART_ENTER_CRITICAL();
	pFF->bEnProtect  = TRUE;
	pFF->u16ProtectPtr = pFF->u16Head;
	//UART_EXIT_CRITICAL();
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
void FIFO_DisableProtect(SFIFO *pFF) {
	//UART_ENTER_CRITICAL();
	pFF->bEnProtect  = FALSE;
	//UART_EXIT_CRITICAL();
}
/** @brief FIFO_RewindHead
 *			Push back the FIFO some marked protected bytes.
 *			This function does not perform thread safe. User must take care of it.
 *  @param
 *  @return 
 *  @note
 */
 void FIFO_RewindHead(SFIFO *pFF) {
	//UART_ENTER_CRITICAL();
	if(pFF->bEnProtect) {
		while(pFF->u16Head != pFF->u16ProtectPtr) {
			if(pFF->u16Head == 0) {
				pFF->u16Head = pFF->u16MaxSize - 1;
			}
			else {
				pFF->u16Head--;
			}
			pFF->u16Size++;
			
			if(pFF->u16Head == pFF->u16Tail)
				break;
		}
	}	
	//UART_EXIT_CRITICAL();
}


/** @brief 	FIFO_IsEnablePush
 *			Check if possible to push more data into the FIFO
 *  @param
 *  @return 
 *  @note
 */
BOOL FIFO_IsEnablePush(SFIFO *pFF) {
	if(  (pFF->u16Size == pFF->u16MaxSize) ||
	     (pFF->bEnProtect == TRUE && pFF->u16Size  != 0 && pFF->u16Tail  == pFF->u16ProtectPtr) 
	  ) {
		return FALSE;	    	
	}
	
	return TRUE;
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
BOOL FIFO_IsEmpty(SFIFO *pFF) {
	return (pFF->u16Size == 0);
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
BOOL FIFO_IsFull(SFIFO *pFF) {
	return (FIFO_IsEnablePush(pFF) ? FALSE : TRUE);
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
uint16_t FIFO_GetCount(SFIFO *pFF) {
	return (pFF->u16Size);
}

/** @brief 
 *
 *  @param
 *  @return 
 *  @note
 */
BOOL FIFO_Reset(SFIFO *pFF) {
	pFF->u16Size 		= 0;
	pFF->u16Head 		= 0;
	pFF->u16Tail 		= 0;
	pFF->bEnProtect     = FALSE;
	pFF->u16ProtectPtr  = 0;	
}

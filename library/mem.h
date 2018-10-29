#ifndef SERIAL_COMM_MEM_H_
#define SERIAL_COMM_MEM_H_


/***************************** Include Files *********************************/
#include "typedefs.h"
#include "app_cfg.h"
#include <fsl_os_abstraction.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

typedef struct _SMem
{
	/** Pointer to next free buffer */
	struct _SMem *pNext;
	/** Pointer to the buffer body */
	uint8_t *u8Body;

} SMem;

/***************** Macros (Inline Functions) Definitions *********************/

#define MEM_BODY(buf) ((buf)->u8Body)
//#define MEM_BODY(buf) ((uint8_t*)(buf) + sizeof(SMem*))

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
extern "C" {
#endif

SMem* 	Mem_Alloc(uint16_t u16Size);
void 	Mem_Free(SMem *pbuffer);

/************************** Variable Definitions *****************************/

/*****************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* SERIAL_COMM_MEM_H_ */

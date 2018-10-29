
/***************************** Include Files *********************************/
#include "mem.h"
#include "queue.h"
#include "typedefs.h"
#include "app_cfg.h"
#include <fsl_debug_console.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

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
SMem *Mem_Alloc(uint16_t u16Size)
{
    SMem *pMem = NULL;
    pMem = (SMem*)OSA_FixedMemMalloc(u16Size);
    if(pMem != NULL) {
    	//pMem->u8Body = (uint8_t*)pMem;
    	pMem->pNext = (SMem*)pMem;
        pMem->u8Body = (uint8_t*)pMem + sizeof(SMem*);
    }
    return pMem;
}

/*****************************************************************************/
/**
 * @brief Frees up a buffer.
 *
 * @param pMem Pointer to buffer that has to be freed.
 * @return Void.
 */
void Mem_Free(SMem *pMem)
{
    ASSERT_VOID(NULL != pMem);
    OSA_FixedMemFree((uint8_t*)pMem);
    pMem = NULL;
}
/*****************************************************************************/




#ifndef RTOS_UCOSIII_SRC_UC_HELPER_OS_HELPER_H_
#define RTOS_UCOSIII_SRC_UC_HELPER_OS_HELPER_H_

/** @FILE NAME:    template.h
 *  @DESCRIPTION:  This file for ...
 *
 *  Copyright (c) 2018 EES Ltd.
 *  All Rights Reserved This program is the confidential and proprietary
 *  product of EES Ltd. Any Unauthorized use, reproduction or transfer
 *  of this program is strictly prohibited.
 *
 *  @Author: HaiHoang
 *  @NOTE:   No Note at the moment
 *  @BUG:    No known bugs.
 *
 *<pre>
 *  MODIFICATION HISTORY:
 *
 *  Ver   Who       Date                Changes
 *  ----- --------- ------------------  ----------------------------------------
 *  1.00  HaiHoang  August 1, 2018      First release
 *
 *
 *</pre>
 ******************************************************************************/

/***************************** Include Files *********************************/
#include "os.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
/************************** Constant Definitions *****************************/
#define STRING_MEM_DISPLAY_INFO     "FREE -> TOTAL = %d\r\n"
#define ERROR_DISPLAY               "Free up memory failed: %d\r\n"


// malloc, free helper
#define	OS_MEM_PARTITION_4                              4       // Not Used
#define	OS_MEM_PARTITION_8                              8       // Used
#define	OS_MEM_PARTITION_16                             16      // Used
#define	OS_MEM_PARTITION_32                             32      // Used
#define	OS_MEM_PARTITION_64                             64      // Used
#define	OS_MEM_PARTITION_128                            128		// Not Used
#define	OS_MEM_PARTITION_250                            250		// Not Used


#define OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK				20
#define OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK				30
#define OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK				10
#define OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK				8
#define OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK			4
#define OS_MEM_PARTITION_250_TOTAL_NUM_BLOCK			0


/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
// malloc, free helper
void*  	OSA_MemFixedMalloc(uint32_t uiSize);
void   	OSA_MemFixedFree(uint8_t* pucAllocMem);
uint8_t OSA_MemFixedInit(void);

/************************** Variable Definitions *****************************/

/*****************************************************************************/


#endif /* RTOS_UCOSIII_SRC_UC_HELPER_OS_HELPER_H_ */

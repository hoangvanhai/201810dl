/** @FILE NAME:    template.c
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
#include <os_helper.h>
#include <fsl_debug_console.h>
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

#define HELPER_TRACE_FREE_STATUS()		{ \
			if (ucError == OS_ERR_NONE)	{ \
				if(g_uiMallocCount) { \
					LREP(STRING_MEM_DISPLAY_INFO, --g_uiMallocCount); \
				} \
			} else { \
				LREP(ERROR_DISPLAY, (long)ucError); } \
			}

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
unsigned int		g_uiMallocCount		= 0;			// The number of memory allocated dynamically

#if OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK > 0
OS_MEM		g_hMemPartition8;
#endif
#if OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK > 0
OS_MEM		g_hMemPartition16;
#endif
#if OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK > 0
OS_MEM		g_hMemPartition32;
#endif
#if OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK > 0
OS_MEM		g_hMemPartition64;
#endif
#if OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK > 0
OS_MEM		g_hMemPartition128;
#endif
#if OS_MEM_PARTITION_250_TOTAL_NUM_BLOCK > 0
OS_MEM		g_hMemPartition250;
#endif


    /*
    For each memory block we need to reserve two first bytes for storing partition ID.
    We also MUST carefully remember that the return pointer of allocated memory to the user
    must be even address, unless we will get some troubles when allocating memory for structure
    that has some big data elements(int, long, ...).This problem is introduced by the Stack Pointer
    MUST point to even address, not odd address. The two last bytes save the partition ID two additional
    bytes (unused by the user)
    */

#if OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK > 0
uint8_t		*g_ucMemStorage8[OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK][10];
#endif
#if OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK > 0
uint8_t		*g_ucMemStorage16[OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK][18];
#endif
#if OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK > 0
uint8_t		*g_ucMemStorage32[OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK][34];
#endif
#if OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK > 0
uint8_t		*g_ucMemStorage64[OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK][66];
#endif
#if OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK > 0
uint8_t		*g_ucMemStorage128[OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK][130];
#endif
#if OS_MEM_PARTITION_250_TOTAL_NUM_BLOCK > 0
uint8_t		*g_ucMemStorage250[OS_MEM_PARTITION_250_TOTAL_NUM_BLOCK][252];
#endif

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Allocate a memory area use the fixed-size partition of OS
//
//	@param	: 	uiSize is size of the memory needed to be allocated
//	@return	: 	Pointer to the allocated memory in the partition. NULL means unsuccessful
//	@note	:	For each memory block we need to reserve two first bytes for storing partition ID.
//				and we MUST carefully remember that the return pointer of allocated memory to the user
//				must be even address, unless we will get some troubles when allocating memory for
//				data structure that has some big data elements(int, long, ...). This problem is
//				introduced by the Stack Pointer MUST point to even address, not odd address.
// ---------------------------------------------------------------------------------------------------
void* OSA_MemFixedMalloc(uint32_t uiSize)
{
	uint8_t* 	pucAllocMem = NULL;
	OS_ERR		ucError		= OS_ERR_NONE;

	// Check the required allocated size to allocate at the most proper partition
#if OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK > 0
	if (uiSize <= OS_MEM_PARTITION_8)
	{

		// Get memory from partition
		pucAllocMem = OSMemGet(&g_hMemPartition8, &ucError);
		// Save the ID of partition as the actual allocated memory
		uiSize = OS_MEM_PARTITION_8;

	}
#endif
#if OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK > 0
	else if ((uiSize > OS_MEM_PARTITION_8) && (uiSize <= OS_MEM_PARTITION_16))
	{

	{
		// Get memory from partition
		pucAllocMem = OSMemGet(&g_hMemPartition16, &ucError);
		// Save the ID of partition as the actual allocated memory
		uiSize = OS_MEM_PARTITION_16;
	}
	}
#endif
#if OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK > 0
	else if ((uiSize > OS_MEM_PARTITION_16) && (uiSize <= OS_MEM_PARTITION_32))
	{

		{
			// Get memory from partition
			pucAllocMem = OSMemGet(&g_hMemPartition32, &ucError);
			// Save the ID of partition as the actual allocated memory
			uiSize = OS_MEM_PARTITION_32;
		}
	}
#endif
#if OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK > 0
	else if ((uiSize > OS_MEM_PARTITION_32) && (uiSize <= OS_MEM_PARTITION_64))
	{

		{
			// Get memory from partition
			pucAllocMem = OSMemGet(&g_hMemPartition64, &ucError);
			// Save the ID of partition as the actual allocated memory
			uiSize = OS_MEM_PARTITION_64;
		}
	}
#endif
#if OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK > 0

	else if ((uiSize > OS_MEM_PARTITION_64) && (uiSize <= OS_MEM_PARTITION_128))
	{

		{
			// Get memory from partition
			pucAllocMem = OSMemGet(&g_hMemPartition128, &ucError);
			// Save the ID of partition as the actual allocated memory
			uiSize = OS_MEM_PARTITION_128;

		}
	}
#endif
#if OS_MEM_PARTITION_250_TOTAL_NUM_BLOCK > 0
	else if ((uiSize > OS_MEM_PARTITION_128) && (uiSize <= OS_MEM_PARTITION_250))
	{
		if (g_hMemPartition250 != NULL)
		{
			// Get memory from partition
			pucAllocMem = OSMemGet(g_hMemPartition250, &ucError);
			// Save the ID of partition as the actual allocated memory
			uiSize = OS_MEM_PARTITION_250;
		}
	}
#endif

	else {
		LREP("Invalid size memory require: %d\r\n", (long)uiSize);
	}

	if (ucError != OS_ERR_NONE) {
		LREP("Allocate memory failed: %d\r\n", (long)ucError);
	}


	if (pucAllocMem != NULL) {
		uint16_t*	puiPartitionID = (uint16_t*)pucAllocMem;
		// Save the ID of partition at the first & second bytes - two additional bytes (unused by the user)
		*puiPartitionID	= uiSize;
		g_uiMallocCount++; // Count up the number of allocated memory
		LREP("MALLOC -> TOTAL = %d\r\n", g_uiMallocCount);
		//LREP("\r\nMalloc ADDR 0x%x", (&pucAllocMem[2]));
		return ((void*)&pucAllocMem[2]);
	} else  {
		return NULL;
	}
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Free a allocated memory area
//
//	@param	: 	pucAllocMem is the Pointer to the allocated memory in the partition
//	@return	: 	Void
//	@note	:
// ---------------------------------------------------------------------------------------------------
void OSA_MemFixedFree(uint8_t* pucAllocMem)
{
	OS_ERR	ucError = OS_ERR_NONE;

	// The real allocated memory start at two locations before the first element of pucAllocMem
	uint8_t*	pucCompleteAllocMem = pucAllocMem - 2;
	// Get the partition ID which stored at the two first bytes
	uint16_t*	puiPartitionID		= (uint16_t*)pucCompleteAllocMem;

	// Check the partition ID of the freed required memory to free it up to the original partition
	switch((uint16_t)*puiPartitionID)
	{
#if OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK > 0
		case OS_MEM_PARTITION_8:
			OSMemPut(&g_hMemPartition8, (void*) pucCompleteAllocMem, &ucError);
			HELPER_TRACE_FREE_STATUS();
			break;
#endif
#if OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK > 0

		case OS_MEM_PARTITION_16:
			OSMemPut(&g_hMemPartition16, (void*) pucCompleteAllocMem, &ucError);
			HELPER_TRACE_FREE_STATUS();
			break;
#endif
#if OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK > 0

		case OS_MEM_PARTITION_32:
			OSMemPut(&g_hMemPartition32, (void*) pucCompleteAllocMem, &ucError);
			HELPER_TRACE_FREE_STATUS();
			break;
#endif
#if OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK > 0
		case OS_MEM_PARTITION_64:
			OSMemPut(&g_hMemPartition64, (void*) pucCompleteAllocMem, &ucError);
			HELPER_TRACE_FREE_STATUS();
			break;
#endif
#if OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK > 0
		case OS_MEM_PARTITION_128:
			OSMemPut(&g_hMemPartition128, (void*) pucCompleteAllocMem, &ucError);
			HELPER_TRACE_FREE_STATUS();
			break;
#endif
#if OS_MEM_PARTITION_250_TOTAL_NUM_BLOCK > 0
		case OS_MEM_PARTITION_250:
			OSMemPut(g_hMemPartition250, (void*) pucCompleteAllocMem, &ucError);
			HELPER_TRACE_FREE_STATUS();
#endif
		default:
			LREP("\r\npPartitionID: %d PartitionID = 0x%x Wrong Param", (uint16_t)(*puiPartitionID), puiPartitionID);
			break;
	}
}


uint8_t OSA_MemFixedInit(void)
{
    OS_ERR   ucError = OS_ERR_NONE;

    #if OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK > 0
    OSMemCreate(&g_hMemPartition8,
    			(CPU_CHAR*)"mem8",
				&g_ucMemStorage8[0][0],
				OS_MEM_PARTITION_8_TOTAL_NUM_BLOCK,
				10,
				&ucError);

    if(ucError != OS_ERR_NONE)
    {
        LREP("\r\nCan not Create Partition 8 bytes");
        return ucError;
    }
    #endif
    #if OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK > 0

    OSMemCreate(&g_hMemPartition16,
    			(CPU_CHAR*)"mem16",
				&g_ucMemStorage16[0][0],
				OS_MEM_PARTITION_16_TOTAL_NUM_BLOCK,
				18,
				&ucError);

    if(ucError != OS_ERR_NONE)
    {
        LREP("\r\nCan not Create Partition 16 bytes");
        return ucError;
    }
    #endif
    #if OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK > 0

    OSMemCreate(&g_hMemPartition32,
       			(CPU_CHAR*)"mem32",
   				&g_ucMemStorage32[0][0],
				OS_MEM_PARTITION_32_TOTAL_NUM_BLOCK,
   				34,
   				&ucError);


    if(ucError != OS_ERR_NONE)
    {
        LREP("\r\nCan not Create Partition 32 bytes");
        return ucError;
    }
    #endif
    #if OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK > 0

    OSMemCreate(&g_hMemPartition64,
       			(CPU_CHAR*)"mem64",
   				&g_ucMemStorage64[0][0],
				OS_MEM_PARTITION_64_TOTAL_NUM_BLOCK,
   				66,
   				&ucError);

    if(ucError != OS_ERR_NONE)
    {
        LREP("\r\nCan not Create Partition 64 bytes");
        return ucError;
    }
    #endif
    #if OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK > 0
    OSMemCreate(&g_hMemPartition128,
       			(CPU_CHAR*)"mem128",
   				&g_ucMemStorage128[0][0],
				OS_MEM_PARTITION_128_TOTAL_NUM_BLOCK,
				130,
   				&ucError);

    if(ucError != OS_ERR_NONE)
    {
        LREP("\r\nCan not Create Partition 128 bytes");
    }
    #endif
    //LREP("\r\nMemory is initialized!");
    return ucError;
}








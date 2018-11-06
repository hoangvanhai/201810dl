
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
#include "fsl_debug_console.h"
#include <board.h>
#include <includes.h>
#include <fsl_sdhc_card.h>
#include <app.h>
#include <task_filesystem.h>
#include <ff.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
int init_filesystem();
/************************** Variable Definitions *****************************/
FATFS SDCARDFS;
/*****************************************************************************/


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

int init_filesystem() {
	FRESULT	retVal;
	FIL		writer;

    retVal = f_mount(1, &SDCARDFS);

    if(retVal != FR_OK) {
    	LREP("fat fs init error: %d\r\n", retVal);
    } else {
    	LREP("fat fs init successful !\r\n");

//    	OSA_EnterCritical(kCriticalLockSched);
    	retVal = f_mkdir("1:this_dir");
//    	OSA_ExitCritical(kCriticalLockSched);
    	if(retVal != FR_OK) {
    		LREP("mkdir err = %d\r\n", retVal);
    	} else {
    		LREP("mkdir successful !\r\n");
    	}

//        retVal = f_open(&writer, "1:this_dir/FILE1.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
//
//        if(retVal != FR_OK) {
//        	LREP("open file error: %d\r\n", retVal);
//        } else {
//    		uint32_t byte_written, size;
//    		size = f_size(&writer);
//    		LREP("file size = %d\r\n", size);
//    		if(size > 0) {
//    			LREP("file size = %d bytes\r\n", size);
//    		} else {
//    			char* msg = "this is test message write to file";
//				retVal = f_write(&writer, (void*)msg, strlen(msg), &byte_written);
//				if(retVal != FR_OK) {
//					LREP("write to byte failed err: %d\r\n", retVal);
//				} else {
//					if(byte_written != strlen(msg)) {
//						LREP("write to file missing data, writereq = %d - writeact: %d\r\n", strlen(msg), byte_written);
//					} else {
//						LREP("write to file totally successful !\r\n");
//					}
//				}
//    		}
//        }
//
//        f_close(&writer);

    }

	return retVal;
}





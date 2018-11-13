
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

bool check_file_existed(const char *path) {
    FRESULT fr;
    FILINFO fno;
    bool retVal = false;
    fr = f_stat(path, &fno);

    switch (fr) {
    case FR_OK:
        LREP("Size: %lu\n", fno.fsize);
        /*LREP("Timestamp: %u/%02u/%02u, %02u:%02u\r\n",
               (fno.fdate >> 9) + 1980,
			   fno.fdate >> 5 & 15,
			   fno.fdate & 31,
               fno.ftime >> 11,
			   fno.ftime >> 5 & 63); */

        LREP("Attributes: %c%c%c%c%c\r\n",
               (fno.fattrib & AM_DIR) ? 'D' : '-',
               (fno.fattrib & AM_RDO) ? 'R' : '-',
               (fno.fattrib & AM_HID) ? 'H' : '-',
               (fno.fattrib & AM_SYS) ? 'S' : '-',
               (fno.fattrib & AM_ARC) ? 'A' : '-');

        retVal = true;
        break;

    case FR_NO_FILE:
    	LREP("It is not exist.\r\n");
        break;

    default:
    	LREP("An error occured. (%d)\r\n", fr);
    }

    return retVal;
}


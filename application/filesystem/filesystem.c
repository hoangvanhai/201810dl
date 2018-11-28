
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
#include <filesystem.h>
#include <ff.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

/*****************************************************************************/


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

bool check_obj_existed(const char *path) {
    FRESULT fr;
    static FILINFO fno;
    bool retVal = false;
    fr = f_stat(path, &fno);

    switch (fr) {
    case FR_OK:
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

bool obj_stat(const char* path) {
    FRESULT fr;
    static FILINFO fno;
    bool retVal = false;
    fr = f_stat(path, &fno);

    switch (fr) {
    case FR_OK:
        LREP("Size: %lu\r\n", fno.fsize);
        LREP("Timestamp: %u/%02u/%02u, %02u:%02u\r\n",
               (fno.fdate >> 9) + 1980,
			   fno.fdate >> 5 & 15,
			   fno.fdate & 31,
               fno.ftime >> 11,
			   fno.ftime >> 5 & 63);

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
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int show_content (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    static FILINFO fno;
    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {

    	do {
			res = f_readdir(&dir, &fno);                   /* Read a directory item */
			if(res == FR_OK && fno.fname[0] != 0) {

				LREP("%-15s%10lu byte ", fno.fname, fno.fsize);
		        /*LREP("Timestamp: %u/%02u/%02u, %02u:%02u\t",
		               (fno.fdate >> 9) + 1980,
					   fno.fdate >> 5 & 15,
					   fno.fdate & 31,
		               fno.ftime >> 11,
					   fno.ftime >> 5 & 63);*/

		        LREP("attr: %c%c%c%c%c\r\n",
		               (fno.fattrib & AM_DIR) ? 'D' : '-',
		               (fno.fattrib & AM_RDO) ? 'R' : '-',
		               (fno.fattrib & AM_HID) ? 'H' : '-',
		               (fno.fattrib & AM_SYS) ? 'S' : '-',
		               (fno.fattrib & AM_ARC) ? 'A' : '-');

			}
    	} while(res == FR_OK && fno.fname[0] != 0);

        f_closedir(&dir);
    }

    return res;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int current_directory() {
	int err = -1;
	uint8_t *buffer = OSA_FixedMemMalloc(250);
	if(buffer != NULL) {
		int len = 250;
		memset(buffer, 0, 250);
		err = f_getcwd((char*)buffer, len);
		if(err == FR_OK) {
			memcpy((char*)pAppObj->currPath, buffer, strlen((const char*)buffer));
			LREP("current dir = %s\r\n", pAppObj->currPath);
		} else {
			LREP("get current dir failed \r\n");
		}
	}
	OSA_FixedMemFree(buffer);
	return err;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

int remove_directory(char *path) {
	UINT i, j;
	FRESULT fr;
	DIR dir;
	static FILINFO fno;

#if _USE_LFN
	fno.fname = 0; /* Disable LFN output */
#endif
	fr = f_opendir(&dir, path);
	if (fr == FR_OK) {
		for (i = 0; path[i]; i++) ;
		path[i++] = '/';
		for (;;) {
			fr = f_readdir(&dir, &fno);
			if (fr != FR_OK || !fno.fname[0]) break;
			if (FF_FS_RPATH && fno.fname[0] == '.') continue;
			j = 0;
			do
				path[i+j] = fno.fname[j];
			while (fno.fname[j++]);
			if (fno.fattrib & AM_DIR) {
				fr = remove_directory(path);
				if (fr != FR_OK) break;
			}
			fr = f_unlink(path);
			if (fr != FR_OK) break;
		}
		path[--i] = '\0';
		f_closedir(&dir);
	}

	if (fr == FR_OK) fr = f_unlink(path);  /* Delete the empty directory */

	return fr;
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

int delete_node (
    TCHAR* path,    /* Path name buffer with the sub-directory to delete */
    UINT sz_buff,   /* Size of path name buffer (items) */
    FILINFO* fno    /* Name read buffer */
)
{
    UINT i, j;
    FRESULT fr;
    DIR dir;


    fr = f_opendir(&dir, path); /* Open the directory */
    if (fr != FR_OK) return fr;

    for (i = 0; path[i]; i++) ; /* Get current path length */
    path[i++] = _T('/');

    for (;;) {
        fr = f_readdir(&dir, fno);  /* Get a directory item */
        if (fr != FR_OK || !fno->fname[0]) break;   /* End of directory? */
        j = 0;
        do {    /* Make a path name */
            if (i + j >= sz_buff) { /* Buffer over flow? */
                fr = 100; break;    /* Fails with 100 when buffer overflow */
            }
            path[i + j] = fno->fname[j];
        } while (fno->fname[j++]);
        if (fno->fattrib & AM_DIR) {    /* Item is a directory */
            fr = delete_node(path, sz_buff, fno);
        } else {                        /* Item is a file */
            fr = f_unlink(path);
        }
        if (fr != FR_OK) break;
    }

    path[--i] = 0;  /* Restore the path name */
    f_closedir(&dir);

    if (fr == FR_OK) fr = f_unlink(path);  /* Delete the empty directory */
    return fr;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int cat(char *path) {
	int retVal;
	FIL file;
	uint8_t *line = OSA_FixedMemMalloc(100);

	if(line != NULL) {
		UINT read;
		retVal = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
		if(retVal == FR_OK) {
			int fr;
			do {
				memset(line, 0, 100);
				fr = f_read(&file, line, 100, &read);
				if(read > 0) {
					LREP("%s", line);
				}
			} while(fr == FR_OK && read > 0);
		} else {
			LREP("cat file err = %d\r\n", retVal);
		}

		OSA_FixedMemFree(line);
	} else {
		retVal = -1;
	}

	return retVal;
}


















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
#include <assert.h>
#include "fsl_debug_console.h"
#include <includes.h>
#include <board.h>
//#include <app.h>
#include <task_filesystem.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

const char *logo_msg = { "\r\n\n"
		"  *******  ******     ***    \r\n"
		"  **   *   **   *    *   *   \r\n"
		"  *        *          *       \r\n"
		" *****    *****        *      \r\n"
		" **       **            *     \r\n"
		" *        *        *     *     \r\n"
		"**     *  **     * *     *      \r\n"
		"*******  ********   ****        \r\n" };



uint32_t gSlaveId;

void App_Periodic(void *param);
FATFS FatFs_SDCARD;

FIL writer;
OSA_TASK_DEFINE(task_periodic,		TASK_PERIODIC_STACK_SIZE);

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
#if 1
int main(void)
{
#if 0
    osa_status_t result;

    result = kStatus_OSA_Error;
    hardware_init();
    GPIO_DRV_Init(NULL, ledPins);

    GPIO_DRV_Init(sdhcCdPin, NULL);

    LREP(logo_msg);
    LREP("Application started built time: " __TIME__ " " __DATE__ "\r\n");

    // Configure the power mode protection
    SMC_HAL_SetProtection(SMC_BASE_PTR, kAllowPowerModeVlp);

    // get cpu uid low value for slave
    gSlaveId = SIM_UIDL_UID(SIM_BASE_PTR);

    LREP("chip id = 0x%x\r\n", gSlaveId);
    LREP("OS Tick rate = %d\r\n", OSCfg_TickRate_Hz);

    LREP("size of tag = %d\r\n", sizeof(STag));

    OSA_Init();

    OSA_FixedMemInit();

    App_Init(&sApp);

    App_CreateAppTask(&sApp);

    OSA_Start();

    for(;;) {}                    // Should not achieve here
}


int App_CreateAppTask(SApp *pApp) {

	osa_status_t result;
	LREP("start create app task \r\n");
    result = OSA_TaskCreate(App_TaskModbus,
                    (uint8_t *)"modbus",
                    TASK_MODBUS_STACK_SIZE,
					pApp->task_modbus_stack,
                    TASK_MODBUS_PRIO,
                    (task_param_t)pApp,
                    false,
                    &pApp->task_modbus_task_handler);
    if(result != kStatus_OSA_Success)
    {
        LREP("Failed to create slave task\r\n\r\n");
        return -1;
    }

//    result = OSA_TaskCreate(task_filesystem,
//                    (uint8_t *)"filesystem",
//                    TASK_FILESYSTEM_STACK_SIZE,
//                    task_filesystem_stack,
//                    TASK_FILESYSTEM_PRIO,
//                    (task_param_t)0,
//                    false,
//                    &pApp->task_filesystem_task_handler);
//    if (result != kStatus_OSA_Success)
//    {
//        LREP("Failed to create filesystem task\r\n\r\n");
//        return -1;
//    }


//    result = OSA_TaskCreate(App_TaskSerialcomm,
//                    (uint8_t *)"serialcomm",
//                    TASK_SERIAL_COMM_STACK_SIZE,
//					pApp->task_serialcomm_stack,
//                    TASK_SERIALCOMM_PRIO,
//                    (task_param_t)pApp,
//                    false,
//                    &pApp->task_serialcomm_task_handler);
//    if (result != kStatus_OSA_Success)
//    {
//        LREP("Failed to create serialcomm task\r\n\r\n");
//        return -1;
//    }

    //  create app tasks
	result = OSA_TaskCreate(App_TaskPeriodic,
					(uint8_t *)"periodic",
					TASK_PERIODIC_STACK_SIZE,
					pApp->task_periodic_stack,
					TASK_PERIODIC_PRIO,
					(task_param_t)pApp,
					false,
					&pApp->task_periodic_task_handler);
	if (result != kStatus_OSA_Success)
	{
		LREP("Failed to create periodic task\r\n\r\n");
		return -1;
	}


    // create app tasks
    result = OSA_TaskCreate(App_TaskShell,
                    (uint8_t *)"shell",
                    TASK_SHELL_STACK_SIZE,
                    pApp->task_shell_stack,
                    TASK_SHELL_PRIO,
                    (task_param_t)pApp,
                    false,
                    &pApp->task_shell_task_handler);
    if (result != kStatus_OSA_Success)
    {
        LREP("Failed to create master task\r\n\r\n");
        return -1;
    }



	return result;
#endif

    uint32_t i;
    osa_status_t retVal;
    // initialize target hw
    hardware_init();

    // initialize the OS services
    OSA_Init();

    LREP("Application started \r\n");
    // enable SDHC module instance
    CLOCK_SYS_EnableSdhcClock(0);

#ifdef MPU_INSTANCE_COUNT
    // disable MPU
    for(i = 0; i < MPU_INSTANCE_COUNT; i++)
    {
        MPU_HAL_Disable(g_mpuBase[i]);
    }
#endif


    retVal = OSA_TaskCreate(App_Periodic,
					(uint8_t *)"periodic",
					TASK_PERIODIC_STACK_SIZE,
					task_periodic_stack,
					TASK_PERIODIC_PRIO,
					(task_param_t)0,
					false,
					&task_periodic_task_handler);
	if (retVal != kStatus_OSA_Success)
	{
		LREP("Failed to create periodic task\r\n\r\n");
		return -1;
	} else {
		LREP("Task create successfully \r\n");
	}




    OSA_Start();

    for(;;) {}                    // Should not achieve here

    return 0;
}



const char *msg = "FS Waking up ok " __TIME__ " " __DATE__;

void App_FS() {

	int retVal;
	memset(&FatFs_SDCARD, 0, sizeof(FATFS));

	retVal = f_mount(1, &FatFs_SDCARD);

	if(retVal != FR_OK) {
		PRINTF("fat fs init error: %d\r\n", retVal);
	} else {
		PRINTF("fat fs init successful !\r\n");

		retVal = f_mkdir("1:this_dir");
		if(retVal != FR_OK) {
			PRINTF("mkdir err = %d\r\n", retVal);
		} else {
			PRINTF("mkdir successful !\r\n");
		}

		retVal = f_open(&writer, "1:this_dir/FILE1.txt", FA_CREATE_ALWAYS | FA_WRITE | FA_READ);

		if(retVal != FR_OK) {
			PRINTF("open file error: %d\r\n", retVal);
		} else {
			uint32_t byte_written, size;
			size = f_size(&writer);
			PRINTF("file size = %d\r\n", size);
			if(size > 0) {
				PRINTF("file size = %d bytes\r\n", size);
			} else {

				retVal = f_write(&writer, (void*)msg, strlen(msg), &byte_written);
				if(retVal != FR_OK) {
					PRINTF("write to byte failed err: %d\r\n", retVal);
				} else {
					if(byte_written != strlen(msg)) {
						PRINTF("write to file missing data, writereq = %d - writeact: %d\r\n", strlen(msg), byte_written);
					} else {
						PRINTF("write to file totally successful !\r\n");
					}
				}
			}
		}

		f_close(&writer);

	}
}

void App_Periodic(void *param) {

	bool run = false;

	while(1) {
		if(run == false) {
			run = true;
			App_FS();
		}
		OSA_TimeDelay(1000);
	}
}


#endif


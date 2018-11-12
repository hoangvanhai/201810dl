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
#include <app.h>
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
    hardware_init();
    LREP(logo_msg);
    LREP("Application started built time: " __TIME__ " " __DATE__ "\r\n");

    // get cpu uid low value for slave
    gSlaveId = SIM_UIDL_UID(SIM_BASE_PTR);

    LREP("chip id = 0x%x\r\n", 		gSlaveId);
    LREP("OS Tick rate = %d\r\n", 	OSCfg_TickRate_Hz);
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
//    result = OSA_TaskCreate(App_TaskModbus,
//                    (uint8_t *)"modbus",
//                    TASK_MODBUS_STACK_SIZE,
//					pApp->task_modbus_stack,
//                    TASK_MODBUS_PRIO,
//                    (task_param_t)pApp,
//                    false,
//                    &pApp->task_modbus_task_handler);
//    if(result != kStatus_OSA_Success)
//    {
//        LREP("Failed to create slave task\r\n\r\n");
//        return -1;
//    }

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

}


#endif


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
#include <hw_profile.h>
#include <board.h>
#include <app.h>

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
//void Clb_TimerControl(void *p_tmr, void *p_arg);
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

OSA_TASK_DEFINE(task_shell, 		TASK_SHELL_STACK_SIZE);
OSA_TASK_DEFINE(task_filesystem, 	TASK_FILESYSTEM_STACK_SIZE);
OSA_TASK_DEFINE(task_modbus, 		TASK_MODBUS_STACK_SIZE);

//OS_TMR	hTimer;

uint32_t gSlaveId;

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int main(void)
{
    osa_status_t result;

    result = kStatus_OSA_Error;
    hardware_init();
    GPIO_DRV_Init(NULL, ledPins);

    LREP(logo_msg);
    LREP("Application started built time: " __TIME__ " " __DATE__ "\r\n");

    // Configure the power mode protection
    SMC_HAL_SetProtection(SMC_BASE_PTR, kAllowPowerModeVlp);

    // get cpu uid low value for slave
    gSlaveId = SIM_UIDL_UID(SIM_BASE_PTR);

    LREP("chip id = 0x%x\r\n", gSlaveId);

    OSA_Init();

    result = OSA_TaskCreate(task_modbus,
                    (uint8_t *)"modbus",
                    TASK_MODBUS_STACK_SIZE,
                    task_modbus_stack,
                    TASK_MODBUS_PRIO,
                    (task_param_t)0,
                    false,
                    &task_modbus_task_handler);
    if(result != kStatus_OSA_Success)
    {
        LREP("Failed to create slave task\r\n\r\n");
        return -1;
    }

    result = OSA_TaskCreate(task_filesystem,
                    (uint8_t *)"filesystem",
                    TASK_FILESYSTEM_STACK_SIZE,
                    task_filesystem_stack,
                    TASK_FILESYSTEM_PRIO,
                    (task_param_t)0,
                    false,
                    &task_filesystem_task_handler);
    if (result != kStatus_OSA_Success)
    {
        LREP("Failed to create sample task\r\n\r\n");
        return -1;
    }


    // create app tasks
    result = OSA_TaskCreate(task_shell,
                    (uint8_t *)"shell",
                    TASK_SHELL_STACK_SIZE,
                    task_shell_stack,
                    TASK_SHELL_PRIO,
                    (task_param_t)0,
                    false,
                    &task_shell_task_handler);
    if (result != kStatus_OSA_Success)
    {
        LREP("Failed to create master task\r\n\r\n");
        return -1;
    }



    OSA_Start();

    for(;;) {}                    // Should not achieve here
}




/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
//void Clb_TimerControl(void *p_tmr, void *p_arg) {
//	LREP("timer clb\r\n");
//}



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

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
void App_TaskStartup(task_param_t arg);
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



/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
#if 1


void delay_us(uint32_t us) {
	while(us--) {
		__NOP();
	}
}

int main(void)
{
	osa_status_t result;
    hardware_init();
    LREP(logo_msg);
    LREP("Application started built time: " __TIME__ " " __DATE__ "\r\n");

    LREP("chip id = 0x%x\r\n", 		SIM_UIDL_UID(SIM_BASE_PTR));
    LREP("OS Tick rate = %d\r\n", 	OSCfg_TickRate_Hz);

    BOARD_CheckResetCause();

#if 0
    LREP("MCG->C1 %x\r\n", 		MCG->C1);
	LREP("MCG->C2 %x\r\n", 		MCG->C2);     /**< MCG Control 2 Register, offset: 0x1 */
	LREP("MCG->C3 %x\r\n", 		MCG->C3);     /**< MCG Control 3 Register, offset: 0x2 */
	LREP("MCG->C4 %x\r\n", 		MCG->C4);     /**< MCG Control 4 Register, offset: 0x3 */
	LREP("MCG->C5 %x\r\n", 		MCG->C5);     /**< MCG Control 5 Register, offset: 0x4 */
	LREP("MCG->C6 %x\r\n", 		MCG->C6);     /**< MCG Control 6 Register, offset: 0x5 */
	LREP("MCG->S %x\r\n", 		MCG->S);      /**< MCG Status Register, offset: 0x6 */
	LREP("MCG->SC %x\r\n", 		MCG->SC);     /**< MCG Status and Control Register, offset: 0x8 */
	LREP("MCG-> ATCVH %x\r\n", 	MCG->ATCVH); /**< MCG Auto Trim Compare Value High Register, offset: 0xA */
	LREP("MCG->ATCVL %x\r\n", 	MCG->ATCVL);  /**< MCG Auto Trim Compare Value Low Register, offset: 0xB */
	LREP("MCG->C7 %x\r\n", 		MCG->C7);     /**< MCG Control 7 Register, offset: 0xC */
	LREP("MCG->C8 %x\r\n", 		MCG->C8);     /**< MCG Control 8 Register, offset: 0xD */
	LREP("MCG->C9 %x\r\n", 		MCG->C9);     /**< MCG Control 9 Register, offset: 0xE */
	LREP("MCG->C11 %x\r\n", 	MCG->C11);    /**< MCG Control 11 Register, offset: 0x10 */
	LREP("MCG->S2 %x\r\n", 		MCG->S2);

	LREP("sysclock = %d\r\n", SystemCoreClock);

	LREP("CLOCK_SYS_GetEnetTimeStampFreq %d\r\n",
			CLOCK_SYS_GetEnetTimeStampFreq(0));
	LREP("CLOCK_SYS_GetEnetRmiiFreq %d\r\n",
			CLOCK_SYS_GetEnetRmiiFreq(0));
#endif


    OSA_Init();

    App_InitTaskHandle(&sApp);

    result = OSA_TaskCreate(App_TaskStartup,
                    (uint8_t *)"startup",
                    TASK_STARTUP_STACK_SIZE,
					task_startup_stack,
                    TASK_STARTUP_PRIO,
                    (task_param_t)&sApp,
                    false,
                    &task_startup_task_handler);
    if (result != kStatus_OSA_Success)
    {
        LREP("Failed to create startup task\r\n\r\n");
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
int App_CreateAppTask(SApp *pApp) {

	osa_status_t result;
	LREP("start create app task \r\n");

//    result = OSA_TaskCreate(App_TaskModbus,
//                    (uint8_t *)"modbus",
//                    TASK_MODBUS_STACK_SIZE,
//					task_modbus_stack,
//                    TASK_MODBUS_PRIO,
//                    (task_param_t)pApp,
//                    false,
//                    &task_modbus_task_handler);
//    if(result != kStatus_OSA_Success)
//    {
//        LREP("Failed to create slave task\r\n\r\n");
//        return -1;
//    }

    result = OSA_TaskCreate(App_TaskSerialcomm,
                    (uint8_t *)"serialcomm",
                    TASK_SERIAL_COMM_STACK_SIZE,
					task_serialcomm_stack,
                    TASK_SERIALCOMM_PRIO,
                    (task_param_t)pApp,
                    false,
                    &task_serialcomm_task_handler);
    if (result != kStatus_OSA_Success)
    {
        LREP("Failed to create serialcomm task\r\n\r\n");
        return -1;
    }

//    result = OSA_TaskCreate(App_TaskUserInterface,
//                    (uint8_t *)"ui",
//                    TASK_UI_STACK_SIZE,
//					task_ui_stack,
//                    TASK_UI_PRIO,
//                    (task_param_t)pApp,
//                    false,
//                    &task_ui_task_handler);
//    if (result != kStatus_OSA_Success)
//    {
//        LREP("Failed to create user interface task\r\n\r\n");
//        return -1;
//    }
//
//
//
//    result = OSA_TaskCreate(App_TaskAnalogIn,
//					(uint8_t *)"ai",
//					TASK_AI_STACK_SIZE,
//					task_ai_stack,
//					TASK_AI_PRIO,
//					(task_param_t)pApp,
//					false,
//					&task_ai_task_handler);
//	if(result != kStatus_OSA_Success)
//	{
//		LREP("Failed to create ai task\r\n\r\n");
//		return -1;
//	}

    // create app tasks
	result = OSA_TaskCreate(App_TaskPeriodic,
					(uint8_t *)"periodic",
					TASK_PERIODIC_STACK_SIZE,
					task_periodic_stack,
					TASK_PERIODIC_PRIO,
					(task_param_t)pApp,
					false,
					&task_periodic_task_handler);
	if (result != kStatus_OSA_Success)
	{
		LREP("Failed to create periodic task\r\n\r\n");
		return -1;
	}

    // create app tasks
    result = OSA_TaskCreate(App_TaskShell,
                    (uint8_t *)"shell",
                    TASK_SHELL_STACK_SIZE,
                    task_shell_stack,
                    TASK_SHELL_PRIO,
                    (task_param_t)pApp,
                    false,
                    &task_shell_task_handler);
    if (result != kStatus_OSA_Success)
    {
        LREP("Failed to create master task\r\n\r\n");
        return -1;
    }

	return result;

}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int App_CreateAppEvent(SApp *pApp) {
	OS_ERR error;

	OSSemCreate(&debug_sem, "debug", 0, &error);
	ASSERT(error == OS_ERR_NONE);

	OSSemCreate(&pApp->hSem, "control", 0, &error);
	ASSERT(error == OS_ERR_NONE);

	return error == OS_ERR_NONE;
}


#endif


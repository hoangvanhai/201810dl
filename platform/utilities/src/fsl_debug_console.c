/*
 * Copyright (c) 2013 - 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_uart_hal.h"
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "print_scan.h"
#include "fsl_uart_driver.h"
#include "shell.h"
#include "app.h"

extern uint32_t g_app_handle;
#if __ICCARM__
#include <yfuns.h>
#endif

static int debug_putc(int ch, void* stream);

/*******************************************************************************
 * Global variable
 ******************************************************************************/
uart_state_t debug_uart_state;
uint8_t rx_char;
/*******************************************************************************
 * Definitions
 ******************************************************************************/
void debug_rx_handle(uint32_t instance, void * uartState);

/*! @brief Operation functions definiations for debug console. */
typedef struct DebugConsoleOperationFunctions {
    union {
        void (* Send)(void *base, const uint8_t *buf, uint32_t count);
#if defined(UART_INSTANCE_COUNT)
        void (* UART_Send)(UART_Type *base, const uint8_t *buf, uint32_t count);
#endif
    } tx_union;
    union{
        void (* Receive)(void *base, uint8_t *buf, uint32_t count);
#if defined(UART_INSTANCE_COUNT)
        uart_status_t (* UART_Receive)(UART_Type *base, uint8_t *buf, uint32_t count);
#endif
    } rx_union;
} debug_console_ops_t;

/*! @brief State structure storing debug console. */
typedef struct DebugConsoleState {
    debug_console_device_type_t type;/*<! Indicator telling whether the debug console is inited. */
    uint8_t instance;               /*<! Instance number indicator. */
    void*   base;                   /*<! Base of the IP register. */
    debug_console_ops_t ops;        /*<! Operation function pointers for debug uart operations. */
} debug_console_state_t;

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief Debug UART state information.*/
static debug_console_state_t s_debugConsole;

/*******************************************************************************
 * Code
 ******************************************************************************/
/* See fsl_debug_console.h for documentation of this function.*/
debug_console_status_t DbgConsole_Init(
        uint32_t uartInstance, uint32_t baudRate, debug_console_device_type_t device)
{
    if (s_debugConsole.type != kDebugConsoleNone)
    {
        return kStatus_DEBUGCONSOLE_Failed;
    }

    /* Set debug console to initialized to avoid duplicated init operation.*/
    s_debugConsole.type = device;
    s_debugConsole.instance = uartInstance;

    /* Switch between different device. */
    switch (device)
    {
		case kDebugConsoleUART:
		{
			uart_user_config_t debug_uart_cfg = {
				.baudRate = baudRate,
				.parityMode = kUartParityDisabled,
				.stopBitCount = kUartTwoStopBit,
				.bitCountPerChar = kUart8BitsPerChar,
			};

			UART_Type * g_Base[UART_INSTANCE_COUNT] = UART_BASE_PTRS;
			UART_Type * base = g_Base[uartInstance];

			UART_DRV_Init(uartInstance, &debug_uart_state, &debug_uart_cfg);
			/* Set the funciton pointer for send and receive for this kind of device. */
			s_debugConsole.ops.tx_union.UART_Send = UART_HAL_SendDataPolling;
//                s_debugConsole.ops.rx_union.UART_Receive = UART_HAL_ReceiveDataPolling;
			UART_DRV_InstallRxCallback(uartInstance, debug_rx_handle, &rx_char, NULL, true);
			s_debugConsole.base = base;
		}
		break;

        /* If new device is requried as the low level device for debug console,
         * Add the case branch and add the preprocessor macro to judge whether
         * this kind of device exist in this SOC. */
        default:
            /* Device identified is invalid, return invalid device error code. */
            return kStatus_DEBUGCONSOLE_InvalidDevice;
    }

    /* Configure the s_debugConsole structure only when the inti operation is successful. */
    s_debugConsole.instance = uartInstance;

    return kStatus_DEBUGCONSOLE_Success;
}


void debug_rx_handle(uint32_t instance, void * uartState) {
	uart_state_t *state = (uart_state_t*)uartState;
	//debug_putchar(state->rxBuff[0]);
	if(shell_push_command(state->rxBuff[0]) == true) {
		OS_ERR err;
		OSTaskSemPost(&TCB_task_shell, OS_OPT_POST_NONE, &err);
		if(err != OS_ERR_NONE) {
			LREP("sem post failed\r\n");
		}
	}
}


/* See fsl_debug_console.h for documentation of this function.*/
debug_console_status_t DbgConsole_DeInit(void)
{
    if (s_debugConsole.type == kDebugConsoleNone)
    {
        return kStatus_DEBUGCONSOLE_Success;
    }

    switch(s_debugConsole.type)
    {

        case kDebugConsoleUART:
            CLOCK_SYS_DisableUartClock(s_debugConsole.instance);
            break;
        default:
            return kStatus_DEBUGCONSOLE_InvalidDevice;
    }

    s_debugConsole.type = kDebugConsoleNone;
    return kStatus_DEBUGCONSOLE_Success;
}


#pragma weak _write
int _write (int handle, char *buffer, int size)
{
    if (buffer == 0)
    {
        /* return -1 if error */
        return -1;
    }

    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    if ((handle != 1) && (handle != 2))
    {
        return -1;
    }

    /* Do nothing if the debug uart is not initialized.*/
    if (s_debugConsole.type == kDebugConsoleNone)
    {
        return -1;
    }

    /* Send data.*/
    s_debugConsole.ops.tx_union.Send(s_debugConsole.base, (uint8_t *)buffer, size);
    return size;
}

#pragma weak _read
int _read(int handle, char *buffer, int size)
{
    /* This function only reads from "standard in", for all other file*/
    /* handles it returns failure.*/
    if (handle != 0)
    {
        return -1;
    }

    /* Do nothing if the debug uart is not initialized.*/
    if (s_debugConsole.type == kDebugConsoleNone)
    {
        return -1;
    }

    /* Receive data.*/
    s_debugConsole.ops.rx_union.Receive(s_debugConsole.base, (uint8_t *)buffer, size);
    return size;
}


/*************Code for debug_printf/scanf/assert*******************************/
int debug_printf(const char  *fmt_s, ...)
{
   va_list  ap;
   int  result;
   /* Do nothing if the debug uart is not initialized.*/
   if (s_debugConsole.type == kDebugConsoleNone)
   {
       return -1;
   }
   va_start(ap, fmt_s);
   result = _doprint(NULL, debug_putc, -1, (char *)fmt_s, ap);
   va_end(ap);

   return result;
}

static int debug_putc(int ch, void* stream)
{
    const unsigned char c = (unsigned char) ch;
    /* Do nothing if the debug uart is not initialized.*/
    if (s_debugConsole.type == kDebugConsoleNone)
    {
        return -1;
    }
    s_debugConsole.ops.tx_union.Send(s_debugConsole.base, &c, 1);

    return 0;

}

int debug_putchar(int ch)
{
    /* Do nothing if the debug uart is not initialized.*/
    if (s_debugConsole.type == kDebugConsoleNone)
    {
        return -1;
    }
    debug_putc(ch, NULL);

    return 1;
}

int debug_scanf(const char  *fmt_ptr, ...)
{
    char    temp_buf[IO_MAXLINE];
    va_list ap;
    uint32_t i;
    char result;

    /* Do nothing if the debug uart is not initialized.*/
    if (s_debugConsole.type == kDebugConsoleNone)
    {
        return -1;
    }
    va_start(ap, fmt_ptr);
    temp_buf[0] = '\0';

    for (i = 0; i < IO_MAXLINE; i++)
    {
        temp_buf[i] = result = debug_getchar();

        if ((result == '\r') || (result == '\n'))
        {
            /* End of Line */
            if (i == 0)
            {
                i = (uint32_t)-1;
            }
            else
            {
                break;
            }
        }

        temp_buf[i + 1] = '\0';
    }

    result = scan_prv(temp_buf, (char *)fmt_ptr, ap);
    va_end(ap);

   return result;
}

int debug_getchar(void)
{
    unsigned char c;
    /* Do nothing if the debug uart is not initialized.*/
    if (s_debugConsole.type == kDebugConsoleNone)
    {
        return -1;
    }

    s_debugConsole.ops.rx_union.Receive(s_debugConsole.base, &c, 1);

    return c;
}




/*******************************************************************************
 * EOF
 ******************************************************************************/

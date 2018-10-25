/*
 * drv_object.c
 *
 *  Created on: Oct 25, 2018
 *      Author: PC
 */


#include <drv_object.h>
#include <board.h>



const uart_user_config_t debug_uart_cfg = {
  .baudRate = BOARD_DEBUG_UART_BAUD,
  .parityMode = kUartParityDisabled,
  .stopBitCount = kUartTwoStopBit,
  .bitCountPerChar = kUart8BitsPerChar,
};

uart_state_t debug_uart_state;

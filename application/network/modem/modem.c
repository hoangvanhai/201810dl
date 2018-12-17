/*
 * modem.c
 *
 *  Created on: Oct 25, 2018
 *      Author: buiti
 */

//#inlcude "platform.h"
#include "modem.h"
#include "modem_debug.h"


static ring_buff_t g_modemATRxBuffer;
static uint8_t g_pu8_modem_rx_bufer[MODEM_RX_BUFF_SIZE] = {0};

static uint8_t modem_wait_for_response(uint32_t time_out, char cmp_chr);
static uint8_t modem_wait_for_str_response(uint32_t time_out_ms, char* res_str, uint8_t len);
static void 	modem_flush_buffer();

void modem_init()
{
	modem_hw_init();
	buffer_init(&g_modemATRxBuffer, 1, MODEM_RX_BUFF_SIZE, g_pu8_modem_rx_bufer);
	buffer_flush(&g_modemATRxBuffer);
	modem_switch_to_command_mode();
	modem_send_at_command("AT&F\r\n", "OK", 1000, 1);
	modem_send_at_command("AT+QFTPCLOSE\r\n", "+QFTPCLOSE:", 5000, 1);
	modem_send_at_command("AT+QIDEACT=1\r\n", "OK", 5000, 1);
}

void modem_switch_to_command_mode(void)
{
	modem_delay_ms(1000);
	modem_tx_data("+++", 3);
	modem_delay_ms(1000);
}

uint8_t	modem_send_at_command(const char *cmd, const char* res_str, int16_t timeout_ms, uint8_t retry)
{
	uint8_t count;
	uint8_t len = strlen(cmd);
	uint8_t res_len = strlen(res_str);
	for (count=0;count<retry;count++)
	{

		modem_tx_data(cmd, len);
		modem_flush_buffer();
//		if(modem_wait_for_response(timeout_ms, 'K') == 1)
//		{
//			return TRUE;
//		}

		// new function
		if(modem_wait_for_str_response(timeout_ms, (char*)res_str, res_len) == TRUE)
		{
			return TRUE;
		}
	}
	MODEM_DEBUG_WARNING("Send command '%s' Timed out!", cmd);
	return FALSE;
}

/**
 * Get module info, including module model, fw version, ...
 * @return
 */
uint8_t modem_get_version(void)
{
	return 0;
}
uint8_t modem_get_simcard_status(void)
{
	return 0;
}
uint8_t modem_get_csq(void)
{
	return 0;
}
uint8_t	modem_get_opn_id(void)
{
	return 0;
}
uint8_t	modem_pdp_connect(uint8_t opn_id)
{
	return 0;
}

void modem_rx_cmplt_callback(void* data, uint16_t len)
{
	buffer_push(&g_modemATRxBuffer, data);
}

uint8_t modem_get_rx_buffer(void *data, uint16_t *len)
{
	return buffer_get_data(&g_modemATRxBuffer, data, len);
}

void 	modem_flush_buffer()
{
	buffer_flush(&g_modemATRxBuffer);
}


uint8_t modem_wait_for_response(uint32_t time_out_ms, char cmp_chr)
{
	uint16_t time;
	uint8_t u8_tmp = 0;

	time=time_out_ms/10;

	while (time--)
	{
		while(buffer_get_count(&g_modemATRxBuffer) > 0)
		{
			if(buffer_pop(&g_modemATRxBuffer, &u8_tmp) != TRUE)
				return FALSE;

			PRINTF(&u8_tmp);

			if(u8_tmp == cmp_chr)
				return TRUE;
		}
		modem_delay_ms(10);		// TODO: manhbt - change to platform_delay_ms(ms) macros
	}
	return FALSE;
}

uint8_t modem_wait_for_str_response(uint32_t time_out_ms, char* res_str, uint8_t len)
{
	uint16_t time;
	uint8_t u8_tmp = 0;

	time=time_out_ms/50;
	uint8_t compare_index = 0;

	while (time--)
	{
		while(buffer_get_count(&g_modemATRxBuffer) > 0)
		{
			if(buffer_pop(&g_modemATRxBuffer, &u8_tmp) != TRUE)
				break;
			PRINTF("%c", u8_tmp);
			compare_index = 0;
			if(u8_tmp == res_str[compare_index])
			{
				compare_index = 1;
				while ((compare_index < len) && (u8_tmp == res_str[compare_index - 1])){
					if(buffer_pop(&g_modemATRxBuffer, &u8_tmp) != TRUE)
						break;
					PRINTF("%c", u8_tmp);

					if(u8_tmp == res_str[compare_index])
						compare_index++;
				};
			}
			if(compare_index == len)
				return TRUE;

		}
		modem_delay_ms(50); 	// TODO: manhbt - change to platform_delay_ms(ms) macros
	}
	return FALSE;
}


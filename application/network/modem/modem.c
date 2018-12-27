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


static mutex_t	g_modem_tx_mtx;
modem_status_t g_modem_status;

static ring_buff_t g_modemStatRxBuffer;
static uint8_t g_pu8_modem_stat_rx_bufer[MODEM_RX_BUFF_SIZE] = {0};

static uint8_t 	modem_wait_for_response(uint32_t time_out, char cmp_chr);
static uint8_t 	modem_wait_for_str_response(uint32_t time_out_ms, char* res_str, uint8_t len);


static uint8_t 	modem_get_response(uint32_t time_out_ms, char* res_str, uint8_t* len);
static void 	modem_flush_buffer();
static void 	modem_flush_stat_buffer();
/**
 * uart rx interrupt handler for modem AT console port
 * please put this function to UART Rx ISR
 * @param data pointer to data buffer
 * @param len data length
 */
void modem_rx_cmplt_callback(void* data, uint16_t len);



void modem_init()
{
	buffer_init(&g_modemATRxBuffer, 1, MODEM_RX_BUFF_SIZE, g_pu8_modem_rx_bufer);
	buffer_flush(&g_modemATRxBuffer);

	buffer_init(&g_modemStatRxBuffer, 1, MODEM_RX_BUFF_SIZE, g_pu8_modem_stat_rx_bufer);
	buffer_flush(&g_modemStatRxBuffer);

	ASSERT_VOID(OSA_MutexCreate(&g_modem_tx_mtx) == kStatus_OSA_Success);

	modem_hw_init();
	modem_hw_add_rx_callback(modem_rx_cmplt_callback);

	modem_switch_to_command_mode();
	modem_send_at_command("AT\r\n", "OK", 200, 1);
	modem_send_at_command("ATE0\r\n", "OK", 200, 1);
	modem_send_at_command("AT&F\r\n", "OK", 200, 1);
	modem_send_at_command("AT+QFTPCLOSE\r\n", "+QFTPCLOSE:", 5000, 1);
	modem_send_at_command("AT+QIDEACT=1\r\n", "OK", 5000, 1);

	modem_get_status(&g_modem_status);

	MODEM_DEBUG("modem status:\r\n\ticcid: %s\r\n\tcsq: %d\r\n\topn: %s",
			g_modem_status.iccid,
			g_modem_status.csq,
			g_modem_status.opn);

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

uint8_t modem_get_iccid(char* iccid)
{
	uint8_t ret = modem_tx_data("AT+QCCID\r\n", 10);
	if(ret) return ret;

	modem_flush_stat_buffer();
	char res_str[MODEM_RX_BUFF_SIZE] = {0};
	uint8_t len;
	ret = modem_get_response(1000, res_str, &len);
	if(ret) return ret;

	char* p = Str_Str(res_str, "+QCCID: ");
	if (p == NULL)
		return 1;

	p+= Str_Len("+QCCID: ");
	uint8_t  index = 0;
	while (*p != 0 && *p != 0x0D  && (index < sizeof(g_modem_status.iccid))) {
		iccid[index++] = *(p++);
	}

	return 0;


}

int8_t 	modem_get_csq(uint8_t *csq)
{
	uint8_t ret = modem_tx_data("AT+CSQ\r\n", 8);
	if(ret) return ret;

	modem_flush_stat_buffer();
	char res_str[MODEM_RX_BUFF_SIZE] = {0};
	uint8_t len;
	ret = modem_get_response(1000, res_str, &len);
	if(ret) return ret;

//	MODEM_DEBUG("res str: %s", res_str);
	char* p = Str_Str(res_str, "+CSQ: ");
	if (p == NULL)
		return 1;

	p+= Str_Len("+CSQ: ");

//	MODEM_DEBUG("res str: %s", p);
	uint8_t  index = 0;
	char tmp[5] =  {0};
	while (*p != 0 && *p != ',' && (index < 5)) {
		tmp[index++] = *(p++);
	}

//	MODEM_DEBUG("CSQ: %s", tmp);
	*csq = atoi(tmp);
	return 0;

}
uint8_t	modem_get_opn_id(char* opn)
{
	//AT+COPS?
	uint8_t ret = modem_tx_data("AT+COPS?\r\n", 10);
	if(ret) return ret;

	modem_flush_stat_buffer();
	char res_str[MODEM_RX_BUFF_SIZE] = {0};
	uint8_t len;
	ret = modem_get_response(1000, res_str, &len);
	if(ret) return ret;

//	MODEM_DEBUG("res str: %s", res_str);
	//+COPS: 0,0,"Vietnamobile",4
	char* p = Str_Str(res_str, "+COPS: 0,0,\"");
	if (p == NULL)
		return 1;

	p+= Str_Len("+COPS: 0,0,\"");
	uint8_t  index = 0;
	while (*p != 0 && *p != '"' && (index < sizeof(g_modem_status.opn))) {
		opn[index++] = *(p++);
	}

	return 0;
}
uint8_t	modem_pdp_connect(uint8_t opn_id)
{
	return 0;
}

void modem_rx_cmplt_callback(void* data, uint16_t len)
{
	buffer_push(&g_modemATRxBuffer, data);

	/**
	 * secondary buffer for check URC
	 */
	buffer_push(&g_modemStatRxBuffer, data);

}

uint8_t modem_get_rx_buffer(void *data, uint16_t *len)
{
	return buffer_get_data(&g_modemATRxBuffer, data, len);
}

void 	modem_flush_buffer()
{
	buffer_flush(&g_modemATRxBuffer);
}

void 	modem_flush_stat_buffer()
{
	buffer_flush(&g_modemStatRxBuffer);
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

uint8_t modem_get_response(uint32_t time_out_ms, char* res_str, uint8_t* pLen)
{
	uint16_t time;
	uint8_t u8_tmp = 0;
	const uint8_t iter = 10;

	time=time_out_ms/iter;

	ASSERT_NONVOID(res_str != NULL && pLen != NULL, 2);
	*pLen = 0;

	while (time--)
	{
		while(buffer_get_count(&g_modemStatRxBuffer) > 0)
		{
			if(buffer_pop(&g_modemStatRxBuffer, &u8_tmp) != TRUE)
				break;
//			PRINTF("%.2x  ", u8_tmp);
			res_str[*pLen]= u8_tmp;
			*pLen += 1;

//			if(u8_tmp == 0x0D)
//				return 0;
//
//			res_str[*pLen++]= u8_tmp;

		}
		modem_delay_ms(iter);
	}
	if (*pLen > 0) return 0;
	return 1;
}


uint8_t modem_get_status(modem_status_t *pStat)
{
	uint8_t result = 0;

	ASSERT_NONVOID(pStat, 2);

	uint8_t ret = modem_get_iccid(pStat->iccid);
	if (ret)
		result |= 1;
	if (modem_get_csq(&pStat->csq))
		result |= 2;
	if (modem_get_opn_id(pStat->opn))
		result |= 4;

	return result;
}

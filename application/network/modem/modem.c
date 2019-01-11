/*
 * modem.c
 *
 *  Created on: Oct 25, 2018
 *      Author: buiti
 */

//#inlcude "platform.h"
#include "modem.h"
#include "modem_debug.h"



static modem_handle_t g_modem_handle;

static const response_table_entry_t ftp_cli_reponse_lookup_table[] =
{
	{"+QFTPOPEN: ",		2U, ','},
	{"+QFTPCWD: ",		2U, ','},
	{"+QFTPMKDIR: ",	2U, ','},
	{"+CONNECT: ",		0U, '\0'},
	{"+QFTPCLOSE: ",	2U, ','},
	{"+CME ERROR: ",	1U, '\0'},
	{"+QICSGP:",		5U, '\0'},
	{"+QFTPCFG:",		3U, '\0'},	//
	{0, 0U, 0}
};

/** TCP Client related  response */

//+QIOPEN: 0,0		--> connection established
//+QISTATE: 0,"TCP","123.31.43.184",2011,43456,2,1,0,1,"uart"
//SEND OK		--> send ok
//+QIURC: "recv",0,33	--> receive data
//<data>
//
///////////////////////////////// close event ///////////////////////////////
//+QIURC: "closed",0
//
///////////////////////////////// connect event ///////////////////////////////
//AT+QIOPEN=1,0,"TCP","123.31.43.184",2011,0,1
//OK
//// connect ok
//+QIOPEN: 0,0
//
//// connect fail (server not open)
//+QIOPEN: 0,552

static const response_table_entry_t tcp_cli_reponse_lookup_table[] =
{
	{"+QIOPEN: ",		2U, ','},
	{"+QISTATE: ",		10U, ','},
	{"SEND OK",			0U, ','},
	{"+QIURC: ",		3U, ','},
	{0, 0U, 0}
};


//static ring_buff_t g_modemATRxBuffer;
//static uint8_t g_pu8_modem_rx_bufer[MODEM_RX_BUFF_SIZE] = {0};

static uint8_t modem_wait_for_response(uint32_t time_out, char cmp_chr);
static uint8_t modem_wait_for_str_response(uint32_t time_out_ms, char* res_str, uint8_t len);
static uint8_t modem_wait_for_str_response2(uint32_t time_out_ms, char* res_str, uint8_t len, bool break_cond);

// Wait for event
static uint8_t modem_wait_for_event(modem_handle_t* handle, bool evt, uint32_t time_out);

static void 	modem_flush_buffer(ring_buff_t* buff);

void modem_rx_dispatcher(void *arg);
OSA_TASK_DEFINE(modem_rx_dispatcher, TASK_MODEM_RX_DISPATCHER_SIZE);

void modem_check_common_status_response(const char* res_str);

/**
 * uart rx interrupt handler for modem AT console port
 * please put this function to UART Rx ISR
 * @param data pointer to data buffer
 * @param len data length
 */
void modem_rx_cmplt_callback(void* data, uint16_t len);

modem_handle_t *modem_get_instance(void) {
	return &g_modem_handle;
}

void modem_init(modem_handle_t* handle)
{
	ASSERT_VOID(handle);



	/* Initialize ring buffer for Rx data - status & tcp client buffer */
	handle->modem_at_dispatch_rx_buf_size = MODEM_RX_BUFF_SIZE;
	handle->modem_at_dispatch_rx_buff = OSA_MemAlloc(handle->modem_at_dispatch_rx_buf_size);
	ASSERT_VOID(handle->modem_at_dispatch_rx_buff);
	buffer_init(&handle->modem_at_dispatch_rx_rb_handle, 1, handle->modem_at_dispatch_rx_buf_size, handle->modem_at_dispatch_rx_buff);
	buffer_flush(&handle->modem_at_dispatch_rx_rb_handle);
	/* Initialize rx mutex */
//	ASSERT_VOID(OSA_MutexCreate(&handle->modem_at_dispatch_rx_mutex) == kStatus_OSA_Success);


	handle->modem_at_rx_buf_size = MODEM_RX_BUFF_SIZE;
	handle->modem_at_rx_buff = OSA_MemAlloc(handle->modem_at_rx_buf_size);
	ASSERT_VOID(handle->modem_at_rx_buff);
	buffer_init(&handle->modem_at_rx_rb_handle, 1, handle->modem_at_rx_buf_size, handle->modem_at_rx_buff);
	buffer_flush(&handle->modem_at_rx_rb_handle);
	/* Initialize rx mutex */
//	ASSERT_VOID(OSA_MutexCreate(&handle->modem_at_rx_mutex) == kStatus_OSA_Success);

//	handle->busy = false;

	modem_hw_init();
	modem_hw_add_rx_callback(modem_rx_cmplt_callback);
	modem_switch_to_command_mode(&g_modem_handle);

	ASSERT(OSA_MutexCreate(&handle->modem_tx_mutex) == kStatus_OSA_Success);

	/* Create Rx Dispatcher task  */
	osa_status_t result = OSA_TaskCreate(modem_rx_dispatcher,
			(uint8_t *) "modem_rx_dispatcher",
			TASK_MODEM_RX_DISPATCHER_SIZE, modem_rx_dispatcher_stack,
			TASK_MODEM_RX_DISPATCHER_PRIO, (task_param_t) 0,
			false, &modem_rx_dispatcher_task_handler);
	if (result != kStatus_OSA_Success) {
		PRINTF("Failed to create ftp client tx task\r\n\r\n");
		return;
	}

	MODEM_DEBUG("Initializing Modem ...");
	modem_send_at_command(&g_modem_handle, "AT\r\n", "OK", 1000, 1);

	modem_switch_to_command_mode(&g_modem_handle);
	modem_send_at_command(&g_modem_handle, "AT&F\r\n", "OK", 1000, 1);
	modem_send_at_command(&g_modem_handle, "AT+QFTPCLOSE\r\n", "+QFTPCLOSE:", 5000, 1);
	modem_send_at_command(&g_modem_handle, "AT+QIDEACT=1\r\n", "OK", 5000, 1);
	MODEM_DEBUG("Modem initialized successfully!!!");
}


void modem_switch_to_command_mode(modem_handle_t* handle)
{
	modem_delay_ms(1000);
	modem_tx_data("+++", 3);
	modem_delay_ms(1000);
}

uint8_t	modem_send_at_command(modem_handle_t* handle, const char *cmd, const char* res_str, int16_t timeout_ms, uint8_t retry)
{
	uint8_t count;
	uint8_t len = strlen(cmd);
	uint8_t res_len = strlen(res_str);
	ASSERT_NONVOID(handle, FALSE);

//	MODEM_ENTER_CRITIAL(handle, 1000);
	for (count=0;count<retry;count++)
	{
		modem_flush_buffer(&handle->modem_at_rx_rb_handle);
		modem_tx_data(cmd, len);

		if(modem_wait_for_str_response(timeout_ms, (char*)res_str, res_len) == TRUE)
		{
//			MODEM_EXIT_CRITIAL(handle);
			return TRUE;
		}
	}
	MODEM_DEBUG_WARNING("Send command '%s' Timed out!", cmd);
//	MODEM_EXIT_CRITIAL(handle);
	return FALSE;
}

void modem_rx_cmplt_callback(void* data, uint16_t len)
{
//	buffer_push(&g_modem_handle.modem_at_dispatch_rx_rb_handle, data);
	// TODO: push data to buffer for ftp client process

//	PRINTF("%x  ", *(uint8_t*)data);
	buffer_push(&g_modem_handle.modem_at_dispatch_rx_rb_handle, data);

	/**
	 * secondary buffer for check URC
	 */
	buffer_push(&g_modem_handle.modem_at_rx_rb_handle, data);
}


void 	modem_flush_buffer(ring_buff_t* buff)
{
	buffer_flush(buff);
}

#if (0)
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

#endif
uint8_t modem_wait_for_str_response(uint32_t time_out_ms, char* res_str, uint8_t len)
{
	uint32_t time = OSA_TimeGetMsec();

	while ((OSA_TimeGetMsec() - time) < time_out_ms) {
		if (Str_Str(g_modem_handle.modem_at_rx_buff, (const char*)res_str) != NULL) {
			return TRUE;
		}
		OSA_SleepMs(10);
	}
	return FALSE;
}

#if (0)
uint8_t modem_wait_for_str_response2(uint32_t time_out_ms, char* res_str, uint8_t len, bool break_cond)
{
	uint32_t time = OSA_TimeGetMsec();

	while (((OSA_TimeGetMsec() - time) < time_out_ms) && (break_cond))
	{
		if (Str_Str(g_pu8_modem_rx_bufer, (const char*)res_str) != NULL)
			return TRUE;
		OSA_SleepMs(10);
	}
	return FALSE;
}
#endif

void modem_rx_dispatcher(void *arg) {

//	FtpClient *pFC = (FtpClient*)arg;
//	OS_MSG_SIZE msg_size;
//	CPU_TS ts;
//	OS_ERR err;

	uint8_t *res_str = OSA_MemAlloc(1024);
	ASSERT_VOID(res_str);
	uint8_t u8_tmp;
	uint16_t len = 0;
	MODEM_DEBUG_WARNING("modem_rx_dispatcher started!!!!");

	while(1) {

//		PRINTF(" ");
		memset(res_str, 0, 1024); len = 0;

		while(buffer_get_count(&g_modem_handle.modem_at_dispatch_rx_rb_handle) > 0)
		{
			if(buffer_pop(&g_modem_handle.modem_at_dispatch_rx_rb_handle, &u8_tmp) != TRUE)
				break;
//			if (u8_tmp <  32) {
//				MODEM_DEBUG_RAW(" <%.2x> ", u8_tmp);
//			} else {
//				MODEM_DEBUG_RAW("%c", u8_tmp);
//			}

			if((u8_tmp == 0x0A) && (len > 0) && (res_str[len-1] == 0x0D)) {
//				MODEM_DEBUG("Got line: %s", res_str);
				/* Manhbt pre-process received data from module */
				//TODO: process FTP Client related code
//				modem_check_ftp_related_response((const char*)res_str);
				//TODO: process TCP client related code
//				modem_check_tcp_related_response((const char*)res_str);
				//TODO: process Common module status response code
				modem_check_common_status_response((const char*)res_str);
				//TODO: process URC (for example: +QIURC: "ccc",0)
//				modem_check_urc((const char*)res_str);
				break;
			}
			res_str[len++]= u8_tmp;
		}
		OSA_SleepMs(10);
	}
}



/**
 * Check for common response from modem
 * @param response: received line from modem
 */
void modem_check_common_status_response(const char* response){
	/**
	 * Check response from modem status:
	 * + SimID: prefix "+QCCID: "
	 * + Signal Quality: prefix "+CSQ: "
	 * + Current connected Operator: prefix "+COPS: "
	 * Other response: Ignored
	 */
	CPU_CHAR  *pTmp = NULL;

	// Process CCID response
	pTmp = Str_Str(response, "+QCCID: ");
	if(pTmp  != NULL) {
//		MODEM_DEBUG("got QCCID message: %s", pTmp);
		// TODO: get CCID and stored
		// +QCCID: 89840200010721069441
		pTmp+= strlen("+QCCID: ");
		Str_Copy_N(g_modem_handle.modem_common_status.iccid, pTmp, sizeof(g_modem_handle.modem_common_status.iccid) - 1);
		MODEM_DEBUG("CCID: %s", g_modem_handle.modem_common_status.iccid);
		return;
	}

	// Process CSQ response
	pTmp = Str_Str(response, "+CSQ: ");
	if(pTmp  != NULL) {
//		MODEM_DEBUG("got CSQ message: %s", pTmp);
		// TODO: get CSQ and stored
		// +CSQ: 17,99
		pTmp+= strlen("+CSQ: ");
		uint8_t csq[3] = {0}, idx = 0;
		while((*pTmp != ',') && (idx < 3)){
			csq[idx++] = *pTmp;
			pTmp++;
		}

		g_modem_handle.modem_common_status.csq = (int8_t)strtol(csq, NULL, 10);
		MODEM_DEBUG("CSQ: %d", g_modem_handle.modem_common_status.csq);

//		Str_Copy_N(g_modem_handle.modem_common_status., pTmp, sizeof(g_modem_handle.modem_common_status.iccid) - 1);
		return;
	}
	// Process COPS response
	pTmp = Str_Str(response, "+COPS: ");
	if(pTmp  != NULL) {
		// TODO: get CCID and stored
		// +COPS: 0,0,"VN VINAPHONE",4
		pTmp = Str_Str(pTmp, "\"");
		ASSERT_VOID(pTmp);
		pTmp++;
		uint8_t idx = 0;
		while((*pTmp != '"') && (idx <  sizeof(g_modem_handle.modem_common_status.opn) - 1)){
			g_modem_handle.modem_common_status.opn[idx++] = *pTmp;
			pTmp++;
		}

		MODEM_DEBUG("OPN: %s", g_modem_handle.modem_common_status.opn);

		return;
	}
}



uint8_t modem_get_status(modem_handle_t* handle, modem_status_t* stat)
{
	uint8_t ret = 0;

	ASSERT_NONVOID(stat, -2);
	ASSERT_NONVOID(handle, -3);

	if (MODEM_ENTER_CRITIAL(handle, 100) != kStatus_OSA_Success) {
//	if (handle->busy){
		ret = 1;
		MODEM_DEBUG_WARNING("Modem is busy!, ret = %d", ret);
		return ret;
	}


	char cmd[128]={0};
	memset(cmd, 0, 128);
	sprintf(cmd, "%s", "AT+QCCID\r\n");
	modem_tx_data(cmd, strlen(cmd));
	OSA_TimeDelay(10);

	memset(cmd, 0, 128);
	sprintf(cmd, "%s", "AT+COPS?\r\n");
	modem_tx_data(cmd, strlen(cmd));
	OSA_TimeDelay(10);

	memset(cmd, 0, 128);
	sprintf(cmd, "%s", "AT+CSQ\r\n");
	modem_tx_data(cmd, strlen(cmd));

	MODEM_EXIT_CRITIAL(handle);
	memcpy(stat, &g_modem_handle.modem_common_status, sizeof(modem_status_t));

	MODEM_DEBUG("Get Modem status OK!, ret = %d", ret);
	return ret;
}

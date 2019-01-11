/*
 * modem.h
 *
 *  Created on: Oct 25, 2018
 *      Author: buiti
 */

#ifndef MODEM_H_
#define MODEM_H_

#include "modem_wrapper.h"
#include "ring_buffer.h"


#define MODEM_RX_BUFF_SIZE 	128

#define MODEM_AT_DEFAULT_TIMEOUT_SEC	20
#define MODEM_AT_DEFAULT_TIMEOUT_MSEC	25000

#define MODEM_AT_FTP_CONNECT_TIMEOUT_MSEC	MODEM_AT_DEFAULT_TIMEOUT_MSEC
#define MODEM_AT_FTP_DISCONNECT_TIMEOUT_MSEC	5000
#define MODEM_AT_FTP_CWDIR_TIMEOUT_MSEC	5000
#define MODEM_AT_FTP_MKDIR_TIMEOUT_MSEC	5000
#define MODEM_AT_FTP_TERM_TIMEOUT_MSEC	5000


/* Modem Rx dispatcher task prio */
#define 	TASK_MODEM_RX_DISPATCHER_PRIO      	11U

/* Modem Rx dispatcher task stack size */
#define 	TASK_MODEM_RX_DISPATCHER_SIZE   	2048U

typedef struct {
	int8_t	csq;
	char	iccid[32];
	char	opn[32];
	char 	version[32];
}modem_status_t;


typedef struct {
	/* Common Module status: CSQ, Model, ICCID, OPN,... */
	int 				modem_at_dispatch_rx_buf_size;
	uint8_t				*modem_at_dispatch_rx_buff;
	ring_buff_t 		modem_at_dispatch_rx_rb_handle;
//	mutex_t				modem_at_dispatch_rx_mutex;
	/* for AT send & check */
	int 				modem_at_rx_buf_size;
	uint8_t				*modem_at_rx_buff;
	ring_buff_t 		modem_at_rx_rb_handle;
//	mutex_t				modem_at_rx_mutex;

	modem_status_t 		modem_common_status;
//	sys_thread_t		modem_at_common_rx_process_thread;
	mutex_t 			modem_tx_mutex;
//	bool				busy;

}modem_handle_t;

typedef struct {
	char 	*prefix;
	uint8_t max_param;
	char	delim;		/** delimiter*/
} response_table_entry_t;


#define MODEM_ENTER_CRITIAL(handle, timeout)		OSA_MutexLock(&handle->modem_tx_mutex, timeout)
#define MODEM_EXIT_CRITIAL(handle)					OSA_MutexUnlock(&handle->modem_tx_mutex)

//void modem_init();
//void modem_switch_to_command_mode(void);
//uint8_t	modem_send_at_command(const char *cmd, const char* res_str, int16_t timeout_ms, uint8_t retry);



/**
 * Get the modem handler
 * @return modem handle pointer
 */
modem_handle_t *modem_get_instance(void);

/**
 * @brief Initialize modem
 * @param handle modem handle
 */
void modem_init(modem_handle_t* handle);

/**
 * @brief Switch modem to 'command mode'
 * @param handle: modem handle
 */
void modem_switch_to_command_mode(modem_handle_t* handle);


/**
 * @brief Send AT command to modem and wait for expected response string
 * @param handle: modem handle
 * @param cmd: command to send
 * @param res_str: expected response string
 * @param timeout_ms: maximum time out
 * @param retry: maximum retry
 * @return @TRUE if success, @FALSE on failed or timed out
 */
uint8_t	modem_send_at_command(modem_handle_t* handle, const char *cmd, const char* res_str, int16_t timeout_ms, uint8_t retry);


///**
// * Get module info, including module model, fw version, ...
// * @return
// */
//uint8_t modem_get_version(void);
//uint8_t modem_get_simcard_status(void);
//uint8_t modem_get_csq(void);
//uint8_t	modem_get_opn_id(void);
//uint8_t	modem_pdp_connect(uint8_t opn_id);
//uint8_t modem_get_rx_buffer(void *data, uint16_t *len);


uint8_t modem_get_status(modem_handle_t* handle, modem_status_t* stat);
//
//void modem_uart_rx_callback(uint32_t instance, void * uartState);

#endif /* MODEM_H_ */

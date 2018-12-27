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


typedef struct {
	int8_t	csq;
	char	iccid[32];
	char	opn[32];
}modem_status_t;

uint8_t	modem_send_at_command(const char *cmd, const char* res_str, int16_t timeout_ms, uint8_t retry);

void modem_init();
void modem_switch_to_command_mode(void);

/**
 * Get module info, including module model, fw version, ...
 * @return
 */
uint8_t modem_get_version(void);
uint8_t modem_get_iccid(char* iccid);
int8_t 	modem_get_csq(uint8_t *csq);
uint8_t	modem_get_opn_id(char* opn);
uint8_t	modem_pdp_connect(uint8_t opn_id);
uint8_t modem_get_rx_buffer(void *data, uint16_t *len);

uint8_t modem_get_status(modem_status_t *pStat);

extern modem_status_t g_modem_status;

//
//void modem_uart_rx_callback(uint32_t instance, void * uartState);

#endif /* MODEM_H_ */

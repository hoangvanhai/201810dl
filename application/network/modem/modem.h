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


#define MODEM_RX_BUFF_SIZE 	64

uint8_t	modem_send_at_command(const char *cmd, const char* res_str, int16_t timeout_ms, uint8_t retry);

void modem_init();
void modem_switch_to_command_mode(void);

/**
 * Get module info, including module model, fw version, ...
 * @return
 */
uint8_t modem_get_version(void);
uint8_t modem_get_simcard_status(void);
uint8_t modem_get_csq(void);
uint8_t	modem_get_opn_id(void);
uint8_t	modem_pdp_connect(uint8_t opn_id);
uint8_t modem_get_rx_buffer(void *data, uint16_t *len);


/**
 * uart rx interrupt handler for modem AT console port
 * please put this function to UART Rx ISR
 * @param data pointer to data buffer
 * @param len data length
 */
void modem_rx_cmplt_callback(void* data, uint16_t len);


#endif /* MODEM_H_ */

/*
 * tcp_listener.h
 *
 *  Created on: Dec 18, 2018
 *      Author: MSI
 */

#ifndef APPLICATION_TCP_SERVER_H_
#define APPLICATION_TCP_SERVER_H_

#include <socket_common.h>


typedef struct TcpServer_ {
	int 				listen_fd;
	int 				tx_buf_size;
	int 				rx_buf_size;
	int 				listen_port;
	ip_addr_t			peer_addr;
	int					peer_port;
	int					peer_fd;
	uint8_t				*rx_buffer;

	NetworkConnNotify	notify;
	NetworkDataEvent	recv_handler;
	NetworkDataEvent	send_handler;
	NetworkDataEvent	error_handler;

	Network_Status		status;
	sys_thread_t		send_thread;
	sys_thread_t		recv_thread;
}TcpServer;

int tcp_server_config(TcpServer *pHandle, int port,
		int rx_size, int tx_size, uint8_t *rx_buff);
void tcp_server_listener(void *arg);
int tcp_server_stop(TcpServer *pHandle);
int tcp_server_get_status(TcpServer *pHandle);
int tcp_server_send_blocking(TcpServer *pEP,
		const uint8_t *data, int len);
int tcp_server_send_nonblocking(TcpServer *pEP,
		const uint8_t *data, int len);
void tcp_server_register_notify(TcpServer *pEP, NetworkConnNotify func);
void tcp_server_register_data_event(TcpServer *pEP,
		Network_DataEvent evt, NetworkDataEvent func);

void tcp_server_sender(void *arg);

#endif /* APPLICATION_TCP_SERVER_H_ */

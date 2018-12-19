/*
 * tcp_socket.h
 *
 *  Created on: Dec 18, 2018
 *      Author: PC
 */

#ifndef APPLICATION_TCP_CLIENT_
#define APPLICATION_TCP_CLIENT_

#include <lwip/api.h>
#include <lwip/ip_addr.h>
#include <lwip/sockets.h>

#include <socket_common.h>



typedef struct TcpClient_ {
	int 				fd;
	int 				tx_buf_size;
	int 				rx_buf_size;
	ip_addr_t			ip;
	int 				port;
	uint8_t				*rx_buff;

	NetworkConnNotify	notify;
	NetworkDataEvent	recv_handler;
	NetworkDataEvent	send_handler;
	NetworkDataEvent	error_handler;

	Network_Status		status;
	sys_thread_t		send_thread;
	sys_thread_t		recv_thread;

}TcpClient;

int 	tcp_client_configuration(TcpClient *pEP, ip_addr_t ip, int port,
							int tx_size, int rx_size, uint8_t *rx_buff);

int 	tcp_client_open_connection(TcpClient *pEP, int timeout);

int 	tcp_client_close_connection(TcpClient *pEP);

int 	tcp_client_send_blocking(TcpClient *pEP, const uint8_t *data, int len);

int 	tcp_client_send_nonblocking(TcpClient *pEP, const uint8_t *data, int len);

void 	tcp_client_register_notify(TcpClient *pEP, NetworkConnNotify func);

void 	tcp_client_register_data_event(TcpClient *pEP,
		Network_DataEvent evt, NetworkDataEvent func);

int		tcp_client_get_status(TcpClient *pEP);

void 	tcp_client_listener(void *arg);

void 	tcp_client_sender(void *arg);

#endif /* APPLICATION_TCP_CLIENT_ */

/*
 * tcp_listener.c
 *
 *  Created on: Dec 18, 2018
 *      Author: MSI
 */

#include <tcp_server.h>
#include <string.h>



int tcp_server_config(TcpServer *pHandle, int port,
					int rx_size, int tx_size,
					uint8_t *rx_buff) {
	if(rx_buff == NULL)
		return -1;
	memset(pHandle, 0, sizeof(TcpServer));
	pHandle->listen_port = port;
	pHandle->rx_buf_size = rx_size;
	pHandle->tx_buf_size = tx_size;
	pHandle->rx_buffer = rx_buff;
	pHandle->status = Status_Disconnected;
	return 0;
}

void tcp_server_listener(void *arg) {
	TcpServer *pHandle = (TcpServer*)arg;
	uint32_t optval;
	int rlen;
	struct sockaddr_in serveraddr; /* server's addr */
	struct sockaddr_in clientaddr; /* client addr */
	socklen_t clientlen;

	while(1) {
		pHandle->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (pHandle->listen_fd < 0) {
			LREP("error create socket\r\n");
			OSA_TimeDelay(1000);
			continue;
		}

		optval = 1;
		setsockopt(pHandle->listen_fd, SOL_SOCKET, SO_REUSEADDR,
			 (const void *)&optval , sizeof(uint32_t));

		bzero((char *) &serveraddr, sizeof(serveraddr));

		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons((unsigned short)pHandle->listen_port);

		if (bind(pHandle->listen_fd, (struct sockaddr *) &serveraddr,
			   sizeof(serveraddr)) < 0) {
			LREP("error bind socket\r\n");
			OSA_TimeDelay(1000);
			continue;
		}

		if (listen(pHandle->listen_fd, 1) < 0) {
			LREP("error listen\r\n");
			OSA_TimeDelay(1000);
			continue;
		}

		clientlen = sizeof(clientaddr);

		while(1) {
			pHandle->peer_fd = accept(pHandle->listen_fd,
					(struct sockaddr *) &clientaddr, &clientlen);
			if(pHandle->peer_fd < 0) {
				LREP("error on accept\r\n");
				OSA_TimeDelay(1000);
				continue;
			}

			//set_blocking(pHandle->peer_fd);
			LREP("peer fd = %d ", pHandle->peer_fd);
			pHandle->status = Status_Connected;
			if(pHandle->notify)
				pHandle->notify(pHandle->status, Interface_Ethernet);

			while(1) {
				int event = wait_event(pHandle->peer_fd, 100, true, false);
				if(event & Event_Readable) {
					rlen = recv(pHandle->peer_fd, pHandle->rx_buffer, 512, 0);
					if(rlen > 0) {
						if(pHandle->recv_handler) {
							pHandle->recv_handler(pHandle->rx_buffer, rlen);
						}
					} else {
						PRINTF("rlen = %d\r\n", rlen);
						break;
					}
				}

				if(event & Event_Error) {
					LREP("event error \r\n");
					break;
				}

			}
			close(pHandle->peer_fd);
			pHandle->status = Status_Disconnected;
			if(pHandle->notify)
				pHandle->notify(pHandle->status, Interface_Ethernet);

		}
	}
}

int tcp_server_stop(TcpServer *pHandle) {
	close(pHandle->listen_fd);
	close(pHandle->peer_fd);
	pHandle->listen_fd = INVALIDSOCK;
	pHandle->peer_fd = INVALIDSOCK;
	return 0;
}

int tcp_server_get_status(TcpServer *pHandle) {
	return pHandle->status;
}

int tcp_server_send_blocking(TcpServer *pHandle, const uint8_t *data, int len) {
	int err = -3;
	if(pHandle->status != Status_Connected)
		return -1;

	if(data == NULL || len <= 0)
		return -2;

	int event = wait_event(pHandle->peer_fd, 100, false, true);
	if(event & Event_Writeable) {
		err = send(pHandle->peer_fd, data, len, 0);
	}

	return err;
}

int tcp_server_send_nonblocking(TcpServer *pHandle, const uint8_t *data, int len) {
	int retVal = 0;
	if(pHandle->status == Status_Connected) {
		SMsg *pMsg = (SMsg*)OSA_FixedMemMalloc(sizeof(SMsg) + len);
		if(pMsg) {
			OS_ERR err;
			pMsg->length = len;
			pMsg->buf = (uint8_t*)pMsg + sizeof(SMsg);
			memcpy(pMsg->buf, data, len);
			OSTaskQPost(pHandle->send_thread, (void*)pMsg,
					sizeof(SMsg) + len, OS_OPT_POST_FIFO, &err);
			if(err != OS_ERR_NONE) {
				retVal = -2;
			}
		} else {
			retVal = -1;
		}
	} else {
		retVal = -3;
	}

	return retVal;
}

void tcp_server_register_notify(TcpServer *pHandle, NetworkConnNotify func) {
	if(func)
		pHandle->notify = func;
}

void tcp_server_register_data_event(TcpServer *pHandle,
		Network_DataEvent evt, NetworkDataEvent func) {
	switch(evt) {
	case Event_DataReceived:
		pHandle->recv_handler = func;
		break;
	case Event_DataSendDone:
		pHandle->send_handler = func;
		break;
	case Event_DataError:
		pHandle->error_handler = func;
		break;

	}
}



void tcp_server_sender(void *arg) {
	TcpServer *pHandle = (TcpServer*)arg;
	OS_MSG_SIZE msg_size;
	CPU_TS ts;
	OS_ERR err;
	SMsg *pMsg;
	int retVal;

	while(1) {
		pMsg = OSTaskQPend(1000, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
		if(err == OS_ERR_NONE) {
			if(pMsg) {
				retVal = tcp_server_send_blocking(pHandle, pMsg->buf, pMsg->length);
				if(retVal > 0) {
					if(pHandle->send_handler) {
						pHandle->send_handler(pMsg->buf, pMsg->length);
					}
				} else {
					if(pHandle->error_handler) {
						pHandle->error_handler(pMsg->buf, pMsg->length);
					}
				}

				OSA_FixedMemFree((uint8_t*)pMsg);
			}
		}
	}
}



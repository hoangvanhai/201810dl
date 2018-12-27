/*
 * tcp_socket.c
 *
 *  Created on: Dec 18, 2018
 *      Author: PC
 */


#include <tcp_client.h>
#include <string.h>



int tcp_client_configuration(TcpClient *pEP, ip_addr_t ip,
		int port, int tx_size, int rx_size, uint8_t *rx_buff) {
	memset(pEP, 0, sizeof(TcpClient));
	pEP->ip = ip;
	pEP->port = port;
	pEP->rx_buf_size = rx_size;
	pEP->tx_buf_size = tx_size;
	pEP->rx_buff = rx_buff;
	pEP->status = Status_Disconnected;
	return 0;
}



int tcp_client_open_connection(TcpClient *pEP, int timeout) {
	int err = -3;

	bool status = true;
	if(Network_GetLinkStatus(&status)) {
		if(status == false) {
			//LREP("tcp_client_open_connection link down ... \r\n");
			return err;
		}
	}

    pEP->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (pEP->fd != INVALIDSOCK) {

    	set_nonblocking(pEP->fd);
    	set_buffer_size(pEP->fd, pEP->tx_buf_size, pEP->rx_buf_size);

        struct sockaddr_in addr;
        addr.sin_addr.s_addr = pEP->ip.addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(pEP->port);

        err = connect(pEP->fd, (struct sockaddr *)&addr, sizeof(struct sockaddr));
		if (err != 0) {
			struct timeval tv;
			fd_set fdw, fderror, fdr;
			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000;
			FD_ZERO(&fdw);
			FD_SET(pEP->fd, &fdw);
			FD_ZERO(&fderror);
			FD_SET(pEP->fd, &fderror);
			FD_ZERO(&fdr);
			FD_SET(pEP->fd, &fdr);
			err = select(pEP->fd + 1, &fdr, &fdw, &fderror, &tv);
			if (err < 0 && errno != EINTR) {
				err = -4;
			} else if (err > 0) {
				if (FD_ISSET(pEP->fd, &fderror)) {
					err = -5;
				} else if (FD_ISSET(pEP->fd, &fdw) && FD_ISSET(pEP->fd, &fdr)) {
					err = -6;
				} else if (FD_ISSET(pEP->fd, &fdw)) {
					err = 0;
				}
			} else if (err == 0) {
				err = -8;
			}
		}

        if (err) {
        	tcp_client_close_connection(pEP);
        }
    }

    return err;
}



int tcp_client_close_connection(TcpClient *pEP) {
    close(pEP->fd);
    pEP->fd = INVALIDSOCK;
    return 0;
}


int tcp_client_send_blocking(TcpClient *pEP, const uint8_t *data,
		int len) {
	int err = -3;
	if(pEP->status != Status_Connected)
		return -1;

	if(data == NULL || len <= 0)
		return -2;

	int event = wait_event(pEP->fd, 100, false, true);
	if(event & Event_Writeable) {
		err = send(pEP->fd, data, len, 0);
	}

	return err;
}

int tcp_client_send_nonblocking(TcpClient *pEP, const uint8_t *data,
		int len) {
	int retVal = 0;
	if(pEP->status == Status_Connected) {
		SMsg *pMsg = (SMsg*)OSA_FixedMemMalloc(sizeof(SMsg) + len);
		if(pMsg) {
			OS_ERR err;
			pMsg->length = len;
			pMsg->buf = (uint8_t*)pMsg + sizeof(SMsg);
			memcpy(pMsg->buf, data, len);
			OSTaskQPost(pEP->send_thread, (void*)pMsg,
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

void tcp_client_register_notify(TcpClient *pEP, NetworkConnNotify func) {
	if(func)
		pEP->notify = func;
}

void 	tcp_client_register_data_event(TcpClient *pEP, Network_DataEvent evt,
		NetworkDataEvent func) {
	switch(evt) {
	case Event_DataReceived:
		pEP->recv_handler = func;
		break;
	case Event_DataSendDone:
		pEP->send_handler = func;
		break;
	case Event_DataError:
		pEP->error_handler = func;
		break;

	}
}

int	tcp_client_get_status(TcpClient *pEP) {
	return pEP->status;
}


void tcp_client_listener(void *arg)  {

	TcpClient *pEP = (TcpClient*)arg;
    int rlen;
	int err;


	while(1) {
		// try connect to server
		err = tcp_client_open_connection(pEP, 2000);
		if(err != 0) {
			PRINTF("connect to %s:%d failed try again\r\n",
					ipaddr_ntoa(&pEP->ip), pEP->port);
			OSA_TimeDelay(2000);
			continue;
		}

		pEP->status = Status_Connected;
		if(pEP->notify)
			pEP->notify(pEP->status, Interface_Ethernet);

		PRINTF("connected to server %s:%d\r\n",
				ipaddr_ntoa(&pEP->ip), pEP->port);
		PRINTF("start listener \r\n");
		while(1) {
			//OSA_TimeDelay(100);
			int event = wait_event(pEP->fd, 100, true, false);
			if(event & Event_Readable) {
				rlen = recv(pEP->fd, pEP->rx_buff, 512, 0);
				if(rlen > 0) {
					if(pEP->recv_handler) {
						pEP->recv_handler(pEP->rx_buff, rlen);
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
		LREP("disconnected to server %s:%d\r\n",
				ipaddr_ntoa(&pEP->ip), pEP->port);
		tcp_client_close_connection(pEP);
		pEP->status = Status_Disconnected;
		if(pEP->notify)
			pEP->notify(pEP->status, Interface_Ethernet);
	}
}


void tcp_client_sender(void *arg) {
	TcpClient *pEP = (TcpClient*)arg;
	OS_MSG_SIZE msg_size;
	CPU_TS ts;
	OS_ERR err;
	SMsg *pMsg;
	int retVal;

	while(1) {
		pMsg = OSTaskQPend(1000, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
		if(err == OS_ERR_NONE) {
			if(pMsg) {
				retVal = tcp_client_send_blocking(pEP, pMsg->buf, pMsg->length);
				if(retVal > 0) {
					if(pEP->send_handler) {
						pEP->send_handler(pMsg->buf, pMsg->length);
					}
				} else {
					if(pEP->error_handler) {
						pEP->error_handler(pMsg->buf, pMsg->length);
					}
				}

				OSA_FixedMemFree((uint8_t*)pMsg);
			}
		}
	}
}





/*
 * ftp_client.c
 *
 *  Created on: Dec 19, 2018
 *      Author: PC
 */

#include <ftp_client.h>
#include <lib_str.h>
#include <string.h>
#include <filesystem.h>

CPU_CHAR buff[256];

int ftp_client_init_handle(FtpClient *pFC, uint8_t *tx_ctrl_buff,
		uint8_t *rx_ctrl_buff, uint8_t *tx_data_buff) {
	pFC->curr_sv_idx = 0;
	pFC->tx_buff_ctrl = tx_ctrl_buff;
	pFC->rx_buff_ctrl = rx_ctrl_buff;
	pFC->tx_buff_data = tx_data_buff;
	return 0;
}

int ftp_client_add_server(FtpClient *pFC, ServerInfo *server, uint8_t index) {
	pFC->server_list[index].enable 	 = server->enable;
	pFC->server_list[index].ip 		 = server->ip;
	pFC->server_list[index].port 	 = server->port;
	pFC->server_list[index].username = server->username;
	pFC->server_list[index].passwd 	 = server->passwd;

	LREP("setup point to ftp server %s:%d\r\n", ipaddr_ntoa(&server->ip), server->port);
	return 0;
}

uint8_t ftp_get_curr_server(FtpClient *pFC) {
	return pFC->curr_sv_idx;
}

int ftp_start_ctrl_sock(FtpClient *pFC) {

	int timeout = 1000;
	int err = -3;
	pFC->fd_ctrl = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (pFC->fd_ctrl != INVALIDSOCK) {

		//set_nonblocking(pEP->fd);
		//set_buffer_size(pEP->fd, pEP->tx_buf_size, pEP->rx_buf_size);

		struct sockaddr_in addr;
		addr.sin_addr.s_addr = pFC->server_list[pFC->curr_sv_idx].ip.addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(pFC->server_list[pFC->curr_sv_idx].port);

		err = connect(pFC->fd_ctrl, (struct sockaddr *)&addr, sizeof(struct sockaddr));
		if (err != 0) {
			struct timeval tv;
			fd_set fdw, fderror, fdr;
			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000;
			FD_ZERO(&fdw);
			FD_SET(pFC->fd_ctrl, &fdw);
			FD_ZERO(&fderror);
			FD_SET(pFC->fd_ctrl, &fderror);
			FD_ZERO(&fdr);
			FD_SET(pFC->fd_ctrl, &fdr);
			err = select(pFC->fd_ctrl + 1, &fdr, &fdw, &fderror, &tv);
			if (err < 0 && errno != EINTR) {
				err = -4;
			} else if (err > 0) {
				if (FD_ISSET(pFC->fd_ctrl, &fderror)) {
					err = -5;
				} else if (FD_ISSET(pFC->fd_ctrl, &fdw) && FD_ISSET(pFC->fd_ctrl, &fdr)) {
					err = -6;
				} else if (FD_ISSET(pFC->fd_ctrl, &fdw)) {
					err = 0;
				}
			} else if (err == 0) {
				err = -8;
			}
		}

		if (err) {
			ftp_close_ctrl_sock(pFC);
		}
	}

	return err;

}


int	ftp_close_ctrl_sock(FtpClient *pFC) {
    close(pFC->fd_ctrl);
    pFC->fd_ctrl = INVALIDSOCK;
    pFC->status_ctrl = Status_Disconnected;
    return 0;
}

int ftp_start_data_sock(FtpClient *pFC, int port) {
	int timeout = 1000;
	int err = -3;
	pFC->fd_data = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (pFC->fd_data != INVALIDSOCK) {

		//set_nonblocking(pEP->fd);
		//set_buffer_size(pEP->fd, pEP->tx_buf_size, pEP->rx_buf_size);

		struct sockaddr_in addr;
		addr.sin_addr.s_addr = pFC->server_list[pFC->curr_sv_idx].ip.addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		err = connect(pFC->fd_data, (struct sockaddr *)&addr, sizeof(struct sockaddr));
		if (err != 0) {
			struct timeval tv;
			fd_set fdw, fderror, fdr;
			tv.tv_sec = timeout / 1000;
			tv.tv_usec = (timeout % 1000) * 1000;
			FD_ZERO(&fdw);
			FD_SET(pFC->fd_data, &fdw);
			FD_ZERO(&fderror);
			FD_SET(pFC->fd_data, &fderror);
			FD_ZERO(&fdr);
			FD_SET(pFC->fd_data, &fdr);
			err = select(pFC->fd_data + 1, &fdr, &fdw, &fderror, &tv);
			if (err < 0 && errno != EINTR) {
				err = -4;
			} else if (err > 0) {
				if (FD_ISSET(pFC->fd_data, &fderror)) {
					err = -5;
				} else if (FD_ISSET(pFC->fd_data, &fdw) && FD_ISSET(pFC->fd_data, &fdr)) {
					err = -6;
				} else if (FD_ISSET(pFC->fd_data, &fdw)) {
					err = 0;
				}
			} else if (err == 0) {
				err = -8;
			}
		}

		if (err) {
			ftp_close_data_sock(pFC);
		}
	}

	return err;
}

int	ftp_close_data_sock(FtpClient *pFC) {
    close(pFC->fd_data);
    pFC->fd_data = INVALIDSOCK;
    pFC->status_data = Status_Disconnected;
    return 0;
}

int ftp_get_pasv_port(FtpClient *pFC) {

	char *pch;
	pch = strtok((char*)pFC->rx_buff_ctrl, ".,()");
	uint8_t high = 0, low = 0;
	int i = 0;
	while(pch != NULL) {

		pch = strtok(NULL, ".,()");
		i++;
		if(i == 5 && pch != NULL) {
			high = atoi(pch);
		}
		if(i == 6 && pch != NULL) {
			low = atoi(pch);
		}
	}

	LREP("high = %d low = %d \r\n", high, low);
	return (256 * high) + low;
}

int ftp_setup_channel(FtpClient *pFC, uint8_t idx) {
	pFC->curr_sv_idx = idx;
	int err;
	err = ftp_start_ctrl_sock(pFC);
	if(err == 0) {
		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err <= 0) {
			return FTP_ERR_RECV_CTRL_MSG;
		} else {
			LREP("recv msg %s\r\n", pFC->rx_buff_ctrl);
		}

		memset(buff, 0, 100);
		Str_Copy(buff, "USER ");
		Str_Cat(buff, pFC->server_list[pFC->curr_sv_idx].username);
		Str_Cat(buff, "\r\n");
		if(ftp_send_ctrl_msg(pFC, buff) <= 0) {
			return FTP_ERR_SEND_CTRL_MSG;
		}

		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err <= 0) {
			return FTP_ERR_RECV_CTRL_MSG;
		} else {
			LREP("recv msg %s\r\n", pFC->rx_buff_ctrl);
		}

		memset(buff, 0, 100);
		Str_Copy(buff, "PASS ");
		Str_Cat(buff, pFC->server_list[pFC->curr_sv_idx].passwd);
		Str_Cat(buff, "\r\n");
		if(ftp_send_ctrl_msg(pFC, buff) <= 0) {
			return FTP_ERR_SEND_CTRL_MSG;
		}

		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err <= 0) {
			return FTP_ERR_RECV_CTRL_MSG;
		} else {
			LREP("recv msg %s\r\n", pFC->rx_buff_ctrl);
		}

		return ftp_start_data_channel(pFC);
	}

	return FTP_ERR_OPEN_CTRL_CONN;;
}

int	ftp_start_data_channel(FtpClient *pFC) {

	int err;
	if(ftp_send_ctrl_msg(pFC, "pasv\r\n") <= 0) {
		LREP("pasv cmd failed\n");
		return FTP_ERR_SEND_CTRL_MSG;
	}

	err = ftp_recv_ctrl_msg(pFC, 100);
	if(err <= 0) {
		LREP("start_data_channel failed\n");
		return FTP_ERR_RECV_CTRL_MSG;
	} else {
		LREP("recv msg %s\r\n", pFC->rx_buff_ctrl);
	}

	int port = ftp_get_pasv_port(pFC);
	LREP("get port = %d\r\n", port);
	if(port != 0) {
		err = ftp_start_data_sock(pFC, port);
		if(err != 0) {
			LREP("data conn failed {}\n", err);
			return FTP_ERR_OPEN_DATA_CONN;
		}
	} else {
		LREP("no ip and port in recv msg\n");
		return FTP_ERR_PASV;
	}

	pFC->status_data = Status_Connected;
	LREP("set up data channel success \r\n");
	return FTP_ERR_NONE;
}

int ftp_remote_cwd(FtpClient *pFC, const char *path) {

	memset(buff, 0, 256);

	Str_Copy(buff, "CWD ");

	Str_Cat(buff, path);

	Str_Cat(buff, "\r\n");

	return ftp_remote_send_recv_ctrl(pFC, buff);
}

int ftp_remote_mkd(FtpClient *pFC, const char *path) {

	memset(buff, 0, 256);

	Str_Copy(buff, "MKD ");

	Str_Cat(buff, path);

	Str_Cat(buff, "\r\n");

	return ftp_remote_send_recv_ctrl(pFC, buff);
}

int ftp_remote_mkd_recursive(FtpClient *pFC, char* remote_path) {

    int err = FTP_ERR_CD;
    LREP("remote path = %d\r\n", remote_path);

    CPU_CHAR *path = (CPU_CHAR*)OSA_FixedMemMalloc(128);
    if(path == NULL)
    	return err;

    memset(path, 0, 128);
	char *pch;
	pch = strtok((char*)remote_path, "/");

	while(pch != NULL) {
		Str_Cat(path, pch);
		Str_Cat(path, "/");
		err = ftp_remote_mkd(pFC, path);
		pch = strtok(NULL, "/");
	}

    if(err == FTP_ERR_NONE) {
        err = ftp_remote_cwd(pFC, path);
        if((err == FTP_ERR_NONE) &&
        	(Str_Str((CPU_CHAR*)pFC->rx_buff_ctrl, "successful") != NULL)) {
                err = FTP_ERR_NONE;
        } else {
            err = FTP_ERR_CD;
        }
    }

    OSA_FixedMemFree(path);

    return err;

}

int ftp_remote_put(FtpClient *pFC, uint8_t index,
								char *filename,
								char *local_path,
								char *remote_path) {

	int retVal;
	int err = FTP_ERR_PUT;
	uint8_t data[128];
	Str_Copy(data, local_path);

	err = ftp_setup_channel(pFC, index);
	if(err != FTP_ERR_NONE)
		return err;

	err = ftp_remote_mkd_recursive(pFC, (char*)remote_path);
	if(err != FTP_ERR_NONE)
			return err;

	memset(pFC->tx_buff_data, 0, 256);
	Str_Copy(pFC->tx_buff_data, "stor ");
	Str_Cat(pFC->tx_buff_data, &filename[1]);
	Str_Cat(pFC->tx_buff_data, "\r\n");
	err = ftp_remote_send_recv_ctrl(pFC, pFC->tx_buff_data);
	if(err != FTP_ERR_NONE)
		return err;

	int code = ftp_get_code(pFC);
	if(code != 125 && code != 150) {
		LREP("get code %d \r\n", code);
		err = FTP_ERR_CODE;
		return err;
	}

	memset(pFC->tx_buff_data, 0, 256);
	Str_Copy(pFC->tx_buff_data, data);
	Str_Cat(pFC->tx_buff_data, filename);



	UINT read;
	FIL file;
	retVal = f_open(&file, pFC->tx_buff_data, FA_OPEN_EXISTING | FA_READ);
	if(retVal == FR_OK) {
		LREP("opened file %s\r\n", pFC->tx_buff_data);
		int fr;
		do {
			fr = f_read(&file, pFC->tx_buff_data, 256, &read);
			if(read > 0) {
				LREP("read line: %s\r\n", pFC->tx_buff_data);
				retVal = ftp_remote_send_data(pFC, pFC->tx_buff_data, read);
				if(retVal <= 0) {
					LREP("send data failed\r\n");
					err = FTP_ERR_SEND_DATA_MSG;
					break;
				}
			}
		} while(fr == FR_OK && read > 0);
	} else {
		LREP("open file %s error\r\n", pFC->tx_buff_data);
		err = FTP_ERR_FILE;
	}

	if(err == FTP_ERR_NONE) {
		err = ftp_recv_ctrl_msg(pFC, 100);

		if(err != FTP_ERR_NONE)
			return err;

		// check control message
		if(Str_Str((CPU_CHAR*)pFC->rx_buff_ctrl, "Complete") != NULL) {
			LREP("Put file success %s\r\n", pFC->rx_buff_ctrl);
		}
	}

	return err;
}



int ftp_remote_send_recv_ctrl(FtpClient *pFC, const char *msg) {
	int err;
	err = ftp_send_ctrl_msg(pFC, msg);
	if(err <= 0) {
		return FTP_ERR_SEND_CTRL_MSG;
	}

	err = ftp_recv_ctrl_msg(pFC, 200);
	if(err <= 0) {
		return FTP_ERR_RECV_CTRL_MSG;
	}

	LREP("recv msg: %s\r\n", pFC->rx_buff_ctrl);

	return FTP_ERR_NONE;
}


int ftp_remote_send_data(FtpClient *pFC, const uint8_t *data, int len) {

	int err = -3;
	if(pFC->status_data != Status_Connected)
		return -1;

	if(data == NULL || len <= 0)
		return -2;

	int event = wait_event(pFC->fd_data, 100, false, true);
	if(event & Event_Writeable) {
		err = send(pFC->fd_data, data, len, 0);
	}

	return err;
}


int	ftp_destroy_channel(FtpClient *pFC) {

	ftp_close_data_sock(pFC);
	ftp_close_ctrl_sock(pFC);

	LREP("close two socket\r\n");
	return 0;
}

int ftp_send_ctrl_msg(FtpClient *pFC, const char *msg) {

	int err = -3;
	int len;
	if(pFC->status_ctrl != Status_Connected)
		return -1;

	if(msg == NULL)
		return -2;

	len = Str_Len_N((CPU_CHAR*)msg, 100);

	int event = wait_event(pFC->fd_ctrl, 100, false, true);
	if(event & Event_Writeable) {
		err = send(pFC->fd_ctrl, msg, len, 0);
		LREP("send %s\r\n", msg);
	}

	return err;
}


int ftp_recv_ctrl_msg(FtpClient *pFC, int timeout) {
	int rlen = -1;
	OSA_SleepMs(timeout);
	int event = wait_event(pFC->fd_ctrl, 100, true, false);
	if(event & Event_Readable) {
		memset(pFC->rx_buff_ctrl, 0, 512);
		rlen = recv(pFC->fd_ctrl, pFC->rx_buff_ctrl, 512, 0);
	}

	return rlen;
}


void ftp_client_sender(void *arg) {

	FtpClient *pFC = (FtpClient*)arg;
	OS_MSG_SIZE msg_size;
	CPU_TS ts;
	OS_ERR err;
	FtpMsg *pMsg;
	int retVal;

	while(1) {
		pMsg = OSTaskQPend(1000, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
		if(err == OS_ERR_NONE) {
			if(pMsg) {
				LREP("get data %s %s\r\n", pMsg->local_path, pMsg->file_name);
				if(pFC->server_list[0].enable) {
					retVal = ftp_remote_put(pFC, 0,
							(char*)pMsg->file_name,
							(char*)pMsg->local_path,
							(char*)pMsg->local_path);

					if(retVal != FTP_ERR_NONE) {
						LREP("put file server 0 err = %d\r\n", retVal);
					}
				}

				ftp_destroy_channel(pFC);

				if(pFC->server_list[1].enable) {
					retVal = ftp_remote_put(pFC, 1,
							(char*)pMsg->file_name,
							(char*)pMsg->local_path,
							(char*)pMsg->local_path);

					if(retVal != FTP_ERR_NONE) {
						LREP("put file server 1 err = %d\r\n", retVal);
					}
				}

				ftp_destroy_channel(pFC);

				OSA_FixedMemFree((uint8_t*)pMsg);
			}
		}
		LREP("thread ftp sender\r\n");
	}
}


int	ftp_add_filename(FtpClient *pFC, const uint8_t *local_path, const uint8_t *file_name) {
	int retVal = 0;

	FtpMsg *pMsg = (FtpMsg*)OSA_FixedMemMalloc(sizeof(FtpMsg));
	if(pMsg) {
		OS_ERR err;

		Str_Copy_N((CPU_CHAR*)pMsg->local_path, (const CPU_CHAR*)local_path,
				sizeof(pMsg->local_path));

		Str_Copy_N((CPU_CHAR*)pMsg->file_name, (const CPU_CHAR*)file_name,
				sizeof(pMsg->file_name));

		OSTaskQPost(pFC->send_thread, (void*)pMsg,
				sizeof(FtpMsg), OS_OPT_POST_FIFO, &err);
		if(err != OS_ERR_NONE) {
			retVal = -2;
		}
	} else {
		retVal = -1;
	}

	return retVal;
}

int	ftp_get_code(FtpClient *pFC) {

	return Str_ParseNbr_Int32U((CPU_CHAR*)pFC->rx_buff_ctrl, NULL, 10);
}






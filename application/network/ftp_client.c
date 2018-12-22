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
#include <checksum.h>
#include <network.h>

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
	pFC->server_list[index].prefix 	 = server->prefix;

	LREP("setup point to ftp server %s:%d\r\n", ipaddr_ntoa(&server->ip), server->port);
	return 0;
}

uint8_t ftp_get_curr_server(FtpClient *pFC) {
	return pFC->curr_sv_idx;
}

int ftp_start_ctrl_sock(FtpClient *pFC) {

	int timeout = 1000;
	int err = -3;
	bool status = true;

	if(Network_GetLinkStatus(&status)) {
		if(status == false) {
			LREP("ftp_start_ctrl_sock link down ... \r\n");
			return err;
		}
	}

	pFC->fd_ctrl = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (pFC->fd_ctrl != INVALIDSOCK) {

		set_nonblocking(pFC->fd_ctrl);
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

	if(err == 0) {
		pFC->status_ctrl = Status_Connected;
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
	bool status = true;

	if(Network_GetLinkStatus(&status)) {
		if(status == false) {
			LREP("ftp_start_data_sock link down ... \r\n");
			return err;
		}
	}

	pFC->fd_data = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (pFC->fd_data != INVALIDSOCK) {

		set_nonblocking(pFC->fd_data);
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

	if(err == 0) {
		pFC->status_data = Status_Connected;
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
			LREP("recv msg %s", pFC->rx_buff_ctrl);
		}

		memset(pFC->tx_buff_ctrl, 0, 100);
		Str_Copy_N((CPU_CHAR*)pFC->tx_buff_ctrl, "USER ", FTP_CLIENT_BUFF_SIZE);
		Str_Cat_N((CPU_CHAR*)pFC->tx_buff_ctrl,
				pFC->server_list[pFC->curr_sv_idx].username,
				FTP_CLIENT_BUFF_SIZE);
		Str_Cat_N((CPU_CHAR*)pFC->tx_buff_ctrl, "\r\n", FTP_CLIENT_BUFF_SIZE);

		err = ftp_send_ctrl_msg(pFC, (char*)pFC->tx_buff_ctrl);
		if(err <= 0) {
			LREP("send username error err = %d\r\n", err);
			return FTP_ERR_SEND_CTRL_MSG;
		}

		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err <= 0) {
			return FTP_ERR_RECV_CTRL_MSG;
		} else {
			LREP("recv msg %s", pFC->rx_buff_ctrl);
		}

		memset(pFC->tx_buff_ctrl, 0, 100);
		Str_Copy_N((CPU_CHAR*)pFC->tx_buff_ctrl, "PASS ", FTP_CLIENT_BUFF_SIZE);
		Str_Cat_N((CPU_CHAR*)pFC->tx_buff_ctrl,
				pFC->server_list[pFC->curr_sv_idx].passwd,
				FTP_CLIENT_BUFF_SIZE);
		Str_Cat_N((CPU_CHAR*)pFC->tx_buff_ctrl, "\r\n", FTP_CLIENT_BUFF_SIZE);
		if(ftp_send_ctrl_msg(pFC, (char*)pFC->tx_buff_ctrl) <= 0) {
			return FTP_ERR_SEND_CTRL_MSG;
		}

		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err <= 0) {
			return FTP_ERR_RECV_CTRL_MSG;
		} else {
			LREP("recv msg %s", pFC->rx_buff_ctrl);
		}

		return ftp_start_data_channel(pFC);
	}

	return FTP_ERR_OPEN_CTRL_CONN;;
}

int	ftp_start_data_channel(FtpClient *pFC) {

	int err;
	err = ftp_remote_send_recv_ctrl(pFC, "PASV\r\n");
	if(err != FTP_ERR_NONE) {
		LREP("start_data_channel failed\n");
		return err;
	}
	//else
	//{
	//	LREP("recv msg %s\r\n", pFC->rx_buff_ctrl);
	//}

	int port = ftp_get_pasv_port(pFC);
	LREP("get port = %d\r\n", port);
	if(port != 0) {
		err = ftp_start_data_sock(pFC, port);
		if(err != 0) {
			LREP("open data channel err = %d\r\n", err);
			return FTP_ERR_OPEN_DATA_CONN;
		}
	} else {
		LREP("no ip and port in recv msg\n");
		return FTP_ERR_PASV;
	}
	//LREP("set up data channel success \r\n");
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
    LREP("prefix = %s remote path = %s\r\n",
    		pFC->server_list[pFC->curr_sv_idx].prefix,
    		remote_path);

    // if set prefix
    if(Str_Len(pFC->server_list[pFC->curr_sv_idx].prefix) > 0) {
    	// try cd to prefix
    	err = ftp_remote_cwd(pFC, pFC->server_list[pFC->curr_sv_idx].prefix);
		if((err == FTP_ERR_NONE) &&
			(Str_Str_N((CPU_CHAR*)pFC->rx_buff_ctrl,
					"successful", FTP_CLIENT_BUFF_SIZE) != NULL)) {
			LREP("cd to prefix %s success\r\n", pFC->server_list[pFC->curr_sv_idx].prefix);
		} else { // if can not cd to, try make frefix
			LREP("cd failed try mkd %s\r\n", pFC->server_list[pFC->curr_sv_idx].prefix);
			err = ftp_remote_mkd(pFC, pFC->server_list[pFC->curr_sv_idx].prefix);
			if(err != FTP_ERR_NONE) { // if make prefix failed
				LREP("mkd error %d \r\n", err);
				ASSERT(FALSE);
				return err;
			} else {
				LREP("mkd ok, cd to %s\r\n", pFC->server_list[pFC->curr_sv_idx].prefix);
				err = ftp_remote_cwd(pFC, pFC->server_list[pFC->curr_sv_idx].prefix);
				if((err == FTP_ERR_NONE) &&
					(Str_Str_N((CPU_CHAR*)pFC->rx_buff_ctrl,
							"successful", FTP_CLIENT_BUFF_SIZE) != NULL)) {
					LREP("cd to prefix %s success\r\n",
							pFC->server_list[pFC->curr_sv_idx].prefix);
				} else {
					ASSERT(FALSE);
					return err;
				}
			}
		}
    } else { // no prefix require
		err = ftp_remote_cwd(pFC, "/");
		if(err != FTP_ERR_NONE) {
			ASSERT(FALSE);
			return err;
		}
    }

    // try cd to prefer localtion
    err = ftp_remote_cwd(pFC, remote_path);
	if((err == FTP_ERR_NONE) &&
		(Str_Str_N((CPU_CHAR*)pFC->rx_buff_ctrl,
				"successful", FTP_CLIENT_BUFF_SIZE) != NULL)) {
		LREP("cd to %s success\r\n", remote_path);
		return FTP_ERR_NONE;
	}

	// if prefer location not existed, make new
	// change to prefix
    if(Str_Len(pFC->server_list[pFC->curr_sv_idx].prefix) == 0) {
		err = ftp_remote_cwd(pFC, "/");
		if(err != FTP_ERR_NONE) {
			ASSERT(FALSE);
			return err;
		}
    }

    // start make new folder
    CPU_CHAR *path = (CPU_CHAR*)OSA_FixedMemMalloc(128);
    if(path == NULL)
    	return FTP_ERR_CD;

    memset(path, 0, 128);
	char *pch;
	pch = strtok((char*)remote_path, "/");

	while(pch != NULL) {
		Str_Cat_N(path, pch, FTP_CLIENT_BUFF_SIZE);
		Str_Cat_N(path, "/", FTP_CLIENT_BUFF_SIZE);
		err = ftp_remote_mkd(pFC, path);
		pch = strtok(NULL, "/");
	}

    if(err == FTP_ERR_NONE && Str_Len(path) > 0) {
        err = ftp_remote_cwd(pFC, path);
        if((err == FTP_ERR_NONE) &&
        	(Str_Str_N((CPU_CHAR*)pFC->rx_buff_ctrl,
        			"successful", FTP_CLIENT_BUFF_SIZE) != NULL)) {
                err = FTP_ERR_NONE;
        } else {
            err = FTP_ERR_CD;
        }
    }

    OSA_FixedMemFree((uint8_t*)path);

    return err;

}

int ftp_remote_put(FtpClient *pFC, uint8_t index,
								char *filename,
								char *local_path,
								char *remote_path) {

	int retVal;
	int err = FTP_ERR_PUT;
	CPU_CHAR data[128];
	memset(data, 0, 128);
	Str_Copy_N(data, local_path, 128);
	LREP("local path = %s \r\n", data);

	err = ftp_setup_channel(pFC, index);
	if(err != FTP_ERR_NONE) {
		ERR("setup channel failed \r\n");
		return err;
	}

	err = ftp_remote_mkd_recursive(pFC, (char*)remote_path);
	if(err != FTP_ERR_NONE) {
		WARN_LINE("make folder recuresive error = %d\r\n", err);
		return err;
	}


	memset(pFC->tx_buff_data, 0, 256);
	Str_Copy_N((CPU_CHAR*)pFC->tx_buff_data, "STOR ", FTP_CLIENT_BUFF_SIZE);
	Str_Cat_N((CPU_CHAR*)pFC->tx_buff_data, &filename[1], FTP_CLIENT_BUFF_SIZE);
	Str_Cat_N((CPU_CHAR*)pFC->tx_buff_data, "\r\n", FTP_CLIENT_BUFF_SIZE);
	err = ftp_remote_send_recv_ctrl(pFC, (char*)pFC->tx_buff_data);
	if(err != FTP_ERR_NONE) {
		WARN_LINE("STOR command failed\r\n");
		return err;
	}

	int code = ftp_get_code(pFC);
	LREP("get code %d \r\n", code);
	if(code != 125 && code != 150) {
		err = FTP_ERR_CODE;
		WARN_LINE("get invalid code %d\r\n", code);
		return err;
	}

	memset(pFC->tx_buff_data, 0, 256);
	Str_Copy_N((CPU_CHAR*)pFC->tx_buff_data, data, FTP_CLIENT_BUFF_SIZE);
	Str_Cat_N((CPU_CHAR*)pFC->tx_buff_data, filename, FTP_CLIENT_BUFF_SIZE);

	// ensure the file is existed before use STOR command
	if(check_obj_existed((char*)pFC->tx_buff_data) == false) {
		ERR_LINE("file %s not found !\r\n", pFC->tx_buff_data);
		return FTP_ERR_FILE;
	}

	UINT read;
	FIL file;
	retVal = f_open(&file, (const char*)pFC->tx_buff_data, FA_OPEN_EXISTING | FA_READ);
	if(retVal == FR_OK) {
		int fr;
		do {
			fr = f_read(&file, pFC->tx_buff_data, 256, &read);
			if(read > 0) {
				retVal = ftp_remote_send_data(pFC, pFC->tx_buff_data, read);
				if(retVal <= 0) {
					LREP("send data failed\r\n");
					err = FTP_ERR_SEND_DATA_MSG;
					break;
				}
				OSA_SleepMs(1000);
			}
		} while(fr == FR_OK && read > 0);
	} else {
		ERR_LINE("open file %s error !\r\n", pFC->tx_buff_data);
		ASSERT(FALSE);
		err = FTP_ERR_FILE;
	}

	ftp_close_data_sock(pFC);

	if(err == FTP_ERR_NONE) {
		retVal = ftp_recv_ctrl_msg(pFC, 500);
		if(retVal <= 0) {
			ASSERT(FALSE);
			err = FTP_ERR_RECV_CTRL_MSG;
		} else {
			// check control message
			LREP("recv msg %s", pFC->rx_buff_ctrl);
			if(Str_Str_N((CPU_CHAR*)pFC->rx_buff_ctrl,
					"complete", FTP_CLIENT_BUFF_SIZE) == NULL) {
				err = FTP_ERR_PUT;
			}
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

	OSA_SleepMs(200);

	err = ftp_recv_ctrl_msg(pFC, 300);
	if(err <= 0) {
		return FTP_ERR_RECV_CTRL_MSG;
	}
	//LREP("recv msg: %s", pFC->rx_buff_ctrl);
	return FTP_ERR_NONE;
}


int ftp_remote_send_data(FtpClient *pFC, const uint8_t *data, int len) {

	int err = -3;
	if(pFC->status_data != Status_Connected)
		return -1;

	if(data == NULL || len <= 0)
		return -2;

	int event = wait_event(pFC->fd_data, 500, false, true);
	if(event & Event_Writeable) {
		err = send(pFC->fd_data, data, len, 0);

	}

	LREP("sent = %d\r\n", err);
	return err;
}


int	ftp_destroy_channel(FtpClient *pFC) {

	ftp_close_data_sock(pFC);
	ftp_close_ctrl_sock(pFC);

	LREP("close socket pair\r\n");
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
		LREP("send msg %s", msg);
	}

	return err;
}


int ftp_recv_ctrl_msg(FtpClient *pFC, int timeout) {
	int rlen = -1;
	OSA_SleepMs(timeout);
	int event = wait_event(pFC->fd_ctrl, 100, true, false);
	if(event & Event_Readable) {
		memset(pFC->rx_buff_ctrl, 0, 256);
		rlen = recv(pFC->fd_ctrl, pFC->rx_buff_ctrl, 256, 0);
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
	bool stat;
	while(1) {
		pMsg = OSTaskQPend(1000, OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
		if(err == OS_ERR_NONE) {
			if(pMsg) {
				LREP("SENDER PROCSESSING: [%s%s]\r\n", pMsg->local_path, pMsg->file_name);
				uint8_t *temp = OSA_FixedMemMalloc(256);
				if(temp != NULL) {
					memset(temp, 0, 256);
					Str_Copy_N((CPU_CHAR*)temp, (CPU_CHAR*)pMsg->local_path,
							FTP_CLIENT_BUFF_SIZE);
					Str_Cat_N((CPU_CHAR*)temp, (CPU_CHAR*)pMsg->file_name,
							FTP_CLIENT_BUFF_SIZE);
					stat = check_obj_existed((char*)temp);
					OSA_FixedMemFree(temp);
					if(stat) {
						CPU_CHAR temp_path[128];
						Str_Copy_N(temp_path, (CPU_CHAR*)pMsg->local_path, 128);
						{
							if(pFC->server_list[0].enable) {
								retVal = ftp_remote_put(pFC, 0,
										(char*)pMsg->file_name,
										(char*)pMsg->local_path,
										(char*)pMsg->local_path);

								if(retVal != FTP_ERR_NONE) {
									LREP("put file server 0 err = %d\r\n", retVal);
									ftp_print_err(retVal);
									Str_Copy_N((CPU_CHAR*)pMsg->local_path, temp_path, 128);

									// process send failed
									ring_file_record_t *pRecord = (ring_file_record_t*)OSA_FixedMemMalloc(sizeof(ring_file_record_t));
									if(pRecord)
									{
										memset(pRecord, 0, sizeof(ring_file_record_t));
										memcpy(pRecord->dir_path, temp_path, strlen(temp_path));
										memcpy((char*)pRecord->file_name, (char*)pMsg->file_name,
												strlen((char*)pMsg->file_name));

										//TODO: mark which FTP server to send by Flag
										pRecord->flag = 1;

										//TODO: calculate CRC of record.
										pRecord->crc = crc_16((const unsigned char*)pRecord, sizeof(ring_file_record_t) - 2);
										// Push to Retry-table
										ring_file_push_back(&g_retryTable, pRecord);

										OSA_FixedMemFree((uint8_t*)pRecord);
									}
									else
									{
										WARN("Unable to allocate memory!!!\r\n");
									}
								}

								ftp_destroy_channel(pFC);
							}

							//if(pFC->server_list[1].enable) {
							//	retVal = ftp_remote_put(pFC, 1,
							//			(char*)pMsg->file_name,
							//			(char*)pMsg->local_path,
							//			(char*)pMsg->local_path);
							//
							//	if(retVal != FTP_ERR_NONE) {
							//		LREP("put file server 1 err = %d\r\n", retVal);
							//		ftp_print_err(retVal);
							//		Str_Copy_N((CPU_CHAR*)pMsg->local_path, temp_path, 128);
							//		OSA_SleepMs(1000);
							//	}
							//	ftp_destroy_channel(pFC);
							//}





						}
					} else {
						ASSERT(FALSE);
					}
				}
				OSA_FixedMemFree((uint8_t*)pMsg);

			} // if msg pointer not null
		} else if(err == OS_ERR_TIMEOUT) { // if not message in queue

			// TODO [manhbt] No new file arrived, process retry table
			if(ring_file_get_count(&g_retryTable) > 0) {

				ring_file_record_t *pRecord = (ring_file_record_t *)OSA_FixedMemMalloc(sizeof(ring_file_record_t));
				if(!pRecord) {
					WARN("Unable to allocate memory for temporarily buffer !!!\r\n");
					continue;
				}

				memset(pRecord, 0, sizeof(ring_file_record_t));

				if(ring_file_get_front(&g_retryTable, pRecord) != TRUE) {
					WARN("Unable to GET record from Retry table");
					OSA_FixedMemFree((uint8_t*)pRecord);
					continue;
				}

				// TODO: [manhbt] verify Record (check CRC)
				uint16_t calc_checksum = crc_16((const unsigned char*)pRecord, sizeof(ring_file_record_t)-2);
				if(calc_checksum != pRecord->crc)
				{
					WARN("CRC not matched, cal: %.2x, got: %.2x", calc_checksum, pRecord->crc);
					OSA_FixedMemFree((uint8_t*)pRecord);
					continue;
				}

				// TODO: [manhbt] Re-send File
				LREP("Trying to re-send file %s/%s", pRecord->dir_path, pRecord->file_name);

				//retVal = net_ftp_client_send_file(pRecord->dir_path, pRecord->file_name);

				if(retVal == FTP_ERR_NONE) {
					//TODO: [manhbt] Pop out & erase record from retry table
					LREP("Re-send file OK, pop out and delete record from retry table");
					ring_file_pop_front(&g_retryTable, pRecord);
				} else {
					WARN("Re-send file FAILED, retry table remains unchanged");
				}
				OSA_FixedMemFree((uint8_t*)pRecord);
			} else {
				LREP(".");//("No new file arrived, process retry table!!!");
			}
		}
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


void ftp_print_err(int err) {
	switch(err) {
	case FTP_ERR_NONE: LREP("FTP_ERR_NONE\r\n"); break;
	case FTP_ERR_FILE: LREP("FTP_ERR_FILE\r\n"); break;
	case FTP_ERR_PUT: LREP("FTP_ERR_PUT\r\n"); break;
	case FTP_ERR_GET: LREP("FTP_ERR_GET\r\n"); break;
	case FTP_ERR_CD: LREP("FTP_ERR_CD\r\n"); break;
	case FTP_ERR_NCD: LREP("FTP_ERR_NCD\r\n"); break;
	case FTP_ERR_MKD: LREP("FTP_ERR_MKD\r\n"); break;
	case FTP_ERR_PWD: LREP("FTP_ERR_PWD\r\n"); break;
	case FTP_ERR_OPEN_CTRL_CONN: LREP("FTP_ERR_OPEN_CTRL_CONN\r\n"); break;
	case FTP_ERR_OPEN_DATA_CONN: LREP("FTP_ERR_OPEN_DATA_CONN\r\n"); break;
	case FTP_ERR_RECV_CTRL_MSG: LREP("FTP_ERR_RECV_CTRL_MSG\r\n"); break;
	case FTP_ERR_RECV_DATA_MSG: LREP("FTP_ERR_RECV_DATA_MSG\r\n"); break;
	case FTP_ERR_SEND_CTRL_MSG: LREP("FTP_ERR_SEND_CTRL_MSG\r\n"); break;
	case FTP_ERR_SEND_DATA_MSG: LREP("FTP_ERR_SEND_DATA_MSG\r\n"); break;
	case FTP_ERR_AUTHEN: LREP("FTP_ERR_AUTHEN\r\n"); break;
	case FTP_ERR_PASV: LREP("FTP_ERR_PASV\r\n"); break;
	case FTP_ERR_CODE: LREP("FTP_ERR_CODE\r\n"); break;

	default:
		break;
	}
}



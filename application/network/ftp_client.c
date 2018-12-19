/*
 * ftp_client.c
 *
 *  Created on: Dec 19, 2018
 *      Author: PC
 */

#include <ftp_client.h>
#include <lib_str.h>
#include <string.h>

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
		if(i == 4) {
			high = atoi(pch);
		}
		if(i == 5) {
			low = atoi(pch);
		}
	}

	return (256 * high) + low;
}

int ftp_setup_channel(FtpClient *pFC, uint8_t idx) {
	pFC->curr_sv_idx = idx;
	int err;
	err = ftp_start_ctrl_sock(pFC);
	if(err == 0) {
		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err != 0) {
			return FTP_ERR_RECV_CTRL_MSG;
		}

		memset(buff, 0, 100);
		Str_Copy(buff, "USER ");
		Str_Cat(buff, pFC->server_list[pFC->curr_sv_idx].username);
		if(ftp_send_ctrl_msg(pFC, buff) <= 0) {
			return FTP_ERR_SEND_CTRL_MSG;
		}

		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err != 0) {
			return FTP_ERR_RECV_CTRL_MSG;
		}

		memset(buff, 0, 100);
		Str_Copy(buff, "PASS ");
		Str_Cat(buff, pFC->server_list[pFC->curr_sv_idx].passwd);
		if(ftp_send_ctrl_msg(pFC, buff) <= 0) {
			return FTP_ERR_SEND_CTRL_MSG;
		}

		err = ftp_recv_ctrl_msg(pFC, 100);
		if(err != 0) {
			return FTP_ERR_RECV_CTRL_MSG;
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
	}

	int port = ftp_get_pasv_port(pFC);
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

	return FTP_ERR_NONE;
}

int ftp_remote_cwd(FtpClient *pFC, const char *path) {

	return 0;
}

int ftp_remote_mkd(FtpClient *pFC, const char *path) {


	return 0;
}

int ftp_remote_put(FtpClient *pFC, uint8_t index,
								const char *filename,
								const char *local_path,
								const char *remote_path) {

	int err = FTP_ERR_PUT;

	err = ftp_setup_channel(pFC, index);
	if(err != FTP_ERR_NONE)
		return err;

	err = ftp_remote_mkd_recursive(pFC, remote_path);
	if(err != FTP_ERR_NONE)
			return err;

	err = ftp_remote_send_recv_ctrl(pFC, filename);
	if(err != FTP_ERR_NONE)
		return err;

	int code = ftp_get_code(pFC);
	if(code != 125 && code != 150) {
		err = FTP_ERR_CODE;
		return err;
	}

	while(true)
	{
		int count = 0;

		if(ftp_remote_send_data(pFC, pFC->tx_buff_data, count) <= 0)
		{
			LREP("send data failed\r\n");
			break;
		}
	}

	err = ftp_recv_ctrl_msg(pFC, 100);

	if(err != FTP_ERR_NONE)
		return err;

	// check control message
	if(pFC->rx_buff_ctrl) {

	}


	ftp_destroy_channel(pFC);

	return err;
}

int ftp_remote_mkd_recursive(FtpClient *pFC, const char* remote_path) {
	int err;

	return err;
}

int ftp_remote_send_recv_ctrl(FtpClient *pFC, const char *msg) {
	int err;

	return err;
}


int ftp_remote_send_data(FtpClient *pFC, const uint8_t *data, int len) {
	int err;

	return err;
}


int	ftp_destroy_channel(FtpClient *pFC) {
	int err;

	return err;
}

int ftp_send_ctrl_msg(FtpClient *pFC, const char *msg) {

}


int ftp_recv_ctrl_msg(FtpClient *pFC, int timeout) {

}





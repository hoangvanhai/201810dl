/*
 * ftp_client.h
 *
 *  Created on: Dec 19, 2018
 *      Author: PC
 */

#ifndef APPLICATION_FTP_CLIENT_H_
#define APPLICATION_FTP_CLIENT_H_

#include <socket_common.h>
#include <lib_str.h>

#define NUM_FTP_SERVER		2
#define SIZE_TX_BUFF_CTRL	100
#define SIZE_RX_BUFF_CTRL	100
#define SIZE_TX_BUFF_DATA	100

enum FtpCode {
	FTP_ERR_NONE = 0,
	FTP_ERR_FILE,
	FTP_ERR_PUT,
	FTP_ERR_GET,
	FTP_ERR_CD,
	FTP_ERR_NCD,
	FTP_ERR_MKD,
	FTP_ERR_PWD,
	FTP_ERR_OPEN_CTRL_CONN,
	FTP_ERR_OPEN_DATA_CONN,
	FTP_ERR_RECV_CTRL_MSG,
	FTP_ERR_RECV_DATA_MSG,
	FTP_ERR_SEND_CTRL_MSG,
	FTP_ERR_SEND_DATA_MSG,
	FTP_ERR_AUTHEN,
	FTP_ERR_PASV,
	FTP_ERR_CODE
};

typedef struct ServerInfo_ {
	ip_addr_t	ip;
	uint16_t	port;
	bool		enable;
	CPU_CHAR	*username;
	CPU_CHAR	*passwd;
}ServerInfo;

typedef struct FtpClient_ {
	int 				fd_ctrl;
	int 				fd_data;
	int 				tx_buf_size;
	int 				rx_buf_size;
	int 				port_data;
	uint8_t				*rx_buff_ctrl;
	uint8_t				*tx_buff_ctrl;
	uint8_t				*tx_buff_data;
	Network_Status		status_ctrl;
	Network_Status		status_data;
	uint8_t				curr_sv_idx;
	ServerInfo			server_list[NUM_FTP_SERVER];

}FtpClient;


int 	ftp_client_init_handle(	FtpClient *pFC,  uint8_t *tx_ctrl_buff,
								uint8_t *rx_ctrl_buff, uint8_t *tx_data_buff);

int 	ftp_client_add_server(FtpClient *pFC, ServerInfo *server, uint8_t index);
uint8_t ftp_get_curr_server(FtpClient *pFC);
int 	ftp_setup_channel(FtpClient *pFC, uint8_t idx);
int		ftp_start_data_channel(FtpClient *pFC);
int 	ftp_start_ctrl_sock(FtpClient *pFC);
int		ftp_close_ctrl_sock(FtpClient *pFC);
int 	ftp_start_data_sock(FtpClient *pFC, int port);
int		ftp_close_data_sock(FtpClient *pFC);
int 	ftp_get_pasv_port(FtpClient *pFC);
int 	ftp_remote_cwd(FtpClient *pFC, const char *path);
int 	ftp_remote_mkd(FtpClient *pFC, const char *path);
int 	ftp_remote_put(FtpClient *pFC, uint8_t index, const char* filename,
						const char *local_path, const char *remote_path);
int 	ftp_remote_mkd_recursive(FtpClient *pFC, const char* remote_path);
int 	ftp_remote_send_recv_ctrl(FtpClient *pFC, const char *msg);
int 	ftp_send_ctrl_msg(FtpClient *pFC, const char *msg);
int 	ftp_recv_ctrl_msg(FtpClient *pFC, int timeout);
int 	ftp_remote_send_data(FtpClient *pFC, const uint8_t *data, int len);

int		ftp_destroy_channel(FtpClient *pFC);
int		ftp_get_code(FtpClient *pFC);


#endif /* APPLICATION_FTP_CLIENT_H_ */

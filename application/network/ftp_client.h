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
#include <network_cfg.h>
#include <common/ring_file.h>

enum FtpCode {
	FTP_ERR_NONE = 0,
	FTP_ERR_FILE,			//1
	FTP_ERR_PUT,			//2
	FTP_ERR_GET,			//3
	FTP_ERR_CD,				//4
	FTP_ERR_NCD,			//5
	FTP_ERR_MKD,			//6
	FTP_ERR_PWD,			//7
	FTP_ERR_OPEN_CTRL_CONN,	//8
	FTP_ERR_OPEN_DATA_CONN, //9
	FTP_ERR_RECV_CTRL_MSG,	//10
	FTP_ERR_RECV_DATA_MSG,	//11
	FTP_ERR_SEND_CTRL_MSG,	//12
	FTP_ERR_SEND_DATA_MSG,	//13
	FTP_ERR_AUTHEN,			//14
	FTP_ERR_PASV,			//15
	FTP_ERR_CODE,			//16
	// manhbt added here
	FTP_ERR_MEM,
	FTP_ERR_INVALID_PARAM,
	FTP_ERR_TIMEOUT,
	FTP_ERR_UNKNOWN,
};

typedef struct ServerInfo_ {
	ip_addr_t	ip;
	uint16_t	port;
	bool		enable;
	CPU_CHAR	*username;
	CPU_CHAR	*passwd;
	CPU_CHAR	*prefix;
}ServerInfo;


typedef struct FtpMsg_ {
	uint8_t local_path[128];
	uint8_t file_name[128];
}FtpMsg;

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
	ServerInfo			server_list[FTP_CLIENT_SERVER_NUM];
	sys_thread_t		send_thread;
	bool				active;

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
int 	ftp_remote_put(FtpClient *pFC, uint8_t index, char* filename,
						char *local_path, char *remote_path);
int 	ftp_remote_mkd_recursive(FtpClient *pFC, char* remote_path);
int 	ftp_remote_send_recv_ctrl(FtpClient *pFC, const char *msg);
int 	ftp_send_ctrl_msg(FtpClient *pFC, const char *msg);
int 	ftp_recv_ctrl_msg(FtpClient *pFC, int timeout);
int 	ftp_remote_send_data(FtpClient *pFC, const uint8_t *data, int len);

int		ftp_destroy_channel(FtpClient *pFC);
int		ftp_get_code(FtpClient *pFC);

void 	ftp_client_sender(void *arg);
int		ftp_add_filename(FtpClient *pFC, const uint8_t * local_path, const uint8_t* file_name);
void 	ftp_print_err(int err);


/**
 *	Send a File from SD card to all server instances
 * @param pFC
 * @param file_name
 * @param local_path
 * @param remote_path
 * @return
 */
int 	ftp_client_process_send_file(FtpClient *pFC, char *file_name, char *local_path, char *remote_path);

/**
 * Check the retry table and resend file if need
 * @param pFC
 */
void 	ftp_client_process_resend(FtpClient *pFC);

#endif /* APPLICATION_FTP_CLIENT_H_ */

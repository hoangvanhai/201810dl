/*
 * modem_ftp_client.h
 *
 *  Created on: Oct 26, 2018
 *      Author: buiti
 */

#ifndef SIMCOM_SIMCOM_FTP_CLIENT_H_
#define SIMCOM_SIMCOM_FTP_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "modem_ftp_command.h"
#include "modem.h"
#include "ring_buffer.h"
#include "linked_list.h"
#include "ftp_client.h"

#define FTP_WAIT_FOREVER	0xFFFFFFFF

//typedef enum
//{
//	FTP_ERR_OK = 0,
//	FTP_ERR_NOT_IMPLEMENT,		//Function is not implemented!
//	FTP_ERR_UNKNOWN = 201,		//201 Unknown error for FTP
//	FTP_ERR_TASK_BUSY ,			//202 FTP task is busy
//	FTP_ERR_SERVER_ADDR , 		//203 Failed to resolve server address
//	FTP_ERR_FTP_TIMEOUT, 		//204 FTP timeout
//	FTP_ERR_FILE_READ,			//205 Failed to read file
//	FTP_ERR_FILE_WRITE, 		//206 Failed to write file
//	FTP_ERR_NOT_ALLOWED,		//207 Not allowed in current state
//	FTP_ERR_LOGIN_FAILED,		//208 Failed to login
//	FTP_ERR_LOGOUT_FAILED,		//209 Failed to logout
//	FTP_ERR_TRANSFER_FAILED,	//210 Failed to transfer data
//	FTP_ERR_COMMAND_REJECTED,	//211 FTP command rejected by server
//	FTP_ERR_MEMORY_ERROR,		//212 Memory error
//	FTP_ERR_INVALID_PARAMS,		//213 Invalid parameter
//	FTP_ERR_NETWORK_ERROR,		//214 Network error
//	FTP_ERR_SOCKET_FAILED,		//215 Failed to connect socket
//	FTP_ERR_SOCKET_SEND_FAILED,	//216 Failed to send data using socket
//	FTP_ERR_SOCKET_RECV_FAILED,	//217 Failed to receive data using socket
//	FTP_ERR_AUTH_FAILED,		//218 Failed to verify user name and password
//	FTP_ERR_SOCKET_TIMEOUT,		//219 Socket connect timeout
//
//} modem_ftp_err_code_t;

typedef enum FtpCode modem_ftp_err_code_t;
//typedef enum
//{
//	FTP_STATE_IDLE	= 0,
//	FTP_STATE_WAIT_CMD_CMPLT,		//State: Wait for FTP Command completed
//	FTP_STATE_CMD_CMPLT,			//State: FTP Command Completed
//	FTP_STATE_WAIT_DATA_CMPLT,		//State: Wait for Data Completed
//	FTP_STATE_DATA_CMPLT,			//State: Data completed with error code
//
//
//} modem_ftp_process_state_t;

//typedef struct
//{
//	int								iHandle;
//	char 							serveraddress[256];
//	int 							port;
//	char 							username[32];
//	char 							password[32];
//	modem_ftp_process_state_t		state;			// FTP Task state
//	uint8_t							busy;			// FTP task is busy
//	uint8_t 						ssl_enable;		// FTPS support
//
//} modem_ftp_client_handle_t;

typedef struct {

	char 		name[128];			// File name
	uint32_t	size;				// File size
	char		owner[64];			// File Owner (ex: manhbt:manhbt)
	uint32_t 	permission;			// File permission (ex: 777)
	uint32_t	date;				// Modified date time, formatted in yyMMddhhmm (ex: 1810301759 <--> 17h59 30-10-2018)
} modem_ftp_file_attr_t;



//extern modem_ftp_client_handle_t g_modem_ftp_client;

/**
 * Initialize modem for FTP Client
 * @param server  server IP
 * @param port server port
 * @param username username for FTP server login
 * @param password passowrd for FTP server login
 * @return
 */
//modem_ftp_err_code_t modem_ftp_init(char* server, int port,  char* username, char* password);

modem_ftp_err_code_t modem_ftp_init(FtpClient *pFC);

/**
 * Check modem is initialized or not
 * @return TRUE if initialized, FALSE if not
 */
BOOL 				modem_ftp_is_initialized(FtpClient *pFC);
/**
 * Connect to FTP server using AT Commands
 * @param pHandle modem ftp client handle
 * @param server [IN] FTP server address
 * @param port [IN] FTP port
 * @param username [IN] User name
 * @param password [IN] password
 * @return FTP Error code
 */
//modem_ftp_err_code_t modem_ftp_connect();
modem_ftp_err_code_t modem_ftp_connect(FtpClient *pFC, uint8_t channel);

/**
 * Get status of FTP sevice
 * @param pHandle
 * @return
 */
modem_ftp_err_code_t modem_ftp_get_status(FtpClient *pFC);


/**
 * Change current directory, try to make new if not exist
 * @param dir:  working dir
 * @return
 */
modem_ftp_err_code_t modem_ftp_cwd(FtpClient *pFC, const char *dir);

/**
 * Change Working Directory on FTP Server
 * @param pHandle
 * @param working_dir working directory on FTP Server
 * @return
 */
modem_ftp_err_code_t modem_ftp_change_working_dir(FtpClient *pFC, char* working_dir);

/**
 * Make directory on FTP server
 * @param pHandle
 * @param dir
 * @return
 */
modem_ftp_err_code_t modem_ftp_make_dir(FtpClient *pFC, char* dir);

/**
 * AT+QFTPRMDIR Delete a Folder on FTP Server
 * @param pHandle
 * @param dir
 * @return
 */
modem_ftp_err_code_t modem_ftp_remove_dir(FtpClient *pFC, char* dir);

/**
 * AT+QFTPRENAME Rename a File or Folder on FTP Server
 * @param pHandle
 * @param old_name
 * @param new_name
 * @return
 */
modem_ftp_err_code_t modem_ftp_rename(FtpClient *pFC, char* old_name, char* new_name);

/**
 * Disconnect from FTP server
 * @param pHandle modem ftp client handle
 * @return FTP Error code
 */
modem_ftp_err_code_t modem_ftp_disconnect(FtpClient *pFC);

/**
 * Get list from FTP server directory
 * @param pHandle modem ftp client handle
 * @param path [IN] directory on FTP server
 * @param filelist [OUT] list files on FTP server directory
 * @param timeout [IN] timeout (milliseconds)
 * @return FTP Error code
 */
modem_ftp_err_code_t modem_ftp_get_list_files(FtpClient *pFC, char* path, char* filelist, uint32_t timeout);

/**
 * Get list from FTP server directory
 * @param pHandle FTP client handle
 * @param path directory
 * @param psFileList pointer to Linked list stored file list
 * @param timeout timeout
 * @return
 */
modem_ftp_err_code_t modem_ftp_get_list(FtpClient *pFC, char* path, SList* psFileList, uint32_t timeout);

/**
 * Get file content from FTP server
 * @param pHandle modem ftp client handle
 * @param filename [IN] file name on FTP server
 * @param content [OUT] file content
 * @param len [OUT]	file content length
 * @param timeout [IN] timeout (milliseconds)
 * @return FTP Error code
 */
modem_ftp_err_code_t modem_ftp_get_file(FtpClient *pFC, char* filename, uint8_t* content, uint32_t* len, uint32_t timeout);

/**
 * Put a file to FTP server
 * @param pHandle modem ftp client handle
 * @param filename [IN] file name
 * @param content [IN] file content
 * @param len [IN] Content length
 * @param timeout [IN] timeout (milliseconds)
 * @return FTP Error code
 */
modem_ftp_err_code_t modem_ftp_put_file(FtpClient *pFC, char* filename, uint8_t* content, uint32_t len, uint32_t timeout);

/**
 *
 * @param pFC
 * @param index
 * @param file_name
 * @param local_path
 * @param remote_path
 * @return
 */
modem_ftp_err_code_t modem_ftp_put_file_from_local(FtpClient *pFC, uint8_t index,
								char *file_name,
								char *local_path,
								char *remote_path);

/**
 * Make dirirectory recursively
 * @param pFC
 * @param dir:
 * @return
 */
modem_ftp_err_code_t modem_ftp_make_dir_recursive(FtpClient *pFC, const char* dir);

#ifdef __cplusplus
}
#endif
#endif /* SIMCOM_SIMCOM_FTP_CLIENT_H_ */

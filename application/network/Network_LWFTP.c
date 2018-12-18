/**
 * @file Network_LWIP.c
 * @author ThinhNT (tienthinh@gmail.com)
 * @brief Module Network FTP Implementation via Ethernet using LWIP library
 * the Network Module use Laurent GONZALEZ <lwip@gezedo.com> simple FTP client
 * from https://github.com/gezedo/lwftp
 * @version 0.1
 * @date 2018-10-17
 *
 * @copyright Copyright (c) 2018
 *
 */

#include <string.h>
#include "Network_LWFTP.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include "ff.h"
#include "modem_debug.h"

/** Enable debugging for LWFTP */
#ifndef LWFTP_DEBUG
#define LWFTP_DEBUG   LWIP_DBG_ON
#endif

#define LWFTP_TRACE   (LWFTP_DEBUG|LWIP_DBG_TRACE)
#define LWFTP_STATE   (LWFTP_DEBUG|LWIP_DBG_STATE)
#define LWFTP_WARNING (LWFTP_DEBUG|LWIP_DBG_LEVEL_WARNING)
#define LWFTP_SERIOUS (LWFTP_DEBUG|LWIP_DBG_LEVEL_SERIOUS)
#define LWFTP_SEVERE  (LWFTP_DEBUG|LWIP_DBG_LEVEL_SEVERE)

#define PTRNLEN(s)  s,(sizeof(s)-1)
//static char* lwftp_curdir;
static bool lwftp_connected = false;
ip_addr_t lwftp_ip;
static char* lwftp_user;
static char* lwftp_passwd;
static int lwftp_port;
struct timeval tv;
static lwftp_state_t lwftp_state = LWFTP_CLOSED;
static int socket_ctrl = NULL, socket_dat = NULL;
static struct sockaddr_in server_ctrl, server_dat;
//char* 	request_msg,reply_msg;
//static char 	tmpStr[LWFTP_CONST_BUF_SIZE+8];
static char request_msg[LWFTP_CONST_BUF_SIZE + 8],
		reply_msg[LWFTP_CONST_BUF_SIZE + 8], data_buf[LWFTP_CONST_BUF_SIZE + 8];
static int lwftp_bufsize = LWFTP_CONST_BUF_SIZE;

// Define static functions
static lwftp_result_t lwftp_reconnect();
static lwftp_result_t lwftp_connect();

/**
 * Reconnect in case not connected
 * @return LWFTP_RESULT_OK if connected
 */
static lwftp_result_t lwftp_reconnect() {
	if (!Network_LWIP_Is_Up()) {
		lwftp_connected = false;
		lwftp_state = LWFTP_CLOSED;
		NET_DEBUG_FTP("lwftp_reconnect Failed because ethernet down \r\n");
		return LWFTP_RESULT_ERR_CONNECT;
	}
	lwftp_result_t ret = LWFTP_RESULT_OK;
	int iTries = 0;
	while (iTries++ < 10) {
		// Step 1: Connect to the server if not connected
		if (lwftp_connected != true) {
			if (lwftp_ip.addr != 0) {
				ret = lwftp_connect();
			}
			if (ret != LWFTP_RESULT_OK) {
				OSA_TimeDelay(1000); // Delay 1s for ... fucking boy
				NET_DEBUG_FTP("LWFTP retry connect %d times ...\r\n", iTries);
				continue; // retries
			}
		} else {
			break;
		}
	}
	return ret;
}

/**
 * Receive socket data from socket and to buffer
 * @note use this for socket receive several times continuously
 * @param sock socket to receive
 * @param buf buffer to receive
 * @param bufsize buffer size to receive
 * @return number of received bytes
 */
static int lwftp_receive_ctrl_data(int sock, char* buf, int bufsize) {
	int resp_len = 0, total_len = 0;
	int max_receive_byte = bufsize;
	do {
		memset(reply_msg, 0x00, sizeof(reply_msg));
		max_receive_byte -= resp_len;
		resp_len = recv(sock, reply_msg, max_receive_byte, 0);
		total_len += resp_len;
		if (resp_len > 0)
		{
			NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len, reply_msg);
			break;
		}

	} while (resp_len > 0 && resp_len < lwftp_bufsize);
	return total_len;
}

static bool lwftp_is_busy() {
	static bool lwftp_busy = false;
	return lwftp_busy;
}
/**
 * Connect using already setup ip address of server + user/pass
 * @return result of connection
 */
static lwftp_result_t lwftp_connect() {
	int ret;
	size_t len = 0;
	size_t sent_len = 0;
	if (socket_ctrl != NULL) {
		close(socket_ctrl);
	}
	socket_ctrl = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_ctrl == -1) {
		perror("Could not create socket");
		lwftp_connected = false;
		return LWFTP_RESULT_ERR_CONNECT;
	} else {
		NET_DEBUG_FTP("Socket id: %d created successfully\r\n");
	}
	tv.tv_sec = 3000; //dkm co gi do sai sai o day khi dung lwip 3000s ma nhu la 3s
	tv.tv_usec = 10000;
	// set timeout of socket
	setsockopt(socket_ctrl, SOL_SOCKET, SO_RCVTIMEO, (const char* )&tv,
			sizeof tv);
	server_ctrl.sin_addr.s_addr = lwftp_ip.addr;// inet_addr(lwftp_ip);
	server_ctrl.sin_family = AF_INET;
	server_ctrl.sin_port = htons(lwftp_port);

	// Connect to server
	ret = connect(socket_ctrl, (struct sockaddr * )&server_ctrl,
			sizeof(server_ctrl));
	if (ret < 0) {
		NET_DEBUG_FTP("FTPCLIENT Connection failed");
		lwftp_connected = false;
		return LWFTP_RESULT_ERR_CONNECT;
	} else {
		NET_DEBUG_FTP("FTPCLIENT Connected\r\n");
		ret = LWFTP_RESULT_OK;
		lwftp_connected = true;
	}

	// 220 Welcome to blah FTP service.
	ret = lwftp_receive_ctrl_data(socket_ctrl, reply_msg, lwftp_bufsize);
	if(Str_Str_N(reply_msg, FTP_SER_RES_220, LWFTP_CONST_BUF_SIZE) != NULL) {
		NET_DEBUG("Welcome OK!");
	} else {
		NET_DEBUG_ERROR("Welcome ERR!, closing socket");
		close(socket_ctrl);
		lwftp_connected = false;
		return LWFTP_RESULT_ERR_CONNECT;
	}

	// Enter logging information USERNAME
	Str_Copy_N(request_msg, "USER ", LWFTP_CONST_BUF_SIZE);
	Str_Cat_N(request_msg, lwftp_user, LWFTP_CONST_BUF_SIZE);
	Str_Cat_N(request_msg, "\r\n", LWFTP_CONST_BUF_SIZE);
	len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
	NET_DEBUG_FTP("Enter Loggin INformation(len = %d) : %s\r\n", len, request_msg);
	sent_len = send(socket_ctrl, request_msg, len, 0);
	NET_DEBUG_FTP("Sent bytes = %d", sent_len);
	ASSERT(sent_len == len);

	//memset(reply_msg,0x00,lwftp_bufsize);
	//	331 Please specify the password.
	ret = lwftp_receive_ctrl_data(socket_ctrl, reply_msg, lwftp_bufsize);
	ASSERT(ret > 0);

	if(Str_Str_N(reply_msg, FTP_SER_RES_331, LWFTP_CONST_BUF_SIZE) != NULL) {
		NET_DEBUG("User name okay, need password !");
	} else {
		NET_DEBUG_ERROR("User name ERR!, closing socket");
		close(socket_ctrl);
		lwftp_connected = false;
		return LWFTP_RESULT_ERR_CONNECT;
	}

	// Enter logging information PASSWORD
	memset(request_msg, 0x00, sizeof(request_msg));
	Str_Copy_N(request_msg, "PASS ", LWFTP_CONST_BUF_SIZE);
	Str_Cat_N(request_msg, lwftp_passwd, LWFTP_CONST_BUF_SIZE);
	Str_Cat_N(request_msg, "\r\n", LWFTP_CONST_BUF_SIZE);
	len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
	NET_DEBUG_FTP("Enter Password (len = %d), send = %s\r\n", len, request_msg);
	sent_len = send(socket_ctrl, request_msg, len, 0);
	NET_DEBUG_FTP("Sent bytes = %d", sent_len);
	ASSERT(sent_len == len);
	//memset(reply_msg,0x00,sizeof(reply_msg));
	//	230 Login successful
	ret = lwftp_receive_ctrl_data(socket_ctrl, reply_msg, lwftp_bufsize);
	ASSERT(ret >= 0);

	if(Str_Str_N(reply_msg, FTP_SER_RES_230, LWFTP_CONST_BUF_SIZE) != NULL) {
		NET_DEBUG("User logged in, proceed. Logged out if appropriate.");
	} else {
		NET_DEBUG_ERROR("User login FAILED");
		close(socket_ctrl);
		lwftp_connected = false;
		return LWFTP_RESULT_ERR_CONNECT;
	}

	lwftp_state = LWFTP_CONNECTED;
	return LWFTP_RESULT_OK;
}

lwftp_result_t Network_LWFTP_Start(ip_addr_t ip, int port,
		const char* usrname, const char* passwd) {
	int ret;
	// TODO: check condition before openning new connection
	lwftp_ip = ip;
	lwftp_port = port;
	lwftp_passwd = passwd;
	lwftp_user = usrname;
	// manhbt:
//	return LWFTP_RESULT_ERR_CONNECT;
	return LWFTP_RESULT_OK;
	// TODO: check if have to connect or not
//	ret = lwftp_connect();
	if (ret != 0) {
		return LWFTP_RESULT_ERR_CONNECT;
	} else {
		return LWFTP_RESULT_OK;
	}
}

char path[256];



lwftp_result_t Network_LWFTP_SendFile(const char *dirPath, const char *fileName) {
#if 0
	if (!Network_LWIP_Is_Up()) {
		NET_DEBUG_FTP(
				"Network_LWFTP_SendFile Failed because ethernet down or DHCP is not Bound \r\n");
		return LWFTP_RESULT_ERR_CONNECT;
	}

	lwftp_result_t ret = LWFTP_RESULT_ERR_UNKNOWN;
	lwftp_result_t result;
	memset(path, 0x00, sizeof(path));
	int iTries = 0;
	unsigned int a, b, c, d, e, f;
	int resp_len = 0, response;
	int max_receive_byte = lwftp_bufsize;
	//(void)(a,b,c,d,e,f);
	char *ptr;
	int resp_code;
	unsigned long data_port; // port for data
	FIL fil; // file object
	UINT btr; // number of file to read
	UINT br; // number of read bytes
	FRESULT fr; // FatFS return code
	size_t len; // written length
	size_t sentLen; // written length
	NET_DEBUG_FTP("\r\n=====Network_LWFTP_SendFile: %s, %s=======\r\n", dirPath,
			fileName);
	// Connect and try at least 10 times
	// TODO: Step 1: Check the connection, if not connect connect and try
	while (iTries++ < 10) {
		// Step 1: Connect to the server if not connected
		result = lwftp_reconnect();
		if (result != LWFTP_RESULT_OK) {
			OSA_TimeDelay(1000);
			continue;
		}
		// Step 2: Change to directory, check if it exists?
		result = Network_LWFTP_CWD(dirPath);
		if (result == LWFTP_RESULT_OK) {
			NET_DEBUG_FTP("Change to directory %s\r\n", dirPath);
		} else {
			// directory not exist create directory
			result = Network_LWFTP_MKD(dirPath);
			if (result == LWFTP_RESULT_OK) {
				result = Network_LWFTP_CWD(dirPath);
			}
			if (result != LWFTP_RESULT_OK) {
				lwftp_connected = false;
				continue;
			}

		}

		// Step 3: Connected, try to send file to server
		// Getting the PASV port
		memset(request_msg, 0x00, sizeof(request_msg));
		strcpy(request_msg, "PASV\r\n");
		len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
		NET_FTP_DEBUG("Send to FTPServer %s len = %d", request_msg, len);
		OSA_TimeDelay(10);
		len = send(socket_ctrl, request_msg, len, 0);
		memset(reply_msg, 0x00, sizeof(reply_msg));
		recv(socket_ctrl, reply_msg, LWFTP_CONST_BUF_SIZE, 0);
		resp_code = strtoul(reply_msg, NULL, 10);
		NET_DEBUG_FTP("reply_msg:  %s resp_code = %d\r\n", reply_msg,
				resp_code);
		// Find pasv port
		ptr = strchr(reply_msg, '(');
		do {
			a = strtoul(ptr + 1, &ptr, 10);
			b = strtoul(ptr + 1, &ptr, 10);
			c = strtoul(ptr + 1, &ptr, 10);
			d = strtoul(ptr + 1, &ptr, 10);
			e = strtoul(ptr + 1, &ptr, 10);
			f = strtoul(ptr + 1, &ptr, 10);
		} while (0);
		data_port = e * 256 + f;
		NET_DEBUG_FTP("pasv port =%d\r\n", data_port);
		lwftp_state = LWFTP_PASV_SENT;
		// connect to data port connection
		socket_dat = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(socket_dat, SOL_SOCKET, SO_RCVTIMEO, (const char* )&tv,
				sizeof tv);
		if (socket_dat == -1) {
			perror("Could not create socket");
			return LWFTP_RESULT_ERR_INTERNAL;
		} else {
			NET_DEBUG_FTP("Socket_dat: %d created successfully\r\n",
					socket_dat);
		}
		server_dat.sin_addr.s_addr = inet_addr(lwftp_ip);
		server_dat.sin_family = AF_INET;
		server_dat.sin_port = htons(data_port);
		// Connect to server for data
		if (connect(socket_dat, (struct sockaddr * )&server_dat,
				sizeof(server_dat)) < 0) {
			perror("Connection data port failed");
			return LWFTP_RESULT_ERR_CONNECT;
		} else {
			NET_DEBUG_FTP("Connected data port\r\n");
		}
		// Store a file read from sdcard "STOR ./<fileName>\r\n"
		memset(request_msg, 0x00, sizeof(request_msg));
		strcpy(request_msg, "STOR ");
		strcat(request_msg, "./"); // concatenate with current directory path
		//strcat(request_msg, "");
		strcat(request_msg, fileName);
		strcat(request_msg, "\r\n");
		//	char cEOF = 0xFF;
		NET_DEBUG_FTP("Send to socket_ctrl: %s\r\n", request_msg);
		len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
		send(socket_ctrl, request_msg, len, 0);
		// Write data to file in server
#ifdef ENABLE_FTP_FILE_TEST
		fr = f_open(&fil, ENABLE_FTP_FILE_TEST, FA_READ);
#else
		char *abs_path = OSA_FixedMemMalloc(256);
		if (!abs_path) {
			NET_DEBUG_ERROR("Unable to allocate memory for abs_path");
			return LWFTP_RESULT_ERR_MEMORY;
		}
		memset(abs_path, 0, 256);
		sprintf(abs_path, "%s/%s", dirPath, fileName);
		NET_DEBUG("opening file %s...", abs_path);
		fr = f_open(&fil, abs_path, FA_READ);
#endif
		btr = LWFTP_CONST_LINE_SIZE;
		if (fr == FR_OK) {
			do {
				// read from file file to buffer reply_msg
				fr = f_read(&fil, data_buf, btr, &br);
				if (fr != FR_OK) {
					NET_DEBUG_FTP("LWFTP read file failed\r\n");
					return LWFTP_RESULT_ERR_LOCAL;
				}
				if (br > 0) {
					NET_DEBUG_FTP("Write to socket data %d bytes\r\n", br);
					len = send(socket_dat, data_buf, br, 0);
					ASSERT(len == br);
				}
			} while (br == btr); // while not reached the EOF
		} else {
			NET_DEBUG_FTP("LWFTP open file failed\r\n");
			return LWFTP_RESULT_ERR_LOCAL;
		}
		// close the file
		f_close(&fil);
		lwftp_state = LWFTP_CLOSING;
		// close data socket
		close(socket_dat);
		// receive message from ctrl socket
		do {
			max_receive_byte -= resp_len;
			resp_len = recv(socket_ctrl, reply_msg, max_receive_byte, 0);
			if (resp_len > 0)
				NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len,
						reply_msg);
			if (reply_msg > 0) {
				response = strtoul(reply_msg, NULL, 10);
				if (response > 0) {
					if (response == 150) {
						ret = LWFTP_RESULT_OK;
						NET_DEBUG_FTP("Send file to FTP Successfully\r\n");
					}
				}
			}
			// process message and check for response
		} while (resp_len > 0 && resp_len < lwftp_bufsize);
		//lwftp_receive_ctrl_data(socket_ctrl, reply_msg, lwftp_bufsize);

		NET_DEBUG_FTP("Goodbye\r\n");
		lwftp_state = LWFTP_CLOSED;
		Network_LWFTP_Disconnect();
		return ret;
	}
#endif
	return LWFTP_RESULT_ERR_UNKNOWN;
}
lwftp_result_t Network_LWFTP_Delete(const char *path) {
	if (!Network_LWIP_Is_Up()) {
		NET_DEBUG_FTP(
				"Network_LWFTP_Delete Failed because ethernet down or dhcp is not bound \r\n");
		return LWFTP_RESULT_ERR_CONNECT;
	}
	NET_DEBUG_FTP("==========Network_LWFTP_Delete: %s\r\n=========", path);
	lwftp_result_t result = LWFTP_RESULT_ERR_FILENAME;
	int resp_len = 0, response;
	int len = 0, sent_len = 0;
	int max_receive_byte = lwftp_bufsize;
	char *next_ptr;
	//int iTries = 0;
	//lwftp_result_t ret;
	// Check for connection and reconnect in case
	lwftp_reconnect();

	if (lwftp_connected == true) {
		// Store a file read from sdcard
		memset(request_msg, 0x00, sizeof(request_msg));
		Str_Copy_N(request_msg, "DELE ", LWFTP_CONST_BUF_SIZE);
		Str_Cat_N(request_msg, path, LWFTP_CONST_BUF_SIZE);
		Str_Cat_N(request_msg, "\r\n", LWFTP_CONST_BUF_SIZE);
		NET_DEBUG_FTP("Delete file cmd: %s\r\n", request_msg);
		len = Str_Len_N(request_msg, lwftp_bufsize);
		sent_len = send(socket_ctrl, request_msg, len, 0);
		ASSERT(sent_len == len);
		result = LWFTP_RESULT_ERR_FILENAME;
		do {
			max_receive_byte -= resp_len;
			resp_len = recv(socket_ctrl, reply_msg, max_receive_byte, 0);
			if (resp_len > 0)
				NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len,
						reply_msg);
			if (reply_msg > 0) {
//				response = strtoul(reply_msg, NULL, 10);
				response = Str_ParseNbr_Int32U(reply_msg, &next_ptr, 10);
				if (response > 0) {
					if (response == 250) {
						result = LWFTP_RESULT_OK;
					}
				}
			}
			// process message and check for response
		} while (resp_len > 0 && resp_len < lwftp_bufsize);
	}
	return result;
}
lwftp_result_t Network_LWFTP_Disconnect() {
	close(socket_ctrl);
	socket_ctrl = NULL;
	lwftp_connected = false;
	return LWFTP_RESULT_OK;
}

ip_addr_t Netwrok_LWIP_Get_ServerIP() {
	return lwftp_ip;
}
lwftp_state_t Network_LWIP_Get_State() {
	return lwftp_state;
}

/**
 * Create directory in the server
 * @param dirpath directory path
 */
//char full_path[256];
lwftp_result_t Network_LWFTP_MKD(const char* dirpath) {
	NET_DEBUG_FTP(
			"===================Network_LWFTP_MKD: %s==================\r\n",
			dirpath);
	if (!Network_LWIP_Is_Up()) {
		NET_DEBUG_FTP("Network_LWFTP_MKD Failed because ethernet down \r\n");
		return LWFTP_RESULT_ERR_CONNECT;
	}
	lwftp_result_t result = LWFTP_RESULT_ERR_FILENAME;

	int resp_len = 0, response;
	int sent_len = 0, len = 0;
	int max_receive_byte = lwftp_bufsize;
	char** tokens;
	char *next_ptr;
	bool isMKDRecursiveOK = false;
	bool isNotOK = false; // in case something wrong in between, to free all tokens buffer alloc dinamically
	int i;
	// Check for connection and reconnect in case
	lwftp_reconnect();
	if (lwftp_connected == true) {
		// Try to make dir with full path first
		memset(request_msg, 0x00, sizeof(request_msg));
		Str_Copy_N(request_msg, "MKD ", LWFTP_CONST_BUF_SIZE);
		Str_Cat_N(request_msg, dirpath, LWFTP_CONST_BUF_SIZE);
		strcat(request_msg, "\r\n");
		NET_DEBUG_FTP("Send mkdir cmd: %s\r\n", request_msg);
		len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
		sent_len = send(socket_ctrl, request_msg, len, 0);
		ASSERT(sent_len == len);
		result = LWFTP_RESULT_ERR_FILENAME;
		do {
			max_receive_byte -= resp_len;
			resp_len = recv(socket_ctrl, reply_msg, max_receive_byte, 0);
//			if (resp_len > 0)
//				NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len,
//						reply_msg);
			if (resp_len > 0) {
				NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len, reply_msg);
//				response = strtoul(reply_msg, NULL, 10);
//				if (response > 0) {
//					if (response == 257) {
//						result = LWFTP_RESULT_OK;
//						break;
//					}
//				}
				if(Str_Str_N(reply_msg, "257", LWFTP_CONST_BUF_SIZE) != NULL)
				{
					result = LWFTP_RESULT_OK;
				}
			}
			// process message and check for response
		} while (resp_len > 0 && resp_len < lwftp_bufsize);
		if (result == LWFTP_RESULT_OK) {
			result == Network_LWFTP_CWD(dirpath);
			if (result == LWFTP_RESULT_OK)
				return result;
		}
	}
	if (dirpath[0] == '/') {
		Network_LWFTP_CWD("/");
		tokens = str_split(&dirpath[1], '/');
	} else {
		tokens = str_split(dirpath, '/');
	}
	if (lwftp_connected == true) {
		if (tokens) {
			for (i = 0; *(tokens + i); i++) {
				if (isNotOK == true) {

					//free(*(tokens + i));
					continue;
				}
				NET_DEBUG_FTP("DIR=[%s]\r\n", *(tokens + i));
				// Change to directory and check if exists or not
				result = Network_LWFTP_CWD(*(tokens + i));
				if (result == LWFTP_RESULT_OK) {
					//free(*(tokens + i));
					continue;
				}
				// If not exists try to create new directory
				memset(request_msg, 0x00, sizeof(request_msg));
				Str_Copy_N(request_msg, "MKD ", LWFTP_CONST_BUF_SIZE);
				Str_Cat_N(request_msg, *(tokens + i), LWFTP_CONST_BUF_SIZE);
				Str_Cat_N(request_msg, "\r\n", LWFTP_CONST_BUF_SIZE);
				NET_DEBUG_FTP("Send mkdir cmd: %s\r\n", request_msg);
				len = Str_Len_N(request_msg, lwftp_bufsize);
				sent_len = send(socket_ctrl, request_msg, len, 0);
				ASSERT(len == sent_len);
				result = LWFTP_RESULT_ERR_FILENAME;
				do {
					max_receive_byte -= resp_len;
					resp_len = recv(socket_ctrl, reply_msg, max_receive_byte,
							0);
//					if (resp_len > 0)
//						NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len,
//								reply_msg);
				if (resp_len > 0) {
						NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len, reply_msg);
//						response = strtoul(reply_msg, NULL, 10);
						response = Str_ParseNbr_Int32U(reply_msg, &next_ptr, 10);
						if (response > 0) {
							if (response == 257) {
								result = LWFTP_RESULT_OK;
							}
						}
					}
					// process message and check for response
				} while (resp_len > 0 && resp_len < lwftp_bufsize);
				result = Network_LWFTP_CWD(*(tokens + i));
				//free(*(tokens + i));
				if (result != LWFTP_RESULT_OK) {
					//free(*(tokens + i));
					isNotOK = true;
				}
			}
			//printf("\n");
			NET_DEBUG_WARNING("OSA_FixedMemFree token (filename) at 0x%x\r\n", tokens);
			OSA_FixedMemFree(tokens);
		}

	}
	return result;
}

/**
 * Change current directory in ftp server to dirpath
 * @param dirpath
 * @return result
 */
lwftp_result_t Network_LWFTP_CWD(const char* dirpath) {
	if (!Network_LWIP_Is_Up()) {
		NET_DEBUG_FTP(
				"Network_LWFTP_Delete Failed because ethernet down or dhcp is not bound \r\n");
		return LWFTP_RESULT_ERR_CONNECT;
	}
	NET_DEBUG_FTP("==========Network_LWFTP_CWD: %s\r\n=========", dirpath);
	lwftp_result_t result = LWFTP_RESULT_ERR_FILENAME;
	int resp_len = 0, response;
	int sent_len = 0, len = 0;
	int max_receive_byte = lwftp_bufsize;
	//int iTries = 0;
	//lwftp_result_t ret;
	// Check for connection and reconnect in case
	lwftp_reconnect();

	if (lwftp_connected == true) {
		// Store a file read from sdcard
		memset(request_msg, 0x00, sizeof(request_msg));
		Str_Copy(request_msg, "CWD ");
		Str_Cat(request_msg, dirpath);
		Str_Cat(request_msg, "\r\n");
		len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
		NET_DEBUG_FTP("Request to CWD (len=%d) %s\r\n", len, request_msg);
		OSA_TimeDelay(10);
		sent_len = send(socket_ctrl, request_msg, len, 0);
		if (len == sent_len){

			result = LWFTP_RESULT_ERR_FILENAME;
			do {
				max_receive_byte -= resp_len;
				resp_len = recv(socket_ctrl, reply_msg, max_receive_byte, 0);
				//			if (resp_len > 0)
				//				NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len,
				//						reply_msg);
				if (resp_len > 0) {
					NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len, reply_msg);
					//				response = strtoul(reply_msg, NULL, 10);
					//				if (response > 0) {
					////					if (response == 250) {
					////						result = LWFTP_RESULT_OK;
					////						break;
					////					} else if (response == 550) {
					////						result = LWFTP_RESULT_ERR_FILENAME;
					////						break;
					////					}
					if(Str_Str_N(reply_msg, "250", LWFTP_CONST_BUF_SIZE) != NULL) {
						result = LWFTP_RESULT_OK;
					} else if (Str_Str_N(reply_msg, "550", LWFTP_CONST_BUF_SIZE) != NULL) {
						result = LWFTP_RESULT_ERR_FILENAME;
					}
					//				}
				}
				// process message and check for response
			} while (resp_len > 0 && resp_len < lwftp_bufsize);
		} else {
			result = LWFTP_RESULT_ERR_SRVR_RESP;
		}
	}
	return result;
}

//MDMFTP_result_t Network_MDMFTP_SendFile(const char *local_path, const char *remote_path, const char *file_name);
lwftp_result_t Network_LWFTP_SendFile2(const char *local_path,
		const char *remote_path, const char *file_name) {
	if (!Network_LWIP_Is_Up()) {
		NET_DEBUG_FTP(
				"Network_LWFTP_SendFile Failed because ethernet down or DHCP is not Bound \r\n");
		return LWFTP_RESULT_ERR_CONNECT;
	}

	lwftp_result_t ret = LWFTP_RESULT_ERR_UNKNOWN;
	lwftp_result_t result;
	memset(path, 0x00, sizeof(path));
	int iTries = 0;
	unsigned int a, b, c, d, e, f;
	int resp_len = 0, response;
	size_t sent_len = 0;
	int max_receive_byte = lwftp_bufsize;
	//(void)(a,b,c,d,e,f);
	char *ptr;
	char *next_ptr;
	int resp_code;
	unsigned long data_port; // port for data
	FIL fil; // file object
	UINT btr; // number of file to read
	UINT br; // number of read bytes
	FRESULT fr; // FatFS return code
	size_t len; // written length
	NET_DEBUG_FTP("\r\n=====Network_LWFTP_SendFile: %s, %s=======\r\n",
			remote_path, file_name);
//	OSTaskStkChk(NULL, &n_free, &n_used, &err);
//	MODEM_DEBUG("n_free = %d, n_used = %d,  err=%d\r\n", n_free, n_used, err);
	// Connect and try at least 10 times
	// TODO: Step 1: Check the connection, if not connect connect and try
	while (iTries++ < 10) {
		// Step 1: Connect to the server if not connected
		result = lwftp_reconnect();
		if (result != LWFTP_RESULT_OK) {
			OSA_TimeDelay(1000);
			continue;
		}
		// Step 2: Change to directory, check if it exists?
		result = Network_LWFTP_CWD(remote_path);
		if (result == LWFTP_RESULT_OK) {
			NET_DEBUG_FTP("Changed to directory %s OK\r\n", remote_path);
		} else {
			// directory not exist create directory
			result = Network_LWFTP_MKD(remote_path);
			if (result == LWFTP_RESULT_OK) {
				result = Network_LWFTP_CWD(remote_path);
			}
			if (result != LWFTP_RESULT_OK) {
				lwftp_connected = false;
				continue;
			}

		}

		// Step 3: Connected, try to send file to server
		// Getting the PASV port
		memset(request_msg, 0x00, sizeof(request_msg));
		Str_Copy(request_msg, "PASV\r\n");
		len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
		NET_DEBUG_FTP("request_msg = %s, len = %d", request_msg, len);
		//OSA_TimeDelay(10);
		sent_len = send(socket_ctrl, request_msg, len, 0);
		memset(reply_msg, 0x00, sizeof(reply_msg));
		resp_len = recv(socket_ctrl, reply_msg, LWFTP_CONST_BUF_SIZE, 0);
		resp_code = Str_ParseNbr_Int32U(reply_msg, &next_ptr, 10);
		//resp_code = strtoul(reply_msg, NULL, 10);
		NET_DEBUG_FTP("THINHNT reply_msg(0x%x)  %s resp_code = %d\r\n", reply_msg, reply_msg,
						resp_code);
		NET_DEBUG_FTP("THINHNT reply_msg(0x%x)  %s resp_code = %d, next_ptr = 0x%x\r\n", reply_msg, reply_msg,
				resp_code, next_ptr);
		NET_DEBUG_FTP(" next_ptr = 0x%x\r\n", (next_ptr));
		// Find pasv port
		ptr = Str_Char_N(reply_msg, resp_len, '(');
		NET_DEBUG_FTP("ptr at 0x%x: %s\r\n", ptr, ptr);
		ASSERT(ptr!=NULL);
//		do {
//			a = strtoul(ptr + 1, &ptr, 10);
//			b = strtoul(ptr + 1, &ptr, 10);
//			c = strtoul(ptr + 1, &ptr, 10);
//			d = strtoul(ptr + 1, &ptr, 10);
//			e = strtoul(ptr + 1, &ptr, 10);
//			f = strtoul(ptr + 1, &ptr, 10);
//		} while (0);

		do {
			a = Str_ParseNbr_Int32U(ptr + 1, &next_ptr, 10); ptr = next_ptr;
			//NET_DEBUG_FTP("a = %d, ptr = 0x%x", a, ptr);
			b = Str_ParseNbr_Int32U(ptr + 1, &next_ptr, 10); ptr = next_ptr;
			//NET_DEBUG_FTP("b = %d, ptr = 0x%x", b, ptr);
			c = Str_ParseNbr_Int32U(ptr + 1, &next_ptr, 10); ptr = next_ptr;
			d = Str_ParseNbr_Int32U(ptr + 1, &next_ptr, 10);  ptr = next_ptr;
			e = Str_ParseNbr_Int32U(ptr + 1, &next_ptr, 10);  ptr = next_ptr;
			f = Str_ParseNbr_Int32U(ptr + 1, &next_ptr, 10);  ptr = next_ptr;
		} while (0);

		data_port = e * 256 + f;
		NET_DEBUG_FTP("pasv port =%d\r\n", data_port);
		lwftp_state = LWFTP_PASV_SENT;
		// connect to data port connection
		socket_dat = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(socket_dat, SOL_SOCKET, SO_RCVTIMEO, (const char* )&tv,
				sizeof tv);
		if (socket_dat == -1) {
			perror("Could not create socket");
			return LWFTP_RESULT_ERR_INTERNAL;
		} else {
			NET_DEBUG_FTP("Socket_dat: %d created successfully\r\n",
					socket_dat);
		}
		server_dat.sin_addr.s_addr = lwftp_ip.addr;// inet_addr(lwftp_ip);
		server_dat.sin_family = AF_INET;
		server_dat.sin_port = htons(data_port);
		// Connect to server for data
		if (connect(socket_dat, (struct sockaddr * )&server_dat,
				sizeof(server_dat)) < 0) {
			perror("Connection data port failed");
			return LWFTP_RESULT_ERR_CONNECT;
		} else {
			NET_DEBUG_FTP("Connected data port\r\n");
		}
		// Store a file read from sdcard "STOR ./<file_name>\r\n"
		memset(request_msg, 0x00, sizeof(request_msg));
		Str_Copy_N(request_msg, "STOR ", LWFTP_CONST_BUF_SIZE);
		Str_Cat_N(request_msg, "./", LWFTP_CONST_BUF_SIZE); // concatenate with current directory path
		//strcat(request_msg, "");
		Str_Cat_N(request_msg, file_name, LWFTP_CONST_BUF_SIZE);
		Str_Cat_N(request_msg, "\r\n", LWFTP_CONST_BUF_SIZE);
		//	char cEOF = 0xFF;
		len = Str_Len_N(request_msg, LWFTP_CONST_BUF_SIZE);
		NET_DEBUG_FTP("Send to socket_ctrl (len = %d): %s\r\n",len, request_msg);
		sent_len = send(socket_ctrl, request_msg, len, 0);
		// Write data to file in server
#ifdef ENABLE_FTP_FILE_TEST
		fr = f_open(&fil, ENABLE_FTP_FILE_TEST, FA_READ);
#else
		char *abs_path = OSA_FixedMemMalloc(256);
		if (!abs_path) {
			NET_DEBUG_ERROR("Unable to allocate memory for abs_path");
			return LWFTP_RESULT_ERR_MEMORY;
		}
		memset(abs_path, 0, 256);
		sprintf(abs_path, "%s/%s", local_path, file_name);
		NET_DEBUG("opening file %s...", abs_path);
		fr = f_open(&fil, abs_path, FA_READ);

		OSA_FixedMemFree(abs_path);
#endif
		btr = LWFTP_CONST_LINE_SIZE;
		if (fr == FR_OK) {
			do {
				// read from file file to buffer reply_msg
				fr = f_read(&fil, data_buf, btr, &br);
				if (fr != FR_OK) {
					NET_DEBUG_FTP("LWFTP read file failed\r\n");
					f_close(&fil);
					// close data socket
					close(socket_dat);
					// close control socket
					Network_LWFTP_Disconnect();
					lwftp_state = LWFTP_CLOSED;
					return LWFTP_RESULT_ERR_LOCAL;
				}
				if (br > 0) {
					NET_DEBUG_FTP("Write to socket data %d bytes\r\n", br);
					sent_len = send(socket_dat, data_buf, br, 0);
					ASSERT(sent_len == br);
				}
			} while (br == btr); // while not reached the EOF
		} else {
			NET_DEBUG_FTP("LWFTP open file failed\r\n");
			f_close(&fil);

			// close data socket
			close(socket_dat);
			// close control socket
			Network_LWFTP_Disconnect();
			lwftp_state = LWFTP_CLOSED;
			return LWFTP_RESULT_ERR_LOCAL;
		}
		// close the file
		f_close(&fil);
		lwftp_state = LWFTP_CLOSING;
		// close data socket
		close(socket_dat);
		// receive message from ctrl socket
		resp_len = 0;
		do {
			max_receive_byte -= resp_len;
			resp_len = recv(socket_ctrl, reply_msg, LWFTP_CONST_BUF_SIZE, 0);
//			if (resp_len > 0)
//				NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len,
//						reply_msg);
			if (resp_len > 0) {
				NET_DEBUG_FTP("reply_msg (len = %d):  %s\r\n", resp_len, reply_msg);
//				response = strtoul(reply_msg, NULL, 10);
//				if (response > 0) {
//					if (response == 150) {
//						ret = LWFTP_RESULT_OK;
//						NET_DEBUG_FTP("Send file to FTP Successfully\r\n");
//					}
//				}
				if(Str_Str_N(reply_msg, "150", LWFTP_CONST_BUF_SIZE)!= NULL) {
					ret = LWFTP_RESULT_OK;
					NET_DEBUG_FTP("Send file to FTP Successfully\r\n");
				}

			}
			// process message and check for response
		} while (resp_len > 0 && resp_len < lwftp_bufsize);
		//lwftp_receive_ctrl_data(socket_ctrl, reply_msg, lwftp_bufsize);

		NET_DEBUG_FTP("Goodbye\r\n");
		lwftp_state = LWFTP_CLOSED;
		Network_LWFTP_Disconnect();
		return ret;
	}
	return LWFTP_RESULT_ERR_UNKNOWN;
}

/**
 * @file Network.c
 * @author ThinhNT (tienthinh@gmail.com)
 * @brief Module Network Implementation
 * @version 0.1
 * @date 2018-10-17
 *
 * @copyright Copyright (c) 2018
 *
 */
#include "Network.h"

static char tmpStr[128];
static char tmpPath[128];
//static char filePath[128];

#include "fsl_sdhc_card.h"
#include "fsl_sdmmc_card.h"
#include "fsl_debug_console.h"
#include "ff.h"
#include "modem.h"
#include "modem_ftp_client.h"
#include "modem_debug.h"
#include  <lib_str.h>
#include <ring_file.h>

static NetStatus net_ftp_client_send_file(const char *dirPath,
		const char *fileName);

OSA_TASK_DEFINE(network_ftpclient_tx, TASK_NETWORK_FTPCLIENT_TX_SIZE);
//OSA_TASK_DEFINE(network_tcpclient, TASK_NETWORK_TCPCLIENT_SIZE);

static uint8_t rxChar;
static uart_state_t uartState;
ring_file_handle_t g_retryTable;

void uart_rx_callback(uint32_t instance, void * uartState) {
	uart_state_t *state = (uart_state_t*) uartState;
	modem_rx_cmplt_callback(state->rxBuff, 1);
}

SNetStt g_Network_Status;

char** str_split(char* a_str, const char a_delim) {
	memset(tmpStr, 0x00, sizeof(tmpStr));
	Str_Copy_N(tmpStr, a_str, sizeof(tmpStr));
//	NET_DEBUG_WARNING("str_split %s at 0x%x\r\n", tmpStr, tmpStr);
	char** result = 0;
	size_t count = 0;
	char* tmp = tmpStr;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;

	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (a_delim == *tmp) {
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	/* Add space for trailing token. */
	count += last_comma < (tmpStr + Str_Len_N(tmpStr, 128) - 1);

	/* Add space for terminating null string so caller
	 knows where the list of returned strings ends. */
	count++;
	NET_DEBUG_WARNING("str_split count = %d\r\n", count);
	result = OSA_FixedMemMalloc(sizeof(char*) * count); //OSA_MemAlloc
	ASSERT_VOID(result != NULL);
	memset(result, 0x00, sizeof(char*) * count);
//	NET_DEBUG_WARNING("str_split result = 0x%x, count = %d\r\n", result, count);
	size_t idx = 0;
	if (result) {

		char* token = strtok(tmpStr, delim);

		while (token) {
			ASSERT_VOID(idx < count);
			*(result + idx++) = (token);
//			NET_DEBUG_WARNING("1 result[%d] at 0x%x = %s, token at 0x%x = %s\r\n",idx-1, result[idx-1],result[idx-1], token, token);
			token = strtok(0, delim);
//			NET_DEBUG_WARNING("2 result[%d] at 0x%x = %s, token at 0x%x = %s\r\n",idx-1, result[idx-1],result[idx-1], token, token);
		}
		ASSERT_VOID(idx == count - 1);
		*(result + idx) = 0;
	}
	NET_DEBUG_WARNING("result at 0x%x, idx = %d\r\n", result, idx);
	return result;
}

/**
 * @brief Khoi tao module SIM + Ethernet, Enable DHCP
 *
 * @return int
 */
int Net_ModuleInitHw() {
	// Init for Ethernet using LWIP
	OS_ERR err;
	// TODO: There should be a class to manage switch connection between
	// ETHERNET & SIMCOMMM
	//Network_LWIP_TCP_Init();
	int32_t currentTick = OSTimeGet(&err);
	Network_LWIP_DHCP_Init();
	NET_DEBUG_TCP("Network_LWIP_DHCP_Init spent ticks: %d\r\n",
			OSTimeGet(&err) - currentTick);
	g_Network_Status.NetConStat = NET_CON_CONNECTED;
	g_Network_Status.NetIF = NET_IF_ETHERNET;

	// Fill in uart config data
	uart_user_config_t uartConfig = { .bitCountPerChar = kUart8BitsPerChar,
			.parityMode = kUartParityDisabled, .stopBitCount = kUartOneStopBit,
			.baudRate = BOARD_MODEM_UART_BAUD };

	// Initialize the uart module with base address and config structure
	UART_DRV_Init(BOARD_MODEM_UART_INSTANCE, &uartState, &uartConfig);

	//Initialize the callback function
	UART_DRV_InstallRxCallback(BOARD_MODEM_UART_INSTANCE, uart_rx_callback,
			&rxChar, &uartState,
			true);

	// Initialize variable uartState of type uart_state_t

//	// Fill in uart config data
//	uart_user_config_t uartConfig = {
//		.bitCountPerChar = kUart8BitsPerChar,
//		.parityMode      = kUartParityDisabled,
//		.stopBitCount    = kUartOneStopBit,
//		.baudRate        = BOARD_MODEM_UART_BAUD
//	};
//
//	// Initialize the uart module with base address and config structure
//	UART_DRV_Init(BOARD_MODEM_UART_INSTANCE, &uartState, &uartConfig);
//
//	//Initialize the callback function
//	UART_DRV_InstallRxCallback(BOARD_MODEM_UART_INSTANCE,
//								  uart_rx_callback,
//								  &rxChar,
//								  &uartState,
//								  true);

//	modem_init();
//
////#define SERVER_IP 	"27.118.20.209"
////#define SERVER_PORT 	21
////#define USER_NAME "ftpuser1"
////#define PASSWORD "zxcvbnm@12"
//	modem_ftp_init("27.118.20.209", 21, "ftpuser1", "zxcvbnm@12");
//	modem_ftp_connect();
	return 1;
}
/**
 * @brief Start 2 TCP Server SIMCOM + LWIP
 * Allow maximum 5 connection
 * @param port
 * @return NetStatus
 */
NetStatus Net_TCPServerStart(int port) {
	lwtcp_result_t ret1 = Network_LWTCPServer_Start(port);
	if (ret1 != LWTCP_RESULT_OK)
		return NET_ERR_LWIP_SERVER;
	return NET_ERR_NONE;
}
/**
 * Check if network is up via Ethernet / SIMCOMM
 * @return
 */
bool Net_Is_Up() {
	bool ret1 = Network_LWIP_Is_Up();
	// TODO: ret2 = SIMCOMM is up
	bool ret2 = true;
	if (ret1)
		return true;
	else {
		if (!ret2)
			return false;
	}

	return true;
}
/**
 * Set the client callback for each client connected to this server
 * @param fn
 * @return
 */
//void Net_RegisterTcpServerDataEvent(Network_DataEvent event, NetworkDataEvent func)
//{
//
//
//	switch (type) {
//		case TCP_SRV_DATA_RECEIVED:
//		case TCP_SRV_CLIENT_CONNECTED:
//		case TCP_SRV_CLIENT_DISCONNECTED:
//			Network_LWTCPServer_Set_Callback(type, fcn);
//			break;
//		default:
//			break;
//	}
////	lwtcp_result_t ret1 = Network_LWTCPServer_Set_Callback(fn);
////	if (ret1 != LWTCP_RESULT_OK)
////		return NET_ERR_LWIP_SERVER;
//	return NET_ERR_NONE;
//}
/**
 * Example callback function for connected client
 * @param fd
 */
void Net_TCP_Echo_ClientCallback(int fd) {
	uint32_t currentTime;	// =  OSA_TimeGetMsec();
	char *tmp = OSA_FixedMemMalloc(128);

	ASSERT_VOID(tmp!=NULL);

	uint32_t lastTime = OSA_TimeGetMsec();
	int len;
	while (true) {
		len = Network_LWTCP_Receive(fd, tmp, 128);
		if (len > 0) {
			// Just do some Echo
			len = Network_LWTCP_Send(fd, tmp, len);
		}
		currentTime = OSA_TimeGetMsec();
		if ((currentTime - lastTime) > 5000) {
			break;
		} else if (len <= 0) {
			OSA_TimeDelay(100);
		}
	}
	OSA_FixedMemFree(tmp);
}
/**
 * Start A TCP Client via current Ethernet / SIMCOM
 * @param ip
 * @param port
 * @return
 */
NetStatus Net_TCPClientStart(const char* ip, int port) {
#if 1
	lwtcp_result_t ret1 = Network_LWTCPClientStart(ip, port);//, usrname, passwd);
	if (ret1 != LWTCP_RESULT_OK) {

		return NET_ERR_LWIP_FTPCLIENT;
	}
#endif
	return NET_ERR_NONE;
}
/**
 * @brief Return Netstatus of IF & connection status
 *
 * @return SNetStt
 */
SNetStt Net_ModuleGetStatus() {
	return g_Network_Status;
}

void network_ftpclient_tx(task_param_t param) {
	NET_DEBUG_TCP("network_ftpclient_tx is started");
	OS_ERR err;
	void *p_msg;
	OS_MSG_SIZE msg_size;
	CPU_TS ts;
	SFTPData *pFTPData;
	NetStatus status;
	CPU_STK_SIZE n_free;
	CPU_STK_SIZE n_used;
//    g_retryTable
	MODEM_DEBUG("Initializing Retry table...");
	ring_file_init(&g_retryTable, "/conf", "retrytable.dat", 5000,
			sizeof(ring_file_record_t));
	MODEM_DEBUG("Init Done!!!");
	ring_file_print(&g_retryTable);

	while (1) {

//		NET_DEBUG_TCP("network_ftpclient_tx is running");
		p_msg = OSTaskQPend(200,
		OS_OPT_PEND_BLOCKING, &msg_size, &ts, &err);
		if (err == OS_ERR_NONE) {
			pFTPData = (SFTPData*) p_msg;
			NET_DEBUG("OSTaskQPend OK: dirPath = %s, fileName = %s",
					pFTPData->dirPath, pFTPData->fileName);
			status = net_ftp_client_send_file(pFTPData->dirPath,
					pFTPData->fileName);
			// TODO: THINHNT xu ly truong hop gui loi, signal callback ...
			if (status != NET_ERR_NONE) {
				MODEM_DEBUG_ERROR("Send file %s/%s FAILED, push to retry table",
						pFTPData->dirPath, pFTPData->fileName);
#if 0
				ring_file_record_t *pRecord = (ring_file_record_t*)OSA_FixedMemMalloc(sizeof(ring_file_record_t));
				if(pRecord)
				{
					memset(pRecord, 0, sizeof(ring_file_record_t));
					memcpy(pRecord->dir_path, pFTPData->dirPath, strlen(pFTPData->dirPath));
					memcpy(pRecord->file_name, pFTPData->fileName, strlen(pFTPData->fileName));

					//TODO: mark which FTP server to send by Flag
					pRecord->flag = 1;

					//TODO: calculate CRC of record.
					pRecord->crc = crc_16((const unsigned char*)pRecord, sizeof(ring_file_record_t) - 2);
					// Push to Retry-table
					ring_file_push_back(&g_retryTable, pRecord);

					OSA_FixedMemFree(pRecord);
				}
				else
				{
					MODEM_DEBUG_CRITICAL("Unable to allocate memory!!!");
				}
#endif
			}
			OSTaskStkChk(NULL, &n_free, &n_used, &err);
			NET_DEBUG("n_free = %d, n_used = %d,  err=%d\r\n",
					TCB_network_ftpclient_tx.StkFree,
					TCB_network_ftpclient_tx.StkUsed, err);
//			OSA_FixedMemFree(pFTPData->dirPath);
//			OSA_FixedMemFree(pFTPData->fileName);
			OSA_FixedMemFree(pFTPData);
			NET_DEBUG("OSA_FixedMemFree(pFTPData) done\r\n");
		}
//		else {
//			MODEM_DEBUG("OSTaskQPend, err = %d", err);
//
//		}
#if 0
//		/* If no message received */
		if(!p_msg)
		{
			// TODO [manhbt] No new file arrived, process retry table

			if(ring_file_get_count(&g_retryTable) > 0)
			{
				ring_file_record_t *pRecord = OSA_FixedMemMalloc(sizeof(ring_file_record_t));

				if(!pRecord)
				{
					MODEM_DEBUG_CRITICAL("Unable to allocate memory for temporarily buffer !!!");
					continue;
				}

				memset(pRecord, 0, sizeof(ring_file_record_t));

				if(ring_file_get_front(&g_retryTable, pRecord) != TRUE)
				{
					MODEM_DEBUG_WARNING("Unable to GET record from Retry table");
					OSA_FixedMemFree(pRecord);
					continue;
				}

				// TODO: [manhbt] verify Record (check CRC)
				uint16_t calc_checksum = crc_16((const unsigned char*)pRecord, sizeof(ring_file_record_t)-2);
				if(calc_checksum != pRecord->crc)
				{
					MODEM_DEBUG_WARNING("CRC not matched, cal: %.2x, got: %.2x", calc_checksum, pRecord->crc);
					OSA_FixedMemFree(pRecord);
					continue;
				}

				// TODO: [manhbt] Re-send File
				MODEM_DEBUG("Trying to re-send file %s/%s", pRecord->dir_path, pRecord->file_name);

				status = net_ftp_client_send_file(pRecord->dir_path, pRecord->file_name);

				if(status == NET_ERR_NONE)
				{
					//TODO: [manhbt] Pop out & erase record from retry table
					MODEM_DEBUG("Re-send file OK, pop out and delete record from retry table");
					ring_file_pop_front(&g_retryTable, pRecord);
				}
				else
				{
					MODEM_DEBUG_WARNING("Re-send file FAILED, retry table remains unchanged");
				}

				OSA_FixedMemFree(pRecord);
			}
			else
			{
				MODEM_DEBUG_RAW(".");//("No new file arrived, process retry table!!!");
			}

		}
#endif
		//OSA_TimeDelay(1000);
	}
}

/**
 * @brief Open FTP Client via Ethernet,
 * Step 1: If not success retry 10 times for each second
 * Step 2: If still failed open via Wireless 10 times for each second
 * Step 3: If still failed retry step 1 & 2 until
 * @param ip Ip address of server
 * @param port port of cmd connection
 * @param usrname username of ftp
 * @param passwd password of ftp
 * @return NetStatus
 */
NetStatus Net_FTPClientStart(const char *ip, int port, const char* usrname,
		const char* passwd) {

	osa_status_t result = OSA_TaskCreate(network_ftpclient_tx,
			(uint8_t *) "network_ftpclient_tx",
			TASK_NETWORK_FTPCLIENT_TX_SIZE, network_ftpclient_tx_stack,
			TASK_NETWORK_FTPCLIENT_TX_PRIO, (task_param_t) 0,
			false, &network_ftpclient_tx_task_handler);
	if (result != kStatus_OSA_Success) {
		PRINTF("Failed to create ftp client tx task\r\n\r\n");
		return -1;
	}

#if 1
	lwtcp_result_t ret1 = Network_LWFTP_Start(ip, port, usrname, passwd);
	if (ret1 != LWTCP_RESULT_OK) {
		NET_DEBUG("LWFTP Client start FAILED!");
//		return NET_ERR_LWIP_FTPCLIENT;
	} else {
		NET_DEBUG("LWFTP Client start OK!");
//		return NET_ERR_NONE;
	}

	// TODO: [manhbt] Init for 2 FTP server (see pApp->sCfg->sComm struct of Hai Do)
	MDMFTP_result_t ret2 = Network_MDMFTP_Start(ip, port, usrname, passwd);
	if (ret2 != MDMFTP_RESULT_OK) {
		NET_DEBUG("Modem FTP Client start FAILED!");
//		return NET_ERR_MODULE_NOT_INIT;
	} else {
		MODEM_DEBUG("Modem FTP Client start OK!");
//		return NET_ERR_NONE;
	}

	if ((ret1 != LWTCP_RESULT_OK) && (ret2 != MDMFTP_RESULT_OK)) {
		NET_DEBUG("FTP Client start FAILED");
		return NET_ERR_MODULE_NOT_INIT;
	}
	return NET_ERR_NONE;

#else
	if(Network_MDMFTP_Start(ip, port, usrname, passwd) != MDMFTP_RESULT_OK)
	{
		MODEM_DEBUG("[manhbt] MDMFTP Client start FAILED!");
		g_Network_Status.NetIF = NET_IF_UNKNOWN;
		g_Network_Status.NetConStat = NET_CON_ERR_UNKNOWN;
	}
	else
	{
		MODEM_DEBUG("[manhbt] MDMFTP Client start OK!");
		g_Network_Status.NetIF = NET_IF_UNKNOWN;
		g_Network_Status.NetConStat = NET_CON_DISCONNECTED;
	}
	return NET_ERR_LWIP_FTPCLIENT;
#endif
}

static NetStatus net_ftp_client_send_file(const char *dirPath,
		const char *fileName) {

	NetStatus ret;
	lwftp_result_t ret1 = LWFTP_RESULT_OK;
	/**
	 * TODO: Seperate filename to get the datetime information AG_SGCE_KHI001_20181107105400.txt
	 * datetime will be /2018/11/07 for folder
	 * AG_SGCE_KHI001_20181107105400.txt will be stored in that folder
	 */
	char** tokens;
	int i = 0, err, retVal;
	int len;
	char cwd[128];
	char tmp[64];
	tokens = str_split(fileName, '_');//AG_SGCE_KHI001_20181107105400.txt then *tokens ->
	//NET_DEBUG_WARNING("Tokens after split = 0x%x\r\n", tokens);
	if (tokens) {
		while (*(tokens + i)) {
			i++;
		}
//		NET_DEBUG_WARNING("i = %d\r\n", i);
		if (i > 0) {
			i -= 1;
			len = Str_Len_N(*(tokens + i), 128);
//			NET_DEBUG_WARNING("tokens[%d] = %s at 0x%x, len = %d\r\n", i, tokens[i], tokens[i], len);
			if (len >= 14) {
				// Do something with this filename  AG_SGCE_KHI001_20181107105400.txt
				memset(tmp, 0, sizeof(tmp));
				memset(tmpPath, 0, sizeof(tmpPath));
				Str_Copy_N(tmpPath, DEFAULT_FTP_FOLDER_PATH, sizeof(tmpPath));//tmpPath = "/home/ftpuser1/test/thinh"
				Str_Cat_N(tmpPath, "/", sizeof(tmpPath)); // tmpPath = "/home/ftpuser1/test/thinh"
				memcpy(tmp, tokens[i], 4); // copy year 2018
				Str_Cat_N(tmpPath, tmp, sizeof(tmpPath)); //tmpPath = "/home/ftpuser1/test/thinh/2018"
				//NET_DEBUG_WARNING("tmpPath = %s\r\n", tmpPath);
				//NET_DEBUG_WARNING("tokens[i] = %s\r\n", tokens[i]);
				memset(tmp, 0, sizeof(tmp));
				memcpy(tmp, tokens[i] + 4, 2);				// copy month 11
				Str_Cat_N(tmpPath, ("/"), sizeof(tmpPath)); // now tmpPath = "/home/ftpuser1/test/thinh/2018/"
				Str_Cat_N(tmpPath, tmp, sizeof(tmpPath)); // now tmpPath = "/home/ftpuser1/test/thinh/2018/11"
				//NET_DEBUG_WARNING("tmpPath = %s\r\n", tmpPath);
				//NET_DEBUG_WARNING("tokens[i] = %s\r\n", tokens[i]);

				memset(tmp, 0, sizeof(tmp));
				memcpy(tmp, tokens[i] + 6, 2);				// copy day 07
				Str_Cat_N(tmpPath, ("/"), sizeof(tmpPath)); // now tmpPath = "/home/ftpuser1/test/thinh/2018/11/"
				Str_Cat_N(tmpPath, tmp, sizeof(tmpPath)); // now tmpPath = "/home/ftpuser1/test/thinh/2018/11/07"
				//NET_DEBUG_WARNING("tmpPath = %s\r\n", tmpPath);
				//NET_DEBUG_WARNING("tokens[i] = %s\r\n", tokens[i]);

//				strcpy(filePath,dirPath); // filePath = "/home
//				strcat(filePath, ("/")); // now tmpPath = "/home/"
//				strcat(filePath, fileName); // now tmpPath = "/home/AG_SGCE_KHI001_20181107105400.txt"

//				err = f_getcwd(cwd, sizeof(cwd));
//				if(err == FR_OK) {
//					LREP("current dir = %s\r\n", cwd);
//					//LREP("cwd successfully\r\n");
//				} else {
//					LREP("cwd failed \r\n");
//				}
//				// Change directory to dirpath
//				retVal = f_chdir(dirPath);
//				if(retVal != FR_OK) {
//					LREP("chdir err = %d\r\n", retVal);
//				}
				// Send file
				NET_DEBUG("SendFile: %s,%s", tmpPath, fileName);
//				ret1 = Network_LWFTP_SendFile(tmpPath, fileName); // dirpath should be//
				ret1 = Network_LWFTP_SendFile2((const char*) dirPath,
						(const char*) tmpPath, (const char*) fileName); // manhbt edited here
				if (ret1 == LWFTP_RESULT_OK) {
					ret = NET_ERR_NONE;
					NET_DEBUG("Send File %s via Ethernet OK", fileName);
				}
//				/**
//				 * manhbt send file via UC15
//				 */
				else {
					LREP("Send File %s via Ethernet FAILED, trying with 3G modem ...\r\n", fileName);
					MDMFTP_result_t ret2 = Network_MDMFTP_SendFile(
							(const char*) dirPath, (const char*) tmpPath,
							(const char*) fileName);
					// TODO: [manhbt] Send file failed, push file info into retry table
					if (ret2 != MDMFTP_RESULT_OK) {
						ret = NET_ERR_SEND_FAILD;
					} else {
						ret = NET_ERR_NONE;
					}
				}
//				// change back to previous directory
//				retVal = f_chdir(cwd);
//				if(retVal != FR_OK) {
//					LREP("chdir err = %d\r\n", retVal);
//				}
			}
		}
		i = 0;
		// Do not need to clear *(tokens +i) because it's tmpStr
//		while(*(tokens+i)) {
//			NET_DEBUG_WARNING("free tokens[%d] at 0x%x\r\n", i, *(tokens+i));
//			free(*(tokens+i));
//			i++;
//		}
		NET_DEBUG_WARNING("OSA_FixedMemFree token (filename) at 0x%x\r\n", tokens);
		OSA_FixedMemFree(tokens);
		return ret;
	} else {
		return NET_ERR_FILENAME;
	}
	return NET_ERR_UNKNOWN;
}

/**
 * @brief If connected then send file.
 * If file send failed retry 3 times
 *
 * @param dirPath directory in the filesystem (sdcard)
 * @param fileName file name (filename should include datetime information)
 * @return NetStatus
 */
NetStatus Net_FTPClientSendFile(const char *dirPath, const char *fileName) {

//#define PATH_SIZE 128
	PRINTF("\r\nNet_FTPClientSendFile (%s,%s)\r\n", dirPath, fileName);

	int strLen = 0;
	SFTPData* pFTPData = (SFTPData*) OSA_FixedMemMalloc(sizeof(SFTPData));
	if (!pFTPData) {
		NET_DEBUG_ERROR("Unable to allocate SFTPData for Net_FTPClientSendFile");
		return NET_ERR_BUSY;
	}

	NET_DEBUG("pFTPData = %x", pFTPData);

	strLen = Str_Len_N(dirPath, LWFTP_CONST_BUF_SIZE);
	if (strLen >= FTP_DATA_SIZE)
		strLen = FTP_DATA_SIZE - 1;
	memset(pFTPData->dirPath, 0x00, FTP_DATA_SIZE);
	Str_Copy_N(pFTPData->dirPath, dirPath, strLen);
	//strcpy(pFTPData->dirPath, dirPath);
	NET_DEBUG("dirPath = %x, strLen = %d, dirPath = %s", pFTPData->dirPath,
			strLen, pFTPData->dirPath);

	strLen = Str_Len_N(fileName, LWFTP_CONST_BUF_SIZE);
	if (strLen >= FTP_DATA_SIZE)
		strLen = FTP_DATA_SIZE - 1;
//	NET_DEBUG("strLen fileName = %d", strLen);
	memset(pFTPData->fileName, 0x00, FTP_DATA_SIZE);
	Str_Copy_N(pFTPData->fileName, fileName, strLen);
	//strcpy(pFTPData->fileName, fileName);//, strLen);
	NET_DEBUG("fileName = %x, strLen = %d, fileName = %s", pFTPData->fileName,
			strLen, pFTPData->fileName);

	OS_ERR err;
	OSTaskQPost(&TCB_network_ftpclient_tx, (void *) pFTPData, sizeof(SFTPData),
	OS_OPT_POST_FIFO, &err);
	if (err != OS_ERR_NONE) {
		NET_DEBUG_FTP("OSTaskQPost err: %d", err);
		return NET_ERR_UNKNOWN;
	} else {
		NET_DEBUG_FTP("OSTaskQPost OK");
		return NET_ERR_NONE;
	}
}

NetStatus Net_FTPClientDeleteFile(const char *path) {
	lwftp_result_t ret1;
	if (g_Network_Status.NetIF == NET_IF_ETHERNET) {
		ret1 = Network_LWFTP_Delete(path);
		if (ret1 == LWFTP_RESULT_OK) {
			return NET_ERR_NONE;
		}
	}
	return NET_ERR_UNKNOWN;
}
/**
 * @brief Send data to server
 *
 * @param data
 * @param length
 * @return NetStatus
 */
NetStatus Net_TCPClientSendData(const uint8_t *data, uint32_t length) {
	// TODO: send to thread /check to send ftp or simcomm
	if (Network_LWIP_Is_Up()) {
		uint32_t len = Network_LWTCPClientSendCmd(data, length);
		if (len == length) {
			NET_DEBUG_TCP("Network_LWTCPClientSendCmd OK\r\n");
			return NET_ERR_NONE;
		}
	}
	return NET_ERR_NONE;
}
/**
 * @brief Send data to all client
 *
 * @param data
 * @param length
 * @return NetStatus
 */
NetStatus Net_TCPServerSendDataToAllClient(const uint8_t *data, uint32_t length) {

}

/**
 * Register Connection Event callback
 * @param func
 */
void Net_RegisterConnEvent(NetworkConnEvent func) {
	// Set LWTCP Client Connection Data Event Handler
	Network_LWTCPClientSetConnEventHandler(func);
}
/**
 * Register Net DataEvent callback function
 * @param event
 * @param func
 */
void Net_RegisterTcpClientDataEvent(Network_DataEvent event,
		NetworkDataEvent func) {
	// Set LWTCP Client Data Event Handler
	Network_LWTCPClientSetDataEventHandler(event, func);
}

void Net_RegisterTcpServerDataEvent(Network_DataEvent event,
		NetworkDataEvent func) {

	Network_LWTCPServer_Set_Callback(event, func);
}

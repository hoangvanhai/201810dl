/*
 * modem_ftp_client.c
 *
 *  Created on: Oct 26, 2018
 *      Author: buiti
 */

#include "modem_ftp_client.h"

#include <debug.h>
#include <string.h>
#include "modem_ftp_command.h"
#include "modem_debug.h"
#include "ff.h"

//static modem_ftp_client_handle_t g_modem_ftp_client;
static BOOL g_modemInitialized = FALSE;

modem_ftp_err_code_t modem_ftp_init(FtpClient *pFC)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;


	/* Configure modem  */
//	modem_init();
////	modem_delay_ms(1000);
//	modem_tx_data("+++", 3);
//	modem_delay_ms(1000);

//	modem_send_at_command(pFC->modem_handler, "AT+QIACT=0\r\n", "OK", 1000, 1);

	modem_send_at_command(pFC->modem_handler, "AT+QICSGP=1,1,\"\",\"\",\"\",1\r\n","OK", 1000, 1);
//		AT+QIACT=1
	modem_send_at_command(pFC->modem_handler, "AT+QIACT=1\r\n", "OK", MODEM_AT_DEFAULT_TIMEOUT_MSEC, 1);
//	modem_send_at_command("AT+QIACT?\r\n", "OK",  1000, 1);
	modem_send_at_command(pFC->modem_handler, "AT+QFTPCFG=\"contextid\",1\r\n", "OK", 500, 1);
	modem_send_at_command(pFC->modem_handler, "AT+QFTPCFG=\"filetype\",1\r\n", "OK", 500, 1);
	modem_send_at_command(pFC->modem_handler, "AT+QFTPCFG=\"transmode\",1\r\n", "OK",500, 1);

	char cmd[128] = {0};
	sprintf(cmd, "AT+QFTPCFG=\"rsptimeout\",%d\r\n", MODEM_AT_DEFAULT_TIMEOUT_SEC);
	modem_send_at_command(pFC->modem_handler, cmd, "OK", 500, 1);
	g_modemInitialized = TRUE;

	return FTP_ERR_NONE;
}


BOOL 				modem_ftp_is_initialized(FtpClient *pFC)
{
	return g_modemInitialized;
}

//int ftp_setup_channel(FtpClient *pFC, uint8_t idx)

modem_ftp_err_code_t modem_ftp_connect(FtpClient *pFC, uint8_t channel)
{

	ASSERT_NONVOID(pFC, FTP_ERR_UNKNOWN);

	uint8_t *cmd = OSA_FixedMemMalloc(128);
	if(!cmd)
	{
		MODEM_DEBUG_CRITICAL("Unable to allocate temporarily buffer for AT command");
		return FTP_ERR_MEM;
	}

	pFC->curr_sv_idx = channel;
	ASSERT_NONVOID(pFC->curr_sv_idx < 2, FTP_ERR_INVALID_PARAM);

	memset (cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+QFTPCFG=\"account\",\"%s\",\"%s\"\r\n",
			pFC->server_list[pFC->curr_sv_idx].username,
			pFC->server_list[pFC->curr_sv_idx].passwd);

	MODEM_DEBUG("[FTP] connecting... server: %s, un: %s, pwd: %s",
			ipaddr_ntoa((const ip_addr_t*)&pFC->server_list[pFC->curr_sv_idx].ip),
			pFC->server_list[pFC->curr_sv_idx].username,
			pFC->server_list[pFC->curr_sv_idx].passwd);

	if (modem_send_at_command(pFC->modem_handler, cmd, "OK", 500, 1) != TRUE)
	{
		OSA_FixedMemFree(cmd);
		return FTP_ERR_AUTHEN;
	}

	memset (cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+QFTPOPEN=\"%s\",%d\r\n",
			ipaddr_ntoa((const ip_addr_t*)&pFC->server_list[pFC->curr_sv_idx].ip),
			pFC->server_list[pFC->curr_sv_idx].port);

	if (modem_send_at_command(pFC->modem_handler, cmd, "+QFTPOPEN: 0,0", MODEM_AT_FTP_CONNECT_TIMEOUT_MSEC, 1) == TRUE)
	{
		OSA_FixedMemFree(cmd);
		MODEM_DEBUG("[FTP] connected");
		return FTP_ERR_NONE;
	}
	else
	{
		MODEM_DEBUG_ERROR("[FTP] connect timed out");
		OSA_FixedMemFree(cmd);
		return FTP_ERR_TIMEOUT;
	}
}


#if 0


modem_ftp_err_code_t modem_ftp_cwd(FtpClient *pFC, const char *dir)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
	int i = 0;

	/**
	 * Example Full file name: /2018/11/11/TINH_COSO_TRAM_20181111001000.txt
	 */


	char ** tokens= str_split(dir, '/');//AG_SGCE_KHI001_20181107105400.txt then *tokens ->
	if (tokens) {
		while(*(tokens+i)) {
			//MODEM_DEBUG("token[i]: %s", *(tokens+i));
			i++;
		}


		if (i > 0) {

			// Check for exist
			uint8_t * dir = OSA_FixedMemMalloc(256);
			if(!dir)
			{
				MODEM_DEBUG_ERROR("Failed to allocate memory!");
				return ret;
			}

			int j = 0, len = 0;
			for (j = 0; i < i; j++)
			{
				strcat(dir, "/");
				len += strlen(*(tokens+j));
				memcpy(dir, *(tokens+j), strlen(*(tokens+j)));

				MODEM_DEBUG("dir: %s", dir);

				if(modem_ftp_change_working_dir(pFC, (const char*)dir) != FTP_ERR_NONE)
				{
					if( modem_ftp_make_dir(pFC, (char*)dir) == FTP_ERR_NONE)
						MODEM_DEBUG("Create dir %s OK!", dir);
					else
						MODEM_DEBUG("Create dir %s ERROR!", dir);
				}
			}

			OSA_FixedMemFree(dir);




//			i-=1;
//			len = strlen(*(tokens+i));
//
//			MODEM_DEBUG("len %d", len);
//			if (len >= 14) {
//				// Do something with this filename  AG_SGCE_KHI001_20181107105400.txt
//				memset(tmpStr,0,sizeof(tmpStr));
//				memset(tmpPath,0,sizeof(tmpPath));
//				strcpy(tmpPath, DEFAULT_FTP_FOLDER_PATH);//tmpPath = "/home/ftpuser1/test/thinh"
//				strcat(tmpPath, "/"); // tmpPath = "/home/ftpuser1/test/thinh"
//				memcpy(tmpStr, *(tokens+i), 4);// copy year 2018
//				strcat(tmpPath, tmpStr); //tmpPath = "/home/ftpuser1/test/thinh/2018"
//
//				memset(tmpStr,0,sizeof(tmpStr));
//				memcpy(tmpStr, (*(tokens+i))+4, 2);// copy month 11
//				strcat(tmpPath, ("/")); // now tmpPath = "/home/ftpuser1/test/thinh/2018/"
//				strcat(tmpPath, tmpStr); // now tmpPath = "/home/ftpuser1/test/thinh/2018/11"
//
//				memset(tmpStr,0,sizeof(tmpStr));
//				memcpy(tmpStr, (*(tokens+i))+6, 2);// copy day 07
//				strcat(tmpPath, ("/")); // now tmpPath = "/home/ftpuser1/test/thinh/2018/11/"
//				strcat(tmpPath, tmpStr); // now tmpPath = "/home/ftpuser1/test/thinh/2018/11/07"
//			}
		}
	}

	return ret;

}
#endif

modem_ftp_err_code_t modem_ftp_change_working_dir(FtpClient *pFC, char* working_dir)
{
	char *cmd = OSA_FixedMemMalloc(128);
	if(!cmd)
	{
		MODEM_DEBUG_CRITICAL("Unable to allocate temporarily buffer for AT command");
		return FTP_ERR_MEM;
	}

	memset (cmd, 0, sizeof(cmd));

	sprintf(cmd, "AT+QFTPCWD=\"%s\"\r\n", working_dir);

	MODEM_DEBUG("[FTP] CWD to: %s", working_dir);

	if (modem_send_at_command(pFC->modem_handler, cmd, "+QFTPCWD: 0,0", MODEM_AT_FTP_CWDIR_TIMEOUT_MSEC, 1) == TRUE)
	{
		OSA_FixedMemFree(cmd);
		MODEM_DEBUG("[FTP] CWD: %s OK!", working_dir);
		return FTP_ERR_NONE;
	}
	else
	{
		MODEM_DEBUG_ERROR("[FTP] CWD: %s FAILED!", working_dir);
		OSA_FixedMemFree(cmd);
		return FTP_ERR_CD;
	}
}

modem_ftp_err_code_t modem_ftp_disconnect(FtpClient *pFC)
{
//	AT+QFTPCLOSE //Logout from FTP server.
//	OK
//	+QFTPCLOSE: 0,0
//	AT+QIDEACT=1

	char *cmd = OSA_FixedMemMalloc(128);
	if(!cmd)
	{
		MODEM_DEBUG_CRITICAL("Unable to allocate temporarily buffer for AT command");
		return FTP_ERR_MEM;
	}

	memset (cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+QFTPCLOSE\r\n");

	MODEM_DEBUG("[FTP] disconnecting ...");

	if(modem_send_at_command(pFC->modem_handler, cmd, "+QFTPCLOSE:", MODEM_AT_FTP_DISCONNECT_TIMEOUT_MSEC, 1) == FALSE)
	{
		OSA_FixedMemFree(cmd);
		MODEM_DEBUG_ERROR("[FTP] disconnect timed out");
		return FTP_ERR_UNKNOWN;
	}
	OSA_FixedMemFree(cmd);
	MODEM_DEBUG("[FTP] disconnected");

	return FTP_ERR_NONE;

//	memset (cmd, 0, sizeof(cmd));
//	sprintf(cmd, "AT+QIDEACT=1\r\n");
//	if (modem_send_at_command(pFC->modem_handler, cmd, "OK", MODEM_AT_DEFAULT_TIMEOUT_MSEC, 1) == TRUE)
//		return FTP_ERR_NONE;
//	else
//		return FTP_ERR_TIMEOUT;

}


modem_ftp_err_code_t modem_ftp_get_list_files(FtpClient *pFC, char* path, char* filelist, uint32_t timeout)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
//	modem_ftp_client_handle_t *pHandle = &g_modem_ftp_client;
//	char cmd[256] = {0};
//	char response[4096] = {0};
//	int recv_len = 0;
//	//get list files
//	recv_len = 0;
//	memset(cmd, 0, sizeof(cmd));
//	memset(response, 0, sizeof(response));
//
//	sprintf(cmd, AT_CMD_SET_FTP_GETLIST, (const char*)path);
//	LREP_WARNING("sending command: %s", cmd);
//	ret = exlib_platform_serial_write_read_response(&pHandle->pSerialPort, cmd, strlen(cmd), response, &recv_len, timeout, "+CFTPLIST: 0");
//
//	if((recv_len > 0) && (strstr(response, "OK") != NULL))
//	{
//		LREP("SIMCOM FTP client AT_CMD_SET_FTP_GETLIST OK!");
//		LREP_DUMP(response, recv_len, "Received data");
//		ret = FTP_ERR_NONE;
//	}
//	else
//	{
//		LREP_WARNING("SIMCOM FTP CLient init [AT_CMD_SET_FTP_GETLIST] failed, recv %d bytes: %s", recv_len, response);
//		ret = FTP_ERR_TIMEOUT;
//	}
//
//

	return ret;
}
modem_ftp_err_code_t modem_ftp_get_list(FtpClient *pFC, char* path, SList* psFileList, uint32_t timeout)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
//	modem_ftp_client_handle_t *pHandle = &g_modem_ftp_client;

//	char cmd[256] = {0};
//	char response[4096] = {0};
//	int recv_len = 0;
//	//get list files
//	recv_len = 0;
//	memset(cmd, 0, sizeof(cmd));
//	memset(response, 0, sizeof(response));
//
//	sprintf(cmd, AT_CMD_SET_FTP_GETLIST, (const char*)path);
//	LREP_WARNING("sending command: %s", cmd);
//	ret = exlib_platform_serial_write_read_response(&pHandle->pSerialPort, cmd, strlen(cmd), response, &recv_len, timeout, "+CFTPLIST: 0");
//
//	if((recv_len > 0) && (strstr(response, "OK") != NULL))
//	{
//		LREP("SIMCOM FTP client AT_CMD_SET_FTP_GETLIST OK!");
//		LREP_DUMP(response, recv_len, "Received data");
//
//		//TODO: process response data
//		ListIsEmpty(psFileList);
//		modem_ftp_file_attr_t attr;
//		for (int i = 0; i < recv_len; i ++)
//		{
//			// TODO: parse data to get file property
////			ListAddTail( SList *psList, void* pvNewElement );
//
//			ListAddTail(psFileList, (void*)&attr);
//		}
//		ret = FTP_ERR_NONE;
//	}
//	else
//	{
//		LREP_WARNING("SIMCOM FTP CLient init [AT_CMD_SET_FTP_GETLIST] failed, recv %d bytes: %s", recv_len, response);
//		ret = FTP_ERR_TIMEOUT;
//	}
//
//

	return ret;
}

modem_ftp_err_code_t modem_ftp_get_file(FtpClient *pFC, char* filename, uint8_t* content, uint32_t* len, uint32_t timeout)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
//	modem_ftp_client_handle_t *pHandle = &g_modem_ftp_client;

//	char cmd[256] = {0};
//	char response[4096] = {0};
//	int recv_len = 0;
//	//get list files
//	recv_len = 0;
//	memset(cmd, 0, sizeof(cmd));
//	memset(response, 0, sizeof(response));
//
//	sprintf(cmd, AT_CMD_SET_FTP_GETLIST, (const char*)path);
//	LREP_WARNING("sending command: %s", cmd);
//	ret = exlib_platform_serial_write_read_response(&pHandle->pSerialPort, cmd, strlen(cmd), response, &recv_len, timeout, "+CFTPLIST: 0");
//
//	if((recv_len > 0) && (strstr(response, "OK") != NULL))
//	{
//		LREP("SIMCOM FTP client AT_CMD_SET_FTP_GETLIST OK!");
//		ret = FTP_ERR_NONE;
//	}
//	else
//	{
//		LREP_WARNING("SIMCOM FTP CLient init [AT_CMD_SET_FTP_GETLIST] failed, recv %d bytes: %s", recv_len, response);
//		ret = FTP_ERR_TIMEOUT;
//	}
//
//
//
	return ret;
}


modem_ftp_err_code_t modem_ftp_put_file(FtpClient *pFC,  char* filename, uint8_t* content, uint32_t len, uint32_t timeout)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
//	modem_ftp_client_handle_t *pHandle = &g_modem_ftp_client;
//	if(!pHandle || !filename || !content)
//	{
//		PRINTF("Param invalid!!!");
//		ret = FTP_ERR_INVALID_PARAM;
//		return ret;
//	}
	char *cmd = OSA_FixedMemMalloc(128);
	if(!cmd)
	{
		MODEM_DEBUG_CRITICAL("Unable to allocate temporarily buffer for AT command");
		return FTP_ERR_MEM;
	}

	memset (cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+QFTPPUT=\"%s\",\"COM:\",0\r\n", filename);

	if(modem_send_at_command(pFC->modem_handler, cmd, "CONNECT", MODEM_AT_DEFAULT_TIMEOUT_MSEC, 1) == TRUE)
	{
		MODEM_DEBUG("Begin Putting File");

		modem_tx_data(content, len);

		modem_delay_ms(2000);

		if(modem_send_at_command(pFC->modem_handler, "+++", "OK", MODEM_AT_DEFAULT_TIMEOUT_MSEC, 1) == TRUE)
			ret = FTP_ERR_NONE;

		MODEM_DEBUG("End Putting File");

	}
	else
	{
		ret = FTP_ERR_PUT;
	}

	OSA_FixedMemFree(cmd);
	return ret;
}


//FtpClient *pFC, uint8_t index,
//								char *filename,
//								char *local_path,
//								char *remote_path
modem_ftp_err_code_t modem_ftp_make_dir(FtpClient *pFC, char* dir)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;

	uint8_t *cmd = OSA_FixedMemMalloc(128);
	if(!cmd)
	{
		MODEM_DEBUG_CRITICAL("Unable to allocate temporarily buffer for AT command");
		return FTP_ERR_MEM;
	}

	memset (cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+QFTPMKDIR=\"%s\"\r\n", dir);
//	MODEM_DEBUG("cmd: %s", cmd);

	if(modem_send_at_command(pFC->modem_handler, (const char *)cmd, "+QFTPMKDIR: 0,0", MODEM_AT_FTP_MKDIR_TIMEOUT_MSEC, 1) == TRUE)
	{
		MODEM_DEBUG("Create DIR %s OK!", dir);
		ret = FTP_ERR_NONE;
	}
	else
	{
		MODEM_DEBUG_WARNING("Create DIR %s FAILED", dir);
		ret = FTP_ERR_MKD;
	}
	OSA_FixedMemFree(cmd);
	return ret;
}


modem_ftp_err_code_t modem_ftp_remove_dir(FtpClient *pFC, char* dir)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
//	modem_ftp_client_handle_t *pHandle = &g_modem_ftp_client;
//	if(!pHandle || !dir)
//	{
//		PRINTF("Param invalid!!!");
//		ret = FTP_ERR_INVALID_PARAM;
//		return ret;
//	}

	//AT+QFTPRMDIR=<folder_name>
	char *cmd = OSA_FixedMemMalloc(128);
	if(!cmd)
	{
		MODEM_DEBUG_CRITICAL("Unable to allocate temporarily buffer for AT command");
		return FTP_ERR_MEM;
	}

	memset (cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+QFTPRMDIR=\"%s\"\r\n", dir);

	if(modem_send_at_command(pFC->modem_handler, cmd, "+QFTPRMDIR: 0,0", MODEM_AT_DEFAULT_TIMEOUT_MSEC, 1) == TRUE)
	{
		PRINTF("Remove DIR OK!\r\n");
			ret = FTP_ERR_NONE;
	}
	else
	{
		PRINTF("Remove DIR FAILED!\r\n");
		ret = FTP_ERR_UNKNOWN;
	}
	OSA_FixedMemFree(cmd);
	return ret;
}


modem_ftp_err_code_t modem_ftp_rename(FtpClient *pFC, char* old_name, char* new_name)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
//	modem_ftp_client_handle_t *pHandle = &g_modem_ftp_client;
//	if(!pHandle || !old_name || !new_name)
//	{
//		PRINTF("Param invalid!!!");
//		ret = FTP_ERR_INVALID_PARAM;
//		return ret;
//	}

	//AT+QFTPRMDIR=<folder_name>
	char *cmd = (char*)OSA_FixedMemMalloc(128);
	if(!cmd)
	{
		MODEM_DEBUG_CRITICAL("Unable to allocate temporarily buffer for AT command");
		return FTP_ERR_MEM;
	}

	memset (cmd, 0, sizeof(cmd));
	sprintf(cmd, "AT+QFTPRENAME=\"%s\",\"%s\"\r\n", old_name, new_name);

	if(modem_send_at_command(pFC->modem_handler, cmd, "+QFTPRENAME: 0,0", MODEM_AT_DEFAULT_TIMEOUT_MSEC, 1) == TRUE)
	{
		PRINTF("Remove DIR OK!\r\n");
			ret = FTP_ERR_NONE;
	}
	else
	{
		PRINTF("Remove DIR FAILED!\r\n");
		ret = FTP_ERR_UNKNOWN;
	}
	OSA_FixedMemFree((uint8_t*)cmd);
	return ret;
}

modem_ftp_err_code_t modem_ftp_get_status(FtpClient *pFC)
{
	return FTP_ERR_NONE;
}



//modem_ftp_err_code_t modem_ftp_put_file_from_sd(const char *local_path, const char *remote_path, const char *file_name){
modem_ftp_err_code_t
modem_ftp_put_file_from_local(FtpClient *pFC, uint8_t index,
								char *file_name,
								char *local_path,
								char *remote_path)
{

	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;
//	MDMFTP_result_t result;

	int iTries = 0;
	char *ptr;

	FIL fil; // file object
	UINT btr; // number of file to read
	UINT br; // number of read bytes
	FRESULT fr; // FatFS return code
	size_t len; // written length
	char buf[256] = {0};
	modem_ftp_err_code_t mdm_ret = FTP_ERR_UNKNOWN;
	if(modem_ftp_is_initialized(pFC) != TRUE)
	{
//		MODEM_DEBUG_WARNING("Modem FTP CLIENT is not configured yet, try again later!");
		return FTP_ERR_UNKNOWN;
	}
	// TODO: Step 1: Check the connection, if not connect connect and try
//	pFC->modem_handler->busy = true;
	if (MODEM_ENTER_CRITIAL(pFC->modem_handler, MODEM_AT_DEFAULT_TIMEOUT_MSEC) != kStatus_OSA_Success)
	{
		ret = FTP_ERR_TIMEOUT;
		MODEM_DEBUG_CRITICAL("Push file failed (modem  busy)");
		return ret;
	}

	// Step 1: Connect to the server if not connected
	// TODO: manhbt - try to reconnect FTP server if necessary
	modem_switch_to_command_mode(pFC->modem_handler);
	mdm_ret = modem_ftp_connect(pFC, index);
	if(mdm_ret != FTP_ERR_NONE)
	{
		MODEM_DEBUG_WARNING("FTP connect timed out!");
		//modem_ftp_disconnect();
		modem_tx_data("AT+QFTPCLOSE\r\n", strlen("AT+QFTPCLOSE\r\n"));
//		pFC->modem_handler->busy = false;
		MODEM_EXIT_CRITIAL(pFC->modem_handler);
		return FTP_ERR_TIMEOUT;
	}
	// Step 2: Change to directory, check if it exists?

	// TODO: [manhbt] Change directory or make (then change) new if not exist

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", pFC->server_list[pFC->curr_sv_idx].prefix, remote_path);
//	MODEM_DEBUG("Change dir to %s", buf);
	mdm_ret = modem_ftp_change_working_dir(pFC, buf);
	if( mdm_ret != FTP_ERR_NONE) {
//		ret = modem_ftp_make_dir(pFC, remote_path);
		ret = modem_ftp_make_dir_recursive(pFC, buf);

		// FIXME: nghi van cho nay cd tiep se bi loi long thu muc
//		if( mdm_ret == FTP_ERR_NONE)
//		{
//			mdm_ret = modem_ftp_change_working_dir(pFC, remote_path);
//		}
		modem_tx_data("AT+QFTPCLOSE\r\n", strlen("AT+QFTPCLOSE\r\n"));
//		pFC->modem_handler->busy = false;
		MODEM_EXIT_CRITIAL(pFC->modem_handler);
		return FTP_ERR_CD;

	}

	// Step 3: Connected, try to send file to server
	// Write data to file in server

	memset(buf, 0, 256);
	sprintf(buf, "%s%s", local_path, file_name);
	MODEM_DEBUG("opening file %s", buf);
#ifdef ENABLE_FTP_FILE_TEST
	fr = f_open(&fil, ENABLE_FTP_FILE_TEST, FA_READ);
#else
	fr = f_open(&fil, buf, FA_READ);
#endif
	btr = 256;
	mdm_ret = FTP_ERR_TIMEOUT;
	if (fr == FR_OK)
	{
		char cmd[128] = {0};

		memset (cmd, 0, sizeof(cmd));
		sprintf(cmd, "AT+QFTPPUT=\"%s\",\"COM:\",0\r\n", &file_name[1]);
		MODEM_DEBUG("Sending AT command '%s'", cmd);
		if(modem_send_at_command(pFC->modem_handler, cmd, "CONNECT", MODEM_AT_FTP_CONNECT_TIMEOUT_MSEC, 1) == TRUE)
		{
			MODEM_DEBUG("Begin put file content");
			do {
				// read from file file to buffer reply_msg
				memset(buf, 0, sizeof(buf));
				fr = f_read(&fil, buf, btr, &br);
				if (fr != FR_OK) {
					MODEM_DEBUG("LWFTP read file failed\r\n");
//					pFC->modem_handler->busy = false;
					MODEM_EXIT_CRITIAL(pFC->modem_handler);
					return FTP_ERR_FILE;
				}
				if (br > 0) {
					MODEM_DEBUG("Putting %d bytes", br);
					//TODO: manhbt - put all content to 01 file (big file???)
					modem_tx_data(buf, br);
				}
			} while (br == btr); // while not reached the EOF

			modem_delay_ms(2000);
			MODEM_DEBUG("Sending terminate sequence");
			if(modem_send_at_command(pFC->modem_handler, "+++", "OK", MODEM_AT_FTP_TERM_TIMEOUT_MSEC, 1) == TRUE)
				ret = FTP_ERR_NONE;
		}
		else {
			ret  = FTP_ERR_TIMEOUT;
			MODEM_DEBUG_WARNING("AT+QFTPPUT timed out");
		}
	}

	else {
		MODEM_DEBUG("Open file failed\r\n");
//		pFC->modem_handler->busy = false;
		MODEM_EXIT_CRITIAL(pFC->modem_handler);
		return FTP_ERR_FILE;
	}
	// close the file
	f_close(&fil);
	modem_ftp_disconnect(pFC);
//	modem_tx_data("AT+QFTPCLOSE\r\n", strlen("AT+QFTPCLOSE\r\n"));
	MODEM_DEBUG("Goodbye\r\n");
//	pFC->modem_handler->busy = false;
	MODEM_EXIT_CRITIAL(pFC->modem_handler);
	return ret;
}

#if (0)
int chdir_up_a_level(const char* path, char* up_dir, char* sub_dir)
{
	printf ("dir: %s\r\n", path);

	char* p = strrchr(path,'/');

	if(p == NULL) {
		//printf("can not find '/'\r\n");
		return -1;
	}

	//printf ("p: %s\r\n, loc: %d", p, p - argv[1] + 1);
	memcpy(up_dir, path, p - path);

	memcpy(sub_dir, p+1, strlen(p+1));

	//printf ("up dir: %s\r\n", updir);
	return 0;
}



int modem_ftp_change_dir(FtpClient *pFC, char* dir) {

	ASSERT_NONVOID(pFC, -1);
	char updir[128] = {0};
	char subdir[128] ={0};
	modem_ftp_err_code_t mdm_ret = modem_ftp_change_working_dir(pFC, dir);
	if( mdm_ret == FTP_ERR_NONE)
	{
//		if(modem_ftp_change_working_dir(pFC, subdir) == FTP_ERR_NONE){
//			MODEM_DEBUG("Change to directory %s/%s OK\r\n", dir, subdir);
//		} else {
//
//		}
		MODEM_DEBUG("Change to directory %s OK\r\n", dir);
	}
	else
	{
		ret = modem_ftp_make_dir(pFC, dir);
		if( mdm_ret == FTP_ERR_NONE)
		{
			mdm_ret = modem_ftp_change_working_dir(pFC, remote_path);
		}
		else {

			int res = chdir_up_a_level((const char *)working_dir,updir, subdir);
			if (res == 0) {

				MODEM_DEBUG("Change directory up a level %s OK\r\n", working_dir);
			} else {
				MODEM_DEBUG("Change dir up a level failed!");
			}
		}
		modem_tx_data("AT+QFTPCLOSE\r\n", strlen("AT+QFTPCLOSE\r\n"));
		return FTP_ERR_CD;
	}

//	while(chdir_up_a_level((const char *)))
}
#endif



modem_ftp_err_code_t modem_ftp_make_dir_recursive(FtpClient *pFC, const char* dir)
{
	modem_ftp_err_code_t ret = FTP_ERR_UNKNOWN;


	ASSERT_NONVOID(pFC,  FTP_ERR_INVALID_PARAM);

	ASSERT_NONVOID(strlen(dir) < 256, FTP_ERR_INVALID_PARAM);

	// copy dir

	uint8_t *cpydir = OSA_FixedMemMalloc(strlen(dir));
	if(!cpydir) {
		MODEM_DEBUG_CRITICAL("Unable to allocate memory");
		return FTP_ERR_MEM;
	}

	memcpy(cpydir, dir, strlen(dir));

	uint8_t	*tmpDir = OSA_FixedMemMalloc(128);
	if (!tmpDir) {
		MODEM_DEBUG_CRITICAL("Unable to allocate memory");
		OSA_FixedMemFree(cpydir);
		return FTP_ERR_MEM;
	}

	memset(tmpDir, 0, 128);

	char *token = strtok(cpydir, "/");

	// Keep printing tokens while one of the
	// delimiters present in str[].
#if (0)
	if(token != NULL){

		strcat(tmpDir, token);
//		printf("token: %s, dir: %s\n", token, tmpDir);
		token = strtok(NULL, "/");
//		if(modem_ftp_change_working_dir(pFC, tmpDir) != FTP_ERR_NONE) {
//			modem_ftp_make_dir(pFC, tmpDir);
//		}
		modem_ftp_make_dir(pFC, tmpDir);
	}
#endif
	while (token != NULL)
	{
		strcat(tmpDir, "/");
		strcat(tmpDir, token);
//		printf("token: %s, dir: %s\n", token, tmpDir);
//		if(modem_ftp_change_working_dir(pFC, tmpDir) != FTP_ERR_NONE) {
//			modem_ftp_make_dir(pFC, tmpDir);
//		}
		modem_ftp_make_dir(pFC, tmpDir);
		token = strtok(NULL, "/");
	}

	OSA_FixedMemFree(cpydir);
	OSA_FixedMemFree(tmpDir);
	return FTP_ERR_NONE;
}

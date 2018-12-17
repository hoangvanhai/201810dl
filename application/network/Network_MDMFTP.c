/**
 * @file Network_MDMFTP.h
 * @author manhbt
 * @brief Module Network FTP client implementation via 3G modem library
 * @version 0.1
 * @date 2018-10-17
 *
 * @copyright Copyright (c) 2018
 *
 */

#include <string.h>
#include "Network_MDMFTP.h"
#include "ff.h"
//static int mdmftp_reconnect();
//static int mdmftp_connect();
static char mdm_file_content_data_buf[256];

MDMFTP_result_t Network_MDMFTP_Start(const char *ip, int port, const char* usrname, const char* passwd)
{


	if(modem_ftp_init(ip, port, usrname, passwd) == FTP_ERR_OK)
	{
		return MDMFTP_RESULT_OK;
	}
	else
		return MDMFTP_RESULT_ERR_CONNECT;
}



//MDMFTP_result_t Network_MDMFTP_SendFile(const char *dirPath, const char *fileName) {
MDMFTP_result_t Network_MDMFTP_SendFile(const char *local_path, const char *remote_path, const char *file_name){


	MDMFTP_result_t ret = MDMFTP_RESULT_ERR_UNKNOWN;
	MDMFTP_result_t result;

	int iTries = 0;
	char *ptr;

	FIL fil; // file object
	UINT btr; // number of file to read
	UINT br; // number of read bytes
	FRESULT fr; // FatFS return code
	size_t len; // written length

	modem_ftp_err_code_t mdm_ret = FTP_ERR_UNKNOWN;
	if(modem_ftp_is_initialized() != TRUE)
	{
		MODEM_DEBUG_WARNING("Modem FTP CLIENT is not configured yet, try again later!");
		return FTP_ERR_NOT_ALLOWED;
	}
	// Connect and try at least 10 times
	// TODO: Step 1: Check the connection, if not connect connect and try
//	while (iTries++ < 1) {
		// Step 1: Connect to the server if not connected
		// TODO: manhbt - try to reconnect FTP server if necessary
		modem_switch_to_command_mode();
		mdm_ret = modem_ftp_connect();
		if(mdm_ret != FTP_ERR_OK)
		{
			MODEM_DEBUG_WARNING("FTP connect timed out!");
			//modem_ftp_disconnect();
			modem_tx_data("AT+QFTPCLOSE\r\n", strlen("AT+QFTPCLOSE\r\n"));
			return MDMFTP_RESULT_ERR_TIMEOUT;
		}
		// Step 2: Change to directory, check if it exists?

		// TODO: [manhbt] Change directory or make (then change) new if not exist
		mdm_ret = modem_ftp_change_working_dir(remote_path);
		if( mdm_ret == FTP_ERR_OK)
		{
			MODEM_DEBUG("Change to directory %s\r\n", remote_path);
		}
		else
		{
			ret = modem_ftp_make_dir(remote_path);
			if( mdm_ret == FTP_ERR_OK)
			{
				mdm_ret = modem_ftp_change_working_dir(remote_path);
			}
			modem_tx_data("AT+QFTPCLOSE\r\n", strlen("AT+QFTPCLOSE\r\n"));
			return MDMFTP_RESULT_ERR_FILENAME;
		}




		// Step 3: Connected, try to send file to server
		// Write data to file in server

		char fileName[256] = {0};
		sprintf(fileName, "%s/%s", local_path, file_name);
		MODEM_DEBUG("opening file %s", fileName);
#ifdef ENABLE_FTP_FILE_TEST
		fr = f_open(&fil, ENABLE_FTP_FILE_TEST, FA_READ);
#else
		fr = f_open(&fil, fileName, FA_READ);
#endif
		btr = LWFTP_CONST_LINE_SIZE;
		mdm_ret = FTP_ERR_TRANSFER_FAILED;
		if (fr == FR_OK)
		{

			char cmd[128] = {0};

			memset (cmd, 0, sizeof(cmd));
			sprintf(cmd, "AT+QFTPPUT=\"%s\",\"COM:\",0\r\n", file_name);
			MODEM_DEBUG("Sending AT command '%s'", cmd);
			if(modem_send_at_command(cmd, "CONNECT", 15000, 1) == TRUE)
			{
				MODEM_DEBUG("Begin put file content");
				do {
					// read from file file to buffer reply_msg
					memset(mdm_file_content_data_buf, 0, sizeof(mdm_file_content_data_buf));
					fr = f_read(&fil, mdm_file_content_data_buf, btr, &br);
					if (fr != FR_OK) {
						MODEM_DEBUG("LWFTP read file failed\r\n");
						return MDMFTP_RESULT_ERR_LOCAL;
					}
					if (br > 0) {
						MODEM_DEBUG("Putting %d bytes", br);
						//TODO: manhbt - put all content to 01 file (big file???)
						modem_tx_data(mdm_file_content_data_buf, br);
					}
				} while (br == btr); // while not reached the EOF

				modem_delay_ms(2000);
				MODEM_DEBUG("Sending terminate sequence");
				if(modem_send_at_command("+++", "OK", 10000, 1) == TRUE)
					ret = MDMFTP_RESULT_OK;
			}
			else
				ret  = MDMFTP_RESULT_ERR_TIMEOUT;
		}

		else {
			MODEM_DEBUG("Open file failed\r\n");
			return MDMFTP_RESULT_ERR_LOCAL;
		}
		// close the file
		f_close(&fil);
		modem_ftp_disconnect();
		MODEM_DEBUG("Goodbye\r\n");

		return ret;
//	}
	return MDMFTP_RESULT_ERR_UNKNOWN;
}

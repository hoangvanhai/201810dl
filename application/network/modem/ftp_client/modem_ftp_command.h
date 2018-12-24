/*
 * simcom_ftp_command.h
 *
 *  Created on: Oct 26, 2018
 *      Author: buiti
 */

#ifndef SIMCOM_SIMCOM_FTP_COMMAND_H_
#define SIMCOM_SIMCOM_FTP_COMMAND_H_

/**
 * Configure for Cellular modem used
 * Please select in list of modem supported as below
 *     SIM7600 - SIMCOM SIM7600CE 4G/LTE Cat4 module
 *     SIM5320	- SIMCOM SIM5320E 3G module
 *     UG95 - Quectel UG95 module
 *     UC20	- Quectel UC20 3G module
 *     UC15	- Quectel UC15 3G module
 *     SIM868	- SIMCOM SIM860 GPRS/GPS Module
 */
enum {
	MODEM_MODEL_BEGIN = 0,
	SIMCOM_SIM7600,
	SIMCOM_SIM5320,
	QUECTEL_UG95,
	QUECTEL_UC20,
	QUECTEL_UC15,
	SIMCOM_SIM868,
	MODEM_MODEL_END,
};



#define SIM_MODEL	QUECTEL_UC15

/**
 * String format for FTP related AT commands on SIM7600 (LTE) module
 */
#if (SIM_MODEL == SIMCOM_SIM7600)
	#define AT_CMD_SET_FTP_SERVER_ADDR 	"AT+CFTPSERV=\"%s\"\r\n"		// Set FTP server address
	#define AT_CMD_SET_FTP_SERVER_PORT 	"AT+CFTPPORT=%d\r\n"			// Set FTP Port, default 21
	#define AT_CMD_SET_FTP_SERVER_MODE 	"AT+CFTPMODE=%d\r\n"			// Set FTP mode
	#define AT_CMD_SET_FTP_SERVER_TYPE 	"AT+CFTPTYPE=\"%s\"\r\n"		// Set FTP type, 'I' - Binary; 'A' - ASCII
	#define AT_CMD_SET_FTP_USERNAME 	"AT+CFTPUN=\"%s\"\r\n"			// Set Username
	#define AT_CMD_SET_FTP_PASSWORD 	"AT+CFTPPW=\"%s\"\r\n"			// Set Password
	#define AT_CMD_SET_FTP_GETLIST 		"AT+CFTPLIST=\"%s\"\r\n"		// Get List of Files on FTP server
	#define AT_CMD_SET_FTP_GETFILE 		"AT+CFTPGET=\"%s\",%d\r\n"		// Get a file from FTP server and output to SIO
	#define AT_CMD_SET_FTP_PUTFILE 		"AT+CFTPPUT=\"%s\"\r\n"			// Put a file from SIO to FTP server
	#define AT_CMD_SET_FTP_DELFILE 		"AT+CFTPDELE=\"%s\"\r\n"		// Delete a file from FTP server
	#define AT_CMD_SET_FTP_MKDIR 		"AT+CFTPMKD=\"%s\"\r\n"			// Make directory on FTP server
	#define AT_CMD_SET_FTP_RMDIR 		"AT+CFTPRMD=\"%s\","			// Delete directory from FTP server
#elif (SIM_MODEL == SIMCOM_SIM5320)

#elif (SIM_MODEL == QUECTEL_UC15)
	#define AT_CMD_SET_FTP_SERVER_ADDR 	"AT+CFTPSERV=\"%s\"\r\n"			// Set FTP server address
	#define AT_CMD_SET_FTP_SERVER_PORT 	"AT+CFTPPORT=%d\r\n"			// Set FTP Port, default 21
	#define AT_CMD_SET_FTP_SERVER_MODE 	"AT+CFTPMODE=%d\r\n"			// Set FTP mode
	#define AT_CMD_SET_FTP_SERVER_TYPE 	"AT+CFTPTYPE=\"%s\"\r\n"		// Set FTP type, 'I' - Binary; 'A' - ASCII
	#define AT_CMD_SET_FTP_USERNAME 	"AT+CFTPUN=\"%s\"\r\n"			// Set Username
	#define AT_CMD_SET_FTP_PASSWORD 	"AT+CFTPPW=\"%s\"\r\n"			// Set Password
	#define AT_CMD_SET_FTP_GETLIST 		"AT+CFTPLIST=\"%s\"\r\n"		// Get List of Files on FTP server
	#define AT_CMD_SET_FTP_GETFILE 		"AT+CFTPGET=\"%s\",%d\r\n"		// Get a file from FTP server and output to SIO
	#define AT_CMD_SET_FTP_PUTFILE 		"AT+CFTPPUT=\"%s\"\r\n"			// Put a file from SIO to FTP server
	#define AT_CMD_SET_FTP_DELFILE 		"AT+CFTPDELE=\"%s\"\r\n"		// Delete a file from FTP server
	#define AT_CMD_SET_FTP_MKDIR 		"AT+CFTPMKD=\"%s\"\r\n"			// Make directory on FTP server
	#define AT_CMD_SET_FTP_RMDIR 		"AT+CFTPRMD=\"%s\","			// Delete directory from FTP server
#endif


#endif /* SIMCOM_SIMCOM_FTP_COMMAND_H_ */

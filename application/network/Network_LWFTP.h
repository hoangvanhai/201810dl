/**
 * @file Network_LWIP.h
 * @author ThinhNT (tienthinh@gmail.com)
 * @brief Module Network implementation via LWIP library
 * @version 0.1
 * @date 2018-10-17
 * 
 * @copyright Copyright (c) 2018
 * 
 */
#ifndef NETWORK_LWFTP_H
#define NETWORK_LWFTP_H

#include "NetCommon.h"
#include "Network.h"
#include "lwip/opt.h"
#if LWIP_NETCONN
#include <stdio.h>

#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"
#include "ethernetif.h"
#include "board.h"
#ifdef __cplusplus
extern "C" {
#endif

#define LWFTP_CONST_MAX_TRIES 10
#define LWFTP_CONST_BUF_SIZE 512
#define LWFTP_CONST_LINE_SIZE 256 // written chuck for each block data to send

#define LWFTP_FILENAME 	"b.zip\r\n"
#define LWFTP_SERVER_IP 	"192.168.0.104"
#define LWFTP_SERVER_PORT 	21
#define LWFTP_USER_NAME "thinhnt\r\n"
#define LWFTP_PASSWORD "123456a@\r\n"
#define LWFTP_PASV "PASV\r\n"

typedef enum lwftp_results {
  LWFTP_RESULT_OK=0,
  LWFTP_RESULT_INPROGRESS,
  LWFTP_RESULT_LOGGED,
  LWFTP_RESULT_ERR_UNKNOWN,   /** Unknown error */
  LWFTP_RESULT_ERR_ARGUMENT,  /** Wrong argument */
  LWFTP_RESULT_ERR_MEMORY,    /** Out of memory */
  LWFTP_RESULT_ERR_CONNECT,   /** Connection to server failed */
  LWFTP_RESULT_ERR_HOSTNAME,  /** Failed to resolve server hostname */
  LWFTP_RESULT_ERR_CLOSED,    /** Connection unexpectedly closed by remote server */
  LWFTP_RESULT_ERR_TIMEOUT,   /** Connection timed out (server didn't respond in time) */
  LWFTP_RESULT_ERR_SRVR_RESP, /** Server responded with an unknown response code */
  LWFTP_RESULT_ERR_INTERNAL,  /** Internal network stack error */
  LWFTP_RESULT_ERR_LOCAL,     /** Local storage error */
  LWFTP_RESULT_ERR_FILENAME   /** Remote host could not find file */
} lwftp_result_t;


/** LWFTP control connection state */
typedef enum  {
  LWFTP_CLOSED=0,
  LWFTP_CONNECTED,
  LWFTP_USER_SENT,
  LWFTP_PASS_SENT,
  LWFTP_LOGGED,
  LWFTP_TYPE_SENT,
  LWFTP_PASV_SENT,
  LWFTP_RETR_SENT,
  LWFTP_STOR_SENT,
  LWFTP_XFERING,
  LWFTP_DATAEND,
  LWFTP_QUIT,
  LWFTP_QUIT_SENT,
  LWFTP_CLOSING,
} lwftp_state_t;

/**
 * FTP server response code
 */
//Code	Explanation
//100 Series	The requested action is being initiated, expect another reply before proceeding with a new command.
//110	Restart marker replay . In this case, the text is exact and not left to the particular implementation; it must read: MARK yyyy = mmmm where yyyy is User-process data stream marker, and mmmm server's equivalent marker (note the spaces between markers and "=").
//120	Service ready in nnn minutes.
//125	Data connection already open; transfer starting.
//150	File status okay; about to open data connection.
//200 Series	The requested action has been successfully completed.
//202	Command not implemented, superfluous at this site.
//211	System status, or system help reply.
//212	Directory status.
//213	File status.
//214	Help message. Explains how to use the server or the meaning of a particular non-standard command. This reply is useful only to the human user.
//215	NAME system type. Where NAME is an official system name from the registry kept by IANA.
//220	Service ready for new user.
//221	Service closing control connection.
//225	Data connection open; no transfer in progress.
//226	Closing data connection. Requested file action successful (for example, file transfer or file abort).
//227	Entering Passive Mode (h1,h2,h3,h4,p1,p2).
//228	Entering Long Passive Mode (long address, port).
//229	Entering Extended Passive Mode (|||port|).
//230	User logged in, proceed. Logged out if appropriate.
//231	User logged out; service terminated.
//232	Logout command noted, will complete when transfer done.
//234	Specifies that the server accepts the authentication mechanism specified by the client, and the exchange of security data is complete. A higher level nonstandard code created by Microsoft.
//250	Requested file action okay, completed.
//257	"PATHNAME" created.
//300 Series	The command has been accepted, but the requested action is on hold, pending receipt of further information.
//331	User name okay, need password.
//332	Need account for login.
//350	Requested file action pending further information
//400 Series	The command was not accepted and the requested action did not take place, but the error condition is temporary and the action may be requested again.
//421	Service not available, closing control connection. This may be a reply to any command if the service knows it must shut down.
//425	Can't open data connection.
//426	Connection closed; transfer aborted.
//430	Invalid username or password
//434	Requested host unavailable.
//450	Requested file action not taken.
//451	Requested action aborted. Local error in processing.
//452	Requested action not taken. Insufficient storage space in system.File unavailable (e.g., file busy).
//500 Series	Syntax error, command unrecognized and the requested action did not take place. This may include errors such as command line too long.
//501	Syntax error in parameters or arguments.
//502	Command not implemented.
//503	Bad sequence of commands.
//504	Command not implemented for that parameter.
//530	Not logged in.
//532	Need account for storing files.
//534	Could Not Connect to Server - Policy Requires SSL
//550	Requested action not taken. File unavailable (e.g., file not found, no access).
//551	Requested action aborted. Page type unknown.
//552	Requested file action aborted. Exceeded storage allocation (for current directory or dataset).
//553	Requested action not taken. File name not allowed.
//600 Series	Replies regarding confidentiality and integrity
//631	Integrity protected reply.
//632	Confidentiality and integrity protected reply.
//633	Confidentiality protected reply.
//10000 Series	Common Winsock Error Codes[2] (These are not FTP return codes)
//10054	Connection reset by peer. The connection was forcibly closed by the remote host.
//10060	Cannot connect to remote server.
//10061	Cannot connect to remote server. The connection is actively refused by the server.
//10066	Directory not empty.
//10068	Too many users, server is full.
#define FTP_SER_RES_100 "100"
#define FTP_SER_RES_200 "200"		//200 Series	The requested action has been successfully completed.
#define FTP_SER_RES_220 "220"		//220	Service ready for new user.
#define FTP_SER_RES_331	"331"		//331	User name okay, need password.
#define FTP_SER_RES_230	"230"		//230	User logged in, proceed. Logged out if appropriate.

/**
 * Change to directory
 * @param dirpath directory path
 */
lwftp_result_t Network_LWFTP_CWD(const char* dirpath);
/**
 * Create directory in the server
 * @param dirpath directory path
 */
lwftp_result_t Network_LWFTP_MKD(const char* dirpath);

/**
 * Setup LWIP thread for FTP
 * @param ipaddr
 * @param port number (usually 21)
 * @param
 */
lwftp_result_t Network_LWFTP_Start(ip_addr_t ipaddr, int port, const char* usrname, const char* passwd);
/**
 * Send file to server
 * @param dirPath Directory Path in FTP Server
 * @param fileName in sdcard
 */
lwftp_result_t Network_LWFTP_SendFile(const char *dirPath, const char *fileName);

/**
 *
 */
lwftp_result_t Network_LWFTP_SendFile2(const char *local_path, const char *remote_path, const char *file_name);

/**
 * Delete file or folder in FTP server
 * @param path Path to file or folder in FTP server
 */
lwftp_result_t Network_LWFTP_Delete(const char *path);
/**
 * Disconnect from FTP Server
 */
lwftp_result_t Network_LWFTP_Disconnect();
/* Helper function */
/**
 * Get current server ip-address / domain name
 */
ip_addr_t Netwrok_LWIP_Get_ServerIP();
/**
 * Get lwip state
 */
lwftp_state_t Network_LWIP_Get_State();

#endif //LWIP_NETCONN
#ifdef __cplusplus
}
#endif

#endif// NETWORK_LWIP_H

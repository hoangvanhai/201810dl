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
#ifndef NETWORK_MDMFTP_H
#define NETWORK_MDMFTP_H

#include "NetCommon.h"
#include "Network.h"

#include <stdio.h>

#include "fsl_clock_manager.h"
#include "fsl_os_abstraction.h"

#include "board.h"
#include "modem.h"
#include "modem_ftp_client.h"
#include "modem_debug.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MDMFTP_results {
  MDMFTP_RESULT_OK=0,
  MDMFTP_RESULT_INPROGRESS,
  MDMFTP_RESULT_LOGGED,
  MDMFTP_RESULT_ERR_UNKNOWN,   /** Unknown error */
  MDMFTP_RESULT_ERR_ARGUMENT,  /** Wrong argument */
  MDMFTP_RESULT_ERR_MEMORY,    /** Out of memory */
  MDMFTP_RESULT_ERR_CONNECT,   /** Connection to server failed */
  MDMFTP_RESULT_ERR_HOSTNAME,  /** Failed to resolve server hostname */
  MDMFTP_RESULT_ERR_CLOSED,    /** Connection unexpectedly closed by remote server */
  MDMFTP_RESULT_ERR_TIMEOUT,   /** Connection timed out (server didn't respond in time) */
  MDMFTP_RESULT_ERR_SRVR_RESP, /** Server responded with an unknown response code */
  MDMFTP_RESULT_ERR_INTERNAL,  /** Internal network stack error */
  MDMFTP_RESULT_ERR_LOCAL,     /** Local storage error */
  MDMFTP_RESULT_ERR_FILENAME   /** Remote host could not find file */
} MDMFTP_result_t;


/** MDMFTP control connection state */
typedef enum  {
  MDMFTP_CLOSED=0,
  MDMFTP_CONNECTED,
  MDMFTP_USER_SENT,
  MDMFTP_PASS_SENT,
  MDMFTP_LOGGED,
  MDMFTP_TYPE_SENT,
  MDMFTP_PASV_SENT,
  MDMFTP_RETR_SENT,
  MDMFTP_STOR_SENT,
  MDMFTP_XFERING,
  MDMFTP_DATAEND,
  MDMFTP_QUIT,
  MDMFTP_QUIT_SENT,
  MDMFTP_CLOSING,
} MDMFTP_state_t;

/**
 * Change to directory
 * @param dirpath directory path
 */
MDMFTP_result_t Network_MDMFTP_CWD(const char* dirpath);
/**
 * Create directory in the server
 * @param dirpath directory path
 */
MDMFTP_result_t Network_MDMFTP_MKD(const char* dirpath);

/**
 * Setup LWIP thread for FTP
 * @param ipaddr
 * @param port number (usually 21)
 * @param
 */
MDMFTP_result_t Network_MDMFTP_Start(const char *ipaddr, int port, const char* usrname, const char* passwd);

/**
 * Send file to FTP server
 * @param local_path	local path (on FS)
 * @param remote_path	remote path (on FTP server)
 * @param file_name		filename
 * @return
 */
MDMFTP_result_t Network_MDMFTP_SendFile(const char *local_path, const char *remote_path, const char *file_name);
/**
 * Delete file or folder in FTP server
 * @param path Path to file or folder in FTP server
 */
MDMFTP_result_t Network_MDMFTP_Delete(const char *path);
/**
 * Disconnect from FTP Server
 */
MDMFTP_result_t Network_MDMFTP_Disconnect();
/* Helper function */
/**
 * Get current server ip-address / domain name
 */
//char *Netwrok_LWIP_Get_ServerIP();
/**
 * Get lwip state
 */
//MDMFTP_state_t Network_LWIP_Get_State();


#ifdef __cplusplus
}
#endif

#endif// NETWORK_LWIP_H

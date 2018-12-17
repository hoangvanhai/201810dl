/**
 * @file Network_LWFTP_Test.h
 * @author ThinhNT (tienthinh@gmail.com)
 * @brief Simple test for Network_LWFTP Module
 * @version 0.1
 * @date 2018-10-17
 *
 * @copyright Copyright (c) 2018
 *
 */


#include "Network.h"

#define BUFSIZ 1024

#define THINHNTPC
#ifdef THINHNTPC
//#define FILENAME 	"b.zip"
//#define SERVER_IP 	"192.168.1.100"
//#define SERVER_PORT 	21
//#define USER_NAME "thinhnt"
//#define PASSWORD "123456a@"
#define FILENAME 	"b.zip"
#define SERVER_IP 	"27.118.20.209"
#define SERVER_PORT 	21
#define USER_NAME "ftpuser1"
#define PASSWORD "zxcvbnm@12"
#define PASV "PASV"
#else
// Server in Local
#define FILENAME 	"test1.txt"
#define SERVER_IP 	"127.0.0.1"
#define SERVER_PORT 	21
#endif


void Net_LWIP_Echo_ClientCallback(int fd);

void Net_TestFTP(void);

void Net_TestTCPServer(void);


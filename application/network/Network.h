/*
 * network.h
 *
 *  Created on: Dec 19, 2018
 *      Author: PC
 */

#ifndef APPLICATION_NETWORK_H_
#define APPLICATION_NETWORK_H_

#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"
#include "definition.h"
#include <tcp_client.h>
#include <tcp_server.h>
#include <ftp_client.h>
#include <network_cfg.h>

typedef enum ActiveIf {
	NET_IF_NONE 	= 0,
	NET_IF_ETHERNET = 1 << 0,
	NET_IF_WIRELESS = 1 << 1,
	NET_IF_ALL = (NET_IF_ETHERNET | NET_IF_WIRELESS),
}EActiveIf;

typedef struct NetworkStt {
	SComStatus  *status;
	EActiveIf 	activeIf;
	uint8_t		rssi;
	uint8_t		simid[22];
	uint8_t		netid[22];
}SNetworkStt;

void Network_InitTcpModule(SCommon *pCM, SComStatus *pStatus);
void Network_InitFtpModule(SCommon *pCM);

void tcp_client_init(ip_addr_t ip, int port);
void tcp_server_init(int port);

void Network_Register_TcpClient_Notify(NetworkConnNotify func);
void Network_Register_TcpClient_DataEvent(Network_DataEvent evt, NetworkDataEvent func);
void Network_Register_TcpServer_Notify(NetworkConnNotify func);
void Network_Register_TcpServer_DataEvent(Network_DataEvent evt, NetworkDataEvent func);


int Network_TcpClient_Send(const uint8_t *data, int len);
int Network_TcpClient_SendWLength(uint16_t cmd, const uint8_t *data, int len);
int Network_TcpServer_Send(const uint8_t *data, int len);

int Network_FtpClient_Send(const uint8_t *local_path,
		const uint8_t *filename,  uint8_t server);

int ftp_client_init(SCommon *pCM);
void Network_Ftpclient_ClearWaitQueue(bool q1, bool q2);

int Network_GetWirelessStatus(void);

extern TcpClient	 		tcpClient;
extern TcpServer 	 		tcpServer;
extern SNetworkStt	 		netStt;
extern ring_file_handle_t 	g_retryTable[FTP_CLIENT_SERVER_NUM];
extern struct netif eth0;

#endif /* APPLICATION_NETWORK_H_ */

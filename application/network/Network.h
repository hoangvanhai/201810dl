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
#include <common/ring_file.h>


#define NET_IF_ETHERNET 	0x01
#define NET_IF_WIRELESS 	0x02
#define NET_IF_ALL			(NET_IF_ETHERNET | NET_IF_ETHERNET)

typedef struct {
    SCommon 			*sSettings;		// point to setting, to save memory
    uint8_t				u8ActiveIf;
    TcpClient			sTcpClient;
    TcpServer			sTcpServer;
    FtpClient			sFtpClient;
    struct netif 		eth0;
    ring_file_handle_t 	retryTable;

}SNetwork;

void Network_InitModule(SCommon *pCM);
void tcp_client_init(TcpClient *pClient, ip_addr_t ip, int port);
void tcp_server_init(TcpServer *pServer, int port);
int ftp_client_init(FtpClient *pFc, ring_file_handle_t *rfile, SCommon *pCM);

void Network_Register_TcpClient_Notify(NetworkConnNotify func);
void Network_Register_TcpClient_DataEvent(Network_DataEvent evt, NetworkDataEvent func);
void Network_Register_TcpServer_Notify(NetworkConnNotify func);
void Network_Register_TcpServer_DataEvent(Network_DataEvent evt, NetworkDataEvent func);


int Network_TcpClient_Send(const uint8_t *data, int len);
int Network_TcpServer_Send(const uint8_t *data, int len);

int Network_FtpClient_Send(const uint8_t *local_path,
		const uint8_t *filename);
extern ring_file_handle_t 	g_retryTable;
extern SNetwork				g_network;


#endif /* APPLICATION_NETWORK_H_ */

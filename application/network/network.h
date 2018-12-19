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




void Network_InitModule(SCommon *pCM);
void tcp_client_init(ip_addr_t ip, int port);
void tcp_server_init(int port);

void Network_Register_TcpClient_Notify(NetworkConnNotify func);
void Network_Register_TcpClient_DataEvent(Network_DataEvent evt, NetworkDataEvent func);
void Network_Register_TcpServer_Notify(NetworkConnNotify func);
void Network_Register_TcpServer_DataEvent(Network_DataEvent evt, NetworkDataEvent func);

extern TcpClient	 tcpClient;
extern TcpServer 	 tcpServer;


#endif /* APPLICATION_NETWORK_H_ */

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
#ifndef NETWORK_LWTCP_H
#define NETWORK_LWTCP_H

#include "NetCommon.h"
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

#define LWTCP_SERVER_MAX_CLIENT 1

typedef struct {
	//int* fd;
	int idx;
} Tcp_Client_Arg;

typedef enum lwtcp_results {
  LWTCP_RESULT_OK=0,
  LWTCP_RESULT_ERR_BUSY,
  LWTCP_RESULT_ERR_OPENING,
  LWTCP_RESULT_ERR_BINDING,
  LWTCP_RESULT_ERR_LISTENING,
  LWTCP_RESULT_ERR_SERVER_CLOSED,
  LWTCP_RESULT_ERR_CLIENT_DISCONNECT,
  LWTCP_RESULT_ERR_SEND_ERR,
  LWTCP_RESULT_ERR_RECV_ERR,
  LWTCP_RESULT_ERR_TIMEOUT,
  LWTCP_RESULT_ERR_UNKNOWN,   /** Unknown error */
} lwtcp_result_t;


typedef enum _lwtcp_client_events {
	LWTCP_CLIENT_EVENT_CONNECT = 1,
	LWTCP_CLIENT_EVENT_DISCONNECT,
	LWTCP_CLIENT_NETWORK_DOWN,
	EVENT_DATA_RECEIVED
} lwtcp_client_event_t;

// protoype functions for tcp server

lwtcp_result_t Network_LWTCPServer_Start(int port);
lwtcp_result_t Network_LWTCPServer_SetClientStackSize(int _sz);
//lwtcp_result_t Network_LWTCPServer_Set_Callback(ClientThread fn);
lwtcp_result_t Network_LWTCPServer_Set_Callback(Network_DataEvent event, NetworkDataEvent fn);
lwtcp_result_t Network_LWTCPServer_SendToAllClientConnected(void* data, int length);

lwtcp_result_t Network_LWTCP_Close(int clientSocketId);
int Network_LWTCP_Send(int clientSocketfd, const uint8_t *data, uint32_t length);
int Network_LWTCP_Receive(int clientSocketfd, uint8_t *buf, uint32_t max_received_bytes);

// protoype functions for tcp client
lwtcp_result_t Network_LWTCPClientStart(ip_addr_t ip, int port);
lwtcp_result_t Network_LWTCPClientProcess();
lwtcp_result_t Network_LWTCPClientStop();
uint32_t Network_LWTCPClientSendCmd(const uint8_t *data, uint32_t length);
lwtcp_result_t Network_LWTCPClientSetDataEventHandler(Network_DataEvent event, NetworkDataEvent fn);
lwtcp_result_t Network_LWTCPClientSetConnEventHandler(NetworkConnEvent fn);

#endif //LWIP_NETCONN
#ifdef __cplusplus
}
#endif

#endif// NETWORK_LWTCP_H

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
#ifndef NETWORK_LWIP_H
#define NETWORK_LWIP_H
#define USE_DHCP 1
#include "lwip/opt.h"
#if LWIP_NETCONN
#include <stdio.h>
#include "NetCommon.h"
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
extern NetworkConnEvent lwtcp_client_conn_handler;
void Network_LWIP_TCP_Init();
void Network_LWIP_DHCP_Init();
bool Network_LWIP_Is_DHCP_Bound();
bool Network_LWIP_Is_Up();
#endif //LWIP_NETCONN
#ifdef __cplusplus
}
#endif

#endif// NETWORK_LWIP_H

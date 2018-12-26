/*
 * network_cfg.h
 *
 *  Created on: Dec 20, 2018
 *      Author: PC
 */

#ifndef APPLICATION_NETWORK_NETWORK_CFG_H_
#define APPLICATION_NETWORK_NETWORK_CFG_H_

#include <lwip/opt.h>
#include <lwipopts.h>

#define FTP_CLIENT_SERVER_NUM		2
#define FTP_CLIENT_BUFF_SIZE		512
#define TCP_CLIENT_BUFF_SIZE		512
#define TCP_SERVER_BUFF_SIZE		512

#define FTP_CLIENT_ETHERNET_RETRY	5
#define FTP_CLIENT_WIRELESS_RETRY	2


#endif /* APPLICATION_NETWORK_NETWORK_CFG_H_ */

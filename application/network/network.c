/*
 * network.c
 *
 *  Created on: Dec 19, 2018
 *      Author: PC
 */

#include <network.h>
#include "ethernetif.h"
#include "netif.h"
#include <app.h>
#include <common/ring_file.h>

TcpClient	 tcpClient;
uint8_t		 tcpclient_recv_buff[TCP_CLIENT_BUFF_SIZE];

TcpServer 	tcpServer;
uint8_t 	tcpserver_recv_buff[TCP_SERVER_BUFF_SIZE];

FtpClient	ftpClient;
uint8_t 	ftpclient_rx_ctrl_buf[FTP_CLIENT_BUFF_SIZE];
uint8_t 	ftpclient_tx_ctrl_buf[FTP_CLIENT_BUFF_SIZE];
uint8_t 	ftpclient_tx_data_buf[FTP_CLIENT_BUFF_SIZE];

ring_file_handle_t g_retryTable;

APP_TASK_DEFINE(tcp_client_sender, 		TASK_TCP_CLIENT_SENDER_PRIO);
APP_TASK_DEFINE(tcp_client_listen, 		TASK_TCP_CLIENT_LISTEN_PRIO);
APP_TASK_DEFINE(tcp_server_sender, 		TASK_TCP_SERVER_SENDER_PRIO);
APP_TASK_DEFINE(tcp_server_listen,		TASK_TCP_SERVER_LISTEN_PRIO);
APP_TASK_DEFINE(ftp_client_sender,		TASK_FTP_CLIENT_SENDER_PRIO);

task_stack_t	tcp_client_sender_stack[TASK_TCP_CLIENT_SENDER_SIZE/ sizeof(task_stack_t)];
task_stack_t	tcp_client_listen_stack[TASK_TCP_CLIENT_LISTEN_SIZE/ sizeof(task_stack_t)];
task_stack_t	tcp_server_sender_stack[TASK_TCP_SERVER_SENDER_SIZE/ sizeof(task_stack_t)];
task_stack_t	tcp_server_listen_stack[TASK_TCP_SERVER_LISTEN_SIZE/ sizeof(task_stack_t)];
task_stack_t	ftp_client_sender_stack[TASK_FTP_CLIENT_SENDER_SIZE/ sizeof(task_stack_t)];

struct netif eth0;

// default callback function
void Clb_Default_DataEvent(const uint8_t *data, int len);
void Clb_Default_Notify(Network_Status status,
		Network_Interface intf);

void static netif_changed_callback(struct netif* netif);
void static netif_link_changed_callback(struct netif* netif);

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Network_InitModule(SCommon *pCM) {
	tcpip_init(NULL,NULL);

	if(pCM->dev_dhcp == false) {
		netif_add(&eth0, &pCM->dev_ip,
				&pCM->dev_netmask, &pCM->dev_gw,
				NULL, ethernetif_init, tcpip_input);

		netif_set_default(&eth0);

		PRINTF("ip 			%s\r\n", ipaddr_ntoa(&eth0.ip_addr));
		PRINTF("nm			%s\r\n", ipaddr_ntoa(&eth0.netmask));
		PRINTF("gw 			%s\r\n", ipaddr_ntoa(&eth0.gw));

		netif_set_up(&eth0);
	} else {
		ip_addr_t ipaddr, netmask, gw;
		IP4_ADDR(&ipaddr, 0,0,0,0);
		IP4_ADDR(&netmask, 0,0,0,0);
		IP4_ADDR(&gw, 0,0,0,0);
		netif_add(&eth0, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
		netif_set_default(&eth0);


		netif_set_status_callback(&eth0, netif_changed_callback);
		netif_set_link_callback(&eth0, netif_link_changed_callback);
	}

	tcp_client_init(pCM->server_ctrl_ip, pCM->server_ctrl_port);
	tcp_server_init(12345);
//	ftp_client_init(pCM);


}


void static netif_changed_callback(struct netif* netif) {
	WARN("netif_changed_callback\r\n");
}

void static netif_link_changed_callback(struct netif* netif) {
	WARN("netif_link_changed_callback\r\n");
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void tcp_client_init(ip_addr_t ip, int port) {

	tcp_client_configuration(&tcpClient, ip, port, 512, 512, tcpclient_recv_buff);

	osa_status_t result;

    result = OSA_TaskCreate(tcp_client_listener,
                    (uint8_t *)"client_listener",
					TASK_TCP_CLIENT_LISTEN_SIZE,
					tcp_client_listen_stack,
					TASK_TCP_CLIENT_LISTEN_PRIO,
                    (task_param_t)&tcpClient,
                    false,
                    &tcp_client_listen_task_handler);
    if (result != kStatus_OSA_Success)
    {
        ERR("Failed to create tcp_client_listen_task_handler task\r\n\r\n");
    }

    tcpClient.recv_thread = tcp_client_listen_task_handler;

    result = OSA_TaskCreate(tcp_client_sender,
                    (uint8_t *)"client_sender",
					TASK_TCP_CLIENT_SENDER_SIZE,
					tcp_client_sender_stack,
					TASK_TCP_CLIENT_SENDER_PRIO,
                    (task_param_t)&tcpClient,
                    false,
                    &tcp_client_sender_task_handler);
    if (result != kStatus_OSA_Success)
    {
        ERR("Failed to create tcp_client_sender_task_handler task\r\n\r\n");
    }

    tcpClient.send_thread = tcp_client_sender_task_handler;
}




/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */

void tcp_server_init(int port) {

	tcp_server_config(&tcpServer, port, 512, 512, tcpserver_recv_buff);
	osa_status_t result;
    result = OSA_TaskCreate(tcp_server_listener,
                    (uint8_t *)"server_listener",
					TASK_TCP_SERVER_LISTEN_SIZE,
					tcp_server_listen_stack,
					TASK_TCP_SERVER_LISTEN_PRIO,
                    (task_param_t)&tcpServer,
                    false,
                    &tcp_server_listen_task_handler);
    if (result != kStatus_OSA_Success)
    {
        ERR("Failed to create tcp_server_listen_task_handler task\r\n\r\n");
    }

    tcpServer.recv_thread = tcp_server_listen_task_handler;

    result = OSA_TaskCreate(tcp_server_sender,
                    (uint8_t *)"server_sender",
					TASK_TCP_SERVER_SENDER_SIZE,
					tcp_server_sender_stack,
					TASK_TCP_SERVER_SENDER_PRIO,
                    (task_param_t)&tcpServer,
                    false,
                    &tcp_server_sender_task_handler);
    if (result != kStatus_OSA_Success)
    {
        ERR("Failed to create tcp_server_sender_task_handler task\r\n\r\n");
    }

    tcpServer.send_thread = tcp_server_sender_task_handler;

}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int ftp_client_init(SCommon *pCM) {

	ServerInfo server;

	ftp_client_init_handle(&ftpClient,
			ftpclient_tx_ctrl_buf,
			ftpclient_rx_ctrl_buf,
			ftpclient_tx_data_buf);

	server.enable = pCM->ftp_enable1;
	server.ip = pCM->server_ftp_ip1;
	server.port = pCM->server_ftp_port1;
	server.username = (CPU_CHAR*)pCM->ftp_usrname1;
	server.passwd = (CPU_CHAR*)pCM->ftp_passwd1;
	server.prefix = (CPU_CHAR*)pCM->ftp_prefix1;

	ftp_client_add_server(&ftpClient, &server, 0);

	server.enable = 0;	//pCM->ftp_enable2;
	server.ip = pCM->server_ftp_ip2;
	server.port = pCM->server_ftp_port2;
	server.username = (CPU_CHAR*)pCM->ftp_usrname2;
	server.passwd = (CPU_CHAR*)pCM->ftp_passwd2;
	server.prefix = (CPU_CHAR*)pCM->ftp_prefix2;

	ftp_client_add_server(&ftpClient, &server, 1);


	osa_status_t result;
    result = OSA_TaskCreate(ftp_client_sender,
                    (uint8_t *)"client_sender",
					TASK_TCP_CLIENT_SENDER_SIZE,
					ftp_client_sender_stack,
					TASK_TCP_CLIENT_SENDER_PRIO,
                    (task_param_t)&ftpClient,
                    false,
                    &ftp_client_sender_task_handler);
    if (result != kStatus_OSA_Success)
    {
        ERR("Failed to create ftp_client_sender_task_handler task\r\n\r\n");
    }

    ftpClient.send_thread = ftp_client_sender_task_handler;

	ring_file_init(&g_retryTable, "/conf", "retrytable.dat", 5000,
			sizeof(ring_file_record_t));
	LREP("Ring file Init Done!!!\r\n");
	ring_file_print(&g_retryTable);

    return result;
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Network_Register_TcpClient_Notify(NetworkConnNotify func) {
	tcp_client_register_notify(&tcpClient, func);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Network_Register_TcpClient_DataEvent(Network_DataEvent evt, NetworkDataEvent func) {
	tcp_client_register_data_event(&tcpClient, evt, func);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Network_Register_TcpServer_Notify(NetworkConnNotify func) {
	tcp_server_register_notify(&tcpServer, func);

}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Network_Register_TcpServer_DataEvent(Network_DataEvent evt, NetworkDataEvent func) {
	tcp_server_register_data_event(&tcpServer, evt, func);
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int Network_TcpClient_Send(const uint8_t *data, int len) {
	return tcp_client_send_nonblocking(&tcpClient, data, len);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int Network_TcpServer_Send(const uint8_t *data, int len) {
	return tcp_server_send_nonblocking(&tcpServer, data, len);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_Default_Notify(Network_Status status,
		Network_Interface intf) {
	LREP("default notify\r\n");
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_Default_DataEvent(const uint8_t *data, int len) {
	LREP("default data event\r\n");
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
int Network_FtpClient_Send(const uint8_t *local_path,
							const uint8_t *filename) {
	return ftp_add_filename(&ftpClient, local_path, filename);
}














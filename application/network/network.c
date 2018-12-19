/*
 * network.c
 *
 *  Created on: Dec 19, 2018
 *      Author: PC
 */

#include <network.h>
#include "ethernetif.h"
#include <app.h>

TcpClient	 tcpClient;
uint8_t		 tcpclient_recv_buff[512];

TcpServer 	tcpServer;
uint8_t 	tcpserver_recv_buff[512];

const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_index_html[] = "<html><head><title>Congrats!</title></head><body><h1>Welcome to our lwIP HTTP server!</h1><p>This is a small test page, served by httpserver-netconn.</body></html>";
struct netif fsl_netif0;

// default callback function
void Clb_TcpServer_SendDone(const uint8_t *data, int len);
void Clb_TcpServer_RecvData(const uint8_t *data, int len);
void Clb_TcpServer_DataError(const uint8_t *data, int len);
void Clb_TcpServer_Notify(Network_Status status,
		Network_Interface intf);
void Clb_TcpClient_SendDone(const uint8_t *data, int len);
void Clb_TcpClient_RecvData(const uint8_t *data, int len);
void Clb_TcpClient_DataError(const uint8_t *data, int len);
void Clb_TcpClient_Notify(Network_Status status,
		Network_Interface intf);
void http_server_netconn_init();


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
		ip_addr_t fsl_netif0_gw;
		IP4_ADDR(&fsl_netif0_gw, 		192,168,0,254);
		netif_add(&fsl_netif0, &pCM->dev_ip,
				&pCM->dev_netmask, &fsl_netif0_gw,
				NULL, ethernetif_init, tcpip_input);

		netif_set_default(&fsl_netif0);

		PRINTF("ip 			%s\r\n", ipaddr_ntoa(&fsl_netif0.ip_addr));
		PRINTF("nm			%s\r\n", ipaddr_ntoa(&fsl_netif0.netmask));
		PRINTF("gw 			%s\r\n", ipaddr_ntoa(&fsl_netif0.gw));
	}

	http_server_netconn_init();
	tcp_client_init(pCM->server_ctrl_ip, pCM->server_ctrl_port);
	tcp_server_init(12345);
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

	tcp_client_register_notify(&tcpClient, Clb_TcpClient_Notify);
	tcp_client_register_data_event(&tcpClient, Event_DataReceived, Clb_TcpClient_RecvData);
	tcp_client_register_data_event(&tcpClient, Event_DataSendDone, Clb_TcpClient_SendDone);
	tcp_client_register_data_event(&tcpClient, Event_DataError, 	Clb_TcpClient_DataError);

    tcpClient.recv_thread = sys_thread_new("listener",
							tcp_client_listener,
							&tcpClient, 2048, 10);

	tcpClient.send_thread = sys_thread_new("sender",
							tcp_client_sender,
							&tcpClient, 2048, 9);
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
	tcp_server_register_notify(&tcpServer,
			Clb_TcpServer_Notify);
	tcp_server_register_data_event(&tcpServer, Event_DataReceived,
			Clb_TcpServer_RecvData);
	tcp_server_register_data_event(&tcpServer, Event_DataSendDone,
			Clb_TcpServer_SendDone);
	tcp_server_register_data_event(&tcpServer, Event_DataError,
			Clb_TcpServer_DataError);

	tcpServer.recv_thread = sys_thread_new("listener",
							tcp_server_listener,
							&tcpServer, 2048, 10);

	tcpServer.send_thread = sys_thread_new("sender",
							tcp_server_sender,
							&tcpServer, 2048, 9);
}


/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
static void http_server_netconn_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    // Read the data from the port, blocking if nothing yet there.
    // We assume the request (the part we care about) is in one netbuf
    err = netconn_recv(conn, &inbuf);

    PRINTF(".");

    if (err == ERR_OK)
    {
        netbuf_data(inbuf, (void**)&buf, &buflen);

        // Is this an HTTP GET command? (only check the first 5 chars, since
        // there are other formats for GET, and we're keeping it very simple )
        if (buflen>=5 &&
                buf[0]=='G' &&
                buf[1]=='E' &&
                buf[2]=='T' &&
                buf[3]==' ' &&
                buf[4]=='/' )
        {

            // Send the HTML header
            // subtract 1 from the size, since we dont send the \0 in the string
            // NETCONN_NOCOPY: our data is const static, so no need to copy it
            netconn_write(conn, http_html_hdr, sizeof(http_html_hdr)-1, NETCONN_NOCOPY);

            // Send our HTML page
            netconn_write(conn, http_index_html, sizeof(http_index_html)-1, NETCONN_NOCOPY);
        }
    }
    // Close the connection (server closes in HTTP)
    PRINTF("close connection \r\n");
    netconn_close(conn);

    // Delete the buffer (netconn_recv gives us ownership,
    // so we have to make sure to deallocate the buffer)
    netbuf_delete(inbuf);
}

/*!
 * @brief The main function containing server thread.
 */
static void
http_server_netconn_thread(void *arg)
{
    struct netconn *conn, *newconn;
    err_t err;
    LWIP_UNUSED_ARG(arg);
    netif_set_up(&fsl_netif0);
    // Create a new TCP connection handle
    conn = netconn_new(NETCONN_TCP);
    LWIP_ERROR("http_server: invalid conn", (conn != NULL), return;);

    // Bind to port 80 (HTTP) with default IP address
    netconn_bind(conn, NULL, 80);

    // Put the connection into LISTEN state
    netconn_listen(conn);
    PRINTF("listening ...\r\n");

    do {
        err = netconn_accept(conn, &newconn);
        PRINTF("accepted \r\n");
        if (err == ERR_OK) {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
    } while(err == ERR_OK);
    LREP("http_server_netconn_thread: netconn_accept received error %d, shutting down", err);
    netconn_close(conn);
    netconn_delete(conn);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void http_server_netconn_init()
{
    sys_thread_new("http_server_netconn",
    		http_server_netconn_thread, NULL,
    		2048, 7);

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
void Clb_TcpServer_SendDone(const uint8_t *data, int len) {
	LREP("server send done len = %d\r\n", len);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TcpServer_RecvData(const uint8_t *data, int len) {
	LREP("server recv data len = %d\r\n", len);
}
/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TcpServer_DataError(const uint8_t *data, int len) {
	LREP("server data failed len = %d\r\n", len);
}

/*****************************************************************************/
/** @brief
 *
 *
 *  @param
 *  @return Void.
 *  @note
 */
void Clb_TcpServer_Notify(Network_Status status,
		Network_Interface intf) {
	switch(status) {
	case Status_Connected:
		LREP("Event connected\r\n");
		break;
	case Status_Disconnected:
		LREP("Event disconnected\r\n");
		break;
	case Status_Network_Down:
		LREP("Event network down\r\n");
		break;
	case Status_Connecting:
		LREP("Event connecting\r\n");
		break;
	default:
		break;
	}
}



void Clb_TcpClient_SendDone(const uint8_t *data, int len) {
	LREP("client send done len = %d\r\n", len);
}


void Clb_TcpClient_RecvData(const uint8_t *data, int len) {
	LREP("client recv data len = %d\r\n", len);
}

void Clb_TcpClient_DataError(const uint8_t *data, int len) {
	LREP("client data failed len = %d\r\n", len);
}


void Clb_TcpClient_Notify(Network_Status status,
		Network_Interface intf) {
	switch(status) {
	case Status_Connected:
		LREP("Event connected\r\n");
		break;
	case Status_Disconnected:
		LREP("Event disconnected\r\n");
		break;
	case Status_Network_Down:
		LREP("Event network down\r\n");
		break;
	case Status_Connecting:
		LREP("Event connecting\r\n");
		break;
	default:
		break;
	}
}
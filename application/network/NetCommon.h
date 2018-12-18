/**
 * @file NetworkCommon.h
 * @author ThinhNT (tienthinh@gmail.com)
 * @brief Module Network Configuration
 * @version 0.1
 * @date 2018-10-17
 *
 * @copyright Copyright (c) 2018
 *
 */
#ifndef NET_COMMON_H
#define NET_COMMON_H

#ifndef LWIP_NETCONN
#define LWIP_NETCONN 1
#endif



#define NETMODULE_ETH_ENABLE	ON
#define NETMODULE_3G_ENABLE		ON

//#ifndef NETMODULE_DEBUG
#define NETMODULE_DEBUG         LWIP_DBG_ON
///#endif

//void DEBUG(const char* msg);

#define COLOR_KNRM  "\x1B[0m"
#define COLOR_KRED  "\x1B[31m"
#define COLOR_KGRN  "\x1B[32m"
#define COLOR_KYEL  "\x1B[33m"
#define COLOR_KBLU  "\x1B[34m"
#define COLOR_KMAG  "\x1B[35m"
#define COLOR_KCYN  "\x1B[36m"
#define COLOR_KWHT  "\x1B[37m"

#define NET_DEBUG_ENABLE 	1

#if NET_DEBUG_ENABLE
#define NET_DEBUG(s, args...)			PRINTF(COLOR_KGRN "[NET] " s "\r\n" COLOR_KNRM, ##args)
#define NET_DEBUG_TCP(s, args...)		PRINTF(COLOR_KBLU "[TCP] " s "\r\n" COLOR_KNRM, ##args)
#define NET_DEBUG_FTP(s, args...)		PRINTF(COLOR_KCYN "[FTP] " s "\r\n" COLOR_KNRM, ##args)

#define NET_DEBUG_RAW(s, args...)     	PRINTF(s, ##args)
#define NET_DEBUG_CRITICAL(s, args...) 	PRINTF(COLOR_KRED "[NET][ERROR]:%s:%d " s "\r\n" COLOR_KNRM, __FILE__, __LINE__, ##args)
#define NET_DEBUG_ERROR(s, args...)   	PRINTF(COLOR_KMAG "[NET][ERROR]: " s "\r\n" COLOR_KNRM, ##args)
#define NET_DEBUG_WARNING(s, args...) 	PRINTF(COLOR_KYEL "[NET][WARNING] " s "\r\n" COLOR_KNRM, ##args)
#define NET_DEBUG_LINE(s, args...) 		PRINTF(COLOR_KNRM "[NET][%s:%d] " s "\r\n" COLOR_KNRM, __FILE__, __LINE__, ##args)
#else
#define NET_DEBUG(s, args...)
#define NET_DEBUG_TCP(s, args...)
#define NET_DEBUG_FTP(s, args...)

#define NET_DEBUG_RAW(s, args...)
#define NET_DEBUG_CRITICAL(s, args...)
#define NET_DEBUG_ERROR(s, args...)
#define NET_DEBUG_WARNING(s, args...)
#define NET_DEBUG_LINE(s, args...)

#endif


#ifndef NET_MAX_MTU
#define NET_MAX_MTU				1500
#endif

#ifndef NET_MAX_TCP_BUF_SIZE
#define NET_MAX_TCP_BUF_SIZE	512
#endif


#define DEFAULT_FTP_FOLDER_PATH "/home/ftpuser1/test/manhbt"
//#define ENABLE_FTP_FILE_TEST "AG_SGCE_KHI001_20181107105400.txt"

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef LOG_ERROR
#define LOG_ERROR PRINTF
#endif
/**
 * Network Commnucation Configuration
 **/
typedef enum eNetComConfig{
    NET_COM_USING_HW = 1,//!< NET_COM_USING_HW
    NET_COM_IP_METHOD,   //!< NET_COM_IP_METHOD
    NET_COM_RECV_BUFF,   //!< NET_COM_RECV_BUFF
    NET_COM_SEND_BUFF,   //!< NET_COM_SEND_BUFF
    NET_COM_FTP_USER,    //!< NET_COM_FTP_USER
    NET_COM_FTP_PASSWD   //!< NET_COM_FTP_PASSWD
} ENetComConfig;

/**
 * Network Status 
 **/
typedef enum eNetStatus {
    NET_ERR_NONE = 0, 
    NET_ERR_MODULE_NOT_INIT, 
    NET_ERR_PARAM, 
	NET_ERR_FILENAME,
    NET_ERR_SEND_FAILD, 
    NET_ERR_LOST_CONNECTION, 
    NET_ERR_BUSY, 
	NET_ERR_LWIP_SERVER,
	NET_ERR_LWIP_FTPCLIENT,
    NET_ERR_UNKNOWN 
} NetStatus;

/**
 * Net interface status
 **/
typedef enum eNetIF {
    NET_IF_ETHERNET = 1,
    NET_IF_WIRELESS,
    NET_IF_UNKNOWN
} NetIF;

/**
 * Connection status 
 **/
typedef enum eNetConStat {
    NET_CON_CONNECTED,
    NET_CON_DISCONNECTED,
    NET_CON_ERR_UNKNOWN
} NetConStat;

typedef struct sNetStt {
    NetIF NetIF;
    NetConStat NetConStat;
} SNetStt;

#define TCP_DATA_SIZE 1024
typedef struct sNetData {
	int len;
	char *buf;
}SNetData;

#define FTP_DATA_SIZE 128
typedef struct sFTPData {
	char dirPath[128];
	char fileName[128];
} SFTPData;

typedef void (*ClientThread)(int client_socket_fd);



typedef enum ENetwork_Interface {
	Interface_Ethernet,
	Interface_Wireless,
	Interface_All
} Network_Interface;
typedef enum ENetwork_ConnEvent {
	NetConn_Disconnected = 0,
	NetConn_Connected,
	NetConn_Network_Down
} Network_ConnEvent;


typedef enum ENetwork_DataEvent {
	NetData_Received = 0,
	NetData_SendDone,
	NetData_Error
} Network_DataEvent;


typedef enum  {
	TCP_SRV_DATA_RECEIVED = 0,
	TCP_SRV_CLIENT_DISCONNECTED,
	TCP_SRV_CLIENT_CONNECTED,

} NetTcpServerEventType_t;

// task prio
#define TASK_NETWORK_FTPCLIENT_TX_PRIO      11U
// task size
#define TASK_NETWORK_FTPCLIENT_TX_SIZE   	2048

// task prio
#define TASK_NETWORK_TCPCLIENT_PRIO      	10U
// task size
#define TASK_NETWORK_TCPCLIENT_SIZE   		2048

// task prio
#define TASK_NETWORK_TCPSERVER_PRIO      	13U
// task size
#define TASK_NETWORK_TCPSERVER_SIZE   		2048

#define TASK_CLIENT_CONNECTED_PRIO      	12U

#define TASK_CLIENT_CONNECTED_SIZE 			1024

//typedef void (*NetTcpServerEvent)(NetTcpServerEventType_t type, void* data, int length);
typedef void (*NetworkDataEvent)(const char* data, int length);
typedef void (*NetworkConnEvent)(Network_ConnEvent event, Network_Interface interface);



#endif //NET_COMMON_H

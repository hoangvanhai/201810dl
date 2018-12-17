#include "Network_LWTCP.h"
// VARIALES for TCP Server application
static int parentfd; /* parent socket */
//static int childfd; /* child socket */
//static int portno; /* port to listen on */
static int clientlen; /* byte size of client's address */
static struct sockaddr_in serveraddr; /* server's addr */
static struct sockaddr_in clientaddr; /* client addr */
static int server_port = 0;
//static char buf[BUFSIZE]; /* message buffer */
//static char *hostaddrp; /* dotted decimal host addr string */
static int optval; /* flag value for setsockopt */
//static int n; /* message byte size */
static bool tcp_server_running = false;
static int number_of_connected_clients = 0;
// client callback
//static ClientThread lwtcp_server_client_callback=NULL;
static NetworkDataEvent lwtcp_server_on_data_received_cb = NULL;
static NetworkDataEvent lwtcp_server_on_data_sent_cb = NULL;
static NetworkDataEvent lwtcp_server_on_error_cb = NULL;

static task_handler_t taskHandler[LWTCP_SERVER_MAX_CLIENT];
static task_stack_t *stackMem[LWTCP_SERVER_MAX_CLIENT] ;
static bool clientThreadActivate[LWTCP_SERVER_MAX_CLIENT];
static int childfds[LWTCP_SERVER_MAX_CLIENT];

mutex_t tcp_Mutex;
osa_status_t status_;
int client_connected_stack_size = TASK_CLIENT_CONNECTED_SIZE;

OSA_TASK_DEFINE(network_tcpclient, TASK_NETWORK_TCPCLIENT_SIZE);

// VARIALES for TCP Client Application
//static char* lwftp_curdir;
static bool tcp_client_running = false;
static char* tcp_server_ip;
static int tcp_server_port;
struct timeval tcp_client_tv;
static int tcp_client_socket = -1;
static struct sockaddr_in tcp_server_ctrl;//, server_dat;
// client callback
NetworkConnEvent lwtcp_client_conn_handler=NULL;
static NetworkDataEvent lwtcp_client_data_received_handler=NULL;
static NetworkDataEvent lwtcp_client_data_sent_handler=NULL;
static NetworkDataEvent lwtcp_client_data_error_handler=NULL;


/**
 * Set TCP Client Data Event handler
 * @param event (RECEIVED, SENDDONE, ERROR)
 * @param fn
 * @return
 */
lwtcp_result_t Network_LWTCPClientSetDataEventHandler(Network_DataEvent event, NetworkDataEvent fn)
{
	switch (event) {
	case NetData_Received:
		lwtcp_client_data_received_handler = fn;
		break;
	case NetData_SendDone:
		lwtcp_client_data_sent_handler = fn;
		break;
	case NetData_Error:
		lwtcp_client_data_error_handler = fn;
	}
}
/**
 * Set TCP Client Connection Event Handler
 * @param fn
 * @return
 */
lwtcp_result_t Network_LWTCPClientSetConnEventHandler(NetworkConnEvent fn)
{
	lwtcp_client_conn_handler = fn;
	return LWTCP_RESULT_OK;
}


//lwtcp_result_t Network_LWTCPServer_Set_Callback(ClientThread fn)
//{
//	lwtcp_server_client_callback = fn;
//	return LWTCP_RESULT_OK;
//}

lwtcp_result_t Network_LWTCPServer_Set_Callback(Network_DataEvent event, NetworkDataEvent fn)
{
	switch (event) {
		case NetData_Received:
			lwtcp_server_on_data_received_cb = fn;
			break;
		case NetData_SendDone:
			lwtcp_server_on_data_sent_cb = fn;
			break;
		case NetData_Error:
			lwtcp_server_on_error_cb = fn;
			break;
		default:
			return LWTCP_RESULT_ERR_UNKNOWN;
	}
	return LWTCP_RESULT_OK;
}

/*
 * Starts a new thread with priority "prio" that will begin its execution in the
 * function "thread()". The "arg" argument will be passed as an argument to the
 * thread() function. The argument "ssize" is the requested stack size for the
 * new thread. The id of the new thread is returned. Both the id and the
 * priority are system dependent.
 */

static sys_thread_t lwtcp_create_thread(task_handler_t taskHandler, task_stack_t* stackMem, const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    int fd = *((int*)(arg));
    NET_DEBUG_TCP("fd = %d\r\n", fd);
    osa_status_t error = OSA_TaskCreate((task_t)thread ,(uint8_t*) name,(uint16_t) stacksize, stackMem,prio,(task_param_t)arg,false,&taskHandler);
    if(error == kStatus_OSA_Success)
        return taskHandler;
    else
        return (sys_thread_t)0;
}

void tcp_connected_client_thread(void* arg) {
	OS_ERR err;
	osa_status_t status_;
	size_t buf_size = NET_MAX_TCP_BUF_SIZE;
	uint8_t *msg = OSA_FixedMemMalloc(buf_size);
	int ret, recv_byte = 0;
	if(!msg)
	{
		NET_DEBUG_ERROR("Unable to allocate memory for tcp_connected_client_thread");
		return;
	}
	Tcp_Client_Arg* tcp_arg = (Tcp_Client_Arg*)arg;
//	/int fd = tcp_arg->fd;
	int idx = tcp_arg->idx; // idx will not change but childfd will be changed
	NET_DEBUG_TCP("Start tcp_connected_client_thread fd=%d, idx=%d ", childfds[idx], idx);
	while (true) {
		//NET_DEBUG_TCP("\r\n client_thread[%d] call clientCallbac(%d) \r\n", idx, childfds[idx]);
		if (clientThreadActivate[idx]) {
//			if (lwtcp_server_client_callback != NULL) {
//				lwtcp_server_client_callback(childfds[idx]);
//			} else {
//				NET_DEBUG_TCP("ClientCallback not exist\r\n");
//			}
//			OSA_MutexLock(&tcp_Mutex, OSA_WAIT_FOREVER);

			ret = recv(childfds[idx], msg, buf_size, 0);
			if (ret > 0) {
				recv_byte  = 0;
				while ((ret > 0) && (recv_byte < buf_size)) {
					recv_byte += ret;
					ret = recv(childfds[idx], &msg[recv_byte], buf_size-recv_byte, 0);
//					NET_DEBUG_TCP("Read(childfds[%d] %d bytes\r\n",idx, ret);
				}

				NET_DEBUG_TCP("Read(childfds[%d] %d total bytes",idx, recv_byte);

				if (lwtcp_server_on_data_received_cb != NULL) {
					lwtcp_server_on_data_received_cb(msg, recv_byte);
				} else {
					NET_DEBUG_TCP("ClientCallback not exist");
				}
				recv_byte = 0;
			} else {

				if(ret == 0) {
					number_of_connected_clients -= 1;
					clientThreadActivate[idx] = false;
					if (childfds[idx] > 0) {
						NET_DEBUG_WARNING("Client ID %d disconnected!", childfds[idx]);
						close(childfds[idx]);
//						if(lwtcp_server_on_error_cb != NULL) {
//							lwtcp_server_on_error_cb(TCP_SRV_CLIENT_DISCONNECTED, NULL, 0);
//						}
					}
				}
//				NET_DEBUG_TCP("read %d bytes", ret);
			}
//				number_of_connected_clients -= 1;
//			clientThreadActivate[idx] = false;
//			if (childfds[idx] > 0)
//				close(childfds[idx]);
//			NET_DEBUG_TCP("Number_of_connected_clients changed to %d\r\n", number_of_connected_clients);
//			OSA_MutexUnlock(&tcp_Mutex);
//			NET_DEBUG_TCP("Client_thread[%d] call clientCallbac(%d) \r\n", idx, childfds[idx]);

		} else {
//			NET_DEBUG_TCP("Client_thread[%d] is not active \r\n", idx);
			//OSA_TimeDelay(2000);
		}
		// Close socket
		OSA_TimeDelay(100);
	}
	//NET_DEBUG_TCP("MemFree stackMem %d\r\n", stackMem);
	/* Task body ... do work! */
//	if (tcp_arg->stackMem) {
//		NET_DEBUG_TCP("MemFree stackMem %d\r\n", tcp_arg->stackMem);
//		status_ = OSA_MemFree((void*)tcp_arg->stackMem);
//		if (status_ == kStatus_OSA_Success) {
//			NET_DEBUG_TCP("OSA_Memfree OK\r\n");
//		} else {
//			NET_DEBUG_TCP("OSA_Memfree NOK\r\n");
//		}
//	}
//	if (stackMem) {
//		NET_DEBUG_TCP("MemFree stackMem %d\r\n", tcp_arg->stackMem);
//		status_ = OSA_MemFree((void*)stackMem);
//		if (status_ == kStatus_OSA_Success) {
//			NET_DEBUG_TCP("OSA_Memfree OK\r\n");
//		} else {
//			NET_DEBUG_TCP("OSA_Memfree NOK\r\n");
//		}
//	}
//	if (tcp_arg->taskHandler) {
//		NET_DEBUG_TCP("OSA_TaskDestroy taskHandler %d, %s\r\n", tcp_arg->taskHandler);
//		status_ =  OSA_MemFree((void*)tcp_arg->taskHandler);
//		//status_ = OSA_TaskDestroy(tcp_arg->taskHandler);
//		if (status_ == kStatus_OSA_Success) {
//			NET_DEBUG_TCP("OSA_TaskDestroy OK\r\n");
//		} else {
//			NET_DEBUG_TCP("OSA_TaskDestroy NOK\r\n");
//		}
//	}
//	OSTaskDel(0, &err);
//	if (tcp_arg->taskHandler) {
//		NET_DEBUG_TCP("OSA_TaskDestroy taskHandler %d, %s\r\n", tcp_arg->taskHandler);
//		//status_ =  OSA_MemFree((void*)tcp_arg->taskHandler);
//		status_ = OSA_TaskDestroy(tcp_arg->taskHandler);
//		if (status_ == kStatus_OSA_Success) {
//			NET_DEBUG_TCP("OSA_TaskDestroy OK\r\n");
//		} else {
//			NET_DEBUG_TCP("OSA_TaskDestroy NOK\r\n");
//		}
//	}

	//OSA_TaskDestroy(tcp_arg->taskHandler);
}

static void tcp_server_thread(void *arg)
{
	// Do not allow create multiple TCP Server
	tcp_server_running = true;
	sys_thread_t threadID;
	lwtcp_result_t ret = LWTCP_RESULT_OK;
	/*
	 * socket: create the parent socket
	 */
	parentfd = socket(AF_INET, SOCK_STREAM, 0);
	if (parentfd < 0) {
		perror("ERROR opening socket");
		ret = LWTCP_RESULT_ERR_OPENING;
		return;
	}
	/* setsockopt: Handy debugging trick that lets
	 * us rerun the server immediately after we kill it;
	 * otherwise we have to wait about 20 secs.
	 * Eliminates "ERROR on binding: Address already in use" error.
	 */
	optval = 1;
	setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
			(const void *)&optval , sizeof(int));

	/*
	 * build the server's Internet address
	 */
	bzero((char *) &serveraddr, sizeof(serveraddr));

	/* this is an Internet address */
	serveraddr.sin_family = AF_INET;

	/* let the system figure out our IP address */
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* this is the port we will listen on */
	serveraddr.sin_port = htons((unsigned short)server_port);

	/*
	 * bind: associate the parent socket with a port
	 */
	if (bind(parentfd, (struct sockaddr *) &serveraddr,
			sizeof(serveraddr)) < 0) {
		ret = LWTCP_RESULT_ERR_OPENING;
		perror("ERROR on binding");
	}

	/*
	 * listen: make this socket ready to accept connection requests
	 */
	if (listen(parentfd, LWTCP_SERVER_MAX_CLIENT) < 0) /* allow 2 requests to queue up */ {
		perror("ERROR on listen");
		ret = LWTCP_RESULT_ERR_LISTENING;
	}
	NET_DEBUG_TCP("TCPServer Start Result = %d\r\n", ret);


	OS_ERR err;
	//sys_thread_t threadID;
	Tcp_Client_Arg tcp_arg[2];
	int childfd;
	int idx = 0, i;
	struct timeval tv;
	//bool threadClientAvailable = false;
	/* Local variables */
	// These task handler & stackmem are allocated for tcp_client_thread created later
    static task_handler_t taskHandler[LWTCP_SERVER_MAX_CLIENT];
    osa_status_t error;
    static task_stack_t *stackMem[LWTCP_SERVER_MAX_CLIENT] ;

    for (i = 0; i < LWTCP_SERVER_MAX_CLIENT; i++) {
    	char tmpStr[128];
    	taskHandler[i] = (task_handler_t)OSA_MemAlloc(sizeof(OS_TCB));
    	if(!taskHandler[i])
    	{
    		NET_DEBUG_TCP("OSA_MemAlloc taskHandler[%d] failed\r\n", i);
    		return;
    	} else {
    		NET_DEBUG_TCP("OSA_MemAlloc taskHandler[%d] OK = %d\r\n", i, taskHandler);
    	}

    	stackMem[i] =  (task_stack_t *)OSA_MemAlloc((size_t)1024);
		if(!stackMem[i])
		{
			NET_DEBUG_TCP("OSA_MemAlloc stackMem[%d] failed\r\n", i);
			return;
		} else {
			NET_DEBUG_TCP("OSA_MemAlloc stackMem[%d] OK = %d\r\n", i, stackMem);
		}
		clientThreadActivate[i] = false;
		tcp_arg[i].idx = i;
		sprintf(tmpStr,"Client Thread %d\r\n", tcp_arg[i].idx);
		PRINTF(tmpStr);
		threadID = lwtcp_create_thread(taskHandler[i], stackMem[i],
				tmpStr, tcp_connected_client_thread, &tcp_arg[i], client_connected_stack_size, TASK_CLIENT_CONNECTED_PRIO);
    }
	/* Do something with ‘p_arg’ */
	/* Task initialization */

	NET_DEBUG_TCP("Start TCP SERVER THREAD\r\n");
	status_ = OSA_MutexCreate(&tcp_Mutex);
	if (status_ != kStatus_OSA_Success) {
		NET_DEBUG_TCP("tcp_Mutex creation failed\r\n");
	}

	tcp_server_running = true;
	tv.tv_sec = 10; //dkm co gi do sai sai o day khi dung lwip 3000s ma nhu la 3s
	tv.tv_usec = 10000;
	// set timeout of socket

	while (tcp_server_running == true) {
		//NET_DEBUG_TCP("TCP_SERVER is RUNING\r\n");
		OSA_TimeDelay(300);
		//threadClientAvailable = false;
		OSA_MutexLock(&tcp_Mutex, OSA_WAIT_FOREVER);
		if (number_of_connected_clients < LWTCP_SERVER_MAX_CLIENT) {
			// Check which IDX to be use
			for (idx = 0; idx < LWTCP_SERVER_MAX_CLIENT; idx++) {
				if (clientThreadActivate[idx] == false) {
					break;
				}
			}
			OSA_MutexUnlock(&tcp_Mutex);
			if (idx < LWTCP_SERVER_MAX_CLIENT) {
				// Wait for new connection and for new thread for new client connection
				/*
				 * accept: wait for a connection request
				 */
				childfd = accept(parentfd, (struct sockaddr *) &clientaddr, &clientlen);

				if (childfd < 0) {
					perror("ERROR on accept");
					NET_DEBUG_ERROR("ERROR on accept");
					continue;
				}
				else {
					setsockopt(childfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
					NET_DEBUG_WARNING("Client ID %d connected!", childfd);

					OSA_MutexLock(&tcp_Mutex, OSA_WAIT_FOREVER);
					clientThreadActivate[idx] = true;
					childfds[idx] = childfd;
					number_of_connected_clients += 1;
					OSA_MutexUnlock(&tcp_Mutex);
//					if(lwtcp_server_on_data_sent_cb != NULL)
//					{
//						lwtcp_server_on_data_sent_cb(TCP_SRV_CLIENT_CONNECTED, NULL, 0);
//					}
					//NET_DEBUG_TCP("number_of_connected_clients changed to %d\r\n", number_of_connected_clients);
				}
			} else {
				NET_DEBUG_TCP("idx(%d) < LWTCP_SERVER_MAX_CLIENT(%d)\r\n", idx, LWTCP_SERVER_MAX_CLIENT);
			}
		} else {
			OSA_MutexUnlock(&tcp_Mutex);
		}

	}
	/* Task body ... do work! */
	OSTaskDel((OS_TCB *)0, &err);
}

lwtcp_result_t Network_LWTCPServer_SetClientStackSize(int _sz)
{
	client_connected_stack_size = _sz;
}
lwtcp_result_t Network_LWTCPServer_Start(int portno) {

	sys_thread_t threadID;
	server_port = portno;
//	lwtcp_result_t ret;
	threadID = sys_thread_new("tcp_server_thread", tcp_server_thread, NULL, TASK_NETWORK_TCPSERVER_SIZE, TASK_NETWORK_TCPSERVER_PRIO);
	if (threadID == NULL) {
		NET_DEBUG_TCP("Server thread Created failed, ID = %d\r\n", threadID);
		return LWTCP_RESULT_ERR_UNKNOWN;
	} else {
		NET_DEBUG_TCP("Server thread created OK, ID = %d\r\n", threadID);
		tcp_server_running = true;
		return LWTCP_RESULT_OK;
	}

}


lwtcp_result_t Network_LWTCPServer_SendToAllClientConnected(void* data, int length)
{
	lwtcp_result_t result = LWTCP_RESULT_OK;
	int ret = -1;

	for (int idx = 0; idx < LWTCP_SERVER_MAX_CLIENT; idx ++) {
		if(clientThreadActivate[idx] == true) {
			ret = Network_LWTCP_Send(childfds[idx], data, length);
			if(ret != length) {
				NET_DEBUG_ERROR("Send To client id %d (sock: %d) error!!!", idx, childfds[idx]);
				result = LWTCP_RESULT_ERR_SEND_ERR;
				break;
			}
		}
	}

	if (result == LWTCP_RESULT_OK) {
		// Invoke send done callback;
		if(lwtcp_server_on_data_sent_cb != NULL) {
			lwtcp_server_on_data_sent_cb(data, length);
		}

	} else {
		// Invoke Error callback
		if(lwtcp_server_on_data_sent_cb != NULL) {
			lwtcp_server_on_data_sent_cb(data, ret);
		}
	}

	return result;
}

/**
 * Send to client
 * @param clientSocketfd
 * @param data
 * @param length
 * @return
 */
int Network_LWTCP_Send(int clientSocketfd, const uint8_t *data, uint32_t length)
{
	return write(clientSocketfd, data, length);
}
/**
 * Receive from socket a number of bytes
 * @param clientSocketfd socket to receive
 * @param buf buffer to receive
 * @param max_received_bytes max number of bytes to receive
 * @return
 */
int Network_LWTCP_Receive(int clientSocketfd, uint8_t *buf, uint32_t max_received_bytes) {
	return recv(clientSocketfd, buf, max_received_bytes, 0);
}


//static int lwtcp_receive_ctrl_data(int sock, char* buf, int bufsize) {
//	int resp_len = 0, total_len = 0;
//	int max_receive_byte = bufsize;
//	do {
//		memset(reply_msg, 0x00, sizeof(reply_msg));
//		max_receive_byte -= resp_len;
//		resp_len = recv(sock, reply_msg, max_receive_byte, 0);
//		total_len += resp_len;
//		if (resp_len > 0)
//			NET_DEBUG_TCP("reply_msg (len = %d):  %s\r\n", resp_len, reply_msg);
//
//	} while (resp_len > 0 && resp_len < lwftp_bufsize);
//	return total_len;
//}

static void tcp_client_connect() {
	int ret;
	// Connect to server
	while (1) {
		if (tcp_client_socket != -1) {
				close(tcp_client_socket);
		}
		tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (tcp_client_socket == -1) {
			perror("Could not create socket");
			tcp_client_running = false;
			//return LWTCP_RESULT_ERR_OPENING;
		} else {
			NET_DEBUG_TCP("TCPClient Thread: Socket id: %d created successfully\r\n, ");
		}
		tcp_client_tv.tv_sec = 10000; //dkm co gi do sai sai o day khi dung lwip 3000s ma nhu la 3s
		tcp_client_tv.tv_usec = 10000;
		// set connection keep alive

		setsockopt(tcp_client_socket, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
//		setsockopt(tcp_client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char* )&tcp_client_tv, sizeof tcp_client_tv);
		tcp_server_ctrl.sin_addr.s_addr = inet_addr(tcp_server_ip);
		tcp_server_ctrl.sin_family = AF_INET;
		tcp_server_ctrl.sin_port = htons(tcp_server_port);
		ret = connect(tcp_client_socket, (struct sockaddr * )&tcp_server_ctrl,
				sizeof(tcp_server_ctrl));
		if (ret < 0) {
			NET_DEBUG_TCP("tcp_client_thread Connection failed, ret = %d\r\n", ret);
			tcp_client_running = false;
			OSA_TimeDelay(5000);
		} else {
			NET_DEBUG_TCP("tcp_client_thread Connected, ret = %d\r\n", ret);
			// TODO: connected
			tcp_client_tv.tv_sec = 10;
			tcp_client_tv.tv_usec = 10000;
			setsockopt(tcp_client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char* )&tcp_client_tv, sizeof tcp_client_tv);
			ret = LWTCP_RESULT_OK;
			tcp_client_running = true;
			break;
		}
	}
}

#if 0
static void tcp_client_thread(void *arg) {
	int ret = -1;
	int optval = 1;
	SNetData netData;
	int maxLen = 1024;
	int len;

	int error = 0;
	//len = sizeof (error);
	//ret = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);

	NET_DEBUG_TCP("tcp_client_thread start\r\n");


	// Connect to server
	tcp_client_connect();
	// TODO: update callback here

	while (1) {
		// TODO: Read from server if connected
		netData.buf = OSA_MemAlloc(maxLen);
		ASSERT(netData.buf!=NULL);

		memset(netData.buf, 0x00, maxLen);
		netData.len = read(tcp_client_socket, netData.buf, maxLen);
		if (netData.len > 0) {
			// TODO: do something here
			NET_DEBUG_TCP("TCP Client received %d bytes:\r\n%s\r\n", netData.len, netData.buf);
			// callback
			if (lwtcp_client_data_received_handler != NULL) {
				lwtcp_client_data_received_handler(netData.buf, netData.len);
			} else {
				len = Network_LWTCPClientSendCmd(netData.buf, netData.len);
				NET_DEBUG_TCP("TCP Client echo %d bytes:\r\nn", len);
			}
		}
		OSA_MemFree(netData.buf);
		// If not connected try to reconnect
		OSA_TimeDelay(1000);
		// Check wether the socket is problem;
		len = sizeof (error);
		ret = getsockopt (tcp_client_socket, SOL_SOCKET, SO_ERROR, &error, &len);
		if (ret != 0) {
			/* there was a problem getting the error code */
			NET_DEBUG_TCP("TCPClient Thread error getting socket error code: %s\n", strerror(ret));
			//return;
		}

		if (error != 0) {
			/* socket has a non zero error status_ */
			NET_DEBUG_TCP("TCPClient Thread socket error: %s\n", strerror(error));
		}
		if (ret != 0 || error != 0) {
			tcp_client_connect();// reconnect if error
		}
	}
}
#else


void network_tcpclient(task_param_t param)
{
	NET_DEBUG_TCP("Thread network_tcpclient started\r\n");
	//	int ret = -1;
	OS_ERR        err;
	void         *p_msg;
	OS_MSG_SIZE   msg_size;
	CPU_TS        ts;
	SNetData 	 *pNetData;
	int optval = 1;
	size_t 		tcp_client_buf_size = NET_MAX_TCP_BUF_SIZE;

	uint8_t *msg = OSA_FixedMemMalloc(tcp_client_buf_size);
	int ret, recv_byte = 0, len=0;
	if(!msg)
	{
		NET_DEBUG_ERROR("Unable to allocate memory for network_tcpclient");
		return;
	}

	int error = 0;
	//len = sizeof (error);
	//ret = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);

	NET_DEBUG_TCP("tcp_client_thread start\r\n");


	// Connect to server
	tcp_client_connect();
	// TODO: update callback here

	while (1) {
		// TODO: Read from server if connected
		bzero(msg, tcp_client_buf_size);
		ret = read(tcp_client_socket, msg, tcp_client_buf_size);
		if (ret > 0) {

			recv_byte  = 0;
			while ((ret > 0) && (recv_byte < tcp_client_buf_size)){
				recv_byte += ret;
				ret = read(tcp_client_socket, &msg[recv_byte], tcp_client_buf_size-recv_byte);
				//					NET_DEBUG_TCP("Read(childfds[%d] %d bytes\r\n",idx, ret);
			}

			NET_DEBUG_TCP("TCPClient read %d total bytes",recv_byte);

			if (lwtcp_client_data_received_handler != NULL) {
				lwtcp_client_data_received_handler(msg, recv_byte);
			}
			//			else {
			//				ret = Network_LWTCPClientSendCmd(msg, recv_byte);
			//				NET_DEBUG_TCP("TCP Client echo %d bytes:\r\nn", recv_byte);
			//			}
			recv_byte = 0;
		}


		// If not connected try to reconnect
		//OSA_TimeDelay(100);
		if (ret == 0) {
			NET_DEBUG_TCP("Ret = 0, lwtcp_client need to reconnect\r\n");
			tcp_client_connect();// reconnect if error
		}
		// TODO: Send data if avaiable
		p_msg = OSTaskQPend(100,
				OS_OPT_PEND_BLOCKING,
				&msg_size,
				&ts,
				&err);
		if (err == OS_ERR_NONE) {
			// There's some data to send
			pNetData = (SNetData*)p_msg;
			len = write(tcp_client_socket, pNetData->buf, pNetData->len);
			if (len == pNetData->len) {
				if (lwtcp_client_data_sent_handler)
					lwtcp_client_data_sent_handler(pNetData->buf, pNetData->len);
			} else {
				if (lwtcp_client_data_error_handler)
					lwtcp_client_data_error_handler(pNetData->buf, pNetData->len);
			}
			if (pNetData->buf) {
				NET_DEBUG_TCP("OSA_FixedMemFree pNetData->buf 0x%x\r\n", pNetData->buf);
				OSA_FixedMemFree(pNetData->buf);
			}
			if (pNetData) {
				NET_DEBUG_TCP("OSA_FixedMemFree pNetData 0x%x\r\n", pNetData);
				OSA_FixedMemFree(pNetData);
			}
		}

		// Check wether the socket is problem;
		//		len = sizeof (error);
		//		ret = getsockopt (tcp_client_socket, SOL_SOCKET, SO_ERROR, &error, &len);
		//		if (ret != 0) {
		//			/* there was a problem getting the error code */
		//			NET_DEBUG_TCP("TCPClient Thread error getting socket error code: %s\n", strerror(ret));
		//			//return;
		//		}
		//
		//		if (error != 0) {
		//			/* socket has a non zero error status_ */
		//			NET_DEBUG_TCP("TCPClient Thread socket error: %s\n", strerror(error));
		//		}
		//		if (ret != 0 || error != 0) {
		//			tcp_client_connect();// reconnect if error
		//		}
	}

	OSA_FixedMemFree(msg);
}

//static void tcp_client_thread(void *arg) {
////	int ret = -1;
//    OS_ERR        err;
//    void         *p_msg;
//    OS_MSG_SIZE   msg_size;
//    CPU_TS        ts;
//    SNetData 	 *pNetData;
//	int optval = 1;
//
//
//	uint8_t *msg = OSA_MemAlloc(NET_MAX_MTU);
//	int ret, recv_byte = 0, len=0;
//	if(!msg)
//	{
//		NET_DEBUG_ERROR("Unable to allocate memory");
//		return;
//	}
//
//	int error = 0;
//	//len = sizeof (error);
//	//ret = getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);
//
//	NET_DEBUG_TCP("tcp_client_thread start\r\n");
//
//
//	// Connect to server
//	tcp_client_connect();
//	// TODO: update callback here
//
//	while (1) {
//		// TODO: Read from server if connected
//
//		ret = read(tcp_client_socket, msg, NET_MAX_MTU);
//		if (ret > 0) {
//
//			recv_byte  = 0;
//			while ((ret > 0) && (recv_byte < NET_MAX_MTU)){
//				recv_byte += ret;
//				ret = read(tcp_client_socket, &msg[recv_byte], NET_MAX_MTU-recv_byte);
////					NET_DEBUG_TCP("Read(childfds[%d] %d bytes\r\n",idx, ret);
//			}
//
//			NET_DEBUG_TCP("TCPClient read %d total bytes",recv_byte);
//
//			if (lwtcp_client_data_received_handler != NULL) {
//				lwtcp_client_data_received_handler(msg, recv_byte);
//			}
////			else {
////				ret = Network_LWTCPClientSendCmd(msg, recv_byte);
////				NET_DEBUG_TCP("TCP Client echo %d bytes:\r\nn", recv_byte);
////			}
//			recv_byte = 0;
//		}
//
//
//		// If not connected try to reconnect
//		OSA_TimeDelay(100);
//		if (ret == 0) {
//			tcp_client_connect();// reconnect if error
//		}
//		// TODO: Send data if avaiable
//		p_msg = OSTaskQPend(200,
//			OS_OPT_PEND_BLOCKING,
//			&msg_size,
//			&ts,
//			&err);
//		if (err == OS_ERR_NONE) {
//			// There's some data to send
//			pNetData = (SNetData*)p_msg;
//			len = write(tcp_client_socket, pNetData->buf, pNetData->len);
//			if (len == pNetData->len) {
//				if (lwtcp_client_data_sent_handler)
//					lwtcp_client_data_sent_handler(pNetData->buf, pNetData->len);
//			} else {
//				if (lwtcp_client_data_error_handler)
//					lwtcp_client_data_error_handler(pNetData->buf, pNetData->len);
//			}
//			OSA_FixedMemFree(pNetData->buf);
//			OSA_FixedMemFree(pNetData);
//		}
//
//		// Check wether the socket is problem;
////		len = sizeof (error);
////		ret = getsockopt (tcp_client_socket, SOL_SOCKET, SO_ERROR, &error, &len);
////		if (ret != 0) {
////			/* there was a problem getting the error code */
////			NET_DEBUG_TCP("TCPClient Thread error getting socket error code: %s\n", strerror(ret));
////			//return;
////		}
////
////		if (error != 0) {
////			/* socket has a non zero error status_ */
////			NET_DEBUG_TCP("TCPClient Thread socket error: %s\n", strerror(error));
////		}
////		if (ret != 0 || error != 0) {
////			tcp_client_connect();// reconnect if error
////		}
//	}
//
//	OSA_MemFree(msg);
//}
#endif
/**
 * Start an TCP client connect forerver to TCP server public ip
 * @param ip
 * @param port
 * @return
 */
lwtcp_result_t Network_LWTCPClientStart(const char* ip, int port)
{
	//TODO: Implement
//	static bool tcp_server_connected = false;
//	static char* tcp_server_ip;
//	static int tcp_server_port;
//	struct timeval tcp_client_tv;
//	static int tcp_client_socket = NULL
//	static struct sockaddr_in tcp_server_ctrl;//, server_dat;
	tcp_server_ip = ip;
	tcp_server_port = port;

	osa_status_t result = OSA_TaskCreate(network_tcpclient,
                    (uint8_t *)"network_tcpclient",
					TASK_NETWORK_TCPCLIENT_SIZE,
					network_tcpclient_stack,
					TASK_NETWORK_TCPCLIENT_PRIO,
                    (task_param_t)0,
                    false,
                    &network_tcpclient_task_handler);


	sys_thread_t threadID;
    if(result != kStatus_OSA_Success)
    {
        PRINTF("Failed to create tcp client task\r\n\r\n");
        return LWTCP_RESULT_ERR_OPENING;
    }
	return LWTCP_RESULT_OK;
}


//static int lwtcp_receive_ctrl_data(int sock, char* buf, int bufsize) {
//	int resp_len = 0, total_len = 0;
//	int max_receive_byte = bufsize;
//	do {
//		memset(reply_msg, 0x00, sizeof(reply_msg));
//		max_receive_byte -= resp_len;
//		resp_len = recv(sock, reply_msg, max_receive_byte, 0);
//		total_len += resp_len;
//		if (resp_len > 0)
//			NET_DEBUG_TCP("reply_msg (len = %d):  %s\r\n", resp_len, reply_msg);
//
//	} while (resp_len > 0 && resp_len < lwftp_bufsize);
//	return total_len;
//}
/**
 * Stop TCP Client
 * @return
 */
lwtcp_result_t Network_LWTCPClientStop()
{
	return LWTCP_RESULT_OK;
}
/**
 * Send data to TCP Client
 * @param data
 * @param length
 * @return
 */
uint32_t Network_LWTCPClientSendCmd(const uint8_t *data, uint32_t length)
{
	if (length > TCP_DATA_SIZE)
		return 0;
	SNetData* pNetData = (SNetData*)OSA_FixedMemMalloc(sizeof(SNetData));
	if (!pNetData) {
		NET_DEBUG_TCP("Cannot OSA_FixedMemMalloc pNetData\r\n");
		return 0;
	} else {
		NET_DEBUG_TCP("OSA_FixedMemMalloc pNetData at 0x%x\r\n", pNetData);
	}
	pNetData->buf = (char*)OSA_FixedMemMalloc(length);
	if (!pNetData->buf)
	{
		NET_DEBUG_TCP("Cannot OSA_FixedMemMalloc pNetData->buf\r\n");
		OSA_FixedMemFree(pNetData);
	} else {
		NET_DEBUG_TCP("OSA_FixedMemMalloc pNetData->buf at 0x%x\r\n", pNetData->buf);
	}
	Mem_Copy(pNetData->buf, data, length);
	pNetData->len = length;
	 OS_ERR  err;
	 OSTaskQPost(&TCB_network_tcpclient,
		 (void *)pNetData,
		 sizeof(SNetData),
		 OS_OPT_POST_FIFO,
		 &err);
	if (err != OS_ERR_NONE) {
		NET_DEBUG_TCP("OSTaskQPost err: %d, clean pNetData->buf 0x%x, pNetData 0x%x", err, pNetData->buf, pNetData);
		OSA_FixedMemFree(pNetData->buf);
		OSA_FixedMemFree(pNetData);
		return 0;
	} else {
		NET_DEBUG_TCP("OSTaskQPost err OK");
		return length;
	}
}



/**
 * @file Network_LWFTP_Test.c
 * @author ThinhNT (tienthinh@gmail.com)
 * @brief Simple test for Network_LWFTP Module
 * @version 0.1
 * @date 2018-10-17
 *
 * @copyright Copyright (c) 2018
 *
 */


#include "Network_LWFTP_Test.h"

#if 0
static void ftp_retr_callback(void *arg, int result)
{
	lwftp_session_t *s = (lwftp_session_t*)arg;

	if ( result != LWFTP_RESULT_OK ) {
		LOG_ERROR("retr failed (%d)\r\n", result);
		return Network_LWFTP_Close(s);
	}
	// Test is done
	Network_LWFTP_Close(s);
}

static uint data_sink(void *arg, const char* ptr, uint len)
{
	static const uint mylen = 12345;
	static char * const myconfig = (char*)0x20000000;
	static uint offset = 0;

	if (ptr) {
		len = min( len, mylen-offset );
		memcpy( myconfig+offset, ptr, len );
		offset += len;
	}
	return len;
}

static void ftp_stor_callback(void *arg, int result)
{
	lwftp_session_t *s = (lwftp_session_t*)arg;
	err_t error;

	if ( result != LWFTP_RESULT_OK ) {
		LOG_ERROR("stor failed (%d)\r\n", result);
		return Network_LWFTP_Close(s);
	}
	// Continue with RETR request
	s->data_sink = data_sink;
	s->done_fn = ftp_retr_callback;
	s->remote_path = "configfile";
	error = Network_LWFTP_Retrieve(s);
	if ( error != LWFTP_RESULT_INPROGRESS ) {
		LOG_ERROR("lwftp_retrieve failed (%d)\r\n", error);
	}
	// FTP session will continue with RETR and sink callbacks
}

static uint data_source(void *arg, const char** pptr, uint maxlen)
{
	static const uint mylen = 12345;
	static const char * const mydata = (char*)0x20000000;
	static uint offset = 0;
	uint len = 0;

	// Check for data request or data sent notice
	if (pptr) {
		len = mylen - offset;
		if ( len > maxlen ) len = maxlen;
		*pptr = mydata + offset;
	} else {
		offset += maxlen;
		if ( offset > mylen ) offset = mylen;
	}
	return len;
}

static void ftp_connect_callback(void *arg, int result)
{
	lwftp_session_t *s = (lwftp_session_t*)arg;
	err_t error;

	if ( result != LWFTP_RESULT_LOGGED ) {
		LOG_ERROR("ftp_connect_callback: login failed (%d)\r\n", result);
		return Network_LWFTP_Close(s);
	}
	// Continue with STOR request
	s->data_source = data_source;
	s->done_fn = ftp_stor_callback;
	s->remote_path = "logfile";
	error = Network_LWFTP_Store(s);
	if ( error != LWFTP_RESULT_INPROGRESS ) {
		LOG_ERROR("lwftp_store failed (%d)\r\n", error);
	}
	// FTP session will continue with STOR and source callbacks
}
#endif
void Network_LWFTP_Test(void)
{
#if 0
	static lwftp_session_t s;   // static content for the whole FTP session
	err_t error;
	PRINTF("===========Start Network_LWFTP_Test================\r\n");
	// Initialize session data
	memset(&s, 0, sizeof(s));
	IP4_ADDR(&s.server_ip, 192,168,0,103);
	s.server_port = 21;
	s.done_fn = ftp_connect_callback;
	s.user = "thinhnt";
	s.pass = "123456a@";
	// We have no extra user data, simply use the session structure
	s.handle = &s;

	// Start the connection state machine
	error = Network_LWFTP_Connect(&s);
	if ( error != LWFTP_RESULT_INPROGRESS ) {
		PRINTF("ThinhNT: error != LWFTP_RESULT_INPROGRESS\r\n");
		LOG_ERROR("lwftp_connect failed (%d)", error);
	}
	// FTP session will continue with the connection callback
#endif
}





int Network_LWFTP_Test_Socket(void)
{

	int 	socket_ctrl, socket_dat;
	struct 	sockaddr_in server, server_dat;
	char 	request_msg[BUFSIZ], reply_msg[BUFSIZ];
	char 	data_buf[BUFSIZ]; // buffer for data
	unsigned int a,b,c,d,e,f;
	int 	resp_code;
	int resp_len = 0;
	char test_str[] = "Viet Nam HCM\r\n";
	unsigned long port; // port for data

	PRINTF("Buffer Size = %d\r\n", BUFSIZ);

	// Variables for the file being received
//	int	file_size,
//		file_desc;
//	char	*data;
	char *ptr;

	// Step 1: Connect to the server
	socket_ctrl = socket(AF_INET, SOCK_STREAM, 0);
	struct timeval tv;
	tv.tv_sec = 3000; //dkm co gi do sai sai o day khi dung lwip 3000s ma nhu la 3s
	tv.tv_usec = 10000;
	// set timeout of socket
	setsockopt(socket_ctrl, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	if (socket_ctrl == -1)
	{
		perror("Could not create socket");
		return 1;
	}

	server.sin_addr.s_addr = inet_addr(SERVER_IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	// Connect to server
	if (connect(socket_ctrl, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("Connection failed");
		return 1;
	} else {
		PRINTF("Connected\r\n");
	}
	memset(reply_msg,0x00,BUFSIZ);
	recv(socket_ctrl, reply_msg, BUFSIZ, 0);
	PRINTF("reply_msg: %s\r\n ", reply_msg);

	// Enter logging information
	strcpy(request_msg, "USER ");
	strcat(request_msg, USER_NAME);
	strcat(request_msg, "\r\n");
	PRINTF("Enter Loggin INformation: %s\r\n", request_msg);
	write(socket_ctrl, request_msg, strlen(request_msg));
	recv(socket_ctrl, reply_msg, BUFSIZ, 0);
	PRINTF("reply_msg:  %s\r\n", reply_msg);

	strcpy(request_msg, "PASS ");
	strcat(request_msg, PASSWORD);
	strcat(request_msg, "\r\n");
	PRINTF("Enter Password: %s\r\n", PASSWORD);
	write(socket_ctrl, request_msg, strlen(request_msg));
	memset(reply_msg,0x00,BUFSIZ);
	recv(socket_ctrl, reply_msg, BUFSIZ, 0);
	PRINTF("reply_msg:  %s\r\n", reply_msg);

	PRINTF("\r\n=============TEST LIST DIRECTORY=========\r\n");
	// Getting the PASV port
	strcpy(request_msg, PASV);
	strcat(request_msg, "\r\n");
	write(socket_ctrl, request_msg, strlen(request_msg));
	memset(reply_msg,0x00,BUFSIZ);
	recv(socket_ctrl, reply_msg, BUFSIZ, 0);
	resp_code = strtoul(reply_msg, NULL, 10);
	PRINTF("reply_msg:  %s resp_code = %d\r\n", reply_msg, resp_code);
	// Find pasv port

	ptr = strchr(reply_msg, '(');
	do {
		a = strtoul(ptr+1,&ptr,10);
		b = strtoul(ptr+1,&ptr,10);
		c = strtoul(ptr+1,&ptr,10);
		d = strtoul(ptr+1,&ptr,10);
		e = strtoul(ptr+1,&ptr,10);
		f = strtoul(ptr+1,&ptr,10);
	} while(0);
	port = e*256+f;
	PRINTF("pasv port =%d\r\n", port);


	// connect to data port connection
	socket_dat = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(socket_dat, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	if (socket_dat == -1)
	{
		perror("Could not create socket");
		return 1;
	}
	server_dat.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_dat.sin_family = AF_INET;
	server_dat.sin_port = htons(port);
	// Connect to server for data
	if (connect(socket_dat, (struct sockaddr *)&server_dat, sizeof(server_dat)) < 0)
	{
		perror("Connection data port failed");
		return 1;
	} else {
		PRINTF("Connected  data port\r\n");
	}

	// Send to get list files port 21
	strcpy(request_msg, "LIST -a\r\n");
	write(socket_ctrl, request_msg, strlen(request_msg));
	// Recv data port data
	memset(data_buf,0x00,BUFSIZ);
	recv(socket_dat, data_buf, BUFSIZ, 0);
	PRINTF("data_buf:  %s \r\n", data_buf);
	// recv ctrl data
	do {
		resp_len = recv(socket_ctrl, reply_msg, BUFSIZ, 0);
		PRINTF("reply_msg (len = %d):  %s\r\n",resp_len, reply_msg);
	} while (resp_len > 0);


	//===================TEST GET DATA===============
	PRINTF("\r\n=============TEST GET DATA=========\r\n");
	// Getting the PASV port
	strcpy(request_msg, PASV);
	strcat(request_msg, "\r\n");
	write(socket_ctrl, request_msg, strlen(request_msg));
	memset(reply_msg,0x00,BUFSIZ);
	recv(socket_ctrl, reply_msg, BUFSIZ, 0);
	resp_code = strtoul(reply_msg, NULL, 10);
	PRINTF("reply_msg:  %s resp_code = %d\r\n", reply_msg, resp_code);
	// Find pasv port
	ptr = strchr(reply_msg, '(');
	do {
		a = strtoul(ptr+1,&ptr,10);
		b = strtoul(ptr+1,&ptr,10);
		c = strtoul(ptr+1,&ptr,10);
		d = strtoul(ptr+1,&ptr,10);
		e = strtoul(ptr+1,&ptr,10);
		f = strtoul(ptr+1,&ptr,10);
	} while(0);
	port = e*256+f;
	PRINTF("pasv port =%d\r\n", port);
	// connect to data port connection
	socket_dat = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(socket_dat, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	if (socket_dat == -1)
	{
		perror("Could not create socket");
		return 1;
	}
	server_dat.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_dat.sin_family = AF_INET;
	server_dat.sin_port = htons(port);
	// Connect to server for data
	if (connect(socket_dat, (struct sockaddr *)&server_dat, sizeof(server_dat)) < 0)
	{
		perror("Connection data port failed");
		return 1;
	} else {
		PRINTF("Connected  data port\r\n");
	}

	// Recv a file
	strcpy(request_msg, "RETR ");
	strcat(request_msg, FILENAME);
	strcat(request_msg, "\r\n");
	write(socket_ctrl, request_msg, strlen(request_msg));
	// Recv data port data
	memset(data_buf,0x00,BUFSIZ);
	int len = recv(socket_dat, data_buf, BUFSIZ, 0);
	for (int i = 0; i <len; i++) {
		PRINTF("%.02X ", (unsigned char) data_buf[i]);
		if ((i+1)%16 == 0) PRINTF("\r\n");
	}

	// recv ctrl data until there's now data
	do {
		resp_len = recv(socket_ctrl, reply_msg, BUFSIZ, 0);
		PRINTF("reply_msg (len = %d):  %s\r\n",resp_len, reply_msg);
	} while (resp_len > 0);

	//===================TEST STORE DATA===============
	PRINTF("\r\n=============TEST STORE DATA=========\r\n");
	// Getting the PASV port
	strcpy(request_msg, PASV);
	strcat(request_msg, "\r\n");
	write(socket_ctrl, request_msg, strlen(request_msg));
	memset(reply_msg,0x00,BUFSIZ);
	recv(socket_ctrl, reply_msg, BUFSIZ, 0);
	resp_code = strtoul(reply_msg, NULL, 10);
	PRINTF("reply_msg:  %s resp_code = %d\r\n", reply_msg, resp_code);
	// Find pasv port
	ptr = strchr(reply_msg, '(');
	do {
		a = strtoul(ptr+1,&ptr,10);
		b = strtoul(ptr+1,&ptr,10);
		c = strtoul(ptr+1,&ptr,10);
		d = strtoul(ptr+1,&ptr,10);
		e = strtoul(ptr+1,&ptr,10);
		f = strtoul(ptr+1,&ptr,10);
	} while(0);
	port = e*256+f;
	PRINTF("pasv port =%d\r\n", port);
	// connect to data port connection
	socket_dat = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(socket_dat, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	if (socket_dat == -1)
	{
		perror("Could not create socket");
		return 1;
	}
	server_dat.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_dat.sin_family = AF_INET;
	server_dat.sin_port = htons(port);
	// Connect to server for data
	if (connect(socket_dat, (struct sockaddr *)&server_dat, sizeof(server_dat)) < 0)
	{
		perror("Connection data port failed");
		return 1;
	} else {
		PRINTF("Connected  data port\r\n");
	}

	// Store a file
	strcpy(request_msg, "STOR ");
	strcat(request_msg, " test_store1.txt\r\n");
//	char cEOF = 0xFF;
	write(socket_ctrl, request_msg, strlen(request_msg));
	// Write data to file in server
	len = write(socket_dat, test_str, strlen(test_str));
	close(socket_dat);
	//len = write(socket_dat, &cEOF, 1);
	// recv ctrl data
	recv(socket_ctrl, reply_msg, BUFSIZ, 0);
	PRINTF("reply_msg:  %s \r\n", reply_msg);
	// Close two sections
	close(socket_ctrl);
	PRINTF("Goodbye\r\n");
	return 0;
}


int Network_LWFTP_Test_Netconn(void)
{
	PRINTF("==============Network_LWFTP_Test_Netconn=============\r\n");
	// Example of netconn client
	struct netconn *pNetcon_ctrl = NULL;
	struct netconn *pNetcon_data = NULL;
	struct ip_addr server_ip;
	struct netbuf *buf;
	char *data;
	char 	request_msg[BUFSIZ];
	//char 	reply_msg[BUFSIZ];
	u16_t len;
	// setup server_ip
	server_ip.addr = htonl(0xc0a80068); //192.168.0.104
//	int rc1, rc2;
	err_t err;
	pNetcon_ctrl = netconn_new(NETCONN_TCP);
	netconn_set_recvtimeout (pNetcon_ctrl, 3000);
	if (pNetcon_ctrl == NULL) {
		/**
		 * No memory for new connection
		 */
		PRINTF("netconn_new failed\r\n");
	}
	err = netconn_connect(pNetcon_ctrl, &server_ip, 21);
	if (err != ERR_OK) {
		PRINTF("netconn_connect failed\r\n");
	}
	// Getting the response message from resp
	/* receive data until the other host closes the connection */
	while((err = netconn_recv(pNetcon_ctrl, &buf)) == ERR_OK) {
		netbuf_data(buf, &data, &len);
		PRINTF("netconn_recv %d bytes: \r\n%s", len, data);
	}
	// Send user name password
	strcpy(request_msg, "USER ");
	strcat(request_msg, USER_NAME);
	strcat(request_msg, "\r\n");
	PRINTF("Enter Login INformation: %s\r\n", request_msg);
	netconn_write(pNetcon_ctrl, request_msg, strlen(request_msg), NETCONN_NOCOPY);
	/* receive data until the other host closes the connection */
	while((err = netconn_recv(pNetcon_ctrl, &buf)) == ERR_OK) {
		netbuf_data(buf, &data, &len);
		PRINTF("netconn_recv %d bytes: \r\n%s", len, data);
	}
	strcpy(request_msg, "PASS ");
	strcat(request_msg, PASSWORD);
	strcat(request_msg, "\r\n");
	PRINTF("Enter Password: %s\r\n", PASSWORD);
	netconn_write(pNetcon_ctrl, request_msg, strlen(request_msg), NETCONN_NOCOPY);
	/* receive data until the other host closes the connection */
	while((err = netconn_recv(pNetcon_ctrl, &buf)) == ERR_OK) {
		netbuf_data(buf, &data, &len);
		PRINTF("netconn_recv %d bytes: \r\n%s", len, data);
	}


	// Delete connection
	err = netconn_delete(pNetcon_ctrl);
	if (err != ERR_OK) {
		PRINTF("netconn_disconnect failed\r\n");
	} else {
		PRINTF("netconn_disconnected OK\r\n");
	}
	return 1;
}

void Network_LWFTP_Test2(void)
{
	Network_LWFTP_Start(SERVER_IP, SERVER_PORT, USER_NAME, PASSWORD);
	int i, j ,iTries = 0;
	lwftp_result_t ret;
	char filename[128];
	char dir[128];
	char path[256];
	//#define DIR_NAME "/home/vkl/dkm"
	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++) {

			//Network_LWFTP_SendFile("/home", filename);
			memset(filename,0x00,sizeof(filename));
			memset(dir,0x00,sizeof(dir));
			memset(path,0x00,sizeof(path));
			// Test use only sendfile functions
			sprintf(filename,"testfile_%d.bin", i*100+j);
			sprintf(dir,"/home/ftpuser1/test/thinh/%d/%d", i,j);
			Network_LWFTP_SendFile(dir, filename);
			if (i == j) {
				sprintf(path,"%s/%s", dir, filename);
				Network_LWFTP_Delete(path);
			}
			OSA_TimeDelay(5000);
		}
	}
}

void Network_LWTCP_Echo_ClientCallback(int fd)
{
	uint32_t currentTime;// =  OSA_TimeGetMsec();
	uint32_t lastTime =  OSA_TimeGetMsec();
	uint8_t tmp[128];
	int len;

	while (true) {
		len = Network_LWTCP_Receive(fd, tmp, 128);
		if (len > 0) {
			// Just do some Echo
			len = Network_LWTCP_Send(fd, tmp, len);
		}
		currentTime = OSA_TimeGetMsec();
		if ((currentTime - lastTime) > 5000) {
			break;
		} else if (len <= 0) {
			OSA_TimeDelay(100);
		}
	}
}

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



#include "Network_Test.h"
#include "ff.h"

static char filename[128];
static char dir[128];
static char path[128];
static char tmp[128];

void Net_TestFTP(void)
{
	Net_FTPClientStart(SERVER_IP, SERVER_PORT, USER_NAME, PASSWORD);
	int i, j ,iTries = 0;
	lwftp_result_t ret;
	int err, len;
	int retVal;

	len = 128;
	//#define DIR_NAME "/home/vkl/dkm"
	for (i = 0; i < 100; i++) {
		for (j = 0; j < 100; j++) {

			//Network_LWFTP_SendFile("/home", filename);
			memset(filename,0x00,sizeof(filename));
			memset(dir,0x00,sizeof(dir));
			memset(path,0x00,sizeof(path));
			// Test use only sendfile functions
			//retVal = f_chdir("/thinhnt");
//			if(retVal != FR_OK) {
//				LREP("chdir err = %d\r\n", retVal);
//			}
			err = f_getcwd(tmp, len);
			if(err == FR_OK) {
				LREP("current dir = %s\r\n", tmp);
				LREP("cwd successfully\r\n");
			} else {
				LREP("cwd failed \r\n");
			}
			sprintf(filename, "AG_SGCE_KHI001_2018110710%02d%02d.txt",i,j);
			//strcpy(filename,"AG_SGCE_KHI001_20181107105400.txt");
			strcpy(dir,"/thinhnt");
			Net_FTPClientSendFile(dir, filename);
//			if (i == j) {
//				sprintf(path,"%s/%s", dir, filename);
//				Net_FTPClientDeleteFile(path);
//			}
			OSA_TimeDelay(1000);
		}
	}
}

void Net_LWIP_Echo_ClientCallback(int fd)
{
	uint32_t currentTime;// =  OSA_TimeGetMsec();
	uint32_t lastTime =  OSA_TimeGetMsec();
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

void Net_TestTCPServer(void)
{
	NetStatus status =	Net_TCPServerStart(2011);
	if (status == NET_ERR_NONE)
		status = Net_TCPServerSetCallback(Net_LWIP_Echo_ClientCallback);
}

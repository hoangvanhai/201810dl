/*
 * socket_common.c
 *
 *  Created on: Dec 18, 2018
 *      Author: MSI
 */


#include <socket_common.h>
#include <fsl_phy_driver.h>
#include <ethernetif.h>
#include <network.h>

int set_blocking(int fd){
	uint32_t mode = 0;
	return ioctlsocket(fd, FIONBIO, &mode);
}

int set_nonblocking(int fd){
	uint32_t mode = 1;
	return ioctlsocket(fd, FIONBIO, &mode);
}


int set_nodelay(int fd) {
	int opt = 1;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
			(const char *)&opt, sizeof(opt));
}

int wait_event(int fd, int timeout, bool r, bool w) {
	bool status = true;
	if(Network_GetLinkStatus(&status)) {
		if(status == false) {
			return Event_Error;
		}
	}

	int event = 0;
	struct timeval tv;
	fd_set fdw, fderror, fdr;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;
	FD_ZERO(&fderror);
	FD_SET(fd, &fderror);

	fd_set *rd = 0, *wd = 0;
	if (r) {
		FD_ZERO(&fdr);
		FD_SET(fd, &fdr);
		rd = &fdr;
	}
	if (w) {
		FD_ZERO(&fdw);
		FD_SET(fd, &fdw);
		wd = &fdw;
	}

	int err = select(fd + 1, rd, wd, &fderror, &tv);
	if (err < 0 && errno != EINTR) {
		event = Event_Error;
	} else if (err > 0) {
		if (FD_ISSET(fd, &fderror)) {
			event = Event_Error;
		} else if (r && FD_ISSET(fd, &fdr)) {
			event |= Event_Readable;
		} else if (w && FD_ISSET(fd, &fdw)) {
			event |= Event_Writeable;
		}
	} else {
		event = Event_Timeout;
	}
	return event;
}


int set_buffer_size(int fd, int tx_size, int rx_size) {
	return 0;
	// lwip not implement yet
    int err = 0;
    int ival = (int)rx_size / 2;
    socklen_t bufsizelen = sizeof(ival);
    if (ival > 0)
        err = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char *)&ival, bufsizelen);
    ival = (int)tx_size / 2;
    if (ival > 0)
        err = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char *)&ival, bufsizelen) || err;
    return err;
}


bool Network_GetLinkStatus(bool *status) {
	bool ret = (PHY_DRV_GetLinkStatus(
			0, enetDevIf[0].phyAddr, status)
			== kStatus_ENET_Success);

	if(ret) {
		nwkStt.activeIf |= NET_IF_ETHERNET;
	} else {
		nwkStt.activeIf &= ~(NET_IF_ETHERNET);
	}
	return ret;
}




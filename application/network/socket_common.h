/*
 * socket_common.h
 *
 *  Created on: Dec 18, 2018
 *      Author: MSI
 */

#ifndef APPLICATION_SOCKET_COMMON_H_
#define APPLICATION_SOCKET_COMMON_H_

#include <lwip/api.h>
#include <lwip/ip_addr.h>
#include <lwip/sockets.h>

#define INVALIDSOCK 	(-1)

enum EVENT {
    Event_Timeout = 	0,
    Event_Readable = 	(1 << 0),
    Event_Writeable = 	(1 << 1),
    Event_Error = 		(1 << 2),
};

typedef enum Network_Status_ {
    Status_Connected,
    Status_Disconnected,
	Status_Connecting,
    Status_Network_Down,
} Network_Status;

typedef enum Network_DataEvent_ {
	Event_DataReceived,
	Event_DataSendDone,
	Event_DataError,
}Network_DataEvent;

typedef enum ENetwork_Interface {
	Interface_Ethernet = 0,
	Interface_Wireless,
	Interface_All,
}Network_Interface;

typedef void (*NetworkDataEvent)(const uint8_t* data, int length);
typedef void (*NetworkConnNotify)(Network_Status status, Network_Interface interface);


typedef struct SMsg_ {
	uint8_t 	type;
	uint16_t 	length;
	uint8_t	 	*buf;
}SMsg;

int set_nonblocking(int fd);
int set_blocking(int fd);
int wait_event(int fd, int timeout, bool r, bool w);
int set_buffer_size(int fd, int tx_size, int rx_size);

#endif /* APPLICATION_SOCKET_COMMON_H_ */

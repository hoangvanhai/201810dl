/*
 *      File name:      ring_buffer.h
 *
 *      Created on:     Aug 20, 2018
 *      Author:         manhbt
 *      Brief:          Ring-buffer FIFO supported
 */

#ifndef QRINGBUFFER_H
#define QRINGBUFFER_H

#include "typedefs.h"
#include <string.h>
#include <stdlib.h>

//#include <ti/sysbios/knl/Semaphore.h>

 typedef struct _ringbuff_t
 {
		void*               pBuffer;					// Data buffer
		uint8_t             u8ElementSize;              // Size of buffer or the total of elements
		uint16_t		    u16BufferSize;				// Size of buffer or the total of elements
		uint16_t    	    u16ElementCount;				// The element count of buffer
		uint16_t		    u16BufferPopPtr;				// The pointer to start reading
		uint16_t		    u16BufferPushPtr;			// The pointer to start writing

		BOOL                bBufferPopEnable;			// Data popping enabling flag
		BOOL                bBufferPushEnable;			// Data pushing enabling flag
//		Semaphore_Struct    busySem;                    // Semaphore for access buffer
//		Semaphore_Handle    busySemHandle;
 }ring_buff_t;


void			buffer_init(ring_buff_t* pu8Buffer,uint8_t u8ElementSize, uint16_t u16Size, void* pBuffer);

BOOL 			buffer_push(ring_buff_t* pu8Buffer, void* pvData);

BOOL 			buffer_push_stream(ring_buff_t* pu8Buffer, void* pvStream, uint16_t u16Length);

uint8_t         buffer_pop(ring_buff_t* pu8Buffer, void* pvData);

uint16_t        buffer_pop_stream(ring_buff_t* pu8Buffer, void* pvStream, uint16_t u16Length);

uint16_t        buffer_get_count(ring_buff_t* pu8Buffer); //{return pu8Buffer->u16ElementCount;}

uint16_t        buffer_get_available_count(ring_buff_t* pu8Buffer); //{return pu8Buffer->u16BufferSize - pu8Buffer->u16ElementCount;}

uint8_t			buffer_get_data(ring_buff_t* pBuffer, void* data, uint16_t* len);

void 			buffer_flush(ring_buff_t* pu8Buffer);

void			buffer_pop_enable(ring_buff_t* pu8Buffer); // {pu8Buffer->bBufferPopEnable = TRUE;}

void			buffer_pop_disable(ring_buff_t* pu8Buffer); // {pu8Buffer->bBufferPopEnable = FALSE;}

void			buffer_push_enable(ring_buff_t* pu8Buffer); // {pu8Buffer->bBufferPushEnable = TRUE;}

void			buffer_push_disable(ring_buff_t* pu8Buffer); // {pu8Buffer->bBufferPushEnable = FALSE;}

BOOL 			buffer_is_push_enable(ring_buff_t* pu8Buffer); // {return pu8Buffer->bBufferPushEnable;}

BOOL 			buffer_is_pop_enable(ring_buff_t* pu8Buffer); // {return pu8Buffer->bBufferPopEnable;}


#endif // QRINGBUFFER_H

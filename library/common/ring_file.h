/*
 *      File name:      ring_buffer.h
 *
 *      Created on:     Aug 20, 2018
 *      Author:         manhbt
 *      Brief:          Ring-buffer FIFO supported
 */

#ifndef __RING_FILE_H__
#define __RING_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "typedefs.h"
#include <string.h>
#include <stdlib.h>
#include <ff.h>

	
/**
 * Retry table header
 */
 #pragma pack(1)
 typedef struct
 {
		uint16_t    	    u16_max_record_count;				// Maximum element count can be storaged
		uint16_t    	    u16_record_count;					// The element count of buffer
		uint16_t            u16_record_size;             		// Size of buffer or the total of elements
		uint32_t		    u32_buffer_size;					// Size of buffer or the total of elements
		uint16_t		    u16_buffer_pop_index;				// The pointer to start reading
		uint16_t		    u16_buffer_push_index;				// The pointer to start writing

 }ring_file_header_t;
#pragma pack()

 /**
  * Retry table record
  */
  #pragma pack(1)
 typedef struct
 {
	 	 // TODO: [manhbt] Use OSA_MemAlloc to against stack overflow
		uint8_t             dir_path[128];              // Retry filr directory (on SD Card)
		uint8_t		    	file_name[128];				// Retry file name (on SD Card)
		uint8_t 			flag;
		uint16_t    	    crc;						// CRC16 of record
 }ring_file_record_t;
#pragma pack()

/**
 * Retry table handler
 */

 typedef struct
 {
 #if 0
	 uint8_t 	path[128];
	 uint8_t	name[128];
#else
//	uint8_t 	path[64];
	 char* 		path;			// Use OSA_MemAlloc to against stack overflow
#endif
	 FIL			fp;
	 ring_file_header_t	header;
 } ring_file_handle_t;


 /************************** Function Prototypes ******************************/

 /**
  * Init Ring file handler
  * @param handle ring-file handler
  * @param path directory of ring file (on SD card)
  * @param name file name of ring file (on SD card)
  * @param count maximum record count
  * @param size size of each record
  */
void			ring_file_init(ring_file_handle_t* handle, const char* path, const char* name, uint16_t count, uint16_t size);

/**
 * Push new record to the end of retry table
 * @param handle ring-file handler
 * @param data pointer to store record data
 * @return TRUE if success, FALSE if failed
 */
BOOL 			ring_file_push_back(ring_file_handle_t* handle, void* data);

/**
 * Get and delete (pop) a record at the front of retry table
 * @param handle ring-file handler
 * @param data pointer to store record data
 * @return TRUE if success, FALSE if failed
 */
BOOL         	ring_file_pop_front(ring_file_handle_t* handle, void* data);

/**
 * Get (but not delete) a record  at the front of retry table
 * @param handle ring-file handler
 * @param data pointer to store record data
 * @return TRUE if success, FALSE if failed
 */
BOOL         	ring_file_get_front(ring_file_handle_t* handle, void* data);

/**
 * Get the total used record stored in retry table
 * @param handle ring-file handler
 * @return	total used record currently stored in retry table
 */
uint16_t        ring_file_get_count(ring_file_handle_t* handle); //{return pu8Buffer->u16ElementCount;}

/**
 * Get the total free records can be used in retry table
 * @param handle retry table handler
 * @return total free records can be used in retry table
 */
uint16_t        ring_file_get_available_count(ring_file_handle_t* handle); //{return pu8Buffer->u16BufferSize - pu8Buffer->u16ElementCount;}


/**
 * Empty the retry table
 * @param handle retry table handler
 */
void 			ring_file_flush(ring_file_handle_t* handle);

/**
 * Print the ringfile handler content
 * @param handle retry table handler
 */
void 			ring_file_print(ring_file_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif // QRINGBUFFER_H

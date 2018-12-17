/*
 *      File name:      ring_buffer.c
 *
 *      Created on:     Aug 20, 2018
 *      Author:         manhbt
 *      Brief:          Ring-buffer FIFO supported
 */

#include "ring_file.h"

#include "modem_debug.h"

static BOOL ring_file_create_new(ring_file_handle_t* hanlde, char* full_path, uint16_t record_count, uint16_t record_size);

//static void ring_file_delay_ms(uint16_t ms)    
//{ 
//	uint16_t i,j; 
//	for( i = 0; i < ms; i++ )
//	{ 
//		for( j = 0; j < 1141; j++ );
//	}
//} 

void ring_file_print(ring_file_handle_t *handle)
{
	MODEM_DEBUG("================================");
	MODEM_DEBUG("   Ring File Info					\r\n\t+max record count: %d\r\n\t+curr record count: %d\r\n\t+record size: %d\r\n\t+push index: %d\r\n\t+pop index: %d\r\n\t+buffer size:%d", 
			handle->header.u16_max_record_count,
			handle->header.u16_record_count, 
			handle->header.u16_record_size,
			handle->header.u16_buffer_push_index,
			handle->header.u16_buffer_pop_index,
			handle->header.u32_buffer_size);
	MODEM_DEBUG("================================");
}

/**
 * Init Ring file handler
 * @param handle ring-file handler
 * @param path	directory of ring file (on SD card)
 * @param name file name of ring file (on SD card)
 * @param record_size	size of each record in retry_table
 * @param max_record_count	maximum of records stored by ring file
 */
void			ring_file_init(ring_file_handle_t* handle, const char* path, const char* name, uint16_t count, uint16_t size)
{
	FIL *fp;
	FRESULT ret;
	uint16_t br = 0;
	uint32_t fuck = 0;

//	char *full_file_name = OSA_FixedMemMalloc(256);
	if(handle == NULL)
	{
		MODEM_DEBUG_ERROR("Retry table handler is NULL\r\n");
		return;
	}
	ASSERT_VOID((handle != NULL));
	
	handle->path = OSA_MemAlloc(256);
	ASSERT_VOID(handle->path);

	memset(handle->path, 0, 256);


	fp = &handle->fp;
	/**
	 *  TODO [manhbt]
	 *  1. Check file exist, if no -> create new file
	 *  2. Verify the file
	 *     2.1 Read the file header
	 *     2.2 Check the size of file with corresponding to data in file header
	 *     2.3 If invalid --> delete & create new file
	 *  3. Fill out the handler header
	 */

	
//	if(!full_file_name)
//	{
//		MODEM_DEBUG_ERROR("Unable to allocate memory");
//		return;
//	}

	sprintf((char*)handle->path, "%s/%s", path, name);
	MODEM_DEBUG("opening .. %s", handle->path);
	ret = f_open(fp, (const char*)handle->path, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);

	
	if(ret)
	{
		MODEM_DEBUG_ERROR("Unable to open/create file");
		return;
	}

		
	MODEM_DEBUG("file %s open OK!!!", handle->path);
	fuck= f_size(fp);
	MODEM_DEBUG("file size is %d (%d) (%d)", fuck,  f_size(&handle->fp),  f_size(fp));
	
	if(fuck != sizeof(ring_file_header_t) +  count *  size)
	{
		MODEM_DEBUG_ERROR("Ring file is not valid in size: %d != %d",
						fuck,
						sizeof(ring_file_header_t) +  count *  size);

		// create new file
		MODEM_DEBUG("Creating new file...");
		ring_file_create_new(handle, (char*)handle->path, count, size);
	}
	else
	{
		MODEM_DEBUG("File OK!");
	}

//	*  2. Verify the file
//		 *     2.1 Read the file header

	ret = f_read(fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&br);

	if(ret)
	{
		MODEM_DEBUG_ERROR("Unable to read ring file header, ret = %d", ret);
	}
	else {
		ring_file_print(handle);
			
	}

//	OSA_FixedMemFree(full_file_name);
	f_close(fp);
	

}

/**
* Push new record to the end of retry table
* @param handle ring-file handler
* @param data pointer to store record data
* @return TRUE if success, FALSE if failed
*/
BOOL 			ring_file_push_back(ring_file_handle_t* handle, void* data)
{
	uint32_t offset  = 0;
	FRESULT ret = FR_OK;
	uint16_t btw = 0, bw=0;

	ASSERT_NONVOID(handle != NULL, FALSE);
	ASSERT_NONVOID(data != NULL, FALSE);


	 if (((handle->header.u16_record_count+ 1)*handle->header.u16_record_size > handle->header.u32_buffer_size))
	    {
			MODEM_DEBUG_ERROR("Buffer is FULL");
			return FALSE;
	    }

	// Move the file pointer to push index

	ret = f_open(&handle->fp, (const TCHAR*)handle->path, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
	if(ret){
		MODEM_DEBUG_ERROR("Open file Failed, err_code: 0x%.2x", ret);
		return FALSE;
	}

	offset = sizeof(ring_file_header_t) + handle->header.u16_record_size*(handle->header.u16_buffer_push_index%handle->header.u16_max_record_count);

	ret = f_lseek(&handle->fp, offset);
	if(ret)
	{
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", offset, ret);
		f_close(&handle->fp);
		return FALSE;
	}

	btw = sizeof(ring_file_record_t), bw = 0;
	ret = f_write(&handle->fp, data, btw, (UINT*)&bw);
	if ((ret != FR_OK))
	{
		MODEM_DEBUG_ERROR("Write Record ERROR, err_code: 0x%.2x", ret);
		f_close(&handle->fp);
		return FALSE;
	}



	/**
	 * Write header
	 */
	handle->header.u16_buffer_push_index++;
	handle->header.u16_record_count++;
	ret = f_lseek(&handle->fp, 0);
	
	ret = f_write(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&bw);
	if(ret){
		MODEM_DEBUG_ERROR("Update File header ERROR, err_code: 0x%.2x", ret);
		f_close(&handle->fp);
		return FALSE;
	}
	

	// read back header
	ret = f_lseek(&handle->fp, 0);
	if(ret)
	{
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return FALSE;
	}
		
	ret = f_read(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&bw);
	if(ret){
		MODEM_DEBUG_ERROR("Read back file header ERROR, err_code: 0x%.2x", ret);
		f_close(&handle->fp);
		return FALSE;
	}
	
//	ring_file_print(handle);

	f_close(&handle->fp);
	
	return TRUE;


}

/**
* Get and delete (pop) a record at the front of retry table
* @param handle ring-file handler
* @param data pointer to store record data
* @return TRUE if success, FALSE if failed
*/
BOOL        	ring_file_pop_front(ring_file_handle_t* handle, void* data)
{
	uint32_t offset  = 0;
	FRESULT ret = FR_OK;
	uint16_t btr = 0, br=0;
	uint16_t btw = 0, bw=0;
	ASSERT_NONVOID(handle != NULL, FALSE);
	ASSERT_NONVOID(data != NULL, FALSE);
	

	/* Nothing to pop */
	if ((handle->header.u16_record_count == 0) )
	{
		MODEM_DEBUG_WARNING("Nothing to pop!");
		return FALSE;
	} 

	/* Calculate the start address for popping out */
	offset = sizeof(ring_file_header_t) + handle->header.u16_record_size*handle->header.u16_buffer_pop_index;

	/* If the pop address is out of address range of the buffer */
	if (offset > handle->header.u32_buffer_size)
	{
		MODEM_DEBUG_ERROR("Unable to pop data");
		return FALSE;
	}

	ret = f_open(&handle->fp, (const TCHAR*)handle->path, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	/* Open file Failed */
	if(ret)
	{
		MODEM_DEBUG_ERROR("Unable to open file '%s', err_code: 0x%.2x", handle->path, ret);
		return FALSE;
	}

	/* Seeking to pop pointer */
	ret = f_lseek(&handle->fp, offset);

	if(ret)
	{
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", offset, ret);
		f_close(&handle->fp);
		return FALSE;
	}
	
	/* Read the record content to Data buffer */
	btr = sizeof(ring_file_record_t), br = 0;
	ret = f_read(&handle->fp, data, btr, (UINT*)&br);
	if ((ret))
	{
		MODEM_DEBUG_ERROR("Read Record ERROR, err_code: 0x%.2x", ret);
		f_close(&handle->fp);
		return FALSE;
	}

	/* Erase Record content */

	ret = f_lseek(&handle->fp, offset);

	if(ret)
	{
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", offset, ret);
		f_close(&handle->fp);
		return FALSE;
	}

	uint8_t *pRecord = OSA_FixedMemMalloc(sizeof(ring_file_record_t));
	if(!pRecord)
	{
		MODEM_DEBUG_ERROR("ERROR while Alloc memory for temporarily buffer");
		f_close(&handle->fp);
		return FALSE;
	}
	memset(pRecord, 0xFF, sizeof(ring_file_record_t));
	btw = sizeof(ring_file_record_t), bw = 0;
	ret = f_write(&handle->fp, pRecord, btw, (UINT*)&bw);
	OSA_FixedMemFree(pRecord);
	if ((ret) || (bw < btw))
	{
		MODEM_DEBUG_ERROR("Erase Record ERROR, err_code: 0x%.2x", ret);
		f_close(&handle->fp);
		return FALSE;
	}

	/* Update the file header */
	handle->header.u16_buffer_pop_index++;
	handle->header.u16_buffer_pop_index %= handle->header.u16_max_record_count;	
	handle->header.u16_record_count--;
	
	ret = f_lseek(&handle->fp, 0);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return FALSE;
	}
	ret =f_write(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&br);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while Update header, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return FALSE;
	}

	// read back header
	ret = f_lseek(&handle->fp, 0);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return FALSE;
	}
	
	ret = f_read(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&br);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while read back  file header, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return FALSE;
	}
//	ring_file_print(handle);

	f_close(&handle->fp);
	return TRUE;
}

/**
* Get (but not delete) a record  at the front of retry table
* @param handle ring-file handler
* @param data pointer to store record data
* @return TRUE if success, FALSE if failed
*/
uint8_t         ring_file_get_front(ring_file_handle_t* handle, void* data)
{
	uint32_t offset  = 0;
	FRESULT ret = FR_OK;
	uint16_t btr = 0, br=0;

	ASSERT_NONVOID(handle != NULL, FALSE);
	ASSERT_NONVOID(data != NULL, FALSE);


	/* Nothing to pop */
	if ((handle->header.u16_record_count == 0) )
	{
		MODEM_DEBUG_WARNING("Nothing to get!");
		return FALSE;
	}

	/* Calculate the start address for popping out */
	offset = sizeof(ring_file_header_t) + handle->header.u16_record_size*handle->header.u16_buffer_pop_index;

	/* If the pop address is out of address range of the buffer */
	if (offset > handle->header.u32_buffer_size)
	{
		MODEM_DEBUG_ERROR("Unable to get data");
		return FALSE;
	}

	ret = f_open(&handle->fp, (const TCHAR*)handle->path, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	/* Open file Failed */
	if(ret)
	{
		MODEM_DEBUG_ERROR("Unable to open file '%s', err_code: 0x%.2x", handle->path, ret);
		return FALSE;
	}

	/* Seeking to pop pointer */
	ret = f_lseek(&handle->fp, offset);

	if(ret)
	{
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", offset, ret);
		f_close(&handle->fp);
		return FALSE;
	}

	/* Read the record content to Data buffer */
	btr = sizeof(ring_file_record_t), br = 0;
	ret = f_read(&handle->fp, data, btr, (UINT*)&br);
	if ((ret))
	{
		MODEM_DEBUG_ERROR("Read Record ERROR, err_code: 0x%.2x", ret);
		f_close(&handle->fp);
		return FALSE;
	}

//	/* Erase Record content */
//
//	ret = f_lseek(&handle->fp, offset);
//
//	if(ret)
//	{
//		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", offset, ret);
//		f_close(&handle->fp);
//		return FALSE;
//	}
//
//	uint8_t *pRecord = OSA_FixedMemMalloc(sizeof(ring_file_record_t));
//	if(!pRecord)
//	{
//		MODEM_DEBUG_ERROR("ERROR while Alloc memory for temporarily buffer");
//		f_close(&handle->fp);
//		return FALSE;
//	}
//	memset(pRecord, 0xFF, sizeof(ring_file_record_t));
//	btw = sizeof(ring_file_record_t), bw = 0;
//	ret = f_write(&handle->fp, pRecord, btr, (UINT*)&br);
//	OSA_FixedMemFree(pRecord);
//	if ((ret))
//	{
//		MODEM_DEBUG_ERROR("Read Record ERROR, err_code: 0x%.2x", ret);
//		f_close(&handle->fp);
//		return FALSE;
//	}

//	/* Update the file header */
//	handle->header.u16_buffer_pop_index++;
//	handle->header.u16_buffer_pop_index %= handle->header.u16_max_record_count;
//	handle->header.u16_record_count--;
//
//	ret = f_lseek(&handle->fp, 0);
//	if(ret){
//		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", 0, ret);
//		f_close(&handle->fp);
//		return FALSE;
//	}
//	ret =f_write(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&br);
//	if(ret){
//		MODEM_DEBUG_ERROR("ERROR while Update header, err_code: 0x%.2x", 0, ret);
//		f_close(&handle->fp);
//		return FALSE;
//	}
//
//	// read back header
//	ret = f_lseek(&handle->fp, 0);
//	if(ret){
//		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", 0, ret);
//		f_close(&handle->fp);
//		return FALSE;
//	}
//
//	ret = f_read(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&br);
//	if(ret){
//		MODEM_DEBUG_ERROR("ERROR while read back  file header, err_code: 0x%.2x", 0, ret);
//		f_close(&handle->fp);
//		return FALSE;
//	}
//	ring_file_print(handle);

	f_close(&handle->fp);
	return TRUE;
}

/**
* Get the total used record stored in retry table
* @param handle ring-file handler
* @return	total used record currently stored in retry table
*/
uint16_t        ring_file_get_count(ring_file_handle_t* handle)
{
	ASSERT_NONVOID(handle != NULL, 0);
	return handle->header.u16_record_count;
}

/**
* Get the total free records can be used in retry table
* @param handle retry table handler
* @return total free records can be used in retry table
*/
uint16_t        ring_file_get_available_count(ring_file_handle_t* handle)
{
	ASSERT_NONVOID(handle != NULL, 0);
	return handle->header.u16_max_record_count - handle->header.u16_record_count;
}


/**
* Empty the retry table
* @param handle retry table handler
*/
void 			ring_file_flush(ring_file_handle_t* handle)
{

	FRESULT ret = FR_OK;
	uint16_t br=0;
	
	ASSERT_VOID(handle != NULL);
	handle->header.u16_record_count = 0;
	handle->header.u16_buffer_pop_index  = 0;
	handle->header.u16_buffer_push_index = 0;

	ret = f_open(&handle->fp, (const TCHAR*)handle->path, FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
	/* Open file Failed */
	if(ret)
	{
		MODEM_DEBUG_ERROR("Unable to open file '%s', err_code: 0x%.2x", handle->path, ret);
		return;
	}


	/* Update the file header */	
	ret = f_lseek(&handle->fp, 0);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return;
	}
	
	ret =f_write(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&br);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while Update header, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return;
	}

	// read back header
	ret = f_lseek(&handle->fp, 0);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while lseek to %.2x, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return;
	}
	
	ret = f_read(&handle->fp, &handle->header, sizeof(ring_file_header_t), (UINT*)&br);
	if(ret){
		MODEM_DEBUG_ERROR("ERROR while read back  file header, err_code: 0x%.2x", 0, ret);
		f_close(&handle->fp);
		return;
	}
	//ring_file_print(handle);

	f_close(&handle->fp);

}


BOOL ring_file_create_new(ring_file_handle_t* hanlde, char* full_path, uint16_t record_count, uint16_t record_size)
{
	BOOL ret = FALSE;
	FRESULT fr = FR_OK;
	UINT cnt;
	UINT btw, bw;
	UINT btr, br;
	ring_file_header_t header = {0};
	ring_file_record_t record = {0};

	header.u16_max_record_count = record_count;
	header.u16_record_count = 0;
	header.u16_record_size = record_size;
	header.u16_buffer_pop_index  = 0;
	header.u16_buffer_push_index = 0;
	header.u32_buffer_size = record_count*record_size;

	// TODO: check error code
	fr = f_close(&hanlde->fp);
	if(fr)
	{
		MODEM_DEBUG_ERROR("clode file '%s' ERROR, err_code: 0x%.2x", full_path, fr);
		return FALSE;
	}
	//f_unlink(full_path);
	f_open(&hanlde->fp, (const TCHAR*)full_path, FA_READ|FA_WRITE|FA_CREATE_ALWAYS);
	// Delete old file
	f_lseek(&hanlde->fp, 0);
	//writing header
	MODEM_DEBUG("Writing new header...");
	f_write(&hanlde->fp, (const void*)&header, sizeof(ring_file_header_t), &bw);

	// Writing records
	MODEM_DEBUG("Writing new Data...");
	btw = sizeof(ring_file_record_t);
	for(cnt = 0; cnt < record_count; cnt ++)
	{
		fr = f_write(&hanlde->fp, (const void*)&record, btw, &bw);	
		//ring_file_delay_ms(10);
		if(bw < btw)
		{
			MODEM_DEBUG_ERROR("Write failed %d != %d, written size: %d bytes (%d records)", bw, btw, cnt*btw+bw, cnt);
			break;
		}
	}

	MODEM_DEBUG("Writing DONE!, seeking to the  header...");
	f_lseek(&hanlde->fp, 0);

	ret = TRUE;
	return ret;
}

//#endif

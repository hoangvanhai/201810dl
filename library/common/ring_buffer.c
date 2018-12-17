/*
 *      File name:      ring_buffer.c
 *
 *      Created on:     Aug 20, 2018
 *      Author:         manhbt
 *      Brief:          Ring-buffer FIFO supported
 */

#include "ring_buffer.h"

#include "modem_debug.h"

void buffer_init(ring_buff_t* pRingBuff, uint8_t u8ElementSize, uint16_t u16Size, void* pBuffer)
{
//	if(pBuffer == NULL)
//	{
//		DEBUG("Buffer is NULL\r\n");
//		return;
//	}
	pRingBuff->pBuffer = pBuffer;
//    if(!pRingBuff->pBuffer)
//        pRingBuff->pBuffer = malloc(u16Size*u8ElementSize);

//    qDebug() << "Malloc pBuffer at " << QString.fromStdString((uint32_t)pBuffer);
//    callbackLock = callbackLockFPtr;
//    callbackUnlock = callbackUnlockFPtr;

    pRingBuff->u16BufferSize = u16Size*u8ElementSize;
    pRingBuff->u8ElementSize = u8ElementSize;
    pRingBuff->u16ElementCount 	= 0;
    pRingBuff->u16BufferPopPtr	= 0;
    pRingBuff->u16BufferPushPtr	= 0;

    pRingBuff->bBufferPopEnable	= TRUE;
    pRingBuff->bBufferPushEnable	= TRUE;

	uint8_t msg[128] = {0};
//	sprintf(msg, "u16ElementCount = %d, u8ElementSize = %d, u16BufferSize = %d",
//			pRingBuff->u16ElementCount ,
//			pRingBuff->u8ElementSize,
//			pRingBuff->u16BufferSize);
//
//	DEBUG(msg);

//    Semaphore_Params semParam;
//    Semaphore_Params_init(&semParam);
//    Semaphore_construct(&pRingBuff->busySem, 1, &semParam);
//    pRingBuff->busySemHandle = Semaphore_handle(&pRingBuff->busySem);
//
//    Semaphore_post(pRingBuff->busySemHandle);
}

// Push a element to queue
BOOL buffer_push(ring_buff_t* pRingBuff, void* pvData)
{
		uint8_t*	pu8Buffer	= NULL;

		if ((pRingBuff->u16ElementCount*pRingBuff->u8ElementSize >= pRingBuff->u16BufferSize) || (pRingBuff->bBufferPushEnable == FALSE))
		{
				return FALSE;
		}

		// Lock accessing to the buffer
//		if (m_pMutex)
//		{
//				m_pMutex->lock();
//		}
//		Semaphore_pend(pRingBuff->busySemHandle, 0);

		// Calculate the start address for pushing in
		pu8Buffer = (uint8_t*)pRingBuff->pBuffer + pRingBuff->u16BufferPushPtr;

		// Push data element
		memcpy(pu8Buffer, pvData, pRingBuff->u8ElementSize);

		// Point the push pointer to the new position
		pRingBuff->u16BufferPushPtr+=pRingBuff->u8ElementSize;

		if (pRingBuff->u16BufferPushPtr >= pRingBuff->u16BufferSize)
		{
				pRingBuff->u16BufferPushPtr = 0;
		}

		// Increase element count of the buffer
		pRingBuff->u16ElementCount++;

		// Unlock accessing to the buffer
//	if (m_pMutex)
//	{
//		m_pMutex->unlock();
//	}
//		Semaphore_post(pRingBuff->busySemHandle);
		return TRUE;
}

uint8_t buffer_pop(ring_buff_t* pRingBuff, void* pvData)
{
    uint8_t*	pu8Data	= NULL;

    if ((pRingBuff->u16ElementCount == 0) || (pRingBuff->bBufferPopEnable == FALSE))
    {
        return 0;
    }

    // Lock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->lock();
//    }
//    Semaphore_pend(pRingBuff->busySemHandle, 0);
    // Calculate the start address for popping out
    pu8Data = (uint8_t*)pRingBuff->pBuffer + pRingBuff->u16BufferPopPtr;

    // Pop data element
    memcpy(pvData, pu8Data, pRingBuff->u8ElementSize);

    // Point the pop pointer to the new position
    pRingBuff->u16BufferPopPtr+=pRingBuff->u8ElementSize;
    if (pRingBuff->u16BufferPopPtr >= pRingBuff->u16BufferSize)
    {
        pRingBuff->u16BufferPopPtr = 0;
    }

    // Decrease element count of the buffer
    pRingBuff->u16ElementCount--;

    // Unlock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->unlock();
//    }
//    Semaphore_post(pRingBuff->busySemHandle);
    return 1;
}

BOOL buffer_push_stream(ring_buff_t* pRingBuff, void* pvStream, uint16_t u16Length)
{
    uint8_t*	pu8Buffer		= NULL;
    uint8_t*	pu8RestStream	= NULL;

    if (((pRingBuff->u16ElementCount + u16Length)*pRingBuff->u8ElementSize > pRingBuff->u16BufferSize) || (pRingBuff->bBufferPushEnable == FALSE))
    {
//    	LREP_ERROR("push stream err");
//    	DEBUG("push stream err\r\n");
//    	if((pRingBuff->bBufferPushEnable == FALSE))
//    		DEBUG("push fucntion disabled!!!\r\n");
//    	else {
//    		uint8_t msg[128] = {0};
//    		sprintf(msg, "u16ElementCount = %d, u16Length = %d, u8ElementSize = %d, u16BufferSize = %d",
//    				pRingBuff->u16ElementCount ,
//					u16Length,
//					pRingBuff->u8ElementSize,
//					pRingBuff->u16BufferSize);
//
//    		DEBUG(msg);
//    	}
        return FALSE;
    }

    // Lock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->lock();
//    }

//    Semaphore_pend(pRingBuff->busySemHandle, 0);
    // Calculate the start address for pushing in
    pu8Buffer = (uint8_t*)pRingBuff->pBuffer + pRingBuff->u16BufferPushPtr;// * psRingBuffer2->uiElementSize;

    // If the pushing address is out of address range of the buffer then we need to push twice
    if ((pRingBuff->u16BufferPushPtr + u16Length*pRingBuff->u8ElementSize) > pRingBuff->u16BufferSize)
    {
        // Push data stream from start address to the final address of the buffer
        memcpy(pu8Buffer, pvStream, (pRingBuff->u16BufferSize - pRingBuff->u16BufferPushPtr));

        // Return the start address to the first address of the buffer for pushing the rest of stream
        pu8Buffer = (uint8_t*)pRingBuff->pBuffer;

        // Point to address of the rest of stream
        pu8RestStream = (uint8_t*)pvStream + (pRingBuff->u16BufferSize - pRingBuff->u16BufferPushPtr);

        // Push the rest of stream
        memcpy(pu8Buffer, pu8RestStream, u16Length*pRingBuff->u8ElementSize + pRingBuff->u16BufferPushPtr - pRingBuff->u16BufferSize);
    }
    else
    {
        // Push data stream
        memcpy(pu8Buffer, pvStream, u16Length*pRingBuff->u8ElementSize);
    }

    // Point the push pointer to the new position
    pRingBuff->u16BufferPushPtr += u16Length*pRingBuff->u8ElementSize;
    if (pRingBuff->u16BufferPushPtr >= pRingBuff->u16BufferSize)
    {
        pRingBuff->u16BufferPushPtr -= pRingBuff->u16BufferSize;
    }

    // Increase element count of the buffer
    pRingBuff->u16ElementCount += u16Length;

    // Unlock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->unlock();
//    }
    //Semaphore_post(pRingBuff->busySemHandle);
    return TRUE;	// Push successfully
}

uint16_t buffer_pop_stream(ring_buff_t* pRingBuff, void* pvStream, uint16_t u16Length)
{
    uint16_t	u16PopCount		= 0;
    uint8_t*	pu8Buffer		= NULL;
    uint8_t*    pu8RestStream	= NULL;

    if ((pRingBuff->u16ElementCount == 0) || (pRingBuff->bBufferPopEnable == FALSE))
    {
        return 0;
    }

    // Lock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->lock();
//    }
    //Semaphore_pend(pRingBuff->busySemHandle, 0);
    // Limit length of data stream will be popped
    if (u16Length < pRingBuff->u16ElementCount)
    {
        u16PopCount = u16Length;
    }
    else
    {
        u16PopCount = pRingBuff->u16ElementCount;
    }

    // Calculate the start address for popping out
    pu8Buffer = (uint8_t*)pRingBuff->pBuffer + pRingBuff->u16BufferPopPtr ;

    // If the popping address is out of address range of the buffer then we need to pop twice
    if ((pRingBuff->u16BufferPopPtr + u16PopCount*pRingBuff->u8ElementSize) > pRingBuff->u16BufferSize)
    {
        // Pop data stream from start address to the final address of the buffer
        memcpy(pvStream, pu8Buffer,pRingBuff->u16BufferSize - pRingBuff->u16BufferPopPtr);

        // Return the start address to the first address of the buffer for popping the rest of stream
        pu8Buffer = (uint8_t*)pRingBuff->pBuffer;

        // Point to address of the rest of stream
        pu8RestStream = (uint8_t*)pvStream + pRingBuff->u16BufferSize - pRingBuff->u16BufferPopPtr;

        // Pop the rest of stream
        memcpy(pu8RestStream, pu8Buffer, u16PopCount*pRingBuff->u8ElementSize + pRingBuff->u16BufferPopPtr - pRingBuff->u16BufferSize);
    }
    else
    {
        // Pop data stream
        memcpy(pvStream, pu8Buffer, u16PopCount*pRingBuff->u8ElementSize);
    }

    // Point the pop pointer to the new position
    pRingBuff->u16BufferPopPtr += u16PopCount*pRingBuff->u8ElementSize;
    if (pRingBuff->u16BufferPopPtr >= pRingBuff->u16BufferSize)
    {
        pRingBuff->u16BufferPopPtr -= pRingBuff->u16BufferSize;
    }

    // Decrease element count of the buffer
    pRingBuff->u16ElementCount -= u16PopCount;

    // Unlock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->unlock();
//    }
    //Semaphore_post(pRingBuff->busySemHandle);
    return u16PopCount;	// Return the number of elements popped out actually
}

void buffer_flush(ring_buff_t* pRingBuff)
{
    // Lock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->lock();
//    }
    //Semaphore_pend(pRingBuff->busySemHandle, 0);
    pRingBuff->u16ElementCount 	= 0;
    pRingBuff->u16BufferPopPtr	= 0;
    pRingBuff->u16BufferPushPtr	= 0;
    pRingBuff->bBufferPopEnable	= TRUE;
    pRingBuff->bBufferPushEnable = TRUE;
    memset(pRingBuff->pBuffer, 0, pRingBuff->u16BufferSize / pRingBuff->u8ElementSize);
    // Unlock accessing to the buffer
//    if (m_pMutex)
//    {
//        m_pMutex->unlock();
//    }
    //Semaphore_post(pRingBuff->busySemHandle);
}

//BOOL BufferPushBack(RingBuffTypeDef_t *pRingBuff, uint16_t u16PushBackNumber)
//{
//    // Cannot push back if the push back number is greater than the available element count
//    if (u16PushBackNumber > (pRingBuff->u16BufferSize - pRingBuff->u16ElementCount))
//    {
//        return FALSE;
//    }
//
//    // Lock accessing to the buffer
////    if (m_pMutex)
////    {
////        m_pMutex->lock();
////    }
//    // Disable pushing to prohibiting changing of push pointer
//    pRingBuff->bBufferPushEnable = FALSE;
//
//    // Push back element count
//    pRingBuff->u16ElementCount += u16PushBackNumber;
//
//    // Push back pop pointer
//    if (pRingBuff->u16BufferPopPtr >= u16PushBackNumber)
//    {
//        pRingBuff->u16BufferPopPtr -= u16PushBackNumber;
//    }
//    else
//    {
//        pRingBuff->u16BufferPopPtr = pRingBuff->u16BufferPopPtr + pRingBuff->u16BufferSize - u16PushBackNumber;
//    }
//
//    // Enable pushing
//    pRingBuff->bBufferPushEnable = TRUE;
//
//    // Unlock accessing to the buffer
////    if (m_pMutex)
////    {
////        m_pMutex->unlock();
////    }
//
//    return TRUE;
//}

//}

uint16_t        buffer_get_count(ring_buff_t* pRingBuff)
{
    uint16_t ret = 0;
    //Semaphore_pend(pRingBuff->busySemHandle, 0);
    ret =pRingBuff->u16ElementCount;
    //Semaphore_post(pRingBuff->busySemHandle);
    return ret;
}

uint8_t			buffer_get_data(ring_buff_t* pRingBuff, void* data, uint16_t* len)
{
	uint8_t ret = 0xff;

//	if((!pRingBuff) || (!data) || (*len > ( pRingBuff->u16BufferSize/pRingBuff->u8ElementSize )))
//	{
//		return ret;
//	}

	//Semaphore_pend(pRingBuff->busySemHandle, 0);
	*len = pRingBuff->u16ElementCount;
	uint16_t u16PopCount = pRingBuff->u16ElementCount;
	uint8_t* pu8Buffer = (uint8_t*)pRingBuff->pBuffer + pRingBuff->u16BufferPopPtr ;
	uint8_t* pu8RestStream = (uint8_t*)data;


//	uint8_t msg[128] = {0};
//	sprintf(msg, "u16ElementCount = %d, u16PopCount = %d, u8ElementSize = %d, u16BufferSize = %d",
//			pRingBuff->u16ElementCount ,
//			u16PopCount,
//			pRingBuff->u8ElementSize,
//			pRingBuff->u16BufferSize);
//
//	DEBUG(msg);

	if ((pRingBuff->u16BufferPopPtr + u16PopCount*pRingBuff->u8ElementSize) > pRingBuff->u16BufferSize)
	{
		// Pop data stream from start address to the final address of the buffer
		memcpy(pu8RestStream, pu8Buffer,pRingBuff->u16BufferSize - pRingBuff->u16BufferPopPtr);

		// Return the start address to the first address of the buffer for popping the rest of stream
		pu8Buffer = (uint8_t*)pRingBuff->pBuffer;

		// Point to address of the rest of stream
		pu8RestStream = (uint8_t*)data + pRingBuff->u16BufferSize - pRingBuff->u16BufferPopPtr;

		// Pop the rest of stream
		memcpy(pu8RestStream, pu8Buffer, u16PopCount*pRingBuff->u8ElementSize + pRingBuff->u16BufferPopPtr - pRingBuff->u16BufferSize);
	}
	else
	{
		// Pop data stream
		memcpy(pu8RestStream, pu8Buffer, u16PopCount*pRingBuff->u8ElementSize);
	}
	//Semaphore_post(pRingBuff->busySemHandle);

	ret = 0;
	return ret;
}



uint16_t        buffer_get_available_count(ring_buff_t* pRingBuff)
{
    uint16_t ret = 0;
    //Semaphore_pend(pRingBuff->busySemHandle, 0);
    ret = pRingBuff->u16BufferSize - pRingBuff->u16ElementCount;
    //Semaphore_post(pRingBuff->busySemHandle);
    return ret;
}

void            buffer_pop_enable(ring_buff_t* pu8Buffer)
{
    pu8Buffer->bBufferPopEnable = TRUE;
}


void            buffer_pop_disable(ring_buff_t* pu8Buffer)
{
    pu8Buffer->bBufferPopEnable = FALSE;
}

void            buffer_push_enable(ring_buff_t* pu8Buffer)
{
    pu8Buffer->bBufferPushEnable = TRUE;
}

void            buffer_push_disable(ring_buff_t* pu8Buffer)
{
    pu8Buffer->bBufferPushEnable = FALSE;
}

BOOL            buffer_is_push_enable(ring_buff_t* pu8Buffer)
{
    return pu8Buffer->bBufferPushEnable;
}

BOOL            buffer_is_pop_enable(ring_buff_t* pu8Buffer)
{
    return pu8Buffer->bBufferPopEnable;
}


//#endif

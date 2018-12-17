
/*
 *      File name:      linked_list.c
 *
 *      Created on:     Aug 20, 2018
 *      Author:         manhbt
 *      Brief:          A link list library of data stream. This link list library can be used for all data
 *                      type: 8-bit, 16, 32,64-bit, structure data types....
 */

////////////////////////////////////////// Include Files /////////////////////////////////////////////
#include "linked_list.h"
//#include "Debug.h"



///////////////////////////////////// Constant Definitions ///////////////////////////////////////////

/////////////////////////////////////// Type Definitions /////////////////////////////////////////////

///////////////////////////// Macros (Inline Functions) Definitions //////////////////////////////////

///////////////////////////////////// Function Prototypes ////////////////////////////////////////////

///////////////////////////////////// Variable Definitions ///////////////////////////////////////////

///////////////////////////////////// Function implements ////////////////////////////////////////////

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Initialize the link list structure
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	uiListSize is the maximum size of the link list
//	@return	: 	Void
//	@note	:
// ---------------------------------------------------------------------------------------------------
void ListInit( SList *psList, unsigned int uiListSize)
{
//	ASSERT_VOID(psList);
    if(!psList) return;
	psList->pNodeHead		= NULL;
	psList->pNodeTail		= NULL;
	psList->uiListSize		= uiListSize;
	psList->uiElementCount	= 0;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get the number of elements of link list
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	The number of elements of link list
//	@note	:
// ---------------------------------------------------------------------------------------------------
unsigned int ListGetCount( SList *psList )
{
	return psList->uiElementCount;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Check if the list is empty
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	TRUE if the list is empty and vice versa
//	@note	:
// ---------------------------------------------------------------------------------------------------
BOOL ListIsEmpty( SList *psList )
{
	return (psList->uiElementCount == 0);
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Check if the list is full
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	TRUE if the list is full and vice versa
//	@note	:
// ---------------------------------------------------------------------------------------------------
BOOL ListIsFull( SList *psList )
{
	return (psList->uiElementCount == psList->uiListSize);
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Add a new node to the head
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvNewElement is the pointer to new element
//	@return	: 	Pointer to the created node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListAddHead( SList *psList, void *pvNewElement )
{
	SNode *pNewNode;
	
    pNewNode = (SNode*) malloc(sizeof(SNode));

	if( pNewNode == NULL )
	{
		return NULL;
	}
	
	pNewNode->pvData = pvNewElement;

	if( psList->pNodeHead != NULL )
	{
		pNewNode->pPrev				= NULL;
		pNewNode->pNext				= psList->pNodeHead;
		psList->pNodeHead->pPrev	= pNewNode;
	}
	else	// This is the first node
	{
		pNewNode->pPrev				= NULL;
		pNewNode->pNext				= NULL;
		psList->pNodeHead			= pNewNode;
		psList->pNodeTail			= pNewNode;
	}

	psList->pNodeHead = pNewNode;

	psList->uiElementCount++;
	
	return (void*)pNewNode;		 
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Add a new element to the tail
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvNewElement is the pointer to new element
//	@return	: 	Pointer to the created node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListAddTail( SList *psList, void* pvNewElement )
{
	SNode *pNewNode;
	
    pNewNode = (SNode*) malloc(sizeof(SNode));

	if( pNewNode == NULL )
	{
		return NULL;
	}
	
	pNewNode->pvData = pvNewElement;

	if( psList->pNodeTail != NULL )
	{
		pNewNode->pPrev				= psList->pNodeTail;
		pNewNode->pNext				= NULL;
		psList->pNodeTail->pNext	= pNewNode;
	}
	else	// This is the first node
	{
		pNewNode->pPrev				= NULL;
		pNewNode->pNext				= NULL;
		psList->pNodeHead			= pNewNode;
		psList->pNodeTail			= pNewNode;
	}
	
	psList->pNodeTail = pNewNode;

	psList->uiElementCount++;
	
	return (void*)pNewNode;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Insert a new element before a specified node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvBeforeNode is the pointer to the node that we would like to add a new node before it
//	@param	: 	pvNewElement is the pointer to the data of the new node
//	@return	: 	Pointer to the new node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListInsertBefore( SList *psList, void* pvBeforeNode, void* pvNewElement )
{
	SNode *pOldNode;
	SNode *pNewNode;

	if( pvBeforeNode == NULL )
	{
		return ListAddHead(psList, pvNewElement);
	}

	pOldNode = (SNode *) pvBeforeNode;
    pNewNode = (SNode *) malloc(sizeof(SNode));

	if( pNewNode == NULL ) 
	{
		return NULL;
	}

	pNewNode->pvData = pvNewElement;

	if( pOldNode->pPrev != NULL ) 
	{
		pNewNode->pPrev			= pOldNode->pPrev;
		pNewNode->pNext			= pOldNode;
		pOldNode->pPrev->pNext	= pNewNode;
	}
	else	// Before node is head
	{
		pNewNode->pPrev			= NULL;
		pNewNode->pNext			= pOldNode;
		psList->pNodeHead		= pNewNode;
	}

	pOldNode->pPrev = pNewNode;

	psList->uiElementCount++;

	return (void*)pNewNode;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Insert a new element after a specified node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvAfterNode is the pointer to the node that we would like to add a new node after it
//	@param	: 	pvNewElement is the pointer to the data of the new node
//	@return	: 	Pointer to the new node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListInsertAfter ( SList *psList, void* pvAfterNode, void* pvNewElement )
{
	SNode *pOldNode;
	SNode *pNewNode;

	if( pvAfterNode == NULL )
	{
		return ListAddTail(psList, pvNewElement);
	}

	pOldNode = (SNode *) pvAfterNode;
    pNewNode = (SNode *) malloc(sizeof(SNode));

	if( pNewNode == NULL ) 
	{
		return NULL;
	}

	pNewNode->pvData = pvNewElement;

	if( pOldNode->pNext != NULL ) 
	{
		pNewNode->pPrev			= pOldNode;
		pNewNode->pNext			= pOldNode->pNext;
		pOldNode->pNext->pPrev	= pNewNode;
	}
	else	// Before node is tail
	{
		pNewNode->pPrev			= pOldNode;
		pNewNode->pNext			= NULL;
		psList->pNodeTail		= pNewNode;
	}

	pOldNode->pNext = pNewNode;

	psList->uiElementCount++;

	return (void*)pNewNode;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Remove the head and return its data
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	Pointer to the data of removed head
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListRemoveHead( SList *psList )
{
	SNode	*pOldNode;
	void	*pvRetData;
	
//	ASSERT_NONVOID(psList && psList->pNodeHead, NULL);

	pOldNode	= psList->pNodeHead;
	pvRetData	= pOldNode->pvData;
	
	psList->pNodeHead = pOldNode->pNext;

	if( psList->pNodeHead != NULL )
	{
		psList->pNodeHead->pPrev = NULL;
	}
	else	// List is empty
	{
		psList->pNodeTail = NULL;
	}
		
    free((unsigned char*)pOldNode);

	psList->uiElementCount--;
	
	return pvRetData;	 
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Remote the tail and return its data
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	Pointer to the data of removed tail
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListRemoveTail( SList *psList )
{
	SNode	*pOldNode;
	void	*pvRetData;

//	ASSERT_NONVOID(psList && psList->pNodeTail, NULL);
	
	pOldNode	= psList->pNodeTail;
	pvRetData	= pOldNode->pvData;
	
	psList->pNodeTail = pOldNode->pPrev;

	if( psList->pNodeTail != NULL )
	{
		psList->pNodeTail->pNext = NULL;
	}
	else	// List is empty
	{
		psList->pNodeHead = NULL;
	}
		
    free((unsigned char*)pOldNode);

	psList->uiElementCount--;

	return pvRetData;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Remove an element at a specified node and return its data
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvRemovedNode is the pointer to the node that we would like to remove
//	@return	: 	Pointer to the data of removed node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListRemoveAt( SList *psList, void* pvRemovedNode )
{
	SNode	*pOldNode;
	void	*pvRetData;

//	ASSERT_NONVOID(psList && pvRemovedNode, NULL);

	pOldNode	= (SNode *)pvRemovedNode;
	pvRetData	= pOldNode->pvData;

	if( pOldNode == psList->pNodeHead )
	{
		psList->pNodeHead = pOldNode->pNext;
	}
	else
	{
		pOldNode->pPrev->pNext	= pOldNode->pNext;
	}
	
	if( pOldNode == psList->pNodeTail ) 
	{
		psList->pNodeTail = pOldNode->pPrev;
	}
	else
	{
		pOldNode->pNext->pPrev	= pOldNode->pPrev;
	}

    free((unsigned char*)pOldNode);

	psList->uiElementCount--;

	return pvRetData;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Remove all list
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	Void
//	@note	:
// ---------------------------------------------------------------------------------------------------
void ListRemoveAll( SList *psList )
{
	while (ListGetCount(psList) > 0)
	{
		ListRemoveTail(psList);
	}
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get head pointer without remove it
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	Pointer to the head node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListGetHead( SList *psList )
{
	return (void*)psList->pNodeHead;	
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get tail pointer without remove it
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	Pointer to the tail node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListGetTail( SList *psList )
{
	return (void*)psList->pNodeTail;	
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get value at current node then jump to next node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	ppvGetNode is the pointer to the pointer of the node which we would like to get its
//				data.
//	@return	: 	Pointer to the data of current node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListGetNext( SList *psList, void** ppvGetNode )
{
	SNode *pNode = (SNode *)(*ppvGetNode);

//	ASSERT_NONVOID(psList && pNode, NULL);
	
	*ppvGetNode = (void*) pNode->pNext;

	return pNode->pvData;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get value at current node then jump to previous node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	ppvGetNode is the pointer to the pointer of the node which we would like to get its
//				data.
//	@return	: 	Pointer to the data of current node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListGetPrev( SList *psList, void** ppvGetNode )
{
	SNode *pNode = (SNode *)(*ppvGetNode);

//	ASSERT_NONVOID(psList && pNode, NULL);
	
	*ppvGetNode = (void*) pNode->pPrev;

	return pNode->pvData;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get data stored in the head node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	Pointer to the data stored in the head node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListPeekHead( SList *psList )
{
//	ASSERT_NONVOID(psList && psList->pNodeHead, NULL);

	return psList->pNodeHead->pvData;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get data stored in the tail node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@return	: 	Pointer to the data stored in the tail node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListPeekTail( SList *psList )
{
//	ASSERT_NONVOID(psList && psList->pNodeTail, NULL);

	return psList->pNodeTail->pvData;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get data of a specified node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvNode is the pointer to the node that we would like to get its data
//	@return	: 	Pointer to the data of this node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListPeekAt( SList *psList, void* pvNode )
{
//	ASSERT_NONVOID(psList && pvNode, NULL);

	return ((SNode*)pvNode)->pvData;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Set data to a specified node
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvNode is the pointer to the node that we would like to set new data
//	@param	: 	pvNewElement is the data we would like to set for this node
//	@return	: 	Void
//	@note	:
// ---------------------------------------------------------------------------------------------------
void ListSetAt( SList *psList, void* pvNode, void* pvSetElement )
{
//	ASSERT_VOID(psList && pvNode);

	((SNode*)pvNode)->pvData = pvSetElement;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Find a node having a specified data value
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	pvSearchData is the pointer to searched data
//	@param	: 	pvStartAfterNode is the pointer to the node that we start for searching
//	@return	: 	Pointer to the found node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListFind( SList *psList, void* pvSearchData, void* pvStartAfterNode )
{
	SNode *pNode;
	
	if( pvStartAfterNode == NULL )
	{
		pNode = psList->pNodeHead;
	}
	else
	{
		pNode = ((SNode *)pvStartAfterNode)->pNext;
	}
		
	for( ; pNode != NULL; pNode = pNode->pNext ) 
	{
		if(	pNode->pvData == pvSearchData ) 
		{
			return (void*)pNode;
		}
	}	
	
	return NULL;
}

// ---------------------------------------------------------------------------------------------------
//	@brief	: 	Get the node at the specified index
//		  
//	@param	: 	psList is the pointer to the link list structure
//	@param	: 	uiIndex is one of the element count
//	@return	: 	Pointer to the found node
//	@note	:
// ---------------------------------------------------------------------------------------------------
void* ListFindIndex( SList *psList, unsigned int uiIndex )
{
	SNode *pNode;
	
	if( uiIndex >= psList->uiElementCount )
	{
		return NULL;
	}
		
	pNode = psList->pNodeHead;
	
	while( uiIndex-- ) 
	{
		pNode = pNode->pNext;
	}	

	return (void*)pNode;	
}

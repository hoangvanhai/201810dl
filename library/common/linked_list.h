/*
 *      File name:      linked_list.h
 *
 *      Created on:     Aug 20, 2018
 *      Author:         manhbt
 *      Brief:          A link list library of data stream. This link list library can be used for all data
 *                      type: 8-bit, 16, 32,64-bit, structure data types....
 */


#ifndef _LINK_LIST_H_
#define _LINK_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////// Include Files /////////////////////////////////////////////
#include "typedefs.h"
#include <string.h>
#include <stdlib.h>
//#include "RTOSHelper.h"

///////////////////////////////////// Constant Definitions ///////////////////////////////////////////

/////////////////////////////////////// Type Definitions /////////////////////////////////////////////
// Node struct
typedef struct S_NODE 
{
	struct S_NODE*		pNext;
	struct S_NODE*		pPrev;
	void*				pvData;

} SNode;

// List struct
typedef struct S_LINK_LIST 
{
	SNode*				pNodeHead;
	SNode*				pNodeTail;
	unsigned int		uiListSize;
	unsigned int		uiElementCount;
	
} SList;

///////////////////////////// Macros (Inline Functions) Definitions //////////////////////////////////

///////////////////////////////////// Function Prototypes ////////////////////////////////////////////
// API Function of Link List library

// Initialize Link List
extern void				ListInit( SList *psList, unsigned int uiListSize);

// Utilities
extern unsigned int		ListGetCount( SList *psList );
extern BOOL				ListIsEmpty( SList *psList );
extern BOOL				ListIsFull( SList *psList );

// Add, Insert new node
extern void*			ListAddHead( SList *psList, void *pvNewElement );
extern void*			ListAddTail( SList *psList, void* pvNewElement );
extern void*			ListInsertBefore( SList *psList, void* pvBeforeNode, void* pvNewElement );
extern void*			ListInsertAfter ( SList *psList, void* pvAfterNode, void* pvNewElement );

// Remove, delete node & return its data
extern void*			ListRemoveHead( SList *psList );
extern void*			ListRemoveTail( SList *psList );
extern void*			ListRemoveAt( SList *psList, void* pvRemovedNode );
extern void				ListRemoveAll( SList *psList );

// Get node & node data
extern void*			ListGetHead( SList *psList );
extern void*			ListGetTail( SList *psList );
extern void*			ListGetNext( SList *psList, void** ppvGetNode );
extern void*			ListGetPrev( SList *psList, void** ppvGetNode );

// Peek node, Set node data
extern void*			ListPeekHead( SList *psList );
extern void*			ListPeekTail( SList *psList );
extern void*			ListPeekAt( SList *psList, void* pvNode );
extern void				ListSetAt( SList *psList, void* pvNode, void* pvSetElement );

// Find, search
extern void*			ListFind( SList *psList, void* pvSearchData, void* pvStartAfterNode );
extern void*			ListFindIndex( SList *psList, unsigned int uiIndex );

///////////////////////////////////// Variable Definitions ///////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif	// _LINK_LIST_H_

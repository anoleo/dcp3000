#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "queue.h"


void *list_dequeue(list_st *plist)
{
	void *pdata = NULL;
	snode_st *snode = NULL;

	snode = list_dequeue_node(plist);

	if(snode){
		pdata = snode->data;
		snode->data = NULL;
		free_snode(snode);
	}
		
	return pdata;
}

void *duList_dequeue(duList_st *pduList)
{
	void *pdata = NULL;
	dnode_st *dnode = NULL;

	dnode = duList_dequeue_node(pduList);

	if(dnode){
		pdata = dnode->data;
		dnode->data = NULL;
		free_dnode(dnode);
	}
		
	return pdata;
}
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "stack.h"


void *list_outstack(list_st *plist)
{
	void *pdata = NULL;
	snode_st *snode = NULL;
	
	snode = list_outstack_node(plist);

	if(snode){
		pdata = snode->data;
		snode->data = NULL;
		free_snode(snode);
	}
	
	return pdata;
}


void *duList_outstack(duList_st *pduList)
{
	void *pdata = NULL;
	dnode_st *dnode = NULL;
	
	dnode = duList_outstack_node(pduList);

	if(dnode){
		pdata = dnode->data;
		dnode->data = NULL;
		free_dnode(dnode);
	}
	
	return pdata;
}



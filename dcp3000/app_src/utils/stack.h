#ifndef __STACK_H__
#define __STACK_H__

#include "list.h"
#include "duList.h"

//push
#define list_instack(plist, pdata, data_flag)		list_insert(plist,  pdata, data_flag)
#define duList_instack(pduList, pdata, data_flag)	duList_insert(pduList,  pdata, data_flag)

//pop
#define list_outstack_node(plist)			list_separate_head(plist)
#define duList_outstack_node(pduList)		duList_separate_head(pduList)

//pop
void *list_outstack(list_st *plist);
void *duList_outstack(duList_st *pduList);

#endif
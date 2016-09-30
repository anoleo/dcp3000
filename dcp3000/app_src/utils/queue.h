#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "list.h"
#include "duList.h"

#define list_enqueue(plist, pdata, data_flag)	list_add(plist,  pdata, data_flag)
#define duList_enqueue(pduList, pdata, data_flag)	duList_add(pduList,  pdata, data_flag)

#define list_dequeue_node(plist)		list_separate_head(plist)
#define duList_dequeue_node(pduList)		duList_separate_head(pduList)

void *list_dequeue(list_st *plist);
void *duList_dequeue(duList_st *pduList);

#endif


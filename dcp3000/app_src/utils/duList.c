#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "duList.h"

void init_duList(duList_st *pduList, int data_type)
{
	if(pduList == NULL){
		return ;
	}
	
	INIT_DULIST(pduList);
	pduList->data_type = data_type;
}

duList_st *create_duList(int data_type)
{
	duList_st *pduList;

	pduList = malloc(sizeof(duList_st));
	if(pduList == NULL){
		perror("duList_st\n");
		exit(-1);
	}

	INIT_DULIST(pduList);
	pduList->data_type = data_type;

	return pduList;
}

void duList_empty(duList_st *pduList)
{
	dnode_st *dnode, *next_dnode;

	
	if(pduList == NULL){
		return ;
	}
	
	wrlock_duList(pduList);

	dnode = pduList->head;
	while(dnode){
		next_dnode = dnode->next;
		free_dnode(dnode);
		dnode = next_dnode;
	}

	pduList->count = 0;
	pduList->head = NULL;
	pduList->tail = NULL;

	unlock_duList(pduList);
	
}

void destroy_duList(duList_st *pduList)
{
	dnode_st *dnode, *next_dnode;

	
	if(pduList == NULL){
		return;
	}
	
	duList_empty(pduList);

	pthread_rwlock_destroy(&pduList->rwlock);
	pthread_mutex_destroy(&pduList->lock);

	free(pduList);
}


int duList_add_node(duList_st *pduList, dnode_st *dnode)
{
	int count;
	
	if(pduList == NULL){
		return -1;
	}

	wrlock_duList(pduList);

	if(pduList->count >= pduList->max_size){
		unlock_duList(pduList);
		return -1;
	}
	
	if(pduList->count == 0){
		pduList->head = dnode;
	}else{
		pduList->tail->next = dnode;
		dnode->prev = pduList->tail;
	}

	pduList->count++;
	pduList->tail = dnode;

	dnode->header.parent = pduList;

	count = pduList->count;
	
	unlock_duList(pduList);

	return count;
}

int duList_insert_node(duList_st *pduList, dnode_st *dnode)
{
	int count;
	
	if(pduList == NULL){
		return -1;
	}

	wrlock_duList(pduList);

	if(pduList->count >= pduList->max_size){
		unlock_duList(pduList);
		return -1;
	}
	
	if(pduList->count == 0){
		pduList->tail = dnode;
	}else{
		pduList->head->prev = dnode;
		dnode->next = pduList->head;
	}

	pduList->count++;
	pduList->head = dnode;

	dnode->header.parent = pduList;

	count = pduList->count;
	
	unlock_duList(pduList);

	return count;
}

dnode_st *duList_separate_node(duList_st *pduList, dnode_st *dnode)
{

	if((pduList == NULL) || (dnode == NULL) || (pduList->count <= 0)){
		return NULL;
	}

	if(dnode->header.parent != pduList){
		return NULL;
	}

	wrlock_duList(pduList);
	
	if(pduList->count == 1){
		
		if(pduList->head == dnode){
			pduList->head = NULL;
			pduList->tail = NULL;

			pduList->count = 0;
		}
		
	}else if(pduList->count > 1){
	
		if(pduList->head == dnode){
			pduList->head = dnode->next;
			pduList->head->prev = NULL;
		}else if(pduList->tail == dnode){
			pduList->tail = dnode->prev;
			pduList->tail->next = NULL;
		}else{
			dnode->prev->next = dnode->next;
			dnode->next->prev = dnode->prev;
		}

		pduList->count--;
	}
	
	unlock_duList(pduList);

	dnode->header.parent = NULL;
	dnode->prev = NULL;
	dnode->next = NULL;

	return dnode;
}

int duList_del_node(duList_st *pduList, dnode_st *dnode)
{

	if(duList_separate_node(pduList, dnode)){
		free_dnode(dnode);	
		return pduList->count;
	}else{
		return -1;
	}

}

dnode_st *duList_separate_head(duList_st *pduList)
{
	dnode_st *dnode = NULL;
	
	if((pduList == NULL)){
		return NULL;
	}

	#if 1
	wrlock_duList(pduList);
	
	if(pduList->count > 0){

		dnode = pduList->head;

		pduList->head = dnode->next;
		
		if(dnode == pduList->tail){
			pduList->tail = NULL;
		}else{
			pduList->head->prev = NULL;
		}
		
		pduList->count--;

	}

	dnode->header.parent = NULL;
	dnode->prev = NULL;
	dnode->next = NULL;
	
	unlock_duList(pduList);
	#else
	dnode = duList_separate_head(pduList, pduList->head);
	#endif
		
	return dnode;
}

dnode_st *duList_separate_tail(duList_st *pduList)
{
	dnode_st *dnode = NULL;
	
	if((pduList == NULL)){
		return NULL;
	}

	#if 1
	wrlock_duList(pduList);
	
	if(pduList->count > 0){
		
		dnode = pduList->tail;

		pduList->tail = dnode->prev;
		
		if(pduList->head == dnode){
			pduList->head = NULL;
		}else{
			pduList->tail->next = NULL;
		}

		pduList->count--;
	}

	dnode->header.parent = NULL;
	dnode->prev = NULL;
	dnode->next = NULL;
	
	unlock_duList(pduList);
	#else
	dnode = duList_separate_head(pduList, pduList->tail);
	#endif
	
	return dnode;
		
}

int dnode_insert(dnode_st *dnode, dnode_st *curdnode)
{

	duList_st *pduList;

	if((curdnode == NULL) || (dnode == NULL)){
		return -1;
	}

	if(dnode->header.parent != curdnode->header.parent){
		return -1;
	}

	pduList = (duList_st *)(dnode->header.parent);

	wrlock_duList(pduList);


	curdnode->prev = dnode->prev;
	curdnode->next = dnode;
	
	if(dnode->prev){
		dnode->prev->next = curdnode;
	}else{
		pduList->head = curdnode;
	}

	dnode->prev = curdnode;

	pduList->count++;

	unlock_duList(pduList);

	return 0;
}

int dnode_append(dnode_st *dnode, dnode_st *curdnode)
{

	duList_st *pduList;

	if((curdnode == NULL) || (dnode == NULL)){
		return -1;
	}

	if(dnode->header.parent != curdnode->header.parent){
		return -1;
	}

	pduList = (duList_st *)(dnode->header.parent);

	wrlock_duList(pduList);


	curdnode->prev = dnode;
	curdnode->next = dnode->next;
	
	if(dnode->next){
		dnode->next->prev = curdnode;
	}else{
		pduList->tail = curdnode;
	}

	dnode->next = curdnode;

	pduList->count++;

	unlock_duList(pduList);

	return 0;
}


void * duList_head(duList_st *pduList)
{
	if((pduList == NULL) || (pduList->head == NULL)){
		return NULL;
	}
	
	return pduList->head->data;
}

void * duList_tail(duList_st *pduList)
{
	if((pduList == NULL) || (pduList->tail == NULL)){
		return NULL;
	}
	
	return pduList->tail->data;
}

dnode_st *duList_get_node(duList_st *pduList, int index)
{
	int count = 0;
	dnode_st *dnode;

	if((pduList == NULL) || (index < 0)){
		return NULL;
	}

	rdlock_duList(pduList);

	if(index < pduList->count){
		dnode = pduList->head;
		while(dnode){
			if(count == index){
				break;
			}
			count++;
			dnode = dnode->next;
		}
	}

	unlock_duList(pduList);

	
	return dnode;
	
}

dnode_st *duList_set_node(duList_st *pduList, dnode_st *dnode, void *pdata, int data_flag)
{

	if((pduList == NULL) || (dnode == NULL)){
		return NULL;
	}

	if(VOID_DATA != pduList->data_type){
		data_flag |= DATA_TYPE_FLAG(pduList->data_type);
	}
	dnode_reset_data(dnode, pdata, data_flag);
	
	return dnode;
}

dnode_st *duList_create_node(duList_st *pduList, void *pdata, int data_flag)
{
	dnode_st *dnode;
	
	if(pduList == NULL){
		return NULL;
	}

	if(VOID_DATA != pduList->data_type){
		data_flag |= DATA_TYPE_FLAG(pduList->data_type);
	}
	dnode = create_dnode(pdata, data_flag);

	dnode->header.parent = pduList;
	
	return dnode;
}

int duList_add(duList_st *pduList, void *pdata, int data_flag)
{
	return duList_add_node(pduList, duList_create_node(pduList, pdata, data_flag));
}

int duList_addto(duList_st *pduList, int index, void *pdata, int data_flag)
{
	int count = -1;
	dnode_st *dnode, *curdnode;
	
	if((pduList == NULL) || (index < 0)){
		return -1;
	}

	dnode =duList_create_node(pduList, pdata, data_flag);
	if(index==0){
		count = duList_insert_node(pduList, dnode);
	}else if(index>0){
		curdnode = duList_get_node(pduList, index-1);
		count = dnode_append(dnode,  curdnode);
	}

	return count;
}

int duList_insert(duList_st *pduList, void *pdata, int data_flag)
{
	return duList_insert_node(pduList, duList_create_node(pduList, pdata, data_flag));
}

void duList_traverse(duList_st *pduList, int (*handler)(void *, void *) , void *arg)
{
	int count, res;
	dnode_st *dnode;
	
	if((pduList == NULL) ||(handler == NULL)){
		return ;
	}

	wrlock_duList(pduList);

	dnode = pduList->head;
	while(dnode){
		if(handler(dnode->data, arg) != 0)
			break;

		dnode = dnode->next;
	}
	
	unlock_duList(pduList);

}

dnode_st *duList_find_node(duList_st *pduList, void *pdata, compare_func compare)
{
	int count, res;
	dnode_st *dnode;
	
	if((pduList == NULL) || (pdata == NULL)){
		return NULL;
	}

	rdlock_duList(pduList);

	dnode = pduList->head;
	while(dnode){
		if(compare){
			if(compare(dnode->data, pdata) == 0)
				break;
		}else{
			res = node_equals(dnode, pdata);
			if(res == 1){
				break;
			}else if(res == -1){
				dnode = NULL;
				break;
			}
		}
		dnode = dnode->next;
	}
	
	unlock_duList(pduList);

	return dnode;
}


void *duList_find(duList_st *pduList, void *pdata, compare_func compare)
{
	dnode_st *dnode;

	dnode = duList_find_node(pduList, pdata, compare);

	if(dnode){
		return dnode->data;
	}else{
		return NULL;
	}

	return dnode;
}

int duList_indexof(duList_st *pduList, void *pdata, compare_func compare)
{
	int index = -1, res;
	dnode_st *dnode;
	
	if((pduList == NULL) || (pdata == NULL)){
		return -1;
	}

	rdlock_duList(pduList);

	dnode = pduList->head;
	while(dnode){
		index++;
		if(compare){
			if(compare(dnode->data, pdata) == 0)
				break;
		}else{
			res = node_equals(dnode, pdata);
			if(res == 1){
				break;
			}else if(res == -1){
				index = -1;
				break;
			}
		}
		dnode = dnode->next;
	}
	
	unlock_duList(pduList);

	return index;
}


int duList_del(duList_st *pduList, void *pdata, compare_func compare)
{
	return duList_del_node(pduList, duList_find(pduList, pdata, compare));
}

int duList_remove(duList_st *pduList, int index)
{
	return duList_del_node(pduList, duList_get_node(pduList, index));
}


void *duList_get(duList_st *pduList, int index)
{

	dnode_st *dnode;

	dnode = duList_get_node(pduList, index);

	if(dnode){
		return dnode->data;
	}else{
		return NULL;
	}
}


dnode_st *duList_set(duList_st *pduList, int index, void *pdata, int data_flag)
{
	return duList_set_node(pduList, duList_get_node(pduList, index), pdata, data_flag);
}

void *duList_access(duList_st *pduList, int index)
{
	dnode_st *dnode;

	dnode = duList_get_node(pduList, index);
	
	rdlock_duList(pduList);
	pthread_mutex_lock(&(pduList->lock));
	
	pduList->cursor = dnode;
	if(pduList->cursor){
		return pduList->cursor->data;
	}else{
		return NULL;
	}
}

void *duList_next(duList_st *pduList)
{
	pduList->cursor = pduList->cursor->next;
	
	if(pduList->cursor){
		return pduList->cursor->data;
	}else{
		return NULL;
	}
}
void *duList_prev(duList_st *pduList)
{
	pduList->cursor = pduList->cursor->prev;
	
	if(pduList->cursor){
		return pduList->cursor->data;
	}else{
		return NULL;
	}
}

void duList_access_end(duList_st *pduList)
{

	pduList->cursor = NULL;
	pthread_mutex_unlock(&(pduList->lock)); 
	unlock_duList(pduList);
}

void duList_print(duList_st *pduList, void (*print_func)(void *))
{
	dnode_st *dnode;

	if((pduList == NULL)){
		return;
	}

	rdlock_duList(pduList);
	
	printf("duList print : count = %d, datatype = %s  \n", pduList->count, datatype_string[pduList->data_type]);
	
	dnode = pduList->head;
	while(dnode){
		if(print_func){
			print_func(dnode->data);
		}else{
			print_data(dnode->data, DATA_FLAG(dnode->header.data_attr, dnode->header.data_type, dnode->header.data_size));
		}
		dnode = dnode->next;
	}

	unlock_duList(pduList);

}


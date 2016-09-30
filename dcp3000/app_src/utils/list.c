#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "list.h"

void init_list(list_st *plist, int data_type)
{
	if(plist == NULL){
		return;
	}
	
	INIT_LIST(plist);
	plist->data_type = data_type;

	//printf("init_list : count = %d, max_size = %d, data_type = %#x \n", plist->count, plist->max_size, data_type);
}

list_st *create_list(int data_type)
{
	list_st *plist;

	plist = malloc(sizeof(list_st));
	if(plist == NULL){
		perror("list_st\n");
		exit(-1);
	}

	INIT_LIST(plist);
	plist->data_type = data_type;

	return plist;
}


void list_empty(list_st *plist)
{
	snode_st *snode, *next_snode;
	
	if(plist == NULL){
		return;
	}
	
	wrlock_list(plist);

	snode = plist->head;
	while(snode){
		next_snode = snode->next;
		free_snode(snode);
		snode = next_snode;
	}

	plist->head = NULL;
	plist->tail = NULL;
	plist->count = 0;
	
	unlock_list(plist);
}


void destroy_list(list_st *plist)
{
	snode_st *snode, *next_snode;
	
	if(plist == NULL){
		return;
	}
	
	list_empty(plist);

	pthread_rwlock_destroy(&plist->rwlock);
	pthread_mutex_destroy(&plist->lock);

	free(plist);
}

int list_add_node(list_st *plist, snode_st *snode)
{
	int count;
	
	if((plist == NULL) || (snode == NULL)){
		return -1;
	}

	//printf("list_add_node : ....... \n");
	
	wrlock_list(plist);
	//printf(("list_add_node : ---------1--------- \n"));
	if(plist->count >= plist->max_size){
		printf("list_add_node : count[%d] >= max_size[%d] , return !\n", plist->count, plist->max_size);
		unlock_list(plist);
		return -1;
	}
	
	if(plist->count == 0){
		plist->head = snode;
	}else{
		plist->tail->next = snode;
	}

	plist->count++;
	plist->tail = snode;

	snode->header.parent = plist;

	count = plist->count;
	
	unlock_list(plist);

	//printf("list_add_node : node count = %d \n", count);

	return count;
}

int list_insert_node(list_st *plist, snode_st *snode)
{
	int count;
	
	if((plist == NULL) || (snode == NULL)){
		return -1;
	}

	wrlock_list(plist);
	
	if(plist->count >= plist->max_size){
		unlock_list(plist);
		return -1;
	}
	
	if(plist->count == 0){
		plist->tail = snode;
	}else{
		snode->next = plist->head;
	}

	plist->count++;
	plist->head = snode;

	snode->header.parent = plist;

	count = plist->count;
	
	unlock_list(plist);

	return count;
}

snode_st *list_separate_node(list_st *plist, snode_st *snode)
{
	snode_st *cur;
	
	if((plist == NULL) || (snode == NULL) || (plist->count <= 0)){
		return NULL;
	}

	if(snode->header.parent != plist){
		return NULL;
	}

	wrlock_list(plist);

	if(plist->head == snode){
		plist->head = snode->next;
	}else{
		cur = plist->head;
		while(cur->next){
			if(cur->next == snode){
				break;
			}
			cur = cur->next;
		}
		cur->next = snode->next;
	}
	
	if(snode == plist->tail){
		plist->tail = cur;
	}
	
	plist->count--;

	unlock_list(plist);

	snode->header.parent = NULL;
	snode->next = NULL;

	return snode;
}

int list_del_node(list_st *plist, snode_st *snode)
{

	if(list_separate_node(plist, snode)){
		free_snode(snode);	
		return plist->count;
	}else{
		return -1;
	}

}

snode_st *list_separate_head(list_st *plist)
{
	snode_st *snode = NULL;
	
	if((plist == NULL)){
		return NULL;
	}

	//printf("list_separate_head : ..... \n");

	wrlock_list(plist);
	
	if(plist->count > 0){

		snode = plist->head;
		
		if(snode == plist->tail){
			plist->tail = NULL;
		}
		
		plist->head = snode->next;
		plist->count--;

		snode->header.parent = NULL;
		snode->next = NULL;
	}

	unlock_list(plist);

	//printf("list_separate_head : node count = %d \n", plist->count);
		
	return snode;
}

snode_st *list_separate_tail(list_st *plist)
{
	if((plist == NULL)){
		return NULL;
	}

	return list_separate_node(plist, plist->tail);
		
}

int snode_insert(snode_st *snode, snode_st *cursnode)
{

	snode_st *cur;
	list_st *plist;

	if((cursnode == NULL) || (snode == NULL)){
		return -1;
	}

	if(snode->header.parent != cursnode->header.parent){
		return -1;
	}

	plist = (list_st *)(snode->header.parent);

	wrlock_list(plist);
	
	if(plist->count >= plist->max_size){
		unlock_list(plist);
		return -1;
	}

	if(plist->head == snode){
		plist->head = cursnode;
	}else{
		cur = plist->head;
		while(cur->next){
			if(cur->next == snode){
				break;
			}
			cur = cur->next;
		}
		cur->next = cursnode;
	}

	cursnode->next = snode;

	plist->count++;

	unlock_list(plist);

	return 0;
}

int snode_append(snode_st *snode, snode_st *cursnode)
{

	list_st *plist;

	if((cursnode == NULL) || (snode == NULL)){
		return -1;
	}

	if(snode->header.parent != cursnode->header.parent){
		return -1;
	}

	plist = (list_st *)(snode->header.parent);

	wrlock_list(plist);
	
	if(plist->count >= plist->max_size){
		unlock_list(plist);
		return -1;
	}

	cursnode->next = snode->next;

	if(plist->tail == snode){
		plist->tail = cursnode;
	}

	snode->next = cursnode;

	plist->count++;

	unlock_list(plist);

	return 0;
}

void * list_head(list_st *plist)
{
	if((plist == NULL) || (plist->head == NULL)){
		return NULL;
	}

	return plist->head->data;
}

void * list_tail(list_st *plist)
{
	if((plist == NULL) || (plist->tail == NULL)){
		return NULL;
	}
	
	return plist->tail->data;
}

snode_st *list_get_node(list_st *plist, int index)
{
	int count = 0;
	snode_st *snode;

	if((plist == NULL) || (index < 0)){
		return NULL;
	}

	rdlock_list(plist);

	if(index < plist->count){
		snode = plist->head;
		while(snode){
			if(count == index){
				break;
			}
			count++;
			snode = snode->next;
		}
	}
	
	unlock_list(plist);

	
	return snode;
	
}

snode_st *list_set_node(list_st *plist, 	snode_st *snode, void *pdata, int data_flag)
{
	int count = 0;
	
	if((plist == NULL) || (snode == NULL)){
		return NULL;
	}

	if(VOID_DATA != plist->data_type){
		data_flag |= DATA_TYPE_FLAG(plist->data_type);
	}
	snode_reset_data(snode, pdata, data_flag);
	
	return snode;
}


snode_st *list_create_node(list_st *plist, void *pdata, int data_flag)
{
	snode_st *snode;
	
	if(plist == NULL){
		return NULL;
	}

	if(VOID_DATA != plist->data_type){
		data_flag |= DATA_TYPE_FLAG(plist->data_type);
	}
	snode = create_snode(pdata, data_flag);

	snode->header.parent = plist;

	//printf("list_create_node : %p data point %p data_flag = %#x \n", snode, pdata, data_flag);
	
	return snode;
}

int list_add(list_st *plist, void *pdata, int data_flag)
{
	return list_add_node(plist, list_create_node(plist, pdata, data_flag));
}

int list_add_array(list_st *plist, void *pdata, int data_size, int count)
{
	int i;

	for(i=0; i<count; i++){
		list_add_node(plist, list_create_node(plist, pdata+data_size*i, data_size));
	}

	return list_size(plist);
}

int list_addto(list_st *plist, int index, void *pdata, int data_flag)
{
	int count = -1;
	snode_st *snode, *cursnode;
	
	if((plist == NULL) || (index < 0)){
		return -1;
	}

	snode =list_create_node(plist, pdata, data_flag);
	if(index==0){
		count = list_insert_node(plist, snode);
	}else if(index>0){
		cursnode = list_get_node(plist, index-1);
		count = snode_append(snode,  cursnode);
	}

	return count;
}

int list_insert(list_st *plist, void *pdata, int data_flag)
{
	return list_insert_node(plist, list_create_node(plist, pdata, data_flag));
}

void list_traverse(list_st *plist, int (*handler)(void *, void *) , void *arg)
{
	int res;
	snode_st *snode;
	
	if((plist == NULL) || (handler == NULL)){
		return ;
	}

	
	wrlock_list(plist);

	snode = plist->head;
	while(snode){
		if(handler(snode->data, arg) != 0)
			break;
	
		snode = snode->next;
	}
	
	unlock_list(plist);

}


snode_st *list_find_node(list_st *plist, void *pdata, compare_func compare)
{
	int res;
	snode_st *snode;
	
	if((plist == NULL) || (pdata == NULL)){
		return NULL;
	}

	
	rdlock_list(plist);

	snode = plist->head;
	while(snode){
		if(compare){
			if(compare(snode->data, pdata) == 0)
				break;
		}else{
			res = node_equals(snode, pdata);
			if(res == 1){
				break;
			}else if(res == -1){
				snode = NULL;
				break;
			}
		}
		snode = snode->next;
	}
	
	unlock_list(plist);

	return snode;
}


void *list_find(list_st *plist, void *pdata, compare_func compare)
{
	snode_st *snode;

	snode = list_find_node(plist, pdata, compare);

	if(snode){
		return snode->data;
	}else{
		return NULL;
	}
}


int list_indexof(list_st *plist, void *pdata, compare_func compare)
{
	int index = -1, res;
	snode_st *snode;
	
	if((plist == NULL) || (pdata == NULL)){
		return -1;
	}

	rdlock_list(plist);

	snode = plist->head;
	while(snode){
		index++;
		if(compare){
			if(compare(snode->data, pdata) == 0)
				break;
		}else{
			res = node_equals(snode, pdata);
			if(res == 1){
				break;
			}else if(res == -1){
				index = -1;
				break;
			}
		}
		snode = snode->next;
	}
	
	unlock_list(plist);

	return index;
}


int list_del(list_st *plist, void *pdata, compare_func compare)
{

	#if 1
	int count, res;
	snode_st *snode;
	snode_st *pre_node = NULL;
	
	if((plist == NULL) || (pdata == NULL)){
		return -1;
	}

	
	wrlock_list(plist);

	snode = plist->head;
	while(snode){
		if(compare){
			if(compare(snode->data, pdata) == 0)
				break;
		}else{
			res = node_equals(snode, pdata);
			if(res == 1){
				break;
			}else if(res == -1){
				snode = NULL;
				break;
			}
		}
		pre_node = snode;
		snode = snode->next;
	}

	if(snode){
		
		if(pre_node){
			pre_node->next = snode->next;
		}else{
			plist->head = snode->next;
		}
		
		if(snode == plist->tail){
			plist->tail = NULL;
		}
		
		plist->count--;
		free_snode(snode);	
	
		count = plist->count;
	}else{
		count = -1;
	}

	unlock_list(plist);

	return count;
	#else
	return list_del_node(plist, list_find(plist, data));
	#endif
}

int list_remove(list_st *plist, int index)
{
	#if 1
	int count = 0;
	snode_st *snode;
	snode_st *pre_node = NULL;

	if((plist == NULL) || (index < 0)){
		return -1;
	}

	wrlock_list(plist);

	if(index < plist->count){
		snode = plist->head;
		while(snode){
			if(count == index){
				break;
			}
			count++;
			pre_node = snode;
			snode = snode->next;
		}
	}

	if(snode){
		
		if(pre_node){
			pre_node->next = snode->next;
		}else{
			plist->head = snode->next;
		}
		
		if(snode == plist->tail){
			plist->tail = NULL;
		}
		
		plist->count--;
		free_snode(snode);	
	}
	
	count = plist->count;
	unlock_list(plist);
	
	return count;
	#else
	return list_del_node(plist, list_get_node(plist, index));
	#endif
}


void *list_get(list_st *plist, int index)
{

	snode_st *snode;

	snode = list_get_node(plist, index);

	if(snode){
		return snode->data;
	}else{
		return NULL;
	}
}


snode_st *list_set(list_st *plist, int index, void *pdata, int data_flag)
{
	return list_set_node(plist, list_get_node(plist, index), pdata, data_flag);
}


void list_access_node(list_st *plist, snode_st *snode)
{

	rdlock_list(plist);
	pthread_mutex_lock(&(plist->lock)); 
	
	plist->cursor = snode;

}

snode_st *list_next_node(list_st *plist)
{

	plist->cursor = plist->cursor->next;

	return plist->cursor;

}


void *list_access(list_st *plist, int index)
{
	
	snode_st *snode = list_get_node(plist, index);

	if(snode == NULL){
		return NULL;
	}
	
	rdlock_list(plist);
	pthread_mutex_lock(&(plist->lock)); 
	
	plist->cursor = snode;
	if(plist->cursor){
		return plist->cursor->data;
	}else{
		return NULL;
	}
}

void *list_next(list_st *plist)
{

	plist->cursor = plist->cursor->next;
	
	if(plist->cursor){
		return plist->cursor->data;
	}else{
		return NULL;
	}
}

void list_access_end(list_st *plist)
{
	if(plist == NULL){
		return;
	}
	
	plist->cursor = NULL;
	
	pthread_mutex_unlock(&(plist->lock)); 
	unlock_list(plist);
}

void list_print(list_st *plist, void (*print_func)(void *))
{
	snode_st *snode;

	if((plist == NULL)){
		return;
	}

	rdlock_list(plist);
	
	printf("list print : count = %d, datatype = %s  \n", plist->count, datatype_string[plist->data_type]);
	
	snode = plist->head;
	while(snode){
		if(print_func){
			print_func(snode->data);
		}else{
			print_data(snode->data, DATA_FLAG(snode->header.data_attr, snode->header.data_type, snode->header.data_size));
		}
		snode = snode->next;
	}

	unlock_list(plist);

}

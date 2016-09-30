
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cirArrList.h"
#include "utils.h"


cirArrList_st *create_cirArrList(int list_size, int data_flag)
{
	int data_size, data_attr;
	cirArrList_st *plist;

	data_attr = DATA_ATTR(data_flag);
	data_size = DATA_SIZE(data_flag);

	if(data_attr == 0){
		data_attr = STATIC_DATA;
	}

	if(data_attr == STATIC_DATA){
		plist = malloc(sizeof(cirArrList_st) + list_size * data_size);
	}else{
		plist = malloc(sizeof(cirArrList_st) + list_size * sizeof(void *));
	}

	if(plist == NULL){
		perror("cirArrList_st\n");
		exit(-1);
	}

	plist->data_type = DATA_TYPE(data_flag);
	plist->data_attr = data_attr;
	plist->data_size = data_size;
	plist->max_size = list_size;
	plist->top = list_size;
	plist->s_idx = 0;
	plist->count = 0;
	
	pthread_rwlock_init(&((plist)->rwlock), NULL);
	
	return plist;
}



void destroy_cirArrList(cirArrList_st *plist)
{
	
	if(plist == NULL){
		return;
	}

	pthread_rwlock_destroy(&plist->rwlock);
	
	free(plist);
}

void cirArrList_empty(cirArrList_st *plist)
{

	if(plist == NULL){
		return;
	}
	
	wrlock_list(plist);

	plist->s_idx = 0;
	plist->count = 0;
	
	unlock_list(plist);

}

int cirArrList_set_top(cirArrList_st *plist, int top)
{

	if((plist == NULL) || (top <= 0)){
		return -1;
	}
	
	wrlock_list(plist);

	if(top>plist->max_size){
		top = plist->max_size;
	}
	
	plist->top = top;
	
	unlock_list(plist);

	printf("cirArrList_set_top %d \n", top);

	return top;
}


int resize_cirArrList(cirArrList_st *plist, int size)
{
	int i, data_size;
	int extra;
	
	if(plist == NULL){
		return -1;
	}
	
	wrlock_list(plist);
	
	if(plist->data_attr == STATIC_DATA){
		data_size = plist->data_size;
	}else{
		data_size = sizeof(void *);
	}
		
	plist = realloc(plist->databuf, size * data_size);
	if(plist == NULL){
		perror("cirArrList\n");
		exit(-1);
	}
	
	if(size > plist->max_size){

		extra = plist->s_idx + 1 + plist->count - plist->max_size;

		for(i=0; i<extra; i++){
			plist->s_idx++;
			plist->s_idx %= size;
			memcpy(plist->databuf + plist->s_idx * data_size, plist->databuf + i * data_size, data_size);
		}
	
	}else if(size<plist->top){
		plist->top = size;
		//数组截短后会丢失数据，请谨慎使用
	}
	
	plist->max_size = size;
	
	unlock_list(plist);
	
	return size;
}

void cirArrList_traverse(cirArrList_st *plist, int (*handler)(void *, void *) , void *arg)
{
	int i, index;
	void *pdata = NULL;

	if((plist == NULL) || (handler == NULL)){
		return ;
	}

	wrlock_list(plist);
	
	for(i=0; i<plist->count; i++){
		
		index = (i + plist->s_idx) % plist->top;

		if(plist->data_attr == STATIC_DATA){
			pdata = plist->databuf + index*plist->data_size;
		}else{
			memcpy(&pdata, plist->databuf + index * sizeof(void *), sizeof(void *));
		}

		if(handler(pdata, arg) != 0){
			break;
		}
	}
	
	unlock_list(plist);
	
}

void *cirArrList_find(cirArrList_st *plist, void *arg, compare_func compare)
{
	int i,index;
	void *pdata = NULL;

	if((plist == NULL) || (compare == NULL)){
		return NULL;
	}

	wrlock_list(plist);

	for(i=0; i<plist->count; i++){

		index = (i + plist->s_idx) % plist->top;
			
		if(plist->data_attr == STATIC_DATA){
			pdata = plist->databuf + index*plist->data_size;
		}else{
			memcpy(&pdata, plist->databuf + index * sizeof(void *), sizeof(void *));
		}
			
		pdata = plist->databuf + index*plist->data_size;
		if(compare(pdata, arg) == 0){
			break;
		}
	}

	unlock_list(plist);
	
	return pdata;
}

int cirArrList_indexof(cirArrList_st *plist, void *arg, compare_func compare)
{
	int i, index = -1;
	void *pdata = NULL;

	if((plist == NULL) || (compare == NULL)){
		return -1;
	}
	
	wrlock_list(plist);
	
	for(i=0; i<plist->count; i++){

		index = (i + plist->s_idx) % plist->top;

		if(plist->data_attr == STATIC_DATA){
			pdata = plist->databuf + index*plist->data_size;
		}else{
			memcpy(&pdata, plist->databuf + index * sizeof(void *), sizeof(void *));
		}
		
		if(compare(pdata, arg) == 0){
			index = i;
			goto Finded_It;
		}
		
	}
	index = -1;
	Finded_It:

	unlock_list(plist);

	return index;
}


void *cirArrList_get(cirArrList_st *plist, int index)
{
	void *pdata = NULL;
	
	if(plist == NULL){
		return NULL;
	}
	
	wrlock_list(plist);

	printf("cirArrList_get : index = %d \n", index);

	if(index < plist->count){	
		index = (index + plist->s_idx) % plist->top;
		printf("cirArrList_get : index = %d , s_idx = %d \n", index, plist->s_idx);
		if(plist->data_attr == STATIC_DATA){
			pdata = plist->databuf + index * plist->data_size;
		}else{
			memcpy(&pdata, plist->databuf + index * sizeof(void *), sizeof(void *));
		}
	}

	unlock_list(plist);

	return pdata;
}


void cirArrList_set(cirArrList_st *plist, int index, void *pdata)
{
	if((plist == NULL) || (pdata == NULL)){
		return;
	}

	wrlock_list(plist);
	
	if(index < plist->count){
		index = (index + plist->s_idx) % plist->top;
		if(plist->data_attr == STATIC_DATA){
			memcpy(plist->databuf + index * plist->data_size, pdata, plist->data_size);
		}else{
			memcpy(plist->databuf + index * sizeof(void *), &pdata, sizeof(void *));
		}
	}

	unlock_list(plist);
}

int cirArrList_remove_head(cirArrList_st *plist)
{
	int count;
	
	if(plist == NULL){
		return -1;
	}

	wrlock_list(plist);
	
	if(plist->count>0){
		plist->s_idx++;
		plist->count--;
		plist->s_idx %= plist->top;
	}

	count = plist->count;
	
	unlock_list(plist);

	return count;
}

int cirArrList_remove_tail(cirArrList_st *plist)
{
	int count;
	
	if(plist == NULL){
		return -1;
	}

	wrlock_list(plist);
	
	if(plist->count>0){
		plist->count--;
	}

	count = plist->count;
	
	unlock_list(plist);

	return count;
}


int cirArrList_insert(cirArrList_st *plist, void *pdata)
{
	int index, count;
	
	if((plist == NULL) || (pdata == NULL)){
		return -1;
	}

	wrlock_list(plist);

	plist->s_idx = (--plist->s_idx + plist->top) % plist->top;
	index = plist->s_idx;
	
	if(plist->data_attr == STATIC_DATA){
		memcpy(plist->databuf + index * plist->data_size, pdata, plist->data_size);
	}else{
		memcpy(plist->databuf + index * sizeof(void *), &pdata, sizeof(void *));
	}

	plist->count++;
	if(plist->count >  plist->top){
		plist->count =  plist->top;
	}
	count = plist->count;
	printf("cirArrList_insert : s_idx = %d, count = %d \n", plist->s_idx, plist->count);
	unlock_list(plist);

	return count;
}

int cirArrList_add(cirArrList_st *plist, void *pdata)
{
	int index, count;
	
	if((plist == NULL) || (pdata == NULL)){
		return -1;
	}

	wrlock_list(plist);

	index = (plist->s_idx + plist->count++) % plist->top;

	printf("cirArrList_add : s_idx = %d, index = %d, count = %d \n", index, plist->s_idx, plist->count);
	
	if(plist->data_attr == STATIC_DATA){
		memcpy(plist->databuf + index * plist->data_size, pdata, plist->data_size);
	}else{
		memcpy(plist->databuf + index * sizeof(void *), &pdata, sizeof(void *));
	}
	
	if(plist->count >  plist->top){
		plist->count =  plist->top;
		plist->s_idx++;
	}
	plist->s_idx %= plist->top;

	count = plist->count;

	printf("cirArrList_add : s_idx = %d, count = %d \n", plist->s_idx, plist->count);

	unlock_list(plist);

	return count;
}

int cirArrList_add_array(cirArrList_st *plist, void *pdata, int count)
{
	int i, index;
	int data_size;
	
	if((plist == NULL) || (pdata == NULL)){
		return -1;
	}

	wrlock_list(plist);

	for(i=0; i<count; i++){
		index = (plist->s_idx++ + plist->count++) % plist->top;
		if(plist->data_attr == STATIC_DATA){
			memcpy(plist->databuf + index * plist->data_size, pdata + i*data_size, plist->data_size);
		}else{
			pdata += i*plist->data_size;
			memcpy(plist->databuf + index * sizeof(void *), &pdata, sizeof(void *));
		}
		
		plist->s_idx %= plist->top;
	}
	
	if(plist->count >  plist->top){
		plist->count =  plist->top;
	}

	count = plist->count;
	
	unlock_list(plist);

	return count;
}


void cirArrList_print(cirArrList_st *plist, void (*print_func)(void *))
{
	int i, index;
	void *pdata;

	if((plist == NULL) || (print_func == NULL)){
		return;
	}

	printf("cirArrList print : s_idx = %d, count = %d, datasize = %d , \n", plist->s_idx, plist->count, plist->data_size);

	wrlock_list(plist);
	
	for(i=0; i<plist->count; i++){
		
		index = (plist->s_idx + i) % plist->top;
		
		if(plist->data_attr == STATIC_DATA){
			pdata = plist->databuf + index*plist->data_size;
		}else{
			memcpy(&pdata, plist->databuf + index * sizeof(void *), sizeof(void *));
		}
		

		print_func(pdata);

	}

	unlock_list(plist);
}



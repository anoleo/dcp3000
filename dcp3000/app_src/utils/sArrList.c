#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "sArrList.h"
#include "utils.h"


sArrList_st *create_sArrList(int list_size, int data_size)
{
	sArrList_st *plist;

	if((list_size <= 0) || (data_size <= 0)){
		return NULL;
	}

	plist = malloc(sizeof(sArrList_st) + list_size * data_size);
	if(plist == NULL){
		perror("sArrList_st\n");
		exit(-1);
	}
	
	plist->data_size = data_size;
	plist->count = list_size;

	return plist;
}



void destroy_sArrList(sArrList_st *plist)
{
	
	if(plist == NULL){
		return;
	}

	free(plist);
}

void sArrList_traverse(sArrList_st *plist, int (*handler)(void *, void *) , void *arg)
{
	int index;
	void *pdata = NULL;

	if((plist == NULL) || (handler == NULL)){
		return ;
	}

	for(index=0; index<plist->count; index++){
		pdata = plist->databuf + index*plist->data_size;
		if(handler(pdata, arg) != 0){
			break;
		}
	}
	
}

void *sArrList_find(sArrList_st *plist, void *arg, compare_func compare)
{
	int index = -1;
	void *pdata = NULL;

	if((plist == NULL) || (compare == NULL)){
		return NULL;
	}

	for(index=0; index<plist->count; index++){
		pdata = plist->databuf + index*plist->data_size;
		if(compare(pdata, arg) == 0){
			break;
		}
	}
	
	return pdata;
}



int sArrList_indexof(sArrList_st *plist, void *arg, compare_func compare)
{
	int i, index = -1;
	void *pdata = NULL;

	if((plist == NULL) || (compare == NULL)){
		return -1;
	}
	

	for(i=0; i<plist->count; i++){

		pdata = plist->databuf + i*plist->data_size;

		if(compare(pdata, arg) == 0){
			index = i;
			break;
		}

	}


	return index;
}


void *sArrList_get(sArrList_st *plist, int index)
{
	void *pdata = NULL;
	
	if(plist == NULL){
		return NULL;
	}
	
	if(index < plist->count){
		pdata = plist->databuf + index * plist->data_size;
	}

	return pdata;
}


void sArrList_set(sArrList_st *plist, int index, void *pdata)
{
	if(plist == NULL){
		return;
	}

	if(index < plist->count){
		memcpy(plist->databuf + index * plist->data_size, pdata, plist->data_size);
	}
	
}


void sArrList_clear(sArrList_st *plist, int index)
{
	int i;
	void *pdata;
	
	if((plist == NULL) || (index >= plist->count)){
		return;
	}
	
	memset(plist->databuf + index*plist->data_size, 0, plist->data_size);

}

void sArrList_print(sArrList_st *plist, void (*print_func)(void *))
{
	int i;
	void *pdata;

	if((plist == NULL)){
		return;
	}

	
	printf("sArrList print : count = %d, datasize = %d \n", plist->count, plist->data_size);
	
	for(i=0; i<plist->count; i++){

		pdata = plist->databuf + i * plist->data_size;
		if(print_func){
			print_func(pdata);
		}else{
			print_bytes(pdata, plist->data_size);
		}

	}

}


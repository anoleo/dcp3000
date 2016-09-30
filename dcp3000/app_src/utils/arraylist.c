#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "arraylist.h"
#include "utils.h"

int init_arraylist(arraylist_st *plist, int list_size, int data_flag)
{
	int masksize;

	//printf("init_arraylist : list_size = %d, data_flag = %#x \n", list_size, data_flag);

	if(plist == NULL){
		return -1;
	}

	
	plist->data_attr = DATA_ATTR(data_flag);
	plist->data_type = DATA_TYPE(data_flag);
	plist->data_size = DATA_SIZE(data_flag);

	if(plist->data_attr == 0){
		plist->data_attr = STATIC_DATA;
	}

	switch(plist->data_type){
		case	CHAR_DATA :
		case	UCHAR_DATA :
			plist->data_size = sizeof(char);
			break;
		case	SHORT_DATA :
		case	USHORT_DATA :
			plist->data_size = sizeof(short);
			break;
		case	INT_DATA :
		case	UINT_DATA :
		case	POINTER_DATA :
			plist->data_size = sizeof(int);
			break;
		case	LLONG_DATA :
			plist->data_size = sizeof(long long);
			break;
		case	FLOAT_DATA :;
			plist->data_size = sizeof(float);
			break;
		case	DOUBLE_DATA :
			plist->data_size = sizeof(double);
			break;
		case	STRING_DATA :
		case	VOID_DATA :
		case	STRUCT_DATA :
		case	ARRAY_DATA :
		default :	
			if((plist->data_size<=0)){
				return -1;
			}
			break;
	}
	
	plist->max_size = list_size;

	if(list_size>0){
		
		masksize = plist->max_size/8;
		if(plist->max_size%8){
			masksize++;
		}

		plist->maskbit = calloc(masksize, sizeof(char));
		if(plist->maskbit == NULL){
			perror("arraylist maskbit\n");
			exit(-1);
		}

		plist->maskbit_size = masksize;

		if(plist->data_attr == STATIC_DATA){
			plist->datalist = calloc(plist->max_size, plist->data_size);
		}else{
			plist->datalist = calloc(plist->max_size, sizeof(void *));
		}
		
		if(plist->datalist == NULL){
			perror("arraylist datalist\n");
			exit(-1);
		}
		
	}else{
		plist->maskbit = NULL;
		plist->datalist = NULL;
	}
	
	plist->count = 0;
	plist->last_index = -1;
	pthread_rwlock_init(&(plist->rwlock), NULL);


	//printf("init_arraylist : %d index , data_flag = %#x \n", plist->data_attr, plist->max_size);

	return 0;
}

arraylist_st *create_arraylist(int list_size, int data_flag)
{
	arraylist_st *plist;

	plist = malloc(sizeof(arraylist_st));
	if(plist == NULL){
		perror("arraylist_st\n");
		exit(-1);
	}
	
	init_arraylist(plist, list_size, data_flag);

	return plist;
}


void arraylist_empty(arraylist_st *plist)
{
	int i;
	void *pdata;
	
	if(plist == NULL){
		return;
	}
	
	wrlock_arraylist(plist);

	if(plist->data_attr == STATIC_DATA){
		//memset(plist->datalist, 0, plist->last_index*plist->data_size);
	}else{
		for(i=0; i<plist->last_index; i++){
			if(check_maskbit(plist->maskbit, i)){
				memcpy(&pdata, plist->datalist + i * sizeof(void *), sizeof(void *));
				memset(plist->datalist + i * sizeof(void *), 0, sizeof(void *));
				if(plist->data_type != POINTER_DATA){
					free(pdata);
				}
			}
		}
	}
	memset(plist->maskbit, 0, plist->maskbit_size);

	plist->count = 0;
	plist->last_index = -1;

	unlock_arraylist(plist);
}


void arraylist_clean(arraylist_st *plist)
{
	int i;
	void *pdata;
	
	if(plist == NULL){
		return;
	}
	
	wrlock_arraylist(plist);

	if(plist->data_attr == STATIC_DATA){
		//memset(plist->datalist, 0, plist->last_index*plist->data_size);
	}else{
		for(i=0; i<plist->last_index; i++){
			if(check_maskbit(plist->maskbit, i)){
				memcpy(&pdata, plist->datalist + i * sizeof(void *), sizeof(void *));
				//memset(plist->datalist + i * sizeof(void *), 0, sizeof(void *));
				if(plist->data_type != POINTER_DATA){
					free(pdata);
				}
			}
		}
	}
	free(plist->maskbit);
	free(plist->datalist);
	
	plist->datalist = NULL;
	plist->maskbit = NULL;

	plist->count = 0;
	plist->last_index = -1;
	plist->max_size = 0;
	plist->maskbit_size = 0;
	plist->data_size = 0;

	unlock_arraylist(plist);
}

void destroy_arraylist(arraylist_st *plist)
{
	int  i;
	void *pdata;
	
	if(plist == NULL){
		return;
	}
	
	wrlock_arraylist(plist);
	if(plist->data_attr == STATIC_DATA){
		//memset(plist->datalist, 0, plist->last_index*plist->data_size);
	}else{
		for(i=0; i<plist->last_index; i++){
			if(check_maskbit(plist->maskbit, i)){
				memcpy(&pdata, plist->datalist + i * sizeof(void *), sizeof(void *));
				//memset(plist->datalist + i * sizeof(void *), 0, sizeof(void *));
				if(plist->data_type != POINTER_DATA){
					free(pdata);
				}
			}
		}
	}
	free(plist->maskbit);
	free(plist->datalist);
	unlock_arraylist(plist);
	
	pthread_rwlock_destroy(&plist->rwlock);

	free(plist);
}



int resize_arraylist(arraylist_st *plist, int size)
{
	int i;
	
	if(plist == NULL){
		return -1;
	}
	
	wrlock_arraylist(plist);

	if(size <= plist->last_index){
		size = plist->last_index + 1;
	}

	plist->maskbit_size = size/8;
	if(size%8){
		plist->maskbit_size++;
	}

	plist->maskbit = realloc(plist->maskbit, plist->maskbit_size);
	if(plist->maskbit == NULL){
		perror("arraylist maskbit\n");
		exit(-1);
	}

	if(plist->data_attr == STATIC_DATA){
		plist->datalist = realloc(plist->datalist, size * plist->data_size);
	}else{
		plist->datalist = realloc(plist->datalist, size * sizeof(void *));
	}
	if(plist->datalist == NULL){
		perror("arraylist datalist\n");
		exit(-1);
	}

	for(i=plist->max_size-1; i<size; i++){
		clean_maskbit(plist->maskbit, i);
		if(plist->data_attr == STATIC_DATA){
			//memset(plist->datalist + i * plist->data_size, 0, plist->data_size);
		}else{
			//memset(plist->datalist + i * sizeof(void *), 0, sizeof(void *));
		}
			
	}

	plist->max_size = size;
	
	unlock_arraylist(plist);

	return size;
}

void arraylist_traverse(arraylist_st *plist, int (*handler)(void *, void *) , void *arg)
{
	int index;
	void *pdata = NULL;

	if((plist == NULL) || (handler == NULL)){
		return ;
	}
	
	wrlock_arraylist(plist);

	for(index=0; index<=plist->last_index; index++){
		if(check_maskbit(plist->maskbit, index)){

			if(plist->data_attr == STATIC_DATA){
				pdata = plist->datalist + index*plist->data_size;
			}else{
				memcpy(&pdata, plist->datalist + index * sizeof(void *), sizeof(void *));
			}

			if(handler(pdata, arg) != 0){
				break;
			}

		}
	}

	unlock_arraylist(plist);

}

void *arraylist_find(arraylist_st *plist, void *arg, compare_func compare)
{
	int index = -1, res;
	void *pdata = NULL;

	if(plist == NULL){
		return NULL;
	}
	
	rdlock_arraylist(plist);

	for(index=0; index<=plist->last_index; index++){
		if(check_maskbit(plist->maskbit, index)){

			if(plist->data_attr == STATIC_DATA){
				pdata = plist->datalist + index*plist->data_size;
			}else{
				memcpy(&pdata, plist->datalist + index * sizeof(void *), sizeof(void *));
			}
			
			if(compare){
				if(compare(pdata, arg) == 0){
					break;
				}
			}else{
				res = pdata_equals(DATA_FLAG(plist->data_attr, plist->data_type, plist->data_size), pdata, arg);
				if(res == 1){
					break;
				}else if(res == -1){
					pdata = NULL;
					break;
				}
			}
		}
	}

	unlock_arraylist(plist);
	
	return pdata;
}


int array_set_data(void *addr, void *pdata, int data_flag)
{
	char data_type, data_attr;
	int data_size;
	
	if(addr == NULL){
		return -1;
	}

	data_size = DATA_SIZE(data_flag);
	data_type = DATA_TYPE(data_flag);
	data_attr = DATA_ATTR(data_flag);

	switch(data_type){
		case	CHAR_DATA :
		case	UCHAR_DATA :
			if(data_attr == IMMEDIATE_DATA)
				data_size = sizeof(char);
			break;
		case	SHORT_DATA :
		case	USHORT_DATA :
			if(data_attr == IMMEDIATE_DATA)
				data_size = sizeof(short);
			break;
		case	INT_DATA :
		case	UINT_DATA :
			if(data_attr == IMMEDIATE_DATA)
				data_size = sizeof(int);
			break;
		case	LLONG_DATA :
		case	ULLONG_DATA :
			data_size = sizeof(long long);
			break;
		case	FLOAT_DATA :;
			data_size = sizeof(float);
			break;
		case	DOUBLE_DATA :
			data_size = sizeof(double);
			break;
		case	STRING_DATA :
			data_size = strlen(pdata)+1;
			break;
		case	VOID_DATA :
		case	STRUCT_DATA :
		case	ARRAY_DATA :
		default :	
			if(data_attr == IMMEDIATE_DATA)
				data_size = sizeof(void *);
			break;
	}

	if(IMMEDIATE_DATA == data_attr){
		memcpy(addr, &pdata, data_size);
	}else{
		if((data_size <= 0) || (pdata == NULL)){return -1;}
		memcpy(addr, pdata, data_size);
	}

	return 0;
}


int arraylist_add(arraylist_st *plist, void *pdata)
{
	int res;

	if(plist == NULL){
		return -1;
	}
	
	wrlock_arraylist(plist);

	if(plist->last_index < plist->max_size){
		plist->last_index++;
		set_maskbit(plist->maskbit, plist->last_index);
		if(plist->data_attr == STATIC_DATA){
			memcpy(plist->datalist + plist->last_index * plist->data_size, pdata, plist->data_size);
		}else{
			memcpy(plist->datalist + plist->last_index * sizeof(void *), &pdata, sizeof(void *));
		}
		plist->count++;
		res = plist->last_index;
	}else{
		res = -1;
	}

	unlock_arraylist(plist);

	return res;
}


int arraylist_remove(arraylist_st *plist, int index)
{
	int res;
	void *pdata;
	
	if(plist == NULL){
		return -1;
	}
	
	wrlock_arraylist(plist);

	if(index <= plist->last_index){
		if(check_maskbit(plist->maskbit, index)){
			clean_maskbit(plist->maskbit, index);
			plist->count--;
			if(index == plist->last_index){
				plist->last_index--;
			}
			if(plist->data_attr != STATIC_DATA){
				memcpy(&pdata, plist->datalist + index * sizeof(void *), sizeof(void *));
				memset(plist->datalist + index * sizeof(void *), 0, sizeof(void *));
				if(plist->data_type != POINTER_DATA){
					free(pdata);
				}
			}
		}else{
			res = -1;
		}
	}else{
		res = -1;
	}

	unlock_arraylist(plist);

	return res;
}


int arraylist_indexof(arraylist_st *plist, void *arg, compare_func compare)
{
	int i, index = -1, res;
	void *pdata = NULL;

	if(plist == NULL){
		return -1;
	}
	
	rdlock_arraylist(plist);

	for(i=0; i<=plist->last_index; i++){
		if(check_maskbit(plist->maskbit, i)){
			
			if(plist->data_attr == STATIC_DATA){
				pdata = plist->datalist + i*plist->data_size;
			}else{
				memcpy(&pdata, plist->datalist + i * sizeof(void *), sizeof(void *));
			}

			if(compare){
				if(compare(pdata, arg) == 0){
					index = i;
					break;
				}
			}else{
				res = pdata_equals(DATA_FLAG(0, plist->data_type, plist->data_size), pdata, arg);
				if(res == 1){
					index = i;
					break;
				}else if(res == -1){
					index = -1;
					break;
				}
			}
		}
	}

	unlock_arraylist(plist);

	return index;
}


int arraylist_del(arraylist_st *plist, void *arg, compare_func compare)
{
	return arraylist_remove(plist, arraylist_indexof(plist, arg, compare));
}

void *arraylist_get(arraylist_st *plist, int index)
{
	void *pdata = NULL;
	
	if(plist == NULL){
		return NULL;
	}
	
	rdlock_arraylist(plist);

	if(index <= plist->last_index){
		if(check_maskbit(plist->maskbit, index)){
			if(plist->data_attr == STATIC_DATA){
				pdata = plist->datalist + index * plist->data_size;
			}else{
				memcpy(&pdata, plist->datalist + index * sizeof(void *), sizeof(void *));
			}
		}
	}

	unlock_arraylist(plist);

	
	//print_bytes(plist->maskbit, plist->maskbit_size);

	return pdata;
}


int arraylist_set(arraylist_st *plist, int index, void *pdata)
{
	int res;
	
	if(plist == NULL){
		return -1;
	}

	//printf("arraylist_set : %d index , max size = %d \n", index, plist->max_size);
	
	wrlock_arraylist(plist);

	if(index < plist->max_size){
		if(plist->data_attr == STATIC_DATA){
			memcpy(plist->datalist + index * plist->data_size, pdata, plist->data_size);
		}else{
			memcpy(plist->datalist + index * sizeof(void *), &pdata, sizeof(void *));
		}
		
		if(!check_maskbit(plist->maskbit, index)){
			set_maskbit(plist->maskbit, index);
			plist->count++;
			if(index > plist->last_index){
				plist->last_index = index;
			}
		}
		res = 0;
	}else{
		res = -1;
	}

	//print_bytes(plist->maskbit, plist->maskbit_size);
	
	unlock_arraylist(plist);

	return res;
}

int arraylist_modify(arraylist_st *plist, int index, dowith_func handler, void *arg)
{
	int res = -1;
	void *pdata;
	
	if(plist == NULL){
		return -1;
	}
	
	wrlock_arraylist(plist);

	if(index <= plist->last_index){
		if(check_maskbit(plist->maskbit, index)){
			if(plist->data_attr == STATIC_DATA){
				pdata = plist->datalist + index * plist->data_size;
			}else{
				memcpy(&pdata, plist->datalist + index * sizeof(void *), sizeof(void *));
			}
			res = handler(pdata, arg);
		}
	}
	
	unlock_arraylist(plist);

	return res;
}

void arraylist_print(arraylist_st *plist, void (*print_func)(void *))
{
	int i;
	void *pdata;

	if((plist == NULL)){
		return;
	}

	rdlock_arraylist(plist);
	
	printf("arraylist print : count = %d, max_count = %d, datatype = %s , datasize = %d \n", plist->count, plist->max_size, datatype_string[plist->data_type], plist->data_size);
	
	for(i=0; i<=plist->last_index; i++){
		
		if(check_maskbit(plist->maskbit, i)){
			if(plist->data_attr == STATIC_DATA){
				pdata = plist->datalist + i*plist->data_size;
			}else{
				memcpy(&pdata, plist->datalist + i * sizeof(void *), sizeof(void *));
			}
			
			if(print_func){
				print_func(pdata);
			}else{
				print_data(pdata, DATA_FLAG(plist->data_attr, plist->data_type, plist->data_size));
			}
		}
	}

	unlock_arraylist(plist);

}


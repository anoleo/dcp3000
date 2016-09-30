/*
 *  simple array List
 */

 #ifndef __SIMPLE_ARRAY_LIST_H__
#define __SIMPLE_ARRAY_LIST_H__

#include <pthread.h>

#include "dataflag.h"

struct simple_array_list{
	int count;
	int data_size;
	char databuf[];
};

typedef struct simple_array_list sArrList_st;


#define sArrList_size(plist)		(plist)->count

sArrList_st *create_sArrList(int list_size, int data_size);
void destroy_sArrList(sArrList_st *plist);

void sArrList_clear(sArrList_st *plist, int index);

void sArrList_traverse(sArrList_st *plist, int (*handler)(void *, void *) , void *arg);
void *sArrList_find(sArrList_st *plist, void *arg, compare_func compare);
int sArrList_indexof(sArrList_st *plist, void *arg, compare_func compare);


#define sArrList_contains(plist, arg, compare)		(sArrList_indexof(plist, arg, compare) >= 0)

void *sArrList_get(sArrList_st *plist, int index);
void sArrList_set(sArrList_st *plist, int index, void *pdata);

void sArrList_print(sArrList_st *plist, void (*print_func)(void *));


#endif


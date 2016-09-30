

 #ifndef __CIRCULAR_ARRAY_LIST_H__
#define __CIRCULAR_ARRAY_LIST_H__

#include <pthread.h>

#include "dataflag.h"

struct circular_array_list{
	pthread_rwlock_t rwlock;
	int s_idx;
	int count;
	int top;
	int max_size;
	int data_size;
	char data_attr;
	char data_type;
	char databuf[];
};

typedef struct circular_array_list cirArrList_st;

#define rdlock_list(plist)		pthread_rwlock_rdlock(&((plist)->rwlock))
#define wrlock_list(plist)		pthread_rwlock_wrlock(&((plist)->rwlock))
#define unlock_list(plist)		pthread_rwlock_unlock(&((plist)->rwlock))

#define cirArrList_size(plist)		(plist)->count

cirArrList_st *create_cirArrList(int list_size, int data_flag);
void destroy_cirArrList(cirArrList_st *plist);

void cirArrList_empty(cirArrList_st *plist);

int cirArrList_set_top(cirArrList_st *plist, int top);
#define cirList_get_top(plist)	(plist)->top

int resize_cirArrList(cirArrList_st *plist, int size);

void cirArrList_traverse(cirArrList_st *plist, int (*handler)(void *, void *) , void *arg);
void *cirArrList_find(cirArrList_st *plist, void *arg, compare_func compare);
int cirArrList_indexof(cirArrList_st *plist, void *arg, compare_func compare);


#define cirArrList_contains(plist, arg, compare)		(cirArrList_indexof(plist, arg, compare) >= 0)

void *cirArrList_get(cirArrList_st *plist, int index);
void cirArrList_set(cirArrList_st *plist, int index, void *pdata);
int cirArrList_insert(cirArrList_st *plist, void *pdata);
int cirArrList_add(cirArrList_st *plist, void *pdata);
int cirArrList_add_array(cirArrList_st *plist, void *pdata, int count);
int cirArrList_remove_head(cirArrList_st *plist);
int cirArrList_remove_tail(cirArrList_st *plist);

void cirArrList_print(cirArrList_st *plist, void (*print_func)(void *));

#endif

/*
 *  array List
 */

 #ifndef __ARRAY_LIST_H__
#define __ARRAY_LIST_H__

#include <pthread.h>

#include "dataflag.h"

struct array_list{
	int count;
	int max_size;
	char data_attr;
	char data_type;
	short maskbit_size;
	int data_size;
	int last_index;
	void *maskbit;
	void *datalist;
	pthread_rwlock_t rwlock;
	
};

typedef struct array_list arraylist_st;

#define	INIT_ARRAYLIST(plist)	({\
	(plist)->count = 0;	\
	(plist)->data_attr = 0;	\
	(plist)->data_type = 0;	\
	(plist)->maskbit_size = 0;	\
	(plist)->max_size = 0;	\
	(plist)->last_index = -1;	\
	(plist)->maskbit = NULL;	\
	(plist)->datalist = NULL;	\
	pthread_rwlock_init(&((plist)->rwlock), NULL); \
})
	
#define rdlock_arraylist(plist)		pthread_rwlock_rdlock(&((plist)->rwlock))
#define wrlock_arraylist(plist)		pthread_rwlock_wrlock(&((plist)->rwlock))
#define unlock_arraylist(plist)		pthread_rwlock_unlock(&((plist)->rwlock))

#define arraylist_size(plist)		(plist)->count

#define arraylist_isEmpty(plist)		((plist)->count == 0)
#define arraylist_isFull(plist)	((plist)->count >= (plist)->max_size)


int init_arraylist(arraylist_st *plist, int list_size, int data_flag);
arraylist_st *create_arraylist(int list_size, int data_flag);
void destroy_arraylist(arraylist_st *plist);

void arraylist_empty(arraylist_st *plist);
void arraylist_clean(arraylist_st *plist);

int arraylist_add(arraylist_st *plist, void *pdata);


int arraylist_del(arraylist_st *plist, void *arg, compare_func compare);
int arraylist_remove(arraylist_st *plist, int index);

void arraylist_traverse(arraylist_st *plist, int (*handler)(void *, void *) , void *arg);
void *arraylist_find(arraylist_st *plist, void *arg, compare_func compare);
int arraylist_indexof(arraylist_st *plist, void *arg, compare_func compare);


#define arraylist_contains(plist, arg, compare)		(arraylist_indexof(plist, arg, compare) >= 0)

void *arraylist_get(arraylist_st *plist, int index);
int arraylist_set(arraylist_st *plist, int index, void *pdata);
int arraylist_modify(arraylist_st *plist, int index, dowith_func handler, void *arg);

void arraylist_print(arraylist_st *plist, void (*print_func)(void *));


#endif


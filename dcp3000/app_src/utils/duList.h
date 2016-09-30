/*
 * Doubly Linked List
 */

#ifndef __DOUBLY_LINKED_LIST_H__
#define __DOUBLY_LINKED_LIST_H__

#include <pthread.h>

#include "node.h"

struct doubly_linked_list{
	int count;
	int data_type;
	int max_size;
	struct doubly_linked_node *head;
	struct doubly_linked_node *tail;
	struct doubly_linked_node *cursor;
	pthread_rwlock_t rwlock;
	pthread_mutex_t lock;
	
};

typedef struct doubly_linked_list duList_st;


#define	INIT_DULIST(pdulist)	({\
	(pdulist)->count = 0;	\
	(pdulist)->data_type = 0;	\
	(pdulist)->max_size = DATA_SIZE_MAX;	\
	(pdulist)->head = NULL;	\
	(pdulist)->tail = NULL;	\
	(pdulist)->cursor = NULL;	\
	pthread_rwlock_init(&((pdulist)->rwlock), NULL); \
	pthread_mutex_init(&((pdulist)->lock), NULL);  \
})


#define rdlock_duList(pduList)		pthread_rwlock_rdlock(&((pduList)->rwlock))
#define wrlock_duList(pduList)		pthread_rwlock_wrlock(&((pduList)->rwlock))
#define unlock_duList(pduList)		pthread_rwlock_unlock(&((pduList)->rwlock))


#define duList_size(pduList)		(pduList)->count
#define resize_duList(pduList, size)		(pduList)->max_size = size


#define duList_isEmpty(pduList)	((pduList)->count <= 0)
#define duList_isFull(pduList)		((pduList)->count >= (pduList)->max_size)

void init_duList(duList_st *pduList, int data_type);
void duList_empty(duList_st *pduList);
duList_st *create_duList(int data_type);
void destroy_duList(duList_st *pduList);


int duList_add_node(duList_st *pduList, dnode_st *dnode);
int duList_insert_node(duList_st *pduList, dnode_st *dnode);
int duList_del_node(duList_st *pduList, dnode_st *dnode);

dnode_st *duList_separate_node(duList_st *pduList, dnode_st *dnode);
dnode_st *duList_separate_head(duList_st *pduList);
dnode_st *duList_separate_tail(duList_st *pduList);


int dnode_insert(dnode_st *dnode, dnode_st *curdnode);
int dnode_append(dnode_st *dnode, dnode_st *curdnode);

dnode_st *duList_get_node(duList_st *pduList, int index);
dnode_st *duList_set_node(duList_st *pduList, dnode_st *dnode, void *pdata, int data_flag);
dnode_st *duList_create_node(duList_st *pduList, void *pdata, int data_flag);
dnode_st *duList_find_node(duList_st *pduList, void *pdata, compare_func compare);
	
void * duList_head(duList_st *pduList);
void * duList_tail(duList_st *pduList);


int duList_add(duList_st *pduList, void *pdata, int data_flag);
int duList_addto(duList_st *pduList, int index, void *pdata, int data_flag);
int duList_insert(duList_st *pduList, void *pdata, int data_flag);

int duList_del(duList_st *pduList, void *pdata, compare_func compare);
int duList_remove(duList_st *pduList, int index);

void duList_traverse(duList_st *pduList, int (*handler)(void *, void *) , void *arg);
void *duList_find(duList_st *pduList, void *pdata, compare_func compare);
int duList_indexof(duList_st *pduList, void *pdata, compare_func compare);

#define duList_contains(pduList, pdata)		(duList_find(pduList, pdata) != NULL)

void *duList_get(duList_st *pduList, int index);
dnode_st *duList_set(duList_st *pduList, int index, void *pdata, int data_flag);

void *duList_access(duList_st *pduList, int index);
void *duList_next(duList_st *pduList);
void *duList_prev(duList_st *pduList);
void duList_access_end(duList_st *pduList);

void duList_print(duList_st *pduList, void (*print_func)(void *));

#endif

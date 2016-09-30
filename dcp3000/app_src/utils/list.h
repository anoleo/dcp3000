/*
 *  Linked List
 */

 #ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <pthread.h>

#include "node.h"

struct linked_list{
	int count;
	int data_type;
	int max_size;
	struct single_linked_node *head;
	struct single_linked_node *tail;
	struct single_linked_node *cursor;
	pthread_rwlock_t rwlock;
	pthread_mutex_t lock;
	
};

typedef struct linked_list list_st;

#define	INIT_LIST(plist)	({\
	(plist)->count = 0;	\
	(plist)->data_type = 0;	\
	(plist)->max_size = DATA_SIZE_MAX;	\
	(plist)->head = NULL;	\
	(plist)->tail = NULL;	\
	(plist)->cursor = NULL;	\
	pthread_rwlock_init(&((plist)->rwlock), NULL); \
	pthread_mutex_init(&((plist)->lock), NULL);  \
})


#define rdlock_list(plist)		pthread_rwlock_rdlock(&((plist)->rwlock))
#define wrlock_list(plist)		pthread_rwlock_wrlock(&((plist)->rwlock))
#define unlock_list(plist)		pthread_rwlock_unlock(&((plist)->rwlock))

#define list_size(plist)		(plist)->count
#define resize_list(plist, size)		(plist)->max_size = size

#define list_isEmpty(plist)		((plist)->count == 0)
#define list_isFull(plist)	((plist)->count >= (plist)->max_size)


void init_list(list_st *plist, int data_type);
void list_empty(list_st *plist);
list_st *create_list(int data_type);
void destroy_list(list_st *plist);


int list_add_node(list_st *plist, snode_st *snode);
int list_insert_node(list_st *plist, snode_st *snode);
int list_del_node(list_st *plist, snode_st *snode);

snode_st *list_separate_node(list_st *plist, snode_st *snode);
snode_st *list_separate_head(list_st *plist);
snode_st *list_separate_tail(list_st *plist);


int snode_insert(snode_st *snode, snode_st *cursnode);
int snode_append(snode_st *snode, snode_st *cursnode);

snode_st *list_get_node(list_st *plist, int index);
snode_st *list_set_node(list_st *plist, 	snode_st *snode, void *pdata, int data_flag);
snode_st *list_create_node(list_st *plist, void *pdata, int data_flag);
snode_st *list_find_node(list_st *plist, void *pdata, compare_func compare);

void * list_head(list_st *plist);
void * list_tail(list_st *plist);


int list_add(list_st *plist, void *pdata, int data_flag);
int list_addto(list_st *plist, int index, void *pdata, int data_flag);
int list_insert(list_st *plist, void *pdata, int data_flag);
int list_add_array(list_st *plist, void *pdata, int data_size, int count);

int list_del(list_st *plist, void *pdata, compare_func compare);
int list_remove(list_st *plist, int index);

void list_traverse(list_st *plist, int (*handler)(void *, void *) , void *arg);
void *list_find(list_st *plist, void *pdata, compare_func compare);
int list_indexof(list_st *plist, void *pdata, compare_func compare);


#define list_contains(plist, pdata)		(list_find(plist, pdata) != NULL)

void *list_get(list_st *plist, int index);
snode_st *list_set(list_st *plist, int index, void *pdata, int data_flag);

void list_access_node(list_st *plist, snode_st *snode);
snode_st *list_next_node(list_st *plist);

void *list_access(list_st *plist, int index);
void *list_next(list_st *plist);

void list_access_end(list_st *plist);

void list_print(list_st *plist, void (*print_func)(void *));

/*
#define list_add(plist, pdata, data_flag)		list_add_node(plist, list_create_node(plist, pdata, data_flag))
#define list_insert(plist, pdata, data_flag)	list_insert_node(plist, list_create_node(plist, pdata, data_flag))

#define list_del(plist, pdata)	list_del_node(plist, list_find(plist, pdata))
#define list_remove(plist, index)		list_del_node(plist, list_get_node(plist, index))

//*/

#endif


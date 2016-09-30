
#ifndef __LINKED_NODE_H__
#define __LINKED_NODE_H__

#include <limits.h>
#include <sys/types.h>

#include "dataflag.h"

#define SNODE_TYPE	1
#define DNODE_TYPE	2

struct node_header{
	int data_size;
	char node_type;
	char node_flag;
	char data_type;
	char data_attr;
	void *parent;
};


#define	NODE_HEADER	int data_size;	int node_flag; void *parent;
		
#define	INIT_NODE_HEADER(pheader)	({\
	(pheader)->data_size = 0;	\
	(pheader)->data_type = 0;	\
	(pheader)->data_attr = 0;	\
	(pheader)->node_type = 0;	\
	(pheader)->node_flag = 0;	\
	(pheader)->parent = NULL;	\
})

struct single_linked_node{
	struct node_header header;
	void *data;
	struct single_linked_node *next;
};

struct doubly_linked_node{
	struct node_header header;
	void *data;
	struct doubly_linked_node *next;
	struct doubly_linked_node *prev;
};


typedef struct single_linked_node snode_st;
typedef struct doubly_linked_node dnode_st;

typedef struct single_linked_node node_st;
//typedef struct doubly_linked_node node_st;


snode_st *create_snode(void *pdata, int data_flag);
snode_st *alloc_snode(int data_size);

dnode_st *create_dnode(void *pdata, int data_flag);
dnode_st *alloc_dnode(int data_size);

void __free_node(node_st *pnode);
int __node_clear(node_st *pnode);
int __node_set_data(node_st *pnode, void *pdata, int data_flag);
int __node_reset_data(node_st *pnode, void *pdata, int data_flag);

int __node_equals(node_st *pnode, void *pdata);
void __print_node(node_st *pnode);

#define node_equals(pnode, pdata)			__node_equals((node_st *)pnode, pdata)
#define print_node(pnode)	 __print_node((node_st *)pnode)

#define free_node(pnode)			__free_node((node_st *)pnode)
#define node_clear(pnode)		__node_clear((node_st *)pnode)
#define node_set_data(pnode, pdata, data_flag)	__node_set_data((node_st *)pnode, pdata, data_flag)
#define node_reset_data(pnode, pdata, data_flag)	__node_reset_data((node_st *)pnode, pdata, data_flag)

#define free_snode(psnode)		free_node(psnode)
#define snode_clear(psnode)		node_clear(psnode)
#define snode_set_data(psnode, pdata, data_flag)		node_set_data(psnode, pdata, data_flag)
#define snode_reset_data(psnode, pdata, data_flag)	node_reset_data(psnode, pdata, data_flag)

#define free_dnode(pdnode)		free_node(pdnode)
#define dnode_clear(pdnode)		node_clear(pdnode)
#define dnode_set_data(pdnode, pdata, data_flag)		node_set_data(pdnode, pdata, data_flag)
#define dnode_reset_data(pdnode, pdata, data_flag)	node_reset_data(pdnode, pdata, data_flag)


#endif




 #ifndef __CIRCULAR_QUEUE_H__
#define __CIRCULAR_QUEUE_H__

#include "list.h"

typedef list_st cirqueue_st;

cirqueue_st *create_cirqueue(int data_type, int queue_size);
void destroy_cirqueue(cirqueue_st *queue);

void cirqueue_empty(cirqueue_st *queue);

#define cirqueue_isEmpty(queue)		list_isEmpty(queue)
#define cirqueue_isFull(queue)		list_isFull(queue)


int circular_enqueue(cirqueue_st *queue, void *pdata, int data_flag);
void *circular_dequeue(cirqueue_st *queue);
snode_st *circular_dequeue_node(cirqueue_st *queue);

#endif

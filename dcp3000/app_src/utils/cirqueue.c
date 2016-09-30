
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cirqueue.h"


cirqueue_st *create_cirqueue(int data_type, int queue_size)
{
	snode_st *snode;
	cirqueue_st *queue;
	int i;

	if(queue_size <= 0){
		return NULL;
	}
	
	queue = create_list(data_type);
	
	if(queue_size>DATA_SIZE_MAX){
		queue_size = DATA_SIZE_MAX;
	}
	//lock_list(queue);
	queue->max_size = queue_size;
	
	for(i=0; i<queue_size; i++){

		snode = list_create_node(queue, NULL, 0);
		
		if(queue->head == NULL){
			queue->head = snode;
		}else{
			queue->tail->next = snode;
		}

		queue->tail = snode;
	}

	queue->tail->next = queue->head;
	queue->tail = queue->head;
	
	//unlock_list(queue);
	
	return queue;
}

void destroy_cirqueue(cirqueue_st *queue)
{

	snode_st *snode, *next_snode;
	
	if(queue == NULL){
		return;
	}
	
	wrlock_list(queue);

	snode = queue->head->next;
	queue->head->next = NULL;
	
	while(snode){
		next_snode = snode->next;
		free_snode(snode);
		snode = next_snode;
	}

	queue->max_size = 0;
	queue->count = 0;
	
	unlock_list(queue);

	pthread_rwlock_destroy(&queue->rwlock);

	free(queue);
	
}


void cirqueue_empty(cirqueue_st *queue)
{
	snode_st *snode;
	
	if(queue == NULL){
		return;
	}
	
	wrlock_list(queue);

	snode = queue->head;
	
	while(snode){
		snode_clear(snode);
		snode = snode->next;
		if(snode == queue->head){
			break;
		}
	}

	queue->count = 0;
	
	unlock_list(queue);

	pthread_rwlock_destroy(&queue->rwlock);

	free(queue);
}

int resize_cirqueue(cirqueue_st *queue, int size)
{
	snode_st *snode, *next_snode = NULL;
	int i;
	
	if(queue == NULL){
		return -1;
	}

	if(size>DATA_SIZE_MAX){
		size = DATA_SIZE_MAX;
	}
	
	wrlock_list(queue);

	if(size<queue->count){
		size = -1;
		goto End_Func;
	}else if(size == queue->max_size){
		goto End_Func;
	}else if(size < queue->max_size){
		for(i=queue->max_size - size; i>0; i--){
			next_snode = queue->tail->next;
			queue->tail->next = next_snode->next;
			free_snode(next_snode);
		}
	}else if(size > queue->max_size){
	
		next_snode = queue->tail->next;
		for(i=queue->max_size; i<size; i++){
			snode = list_create_node(queue, NULL, 0);
			queue->tail->next = snode;
			queue->tail = snode;
		}
		queue->tail->next = next_snode;
	}
		
	queue->max_size = size;

	End_Func	:
	unlock_list(queue);
	
	return size;
}


int circular_enqueue(cirqueue_st *queue, void *pdata, int data_flag)
{
	snode_st *snode;

	if((queue == NULL) || (pdata == NULL)){
		return -1;
	}

	wrlock_list(queue);
	
	if(queue->count >= queue->max_size){
		unlock_list(queue);
		return -1;
	}
	
	snode = queue->tail;
	queue->tail = snode->next;
	
	if(VOID_DATA != queue->data_type){
		data_flag |= (queue->data_type << 24);
	}
	snode_reset_data(snode, pdata, data_flag);
	
	queue->count++;
	
	unlock_list(queue);
		
	return queue->count;
}

snode_st *circular_dequeue_node(cirqueue_st *queue)
{
	snode_st *snode = NULL;
	
	if((queue == NULL)){
		return NULL;
	}


	wrlock_list(queue);

	if(queue->count > 0){

		snode = queue->head;
		
		//if(snode == queue->tail){
			
		//}
		
		queue->head = snode->next;
		
		queue->count--;

	}

	unlock_list(queue);
		
	return snode;
}

void *circular_dequeue(cirqueue_st *queue)
{
	void *pdata = NULL;
	snode_st *snode = NULL;

	snode = circular_dequeue_node(queue);

	if(snode){
		pdata = snode->data;
		snode->data = NULL;
		snode_clear(snode);
	}
		
	return pdata;
}
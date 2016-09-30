#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "node.h"
#include "utils.h"


int __node_equals(node_st *pnode, void *pdata)
{
	if(pnode == NULL){
		return -1;
	}

	if(IMMEDIATE_DATA == pnode->header.data_attr){
		if(pdata == pnode->data)
			return 1;
		else
			return 0;
	}
	
	switch(pnode->header.data_type){
		case	VOID_DATA :
		case	STRUCT_DATA :
		case	ARRAY_DATA :
			if(ASSIGN_DATA == pnode->header.data_attr){
				if(pdata == pnode->data)
					return 1;
			}else if(ALLOC_DATA == pnode->header.data_attr){
				if(memcmp(pnode->data, pdata, pnode->header.data_size)==0)
					return 1;
			}else if(STATIC_DATA == pnode->header.data_attr){
				if(pdata == pnode->data)
					return 1;
				else if(memcmp(pnode->data, pdata, pnode->header.data_size)==0)
					return 1;
			}
			
			break;
		case	CHAR_DATA :
		case	UCHAR_DATA :
		case	SHORT_DATA :
		case	USHORT_DATA :
		case	INT_DATA :
		case	UINT_DATA :
		case	POINTER_DATA :
			if(pdata == pnode->data)
				return 1;
			break;
		case	LLONG_DATA :
			if(memcmp(pnode->data, pdata, sizeof(long long))==0)
				return 1;
			break;				
		case	FLOAT_DATA :
			if(floatcmp((float *)pnode->data, (float *) pdata) == 0)
				return 1;
			break;
		case	DOUBLE_DATA :
			if(doublecmp((double *)pnode->data, (double *) pdata) == 0)
				return 1;
			break;
		case	STRING_DATA :
			if(pnode->data == pdata)
				return 1;
			else if(strcmp(pdata, pnode->data) == 0)
				return 1;
			if(ASSIGN_DATA == pnode->header.data_attr){
				if(pnode->data == pdata)
					return 1;
			}else if(ALLOC_DATA == pnode->header.data_attr){
				if(strcmp(pdata, pnode->data) == 0)
					return 1;
			}else if(STATIC_DATA == pnode->header.data_attr){
				if(pnode->data == pdata)
					return 1;
				else if(strcmp(pdata, pnode->data) == 0)
					return 1;
			}
			break;
		default :
			return -1;
			break;
	}

	return 0;
}


int __node_set_data(node_st *pnode, void *pdata, int data_flag)
{
	char data_type, data_attr;
	int data_size;
	
	if(pnode == NULL){
		return -1;
	}

	data_size = DATA_SIZE(data_flag);
	data_type = DATA_TYPE(data_flag);
	data_attr = DATA_ATTR(data_flag);

	if((data_attr == 0) && pdata){
		data_attr = ALLOC_DATA;
	}

	switch(data_type){
		case	CHAR_DATA :
		case	UCHAR_DATA :
			data_attr = IMMEDIATE_DATA;
			data_size = sizeof(char);
			break;
		case	SHORT_DATA :
		case	USHORT_DATA :
			data_attr = IMMEDIATE_DATA;
			data_size = sizeof(short);
			break;
		case	INT_DATA :
		case	UINT_DATA :
		case	POINTER_DATA :
			data_attr = IMMEDIATE_DATA;
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
			if((data_size<=0) && (data_attr == ALLOC_DATA)){
				return -1;
			}
			break;
	}

	if((pdata == NULL) && (data_attr!=IMMEDIATE_DATA)){
		return -1;
	}

	pnode->header.data_type = data_type;
	pnode->header.data_size = data_size;
	pnode->header.data_attr = data_attr;

	if(ALLOC_DATA == data_attr){
		if(data_size < 0){return 0;}
		pnode->data = malloc(data_size);
		if(pnode->data == NULL){
			perror("node data\n");
			exit(-1);
		}
		memcpy(pnode->data, pdata, data_size);
	}else{
		pnode->data = pdata;
	}

	return 0;
}


void __free_node(node_st *pnode)
{
	if(pnode == NULL){
		return;
	}

	if((pnode->header.data_attr == ASSIGN_DATA) || (pnode->header.data_attr == ALLOC_DATA)){
		if(pnode->data)
			free(pnode->data);
	}

	free(pnode);
}


int __node_clear(node_st *pnode)
{
	if(pnode == NULL){
		return -1;
	}
	

	if((pnode->header.data_attr == ASSIGN_DATA) || (pnode->header.data_attr == ALLOC_DATA)){
		if(pnode->data)
		free(pnode->data);
	}
	
	pnode->header.data_size = 0;
	pnode->header.data_attr = NULL_DATA;
	
	pnode->data = NULL;

	return 0;
}

int __node_reset_data(node_st *pnode, void *pdata, int data_flag)
{
	if(pnode == NULL){
		return -1;
	}

	if((pnode->header.data_attr == ASSIGN_DATA) || (pnode->header.data_attr == ALLOC_DATA)){
		if(pnode->data)
		free(pnode->data);
	}

	return node_set_data(pnode, pdata, data_flag);
}

snode_st *alloc_snode(int data_size)
{
	snode_st *psnode;
	
	psnode = malloc(sizeof(snode_st));
	if(psnode == NULL){
		perror("snode_st\n");
		exit(-1);
	}

	INIT_NODE_HEADER(&psnode->header);
	psnode->header.node_type = SNODE_TYPE;

	if(data_size>0){
			
		psnode->header.data_size = data_size;
		psnode->header.data_attr = ALLOC_DATA;
		
		psnode->data = malloc(data_size);
		if(psnode->data == NULL){
			perror("snode data\n");
			exit(-1);
		}

	}
	
	psnode->next = NULL;
	
	return psnode;
}

snode_st *create_snode(void *pdata, int data_flag)
{
	snode_st *psnode;

	psnode = malloc(sizeof(snode_st));
	if(psnode == NULL){
		perror("snode_st\n");
		exit(-1);
	}

	if(node_set_data((node_st *)psnode, pdata, data_flag) < 0){
		free(psnode);
		return NULL;
	}

	
	psnode->header.node_type = SNODE_TYPE;
	psnode->header.parent = NULL;
	psnode->next = NULL;
	
	return psnode;
}


dnode_st *alloc_dnode(int data_size)
{
	dnode_st *pdnode;
	
	pdnode = malloc(sizeof(dnode_st));
	if(pdnode == NULL){
		perror("dnode_st\n");
		exit(-1);
	}

	INIT_NODE_HEADER(&pdnode->header);
	pdnode->header.node_type = DNODE_TYPE;

	if(data_size>0){
		
		pdnode->header.data_size = data_size;
		pdnode->header.data_attr = ALLOC_DATA;
		
		pdnode->data = malloc(data_size);
		if(pdnode->data == NULL){
			perror("dnode data\n");
			exit(-1);
		}

	}
	
	pdnode->next = NULL;
	pdnode->prev = NULL;
	
	return pdnode;
}

dnode_st *create_dnode(void *pdata, int data_flag)
{
	dnode_st *pdnode;

	pdnode = malloc(sizeof(dnode_st));
	if(pdnode == NULL){
		perror("dnode_st\n");
		exit(-1);
	}

	if(node_set_data((node_st *)pdnode, pdata, data_flag) < 0){
		free(pdnode);
		return NULL;
	}
	
	pdnode->header.node_type = DNODE_TYPE;
	pdnode->header.parent = NULL;
	pdnode->next = NULL;
	pdnode->prev = NULL;

	return pdnode;
}

void __print_node(node_st *pnode)
{
	if(pnode == NULL){
		return;
	}

	print_data(pnode->data, DATA_FLAG(pnode->header.data_attr, pnode->header.data_type, pnode->header.data_size));
}


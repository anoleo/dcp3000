
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "board_info.h"

#include "utils/queue.h"



int compare_devif_type(void *data1, void *data2)
{
	devif_st *pdevif = (devif_st *)data1;

	//printf("compare_devif_type, devif type = %d, param %d \n", pdevif->type, (int)data2);

	if(pdevif->type == (int)data2){
		return 0;
	}else if(pdevif->type > (int)data2){
		return 1;
	}else if(pdevif->type < (int)data2){
		return -1;
	}
	
}

void print_funcchan(void *data)
{
	funcchan_st *pchan = (funcchan_st *)data;
	devif_st *devif;
	if(pchan == NULL){
		return ;
	}

	printf("chan type = %d \n", pchan->type);
	printf("chan trans_mode = %d \n", pchan->trans_mode);
	
	if(pchan->type != ABSTRACT_CHANIF){
		if(pchan->devif){
			devif = funcchan_devif(pchan);
			//printf("print_funcchan : %p, %p, %p \n", pchan, pchan->devif, devif);
			printf("chan devif = %s \n", devif->name);
		}
	
		if(pchan->type == ONE2MANY_CHANIF){
			printf("chan ifno = %d \n", pchan->ifno);
		}else if((pchan->type == ONE2ONE_CHANIF) || (pchan->type == MANY2MANY_CHANIF)){
			printf("chan start_ifno = %d \n", pchan->start_ifno);
			printf("chan end_ifno = %d \n", pchan->end_ifno);
		}
	}
	
	printf("chan start_indexf = %d \n", pchan->start_index);
	printf("chan end_index = %d \n", pchan->end_index);

}

void print_devif(void *data)
{
	devif_st *pdevif = (devif_st *)data;

	if(pdevif == NULL){
		return ;
	}

	printf("devif type = %#x \n", pdevif->type);
	printf("devif name = %s \n", pdevif->name);
	printf("devif count = %d \n", pdevif->count);
}

void print_funcmod(void *data)
{
	int i;
	sArrList_st *chanlist;
	funcmod_st *pfunc = (funcmod_st *)data;
	funcchan_st *pchan;
	
	if(pfunc == NULL){
		return ;
	}

	printf("func type = %#x \n", pfunc->type);
	printf("func name = %s \n", pfunc->name);
	printf("func channel count = %d \n", pfunc->channel_count);

	//chanlist = pfunc->channel_list;
	chanlist = func_chanlist(pfunc);
	sArrList_print(chanlist, print_funcchan);

	
}

void print_boardinfo(struct board_card *pboard)
{
	int i, j, k;
	funcmod_st *func;
	funcchan_st *chan;
	sArrList_st *deviflist;
	sArrList_st *funclist;
	sArrList_st *chanlist;
	devif_st *devif;
	
	if(pboard == NULL){
		return;
	}
	
	printf("board slot = %d \n", pboard->slot);
	printf("board type = %#x \n", pboard->type);
	printf("board name = %s \n", pboard->name);
	printf("board descriptions = %s \n", pboard->descriptions);
	printf("board version = %#x \n", pboard->version);

	//deviflist = pboard->devif_list;
	//funclist = pboard->func_list;
	
	deviflist = board_deviflist(pboard);
	funclist = board_funclist(pboard);
	
	sArrList_print(deviflist, print_devif);
	sArrList_print(funclist, print_funcmod);


}


void free_boardinfo(struct board_card *pboard)
{
	if(pboard == NULL){
		return;
	}
	
	//printf("free %d boardinfo \n", pboard->slot);
	destroy_sArrList(pboard->devif_list);
	destroy_sArrList(pboard->func_list);
	free(pboard);
}

board_st *parse_board_info(char *board_file)
{
	xmlDocPtr doc;
	xmlNodePtr root, lv1node, lv2node, lv3node, lv4node;
	xmlChar *nodetext, *attrval;

	board_st *board;
	devif_st *devif;
	funcmod_st *func;
	funcchan_st *chan;

	int devif_count = 0, func_count = 0, chans_count = 0;
	list_st *devifList,*funcList, *chansList;


	doc = xmlReadFile(board_file, "utf-8", 256);

	if (doc == NULL ) {
		fprintf(stderr,"Document %s not parsed successfully. \n", board_file);
		return NULL;	
	}

	printf("read %s \n", board_file);

	devifList = create_list(STRUCT_DATA);
	funcList = create_list(STRUCT_DATA);
	chansList = create_list(STRUCT_DATA);
			
	root = xmlDocGetRootElement(doc);  
	if (root == NULL) {
		fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	if (xmlStrcmp(root->name, (const xmlChar *) "broad_card")) {
		fprintf(stderr,"document of the wrong type, root node != broad_card");
		xmlFreeDoc(doc);
		return NULL;
	}

	InfoPrintf("get the root node broad_card \n");

	board = malloc(sizeof(board_st));
	if(board == NULL){
		perror("board_card");
		exit(-1);
	}

	board->descriptions[0] = 0;
	
	lv1node = root->xmlChildrenNode;
	while(lv1node!=NULL) {
		
		if (xmlStrEqual(lv1node->name, (const xmlChar *)"type")) {
			
			nodetext = xmlNodeListGetString(doc, lv1node->xmlChildrenNode, 1);
			InfoPrintf("broad_card type: %s\n", nodetext);
			board->type = strtoul(nodetext, NULL, 16);
			xmlFree(nodetext);
			
		}else if (xmlStrEqual(lv1node->name, (const xmlChar *)"name")) {
		
			nodetext = xmlNodeListGetString(doc, lv1node->xmlChildrenNode, 1);
			InfoPrintf("broad_card name: %s\n", nodetext);
			strcpy(board->name, nodetext);
			xmlFree(nodetext);
			
		}
		//*
		else if (xmlStrEqual(lv1node->name, (const xmlChar *)"descriptions")) {
		
			nodetext = xmlNodeListGetString(doc, lv1node->xmlChildrenNode, 1);
			InfoPrintf("broad_card descriptions: %s\n", nodetext);
			if(nodetext){
				strcpy(board->descriptions, nodetext);
				xmlFree(nodetext);
			}
			
		}
		//*/
		else if (xmlStrEqual(lv1node->name, (const xmlChar *)"version")) {
		
			nodetext = xmlNodeListGetString(doc, lv1node->xmlChildrenNode, 1);
			InfoPrintf("broad_card version: %s\n", nodetext);
			board->version = strtoul(nodetext, NULL, 16);
			xmlFree(nodetext);
			
		}else if (xmlStrEqual(lv1node->name, (const xmlChar *)"device_interfaces")) {

			/*
			attrval = xmlGetProp (lv1node, (const xmlChar *)"devif_count");
			InfoPrintf("device_interfaces devif_count: %s\n", attrval);
			board->devif_list = create_sArrList(atoi(attrval), sizeof(devif_st));
			xmlFree(attrval);
			//*/
			
			devif_count = 0;
			lv2node = lv1node->xmlChildrenNode;
			while(lv2node){
				
				if (xmlStrEqual(lv2node->name, (const xmlChar *)"devif")) {
					//devif = (devif_st *)sArrList_get(board->devif_list, devif_count++);
					devif = xMalloc(sizeof(devif_st));
					list_enqueue(devifList, devif, ASSIGN_FLAG);
					
					lv3node = lv2node->xmlChildrenNode;
					while(lv3node){
						
						if (xmlStrEqual(lv3node->name, (const xmlChar *)"type")) {
							nodetext = xmlNodeListGetString(doc, lv3node->xmlChildrenNode, 1);
							InfoPrintf("devif type: %s\n", nodetext);
							devif->type = strtoul(nodetext, NULL, 16);
							xmlFree(nodetext);
						}else if (xmlStrEqual(lv3node->name, (const xmlChar *)"name")) {
							nodetext = xmlNodeListGetString(doc, lv3node->xmlChildrenNode, 1);
							InfoPrintf("devif name: %s\n", nodetext);
							strcpy(devif->name, nodetext);
							xmlFree(nodetext);
						}else if (xmlStrEqual(lv3node->name, (const xmlChar *)"count")) {
							nodetext = xmlNodeListGetString(doc, lv3node->xmlChildrenNode, 1);
							InfoPrintf("devif count: %s\n", nodetext);
							devif->count = atoi(nodetext);
							xmlFree(nodetext);
						}

						lv3node= lv3node->next;
					}

				}

				lv2node= lv2node->next;
			}
			
			board->devif_list = create_sArrList(devifList->count, sizeof(devif_st));
			devif = (devif_st *)list_dequeue(devifList);
			while(devif){
				sArrList_set(board->devif_list, devif_count++, devif);
				devif = (devif_st *)list_dequeue(devifList);
			}

		}else if (xmlStrEqual(lv1node->name, (const xmlChar *)"function_modules")) {

			/*
			attrval = xmlGetProp (lv1node, (const xmlChar *)"func_count");
			InfoPrintf("function_modules func_count: %s\n", attrval);
			board->func_list = create_sArrList(atoi(attrval), sizeof(funcmod_st));
			xmlFree(attrval);
			//*/
					
			func_count = 0;
			lv2node = lv1node->xmlChildrenNode;
			while(lv2node){
				
				if (xmlStrEqual(lv2node->name, (const xmlChar *)"func")) {
					
					//func = (funcmod_st *)sArrList_get(board->func_list, func_count++);
					func = xMalloc(sizeof(funcmod_st));
					list_enqueue(funcList, func, ASSIGN_FLAG);
					func->channel_count = -1;

					/*
					attrval = xmlGetProp (lv2node, (const xmlChar *)"chan_count");
					InfoPrintf("function chan_count: %s\n", attrval);
					func->channel_list = create_sArrList(atoi(attrval), sizeof(funcchan_st));
					xmlFree(attrval);
					//*/
					
					chans_count = 0;
					lv3node = lv2node->xmlChildrenNode;
					while(lv3node){
						
						if (xmlStrEqual(lv3node->name, (const xmlChar *)"type")) {
							
							nodetext = xmlNodeListGetString(doc, lv3node->xmlChildrenNode, 1);
							InfoPrintf("func type: %s\n", nodetext);
							func->type = strtoul(nodetext, NULL, 16);
							xmlFree(nodetext);
							
						}else if (xmlStrEqual(lv3node->name, (const xmlChar *)"name")) {
						
							nodetext = xmlNodeListGetString(doc, lv3node->xmlChildrenNode, 1);
							InfoPrintf("func name: %s\n", nodetext);
							strcpy(func->name, nodetext);
							xmlFree(nodetext);
							
						}else if (xmlStrEqual(lv3node->name, (const xmlChar *)"channels")) {
							
							//chan = (funcchan_st *)sArrList_get(func->channel_list, chans_count++);
							chan = xMalloc(sizeof(funcchan_st));
							list_enqueue(chansList, chan, ASSIGN_FLAG);
							
							attrval = xmlGetProp (lv3node, (const xmlChar *)"type");
							InfoPrintf("channels type: %s\n", attrval);
							chan->type = strtoul(attrval, NULL, 16);
							xmlFree(attrval);
							
							lv4node = lv3node->xmlChildrenNode;
							while(lv4node){
								
								if (xmlStrEqual(lv4node->name, (const xmlChar *)"iftype")) {
									nodetext = xmlNodeListGetString(doc, lv4node->xmlChildrenNode, 1);
									InfoPrintf("channels iftype: %s\n", nodetext);
									//chan->devif = (devif_st *)sArrList_find(board->devif_list, (void *)atoi(nodetext), compare_devif_type);
									chan->devif = (devif_st *)atoi(nodetext);
									xmlFree(nodetext);
								}else if (xmlStrEqual(lv4node->name, (const xmlChar *)"ifno")) {
									nodetext = xmlNodeListGetString(doc, lv4node->xmlChildrenNode, 1);
									InfoPrintf("channels ifno: %s\n", nodetext);
									chan->ifno = atoi(nodetext);
									xmlFree(nodetext);
								}else if (xmlStrEqual(lv4node->name, (const xmlChar *)"start_ifno")) {
									nodetext = xmlNodeListGetString(doc, lv4node->xmlChildrenNode, 1);
									InfoPrintf("channels start_ifno: %s\n", nodetext);
									chan->start_ifno = atoi(nodetext);
									xmlFree(nodetext);
								}else if (xmlStrEqual(lv4node->name, (const xmlChar *)"end_ifno")) {
									nodetext = xmlNodeListGetString(doc, lv4node->xmlChildrenNode, 1);
									InfoPrintf("channels end_ifno: %s\n", nodetext);
									chan->end_ifno = atoi(nodetext);
									xmlFree(nodetext);
								}else if (xmlStrEqual(lv4node->name, (const xmlChar *)"start_index")) {
									nodetext = xmlNodeListGetString(doc, lv4node->xmlChildrenNode, 1);
									InfoPrintf("channels start_index: %s\n", nodetext);
									chan->start_index = atoi(nodetext);
									xmlFree(nodetext);
								}else if (xmlStrEqual(lv4node->name, (const xmlChar *)"end_index")) {
									nodetext = xmlNodeListGetString(doc, lv4node->xmlChildrenNode, 1);
									InfoPrintf("channels end_index: %s\n", nodetext);
									chan->end_index = atoi(nodetext);
									xmlFree(nodetext);
									if(chan->end_index > func->channel_count){
										func->channel_count = chan->end_index;
									}
								}else if (xmlStrEqual(lv4node->name, (const xmlChar *)"trans_mode")) {
									nodetext = xmlNodeListGetString(doc, lv4node->xmlChildrenNode, 1);
									InfoPrintf("channels trans_mode: %s\n", nodetext);
									chan->trans_mode = atoi(nodetext);
									xmlFree(nodetext);
								}

								lv4node = lv4node->next;
							}


						}

						lv3node= lv3node->next;
					}
					
					func->channel_list = create_sArrList(chansList->count, sizeof(funcchan_st));
					chan = (funcchan_st *)list_dequeue(chansList);
					while(chan){
						sArrList_set(func->channel_list, chans_count++, chan);
						chan = (funcchan_st *)list_dequeue(chansList);
					}
							
					//func->channel_count = ((funcchan_st *)(sArrList_get(func->channel_list, func->channel_list->count-1)))->end_index+1;
					func->channel_count += 1;
					
				}

				lv2node= lv2node->next;
			}

			board->func_list = create_sArrList(funcList->count, sizeof(funcmod_st));
			func = (funcmod_st *)list_dequeue(funcList);
			while(func){
				sArrList_set(board->func_list, func_count++, func);
				func = (funcmod_st *)list_dequeue(funcList);
			}
			
		}

		lv1node = lv1node->next;
	}

	destroy_list(devifList);
	destroy_list(funcList);
	destroy_list(chansList);
	
	xmlFreeDoc(doc); 

	//print_boardinfo(board);
	
	return board;
}

funcmod_st *get_board_funcmod(board_st *board, u_int func_type)
{
	int i, j;
	funcmod_st *func;
	sArrList_st *funclist;

	if(board == NULL){
		return NULL;
	}
	
	funclist = board_funclist(board);

	DebugPrintf("get %d slot board %s : func type = %#x \n", board->slot, board->name, func_type);
	
	for(j=0; j<funclist->count; j++){
		func = sArrList_get(funclist, j);
		//TagPrintf(func->type);
		if(func && (func->type == func_type)){
			return func;
		}
	}

	return NULL;
}

int  fill_board_info_into_buf(board_st *board, char *data_buf)
{

	int i,  offset, len, data_len;
	funcmod_st *func;
	void *board_data, *chan_data;

	board_data = data_buf;

	offset = 0;
	memcpy(board_data + offset, board, sizeof(board_st));
	offset += sizeof(board_st);
	
	memcpy(board_data + offset - 8, &offset, 4);
	len = board->devif_list->count * board->devif_list->data_size + 8;
	memcpy(board_data + offset, board->devif_list, len);

	board_data += offset;

	offset += len;
	memcpy(board_data  - 4, &offset, 4);
	board_data += len;
	len = board->func_list->count * board->func_list->data_size + 8;
	memcpy(board_data, board->func_list, len);

	
	data_len = offset + len;

	chan_data = board_data + len;
	board_data += 8;
	offset = len - 8;
	for(i=1; i<=board->func_list->count; i++){
		func = sArrList_get(board->func_list, i-1);
		memcpy(board_data+ i*board->func_list->data_size -4, &offset, 4);
		len = func->channel_list->count * func->channel_list->data_size + 8;
		memcpy(chan_data, func->channel_list, len);
		offset += len - board->func_list->data_size;
		chan_data += len;
		data_len += len;
	}

	match_channel_devif((board_st *)data_buf);

	return data_len;
}


#if 0
void match_channel_devif(board_st *board)
{
	int i, j;
	funcmod_st *func;
	funcchan_st *chan;
	sArrList_st *deviflist;
	sArrList_st *funclist;
	sArrList_st *chanlist;

	funclist = board->func_list;

	for(i=0; i<funclist->count; i++){
		func = (funcmod_st *)sArrList_get(funclist, i);
		for(j=0; j<func->channel_list->count; j++){
			chan = (funcchan_st *)sArrList_get(func->channel_list, j);
			if(chan->type){
			chan->devif = (devif_st *)sArrList_find(board->devif_list, (void *)chan->devif, compare_devif_type);
			}else{
				chan->devif = NULL;
			}
		}
	}
}
#else
void match_channel_devif(board_st *board)
{
	int i, j, k;
	funcmod_st *func;
	funcchan_st *chan;
	sArrList_st *deviflist;
	sArrList_st *funclist;
	sArrList_st *chanlist;
	devif_st *devif;

	if(board==NULL){
		return ;
	}

	deviflist = board_deviflist(board);
	//board->devif_list = deviflist;
	funclist = board_funclist(board);
	//board->func_list = funclist;
	
	for(j=0; j<funclist->count; j++){
		func = sArrList_get(funclist, j);
		chanlist = func_chanlist(func);
		//func->channel_list = chanlist;
		for(k=0; k<chanlist->count; k++){
			chan = sArrList_get(chanlist, k);
			devif = sArrList_find(deviflist, (void *)chan->devif, compare_devif_type);
			chan->devif = (void *)chan  - (u_int)devif;
			//printf("match_channel_devif : %p, %p, %p \n", chan, chan->devif, devif);
		}
	}
}
#endif




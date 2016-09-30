
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../basedef.h"
#include "../utils/utils.h"
#include "table.h"

u_short get_sub_table_id(u_char *p_buf)
{
	u_short sub_table_id;
	
	sub_table_id = (p_buf[3]<<8) | p_buf[4];

	return sub_table_id;
}

section_t *create_section(void)
{
	section_t *p_section;
	
	p_section = malloc(sizeof(section_t));
	if(p_section == NULL){
 		printf("allocate section_t space fail, exit!\n");
		exit(1);
 	}

	p_section->len = 0;
	p_section->now_len = 0;
	p_section->crc32 = -1;
	p_section->section_number = 0xff;
	p_section->next = NULL;
	memset(p_section->data, 0xff, sizeof(p_section->data));

	return p_section;
}

void add_section(section_t **pp_head_section, section_t *p_section)
{
	section_t *cur_section;
	

	if(pp_head_section == NULL || p_section == NULL){
		return;
	}

#if 0
	if(*pp_head_section == NULL){
		
		*pp_head_section = p_section;
		
	}else{
	
		cur_section = *pp_head_section;
		while(cur_section->next){
			cur_section = cur_section->next;
		}

		cur_section->next = p_section;
	}
#else
	p_section->next = *pp_head_section;
	*pp_head_section = p_section;

#endif
	
}

void free_all_section(section_t *head_section)
{
	section_t *p_section, *next_section;
	
	p_section = head_section;

	while(p_section){
		next_section = p_section->next;
		free(p_section);
		p_section = next_section;
	}
}


eit_section_t *create_eit_section(void)
{
	eit_section_t *p_section;
	
	p_section = malloc(sizeof(eit_section_t));
	if(p_section == NULL){
 		printf("allocate eit section_t space fail, exit!\n");
		exit(1);
 	}

	p_section->len = 0;
	p_section->now_len = 0;
	p_section->crc32 = -1;
	p_section->section_number = 0xff;
	p_section->next = NULL;
	memset(p_section->data, 0xff, sizeof(p_section->data));

	return p_section;
}

void add_eit_section(eit_section_t **pp_head_section, eit_section_t *p_section)
{
	eit_section_t *cur_section;
	

	if(pp_head_section == NULL || p_section == NULL){
		return;
	}

#if 0
	if(*pp_head_section == NULL){
		
		*pp_head_section = p_section;
		
	}else{
	
		cur_section = *pp_head_section;
		while(cur_section->next){
			cur_section = cur_section->next;
		}

		cur_section->next = p_section;
	}
#else
	p_section->next = *pp_head_section;
	*pp_head_section = p_section;

#endif
	
}

void free_all_eit_section(eit_section_t *head_section)
{
	eit_section_t *p_section, *next_section;
	
	p_section = head_section;

	while(p_section){
		next_section = p_section->next;
		free(p_section);
		p_section = next_section;
	}
}


ts_sub_table_t *create_ts_sub_table(void)
{

	ts_sub_table_t *p_sub_table;

	p_sub_table = malloc(sizeof(ts_sub_table_t));
	if(p_sub_table == NULL){
		printf("malloc ts_sub_table failed,exit\n");
		exit(1);
	}

	p_sub_table->table_id = 0xff;
	p_sub_table->sub_table_id = 0xffff;
	p_sub_table->now_section_number = 0;
	p_sub_table->last_section_number = 0xffff;
	p_sub_table->section_count = 0;
	p_sub_table->isComplete = 0;
	p_sub_table->section = NULL;
	p_sub_table->next = NULL;
	memset(p_sub_table->section_num_mask, 0, SECTION_NUM_MASK_SIZE);


	return p_sub_table;
}

void free_all_ts_sub_table(ts_sub_table_t *head_sub_table)
{

	ts_sub_table_t *p_sub_table, *next_sub_table;

	p_sub_table = head_sub_table;
	while(p_sub_table){
		next_sub_table = p_sub_table->next;
		free_all_section(p_sub_table->section);
		free(p_sub_table);
		p_sub_table = next_sub_table;
	}
}

eit_segment_t *create_eit_segment(void)
{
	eit_segment_t *p_segment;

	p_segment = malloc(sizeof(eit_segment_t));
	if(p_segment == NULL){
		printf("malloc eit_segment failed, exit\n");
		exit(1);
	}

	p_segment->section = NULL;
	p_segment->section_count = 0;
	p_segment->segment_num = 0xff;
	p_segment->segment_last_section_number = 0;
	p_segment->next = 0;

	return p_segment;
}

void free_all_eit_segment(eit_segment_t *head_eit_segment)
{
	eit_segment_t *p_eit_segment, *next_eit_segment;

	p_eit_segment = head_eit_segment;
	while(p_eit_segment){
		next_eit_segment = p_eit_segment->next;
		free_all_eit_section(p_eit_segment->section);
		free(p_eit_segment);
		p_eit_segment = next_eit_segment;
	}
}

eit_sub_table_t *create_eit_sub_table(void)
{

	eit_sub_table_t *p_sub_table;

	p_sub_table = malloc(sizeof(eit_sub_table_t));
	if(p_sub_table == NULL){
		printf("malloc eit_sub_table failed,exit\n");
		exit(1);
	}

	p_sub_table->table_id = 0xff;
	p_sub_table->now_section_number = 0;
	p_sub_table->last_section_number = 0xffff;
	p_sub_table->segment_count = 0;
	p_sub_table->isComplete = 0;
	p_sub_table->segments = NULL;
	p_sub_table->cur_segment = NULL;
	p_sub_table->next = NULL;
	memset(p_sub_table->section_num_mask, 0, SECTION_NUM_MASK_SIZE);


	return p_sub_table;
}

void free_all_eit_sub_table(eit_sub_table_t *head_sub_table)
{

	eit_sub_table_t *p_sub_table, *next_sub_table;

	p_sub_table = head_sub_table;
	while(p_sub_table){
		next_sub_table = p_sub_table->next;
		free_all_eit_segment(p_sub_table->segments);
		free(p_sub_table);
		p_sub_table = next_sub_table;
	}
}



eit_table_t *create_eit_table(void)
{
	eit_table_t *p_eit;

	p_eit = malloc(sizeof(eit_table_t));
	if(p_eit == NULL){
		printf("malloc eit_table failed, exit\n");
		exit(1);
	}

	p_eit->isComplete = 0;
	p_eit->sub_table_count = 0;
	p_eit->last_table_id = 0;
	p_eit->service_id = 0xffff;
	p_eit->sub_tables = NULL;
	p_eit->next = NULL;

	return p_eit;
}

void free_all_eit_table(eit_table_t *head_eit)
{
	eit_table_t *p_eit, *next_eit;

	p_eit = head_eit;
	while(p_eit){
		next_eit = p_eit->next;
		free_all_eit_sub_table(p_eit->sub_tables);
		free(p_eit);
		p_eit = next_eit;
	}
}

void free_ts_eit_table(ts_in_table_t *p_ts_table, u_char table_id, u_short service_id)
{
	program_t *p_program;
	eit_table_t *p_eit, *prev_eit;
	eit_sub_table_t *p_sub_table, *prev_sub_table;

	if(p_ts_table==NULL){
		return;
	}

	printf("--------------free_ts_eit_table : table_id[ %#x ], service_id[ %d ]----------\n", table_id, service_id);
	
	p_program = find_ts_in_program(p_ts_table->pmt_tables, service_id);
	while(p_program){
		if(p_program->program_number == service_id){
			//printf("--------------free_ts_eit_table : ----pn-%d-----\n",  service_id);
	
			prev_eit = NULL;
			p_eit = p_program->eit_tables;
			while(p_eit){
				//printf("--------------free_ts_eit_table : -----1-----\n");
				prev_sub_table = NULL;
				p_sub_table = p_eit->sub_tables;
				if(p_sub_table){
					//printf("--------------free_ts_eit_table : -----2-----\n");
					while(p_sub_table){ 
						if(p_sub_table->table_id == table_id){
							//printf("--------------free_ts_eit_table : -----3-- table_id = %d---\n", table_id);
							free_all_eit_segment(p_sub_table->segments);
							if(prev_sub_table){
								//printf("--------------free_ts_eit_table : -----4.1-----\n");
								prev_sub_table->next = p_sub_table->next;
							}else{
								//printf("--------------free_ts_eit_table : -----4.2-----\n");
								p_eit->sub_tables = p_sub_table->next;
							}
							
							free(p_sub_table);
							break;
						}
						prev_sub_table = p_sub_table;
						p_sub_table = p_sub_table->next;
					}
					
					
				}

				if(p_eit->sub_tables == NULL){
					//printf("--------------free_ts_eit_table : -----5.1-----\n");
					if(prev_eit){
						//printf("--------------free_ts_eit_table : -----6.1-----\n");
						prev_eit->next = p_eit->next;
						free(p_eit);
						p_eit = prev_eit->next;
					}else{
					//printf("--------------free_ts_eit_table : -----6.2-----\n");
						p_program->eit_tables = p_eit->next;
						free(p_eit);
						p_eit = p_program->eit_tables;
					}
				}else{
				//printf("--------------free_ts_eit_table : -----5.2-----\n");
					p_eit->isComplete = 0;
					p_eit->sub_table_count--;
					if(table_id>=p_eit->last_table_id){
						p_eit->last_table_id = 0;
					}
					prev_eit = p_eit;
					p_eit = p_eit->next;
				}
			
			}
			break;
		}
		p_program = p_program->next;
	}
	
}

void free_all_service_eit_table(ts_in_table_t *p_ts_table)
{
	pmt_table_t *p_pmt;
	program_t *p_program;
	eit_table_t *p_eit;

	if(p_ts_table==NULL){
		return;
	}

	printf("--------------free_all_service_eit_table ----------\n");

	p_pmt = p_ts_table->pmt_tables;
	while(p_pmt){
		p_program = p_pmt->p_program;
		while(p_program){
			free_all_eit_table(p_program->eit_tables);
			p_program->eit_tables = NULL;
			p_program = p_program->next;
		}
		p_pmt = p_pmt->next;
	}
	
}

program_t * create_program(void)
{
	program_t *program = malloc(sizeof(program_t));
	if(program == NULL){
 		printf("allocate program_t space fail, exit\n");
		exit(1);
 	}
	
	program->eit_tables = NULL;

	program->pmt_section = NULL;
	program->isComplete = 0;

	program->program_number = -1;

	program->next = NULL;

	program->pcr_pid = 0x1fff;

	
	memset(program->es_pid, 0xff, ES_PID_QTY_IN_A_PROGRAM * 2);
	//printf("allocate program_t space ok ------4.1----\n");
	memset(program->ecm_pid, 0xff, ECM_PID_QTY_IN_A_PROGRAM * 2);
	//printf("allocate program_t space ok ------4.2----\n");
	memset(program->service_info.service_name, 0, MAX_PROGRAM_NAME_LEN);
	memset(program->service_info.provider_name, 0, MAX_PROGRAM_NAME_LEN);
	program->service_info.service_type = 0xff;
	program->EIT_s_flag = 0;
	program->EIT_pf_flag = 0;
	program->running_status = 0;
	program->free_CA_mode = 0;

	return program;
}

pmt_table_t * create_pmt_table(void)
{
	pmt_table_t *p_pmt;
	
	p_pmt = malloc(sizeof(pmt_table_t));
	if(p_pmt == NULL){
 		printf("allocate pmt_table_t space fail, exit\n");
		exit(1);
 	}

	p_pmt->isComplete = 0;
	p_pmt->packet_cc = 0xff;
	p_pmt->pmt_pid = 0x1fff;
	p_pmt->p_program = NULL;
	p_pmt->cur_program = NULL;
	p_pmt->next = NULL;

	return p_pmt;
}

void free_all_program(program_t *head_program)
{
	program_t *p_program, *next_program;

	p_program = head_program;

	while(p_program){
		next_program = p_program->next;
		free_all_section(p_program->pmt_section);
		free_all_eit_table(p_program->eit_tables);
		free(p_program);
		p_program = next_program;
	}
}

void free_all_pmt_table(pmt_table_t *head_pmt)
{
	pmt_table_t *p_pmt, *next_pmt;

	p_pmt = head_pmt;

	while(p_pmt){
		next_pmt = p_pmt->next;
		free_all_program(p_pmt->p_program);
		free(p_pmt);
		p_pmt = next_pmt;
	}
}


program_t *find_ts_in_program(pmt_table_t *head_pmt, u_short program_number)
{

	pmt_table_t *p_pmt;
	program_t *p_program;


	p_pmt = head_pmt;
	while(p_pmt){
		
		p_program = p_pmt->p_program;
		while(p_program){
			if(p_program->program_number == program_number){
				return p_program;
			}
			p_program = p_program->next;
		}
		
		p_pmt = p_pmt->next;
	}
	
	return NULL;
}


pmt_table_t *find_ts_pmt_table(ts_in_table_t *p_ts_table, u_short pmt_pid)
{

	pmt_table_t *p_pmt;

	if(p_ts_table == NULL){
		return NULL;
	}
	
	p_pmt = p_ts_table->pmt_tables;
	
	while(p_pmt){
		if(p_pmt->pmt_pid == pmt_pid){
			return p_pmt;
		}
		p_pmt = p_pmt->next;
	}

	return NULL;
}

void init_ts_in_table(ts_in_table_t *p_ts_table)
{
	if(p_ts_table == NULL){
		return;
	}

	p_ts_table->table_valid = INVALID;

	p_ts_table->capture_error_count = 0;
	
	p_ts_table->complete_mask = 0;
	p_ts_table->capture_mask = 0;
	
	p_ts_table->ts_id = 0xffff;
	p_ts_table->network_id = 0xffff;
	p_ts_table->original_network_id = 0xffff;


	/* PAT table */
	p_ts_table->pat_count = 0;
	p_ts_table->pat_cc = 0xff;
	p_ts_table->pat_section_num_mask = 0;
	p_ts_table->pat_section = NULL;
	p_ts_table->cur_pat_section = NULL;

	/*PMT Table */
	p_ts_table->program_count = 0;
	p_ts_table->pmt_tables = NULL;

	/* SDT Table */
	p_ts_table->sdt_count = 0;
	p_ts_table->sdt_cc = 0xff;
	p_ts_table->sdt_section_num_mask = 0;
	p_ts_table->sdt_section = NULL;
	p_ts_table->cur_sdt_section = NULL;

	/*CAT Table */
	p_ts_table->cat_count = 0;
	p_ts_table->cat_cc = 0xff;
	p_ts_table->cat_section_num_mask = 0;
	p_ts_table->cat_section = NULL;
	p_ts_table->cur_cat_section = NULL;

	/*NIT table */
	p_ts_table->nit_count = 0;
	p_ts_table->nit_cc = 0xff;
	p_ts_table->nit_section_num_mask = 0;
	p_ts_table->nit_section = NULL;
	p_ts_table->cur_nit_section = NULL;

	/*EIT table */
	p_ts_table->eit_cc = 0xff;
	
	p_ts_table->cur_eit_sub_table = NULL;

	//p_ts_table->eit_capture_cond = NULL;
	
	memset(p_ts_table->emm_pid, 0xff, EMM_PID_QTY_IN_A_TS * 2);

	/*input pid table*/
	//p_ts_table->pid_point = 0;
	//memset(p_ts_table->in_pids, 0xff, sizeof(p_ts_table->in_pids));

}


ts_in_table_t *create_ts_in_table(void)
{
	int i;
	ts_in_table_t *p_ts_table;

	p_ts_table = malloc(sizeof(ts_in_table_t));
	if(p_ts_table == NULL){
		printf(" malloc ts_tables  faild, exit \n");
		exit(1);
	}
	
	init_ts_in_table(p_ts_table);

	return p_ts_table;

}

void free_ts_table(ts_in_table_t *p_ts_table)
{
	if(p_ts_table == NULL){
		return;
	}

	free_all_pmt_table(p_ts_table->pmt_tables);
	free_all_section(p_ts_table->pat_section);
	free_all_section(p_ts_table->cat_section);
	free_all_section(p_ts_table->sdt_section);
	free_all_section(p_ts_table->nit_section);

	free(p_ts_table);
	
	printf("--free ts table finish!--\n");

}




void reset_ts_in_table(ts_in_table_t *p_ts_table, int reset_mask)
{
	int i;
	
	if(p_ts_table == NULL){
		return;
	}

	//printf("reset_ts_in_table : reset_mask = %#x \n", reset_mask);

	//p_ts_table->table_valid = INVALID;

	p_ts_table->capture_error_count = 0;
	
	p_ts_table->complete_mask = 0;
	p_ts_table->capture_mask = 0;
	

	

	if(reset_mask & PAT_MASK){
	//if(reset_mask){
		/* PAT table */
	
	//printf("reset_ts_in_table : pat  \n");

	
		p_ts_table->pat_cc = 0xff;
		p_ts_table->ts_id = 0xffff;
	
		free_all_section(p_ts_table->pat_section);
		p_ts_table->pat_count = 0;
		p_ts_table->pat_section_num_mask = 0;
		p_ts_table->pat_section = NULL;
		p_ts_table->cur_pat_section = NULL;
	}
	

	if(reset_mask & PMT_MASK){	
		/*PMT Table */
		//printf("reset_ts_in_table : pmt  \n");
		free_all_pmt_table(p_ts_table->pmt_tables);
		p_ts_table->program_count = 0;
		p_ts_table->pmt_tables = NULL;
	}

	
	if(reset_mask & SDT_MASK){
		/* SDT Table */
		//printf("reset_ts_in_table : sdt  \n");
		
		p_ts_table->sdt_cc = 0xff;
		p_ts_table->original_network_id = 0xffff;
		
		free_all_section(p_ts_table->sdt_section);
		p_ts_table->sdt_count = 0;
		p_ts_table->sdt_section_num_mask = 0;
		p_ts_table->sdt_section = NULL;
		p_ts_table->cur_sdt_section = NULL;
	}

	
	if(reset_mask & CAT_MASK){
		/*CAT Table */
		//printf("reset_ts_in_table : cat  \n");
		
		p_ts_table->cat_cc = 0xff;
		
		free_all_section(p_ts_table->cat_section);
		p_ts_table->cat_count = 0;
		p_ts_table->cat_section_num_mask = 0;
		p_ts_table->cat_section = NULL;
		p_ts_table->cur_cat_section = NULL;

		memset(p_ts_table->emm_pid, 0xff, EMM_PID_QTY_IN_A_TS * 2);
	}
	

	if(reset_mask & NIT_MASK){
		/*NIT table */
		//printf("reset_ts_in_table : nit  \n");
	
		p_ts_table->nit_cc = 0xff;
		p_ts_table->network_id = 0xffff;
		
		free_all_section(p_ts_table->nit_section);
		p_ts_table->nit_count = 0;
		p_ts_table->nit_section_num_mask = 0;
		p_ts_table->nit_section = NULL;
		p_ts_table->cur_nit_section = NULL;
	}

	
	

	if(reset_mask & EIT_MASK){
		/*EIT table */
		//printf("reset_ts_in_table : eit  \n");
		
		p_ts_table->eit_cc = 0xff;
		p_ts_table->cur_eit_sub_table = NULL;

		//free_all_eit_qualifier(p_ts_table->eit_capture_cond);
		//p_ts_table->eit_capture_cond = NULL;
	}
	

	/*input pid table*/
	//p_ts_table->pid_point = 0;
	//memset(p_ts_table->in_pids, 0xff, sizeof(p_ts_table->in_pids));

	//printf("--reset ts table finish!--\n");

}

void reset_pat_in_ts_table(ts_in_table_t *p_ts_table)
{
	free_all_section(p_ts_table->pat_section);
	p_ts_table->pat_count = 0;
	p_ts_table->pat_section_num_mask = 0;
	p_ts_table->pat_section = NULL;
}

void print_all_section(section_t *p_section)
{
	section_t *cur_section;
	cur_section = p_section;
	while(cur_section){
		printf("-------------print----section----: point addr [%p]-------------------------\n", cur_section);
		
		printf("section->section_number = 0x%02x\n", cur_section->section_number);
		printf("section->crc32 = 0x%08x\n", cur_section->crc32);
		
		print_bytes(cur_section->data, cur_section->len);
		cur_section = cur_section->next;
	}
}

void print_eit_table(eit_table_t *p_eit)
{
	eit_sub_table_t *p_sub_table;
	eit_segment_t *p_segment;
	printf("---------------print eit------------\n");

	while(p_eit){
		printf("********************************p_eit->service_id = %d**************\n", p_eit->service_id);
		printf("p_eit->sub_table_count = %d\n", p_eit->sub_table_count);
		printf("p_eit->last_table_id = %#x\n", p_eit->last_table_id);

		p_sub_table = p_eit->sub_tables;
		while(p_sub_table){
			printf("eit sub table : table_id = %#x\n", p_sub_table->table_id);
		
			printf("eit sub table : segment_count = %d\n", p_sub_table->segment_count);
			printf("eit sub table : last_section_number = %d\n", p_sub_table->last_section_number);
			printf("eit sub table : section_num_mask : \n");
			print_bytes(p_sub_table->section_num_mask, SECTION_NUM_MASK_SIZE*sizeof(u_char));

			/*
			p_segment = p_sub_table->segments;
			while(p_segment){
				printf("eit segment : segment_num = %d\n", p_segment->segment_num);
				printf("eit segment : section_count = %d\n", p_segment->section_count);
				printf("eit_segment->segment_last_section_number = %#x\n", p_segment->segment_last_section_number);
				print_all_section(p_segment->section);

				p_segment = p_segment->next;
			}
			//*/

			p_sub_table = p_sub_table->next;
		}
		p_eit = p_eit->next;
	}
}


void print_all_program(program_t *p_program)
{
	int i;
	program_t *cur_program;
	
	cur_program = p_program;
	while(cur_program){
		//if(cur_program->pmt_section){
			
			printf("---------------print program------------\n");
			printf("program->program_number = %d\n", cur_program->program_number);
			printf("service type : %#x, name : %s \n", cur_program->service_info.service_type, cur_program->service_info.service_name);
			print_bytes(cur_program->service_info.service_name, 48);
			print_bytes(cur_program->service_info.provider_name, 48);
			//print_all_section(cur_program->pmt_section);

			/*EIT table */
			print_eit_table(cur_program->eit_tables);
		//}
		cur_program = cur_program->next;
	}
}

void print_all_pmt_table(pmt_table_t *head_pmt_table)
{
	pmt_table_t *p_pmt;
	
	p_pmt = head_pmt_table;
	while(p_pmt){
		printf("---------------print in pmt table------------\n");
		printf("pmt ->pmt_pid = 0x%04x\n", p_pmt->pmt_pid);
		print_all_program(p_pmt->p_program);
		p_pmt = p_pmt->next;
	}
	
}



void print_ts_table(ts_in_table_t *p_ts_table)
{
	int i;
	
	if(p_ts_table == NULL || p_ts_table->table_valid == INVALID){
		return;
	}

	#if 1
	/* PAT table */
	printf("p_ts_table->pat_count = %d\n", p_ts_table->pat_count);
	printf("p_ts_table->pat_section_num_mask = 0x%08x\n", p_ts_table->pat_section_num_mask);
	//print_all_section(p_ts_table->pat_section);
	

	/*PMT Table */
	printf("p_ts_table->program_count = %d\n", p_ts_table->program_count);
	print_all_pmt_table(p_ts_table->pmt_tables);

	
	/* SDT Table */
	printf("p_ts_table->sdt_count = %d\n", p_ts_table->sdt_count);
	printf("p_ts_table->sdt_section_num_mask = 0x%08x\n", p_ts_table->sdt_section_num_mask);
	//print_all_section(p_ts_table->sdt_section);

	/*CAT Table */
	printf("p_ts_table->cat_count = %d\n", p_ts_table->cat_count);
	printf("p_ts_table->cat_section_num_mask = 0x%08x\n", p_ts_table->cat_section_num_mask);
	//print_all_section(p_ts_table->cat_section);


	/*NIT table */
	printf("p_ts_table->nit_count = %d\n", p_ts_table->nit_count);
	printf("p_ts_table->nit_section_num_mask = 0x%08x\n", p_ts_table->nit_section_num_mask);
	//print_all_section(p_ts_table->nit_section);

	#endif
	
	
	
}

//-------------------------------------------------------------



mux_packet_t *create_mux_packet()
{
	mux_packet_t *p_packet;
	p_packet = malloc(sizeof(mux_packet_t));
	if(p_packet == NULL){
		printf("malloc mux_packet faild, exit!\n");
		exit(1);
	}

	p_packet->len = 0;
	p_packet->index = 0;
	p_packet->next = NULL;
	memset(p_packet->buf, 0xff, PACKAGE_MAX_LEN);

	return p_packet;
}


void free_all_mux_packet(mux_packet_t *head_packet)
{
	mux_packet_t *p_packet, *next_packet;

	p_packet = head_packet;
	while(p_packet){
		
		next_packet = p_packet->next;

		free(p_packet);

		p_packet = next_packet;
	}
}


mux_section_t *create_mux_section()
{
	mux_section_t *p_section;
	p_section = malloc(sizeof(mux_section_t));
	if(p_section == NULL){
		printf("malloc mux_section faild, exit!\n");
		exit(1);
	}

	p_section->pid = 0x1fff;
	p_section->len = 0;
	p_section->next = NULL;
	memset(p_section->data, 0xff, MAX_SECTION_LEN);

	return p_section;
}

void free_all_mux_section(mux_section_t *head_section)
{
	mux_section_t *p_section, *next_section;

	p_section = head_section;
	while(p_section){
		
		next_section = p_section->next;

		free(p_section);

		p_section = next_section;
	}
}

mux_packet_t * packetData2packets(u_char *p_data, int data_len)
{
	int offset = 0;
	
	int p = 0;

	mux_packet_t *head_packet = NULL, *cur_packet;



	while(1){
		
		if(head_packet == NULL){
			cur_packet = create_mux_packet();
			head_packet = cur_packet;
		}else{
			cur_packet->next = create_mux_packet();
			cur_packet = cur_packet->next;
		}
				
		p = 0;
		while(p < PACKAGE_MAX_LEN){
			
			cur_packet->buf[p++] = p_data[offset++];
			
			if(offset >= data_len){

				/*
				for(; p < PACKAGE_MAX_LEN; p++){
					cur_packet->data[p] = 0xff;
				}
				//*/
				
				
				return head_packet;
			}
			
		}
		
		
		//print_bytes(cur_packet->buf,188);

	}


}



mux_packet_t *sectionData2packets(u_char *p_data, int data_len, u_short pid, int *cc_index)
{
	int offset = 0;
	int index = 0;
	int p = 0;

	mux_packet_t *head_packet = NULL, *cur_packet;


	index = *cc_index;


	while(1){
		
		if(head_packet == NULL){
			cur_packet = create_mux_packet();
			head_packet = cur_packet;
		}else{
			cur_packet->next = create_mux_packet();
			cur_packet = cur_packet->next;
		}

		cur_packet->index = index;
				
		cur_packet->buf[0] = 0x47;
		
		if(offset == 0){
			cur_packet->buf[1] = 0x40 | ((pid >> 8) & 0x1f);
			cur_packet->buf[4] = 0;
			p = 5;
		}else{
			cur_packet->buf[1] = 0x00 | ((pid >> 8) & 0x1f);
			p = 4;
		}
		
		cur_packet->buf[2] = pid & 0xff;
		cur_packet->buf[3] = 0x10 | ((index % 16) & 0xf);

		index++;
		
		//printf("----sectionData2packets-----, pid = %d,[%#.4x], index = %d \n", pid, pid, index);

		while(p < PACKAGE_MAX_LEN){
			
			cur_packet->buf[p++] = p_data[offset++];
			
			if(offset >= data_len){

				/*
				for(; p < PACKAGE_MAX_LEN; p++){
					cur_packet->data[p] = 0xff;
				}
				//*/
				*cc_index = ((index % 16) & 0xf);
				
				return head_packet;
			}
			
		}
		
		
		//print_bytes(cur_packet->buf,188);

	}

	return NULL;

}


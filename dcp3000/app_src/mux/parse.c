
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"
#include "psi_si.h"
#include "../basedef.h"
#include "../utils/utils.h"


#define PARSE_SI_INFO	1
#define PARSE_SI_DEBUG	0

int parse_tsin_pat(ts_in_table_t *p_ts_table)
{
	u_char *p_buf;
	u_short program_number, pmt_pid;
	int temp_len;
	program_t *p_program, *cur_program;
	pmt_table_t *p_pmt, *cur_pmt;
	section_t *p_section;
	
	if(p_ts_table == NULL){
		return EXEC_ERR;
	}

	p_section = p_ts_table->pat_section;
		
	printf("----start parse pat ------\n");
	while(p_section){

		p_buf = p_section->data;
		
		p_buf += 8;				/*point to first program number */
		temp_len = p_section->len - 9;	/*5: head 5 byte, 4:crc byte */

		#if PARSE_SI_DEBUG
		printf("----start parse pat ------- len=%d----------\n", temp_len);
		#endif
		while(temp_len >= 4){	
		
			program_number =  get_bits(p_buf, 0, 0, 16);
			pmt_pid = get_bits(p_buf, 2, 3, 13);
			
			if(program_number == 0){
				
			}else{
			
				p_pmt = find_ts_pmt_table(p_ts_table, pmt_pid);
				
				if(p_pmt == NULL){
					
					p_pmt = create_pmt_table();

					if(p_ts_table->pmt_tables){
						cur_pmt->next = p_pmt;
					}else{
						p_ts_table->pmt_tables = p_pmt;
					}
					
					cur_pmt = p_pmt;
				}
					
				p_program = create_program();

				p_program->program_number = program_number;
				
				p_pmt->pmt_pid = pmt_pid;	// | (INS_STREAM << 13);
				p_program->pmt_pid = p_pmt->pmt_pid;
				p_ts_table->program_count++;

				if(p_pmt->p_program){
					cur_program->next = p_program;
				}else{
					p_pmt->p_program = p_program;
				}
				
				cur_program = p_program;

				#if PARSE_SI_DEBUG
				printf("program number = %#x, pmt pid = 0x%x \n", p_program->program_number, p_pmt->pmt_pid);
				#endif

			}
			p_buf += 4;
			temp_len -= 4;
			
		}

		p_section = p_section->next;
	}

	return EXEC_OK;
	
}


//parse es pid only
int parse_tsin_pmt(ts_in_table_t *p_ts_table)
{

	u_char *buf;
	u_char stream_type;
	u_short stream_pid;
	int temp_len, info_len, streams_len;
	int i, count = 0 , ret;
	int es_count, ecm_count;
	descriptor_t desc;
	program_t *p_program;
	pmt_table_t *p_pmt;
	
	if(p_ts_table == NULL){
		return EXEC_ERR;
	}
	

	
	printf("----start parse  pmt ------\n");

	p_pmt = p_ts_table->pmt_tables;
	while(p_pmt){
		#if PARSE_SI_INFO
			printf("+++++++++parse pmt :  pmt pid : 0x%x-----\n", p_pmt->pmt_pid);
		#endif
			
		p_program = p_pmt->p_program;
		while(p_program){

			#if PARSE_SI_INFO
			printf("++++parse pmt : program number ..%#x.. +++++ ...........\n", p_program->program_number);
			#endif
			
			if((p_program->pmt_section == NULL) || (p_program->pmt_section->len <= 12))
			{
				printf("program number %d  :  PMT section is NULL or length is error!!!\n", p_program->program_number); 
				
				p_program = p_program->next;
				continue;

			}
		
			
			
			buf = p_program->pmt_section->data;

			p_program->pcr_pid = get_bits(buf, 8, 3, 13);

			info_len = get_bits (buf, 10, 4, 12);	//program_info_length
			streams_len = p_program->pmt_section->len - info_len - 12;	// total stream length
			buf += 12;				/* point to first pmt_descriptor */

			ecm_count = 0;
			while(info_len>0){
				desc.desc_tag = get_bits (buf, 0, 0, 8);
				desc.desc_len = get_bits (buf, 1, 0, 8);
				#if PARSE_SI_DEBUG
				printf("parse pmt ---desc_tag--%x----------\n", desc.desc_tag);
				printf("parse pmt ---desc_len--%d----------\n", desc.desc_len);
				#endif
				desc.desc_data = buf + 2;
				ret = parseDescriptor(&desc);
				if(ret == 3){
					p_program->free_CA_mode = 1;
					if(desc.desc_info.descCA.CA_system_ID == BISS_CA_SYSTEM_ID){
						p_ts_table->hasBissCA = 1;
					}else{
						p_ts_table->hasSimulCA = 1;
					}
					
					for(i=0; i<ECM_PID_QTY_IN_A_PROGRAM; i++){
						if((p_program->ecm_pid[i] & 0x1fff) == 0x1fff){
							p_program->ecm_pid[i] = desc.desc_info.descCA.CA_PID;
							#if PARSE_SI_DEBUG
							printf("parse pmt ---ecm_pid[%d]--%#x----------\n", i, desc.desc_info.descCA.CA_PID);
							#endif
							break;
						}else if((p_program->ecm_pid[i] & 0x1fff) == desc.desc_info.descCA.CA_PID){
							#if PARSE_SI_DEBUG
							printf("parse pmt ---ecm_pid[%d]--%#x----already exist------\n", i, desc.desc_info.descCA.CA_PID);
							#endif
							break;
						}
					}
				}
				
				buf += desc.desc_len + 2;
				info_len -= desc.desc_len + 2;
			}
			

			/* ponit to first ts desc */
			es_count = 0;
			while(streams_len > 4) { 	/*parse stream */
		
#if 1
				stream_type = get_bits (buf, 0,	0,	8);
				stream_pid = get_bits (buf, 1, 3, 13);

				#if PARSE_SI_DEBUG
				printf("parse pmt ---stream_type--%d----------\n", stream_type);
				printf("parse pmt ---stream_pid--%d----------\n", stream_pid);
				#endif

				//*
				if(es_count < ES_PID_QTY_IN_A_PROGRAM){
					p_program->es_pid[es_count++] = stream_pid | (stream_type << 16);
				}
				//*/
#endif

				temp_len = get_bits (buf, 3, 4, 12);	//es info length

				streams_len -= (temp_len + 5);		//rest length

				buf += 5;
				while(temp_len>0){
					desc.desc_tag = get_bits (buf, 0, 0, 8);
					desc.desc_len = get_bits (buf, 1, 0, 8);
					#if PARSE_SI_DEBUG
					printf("parse pmt ---desc_tag--%x----------\n", desc.desc_tag);
					printf("parse pmt ---desc_len--%d----------\n", desc.desc_len);
					#endif
					desc.desc_data = buf + 2;
					ret = parseDescriptor(&desc);
					if(ret == 3){

						p_program->free_CA_mode = 1;
						if(desc.desc_info.descCA.CA_system_ID == BISS_CA_SYSTEM_ID){
							p_ts_table->hasBissCA = 1;
						}else{
							p_ts_table->hasSimulCA = 1;
						}
						
						for(i=0; i<ECM_PID_QTY_IN_A_PROGRAM; i++){
							if((p_program->ecm_pid[i] & 0x1fff) == 0x1fff){
								p_program->ecm_pid[i] = desc.desc_info.descCA.CA_PID;
								#if PARSE_SI_DEBUG
								printf("parse pmt ---ecm_pid[%d]--%#x----------\n", i, desc.desc_info.descCA.CA_PID);
								#endif
								break;
							}else if((p_program->ecm_pid[i] & 0x1fff) == desc.desc_info.descCA.CA_PID){
								#if PARSE_SI_DEBUG
								printf("parse pmt ---ecm_pid[%d]--%#x----already exist------\n", i, desc.desc_info.descCA.CA_PID);
								#endif
								break;
							}
						}
						
					}
					
					buf += desc.desc_len + 2;
					temp_len -= desc.desc_len + 2;
				}
				
			}
			count++;
			
			//cur_mux_program = p_mux_program;
			//p_pmt_section = p_pmt_section->next;
			p_program = p_program->next;
		}
		#if PARSE_SI_INFO
		printf("parse tsin pmt --0x%x-- ok\n", p_pmt->pmt_pid);
		#endif
		p_pmt = p_pmt->next;
	}
	
	
	#if PARSE_SI_DEBUG
	//printf("mux pmt parse %d program\n", count);
	#endif
	return EXEC_OK;

}


//parse program name only
int parse_tsin_sdt(ts_in_table_t *p_ts_table)
{

	u_char *p_ucOri;
	u_char des_len, des_type;
	int	des_loop_len,temp_len, offset, name_len;
	int section_len = 0, ret;
	u_short service_id;
	program_t *p_program;
	section_t *sdt_section;
	descriptor_t desc;

	char program_name[100];

	if(p_ts_table == NULL){
		return EXEC_ERR;
	}
	
	printf("----start parse_sdt ------\n");
	
	sdt_section = p_ts_table->sdt_section;

	while(sdt_section){
	
		if((sdt_section->len <= 12))
		{
			printf("SDT section length is too short!!!\n"); 
			sdt_section = sdt_section->next;
			continue;
		}

		section_len = sdt_section->len;
		p_ucOri = sdt_section->data;

		#if PARSE_SI_DEBUG
		printf("--------------------- parse_sdt -section_len = %d-------------\n", section_len);
		//print_bytes(p_ucOri, section_len);
		#endif
					

		p_ucOri += 11;
		section_len -= 11;

		while(section_len>4){
			
			service_id = get_bits(p_ucOri, 0, 0, 16);
			#if PARSE_SI_DEBUG
			printf("--------------------- parse_sdt -service_id = %d-------------\n", service_id);
			#endif
			
			des_loop_len = get_bits(p_ucOri, 3, 4, 12);
			section_len -= (des_loop_len + 5);
			
			p_program = find_ts_in_program(p_ts_table->pmt_tables, service_id);
			
			if(p_program == NULL){
				p_ucOri += des_loop_len+5;
				continue;
			}

			p_program->EIT_s_flag = get_bits(p_ucOri, 2, 6, 1);
			p_program->EIT_pf_flag = get_bits(p_ucOri, 2, 7, 1);
			p_program->running_status = get_bits(p_ucOri, 3, 0, 3);
			//p_program->free_CA_mode = get_bits(p_ucOri, 3, 3, 1);

			#if PARSE_SI_DEBUG
			printf("parse sdt ---EIT_s_flag--%x----------\n", p_program->EIT_s_flag);
			printf("parse sdt ---EIT_pf_flag--%d----------\n", p_program->EIT_pf_flag);
			printf("parse sdt ---running_status--%x----------\n", p_program->running_status);
			printf("parse sdt ---free_CA_mode--%d----------\n", p_program->free_CA_mode);
			#endif



			p_ucOri += 5;

			while(des_loop_len>0){

				desc.desc_tag = get_bits(p_ucOri, 0, 0, 8);
				desc.desc_len = get_bits(p_ucOri, 1, 0, 8);
				#if PARSE_SI_DEBUG
				printf("parse sdt ---desc_tag--%x----------\n", desc.desc_tag);
				//printf("parse sdt ---desc_len--%d----------\n", desc.desc_len);
				#endif
				desc.desc_data = p_ucOri + 2;
				ret = parseDescriptor(&desc);
				if(ret == 1){
					p_program->service_info.service_type = desc.desc_info.descService.service_type;
					strcpy(p_program->service_info.service_name, desc.desc_info.descService.service_name);
					strcpy(p_program->service_info.provider_name, desc.desc_info.descService.service_provider_name);
					#if PARSE_SI_DEBUG
					printf("parse sdt ---service_type--%x----------\n", p_program->service_info.service_type);
					printf("parse sdt ---service_name--%s----------\n", p_program->service_info.service_name);
					#endif
				}
				
				p_ucOri += desc.desc_len + 2;
				des_loop_len -= desc.desc_len + 2;
			}

		}

		#if PARSE_SI_DEBUG
		//print_bytes(p_ucOri, section_len);
		#endif


		sdt_section = sdt_section->next;
	
	}

	return EXEC_OK;

}

int parse_tsin_cat(ts_in_table_t *p_ts_table)
{
	u_char *p_buf;
	u_short program_number;
	int temp_len, i;
	program_t *p_program;
	section_t *p_section;
	descriptor_t desc;
	
	if(p_ts_table == NULL){
		return EXEC_ERR;
	}

	p_section = p_ts_table->cat_section;
		
	printf("----start parse cat ------\n");
	while(p_section){

		p_buf = p_section->data;
		
		p_buf += 8;				/*point to ca descriptor list */
		temp_len = p_section->len - 12;	/*5: head 8 byte, 4:crc byte */

		#if PARSE_SI_DEBUG
		printf("----start parse cat -------ca descriptor list len---%d----------\n", temp_len);
		#endif
		while(temp_len >= 6){	
		
			desc.desc_tag = get_bits (p_buf, 0, 0, 8);
			desc.desc_len = get_bits (p_buf, 1, 0, 8);
			#if PARSE_SI_DEBUG
			printf("parse cat ---desc_tag--%x----------\n", desc.desc_tag);
			printf("parse cat ---desc_len--%d----------\n", desc.desc_len);
			#endif
			desc.desc_data = p_buf + 2;

			if(parseDescriptor(&desc) == 3){
				for(i=0; i<EMM_PID_QTY_IN_A_TS; i++){
					if((p_ts_table->emm_pid[i] & 0x1fff) == 0x1fff){
						p_ts_table->emm_pid[i] = desc.desc_info.descCA.CA_PID;
						//printf("parse cat ---emm_pid[%d]--%#x----------\n", i, desc.desc_info.descCA.CA_PID);
						break;
					}else if((p_ts_table->emm_pid[i] & 0x1fff) == desc.desc_info.descCA.CA_PID){
						//printf("parse cat ---emm_pid[%d]--%#x----already exist------\n", i, desc.desc_info.descCA.CA_PID);
						break;
					}
				}
			}
			
			p_buf += desc.desc_len + 2;
			temp_len -= desc.desc_len + 2;
			
			
		}

		p_section = p_section->next;
	}

	return EXEC_OK;
}

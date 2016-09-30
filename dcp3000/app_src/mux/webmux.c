

#include "../appref.h"

#include "mux.h"
#include "webmux.h"

void print_all_mux_program(int mux_chan)
{
	int i;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;


	p_mux_channel = &mux_channels[mux_chan];

	//p_mux_program = p_mux_channel->mux_programs;
	p_mux_program = p_mux_channel->mux_programs_raw;


	
	printf("print_all_mux_program :  %d channel mux, program count = %d\n"
		, mux_chan, p_mux_channel->program_count);
	

	if((p_mux_program == NULL) || (p_mux_channel->program_count<=0)){
		return ;
	}

	while(p_mux_program){

		printf(" ---in_chan--%d----------\n", p_mux_program->in_chan);
				
		printf(" ---old_pn--%d----------\n", p_mux_program->old_pn);
		printf(" ---new_pn--%d----------\n", p_mux_program->new_pn);
		
		printf(" ---in_pcr_pid--%#x----------\n", p_mux_program->in_pcr_pid);
		printf(" ---mux_pcr_pid--%#x----------\n", p_mux_program->mux_pcr_pid);
		
		printf(" ---in_pmt_pid--%#x----------\n", p_mux_program->in_pmt_pid);
		printf(" ---mux_pmt_pid--%#x----------\n", p_mux_program->mux_pmt_pid);

		print_bytes((char *)p_mux_program->es_pid_mapping, ES_PID_QTY_IN_A_PROGRAM*2*3);

		printf(" ---char_type--%#x----------\n", p_mux_program->char_type);
		printf(" ---service_type--%#x----------\n", p_mux_program->service_info.service_type);
		printf(" ---service_name--%s----------\n", p_mux_program->service_info.service_name);
		printf(" ---provider_name--%s----------\n", p_mux_program->service_info.provider_name);

		printf(" ---cas_mode--%#x----------\n", p_mux_program->cas_mode);
		printf(" ---free_CA_mode--%#x----------\n", p_mux_program->free_CA_mode);

		
		//p_mux_program = p_mux_program->next;
		p_mux_program = p_mux_program->link;
	}

	printf("print_all_mux_program , ---------end=============\n");
}

mux_program_t *find_mux_program_by_oldpn(int mux_chan, int in_chan, u_short old_pn)
{
	int i;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	
	//printf("find_mux_program_by_oldpn :  %d channel mux, in_chan %d , new_pn %d \n", mux_chan, in_chan, old_pn);

	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;

	if((p_mux_program == NULL) || (p_mux_channel->program_count<=0)){
		return NULL;
	}

	while(p_mux_program){
		if((p_mux_program->in_chan == in_chan) && (p_mux_program->old_pn == old_pn)){
			return p_mux_program;
		}
		p_mux_program = p_mux_program->next;
	}

	return NULL;
}


mux_program_t *find_mux_program_by_newpn(int mux_chan, u_short new_pn)
{
	int i;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	
	//printf("find_mux_program_by_newpn :  %d channel mux , new_pn %d \n", mux_chan, new_pn);

	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;

	if((p_mux_program == NULL) || (p_mux_channel->program_count<=0)){
		return NULL;
	}

	while(p_mux_program){
		if(p_mux_program->new_pn == new_pn){
			return p_mux_program;
		}
		p_mux_program = p_mux_program->next;
	}

	return NULL;
}

mux_program_t *find_mux_program_by_pid(int mux_chan, u_short pid)
{
	int i;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	//printf("find_mux_program_by_pid :  %d channel mux , pmt_pid %#x \n", mux_chan, pid);

	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;

	if((p_mux_program == NULL) || (p_mux_channel->program_count<=0)){
		return NULL;
	}

	while(p_mux_program){
		if(p_mux_program->mux_pmt_pid == pid){
			return p_mux_program;
		}
		p_mux_program = p_mux_program->next;
	}

	return NULL;
}

int check_newpn_in_mux_program(int mux_chan, u_short new_pn)
{
	int i;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	//printf("check_newpn_in_mux_program :  %d channel mux , new_pn %d \n", mux_chan, new_pn);
	

	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;

	if((p_mux_program == NULL) || (p_mux_channel->program_count<=0)){
		return 0;
	}

	while(p_mux_program){
		
		if((p_mux_program->new_pn == new_pn)){
			return 1;
		}
		
		p_mux_program = p_mux_program->next;
	}

	return 0;
}

#if 0
int add_program_mapping(int mux_chan, int in_chan, u_short old_pn, u_short new_pn)
{
	int endflag = 0;
	
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	
	mux_program_t *prev_node = NULL, *goal_node = NULL;
	mux_program_t *anchor_node = NULL, *junction_node = NULL;

	ts_in_table_t *p_ts_table;
	program_t *p_program;

	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;


	/*
	printf("update_program_mapping :  %d channel mux , in_chan %d , old_pn %d, new_pn %d\n"
		, mux_chan, in_chan, old_pn, new_pn);
	//*/


	while(p_mux_program){
		
		if((p_mux_program->in_chan == in_chan) && (p_mux_program->old_pn == old_pn)){

			//the node that preparing to add and this node is same

			return 1;
			
		}else{

			//the node that preparing to add and this node is not same
			if((p_mux_program->new_pn == new_pn)){
				return -1;
				
			}else if(p_mux_program->new_pn > new_pn){
				anchor_node = prev_node;
				junction_node = p_mux_program;

				break;
				
			}else{
				
			}
						
		}

		prev_node = p_mux_program;
		p_mux_program = p_mux_program->next;
	}

	//if the goal node is non-existent then create it
 	if(goal_node == NULL){
		goal_node = create_mux_program();
		goal_node->in_chan = in_chan;
		goal_node->old_pn = old_pn;
		
		p_mux_channel->program_count++;
	}

	//insert the goal node to the insertion point
	if(anchor_node){
		anchor_node->next = goal_node;
	}else{
		if(prev_node){
			prev_node->next = goal_node;
		}else{
			p_mux_channel->mux_programs = goal_node;
		}
	}

	goal_node->next = junction_node;

	goal_node->new_pn = new_pn;

	if(p_mux_channel->mux_programs_raw == NULL){
		p_mux_channel->mux_programs_raw = goal_node;
		p_mux_channel->mux_programs_tail = goal_node;
	}else{
		p_mux_channel->mux_programs_tail->link = goal_node;
		p_mux_channel->mux_programs_tail = goal_node;
	}


	return goal_node;
}
#endif

#if 1

mux_program_t *update_program_mapping(int mux_chan, int in_chan, u_short old_pn, u_short new_pn)
{
	int endflag = 0;
	
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	
	mux_program_t *prev_node = NULL, *goal_node = NULL;
	mux_program_t *anchor_node = NULL, *junction_node = NULL;

	ts_in_table_t *p_ts_table;
	program_t *p_program;

	if((in_chan>=mux_info.total_tsin_channels) || (mux_chan>=mux_info.mux_channels)){
			return NULL;
	}

	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;


	//*
	printf("update_program_mapping :  %d channel mux , in_chan %d , old_pn %d, new_pn %d\n"
		, mux_chan, in_chan, old_pn, new_pn);
	//*/


	while(p_mux_program){

		printf("update_program_mapping : ---- in_chan %d , old_pn %d, new_pn %d------\n"
		, p_mux_program->in_chan, p_mux_program->old_pn, p_mux_program->new_pn);
		
		if((p_mux_program->in_chan == in_chan) && (p_mux_program->old_pn == old_pn)){

			//the node that preparing to add and this node is same
			//printf("update_program_mapping : ---- -----------1----------\n");

			return p_mux_program;
			
		}else{

			//the node that preparing to add and this node is not same
			//printf("update_program_mapping : ---- -----------2----------\n");
			if((p_mux_program->new_pn == new_pn)){
				//printf("update_program_mapping : ---- -----------2.1----------\n");
				new_pn++;
				
			}else if(p_mux_program->new_pn > new_pn){
			//printf("update_program_mapping : ---- -----------2.2----------\n");
				anchor_node = prev_node;
				junction_node = p_mux_program;

				break;
				
			}else{
				//printf("update_program_mapping : ---- -----------2.3----------\n");
			}
						
		}

		prev_node = p_mux_program;
		p_mux_program = p_mux_program->next;
	}

	//printf("update_program_mapping : ---- -----------3----------\n");
	//if the goal node is non-existent then create it
 	if(goal_node == NULL){
		//printf("update_program_mapping : ---- -----------4----------\n");
		goal_node = create_mux_program();
		goal_node->in_chan = in_chan;
		goal_node->old_pn = old_pn;
		
		p_mux_channel->program_count++;
		//printf("update_program_mapping : ---- ---program_count %d-----\n", p_mux_channel->program_count);
	}
	//printf("update_program_mapping : ---- -----------5----------\n");

	//insert the goal node to the insertion point
	if(anchor_node){
		//printf("update_program_mapping : ---- -----------6.1----------\n");
		anchor_node->next = goal_node;
	}else{
	//printf("update_program_mapping : ---- -----------6.2----------\n");
		if(prev_node){
			//printf("update_program_mapping : ---- -----------6.2.1----------\n");
			prev_node->next = goal_node;
		}else{
		//printf("update_program_mapping : ---- -----------6.2.2----------\n");
			p_mux_channel->mux_programs = goal_node;
		}
	}

	goal_node->next = junction_node;

	goal_node->new_pn = new_pn;

	if(p_mux_channel->mux_programs_raw == NULL){
		p_mux_channel->mux_programs_raw = goal_node;
		p_mux_channel->mux_programs_tail = goal_node;
	}else{
		p_mux_channel->mux_programs_tail->link = goal_node;
		p_mux_channel->mux_programs_tail = goal_node;
	}

	add_tsin_within_mux(mux_chan, in_chan);

	#if 0
	if((goal_node->mux_pmt_pid & 0x1fff) == 0x1fff){
		p_ts_table = &ts_in_channels[in_chan].ts_tables;
		p_program = find_ts_in_program(p_ts_table->pmt_section, goal_node->old_pn);
		if(p_program){
            
			goal_node->in_pcr_pid = p_program->pcr_pid | (INS_STREAM << 13);
			goal_node->mux_pcr_pid = p_program->pcr_pid;

			/*
			goal_node->in_pmt_pid = p_program->pmt_pid | (INS_STREAM << 13);
			goal_node->mux_pmt_pid = p_program->pmt_pid;
			memcpy(&goal_node->service_info, &p_program->service_info, sizeof(service_info_t));
			//*/

			//printf("update_program_mapping : ---- in_pmt_pid %#x, mux_pmt_pid %#x, service name %s----------\n"
			//	,goal_node->in_pmt_pid, goal_node->pmt_pid, goal_node->service_info.service_name);

		}
	}
	#endif

	return goal_node;
}


#else
mux_program_t *update_program_mapping(int mux_chan, int in_chan, u_short old_pn, u_short new_pn)
{
	int endflag = 0;
	
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	
	mux_program_t *prev_node = NULL, *goal_node = NULL;
	mux_program_t *anchor_node = NULL, *junction_node = NULL;

	ts_in_table_t *p_ts_table;
	program_t *p_program;

	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;


	printf("update_program_mapping :  %d channel mux , in_chan %d , old_pn %d, new_pn %d\n"
		, mux_chan, in_chan, old_pn, new_pn);


	while(p_mux_program){

		//printf("update_program_mapping : ---- in_chan %d , old_pn %d, new_pn %d------\n"
		//, p_mux_program->in_chan, p_mux_program->old_pn, p_mux_program->new_pn);
		
		if((p_mux_program->in_chan == in_chan) && (p_mux_program->old_pn == old_pn)){

			//the node that preparing to add and this node is same
			//printf("update_program_mapping : ---- -----------1----------\n");

			if((p_mux_program->new_pn == new_pn)){
				//printf("update_program_mapping : ---- -----------1.1----------\n");
				//the node that preparing to add already exist, and won't do any thing.
				return p_mux_program;
				
			}else{
			
				//printf("update_program_mapping : ---- -----------1.2----------\n");

				//take this node out from the link table.
			
				goal_node = p_mux_program;

				if(prev_node){
					//printf("update_program_mapping : ---- -----------1.2.1----------\n");
					prev_node->next = p_mux_program->next;
				}else{
				//printf("update_program_mapping : ---- -----------1.2.2----------\n");
					p_mux_channel->mux_programs = p_mux_program->next;
				}
				
				
				p_mux_program = p_mux_program->next;

				if(endflag){
					//printf("update_program_mapping : ---- -----------1.2.3----------\n");
					//the new insertion point already found, and end the loop
					break;
				}else{
				//printf("update_program_mapping : ---- -----------1.2.4----------\n");
					//look for the new insertion point
					endflag = 1;
					continue;
				}
				
			}
			
		}else{

			//the node that preparing to add and this node is not same
			//printf("update_program_mapping : ---- -----------2----------\n");
			if((p_mux_program->new_pn == new_pn)){
				//printf("update_program_mapping : ---- -----------2.1----------\n");
				new_pn++;
				
			}else if(p_mux_program->new_pn > new_pn){
			//printf("update_program_mapping : ---- -----------2.2----------\n");
				anchor_node = prev_node;
				junction_node = p_mux_program;

				if(endflag){
					//printf("update_program_mapping : ---- -----------2.2.1----------\n");
					//the node need to be replaced already found, and end the loop
					break;
				}else{
				//printf("update_program_mapping : ---- -----------2.2.2----------\n");
					//look for the node need to be replaced
					endflag = 1;
					p_mux_program = p_mux_program->next;
					continue;
				}
				
			}else{
				//printf("update_program_mapping : ---- -----------2.3----------\n");
			}
						
		}

		prev_node = p_mux_program;
		p_mux_program = p_mux_program->next;
	}

	//printf("update_program_mapping : ---- -----------3----------\n");
	//if the goal node is non-existent then create it
 	if(goal_node == NULL){
		//printf("update_program_mapping : ---- -----------4----------\n");
		goal_node = create_mux_program();
		goal_node->in_chan = in_chan;
		goal_node->old_pn = old_pn;
		
		p_mux_channel->program_count++;
		//printf("update_program_mapping : ---- ---program_count %d-----\n", p_mux_channel->program_count);
	}
	//printf("update_program_mapping : ---- -----------5----------\n");

	//insert the goal node to the insertion point
	if(anchor_node){
		//printf("update_program_mapping : ---- -----------6.1----------\n");
		anchor_node->next = goal_node;
	}else{
	//printf("update_program_mapping : ---- -----------6.2----------\n");
		if(prev_node){
			//printf("update_program_mapping : ---- -----------6.2.1----------\n");
			prev_node->next = goal_node;
		}else{
		//printf("update_program_mapping : ---- -----------6.2.2----------\n");
			p_mux_channel->mux_programs = goal_node;
		}
	}

	goal_node->next = junction_node;

	goal_node->new_pn = new_pn;
	//printf("update_program_mapping : ---- -----------7----------\n");

	if((goal_node->pmt_pid & 0x1fff) == 0x1fff){
		p_ts_table = &ts_in_channels[in_chan].ts_tables;
		//printf("update_program_mapping : ---- -----------8----------\n");
		p_program = find_ts_in_program(p_ts_table->pmt_section, goal_node->old_pn);
		if(p_program){
			//printf("update_program_mapping : ---- -----------8.1----------\n");
			goal_node->in_pmt_pid = p_program->pmt_pid | (INS_STREAM << 13);
			goal_node->pmt_pid = p_program->pmt_pid;
			memcpy(&goal_node->service_info, &p_program->service_info, sizeof(service_info_t));

			//printf("update_program_mapping : ---- in_pmt_pid %#x, mux_pmt_pid %#x, service name %s----------\n"
			//	,goal_node->in_pmt_pid, goal_node->pmt_pid, goal_node->service_info.service_name);

		}
	}

	return goal_node;
}
#endif


u_short find_muxpid_in_mapping(int mux_chan, int in_chan, u_short in_pid)
{
	mux_channel_t *p_mux_channel;
	pid_mapping_t *p_pid_mapping;
	
	p_mux_channel = &mux_channels[mux_chan];

	p_pid_mapping = p_mux_channel->pid_mapping_list;
	
	while(p_pid_mapping){
		
		if((p_pid_mapping->in_chan == in_chan) && ((p_pid_mapping->in_pid & 0x1fff) == (in_pid & 0x1fff))){
			return p_pid_mapping->mux_pid;
		}

		p_pid_mapping = p_pid_mapping->next;
	}

	return 0x1fff;
}

int check_muxpid_in_mapping(int mux_chan, u_short mux_pid)
{
	mux_channel_t *p_mux_channel;
	pid_mapping_t *p_pid_mapping;
	
	p_mux_channel = &mux_channels[mux_chan];

	p_pid_mapping = p_mux_channel->pid_mapping_list;

	mux_pid &= 0x1fff;

	//printf("check_muxpid_in_mapping :  %d channel mux , mux_pid %#x\n", mux_chan, mux_pid);

	while(p_pid_mapping){
		
		if((p_pid_mapping->mux_pid == mux_pid)){
			return 1;
		}

		p_pid_mapping = p_pid_mapping->next;
	}

	return 0;
}


/*
pmt的pid可以忽略 in_chan和in_pid,
//*/
int update_pid_mapping(int mux_chan, int in_chan, u_short in_pid, u_short mux_pid)
{
	int endflag = 0;
	u_char pid_type;
	
	mux_channel_t *p_mux_channel;
	pid_mapping_t *p_pid_mapping;
	
	pid_mapping_t *prev_node = NULL, *goal_node = NULL;
	pid_mapping_t *anchor_node = NULL, *junction_node = NULL;

	pid_type = (in_pid >> 13) & 0x7;
	mux_pid &= 0x1fff;


	p_mux_channel = &mux_channels[mux_chan];

	#if REGEN_SI_INFO
	//*
	printf("update_pid_mapping :  %d channel mux , in_chan %d , in_pid %#x, mux_pid %#x\n"
		, mux_chan, in_chan, in_pid, mux_pid);
	//*/
	#endif

	Check_And_Traverse_Pid_Start:
		
	prev_node = NULL;
	goal_node = NULL;
	anchor_node = NULL;
	junction_node = NULL;

	p_pid_mapping = p_mux_channel->pid_mapping_list;
 

	while(p_pid_mapping){

		//printf("update_pid_mapping : ---- in_chan %d , in_pid %#x, mux_pid %#x------\n"
		//, p_pid_mapping->in_chan, p_pid_mapping->in_pid, p_pid_mapping->mux_pid);

	
		/*
		if(((p_pid_mapping->in_pid >> 13) == PMT_STREAM) && (pid_type == PMT_STREAM)){
			//permit the pmtpid that same in_chan and same in_pid modity diffrent pit;
			//printf("update_pid_mapping : ---- -----------1.1----------\n");
		}else 
		//*/
		
		if((p_pid_mapping->in_chan == in_chan) && ((p_pid_mapping->in_pid & 0x1fff) == (in_pid & 0x1fff))){
			//the node that preparing to add and this node is same
		//printf("update_pid_mapping : ---- -----------1.2----------\n");
			return p_pid_mapping->mux_pid;
		}

		//else{

			

			//the node that preparing to add and this node is not same
			//printf("update_pid_mapping : ---- -----------2----------\n");
			if((p_pid_mapping->mux_pid & 0x1fff) == mux_pid){
				//printf("update_pid_mapping : ---- -----------2.1----------\n");

				if((((p_pid_mapping->in_pid >> 13) & 0x7) == PMT_PACKET) && (pid_type == PMT_PACKET)){
					printf("update_pid_mapping : ---- find the same mux_pid[%#x] pmt ---------\n", p_pid_mapping->mux_pid);
					return p_pid_mapping->mux_pid;
				}
				
				mux_pid++;

				if(0x1fff <= mux_pid){
					printf("update_pid_mapping : ---- ---0x1fff <= mux_pid---------\n");

					mux_pid = 0x1000;
				
					goto Check_And_Traverse_Pid_Start;

				}
				
			}else if((p_pid_mapping->mux_pid & 0x1fff) > mux_pid){
			//printf("update_pid_mapping : ---- -----------2.2----------\n");
				anchor_node = prev_node;
				junction_node = p_pid_mapping;
				
				break;
				
			}else{
				//printf("update_pid_mapping : ---- -----------2.3----------\n");
			}
				
		//}

		prev_node = p_pid_mapping;
		p_pid_mapping = p_pid_mapping->next;
	}

	//printf("update_pid_mapping : ---- -----------3----------\n");
	//if the goal node is non-existent then create it
	if(goal_node == NULL){
		//printf("update_pid_mapping : ---- -----------4----------\n");
		goal_node = create_pid_mapping();
		goal_node->in_chan = in_chan;
		goal_node->in_pid = in_pid;
	}
	//printf("update_pid_mapping : ---- -----------5----------\n");

	//insert the goal node to the insertion point
	if(anchor_node){
		//printf("update_pid_mapping : ---- -----------6.1----------\n");
		anchor_node->next = goal_node;
	}else{
	//printf("update_pid_mapping : ---- -----------6.2----------\n");
		if(prev_node){
			//printf("update_pid_mapping : ---- -----------6.2.1----------\n");
			prev_node->next = goal_node;
		}else{
		//printf("update_pid_mapping : ---- -----------6.2.2----------\n");
			p_mux_channel->pid_mapping_list = goal_node;
		}
	}

	goal_node->next = junction_node;

	goal_node->mux_pid = mux_pid;

	#if REGEN_SI_INFO
	printf("update_pid_mapping : --finished--  mux_pid = %#x------\n", mux_pid);
	#endif


	return mux_pid;
}

int check_and_update_mux_pmtpid(int mux_chan, u_short pmt_pid)
{
	check_muxpid_in_mapping(mux_chan, pmt_pid);
	return 0;
}

u_short *find_muxpid_in_mux_program(mux_program_t *p_mux_program, u_short in_pid)
{
	int i;
	
	if(p_mux_program == NULL){
		return NULL;
	}

	in_pid &= 0x1fff;

	for(i=0; i<ES_PID_QTY_IN_A_PROGRAM; i++){
		if((p_mux_program->es_pid_mapping[i][0] & 0x1fff) == in_pid){
			return &p_mux_program->es_pid_mapping[i][1];
		}
	}

	return NULL;
}


#define IN_CHANNEL_MASK_SIZE	32


mux_section_t *regen_pat_section(int mux_chan)
{
	int i, len = 0;
	char *p_data;
	u_int crc32;
	u_short pmt_pid;
	
	mux_section_t *p_mux_section;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	
	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;

	if((p_mux_program == NULL) || (p_mux_channel->program_count<=0)){
		return NULL;
	}

	#if REGEN_SI_INFO
	printf("regen_pat_section : %d mux_chan -------start----------\n", mux_chan);
	#endif

	p_mux_section = create_mux_section();

	p_data = p_mux_section->data;
	len = 0;

	p_data[len++] = PAT_TABLEID;

	len += 2;
	
	p_data[len++] = p_mux_channel->ts_id >> 8;
	p_data[len++] = p_mux_channel->ts_id & 0xff;

	p_data[len++] = 0xc0 | (0x1<<1) |0x1;	//set version number
	p_data[len++] = 0x00;	//set section number
	p_data[len++] = 0x00;	//set last section number

	p_data[len++] = 0x00;	//set nit program number high 8 bit
	p_data[len++] = 0x00;	//set nit program number low 8 bit
	p_data[len++] = 0xe0;	//set nit pid high 5 bit
	p_data[len++] = NIT_PID;	//set nit pid low 8 bit

	
	while(p_mux_program){

		p_mux_program->free_CA_mode = 0;
		/*
		if(the_first_run_flag[mux_chan]){
			ts_in_channels[p_mux_program->in_chan].ts_flag &= ~TSIN_PSI_CHANGE;
		}
		//*/

		#if REGEN_SI_DEBUG
		//printf("regen_pat_section ---pmt pid--%#x----------\n", p_mux_program->in_pmt_pid);
		#endif

		if(p_mux_channel->pid_set_auto){
			pmt_pid = p_mux_program->in_pmt_pid | (PMT_PACKET << 13);
			pmt_pid = update_pid_mapping(mux_chan, p_mux_program->in_chan, pmt_pid, pmt_pid);
			p_mux_program->mux_pmt_pid = pmt_pid;
		}else{
			pmt_pid = p_mux_program->mux_pmt_pid;
		}
		

		#if REGEN_SI_DEBUG | 1
		printf("regen_pat_section : %d mux_chan -------new pn %d, pmt pid %#x----------\n"
			, mux_chan, p_mux_program->new_pn, p_mux_program->mux_pmt_pid);
		#endif
		
		p_data[len++] = p_mux_program->new_pn >> 8;	//set program number high 8 bit
		p_data[len++] = p_mux_program->new_pn & 0xff;	//set program number low 8 bit
		p_data[len++] = 0xe0 | (pmt_pid >> 8);	//set pmt pid high 5 bit
		p_data[len++] = pmt_pid & 0xff;	//set pmt pid low 8 bit

		p_mux_program = p_mux_program->next;
	}


	len += 4 - 3;
	p_data[1] = 0xb0 | (len >> 8);	//set section length high 4 bit
	p_data[2] = len & 0xff;	//set section length low 8 bit

	len -= 4 - 3;
	crc32 = calc_crc32(p_data, len);

	p_data[len++] = (crc32 >> 24) & 0xff;
	p_data[len++] = (crc32 >> 16) & 0xff;
	p_data[len++] = (crc32 >> 8) & 0xff;
	p_data[len++] = (crc32 >> 0) & 0xff;


	p_mux_section->len = len;
	p_mux_section->pid = PAT_PID;


	#if REGEN_SI_DEBUG
	//print_bytes(p_data, p_mux_section->len);
	#endif

	add_section_to_mux(mux_chan, p_mux_section);
	
	return p_mux_section;
}

mux_section_t *regen_pmt_section(int mux_chan)
{
	u_char *p_ucOri, *p_ucFinal;
	
	int info_len, streams_len, desc_len,temp_len, offset;
	int i, len = 0;//, result;
	
	int in_chan;
	u_short pcr_pid, es_pid, temp_pid, *p_pid;
	u_char section_version, es_type;
	u_int crc32;
	
	descriptor_t desc;
	
	mux_section_t *p_pmt_section, *head_pmt_section = NULL;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	program_t *p_program;

	
	p_mux_channel = &mux_channels[mux_chan];

	p_mux_program = p_mux_channel->mux_programs;

	if((p_mux_program == NULL) || (p_mux_channel->program_count<=0)){
		return NULL;
	}

	#if REGEN_SI_INFO
	printf("regen_pmt_section : %d mux_chan -------start----------\n", mux_chan);
	#endif
	
	while(p_mux_program){
		

		in_chan = p_mux_program->in_chan;	
		p_program = find_ts_in_program(ts_in_channels[in_chan].ts_tables.pmt_tables, p_mux_program->old_pn);

		/*
		if(p_program == NULL || p_program->pmt_section == NULL){
			p_mux_program = p_mux_program->next;
			continue;
		}
		//*/

		
		p_pmt_section = create_mux_section();

		p_mux_program->p_section = p_pmt_section;
		
		//p_pmt_section->next = head_pmt_section;
		//head_pmt_section = p_pmt_section;
		add_section_to_mux(mux_chan, p_pmt_section);
		
		p_ucFinal = p_pmt_section->data;
		len = 0;

		p_ucFinal[len++] = PMT_TABLEID;

		len += 2;
		
		p_ucFinal[len++] = p_mux_program->new_pn >> 8;
		p_ucFinal[len++] = p_mux_program->new_pn & 0xff;

		#if 0
		section_version = get_bits(p_ucOri, 5, 2, 5);
		#else
		section_version = 0;//p_program->pmt_version_number;
		#endif
		
		p_ucFinal[len++] = 0xc0 | (((section_version+1)%0x1f)<<1) |0x1;	//set version number
		p_ucFinal[len++] = 0x00;	//set section number
		p_ucFinal[len++] = 0x00;	//set last section number

		#if 0
		pcr_pid = get_bits(p_ucOri, 8, 3, 13) | (PCR_STREAM << 13);
		#else
		pcr_pid = p_mux_program->in_pcr_pid;// | (PCR_STREAM << 13);
		#endif
		
		if(p_mux_channel->pid_set_auto){
			temp_pid = update_pid_mapping(mux_chan, in_chan, pcr_pid, pcr_pid);
		}else{
			temp_pid = p_mux_program->mux_pcr_pid;
		}
		
		#if REGEN_SI_DEBUG
		printf("regen_pmt_section ---pcr pid--%#x----------\n", temp_pid);
		#endif


		p_ucFinal[len++] = 0xe0 | (temp_pid >> 8);	//set pcr pid high 5 bit
		p_ucFinal[len++] = temp_pid & 0xff;	//set pcr pid low 8 bit


		if(p_program && p_program->pmt_section){
		
			p_ucOri = p_program->pmt_section->data;

			memcpy(p_ucFinal + len, p_ucOri + 10, 2);
			len += 2;

			info_len = get_bits (p_ucOri, 10, 4, 12);	//program_info_length
			streams_len = p_program->pmt_section->len - info_len - 12;	// total stream length
			p_ucOri += 12;				/* point to first pmt_descriptor */

			temp_len = info_len;
			while(temp_len>0){
				
				
				desc.desc_tag = get_bits (p_ucOri, 0, 0, 8);
				desc.desc_len = get_bits (p_ucOri, 1, 0, 8);
				#if REGEN_SI_DEBUG
				printf("regen_pmt_section ---desc_tag--%#x----------\n", desc.desc_tag);
				printf("regen_pmt_section ---desc_len--%d----------\n", desc.desc_len);
				#endif

				
				desc.desc_data = p_ucOri + 2;

				if(parseDescriptor(&desc) == 3){
					if(p_mux_channel->remove_ca_flag == 0){
						memcpy(p_ucFinal + len, p_ucOri, desc.desc_len + 2);
						p_mux_channel->free_CA_mode = 1;
						p_mux_program->free_CA_mode = 1;
						temp_pid = desc.desc_info.descCA.CA_PID;
						#if REGEN_SI_DEBUG
						printf("regen_pmt_section ---ecm pid--%#x----------\n", temp_pid);
						#endif
						temp_pid = update_pid_mapping(mux_chan, in_chan, temp_pid, temp_pid);
						set_bits(p_ucFinal + len, 4, 3, 13, temp_pid);	//set ecm pid 
					}
				}else{				
					memcpy(p_ucFinal + len, p_ucOri, desc.desc_len + 2);
				}

				len += desc.desc_len + 2;
				
				p_ucOri += desc.desc_len + 2;
				temp_len -= desc.desc_len + 2;
			}


			////////////////////////////////////////////////////////
		
			while(streams_len > 4) { 	/*parse stream */
				offset = len;
				memcpy(p_ucFinal + len, p_ucOri, 5);

				es_type = get_bits (p_ucOri, 0,	0,	8);
				es_pid = get_bits (p_ucOri, 1, 3, 13);

				

				info_len = get_bits (p_ucOri, 3, 4, 12);	//es info length

				streams_len -= (info_len + 5);		//rest length


				if(p_mux_channel->pid_set_auto){
					
					//es_pid |= (es_type << 13);

					#if REGEN_SI_DEBUG
					printf("regen_pmt_section ---stream_pid--%#x----------\n", es_pid);
					#endif
					temp_pid = update_pid_mapping(mux_chan, in_chan, es_pid, es_pid);

				}else{
					p_pid = find_muxpid_in_mux_program(p_mux_program, es_pid);
					if(p_pid == NULL){
						p_ucOri += (info_len + 5);
						continue;
					}
					temp_pid = *p_pid;
				}

				set_bits(p_ucFinal + len, 1, 3, 13, temp_pid & 0x1fff);	//set es pid 
				len += 5;

				

				temp_len = info_len;
				p_ucOri += 5;
				while(temp_len>0){
					
					desc.desc_tag = get_bits (p_ucOri, 0, 0, 8);
					desc.desc_len = get_bits (p_ucOri, 1, 0, 8);
					#if REGEN_SI_DEBUG
					printf("regen_pmt_section ---desc_tag--%#x----------\n", desc.desc_tag);
					printf("regen_pmt_section ---desc_len--%d----------\n", desc.desc_len);
					#endif
		
					
					desc.desc_data = p_ucOri + 2;
		
					if(parseDescriptor(&desc) == 3){
						if(p_mux_channel->remove_ca_flag == 0){
							p_mux_channel->free_CA_mode = 1;	
							p_mux_program->free_CA_mode = 1;			
							memcpy(p_ucFinal + len, p_ucOri, desc.desc_len + 2);
							temp_pid = desc.desc_info.descCA.CA_PID;
							temp_pid = update_pid_mapping(mux_chan, in_chan, temp_pid, temp_pid);
							set_bits(p_ucFinal + len, 4, 3, 13, temp_pid);	//set ecm pid 
						}
					}else{					
						memcpy(p_ucFinal + len, p_ucOri, desc.desc_len + 2);
					}
					
					
					len += desc.desc_len + 2;
				
					p_ucOri += desc.desc_len + 2;
					temp_len -= desc.desc_len + 2;
				}

				

				
				set_bits(p_ucFinal+offset, 3, 4, 12, info_len);

				//========================================================
				
			}
		}else{
			
			#if REGEN_SI_DEBUG
			printf("regen pmt section : %d mux_chan , from the beginning thoroughly \n", mux_chan);
			#endif
			
			info_len = 0;	//program_info_length

			 /*
			  *	-----program descriptors-------
			//*/
			 
			p_ucFinal[len++] = 0xf0 | (info_len >> 8);	//set program_info_length high 4 bit
			p_ucFinal[len++] = info_len & 0xff;	//set program_info_length low 8 bit

			////////////////////////////////////////////////////////////////////////


			for(i=0; i<ES_PID_QTY_IN_A_PROGRAM; i++){
				
				es_pid = p_mux_program->es_pid_mapping[i][0];
				
				if((es_pid&0x1fff) == 0x1fff){
					break;
				}
				
				es_type = es_pid >> 13;
				
				p_ucFinal[len++] = es_type;
				
				p_ucFinal[len++] = 0xe0 | (es_pid >> 8);
				p_ucFinal[len++] = (es_pid & 0xff);

				streams_len = 0;	//es info length

				 /*
				  *	-----elementary stream descriptors-------
				//*/
				 
				p_ucFinal[len++] = 0xf0 | (streams_len >> 8);	//set es_info len high 4 bit
				p_ucFinal[len++] = streams_len & 0xff;	//set es_info len low 8 bit
				
			}
			
		}

		len += 4 - 3;
		p_ucFinal[1] = 0xb0 | (len >> 8);	//set section length high 4 bit
		p_ucFinal[2] = len & 0xff;	//set section length low 8 bit

		len -= 4 - 3;
		crc32 = calc_crc32(p_ucFinal, len);

		p_ucFinal[len++] = (crc32 >> 24) & 0xff;
		p_ucFinal[len++] = (crc32 >> 16) & 0xff;
		p_ucFinal[len++] = (crc32 >> 8) & 0xff;
		p_ucFinal[len++] = (crc32 >> 0) & 0xff;

		p_pmt_section->len = len;
		p_pmt_section->pid = p_mux_program->mux_pmt_pid & 0x1fff;

		#if REGEN_SI_DEBUG
		//print_bytes(p_ucFinal, len);
		#endif

		p_mux_program = p_mux_program->next;
	}

	
	return head_pmt_section;
}

mux_section_t *regen_cat_section(int mux_chan)
{
	int i, len = 0;
	u_int crc32;
	
	u_char *p_ucOri, *p_ucFinal;
	int info_len;
	
	section_t *p_section;
	mux_section_t *p_mux_section;
	mux_channel_t *p_mux_channel;
	tsin_within_mux_t *p_used_tsin;
	


	
	p_mux_channel = &mux_channels[mux_chan];
	p_used_tsin = p_mux_channel->used_tsin;
	
	int in_chan;
	u_short temp_pid;
	
	descriptor_t desc;
	int isExist_cat = 0;

	if(p_used_tsin == NULL){
		return NULL;
	}


	#if REGEN_SI_INFO
	printf("regen_cat_section : %d mux_chan -------start----------\n", mux_chan);
	#endif

	p_mux_section = create_mux_section();

	p_ucFinal = p_mux_section->data;
	len = 0;

	p_ucFinal[len++] = CAT_TABLEID;

	len += 2;
	
	p_ucFinal[len++] = 0xff;
	p_ucFinal[len++] = 0xff;

	p_ucFinal[len++] = 0xc0 | (0x1<<1) |0x1;	//set version number
	p_ucFinal[len++] = 0x00;	//set section number
	p_ucFinal[len++] = 0x00;	//set last section number
	
	while(p_used_tsin){
		
		in_chan = p_used_tsin->in_chan;
		
		p_section = ts_in_channels[in_chan].ts_tables.cat_section;
		
		if(p_section){
			
			info_len = p_section->len - 8 - 4;
			p_ucOri = p_section->data + 8;

			while(info_len>0){
		
				desc.desc_tag = get_bits (p_ucOri, 0, 0, 8);
				desc.desc_len = get_bits (p_ucOri, 1, 0, 8);
				#if REGEN_SI_DEBUG
				printf("regen_cat_section ---desc_tag--%x----------\n", desc.desc_tag);
				printf("regen_cat_section ---desc_len--%d----------\n", desc.desc_len);
				#endif

				memcpy(p_ucFinal + len, p_ucOri, desc.desc_len + 2);
				
				desc.desc_data = p_ucOri + 2;

				if(parseDescriptor(&desc) == 3){

					temp_pid = desc.desc_info.descCA.CA_PID ;
					#if REGEN_SI_DEBUG
					printf("regen_cat_section ---emm pid--%#x----------\n", temp_pid);
					#endif
					temp_pid = update_pid_mapping(mux_chan, in_chan, temp_pid, temp_pid);
					set_bits(p_ucFinal + len, 4, 3, 13, temp_pid);	//set emm pid 

				}

				len += desc.desc_len + 2;
				
				p_ucOri += desc.desc_len + 2;
				info_len -= desc.desc_len + 2;
			}
		}
	
		isExist_cat = 1;

		p_used_tsin = p_used_tsin->next;
	}



	if(isExist_cat){
		len += 4 - 3;
		p_ucFinal[1] = 0xb0 | (len >> 8);	//set section length high 4 bit
		p_ucFinal[2] = len & 0xff;	//set section length low 8 bit

		len -= 4 - 3;
		crc32 = calc_crc32(p_ucFinal, len);

		p_ucFinal[len++] = (crc32 >> 24) & 0xff;
		p_ucFinal[len++] = (crc32 >> 16) & 0xff;
		p_ucFinal[len++] = (crc32 >> 8) & 0xff;
		p_ucFinal[len++] = (crc32 >> 0) & 0xff;

		p_mux_section->len = len;
		p_mux_section->pid = CAT_PID;
		
		#if REGEN_SI_DEBUG
		//print_bytes(p_ucFinal, len);
		#endif

		add_section_to_mux(mux_chan, p_mux_section);
		
		return p_mux_section;
	}else{
		free(p_mux_section);
		return NULL;
	}

}


mux_section_t *regen_sdt_section(int mux_chan)
{
	int i, len = 0;
	u_int crc32;
	
	u_char *p_ucOri, *p_ucFinal;
	int info_len, service_name_len, provider_name_len;
	int des_loop_len, offset;
	u_char desc_len, desc_type;
	u_short service_id, new_pn;
	mux_program_t *p_mux_program, *cur_mux_program;

	section_t *p_section;
	mux_section_t *p_mux_section;
	mux_channel_t *p_mux_channel;
	

	u_char in_chan_masks[IN_CHANNEL_MASK_SIZE];
	int in_chan;
	u_short temp_pid;
	
	#if REGEN_SI_INFO
	printf("regen_sdt_section : %d mux_chan -------start----------\n", mux_chan);
	#endif
	
	p_mux_channel = &mux_channels[mux_chan];
	p_mux_program = p_mux_channel->mux_programs;


	if(p_mux_program == NULL){
		return NULL;
	}
	
	p_mux_section = create_mux_section();

	p_ucFinal = p_mux_section->data;
	len = 0;

	p_ucFinal[len++] = SDT_TABLEID;

	len += 2;
	
	p_ucFinal[len++] = p_mux_channel->ts_id >> 8;
	p_ucFinal[len++] = p_mux_channel->ts_id & 0xff;


	p_ucFinal[len++] = 0xc0 | (0x1<<1) |0x1;	//set version number
	p_ucFinal[len++] = 0x00;	//set section number
	p_ucFinal[len++] = 0x00;	//set last section number

	p_ucFinal[len++] = p_mux_channel->ori_network_id >> 8;
	p_ucFinal[len++] = p_mux_channel->ori_network_id & 0xff;


	p_ucFinal[len++] = 0xff;

	memset(in_chan_masks, 0, IN_CHANNEL_MASK_SIZE);
	
	while(p_mux_program){
		
		in_chan = p_mux_program->in_chan;

		//ts_in_channels[in_chan].ts_flag &= ~TSIN_SDT_CHANGE;
		
		p_section = ts_in_channels[in_chan].ts_tables.sdt_section;
			
		if(p_section){
				
			if(check_maskbit(in_chan_masks, in_chan) == 0){
							
				info_len = p_section->len - 8 - 3 - 4;
				p_ucOri = p_section->data + 11;

				while(info_len>0){
			
					service_id = get_bits (p_ucOri, 0, 0, 16);
					#if REGEN_SI_DEBUG
					printf("--------------------- regen_sdt_section -service_id = %d-------------\n", service_id);
					#endif
					
					des_loop_len = get_bits (p_ucOri, 3, 4, 12);

					#if REGEN_SI_DEBUG
					printf("regen_sdt_section  ---info_len--%d--, --des_loop_len = %d------\n", info_len, des_loop_len);
					#endif
					
					info_len -= (des_loop_len + 5);
					
					//*
					cur_mux_program = find_mux_program_by_oldpn(mux_chan, in_chan, service_id);
					//new_pn = find_newpn_in_mapping(mux_chan, in_chan, service_id);
					if(cur_mux_program == NULL){
						p_ucOri += (des_loop_len + 5);
						continue;
					}
					//*/
					new_pn = cur_mux_program->new_pn;

					#if REGEN_SI_DEBUG
					printf("regen_sdt_section  ---new_pn--%d----------\n", new_pn);
					#endif
										
					memcpy(p_ucFinal + len, p_ucOri, 5);
					set_bits(p_ucFinal + len, 0, 0, 16, new_pn);

					if(p_mux_channel->remove_ca_flag == 0){

					}else{
						set_bits(p_ucFinal + len, 3, 3, 1, 0);
					}
						
					
					p_ucOri += 5;
					offset = 5;

					while(des_loop_len>0){

						desc_type = get_bits (p_ucOri, 0, 0, 8);
						desc_len = get_bits (p_ucOri, 1, 0, 8);
						#if REGEN_SI_DEBUG
						printf("regen_sdt_section  ---desc_tag--%x----------\n", desc_type);
						printf("regen_sdt_section ---desc_len--%d----------\n", desc_len);
						#endif

						des_loop_len -= desc_len + 2;

						
						
						if(desc_type == SERVICE_DESTAG){
							
							p_ucOri += desc_len + 2;
						
							p_ucFinal[len + offset + 2] = cur_mux_program->service_info.service_type;
							provider_name_len = strlen(cur_mux_program->service_info.provider_name);
							#if REGEN_SI_DEBUG
							printf("regen_sdt_section ---provider_name_len--%d----------\n", provider_name_len);
							#endif
							p_ucFinal[len + offset + 3] = provider_name_len;
							memcpy(p_ucFinal + len + offset + 4, cur_mux_program->service_info.provider_name, provider_name_len);
							service_name_len = strlen(cur_mux_program->service_info.service_name);
							#if REGEN_SI_DEBUG
							printf("regen_sdt_section ---service_name_len--%d----------\n", service_name_len);
							#endif
							p_ucFinal[len + offset + 4 + provider_name_len] = service_name_len;
							memcpy(p_ucFinal + len + offset + 5 + provider_name_len, cur_mux_program->service_info.service_name, service_name_len);

							desc_len = provider_name_len + service_name_len + 3;
							#if REGEN_SI_DEBUG
							printf("regen_sdt_section ---service info desc_len--%d----------\n", desc_len);
							#endif
						
							p_ucFinal[len + offset + 1] = desc_len;
							p_ucFinal[len + offset + 0] = desc_type;
							#if REGEN_SI_DEBUG
							printf("regen_sdt_section ---service name--%s----------\n", cur_mux_program->service_info.service_name);
							#endif
						}else{
							memcpy(p_ucFinal + len + offset, p_ucOri, desc_len + 2);
							p_ucOri += desc_len + 2;
						}

						offset += desc_len + 2;
					}
					
					set_bits(p_ucFinal + len, 3, 4, 12, offset - 5);
					len += offset;
					
				}
				
				set_maskbit(in_chan_masks, in_chan);
			}
			
		}else{
			#if REGEN_SI_DEBUG
			printf("regen sdt section : %d mux_chan , from the beginning thoroughly \n", mux_chan);
			#endif
		
			service_id = p_mux_program->new_pn;
			
			p_ucFinal[len+0] = service_id >> 8;
			p_ucFinal[len+1] = service_id & 0xff;


			p_ucFinal[len+2] = 0xfc;		//eit pf and s all not exsit
			p_ucFinal[len+3] = 0x80;		//running and unscrambled
			
			des_loop_len = 0;
			
			offset = 5;
			desc_len = 0;
			
			p_ucFinal[len + offset + 0] = SERVICE_DESTAG;

			p_ucFinal[len + offset + 2] = p_mux_program->service_info.service_type; //set service type

			provider_name_len = strlen(p_mux_program->service_info.provider_name);
			p_ucFinal[len + offset + 3] = provider_name_len;
			memcpy(p_ucFinal + len + offset + 4, p_mux_program->service_info.provider_name, provider_name_len);

			service_name_len = strlen(p_mux_program->service_info.service_name);
			p_ucFinal[len + offset + 4 + provider_name_len] = service_name_len;
			memcpy(p_ucFinal + len + offset + 5 + provider_name_len, p_mux_program->service_info.service_name, service_name_len);

			desc_len = provider_name_len + service_name_len + 3;
			p_ucFinal[len + offset + 1] = desc_len;
			

			des_loop_len += desc_len + 2; 
			p_ucFinal[len+3] |= (des_loop_len >> 8) & 0xf;		//set des_loop_len high 4 bit
			p_ucFinal[len+4] = (des_loop_len) & 0xff;		//set des_loop_len low 8 bit
			
			len += des_loop_len + 5;
		}

		p_mux_program = p_mux_program->next;
	}

	len += 4 - 3;
	p_ucFinal[1] = 0xb0 | (len >> 8);	//set section length high 4 bit
	p_ucFinal[2] = len & 0xff;	//set section length low 8 bit

	len -= 4 - 3;
	crc32 = calc_crc32(p_ucFinal, len);

	p_ucFinal[len++] = (crc32 >> 24) & 0xff;
	p_ucFinal[len++] = (crc32 >> 16) & 0xff;
	p_ucFinal[len++] = (crc32 >> 8) & 0xff;
	p_ucFinal[len++] = (crc32 >> 0) & 0xff;

	p_mux_section->len = len;
	p_mux_section->pid = SDT_PID;

	#if REGEN_SI_DEBUG
	//print_bytes(p_ucFinal, len);
	#endif

	add_section_to_mux(mux_chan, p_mux_section);
	
	return p_mux_section;
}

mux_section_t *regen_nit_section(int mux_chan)
{

	//ts_in_channels[in_chan].ts_flag &= ~TSIN_NIT_CHANGE;

	return NULL;
}

#if 1
int regen_mux_pid_mapping(int mux_chan)
{
	int i;
	mux_channel_t *p_mux_channel;
	pid_mapping_t *p_pid_mapping;
	mux_program_t *p_mux_program;

	ts_in_table_t *p_ts_table;

	int in_chan;
	u_short temp_pid;
	u_short (*p_es_pid)[2];


	#if MUX_INFO
	printf(" %d mux_channel regen_mux_pid_mapping  !!!\n", mux_chan);
	#endif

	p_mux_channel = &mux_channels[mux_chan];


	p_pid_mapping = p_mux_channel->pass_pids;
	while(p_pid_mapping){
		p_pid_mapping->mux_pid = update_pid_mapping(mux_chan, p_pid_mapping->in_chan
			, p_pid_mapping->in_pid, p_pid_mapping->mux_pid);
		p_pid_mapping = p_pid_mapping->next;
	}

	if(p_mux_channel->pid_set_auto){
		return EXEC_OK;
	}

	
	p_mux_program = p_mux_channel->mux_programs;
	while(p_mux_program){
		
		in_chan = p_mux_program->in_chan;


		temp_pid = p_mux_program->in_pmt_pid  | (PMT_PACKET << 13);
		temp_pid = update_pid_mapping(mux_chan, in_chan, temp_pid, p_mux_program->mux_pmt_pid);
		p_mux_program->mux_pmt_pid &= (0x7 << 13);
		p_mux_program->mux_pmt_pid |= temp_pid;

		
		p_mux_program = p_mux_program->next;
	}

	
	p_mux_program = p_mux_channel->mux_programs;
	while(p_mux_program){
		
		in_chan = p_mux_program->in_chan;
	
		
		temp_pid = p_mux_program->in_pcr_pid;
		temp_pid = update_pid_mapping(mux_chan, in_chan, temp_pid, p_mux_program->mux_pcr_pid);
		p_mux_program->mux_pcr_pid &= (0x7 << 13);
		p_mux_program->mux_pcr_pid |= temp_pid;
		
		for(i=0; i<ES_PID_QTY_IN_A_PROGRAM; i++){
			p_es_pid = &p_mux_program->es_pid_mapping[i];
			if(((*p_es_pid)[0] & 0x1fff) != 0x1fff){
				temp_pid = update_pid_mapping(mux_chan, in_chan, (*p_es_pid)[0], (*p_es_pid)[1]);
				(*p_es_pid)[1] &= (0x7 << 13);
				(*p_es_pid)[1] |= temp_pid;
			}else{
				break;
			}
		}

		
		p_mux_program = p_mux_program->next;
	}

	
	return EXEC_OK;
}

#endif

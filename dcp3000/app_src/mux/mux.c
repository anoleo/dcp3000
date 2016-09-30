#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/utils.h"
#include "../utils/thread_pool.h"

#include "mux.h"

#include "mux_sql.h"

#include "../appref.h"

mux_channel_t mux_channels[MAX_MUX_CHANNEL_QTY]; 
char the_first_run_flag[MAX_MUX_CHANNEL_QTY];

/*
pat 10 hz, cat 10 hz, pmt 10 hz, nit 1/10 hz, bat 1/10 hz, sdt 1/2 hz
eit pf actual 1/2 hz, eit s actual <= 1day 1/10 hz, eit s actual > 1day 1/30 hz
ticker 10us

max ticker : 10000, sdt 200000, nit 1000000 , 
eit pf actual 200000, eit s actual <= 1day 1000000  , 
eit s actual > 1day 3000000 ; min ticker 2500
//*/
u_int mux_si_ticker[PSISI_SI_ZONE_QTY] = {SI_ZONE_0_TICKER, SI_ZONE_1_TICKER, SI_ZONE_2_TICKER, SI_ZONE_3_TICKER};

mux_info_t mux_info;

pid_hash_buf_t *create_pid_hash_buf(int pid_count)
{
	pid_hash_buf_t *p_hash_buf;
	p_hash_buf = malloc(sizeof(pid_hash_buf_t));
	if(p_hash_buf == NULL){
		printf("malloc pid_hash_buf_t faild, exit\n");
		exit(1);
	}

	p_hash_buf->buf = malloc(pid_count*sizeof(siidmap_st));
	if(p_hash_buf == NULL){
		printf("malloc  hash_buf of pid_hash_buf_t faild, exit\n");
		exit(1);
	}

	p_hash_buf->count = pid_count;
	p_hash_buf->offset = 0;

	return p_hash_buf;
}


void free_pid_hash_buf(pid_hash_buf_t *p_hash_buf)
{
	if(p_hash_buf == NULL){
		return;
	}

	free(p_hash_buf->buf);
	free(p_hash_buf);
	
}

u_char calc_pid_hash(u_short pid)
{
	u_char hash_value;
	u_char base_prime = 127;
	u_char hi_val, lo_val;

	hi_val = (pid>>8)+7;
	lo_val = (pid&0xff);
	
	hash_value = pid%base_prime;
	hash_value +=  pid/base_prime;

	hash_value += lo_val%hi_val;
	hash_value +=  lo_val/hi_val;

	printf("............calc_pid_hash, pid = %#04x, hash_value = %#02x\n..........", pid, hash_value);
	return hash_value;
}
#if 1
u_char get_si_id(u_short pid, pid_hash_buf_t *hash_buf)
{
	u_short si_id;
	int i;
	siidmap_st *p_buf;
	u_short tmp_pid;
	
	if(hash_buf == NULL){
		return 0;
	}
	

	p_buf = hash_buf->buf;
	for(i=0; i<hash_buf->offset; i++){
		if(p_buf[i].pid == pid){
			return p_buf[i].siid;
		}
		
	}

	
	if(hash_buf->offset>=hash_buf->count){
		return 0;
	}

	si_id = hash_buf->offset + 0x20;

	p_buf[hash_buf->offset].pid = pid;
	p_buf[hash_buf->offset].siid = si_id;
	
	printf("............get_si_id, pid = %#04x, si_id = %#02x\n..........", pid, si_id);
	
	hash_buf->offset++;

	return si_id;
}
#else
u_char get_si_id(u_short pid, pid_hash_buf_t *hash_buf)
{
	u_short si_id;
	int i;
	u_char *p_buf;
	u_short tmp_pid;
	
	if(hash_buf == NULL){
		return 0;
	}
	
	si_id = pid;
	
	do{
		si_id = calc_pid_hash(si_id+220);

		p_buf = hash_buf->hash_buf;
		for(i=0; i<hash_buf->offset; i+=3){
			tmp_pid = (p_buf[i]<<8) | p_buf[i+1];
			if(tmp_pid == pid){
				return p_buf[i+2];
			}else{
				if(si_id == p_buf[i+2]){
					break;
				}
			}
			
		}
		if(i<hash_buf->offset){
			continue;
		}else{
			break;
		}
	}while(1);

	p_buf[hash_buf->offset++] = pid >> 8;
	p_buf[hash_buf->offset++] = pid & 0xff;
	p_buf[hash_buf->offset++] = si_id;

	//printf("............get_si_id, pid = %#04x, si_id = %#02x\n..........", pid, si_id);
	

	return si_id;
}
#endif



struct mux_channel_lock_st{
	int mux_chan;
	pthread_mutex_t chan_lock[MAX_MUX_CHANNEL_QTY];
	pthread_mutex_t emm_lock[MAX_MUX_CHANNEL_QTY];
	pthread_mutex_t fpga_lock;
};
struct mux_channel_lock_st mux_chan_lock;

void init_mux_channel_lock(void)
{
	int i;
	
	mux_chan_lock.mux_chan = -1;
	pthread_mutex_init(&mux_chan_lock.fpga_lock, NULL);

	for(i=0; i<mux_info.mux_channels; i++){
		pthread_mutex_init(&mux_chan_lock.chan_lock[i], NULL);
		pthread_mutex_init(&mux_chan_lock.emm_lock[i], NULL);
	}
	
}

void lock_mux_channel(int mux_chan)
{
	//printf("!!!!!!!!!!!!!lock_mux_channel!!!!!!!!!!\n");
	pthread_mutex_lock(&(mux_chan_lock.chan_lock[mux_chan]));

}

void unlock_mux_channel(int mux_chan)
{
	//printf("!!!!!!!!!!!!!unlock_mux_channel!!!!!!!!!!\n");
	pthread_mutex_unlock(&(mux_chan_lock.chan_lock[mux_chan])); 
}


void lock_fpga_mux_channel(void)
{
	//printf("!!!!!!!!!!!!!lock_fpga_mux_channel!!!!!!!!!!\n");
	pthread_mutex_lock(&(mux_chan_lock.fpga_lock));

}

void unlock_fpga_mux_channel(void)
{
	//printf("!!!!!!!!!!!!!unlock_fpga_mux_channel!!!!!!!!!!\n");
	pthread_mutex_unlock(&(mux_chan_lock.fpga_lock)); 
}




int check_mux_status(int mux_chan)
{
	return (mux_channels[mux_chan].flag & MUX_SET_FPGA_OK);
}

int check_mux_si_update_status(int mux_chan)
{
	if(mux_channels[mux_chan].tsin_change_flag){
		return (mux_channels[mux_chan].flag & MUX_SI_UPDATE_END_OK);
	}else{
		return 1;
	}
}


int check_mux_input_src(int mux_chan)
{
	int hasSrc = 0;

	tsin_within_mux_t *p_used_tsin;

	p_used_tsin = mux_channels[mux_chan].used_tsin;
		
	while(p_used_tsin){
		//read_tsin_bitrate(p_mux_program->in_chan);
		if(ts_in_channels[p_used_tsin->in_chan].ts_flag & TSIN_LOCK){
			return 1;
		}

		p_used_tsin = p_used_tsin->next;
	}

	return 0;
}



int mux_abnormal_flag = 0;
/*
mux的有效码率寄存器由每1s更新改为250ms更新一次。
mux的有效码率计算公式修改为:
如64QAM,6.875M, = 38014706 bps 
有效码率% = (38014706 – N*188*8*4)/38014706

*/
int read_mux_validrate(int mux_chan)
{
	u_int null_count;
	u_int validrate,total_bitrate,null_bitrate;
	u_short mux_enable_flag, run_status = 0;

	
	lock_fpga_mux_channel();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(MUX_TOP_MUX_CHANNEL, mux_chan);
	mux_enable_flag = FPGA_MUX_READ(MUX_TOP_MUX_ENABLE);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	
	unlock_fpga_mux_channel();
/*
	//printf("---read_mux_validrate---, %d mux_chan, mux_enable_flag = %#x\n", mux_chan, mux_enable_flag);


	if(check_mux_input_src(mux_chan) == 0){
		if((mux_enable_flag&0x3) == 3){
			//disable_fpga_mux_channel(mux_chan);
			//add_log(EVENT_CODE(REMUX_EVENT_OBJ,EVENT_ACT_DISABLE,EVENT_PARAM(0,mux_chan+1)));
		}
		validrate = 0;
		//printf("---read_mux_validrate---, %d mux_chan, ...................\n", mux_chan);
		goto Read_End;
	}else{
		if((mux_enable_flag&0x3) != 3){
			validrate = 0;
			goto Read_End;
			if(mux_channels[mux_chan].enable){
				//enable_fpga_mux_channel(mux_chan, 1);
			}else{
				
			}
		}
	}

	//*/

	validrate = read_transceiver_channel_valid_byterate(0xb00+mux_chan)*8;

	validrate = (validrate + 500) / 1000;

	Read_End:
		

	#if 1
		
		run_status = MUX_NORMAL;

		if(mux_enable_flag){
			if(validrate == 0){
				run_status = MUX_NOT_TS;
				mux_abnormal_flag = 1;
			}else if(validrate>= total_bitrate*0.95){
				run_status = MUX_OVERFLOW;
				mux_abnormal_flag = 1;
			}
		}else{
			run_status = MUX_SHUTDOWN;
		}

	#endif
	mux_channels[mux_chan].run_status = run_status;
	mux_channels[mux_chan].valid_bitrate = validrate;
	//printf("calc %d mux : total_bitrate = %d, validrate = %d--\n", mux_chan,mux_channels[mux_chan].total_bitrate,validrate);

	return mux_channels[mux_chan].valid_bitrate;
}

void print_mux_si_zone(mux_si_zone_t *p_zone)
{
	mux_packet_t *p_packet, *p_next;

	printf(" zone:%d,  packet_point:%d\n", p_zone->zone_num, p_zone->packet_point);
	
	p_packet = p_zone->p_packet;
	while(p_packet){
		p_next = p_packet->next;
		printf("p_zone %d,    index:%d   si_id = %d\n", p_zone->zone_num
			, p_packet->index, p_packet->si_id);
		//PrintBuffer(p_packet->buf, 188);
		printf("--------------------------------------------------\n");
		//free(p_packet);
		p_packet = p_next;
	}
}


void print_mux_pid_mapping(int mux_chan)
{
	int i = 0;
	pid_mapping_t *p_pid_filter;
	
	printf("***************************************************************************\n");

	#if 1
	printf("----------channel:%d------pid_filter---------------\n", mux_chan);
	p_pid_filter = mux_channels[mux_chan].pid_mapping_list;
	while(p_pid_filter){
		printf("pid_filter : %d in_chan,  in_pid [0x%x],  mux_pid [0x%x] \n"
			, p_pid_filter->in_chan, p_pid_filter->in_pid, p_pid_filter->mux_pid);
		p_pid_filter = p_pid_filter->next;
	}
	
	#endif

} 

void print_mux_channel(int mux_chan)
{
	int i = 0;
	mux_packet_t *p_packet, *p_next;
	mux_si_zone_t *p_zone;
	pid_mapping_t *p_pid_filter;

	//for(mux_chan = 0; mux_chan < mux_info.mux_channels; mux_chan++){
		if((mux_channels[mux_chan].flag & MUX_PACKET_FINISH) == 0){
		//	return;
			//continue;
		}

		printf("%d mux_channel : flag = %#x \n ", mux_chan, mux_channels[mux_chan].flag);
		//printf("%d mux_channel : reperform_flag = %#x \n ", mux_chan, mux_channels[mux_chan].reperform_flag);
		printf("%d mux_channel : enable = %d \n ", mux_chan, mux_channels[mux_chan].enable);
		printf("%d mux_channel : pid_set_auto = %d \n ", mux_chan, mux_channels[mux_chan].pid_set_auto);
		printf("%d mux_channel : free_CA_mode = %d \n ", mux_chan, mux_channels[mux_chan].free_CA_mode);
		printf("%d mux_channel : insert_sdt_flag = %d \n ", mux_chan, mux_channels[mux_chan].insert_sdt_flag);
		printf("%d mux_channel : insert_nit_flag = %d \n ", mux_chan, mux_channels[mux_chan].insert_nit_flag);
		printf("%d mux_channel : insert_eit_flag = %d \n ", mux_chan, mux_channels[mux_chan].insert_eit_flag);
		printf("%d mux_channel : ts_id = %d \n ", mux_chan, mux_channels[mux_chan].ts_id);
		printf("%d mux_channel : network_id = %d \n ", mux_chan, mux_channels[mux_chan].network_id);
		printf("%d mux_channel : ori_network_id = %d \n ", mux_chan, mux_channels[mux_chan].ori_network_id);
		printf("%d mux_channel : total_bitrate = %d \n ", mux_chan, mux_channels[mux_chan].total_bitrate);
		printf("%d mux_channel : valid_bitrate = %d \n ", mux_chan, mux_channels[mux_chan].valid_bitrate);

		printf("***************************************************************************\n");

		print_all_mux_program(mux_chan);
			

		/*
		p_zone = mux_channels[mux_chan].si_zone;
		for(i=0; i<3; i++){
			
			printf("--channel:%d--, zone:%d,  packet_point:%d\n", mux_channels[mux_chan].mux_chan
			, p_zone[i].zone_num, p_zone[i].packet_point);
			
			p_packet = p_zone[i].p_packet;
			while(p_packet){
				p_next = p_packet->next;
				printf("p_zone %d,   index:%d\n", p_zone[i].zone_num, p_packet->index);
				print_bytes(p_packet->buf, 188);
				printf("--------------------------------------------------\n");
				//free(p_packet);
				p_packet = p_next;
			}

		}
		//*/

		#if 1
		printf("----------channel:%d------pid_filter---------------\n", mux_chan);
		p_pid_filter = mux_channels[mux_chan].pid_mapping_list;
		while(p_pid_filter){
			printf("pid_filter : %d in_chan,  in_pid [0x%x],  mux_pid [0x%x] \n"
				, p_pid_filter->in_chan, p_pid_filter->in_pid, p_pid_filter->mux_pid);
			p_pid_filter = p_pid_filter->next;
		}
		
		#endif
	//}

}

void reset_mux_si_zone(mux_si_zone_t *p_si_zone)
{

	mux_packet_t *p_packet, *next_packet;


	if(p_si_zone == NULL){
		return;
	}
		
	p_packet = p_si_zone->p_packet;
	while(p_packet){
		next_packet = p_packet->next;
		free(p_packet);
		p_packet = next_packet;
	}
		
	p_si_zone->cur_packet = NULL;
	p_si_zone->p_packet = NULL;
	p_si_zone->packet_point = 0;
	//p_si_zone->zone_num = -1;
	p_si_zone->mutex = MUTEX_UNLOCK;
	p_si_zone->enable = 0;
	
}

void init_mux_si_zones(int mux_chan)
{
	int i;
	mux_si_zone_t *si_zone;

	if(mux_chan<0 || mux_chan>=mux_info.mux_channels){
		return;
	}

	si_zone = mux_channels[mux_chan].si_zone;

//	lock_fpga_mux_channel();
	
	#ifdef MUX_REG_PAGE
//	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
//	FPGA_MUX_WRITE(MUX_TOP_MUX_CHANNEL, mux_chan);

	for(i=0; i<MUX_SI_ZONE_NUM; i++){
		si_zone[i].cur_packet = NULL;
		si_zone[i].p_packet = NULL;
		si_zone[i].packet_point = 0;
		si_zone[i].zone_num = i;
		si_zone[i].mutex = MUTEX_UNLOCK;
		si_zone[i].enable = 0;

//		FPGA_MUX_WRITE(MUX_TOP_SI_CHANNEL, i);

//		FPGA_MUX_WRITE(MUX_TOP_SI_TICKLE_LO, 0);
//		FPGA_MUX_WRITE(MUX_TOP_SI_TICKLE_HI, 0);
//		FPGA_MUX_WRITE(MUX_TOP_SI_PACKET_NUM, 0);
	}
	
//	FPGA_MUX_WRITE(MUX_TOP_SI_COUNT, MUX_SI_ZONE_NUM);
	#ifdef MUX_REG_PAGE
//	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	
	//unlock_fpga_mux_channel();
	
}

/*
void free_mux_ts_zones(mux_si_zone_t *head_si_zone)
{
	mux_si_zone_t *si_zone, *next_zone;
	mux_packet_t *p_packet, *next_packet;

	si_zone = head_si_zone;
	while(si_zone){
		
		next_zone = si_zone->next;
		
		p_packet = si_zone->p_packet;
		while(p_packet){
			next_packet = p_packet->next;
			free(p_packet);
			p_packet = next_packet;
		}
		
		free(si_zone);

		si_zone = next_zone;
	}
	
}

mux_si_zone_t *add_mux_si_zone(mux_channel_t *p_mux, u_char zone_num)
{
	mux_si_zone_t *si_zone, *p_zone;

	si_zone = malloc(sizeof(mux_si_zone_t));
	if(si_zone == NULL){
		printf("si_zone  alloc is faild !, exit!\n");
		exit(1);
	}

	si_zone->zone_num = zone_num;
	si_zone->packet_point = 0;
	si_zone->p_packet = NULL;
	si_zone->cur_packet = NULL;
	si_zone->next = NULL;
	si_zone->mutex = MUTEX_UNLOCK;

	if(p_mux->si_zone == NULL){
		p_mux->si_zone = si_zone;
	}else{
		p_zone = p_mux->si_zone;
		while(p_zone->next){
			p_zone = p_zone->next;
		}
		p_zone->next = si_zone;
	}

	return si_zone;
}
//*/


void free_pid_filters(pid_mapping_t *head_pid_filter)
{
	pid_mapping_t *pid_filter, *next_filter;

	pid_filter = head_pid_filter;
	while(pid_filter){
		next_filter = pid_filter->next;

		free(pid_filter);
		
		pid_filter = next_filter;
	}
}

void remove_all_mux_pid_filters(int mux_chan)
{
	mux_channel_t *p_mux_channel;
	p_mux_channel = &mux_channels[mux_chan];
	free_pid_filters(p_mux_channel->pass_pids);
	p_mux_channel->pass_pids = NULL;
	p_mux_channel->pass_pid_count = 0;
}

tsin_within_mux_t *create_tsin_within_mux(void)
{
	tsin_within_mux_t *p_used_tsin;

	p_used_tsin = malloc(sizeof(tsin_within_mux_t));
	if(p_used_tsin == NULL){
		printf("tsin_within_mux malloc faild , exit !\n");
		exit(1);
	}

	p_used_tsin->in_chan = -1;
	p_used_tsin->operated_mask = 0;
	p_used_tsin->next = NULL;

	return p_used_tsin;
}


void *free_all_tsin_within_mux(tsin_within_mux_t *head_tsin_within_mux)
{
	tsin_within_mux_t *p_used_tsin, *next_used_tsin;

	p_used_tsin = head_tsin_within_mux;

	while(p_used_tsin){
		next_used_tsin = p_used_tsin->next;
		free(p_used_tsin);
		p_used_tsin = next_used_tsin;
	}
}

tsin_within_mux_t *add_tsin_within_mux(int mux_chan, int in_chan)
{
	
	int endflag = 0;
	
	mux_channel_t *p_mux_channel;
	tsin_within_mux_t *p_used_tsin;
	
	tsin_within_mux_t *prev_node = NULL, *goal_node = NULL;
	tsin_within_mux_t *anchor_node = NULL, *junction_node = NULL;


	p_mux_channel = &mux_channels[mux_chan];

	p_used_tsin = p_mux_channel->used_tsin;


	/*
	printf("add_tsin_within_mux :  %d channel mux , in_chan %d \n", mux_chan, in_chan);
	//*/


	while(p_used_tsin){

		if((p_used_tsin->in_chan == in_chan)){

			//the node that preparing to add and this node is same

			return p_used_tsin;
			
		}else if(p_used_tsin->in_chan > in_chan){

			anchor_node = prev_node;
			junction_node = p_used_tsin;

			break;	
		}

		prev_node = p_used_tsin;
		p_used_tsin = p_used_tsin->next;
	}

	//if the goal node is non-existent then create it
 	if(goal_node == NULL){
		goal_node = create_tsin_within_mux();
		goal_node->in_chan = in_chan;
	}


	//insert the goal node to the insertion point
	if(anchor_node){
		anchor_node->next = goal_node;
	}else{
		if(prev_node){
			prev_node->next = goal_node;
		}else{
			p_mux_channel->used_tsin = goal_node;
		}
	}

	goal_node->next = junction_node;

	return goal_node;
}



void free_all_mux_program(mux_program_t *head_mux_program)
{
	mux_program_t *p_mux_program, *next_mux_program;

	p_mux_program = head_mux_program;
	while(p_mux_program){
		next_mux_program = p_mux_program->next;

		free(p_mux_program);
		
		p_mux_program = next_mux_program;
	}
}

void remove_all_mux_program(int mux_chan)
{
	
	mux_channel_t *p_mux_channel;
	p_mux_channel= &mux_channels[mux_chan];

	free_all_mux_program(p_mux_channel->mux_programs);
	p_mux_channel->mux_programs = NULL;
	p_mux_channel->mux_programs_raw = NULL;
	p_mux_channel->mux_programs_tail = NULL;
	p_mux_channel->program_count = 0;

	printf("remove %d mux all programs ... \n", mux_chan);
}

void reset_mux_programs(int mux_chan)
{
	mux_channel_t *p_mux_channel;

	p_mux_channel = &mux_channels[mux_chan];
	
	free_all_mux_program(p_mux_channel->mux_programs);
	p_mux_channel->mux_programs = NULL;

	p_mux_channel->mux_programs_raw = NULL;
	p_mux_channel->mux_programs_tail = NULL;

	p_mux_channel->program_count = 0;

	
	free_all_tsin_within_mux(p_mux_channel->used_tsin);
	p_mux_channel->used_tsin = NULL;
}



mux_program_t *create_mux_program(void)
{

	mux_program_t *p_mux_program;

	p_mux_program = malloc(sizeof(mux_program_t));
	if(p_mux_program == NULL){
		printf("mux_program malloc faild , exit !\n");
		exit(1);
	}

	//memset(p_mux_program, 0, sizeof(mux_program_t));
	p_mux_program->in_chan = -1;
	p_mux_program->old_pn = 0xffff;
	p_mux_program->new_pn = 0xffff;
	
	p_mux_program->mux_pmt_pid = 0x1fff;
	p_mux_program->in_pmt_pid = 0x1fff;

	p_mux_program->mux_pcr_pid = 0x1fff;
	p_mux_program->in_pcr_pid = 0x1fff;
	p_mux_program->free_CA_mode = 0;
	
	p_mux_program->cas_mode = 0;


	memset(p_mux_program->es_pid_mapping, 0xff, ES_PID_QTY_IN_A_PROGRAM * 4);
	memset(&p_mux_program->service_info, 0, sizeof(service_info_t));
	p_mux_program->next = NULL;
	p_mux_program->link  = NULL;

	p_mux_program->p_section = NULL;

	return p_mux_program;
}


pid_mapping_t *create_pid_mapping(void)
{

	pid_mapping_t *p_pid_mapping;

	p_pid_mapping = malloc(sizeof(pid_mapping_t));
	if(p_pid_mapping == NULL){
		printf("pid mapping malloc faild , exit !\n");
		exit(1);
	}

	p_pid_mapping->in_chan = -1;
	p_pid_mapping->in_pid = 0x1fff;
	p_pid_mapping->mux_pid = 0x1fff;
	p_pid_mapping->next = NULL;
	

	return p_pid_mapping;
}

mux_program_t *add_program_mapping(int mux_chan, int in_chan, u_short old_pn, u_short new_pn)
{
	int endflag = 0;
	
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	
	p_mux_channel = &mux_channels[mux_chan];


	p_mux_program = create_mux_program();
	p_mux_program->in_chan = in_chan;
	p_mux_program->old_pn = old_pn;
	
	p_mux_channel->program_count++;

	p_mux_program->new_pn = new_pn;


	p_mux_program->next = p_mux_channel->mux_programs;
	p_mux_channel->mux_programs = p_mux_program;


	add_tsin_within_mux(mux_chan, in_chan);

	return p_mux_program;
}


int add_pid_mapping(int mux_chan, int in_chan, u_short in_pid, u_short mux_pid)
{
	
	mux_channel_t *p_mux_channel;
	pid_mapping_t *p_pid_mapping;
	

	
	printf("add_pid_mapping :  %d mux, %d ts in , %#x in_pid, %#x mux_pid  \n", mux_chan, in_chan, in_pid, mux_pid);
	mux_pid &= 0x1fff;

	p_mux_channel = &mux_channels[mux_chan];


	p_pid_mapping = create_pid_mapping();
	p_pid_mapping->in_chan = in_chan;
	p_pid_mapping->in_pid = in_pid;

	p_pid_mapping->next = p_mux_channel->pass_pids;
	p_mux_channel->pass_pids = p_pid_mapping;

	p_pid_mapping->mux_pid = mux_pid;

	p_mux_channel->pass_pid_count++;

	return mux_pid;
}


mux_section_t *add_mux_section(int mux_chan, u_short pid, u_short section_len, char *section_data)
{
	
	mux_channel_t *p_mux_channel;
	mux_section_t *p_mux_section;
	

	printf("add_mux_section :  %d mux, %#x pid, %d len  \n", mux_chan, pid, section_len);

	pid &= 0x1fff;


	p_mux_channel = &mux_channels[mux_chan];


	p_mux_section = create_mux_section();
	p_mux_section->pid = pid;
	p_mux_section->len = section_len;
	memcpy(p_mux_section->data, section_data, section_len);

	p_mux_section->next = p_mux_channel->mux_sections;
	p_mux_channel->mux_sections = p_mux_section;


	p_mux_channel->section_count++;

	return p_mux_section;
}

void add_section_to_mux(int mux_chan, mux_section_t *p_mux_section)
{
	
	mux_channel_t *p_mux_channel;
	

	printf("add_section_to %d mux  \n", mux_chan);


	p_mux_channel = &mux_channels[mux_chan];

	p_mux_section->next = p_mux_channel->mux_sections;
	p_mux_channel->mux_sections = p_mux_section;


	p_mux_channel->section_count++;

	return p_mux_section;
}



void reset_mux_pid_mapping(int mux_chan)
{
	free_pid_filters(mux_channels[mux_chan].pass_pids);
	mux_channels[mux_chan].pass_pids = NULL;
	mux_channels[mux_chan].pass_pid_count = 0;
	
}

void reset_mux_sections(int mux_chan)
{
	free_all_mux_section(mux_channels[mux_chan].mux_sections);
	mux_channels[mux_chan].mux_sections = NULL;
	mux_channels[mux_chan].section_count = 0;
}

void reset_mux_parameters(int mux_chan)
{ 
	mux_channels[mux_chan].reperform_flag = 0;
 
	mux_channels[mux_chan].tsin_change_flag = 0;

	mux_channels[mux_chan].valid_bitrate = 0;
	mux_channels[mux_chan].total_bitrate = 0;

	mux_channels[mux_chan].enable = 0;
	mux_channels[mux_chan].pid_set_auto = 1;
	
	mux_channels[mux_chan].free_CA_mode = 0;
	
	mux_channels[mux_chan].insert_sdt_flag = 0;
	mux_channels[mux_chan].insert_nit_flag = 0;
	mux_channels[mux_chan].insert_eit_flag = 0;
	mux_channels[mux_chan].packet_length = 0;
			
	mux_channels[mux_chan].ts_id = 0;
	mux_channels[mux_chan].network_id = 0;
	mux_channels[mux_chan].ori_network_id = 0;
	  
}

void clean_mux_config(int mux_chan)
{

	lock_mux_channel(mux_chan);
	
	disable_fpga_mux_channel(mux_chan);

	reset_mux_parameters(mux_chan);

	reset_mux_programs(mux_chan);

	reset_mux_pid_mapping(mux_chan);

	reset_mux_sections(mux_chan);


	unlock_mux_channel(mux_chan);
}

void clean_all_mux_data(void)
{
	int mux_chan;

	for(mux_chan=0; mux_chan<MAX_MUX_CHANNEL_QTY; mux_chan++){
		clean_mux_config(mux_chan);
	}
	//system_cmd("rm -f %s", sqlite_db_filename);

	
}

void init_mux_channels(void)
{

	int mux_chan;
	
	init_mux_channel_lock();
	
	for(mux_chan=0; mux_chan<mux_info.mux_channels; mux_chan++){
		
		mux_channels[mux_chan].mux_chan = mux_chan;
		mux_channels[mux_chan].flag = 0;
		mux_channels[mux_chan].reperform_flag = 0;
		
		mux_channels[mux_chan].tsin_change_flag = 0;

		mux_channels[mux_chan].used_tsin = NULL;
		
		mux_channels[mux_chan].valid_bitrate = 0;
		//mux_channels[mux_chan].total_bitrate = qam_channel[mux_chan].max_bitrate * 1000;

		mux_channels[mux_chan].si_id_buf = NULL;
		//free_mux_ts_zones(mux_channels[mux_chan].si_zone);
		//mux_channels[mux_chan].si_zone = NULL;
		init_mux_si_zones(mux_chan);
		
		//free_pid_filters(mux_channels[mux_chan].pid_filter);
		mux_channels[mux_chan].pid_mapping_list = NULL;
		mux_channels[mux_chan].pass_pids = NULL;
		mux_channels[mux_chan].last_pid_mapping_list = NULL;

		mux_channels[mux_chan].enable = 0;
		mux_channels[mux_chan].pid_set_auto = 1;
		
		mux_channels[mux_chan].free_CA_mode = 0;
		
		mux_channels[mux_chan].remove_ca_flag = 0;
		mux_channels[mux_chan].insert_sdt_flag = 1;
		mux_channels[mux_chan].insert_nit_flag = 1;
		mux_channels[mux_chan].insert_eit_flag = 1;
		mux_channels[mux_chan].packet_length = 0;
				
		mux_channels[mux_chan].ts_id = 0;
		mux_channels[mux_chan].network_id = 0;
		mux_channels[mux_chan].ori_network_id = 0;
		
		//free_pid_filters(mux_channels[mux_chan].pid_filter);
		mux_channels[mux_chan].pid_mapping_list = NULL;
		mux_channels[mux_chan].last_pid_mapping_list = NULL;
		mux_channels[mux_chan].pass_pid_count = 0;
	//	mux_channels[mux_chan].pass_pids = NULL;
		mux_channels[mux_chan].program_count = 0;
		mux_channels[mux_chan].mux_programs = NULL;
		mux_channels[mux_chan].mux_programs_raw = NULL;
		mux_channels[mux_chan].mux_programs_tail = NULL;

		mux_channels[mux_chan].mux_sections = NULL;
		mux_channels[mux_chan].section_count = 0;


		the_first_run_flag[mux_chan] = 0;

		//printf("init mux %d channel ok !\n", mux_chan);
	}
	
}


void reset_mux_channel(int mux_chan)
{

	int i;
	
	//mux_channels[mux_chan].channel = mux_chan;
	mux_channels[mux_chan].flag = 0;
	//mux_channels[mux_chan].valid_bitrate = 0;
	//mux_channels[mux_chan].free_CA_mode = 0;

	//mux_channels[mux_chan].total_bitrate = remux_parameters[mux_chan].total_bitrate*1000;
	/*
	if(modulation_version){
		mux_channels[mux_chan].total_bitrate = qam_channel[mux_chan].max_bitrate * 1000;
	}
	//*/

	//free_mux_ts_zones(mux_channels[mux_chan].si_zone);
	//mux_channels[mux_chan].si_zone = NULL;

	free_pid_hash_buf(mux_channels[mux_chan].si_id_buf);
	mux_channels[mux_chan].si_id_buf = NULL;

	//free_all_tsin_within_mux(mux_channels[mux_chan].used_tsin);
	//mux_channels[mux_chan].used_tsin = NULL;

	/*
	for(i=0; i<3; i++){
		reset_mux_si_zone(&mux_channels[mux_chan].si_zone[i]);
	}
	//*/
	//free_pid_filters(mux_channels[mux_chan].pid_filter);
	//mux_channels[mux_chan].pid_filter = NULL;

	//printf("init mux %d channel ok !\n", mux_chan);
}


void enable_fpga_mux_channel(int mux_chan)
{

	lock_fpga_mux_channel();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(MUX_TOP_MUX_CHANNEL, mux_chan);
	FPGA_MUX_WRITE(MUX_TOP_MUX_ENABLE, 0x3);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	
	unlock_fpga_mux_channel();


	printf("%d mux_chan, enable_fpga_mux_channel ... \n", mux_chan);
}

void disable_fpga_mux_channel(int mux_chan)
{


	lock_fpga_mux_channel();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(MUX_TOP_MUX_CHANNEL, mux_chan);
	FPGA_MUX_WRITE(MUX_TOP_MUX_ENABLE, 0x0);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	
	unlock_fpga_mux_channel();



	printf("%d mux_chan, disable_fpga_mux_channel !!! \n", mux_chan);
}

int check_mux_enable(int mux_chan)
{
	u_short mux_enable_flag;

	lock_fpga_mux_channel();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(MUX_TOP_MUX_CHANNEL, mux_chan);
	mux_enable_flag = FPGA_MUX_READ(MUX_TOP_MUX_ENABLE);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	
	unlock_fpga_mux_channel();

	return ((mux_enable_flag & 0x3) == 3);
}

void disable_all_mux_channel(void)
{
	int i;

	for(i=0; i<mux_info.mux_channels; i++){
		if(mux_channels[i].enable == 1){
			disable_fpga_mux_channel(i);
		}
	}
}

void enable_all_mux_channel(void)
{
	int i;

	for(i=0; i<mux_info.mux_channels; i++){
		if(mux_channels[i].enable == 1){
			enable_fpga_mux_channel(i);
		}
	}
}


void init_mux_si_output(int mux_chan)
{
	int i;
	int page, addr_offset;

	page = DDR_Page_MUX_SI_Buf(mux_chan);

	addr_offset = DDR_Offset_MUX_SI_Buf(mux_chan);
	
	WRITE_FPGA_RAM_PAGE(page);

	for(i=0; i<MUX_SI_OUTPUT_BYTES_IN_CHAN; i+=(DDR_BIT_WIDTH/8)){
		FPGA_MUX_DDR_WRITE(addr_offset + i , (-1UL >> (32-DDR_BIT_WIDTH)));
	}

	UNLOCK_FPGA_RAM_PAGE(page);
	//printf("init fpga mux si output area success \n");
}


void set_mux_pll(int mux_chan)
{
	u_int nco2;

	lock_fpga_mux_channel();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(MUX_TOP_MUX_CHANNEL, mux_chan);
	  
	/*
	Example: 输出码率38Mbps, = 38M/8 = 4.75M bytes/s =	4.75MB/188 = 25.266 K packets/s 	
	Nco2 = 25.266*2^32/27000 = 4019135
	*/
	//nco2 = ((double)mux_channels[mux_chan].total_bitrate/(8*188)) * 0x400000 / 27000; 
	nco2 = ((double)mux_channels[mux_chan].total_bitrate/(8*188)) * 0x100000 * 0x1000/ 27000; 
	FPGA_MUX_WRITE(MUX_TOP_NCO2_LOW, nco2&0xffff );//set nco2 low bit
	FPGA_MUX_WRITE(MUX_TOP_NCO2_HIGH, nco2>>16 );//set nco2 high bit
	FPGA_MUX_WRITE(MUX_TOP_NCO2_PLL_SEL, 1);//choose 27M clock sorce
	FPGA_MUX_WRITE(MUX_TOP_BUF_OUT_DEPTH, 4);//Buf_OUT_Depth

	//printf(" start set %d channel fpga mux, total_bitrate = %d [ 0x%x ], nco2 = 0x%x\n", mux_chan
	//	, mux_channels[mux_chan].total_bitrate, mux_channels[mux_chan].total_bitrate, nco2);

	
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
		
	unlock_fpga_mux_channel();
	

}

void reset_all_mux_pll(void)
{
	int mux_chan;

	for(mux_chan=0; mux_chan<mux_info.mux_channels; mux_chan++){
		set_mux_pll(mux_chan);
	}
	
}


//-------------------------------------------------------------------------------------
int add_packet_to_mux_si_zone(mux_si_zone_t *si_zone, mux_packet_t *p_packet, int si_id)
{
	mux_packet_t *cur_packet = NULL;
	
	if((si_zone == NULL) || (p_packet == NULL)){
		return EXEC_NULL;
	}

	//printf("============add_packet_to_mux_si_zone, ===%d si_zone====si_id = %#.2x==========\n", si_zone->zone_num, si_id);

	while(si_zone->mutex != MUTEX_UNLOCK){
		//printf("---SectionArray2Packet2--MUX_PACKET_MUTEX\n");

		usleep(1000);
	}

	si_zone->mutex = MUTEX_LOCKED;

	
	if(si_zone->p_packet == NULL){
		si_zone->p_packet = p_packet;
	}else{
		si_zone->cur_packet->next = p_packet;
	}
	

	while(p_packet){
		
		if(si_zone->packet_point >= MAX_MUX_PACKET_NUMBER){
			printf("muxpacket pool too short\n");
			si_zone->mutex = MUTEX_UNLOCK;
			return EXEC_ERR;
		}


		si_zone->packet_point++;
		
		p_packet->si_id = si_id;

		if(p_packet->next == NULL){
			break;
		}

		p_packet = p_packet->next;

	}

	
	si_zone->cur_packet = p_packet;
	
	si_zone->mutex = MUTEX_UNLOCK;
	//printf("add_packet_to_mux_si_zone count = %d\n", si_zone->packet_point);
	
	return EXEC_OK;
	
}

int SectionArray2Packet(mux_si_zone_t *si_zone, mux_section_t *p_section, u_int pid, u_char si_id)
{
	int start = -1, current;
	int count = 0;
	int index = 0;
	int first;
	int packet_count = 0;
	int p;
	int i;
	u_char *section;
	u_int len;
	u_int pidtemp = 0;
	u_char continue_next_section_flag = 0;

	mux_packet_t *mux_packet = NULL, *cur_packet;

	while(si_zone->mutex != MUTEX_UNLOCK){
		//printf("---SectionArray2Packet2--MUX_PACKET_MUTEX\n");
		//return -1;

		usleep(1000);
	}

	si_zone->mutex = MUTEX_LOCKED;


	while(p_section){
		if(p_section->pid != pid){
			p_section = p_section->next;
			continue;
		}
		
		len = p_section->len;
		section = p_section->data;
		first = 1;
		count = 0;
		while(1){
			
			if(si_zone->packet_point >= MAX_MUX_PACKET_NUMBER){
				printf("muxpacket pool too short\n");
				si_zone->mutex = MUTEX_UNLOCK;
				return EXEC_ERR;
			}
			
			if(start == -1){
				start = si_zone->packet_point;
			}
			si_zone->packet_point++;

			if(mux_packet == NULL){
				cur_packet = create_mux_packet();
				
				mux_packet = cur_packet;
				
			}else{
				cur_packet->next = create_mux_packet();
				
				cur_packet = cur_packet->next;
				
			}

			
			//printf("SectionArray2Packet2 : packet_index=%d, packet_point=%d -----\n", 
			//	packet_index, si_zone->packet_point);
			
			cur_packet->si_id = si_id;
			cur_packet->index = index;

			
			cur_packet->buf[0] = 0x47;
			if(first) {
				cur_packet->buf[1] = 0x40 | ((pid >> 8) & 0x1f);
				cur_packet->buf[4] = 0;
				p = 5;
				first = 0;
			} else {
				cur_packet->buf[1] = 0x00 | ((pid >> 8) & 0x1f);
				p = 4;
			}
			cur_packet->buf[2] = pid & 0xff;
			cur_packet->buf[3] = 0x10 | ((index % 16) & 0xf);
			
			index++;
			packet_count++;

			for(; p < PACKAGE_MAX_LEN; p++) {
				cur_packet->buf[p] = section[count++];
				if(count >= len){
					p++;
					for(; p < PACKAGE_MAX_LEN; p++)
						cur_packet->buf[p] = 0xff;
					continue_next_section_flag = 1;
					break;
				}
			}
			
			if(continue_next_section_flag == 1)
			{
				continue_next_section_flag = 0;
				break;
			}
		}
		//PrintBuffer(cur_packet->buf,188);
		p_section = p_section->next;
	}

	if(start == -1){
		//printf("SectionArray2Packet:None %d Type Section\n", type);
		si_zone->mutex = MUTEX_UNLOCK;
		return EXEC_NULL;
	}
	
	if(si_zone->p_packet == NULL){
		si_zone->p_packet = mux_packet;
		si_zone->cur_packet = cur_packet;
	}else{
		si_zone->cur_packet->next = mux_packet;
		si_zone->cur_packet = cur_packet;
	}

	si_zone->mutex = MUTEX_UNLOCK;
	//printf("SectionArray2Packet count = %d\n", packet_count);
	
	return EXEC_OK;
	
}



//=====================================================================================

int precheck_packet_cc(mux_si_zone_t *si_zone, u_int pid)
{
	int cc = -1;
	u_int pidtemp;
	int find_the_same_pid = 0;
	mux_packet_t *p_packet;

	if(si_zone == NULL){
		return -1;
	}

	//printf("precheck_packet_cc : %d si_zone, pid = %d\n", si_zone->zone_num, pid);
	
	p_packet = si_zone->p_packet;

	while(p_packet){
		pidtemp = ((p_packet->buf[1] & 0x1f )<< 8) | p_packet->buf[2];
		if(pidtemp == pid){
			cc = p_packet->buf[3] & 0xf;
			//find_the_same_pid = 1;
		}else{
			if(find_the_same_pid == 1){
				break;
			}
		}
		p_packet = p_packet->next;
	}

	cc++;
	//printf("precheck_packet_cc : pid = %d, cc = %d\n", pid, cc);
	
	return cc;
}

int Section2Packet(mux_si_zone_t *si_zone, mux_section_t *p_section, u_char si_id)
{
	int start = 0;
	int offset = 0;
	int index = 0;
	int packet_count = 0;
	//u_int uiLast_pid = 0x1fff;
	//u_int pidtemp = 0;
	int p = 0;
	int i = 0;
	u_char *section = NULL;
	u_int len = 0;
	u_short pid = 0;

	mux_packet_t *mux_packet = NULL, *cur_packet;
	
	if(si_zone == NULL || p_section == NULL){
		return EXEC_NULL;
	}

	while(si_zone->mutex != MUTEX_UNLOCK){
		//printf("---Section2Packet2--MUX_PACKET_MUTEX\n");
		//return -1;

		usleep(1000);
	}

	si_zone->mutex = MUTEX_LOCKED;
		
	section = p_section->data;
	len = p_section->len;
	pid = p_section->pid;



	index = precheck_packet_cc(si_zone, pid);

	while(1){
		
		if(si_zone->packet_point >= MAX_MUX_PACKET_NUMBER){
			printf("+++++++muxpacket pool is full\n");
			si_zone->mutex = MUTEX_UNLOCK;
			return EXEC_ERR;
		}
		
		if(mux_packet == NULL){
			cur_packet = create_mux_packet();
			mux_packet = cur_packet;
		}else{
			cur_packet->next = create_mux_packet();
			cur_packet = cur_packet->next;
		}
			
		si_zone->packet_point++;

		cur_packet->si_id = si_id;

		cur_packet->index = index;
		
		cur_packet->buf[0] = 0x47;
		
		if(start == 0) {
			cur_packet->buf[1] = 0x40 | ((pid >> 8) & 0x1f);
			cur_packet->buf[4] = 0;
			p = 5;
			start = 1;
		}else{
			cur_packet->buf[1] = 0x00 | ((pid >> 8) & 0x1f);
			p = 4;
		}
		
		cur_packet->buf[2] = pid & 0xff;
		cur_packet->buf[3] = 0x10 | ((index % 16) & 0xf);

		//printf("Section2Packet2 : packet_index=%d, packet_point=%d -----\n", 
		//		packet_index, si_zone->packet_point);

		index++;
		//packet_count++;

		for(; p < PACKAGE_MAX_LEN; p++){

			cur_packet->buf[p] = section[offset++];
			
			if(offset >= len){
				
				p++;
				for(; p < PACKAGE_MAX_LEN; p++){
					cur_packet->buf[p] = 0xff;
				}
				
				//print_bytes(cur_packet->buf,188);

				if(si_zone->p_packet == NULL){
					si_zone->p_packet = mux_packet;
					si_zone->cur_packet = cur_packet;
				}else{
					si_zone->cur_packet->next = mux_packet;
					si_zone->cur_packet = cur_packet;
				}

				//printf("Section2Packet count = %d\n", packet_count);

				si_zone->mutex = MUTEX_UNLOCK;
				
				return EXEC_OK;
			}
		}

	}

	si_zone->mutex = MUTEX_UNLOCK;
	return EXEC_OK;
	
}

int eitTable2Packet(mux_si_zone_t *si_zone, eit_table_t *p_eit, u_short ts_id, u_short on_id, u_short new_pn, int si_id)
{
	int start = -1, current;
	int count = 0;
	int index = 0;
	int first;
	int packet_count = 0;
	int p;
	int i;
	u_char *ucOdata;
	u_int len;
	u_int crc32 = 0;
	u_char continue_next_section_flag = 0;
	u_short pid = EIT_PID;

	eit_sub_table_t *p_sub_table;
	eit_segment_t *p_segment;
	eit_section_t *p_section;

	mux_packet_t *mux_packet = NULL, *cur_packet;

	if(si_zone == NULL || p_eit == NULL){
		return EXEC_NULL;
	}

	while(si_zone->mutex != MUTEX_UNLOCK){
		//printf("---eitTable2Packet--MUX_PACKET_MUTEX\n");
		//return -1;

		usleep(1000);
	}

	si_zone->mutex = MUTEX_LOCKED;


	p_sub_table = p_eit->sub_tables;
	

	while(p_sub_table){

	mux_DebugPrintf("---eitTable2Packet--service id = %d, table id = %#x\n", p_eit->service_id, p_sub_table->table_id);
	
		p_segment = p_sub_table->segments;
		while(p_segment){
			p_section = p_segment->section;
			while(p_section){
				
				len = p_section->len;
				ucOdata = p_section->data;
				first = 1;
				count = 0;
				while(1){
					
					if(si_zone->packet_point >= MAX_MUX_PACKET_NUMBER){
						printf("muxpacket pool too short\n");
						si_zone->mutex = MUTEX_UNLOCK;
						return EXEC_ERR;
					}
					
					if(start == -1){
						start = si_zone->packet_point;
					}
					si_zone->packet_point++;

					if(mux_packet == NULL){
						cur_packet = create_mux_packet();
						
						mux_packet = cur_packet;
						
					}else{
						cur_packet->next = create_mux_packet();
						
						cur_packet = cur_packet->next;
						
					}

					
					cur_packet->si_id = si_id;
					cur_packet->index = index;

					
					cur_packet->buf[0] = 0x47;
					if(first) {
						cur_packet->buf[1] = 0x40 | ((pid >> 8) & 0x1f);
						cur_packet->buf[4] = 0;
						p = 5;
						first = 0;
					} else {
						cur_packet->buf[1] = 0x00 | ((pid >> 8) & 0x1f);
						p = 4;
					}
					cur_packet->buf[2] = pid & 0xff;
					cur_packet->buf[3] = 0x10 | ((index % 16) & 0xf);
					
					index++;
					
					packet_count++;

					for(; p < PACKAGE_MAX_LEN; p++) {
						
						if(count == 3){
							ucOdata[count] = new_pn >> 8;
						}else if(count == 4){
							ucOdata[count] = new_pn & 0xff;
						}else if(count == 8){
							ucOdata[count] = ts_id >> 8;
						}else if(count == 9){
							ucOdata[count] = ts_id & 0xff;
						}else if(count == 10){
							ucOdata[count] = on_id >> 8;
						}else if(count == 11){
							ucOdata[count] = on_id & 0xff;
						}
						
						cur_packet->buf[p] = ucOdata[count];
						count++;
						
						if(count >= len - 4){
							p++;

							crc32 = calc_crc32(ucOdata, len - 4);
							
							ucOdata[count++] = (crc32>>24) & 0xff;
							ucOdata[count++] = (crc32>>16) & 0xff;
							ucOdata[count++] = (crc32>>8) & 0xff;
							ucOdata[count++] = (crc32) & 0xff;

							cur_packet->buf[p++] = (crc32>>24) & 0xff;
							cur_packet->buf[p++] = (crc32>>16) & 0xff;
							cur_packet->buf[p++] = (crc32>>8) & 0xff;
							cur_packet->buf[p++] = (crc32) & 0xff;
							
							for(; p < PACKAGE_MAX_LEN; p++){
								cur_packet->buf[p] = 0xff;
							}
							continue_next_section_flag = 1;
							break;
						}
					}
	
					//print_bytes(cur_packet->buf,PACKAGE_MAX_LEN);
					
					if(continue_next_section_flag == 1){
						continue_next_section_flag = 0;
						break;
					}
				}
				p_section = p_section->next;
			}
			p_segment = p_segment->next;
		}

		p_sub_table = p_sub_table->next;
	}


	if(start == -1){
		printf("eit  table to Packet:None \n");
		si_zone->mutex = MUTEX_UNLOCK;
		return EXEC_NULL;
	}
	
	if(si_zone->p_packet == NULL){
		si_zone->p_packet = mux_packet;
		si_zone->cur_packet = cur_packet;
	}else{
		si_zone->cur_packet->next = mux_packet;
		si_zone->cur_packet = cur_packet;
	}




	si_zone->mutex = MUTEX_UNLOCK;
	//printf("SectionArray2Packet count = %d\n", packet_count);
	
	return EXEC_OK;
	
}


int prepare_mux_psi_packet(int mux_chan)
{
	
	int result = 0;
	
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	
	//mux_si_zone_t *si_zone;
	mux_si_zone_t *p_si_zone;
	mux_section_t *p_mux_section;
	//mux_section_t *p_pmt_section;


	pid_hash_buf_t *si_id_buf;
	u_char si_id = 0;

	int si_zone_num = 0;
	
	//printf("start packet %d mux psi section--\n", mux_chan);

	p_mux_channel = &mux_channels[mux_chan];
	si_id_buf = p_mux_channel->si_id_buf;
	
	p_mux_channel->free_CA_mode = 0;

	p_si_zone = &p_mux_channel->si_zone[si_zone_num];
	reset_mux_si_zone(p_si_zone);
	if(the_first_run_flag[mux_chan]){
		p_mux_section = p_mux_channel->mux_sections;	
	}else{
		p_mux_section = regen_pat_section(mux_chan);
	}

	if(p_mux_section){
		
		si_id = PAT_PID;
		//si_id++;
		
		//si_zone = add_mux_si_zone(p_mux_channel, 0);
		
		result = SectionArray2Packet(p_si_zone, p_mux_section, PAT_PID, si_id);
 		//free_all_mux_section(p_mux_section);
		if(result != EXEC_OK){
			printf("A pat SectionArrayPacket error, return \n");
			return EXEC_NULL;
		}
	}

	p_si_zone->enable = 1;

	mux_InfoPrintf("%d mux_chan, pmt_pc_insert packet start!\n", mux_chan);

	if(the_first_run_flag[mux_chan]){
		//p_mux_section = p_mux_channel->mux_sections;	
	}else{
		regen_pmt_section(mux_chan);
	}
	//p_pmt_section = p_mux_channel->mux_sections;
	p_mux_program = p_mux_channel->mux_programs;
		
	//p_mux_section = p_pmt_section;
	while(p_mux_program){
		p_mux_section = p_mux_program->p_section;
		if(p_mux_section && (p_mux_section->data[0] == PMT_TABLEID)){
			si_id = get_si_id(p_mux_section->pid, si_id_buf);
			//si_id++;

			mux_InfoPrintf("%d mux_chan, pmt [ %#.4x ] insert  start!\n", mux_chan, p_mux_section->pid);

			result = Section2Packet(p_si_zone, p_mux_section, si_id);
			if(result != EXEC_OK){
				printf("A pmt Section2Packet error\n");
			}
		}
	
		p_mux_program = p_mux_program->next;
	}
 	//free_all_mux_section(p_pmt_section);

	si_id_buf->offset -= p_mux_channel->program_count;

	if(p_mux_channel->remove_ca_flag == 0){

		if(the_first_run_flag[mux_chan]){
			p_mux_section = p_mux_channel->mux_sections;	
		}else{
		p_mux_section = regen_cat_section(mux_chan);
		}

		if(p_mux_section){
			
			mux_InfoPrintf("%d mux_chan, cat_pc_insert arraypacket start!\n", mux_chan);

			si_id = CAT_PID;
			//si_id++;

			result = SectionArray2Packet(p_si_zone, p_mux_section, CAT_PID, si_id);
 			//free_all_mux_section(p_mux_section);
			if(result != EXEC_OK){
				printf("A cat SectionArrayPacket error\n");
			}

		}
	}
	
	p_mux_channel->tsin_change_flag &= ~TSIN_PSI_CHANGE;

	mux_InfoPrintf("prepare_mux_psi_packet %d mux_chan : tsin_change_flag = %#x \n", mux_chan, p_mux_channel->tsin_change_flag);

	return EXEC_OK;
}


int prepare_mux_sdt_packet(int mux_chan)
{
	int result = 0;

	mux_channel_t *p_mux_channel;
	
	//mux_si_zone_t *si_zone;
	mux_si_zone_t *p_si_zone;
	mux_section_t *p_mux_section;

	pid_hash_buf_t *si_id_buf;
	u_char si_id = 0;
	int si_zone_num = 1;
	
	//printf("start packet %d mux sdt section--\n", mux_chan);
	
	p_mux_channel = &mux_channels[mux_chan];

	//si_id_buf = p_mux_channel->si_id_buf;
	
	p_si_zone = &p_mux_channel->si_zone[si_zone_num];
	reset_mux_si_zone(p_si_zone);
	if(the_first_run_flag[mux_chan]){
		p_mux_section = p_mux_channel->mux_sections;	
	}else{
	p_mux_section = regen_sdt_section(mux_chan);
	}
	if(p_mux_section){

		mux_InfoPrintf("%d mux_chan, sdt_pc_insert arraypacket start!\n", mux_chan);

		//si_zone = add_mux_si_zone(p_mux_channel, 1);
		p_si_zone->enable = 1;
		si_id = SDT_PID;
		//si_id++;
		
		result = SectionArray2Packet(p_si_zone, p_mux_section, SDT_PID, si_id);
		//free_all_mux_section(p_mux_section);
		if(result != EXEC_OK){
			printf("A sdt SectionArrayPacket error\n");
		}


	}
	
	p_mux_channel->tsin_change_flag &= ~TSIN_SDT_CHANGE;

	mux_InfoPrintf("prepare_mux_sdt_packet %d mux_chan : tsin_change_flag = %#x \n", mux_chan, p_mux_channel->tsin_change_flag);

	return EXEC_OK;
}


int prepare_mux_nit_packet(int mux_chan)
{
	int result = 0;

	mux_channel_t *p_mux_channel;
	
	//mux_si_zone_t *si_zone;
	mux_si_zone_t *p_si_zone;
	mux_section_t *p_mux_section;

	pid_hash_buf_t *si_id_buf;
	u_char si_id = 0;
	
	int si_zone_num = 2;
	
	//printf("start packet %d mux nit section--\n", mux_chan);
	
	p_mux_channel = &mux_channels[mux_chan];

	//si_id_buf = p_mux_channel->si_id_buf;
	
	p_si_zone = &p_mux_channel->si_zone[si_zone_num];
	reset_mux_si_zone(p_si_zone);

	

	//p_mux_section = regen_nit_section(mux_chan);
	p_mux_section = p_mux_channel->mux_sections;
	if(p_mux_section){

		mux_InfoPrintf("%d mux_chan,  nit_pc_insert arraypacket start!\n", mux_chan);

		//si_zone = add_mux_si_zone(p_mux_channel, 2);
		p_si_zone->enable = 1;
		si_id = NIT_PID;
		//si_id++;
		
		result = SectionArray2Packet(p_si_zone, p_mux_section, NIT_PID, si_id);
		//free_all_mux_section(p_mux_section);
		if(result != EXEC_OK){
			printf("A nit SectionArrayPacket error\n");
		}

	}
	
	//p_mux_channel->tsin_change_flag &= ~TSIN_NIT_CHANGE;

	mux_InfoPrintf("prepare_mux_nit_packet %d mux_chan : tsin_change_flag = %#x \n", mux_chan, p_mux_channel->tsin_change_flag);

	return EXEC_OK;
}


int prepare_mux_eit_packet(int mux_chan)
{
	int result = 0;
	int in_chan, ts_id, on_id, old_pn, new_pn;

	ts_in_channel_t *p_tsin_channel;
	mux_channel_t *p_mux_channel;
	eit_table_t *p_eit;

	mux_program_t *p_mux_program;
	
	mux_si_zone_t *p_si_zone;

	u_int flag_mask = 0;

	int si_id_count;
	pid_hash_buf_t *si_id_buf;
	u_char si_id = 0;

	int si_zone_num = 3;
	
	
	//printf("start packet %d mux eit section--\n", mux_chan);
	
	p_mux_channel = &mux_channels[mux_chan];
	
	p_si_zone = &p_mux_channel->si_zone[si_zone_num];
	reset_mux_si_zone(p_si_zone);
	
	p_mux_program = p_mux_channel->mux_programs;
	if(p_mux_program == NULL){
		return EXEC_NULL;
	}
	
	//si_id_buf = p_mux_channel->si_id_buf;

	si_id = EIT_PID;
	//si_id_buf->offset--;
	
	ts_id = p_mux_channel->ts_id;
	on_id = p_mux_channel->ori_network_id;

	mux_InfoPrintf("%d mux_chan, prepare eit packet : ts_id = %#x !!!\n", mux_chan, ts_id);
	
	flag_mask = (TSIN_LOCK | TSIN_PSI_READY | TSIN_EIT_READY);


	while(p_mux_program){
		
		in_chan = p_mux_program->in_chan;

		p_tsin_channel = &ts_in_channels[in_chan];
		
		//printf("%d mux_chan, prepare eit packet : %d in chan -ts_flag[%#x]--\n", mux_chan, in_chan, p_tsin_channel->ts_flag);

		if((p_tsin_channel->ts_flag & flag_mask) != flag_mask){
			p_mux_program = p_mux_program->next;
			continue;
		}
		
		old_pn = p_mux_program->old_pn;
		new_pn = p_mux_program->new_pn;

		mux_DebugPrintf("prepare eit packet : in_chan = %d, old_pn = %d, new_pn = %d !!!\n", in_chan, old_pn, new_pn);

		p_eit = find_ts_table_eit(in_chan, old_pn);
		
		eitTable2Packet(p_si_zone, p_eit, ts_id, on_id, new_pn, si_id);

		//p_tsin_channel->ts_flag &= ~TSIN_EIT_CHANGE;

		p_mux_program = p_mux_program->next;
	}


	p_si_zone->enable = 1;

	mux_InfoPrintf("%d mux_chan, prepare eit packet ok !!!\n", mux_chan);
	
	p_mux_channel->tsin_change_flag &= ~TSIN_EIT_CHANGE;

	mux_InfoPrintf("prepare_mux_eit_packet %d mux_chan : tsin_change_flag = %#x \n", mux_chan, p_mux_channel->tsin_change_flag);

	return EXEC_OK;
}


void clean_mapping_pid(int mux_chan, int in_chan, int in_pid)
{
	int page, offset, tempoffset;
	u_char bit_offset;
	u_short tmp_pid, v_pid;
	u_int tempval;

	
	page = DDR_Page_PidLookup(in_chan);

	tmp_pid = in_pid & 0x1fff;
	offset = DDR_Offset_PidLookup(in_chan, tmp_pid ); 

	
	mux_DebugPrintf("clean fpga pid filter , input channel %d, input pid %#x, page %#x, addr offset = %#x \n", in_chan, in_pid, page, offset);

	tempoffset = offset+((PID_MUX_MAPPING_BIT_S + mux_chan*13)/DDR_BIT_WIDTH)*(DDR_BIT_WIDTH/8);
	bit_offset = (PID_MUX_MAPPING_BIT_S+mux_chan*13)%DDR_BIT_WIDTH;
	
	WRITE_FPGA_RAM_PAGE(page);
	
	tempval = FPGA_MUX_DDR_READ(tempoffset);
	tempval |= 0x1fff << bit_offset;
	FPGA_MUX_DDR_WRITE(tempoffset, tempval);
	
	if((bit_offset + 13) > DDR_BIT_WIDTH){
		tempval = FPGA_MUX_DDR_READ(tempoffset+DDR_BIT_WIDTH/8);
		tempval |= 0x1fff >> (DDR_BIT_WIDTH-bit_offset) ;
		FPGA_MUX_DDR_WRITE(tempoffset+DDR_BIT_WIDTH/8, tempval);

	}

	tempoffset = offset + (mux_chan+PID_MUX_OUT_SW_BIT_S)/DDR_BIT_WIDTH*(DDR_BIT_WIDTH/8);
	bit_offset = (mux_chan+PID_MUX_OUT_SW_BIT_S)%DDR_BIT_WIDTH;
	
	tempval = FPGA_MUX_DDR_READ(tempoffset);
	tempval &= ~(1<<bit_offset);
	FPGA_MUX_DDR_WRITE(tempoffset, tempval);

	UNLOCK_FPGA_RAM_PAGE(page);
	
}

int prepare_pid_mapping(int mux_chan)
{
	int page, offset, tempoffset;
	u_char	bit_offset;
	u_short tempval, tmp_pid;

	pid_mapping_t *pid_filter, *next_pid_filter;

	//printf("prepare_pid_mapping : the_first_run_flag = %#x\n", the_first_run_flag[mux_chan]);

	if(the_first_run_flag[mux_chan]){
		mux_channels[mux_chan].flag |=MUX_PID_MAP_FINISH;
		return EXEC_OK;
	}

	//mux_InfoPrintf(" %d mux_channel prepare pid_filter_table map start ...\n", mux_chan);

	pid_filter =mux_channels[mux_chan].pid_mapping_list;
	//pid_filter = mux_channels[mux_chan].last_pid_mapping_list;
	
	while(pid_filter){
		
		next_pid_filter = pid_filter->next;

		if(pid_filter->in_chan >= mux_info.total_tsin_channels){
			goto Free_And_Continue;
		}


		if(pid_filter->in_chan >= mux_info.total_tsin_channels){
			printf("%d mux_chan, %d in_chan, prepare pid_filter_table, page overflow: %#x\n", mux_chan, pid_filter->in_chan, page);
			goto Free_And_Continue;
			//return EXEC_ERR;
		}
		
		if(((pid_filter->in_pid >> 13) & NOPASS_FLAG)){
			goto Free_And_Continue;
		}

		clean_mapping_pid(mux_chan, pid_filter->in_chan, pid_filter->in_pid);

		Free_And_Continue:
		free(pid_filter);
		pid_filter = next_pid_filter;
	}

	mux_channels[mux_chan].pid_mapping_list = NULL;
	//mux_channels[mux_chan].last_pid_mapping_list = NULL;
	
//*
	if(regen_mux_pid_mapping(mux_chan) != EXEC_OK){
		return EXEC_ERR;
	}
//*/
	mux_channels[mux_chan].flag |=MUX_PID_MAP_FINISH;

	mux_InfoPrintf(" %d mux_channel prepare pid_filter_table map ok !!!\n", mux_chan);

	return EXEC_OK;
}

int prepare_mux_packet(int mux_chan)
{
	int result = 0;

	mux_channel_t *p_mux_channel = &mux_channels[mux_chan];
	

	int si_id_count;
	pid_hash_buf_t *si_id_buf;
	u_char si_id = 0;
	
	//printf("start packet %d mux section--\n", mux_chan);

	si_id_count = 5+p_mux_channel->program_count  + 1;
	
	si_id_buf = create_pid_hash_buf(si_id_count);
	

	si_id_buf->buf[si_id_buf->offset].pid = PAT_PID;
	si_id_buf->buf[si_id_buf->offset++].siid = 0xfe;
	si_id_buf->buf[si_id_buf->offset].pid = CAT_PID;
	si_id_buf->buf[si_id_buf->offset++].siid  = CAT_PID;
	si_id_buf->buf[si_id_buf->offset].pid = SDT_PID;
	si_id_buf->buf[si_id_buf->offset++].siid  = SDT_PID;
	si_id_buf->buf[si_id_buf->offset].pid = NIT_PID;
	si_id_buf->buf[si_id_buf->offset++].siid  = NIT_PID;
	si_id_buf->buf[si_id_buf->offset].pid = EIT_PID;
	si_id_buf->buf[si_id_buf->offset++].siid  = EIT_PID;
	
	
	p_mux_channel->si_id_buf = si_id_buf;


	if(prepare_mux_psi_packet(mux_chan)!=EXEC_OK){return EXEC_ERR;}

	if(p_mux_channel->insert_sdt_flag){
		if(prepare_mux_sdt_packet(mux_chan)!=EXEC_OK){return EXEC_ERR;}
	}

	if(p_mux_channel->insert_nit_flag){
		prepare_mux_nit_packet(mux_chan);
	}
	
	if(p_mux_channel->insert_eit_flag){
		prepare_mux_eit_packet(mux_chan);
	}
		
	//free_all_mux_section(p_mux_channel->mux_sections);
	//p_mux_channel->mux_sections = NULL;
	//free_pid_hash_buf(si_id_buf);

	p_mux_channel->flag |= MUX_PACKET_FINISH;

	mux_InfoPrintf("%d mux_chan, prepare mux packet ok !!!\n", mux_chan);

	return EXEC_OK;
}

void write_mapping_pid(int mux_chan, int in_chan, int in_pid, int mux_pid)
{
	int page, offset, tempoffset;
	u_char bit_offset;
	u_short temp_pid, v_pid;
	u_int tempval;
	
	page = DDR_Page_PidLookup(in_chan);

	v_pid = in_pid & 0x1fff;
	offset = DDR_Offset_PidLookup(in_chan, v_pid); 
	
	mux_DebugPrintf("write fpga pid filter , input channel %d, input pid %#x, mux pid %#x, page %#x , offset %#x\n", in_chan, in_pid, mux_pid, page, offset);
	
	WRITE_FPGA_RAM_PAGE(page);
	
	temp_pid = mux_pid & 0x1fff;

	//set mux out pid
	//if((temp_pid &= 0x1fff) != 0x1fff){
	if(temp_pid != 0x1fff){

		if((v_pid==0x1fff)){
				printf("write %d mux fpga pid filter , input channel %d, input pid %#x, mux pid %#x, page %#x , offset %#x\n", mux_chan, in_chan, in_pid, mux_pid, page, offset);
		}

		tempoffset = offset+((PID_MUX_MAPPING_BIT_S + mux_chan*13)/DDR_BIT_WIDTH)*(DDR_BIT_WIDTH/8);
		bit_offset = (PID_MUX_MAPPING_BIT_S+mux_chan*13)%DDR_BIT_WIDTH;
		
		tempval = FPGA_MUX_DDR_READ(tempoffset);
		//printf("FPGA_RAM_READ pid_loop_up offset+2,  0x%04x, ---\n", tempval);
		tempval &= ~(0x1fff << bit_offset);
		tempval |= temp_pid << bit_offset;
		//printf("FPGA_RAM_READ pid_loop_up offset+2,  0x%04x, ===\n", tempval);
		FPGA_MUX_DDR_WRITE(tempoffset, tempval);
		//printf("FPGA_RAM_READ pid_loop_up offset+2,  0x%04x, ***\n", FPGA_RAM_READ(tempoffset));
	
		if((bit_offset + 13) > DDR_BIT_WIDTH){

			tempval = FPGA_MUX_DDR_READ(tempoffset+DDR_BIT_WIDTH/8);
			//printf("FPGA_RAM_READ pid_loop_up offset+4,  0x%04x, ---\n", tempval);
			tempval &= 0xffffffff << (13+bit_offset-DDR_BIT_WIDTH);
			tempval |= temp_pid >> (DDR_BIT_WIDTH-bit_offset) ;
			//printf("FPGA_RAM_READ pid_loop_up offset+4,  0x%04x, ===\n", tempval);
			FPGA_MUX_DDR_WRITE(tempoffset+DDR_BIT_WIDTH/8, tempval);
			//printf("FPGA_RAM_READ pid_loop_up offset+4,  0x%04x, ***\n", FPGA_RAM_READ(tempoffset+2));
			
		}

		
	}

	//set mux out switch

	tempoffset = offset + (mux_chan+PID_MUX_OUT_SW_BIT_S)/DDR_BIT_WIDTH*(DDR_BIT_WIDTH/8);
	bit_offset = (mux_chan+PID_MUX_OUT_SW_BIT_S)%DDR_BIT_WIDTH;
	tempval = FPGA_MUX_DDR_READ(tempoffset);
	tempval |= (1<<bit_offset);
	FPGA_MUX_DDR_WRITE(tempoffset, tempval);

	tempoffset = offset + (PID_MAPPING_SW_BIT)/DDR_BIT_WIDTH*(DDR_BIT_WIDTH/8);
	bit_offset = (PID_MAPPING_SW_BIT)%DDR_BIT_WIDTH;
	tempval = FPGA_MUX_DDR_READ(tempoffset);
	if((tempval & (1<<bit_offset)) == 0){
		tempval |= (1<<bit_offset);
		FPGA_MUX_DDR_WRITE(tempoffset, tempval);
	}
	
	
	UNLOCK_FPGA_RAM_PAGE(page);
	
}

int write_fpga_pid(int mux_chan)
{

	int page, offset, tempoffset;
	u_char bit_offset;
	u_short temp_pid, v_pid;
	u_int tempval;
	pid_mapping_t *pid_filter;

	if((mux_channels[mux_chan].flag & MUX_PID_MAP_FINISH) == 0){
		printf(" %d mux_channel mux_pid map not finish!\n", mux_chan);
		return EXEC_NULL;
	}

	mux_InfoPrintf("%d mux_chan, start write pid filter to fpga ...... \n", mux_chan);
	
	pid_filter = mux_channels[mux_chan].pid_mapping_list;
	//mux_channels[mux_chan].last_pid_mapping_list = pid_filter;
	
	while(pid_filter){
		
		if(pid_filter->in_chan >= mux_info.total_tsin_channels){
			pid_filter = pid_filter->next;
			continue;
		}
		

		if(((pid_filter->in_pid >> 13) & NOPASS_FLAG)){
			pid_filter = pid_filter->next;
			continue;
		}


		write_mapping_pid(mux_chan, pid_filter->in_chan, pid_filter->in_pid, pid_filter->mux_pid);
		//clean_mapping_pid(mux_chan, pid_filter->in_chan, 0x1fff);
		
		//printf("write fpga pid filter , input channel %d, sw_pid_out 0x%04x \n", pid_filter->in_chan, FPGA_RAM_READ(offset));
			

		pid_filter = pid_filter->next;
	}

	mux_InfoPrintf("%d mux_chan,  write pid filter to fpga ...ok!... \n", mux_chan);

	return EXEC_OK;
}

int write_packet_to_fpga_si_zone(int page, u_int *addr_base, mux_si_zone_t *p_si_zone)
{
	int  i, j;
	u_char *p_buf;
	u_int tempval;
	mux_packet_t *p_packet;
	int  addr_offset = 0;


	if(p_si_zone == NULL){
		return EXEC_NULL;
	}
	
	mux_DebugPrintf("write packet to fpga  %d si zone, packet_count:%d, addr offset%#x \n", p_si_zone->zone_num, p_si_zone->packet_point, *addr_base);
	
	addr_offset = 0;

	
	//while(p_si_zone){
		
		while(p_si_zone->mutex != MUTEX_UNLOCK){
			usleep(1000);
		}
		p_si_zone->mutex = MUTEX_LOCKED;


		p_packet = p_si_zone->p_packet;
		while(p_packet){

			if((MUX_SI_OUTPUT_BYTES_IN_CHAN/2 - addr_offset ) < 192){
				printf(" fpga mux packet address overflow \n");
				return EXEC_ERR;
			}
			
			WRITE_FPGA_RAM_PAGE(page);
			p_buf = p_packet->buf;

			tempval = p_buf[0];
			#if (DDR_BIT_WIDTH == 16)
			FPGA_MUX_DDR_WRITE(*addr_base + ((addr_offset )*2), tempval);
			addr_offset += (DDR_BIT_WIDTH/16);
			tempval = p_buf[1] | (p_packet->si_id << 8);
			#else
			tempval |= (p_buf[1] | (p_packet->si_id << 8))<<16;
			#endif

			printf("mux %#.2x, packet[2,3] = %#.2x,%.2x\n", p_packet->si_id, p_buf[2], p_buf[2]);
			
			FPGA_MUX_DDR_WRITE(*addr_base + ((addr_offset )*2), tempval);
			addr_offset += (DDR_BIT_WIDTH/16);
			
			for(i=2; i<PACKAGE_MAX_LEN;){
				
				tempval = p_buf[i++];
				#if (DDR_BIT_WIDTH == 32)
				tempval |= (p_buf[i++])<<16;
				#endif

				//printf("packet : addr %#x , %#x \n",  *addr_base + ((addr_offset)*2), tempval);
				FPGA_MUX_DDR_WRITE(*addr_base + ((addr_offset )*2), tempval);

				addr_offset += (DDR_BIT_WIDTH/16);
			}
			UNLOCK_FPGA_RAM_PAGE(page);
			
			addr_offset += (192-PACKAGE_MAX_LEN);

			p_packet = p_packet->next;
		}

		*addr_base += addr_offset*2;

		p_si_zone->mutex = MUTEX_UNLOCK;
	//}

	return EXEC_OK;
}


int write_fpga_packet(int mux_chan)
{
	int page, i;
	int zone_num;
	mux_si_zone_t *p_si_zone;
	int addr_base;


	if((mux_channels[mux_chan].flag & MUX_PACKET_FINISH) == 0){
		printf(" %d mux_channel mux_packet not finish!\n", mux_chan);
		return EXEC_NULL;
	}

	
	page = DDR_Page_MUX_SI_Buf(mux_chan);

	addr_base = DDR_Offset_MUX_SI_Buf(mux_chan);

	
	mux_InfoPrintf("%d mux_chan, start write packet to fpga , %#x page , %#x addr offset......\n", mux_chan, page, addr_base);

	//printf(" write fpga packet , %d channel, %d page, offset address : 0x%x  \n"
	//	, mux_chan, page, addr_base);
	
	p_si_zone = mux_channels[mux_chan].si_zone;
	for(zone_num=0; zone_num<PSISI_SI_ZONE_QTY; zone_num++){
		write_packet_to_fpga_si_zone(page, &addr_base, &p_si_zone[zone_num]);
	}


	mux_channels[mux_chan].flag |= MUX_PACKET_WRITED_FPGA;

	mux_InfoPrintf("%d mux_chan, write packet to fpga ...ok!...\n", mux_chan);

	return EXEC_OK;
}


int set_fpga_mux(int mux_chan)
{
	int zone_count, i;
	u_int tempval;
	u_int zone_offset;
	
	mux_si_zone_t *si_zone;
	
	lock_fpga_mux_channel();

	//choose page
	mux_InfoPrintf(" start set %d channel fpga mux ... \n", mux_chan);


	if((mux_channels[mux_chan].flag & MUX_PACKET_WRITED_FPGA) == 0){
		printf(" %d channel mux_packet write to fpga not finish!\n", mux_chan);
		unlock_fpga_mux_channel();
		return EXEC_ERR;
	}

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	
	//choose mux channel
	FPGA_MUX_WRITE(MUX_TOP_MUX_CHANNEL, mux_chan);



	//disable edgeQAM Mode
	//FPGA_MUX_WRITE(MUX_TOP_EDGEQAM_MODE, 0);
	

	zone_count = 0;
	zone_offset = 0;
	
	si_zone = mux_channels[mux_chan].si_zone;
	
	//for(i=0; i<MUX_SI_ZONE_NUM; i++){
	for(i=0; i<PSISI_SI_ZONE_QTY; i++){
	
		//choose si zone
		FPGA_MUX_WRITE(MUX_TOP_SI_CHANNEL, i);

		if(si_zone[i].enable && si_zone[i].packet_point>0){
			

			//set si zone offset address
			FPGA_MUX_WRITE(MUX_TOP_SI_ADDRESS_LO, zone_offset&0xffff);
			FPGA_MUX_WRITE(MUX_TOP_SI_ADDRESS_HI, zone_offset>>16);
			zone_offset += si_zone[i].packet_point * (0xc0 << 1);

			//set si zone ticker
			tempval = mux_si_ticker[i] / si_zone[i].packet_point;
			FPGA_MUX_WRITE(MUX_TOP_SI_TICKLE_LO, tempval&0xffff);
			FPGA_MUX_WRITE(MUX_TOP_SI_TICKLE_HI, 0xc000 | (tempval >> 16));
	
			//set si zone total packet count;
			FPGA_MUX_WRITE(MUX_TOP_SI_PACKET_NUM, si_zone[i].packet_point);
			
			
			//zone_count++;
		}else{
			FPGA_MUX_WRITE(MUX_TOP_SI_TICKLE_LO, 0);
			FPGA_MUX_WRITE(MUX_TOP_SI_TICKLE_HI, 0);
			FPGA_MUX_WRITE(MUX_TOP_SI_PACKET_NUM, 0);
		}

		mux_InfoPrintf(" start set %d channel fpga mux %d zone_offset = 0x%x, packet_count = %d\n", mux_chan, si_zone[i].zone_num, zone_offset, si_zone[i].packet_point);
	
	}

	//set si zone count
	//FPGA_RAM_WRITE(MUX_TOP_SI_COUNT, zone_count);
	
	//printf(" start set %d channel fpga mux , zone_count = %d\n", mux_chan, zone_count);


	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	unlock_fpga_mux_channel();

	
	mux_channels[mux_chan].flag |= MUX_SET_FPGA_OK;

	mux_InfoPrintf(" set %d channel fpga mux ...ok! \n", mux_chan);

	return EXEC_OK;
}




int remux(int mux_chan)
{

	int ret = EXEC_OK;
	int isEnable;

	TagPrintf(mux_chan);

	if(mux_channels[mux_chan].reperform_flag == 0){
		return EXEC_OK;
	}
	mux_channels[mux_chan].reperform_flag = 0;

	//*
	TagPrintf(mux_chan);

	isEnable = check_mux_enable(mux_chan);

	if(!mux_channels[mux_chan].enable){
		if(isEnable){
			disable_fpga_mux_channel(mux_chan);
		}
		return EXEC_OK;
	}
	//*/
	TagPrintf(mux_chan);
	
	reset_mux_channel(mux_chan);
	

	init_mux_si_output(mux_chan);

	//if(prepare_mux_programs(mux_chan) != EXEC_OK)return EXEC_ERR;
	ret = prepare_pid_mapping(mux_chan);

	if(ret != EXEC_OK)return EXEC_ERR;
 
	
	if(mux_channels[mux_chan].program_count==0){
		return EXEC_OK;
	}

	
	lock_mux_channel(mux_chan);
	

	
	if(prepare_mux_packet(mux_chan) != EXEC_OK){
		ret = EXEC_ERR;
		goto EndMux;
	}
	

	disable_fpga_mux_channel(mux_chan);

	//print_mux_channel(mux_chan);
	usleep(10000);

	if(write_fpga_pid(mux_chan) != EXEC_OK){
		ret = EXEC_ERR;
		goto EndMux;
	}
	usleep(10000);
	if(write_fpga_packet(mux_chan) != EXEC_OK){
		ret = EXEC_ERR;
		goto EndMux;
	}
	usleep(10000);

	if(set_fpga_mux(mux_chan) != EXEC_OK){
		ret = EXEC_ERR;
		goto EndMux;
	}
	usleep(5000);
	set_mux_pll(mux_chan);
	usleep(10000);



	//*
	if(isEnable == 0){
		
	}
	//*/
	usleep(10000);
	enable_fpga_mux_channel(mux_chan);


	//printf("....................%d mux_chan, mux success!........exec_times %d.....................\n\n", mux_chan, mux_exec_times);

	EndMux:
	unlock_mux_channel(mux_chan);


	return ret;
}


thread_pool_t *mux_thread_pool;

thread_pool_t *init_thread_pool;

void *mux_task(void *args)
{
	int  mux_chan;

	thread_pool_t *update_mux_si_thread_pool;
	task_arg_t task_arg;


	printf("mux_task thread: start....\n");


	
	task_arg.arg_size = 4;
	

	//printf("mux_task thread: start execute mux ...\n");


	#if 1
	if(sysinfo->run_flag<7){
		sysinfo->run_flag = 7;
		mux_thread_pool = create_thread_pool(mux_info.mux_channels, SZ_1M);

		for(mux_chan=0; mux_chan<mux_info.mux_channels; mux_chan++){

			the_first_run_flag[mux_chan] = 1;

			if(mux_channels[mux_chan].enable){
				
				mux_channels[mux_chan].reperform_flag = 1;
				
				task_arg.mask_bit = mux_chan;
				task_arg.arg = (void *)mux_chan;
				thread_pool_add_task(mux_thread_pool, (proc_fun_t)remux, &task_arg);
				//remux(mux_chan);
			}else{
				//the_first_run_flag[mux_chan] = 0;
				disable_fpga_mux_channel(mux_chan);
			}
			usleep(1000);
		}

		thread_pool_destroy(mux_thread_pool);
		//thread_pool_wait(mux_thread_pool);
		for(mux_chan=0; mux_chan<mux_info.mux_channels; mux_chan++){
			the_first_run_flag[mux_chan] = 0;
		}
	}
		
	#endif



	#if 1
		
		update_mux_si_thread_pool = create_thread_pool(mux_info.mux_channels, SZ_1M);
			
		while(1){
			
			for(mux_chan=0; mux_chan<mux_info.mux_channels; mux_chan++){
				//if((mux_channels[mux_chan].enable) && check_mux_status(mux_chan)){

					/*
					if(mux_info.support_crc_monitor){
						task_arg.mask_bit = mux_chan;
						task_arg.arg = (void *)mux_chan;
						thread_pool_add_task(update_mux_si_thread_pool, (proc_fun_t)read_mux_validrate, &task_arg);
					}
					//*/
					read_mux_validrate(mux_chan);
				//}
				usleep(100000);
			}

			#if 1

				if(mux_abnormal_flag == 0){
					
				}else{
					
				}

			#endif
			
			sched_yield();
			sleep(3);
		}
		
		thread_pool_destroy(update_mux_si_thread_pool);
		#else
		check_and_update_mux_si_task();
		#endif

	return (void *)0;
}


void init_mux_fpga(void)
{
	int in_chan;
	int tread_qty;

	task_arg_t task_arg;

		//printf("mux_task thread: start init pid loopup ...\n");
	tread_qty = mux_info.total_tsin_channels;
	if(tread_qty>16){
		tread_qty = 16;
	}

	init_thread_pool = create_thread_pool(tread_qty, SZ_1M);

	task_arg.mask_bit = -1;
	task_arg.arg_size = 4;
	for(in_chan=0; in_chan<mux_info.total_tsin_channels; in_chan++){
		task_arg.arg = (void *)in_chan;
		thread_pool_add_task(init_thread_pool, (proc_fun_t)init_pid_lookup, &task_arg);
		//init_pid_lookup(in_chan,PID_LOOKUP_FILTER_PART);
	}

	
	thread_pool_destroy(init_thread_pool);
	
	//printf("mux_task thread: init pid loopup finished ...\n");
}

void init_mux(int mux_channel_qty, int tsin_channel_qty, int crc_monitor, int support_ca)
{
	mux_info.mux_channels = mux_channel_qty;
	mux_info.total_tsin_channels = tsin_channel_qty;
	mux_info.support_crc_monitor = crc_monitor;
	mux_info.support_ca = support_ca;


	init_mux_channels();
	init_tsin_all_channel();

	read_all_remux_data();

}

int get_mux_valid_bitrate(int channel)
{
	return mux_channels[channel].valid_bitrate;
}

int get_mux_run_status(int channel)
{
	return mux_channels[channel].run_status;
}

void startup_mux_task(void)
{
	pthread_t thread_capture, thread_mux;
	
	//*
	if (pthread_create(&thread_mux,  NULL, mux_task, NULL) != 0){
		printf( "Create thread_mux failure\n");
		exit(1);
	}
	//*/

	//*
	if (pthread_create(&thread_capture,  NULL, capture_task, NULL) != 0){
		printf( "Create thread_capture failure\n");
		exit(1);
	}
	//*/
}

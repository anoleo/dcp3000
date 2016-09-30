 
#include "appref.h"
#include "ipcmsgsrv.h"
//#include "mux/mux.h"
#include "front_panel/front_panel.h"

#define MSG_FRONTPANEL_LED		0x07

#define MSG_SPI_MASTER		0x08
#define MSG_SPI_TRANSFER		0x09

#if 0
#define MSG_TSIN_CAPTURE		0x10
#define MSG_TSIN_CAPTURE_CTRL		0x11
#define MSG_TSIN_CHANNEL_CAPTURE_CTRL		0x12
#define MSG_TSIN_CHANNEL_PRINTTABLE		0x13



#define MSG_REMUX		0x20
#define MSG_REMUX_ENABLE		0x21
#define MSG_REMUX_SETPIDMAPPING		0x22
#define MSG_REMUX_CLRPIDMAPPING		0x23



void tsin_print_table_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	ts_in_channel_t *p_tsin_chan;
	int feedback_value = 0;

	u_int globle_ctl, v_u32;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&i, p_data, 4);

	print_ts_table(&ts_in_channels[i].ts_tables);

}


void tsin_capture_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	ts_in_channel_t *p_tsin_chan;
	int feedback_value = 0;

	u_int globle_ctl, v_u32;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	offset = 0;
	while(offset<data_len){

		memcpy(&i, p_data + offset, 4);
		offset += 4;
		
		set_capture_flag(i, CAPTURE_FLOW_FLAG, CAPTURE_REDO);
		captureTS(i);
		
	}

}

void tsin_capture_control_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	ts_in_channel_t *p_tsin_chan;
	int feedback_value = 0;

	u_int globle_ctl, v_u32;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&globle_ctl, p_data, 4);
	capture_control.auto_capture = (globle_ctl >> 0) & 1;

	save_capture_ctrl();
		
}

void tsin_channel_capture_control_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	ts_in_channel_t *p_tsin_chan;
	int feedback_value = 0;

	u_int globle_ctl, v_u32;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	
	
	offset = 0;
	while(offset<data_len){

		memcpy(&i, p_data + offset, 4);
		offset += 4;
		memcpy(&v_u32, p_data + offset, 4);
		offset += 4;
		
		ts_in_channels[i].dowith_eit_pf = (v_u32>>8)&1;
		ts_in_channels[i].dowith_eit_s = (v_u32>>9)&1;
		ts_in_channels[i].dowith_nit = (v_u32>>10)&1;

		if((v_u32>>0)&1){
			re_captureTs(i);
		}

		save_channel_capture_ctrl(i);

		#if IPCMSG_DEBUG
			//print_bytes(p_mux_parameter, sizeof(mux_parameter));
			printf(" %d  ts_in capture control :   dowith_eit_pf = %#x ,  dowith_eit_s = %#x \n"
			, i, ts_in_channels[i].dowith_eit_pf, ts_in_channels[i].dowith_eit_s);
		#endif

	}

}



void add_mux_section_by_tsin(int mux_chan, u_short pid, section_t * p_section)
{

	printf("add_mux_section_by_tsin : %d mux, %d pid, %d len \n", mux_chan, pid, p_section->len);
	while(p_section){
		add_mux_section(mux_chan, pid, p_section->len, p_section->data);	
		p_section = p_section->next;
	}
}



int add_program_to_mux(int out_chan, int in_chan, program_t *p_program)
{
	int es_count;
	mux_channel_t *p_mux_channel = &mux_channels[out_chan];
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	u_short program_number, espid, estype;
	mux_program_t *p_mux_program;


	if(p_program){

		program_number = p_program->program_number;
		
		printf("add %d tsin %d program to %d mux  \n", in_chan, program_number, out_chan);
	
		p_mux_program = update_program_mapping(out_chan, in_chan, program_number, program_number);
				
		p_mux_program->in_pmt_pid = p_program->pmt_pid;
		p_mux_program->mux_pmt_pid = p_program->pmt_pid;
		p_mux_program->in_pcr_pid = p_program->pcr_pid;
		p_mux_program->mux_pcr_pid = p_program->pcr_pid;
		
		memcpy(&p_mux_program->service_info, &p_program->service_info, sizeof(service_info_t));
		

		es_count = 0;
		while(es_count < ES_PID_QTY_IN_A_PROGRAM){
			espid = (p_program->es_pid[es_count] & 0x1fff);
			if(espid == 0x1fff){
				break;
			}
			estype = (p_program->es_pid[es_count]>>16)&0xff;
			p_mux_program->es_pid_mapping[es_count][0] = espid;
			p_mux_program->es_pid_mapping[es_count][1] = espid;
			p_mux_program->es_pid_mapping[es_count][2] = estype;
			es_count++;
		}
	
		return 0;
	}

}



void remux_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	ts_in_channel_t *p_tsin_chan;
	int feedback_value = 0;

	u_int  v_u32;
	u_int  total_bitrate;
	ts_in_table_t *p_ts_table;
	mux_channel_t *p_mux_channel;
	mux_section_t *p_mux_section;
	int es_count;
	int in_chan, mux_chan;
	pmt_table_t *p_pmt;
	program_t *p_program;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&mux_chan, p_data + 0, 4);
	memcpy(&in_chan, p_data + 4, 4);
	memcpy(&total_bitrate, p_data + 8, 4);
	memcpy(&v_u32, p_data + 12, 4);

	if((in_chan>=mux_info.total_tsin_channels) || (mux_chan>=mux_info.mux_channels)){
				return -1;
		}
		
		p_ts_table = &ts_in_channels[in_chan].ts_tables;	
		p_mux_channel = &mux_channels[mux_chan];

		
		p_mux_channel->total_bitrate = total_bitrate;

		p_mux_channel->reperform_flag = 1;
		p_mux_channel->enable = 1;
		p_mux_channel->program_count = 0;


		
		p_mux_channel->free_CA_mode = v_u32&(1<<0);
		p_mux_channel->insert_sdt_flag = v_u32&(1<<1);
		p_mux_channel->insert_nit_flag = v_u32&(1<<2);
		p_mux_channel->insert_eit_flag = v_u32&(1<<3);
		

		printf("ts  mux : mux_chan %d, in_chan %d, total_bitrate = %d \n", mux_chan, in_chan, p_mux_channel->total_bitrate);

		if(p_ts_table->table_valid){

			p_pmt = p_ts_table->pmt_tables;
			while(p_pmt){
				
				p_program = p_pmt->p_program;
				while(p_program){
					add_program_to_mux(mux_chan, in_chan, p_program);
					p_program = p_program->next;
				}
				
				p_pmt = p_pmt->next;
			}
		
			remux(mux_chan);

			reset_mux_programs(mux_chan);

		}
	
}


void remux_enable_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	int feedback_value = 0;

	u_int globle_ctl, v_u32;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	
	memcpy(&i, p_data + 0, 4);
	memcpy(&v_u32, p_data + 4, 4);

	if(v_u32){
		enable_fpga_mux_channel(i);
	}else{
		disable_fpga_mux_channel(i);
	}
		

}


	
void remux_setpidmapping_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	int feedback_value = 0;

	u_int globle_ctl, v_u32;
	int out_chan, in_chan, in_pid, mux_pid;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&out_chan, p_data + 0, 4);
	memcpy(&in_chan, p_data + 4, 4);
	memcpy(&in_pid, p_data + 8, 4);
	memcpy(&mux_pid, p_data + 12, 4);

	write_mapping_pid(out_chan, in_chan, in_pid, mux_pid);


}

	
void remux_clrpidmapping_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	int feedback_value = 0;

	u_int globle_ctl, v_u32;

	int out_chan, in_chan, in_pid, mux_pid;

	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&out_chan, p_data + 0, 4);
	memcpy(&in_chan, p_data + 4, 4);
	memcpy(&in_pid, p_data + 8, 4);

	clean_mapping_pid(out_chan, in_chan, in_pid);
	
}
#endif

void spi_master_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	int feedback_value = 0;

	u_int  v_u32;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&i, p_data, 4);
	memcpy(&v_u32, p_data+4, 4);

	if(v_u32 == 1){
		start_spi_transfer_task(i);
	}else if(v_u32 == 2){
		end_spi_transfer_task(i);
	}else if(v_u32 == 3){
		print_spi_status(i);
	}

}

	
void spi_transfer_task_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	int feedback_value = 0;

	u_int v_u32;
	
	spitask_st *p_spitask;
	int slot,  addr, transferlen, transfer_mode;
	char *transferdata;

	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&slot, p_data + 0, 4);
	memcpy(&addr, p_data + 4, 4);
	memcpy(&transfer_mode, p_data + 8, 4);
	memcpy(&transferlen, p_data + 12, 4);
	if(transferlen>0){
		transferdata = p_data+16;
	}else{
		transferdata = NULL;
	}
	if(addr == SLAVER_SPI_MESSAGE_ADDR){
		transfer_mode |= 0x10;
	}

	if(transfer_mode&0x10000){
		p_spitask = insert_spi_transfer_task(slot, addr, transferdata, transferlen, transfer_mode);
	}else{
		p_spitask = add_spi_transfer_task(slot, addr, transferdata, transferlen, transfer_mode);
	}

	if(p_spitask){
		if(wait_spi_transfer_end(p_spitask) == 0){
			feedback_value = 1;
		}else{
			feedback_value = 0;
		}
		free(p_spitask);
	}else{
		feedback_value = 0;
	}

	
	if(transfer_mode & 2){
		msg_buf->msglen = 4;
		msg_buf->msgdata = &feedback_value;
	}else{
		if(feedback_value){
			msg_buf->msglen = p_spitask->len;
			msg_buf->msgdata = p_spitask->data;
		}else{
			msg_buf->msglen = 0;
			msg_buf->msgdata = NULL;
		}
	}

	shmem_msg_send(p_shm_msg, msg_buf, SHM_WRITE_TIME_OUT_MS);
	
}

void frontpanel_led_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{
	int i;

	char *p_data;
	int data_len, offset = 0;
	
	int color;


	if(p_shm_msg == NULL || msg_buf == NULL){
		return ;
	}

	p_data = msg_buf->msgdata;
	data_len = msg_buf->msglen;

	memcpy(&i, p_data, 4);
	memcpy(&color, p_data+4, 4);

	
	if(i < 8){
		sysinfo->slot_status[i] = color;
		Extend_Led_Set(i, color);
	}else if(i == 9){
		Led_Warning_Set(color);
	}else if(i == 10){
		Led_Alarm_Set(color);
	}

}


void shmmsg_task_handler(shmem_message_t *p_shm_msg, msg_buf_t *msg_buf)
{


	char *msg_str;

	/*
	printf("shm msg rcv : msgid = %d \n", msg_buf->msgid);
	printf("shm msg rcv : program_id = %d \n", msg_buf->program_id);
	printf("shm msg rcv : process_id = %d \n", msg_buf->process_id);
	printf("shm msg rcv : msglen = %d \n", msg_buf->msglen);
	print_bytes(msg_buf->msgdata, msg_buf->msglen);
	//*/	


	switch(msg_buf->msgid){
		case MSG_FRONTPANEL_LED:
			frontpanel_led_handler(p_shm_msg, msg_buf);
			break;
		case MSG_SPI_MASTER:
			spi_master_handler(p_shm_msg, msg_buf);
			break;
		case MSG_SPI_TRANSFER:
			spi_transfer_task_handler(p_shm_msg, msg_buf);
			break;
		/*
		case MSG_TSIN_CAPTURE:
			tsin_capture_handler(p_shm_msg, msg_buf);
			break;
		case MSG_TSIN_CAPTURE_CTRL:
			tsin_capture_control_handler(p_shm_msg, msg_buf);
			break;
		case MSG_TSIN_CHANNEL_CAPTURE_CTRL:
			tsin_channel_capture_control_handler(p_shm_msg, msg_buf);
			break;

		case MSG_TSIN_CHANNEL_PRINTTABLE :
			tsin_print_table_handler(p_shm_msg, msg_buf);
			break;
		
		case MSG_REMUX:
			remux_handler(p_shm_msg, msg_buf);
			break;
		case MSG_REMUX_ENABLE:
			remux_enable_handler(p_shm_msg, msg_buf);
			break;
		case MSG_REMUX_SETPIDMAPPING:
			remux_setpidmapping_handler(p_shm_msg, msg_buf);
			break;
		case MSG_REMUX_CLRPIDMAPPING:
			remux_clrpidmapping_handler(p_shm_msg, msg_buf);
			break;
		//*/
		default :
			break;
	}

	/*
	msg_buf->msglen = strlen(msg_str);
	msg_buf->msgdata = msg_str;
	shmem_msg_send(p_shm_msg, msg_buf, SHM_WRITE_TIME_OUT_MS);
	//*/

}



void *ipcmsgsrv_task(void *arg)
{

	
	int tread_num = 4;
	shm_task_mng_t *p_shm_task_mng;
	shmem_obj_t *p_shmem;



	printf("ipcmsgsrv_task thread: start ...... \n");


	//char *shm_name = "my_shm";
	p_shmem = shmem_init(723, IPCMSG_BUF_SIZE, SHM_SYSTEMV_IF);
	p_shm_task_mng = init_shm_task_mng(tread_num, shmmsg_task_handler);



	shm_task_startup(p_shm_task_mng, p_shmem, MYSELF_SHMEM_PROGRAM_ID);


	free_shm_task_mng(p_shm_task_mng);
	shmem_destroy(p_shmem);
	
	return arg;		
}




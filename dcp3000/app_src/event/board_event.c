
#include "../appref.h"
#include "board_event.h"

event_logs_st board_event_logs[MAX_SLOT_QTY];



void init_board_logs(int slot)
{

	event_logs_st *p_eventlogs;

	
	printf("init %d board event logs ...\n", slot);
	
	p_eventlogs = &board_event_logs[slot];

	p_eventlogs->max_qty = EVENT_LOG_MAX_NUM;


	p_eventlogs->gottenlogs_start_sn = 0;
	p_eventlogs->gottenlogs_qty = 0;
	
	p_eventlogs->log_count = 0;
	p_eventlogs->log_cursor = 0;
	p_eventlogs->last_sn = 0;

	p_eventlogs->log_list = NULL;

	p_eventlogs->devif_maps = NULL;
	p_eventlogs->functype_maps = NULL;
	p_eventlogs->actiontype_maps = NULL;
		
	pthread_mutex_init(&(p_eventlogs->mutex_lock), NULL); 

	
}

void end_board_logs(int slot)
{

	event_logs_st *p_eventlogs;

	
	p_eventlogs = &board_event_logs[slot];

	hashmap_destroy(p_eventlogs->devif_maps, NULL);
	hashmap_destroy(p_eventlogs->functype_maps, NULL);
	hashmap_destroy(p_eventlogs->actiontype_maps, NULL);

	pthread_mutex_destroy(&(p_eventlogs->mutex_lock));

}


#define SPI_MSG_CLEAN_BOARD_LOGS		0x7f000101
#define SPI_MSG_GET_BOARD_LOGS_BY_SN		0x7f000102
#define SPI_MSG_GET_BOARD_LOGS_WITH_CONDITION		0x7f000103

int clean_board_logs(int slot)
{
	int len;
	int msgid = SPI_MSG_CLEAN_BOARD_LOGS;

	len = spi_send_msg(slot, msgid, NULL, 0);
	if(len < 0){
		return -1;
	}

	return 0;
}

void free_board_logbuf(int slot)
{
	
	event_logs_st *p_eventlogs;

	p_eventlogs = &board_event_logs[slot];

	free(p_eventlogs->log_list);

}

//获取板卡slot 从序号log_sn 开始的count  条日志
int get_board_logs_by_sn(int slot, int log_sn, int count)
{
	int len;
	int msgid = SPI_MSG_GET_BOARD_LOGS_BY_SN; 
	char databuf[100];
	char *rcvmsg = NULL;
	event_logs_st *p_eventlogs;

	printf("---- get %d board event logs by sn ---log_sn = %d, item_qty = %d \n", slot, log_sn, count);
	
	memcpy(databuf, &log_sn, 4);
	memcpy(databuf+4, &count, 4);

	len = spi_rcv_msg(slot, msgid, databuf, 8, &rcvmsg);

	if(len <= 0){
		return -1;
	}


	p_eventlogs = &board_event_logs[slot];

	memcpy(&p_eventlogs->log_count, rcvmsg, 4);
	memcpy(&p_eventlogs->last_sn, rcvmsg+4, 4);
	memcpy(&p_eventlogs->gottenlogs_start_sn, rcvmsg+8, 4);
	memcpy(&p_eventlogs->gottenlogs_qty, rcvmsg+12, 4);

	p_eventlogs->logs_buf = (log_record_st *)(rcvmsg+16);
	p_eventlogs->log_list = (cirArrList_st *)rcvmsg;
	
	//memcpy(log_buf, rcvmsg+16, sizeof(event_logs_st)*p_eventlogs->gottenlogs_qty);

	//destroy_cirArrList(p_eventlogs->log_list);
	//p_eventlogs->log_list = NULL;

	//free(rcvmsg);
	
	return p_eventlogs->gottenlogs_qty;
}

int get_board_logs_with_condition(int slot, int top, time_t s_time, time_t e_time, int event_obj, int action_type, int event_level)
{
	int len;
	int msgid = SPI_MSG_GET_BOARD_LOGS_WITH_CONDITION; 
	u_int v_u32;
	char databuf[100];
	char *rcvmsg;
	event_logs_st *p_eventlogs;

	printf("get %d board logs with condition top to %d \n", slot, top);
	printf("s_time : %#x , e_time : %#x , event_obj %#x , action_type %#x, event_level %d \n"
		, s_time, e_time, event_obj, action_type, event_level);

	memcpy(databuf, &top, 4);
	memcpy(databuf+4, &s_time, 4);
	memcpy(databuf+8, &e_time, 4);
	memcpy(databuf+12, &event_obj, 4);
	memcpy(databuf+16, &action_type, 4);
	memcpy(databuf+20, &event_level, 4);
	
	len = spi_rcv_msg(slot, msgid, databuf, 24, &rcvmsg);

	if(len <= 0){
		return -1;
	}

	p_eventlogs = &board_event_logs[slot];

	memcpy(&p_eventlogs->log_count, rcvmsg, 4);
	memcpy(&p_eventlogs->last_sn, rcvmsg+4, 4);
	memcpy(&p_eventlogs->gottenlogs_start_sn, rcvmsg+8, 4);
	memcpy(&p_eventlogs->gottenlogs_qty, rcvmsg+12, 4);

	p_eventlogs->logs_buf = (log_record_st *)(rcvmsg+16);
	p_eventlogs->log_list = (cirArrList_st *)rcvmsg;
	/*
	destroy_cirArrList(p_eventlogs->log_list);
	p_eventlogs->log_list = create_cirArrList(p_eventlogs->gottenlogs_qty, sizeof(log_record_st) | STRUCT_FLAG);
	cirArrList_add_array(p_eventlogs->log_list, rcvmsg+16, p_eventlogs->gottenlogs_qty);

	free(rcvmsg);
	//*/
	
	return p_eventlogs->gottenlogs_qty;
}

int get_board_irq_events(int slot)
{
	int len;
	spitask_st *p_spitask;

	len = sizeof(board_event_logs[slot].irq_events);

	//memset(board_event_logs[slot].irq_events, 0, len);
	
	p_spitask = add_spi_transfer_task(slot, SLAVER_SPI_GET_IRQ_EVNET_ADDR
		, board_event_logs[slot].irq_events, len, SPI_BLOCK_READ);

	if(p_spitask == NULL){
		printf("get %d board irq_events : insert spi task failed \n", slot);
		return -1;
	}

	if(wait_spi_transfer_end(p_spitask) < 0){
		printf("get board irq_events : wait spi task failed \n");
		len = -1;
	}else{
		InfoPrintf("finished len = %d \n", p_spitask->len);
		len = p_spitask->len;
	}

	free(p_spitask);

	return len;
}

int get_boardlog_text(int slot, log_record_st *p_log, char *str_buf)
{
	static char *event_level_str[] = {
		"Info",
		"normal",
		"Warning",
		"Error"
	};

	char *p_buf, *pstr;
	int len;
	
	int event_level;	// 2bit
	int event_obj;	//8bit, begin with 1
	int action_type;	//8bit, begin with 1
	int obj_port;		// 4bit, begin with 1
	int obj_channel;	//10bit, begin with 1

	event_obj = (p_log->event_code >> 24) & 0xff;
	action_type = (p_log->event_code >> 16) & 0xff;
	event_level = (p_log->event_code >> 14) & 0x3;
	obj_port = (p_log->event_code >> 10) & 0xf;
	obj_channel = (p_log->event_code) & 0x3ff;


	/*
	printf("event_obj = %#x\n", event_obj);
	printf("action_type = %#x\n", action_type);
	printf("obj_port = %#x\n", obj_port);
	printf("obj_channel = %#x\n", obj_channel);
	//*/

	
	p_buf = str_buf;
		
	len = time2str(&p_log->datatime, p_buf);
	//len += strftime(p_buf, 20, "%Y-%m-%d %H:%M:%S", localtime((time_t *)&p_log->datatime));
	
	sprintf(p_buf + len, " [%s] : ", event_level_str[event_level]);

	if(event_obj>0){
		pstr = get_property(board_event_logs[slot].functype_maps, event_obj);
		if(pstr == NULL){
			pstr = get_property(event_logs.functype_maps, event_obj);
		}
		if(pstr){
			sprintf(p_buf+strlen(p_buf), "%s", pstr);
		}else{
			sprintf(p_buf+strlen(p_buf), "%#.2x", event_obj);
		}
		
	}

	if(obj_port>0){
		sprintf(p_buf+strlen(p_buf), "-%d ", obj_port);
	}

	if(obj_channel>0){
		sprintf(p_buf+strlen(p_buf), "Channel-%d ", obj_channel);
	}

	pstr = get_property(board_event_logs[slot].actiontype_maps, action_type);
	if(pstr == NULL){
		pstr = get_property(event_logs.actiontype_maps, action_type);
	}
	if(pstr){
		sprintf(p_buf+strlen(p_buf), "%s ", pstr);
	}else{
		sprintf(p_buf+strlen(p_buf), "%#.2x ", action_type);
	}


	//printf("log_display_str = %s\n", str_buf);

	return strlen(str_buf);
}

void print_board_log(int slot, log_record_st *log_record)
{
	char strbuf[128];
	get_boardlog_text(slot, log_record, strbuf);
	printf("event_code = %#.8x \n", log_record->event_code);
	print_time((time_t)log_record->datatime);
	printf("text : %s \n", strbuf);
}


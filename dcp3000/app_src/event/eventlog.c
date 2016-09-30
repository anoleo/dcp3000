
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <signal.h>

#include <sys/stat.h>

#include "eventlog.h"

#include "../utils/utils.h"
#include "../system.h"



static int log_fd;

event_logs_st event_logs;

#define EVENT_LOG_START_FLAG	0x32f32982

#define logfile_rdlock(fd)		file_rdlock_wait(fd, SEEK_SET, 0, 0)
#define logfile_wrlock(fd)		file_wrlock_wait(fd, SEEK_SET, 0, 0)
#define logfile_unlock(fd)		file_unlock(fd, SEEK_SET, 0, 0)


void init_eventlogs(void)
{
	char log_buf[16];
	u_int start_flag;

	log_fd = open(OPT_DIR"/running.log", O_RDWR |O_CREAT, 0644);

	logfile_wrlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	//print_bytes(log_buf, 16);
	
	event_logs.max_qty = EVENT_LOG_MAX_NUM;
	
	event_logs.gottenlogs_start_sn = 0;
	event_logs.gottenlogs_qty = 0;

	event_logs.log_list = NULL;

	event_logs.devif_maps = NULL;
	event_logs.functype_maps = NULL;
	event_logs.actiontype_maps = NULL;
		
	//pthread_mutex_init(&(event_logs.mutex_lock), NULL); 

	printf("init event logs ...\n");

	
	bcopy(log_buf, &start_flag, 4);

	if(start_flag == EVENT_LOG_START_FLAG){
		bcopy(log_buf+4, &event_logs.last_sn, 4);
		bcopy(log_buf+8, &event_logs.log_count, 4);
		bcopy(log_buf+12, &event_logs.log_cursor, 4);
	}else{
		event_logs.log_count = 0;
		event_logs.log_cursor = 0;
		event_logs.last_sn = 0;
		start_flag = EVENT_LOG_START_FLAG;

		write(log_fd, &start_flag, 4);
		write(log_fd, &event_logs.last_sn, 4);
		write(log_fd, &event_logs.log_count, 4);
		write(log_fd, &event_logs.log_cursor, 4);	
	}

	logfile_unlock(log_fd);
	
	printf("init event_log  last_sn = %u, log_count = %u, log_cursor = %u \n"
		, event_logs.last_sn, event_logs.log_count, event_logs.log_cursor);

	
	

	//add_log(EVENT_CODE(POWER_EVENT_OBJ, EVENT_ACT_ON, 0, 0), 1);
	
}


void clean_logs(void)
{
	
	printf("clean event_logs\n");

	pthread_mutex_lock(&(event_logs.mutex_lock)); 
	
	event_logs.log_count = 0;
	event_logs.log_cursor = 0;
	event_logs.last_sn = 0;

	destroy_cirArrList(event_logs.log_list);
	
	event_logs.log_list = NULL;
	event_logs.logs_buf =  NULL;

	logfile_wrlock(log_fd);
	
	lseek(log_fd, 4, SEEK_SET);

	write(log_fd, &event_logs.last_sn, 4);
	write(log_fd, &event_logs.log_count, 4);
	write(log_fd, &event_logs.log_cursor, 4);	

	logfile_unlock(log_fd);
		
	pthread_mutex_unlock(&(event_logs.mutex_lock)); 


	//add_log(EVENT_CODE(LOG_EVENT_OBJ,EVENT_ACT_CLEAN,0), 1);


}


void end_eventlogs(void)
{
	hashmap_destroy(event_logs.devif_maps, NULL);
	hashmap_destroy(event_logs.functype_maps, NULL);
	hashmap_destroy(event_logs.actiontype_maps, NULL);
	
	destroy_cirArrList(event_logs.log_list);
	pthread_mutex_destroy(&(event_logs.mutex_lock)); 
	close(log_fd);
}

void add_log(u_int event_code, int level)
{
	log_record_st *p_log;
	char log_buf[16];
	static log_record_st log;
	time_t cur_time;

	event_code |= (level&3)<<14;
	
	if(time(&cur_time)<0){
		printf("get time failed, add_log stoped\n");
		return;
	}

	//pthread_mutex_lock(&(event_logs.mutex_lock)); 

	printf("add_log event_code = %#.8x , cur_time = %u\n", event_code, (u_int)cur_time);

	logfile_wrlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	bcopy(log_buf+4, &event_logs.last_sn, 4);
	bcopy(log_buf+8, &event_logs.log_count, 4);
	bcopy(log_buf+12, &event_logs.log_cursor, 4);
		
	event_logs.log_cursor++;
	event_logs.log_count++;
	event_logs.last_sn++;

	if(event_logs.log_count>event_logs.max_qty){
		event_logs.log_count = event_logs.max_qty;
	}
	if(event_logs.log_cursor>event_logs.max_qty){
		event_logs.log_cursor = 1;
	}

	//printf("event_log  last_sn = %u, log_count = %u, log_cursor = %u \n"
	//		, event_logs.last_sn, event_logs.log_count, event_logs.log_cursor);

	log.event_code = event_code;
	log.datatime = cur_time;

	lseek(log_fd, (event_logs.log_cursor-1)*sizeof(log_record_st)+16, SEEK_SET);
	write(log_fd, &log, sizeof(log_record_st));
	
	lseek(log_fd, 4, SEEK_SET);

	write(log_fd, &event_logs.last_sn, 4);
	write(log_fd, &event_logs.log_count, 4);
	write(log_fd, &event_logs.log_cursor, 4);	

	logfile_unlock(log_fd);
	
	//pthread_mutex_unlock(&(event_logs.mutex_lock)); 

}

//获取日志存取区从索引index  开始的count 条日志
int read_logs(log_record_st *logs_buf, int index, int count)
{
	int extra = 0;
	
	if(logs_buf == NULL){
		return -1;
	}

	printf("read logs  index : %d , count = %d \n", index, count);

	if(index < 1){
		index = 1;
	}

		
	if(count > event_logs.log_count){
		count = event_logs.log_count;
	}

	if(count==0){
		printf("read logs  count = %d \n", count);
		return 0;
	}else if(count>1){
		extra = index + count -1 - event_logs.log_count;
		if(extra<0){extra = 0;}
		count -= extra;

		if(event_logs.log_count < event_logs.max_qty){
			extra = 0;
		}
	}

	lseek(log_fd, (index-1)*sizeof(log_record_st)+16, SEEK_SET);
	read(log_fd, logs_buf, sizeof(log_record_st)*count);
	
	if(extra > 0){
		lseek(log_fd, 16, SEEK_SET);
		read(log_fd, logs_buf+count, sizeof(log_record_st)*extra);
	}

	printf("read logs  count = %d ,  extra = %d \n", count, extra);

	return (count+extra);
}

void get_loginfo(void)
{

	char log_buf[16];

	logfile_rdlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	bcopy(log_buf+4, &event_logs.last_sn, 4);
	bcopy(log_buf+8, &event_logs.log_count, 4);
	bcopy(log_buf+12, &event_logs.log_cursor, 4);

	logfile_unlock(log_fd);

	print_bytes(log_buf, 16);
	
	printf("eventlog : last_sn = %d \n", event_logs.last_sn);
	printf("eventlog : log_count = %d \n", event_logs.log_count);
	printf("eventlog : log_cursor = %d \n", event_logs.log_cursor);

}


//获取第index  条日志
log_record_st *get_log(int index)
{
	int start_idx = index;
	static log_record_st log_record;
	char log_buf[16];

	logfile_rdlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	bcopy(log_buf+4, &event_logs.last_sn, 4);
	bcopy(log_buf+8, &event_logs.log_count, 4);
	bcopy(log_buf+12, &event_logs.log_cursor, 4);

	
	if(index > 0 && index <= event_logs.log_count){
	
		//printf("get log index : %d \n", index);
		
		//pthread_mutex_lock(&(event_logs.mutex_lock));

		index = (event_logs.log_cursor + start_idx+event_logs.max_qty-event_logs.log_count) % event_logs.max_qty;

		lseek(log_fd, (index-1)*sizeof(log_record_st)+16, SEEK_SET);
		read(log_fd, &log_record, sizeof(log_record_st));

		logfile_unlock(log_fd);
		
		//pthread_mutex_unlock(&(event_logs.mutex_lock)); 

	}
	printf("get %d log : code = %#.8x , time = %#.8x \n", start_idx, log_record.event_code, log_record.datatime);
	
	return &log_record;
}

//获取从 index 开始的count 条日志,
//如果给定的logs_buf为NULL 则会创建一个环形数组列表,
//用于存放获取的日志记录
int get_logs_asc(log_record_st *logs_buf, int index, int count)
{
	char log_buf[16];
	int start_idx = index;

	
	logfile_rdlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	bcopy(log_buf+4, &event_logs.last_sn, 4);
	bcopy(log_buf+8, &event_logs.log_count, 4);
	bcopy(log_buf+12, &event_logs.log_cursor, 4);
	
	if(index < 1 || index >event_logs.log_count){
		return -1;
	}

	//printf("get logs by asc index : %d , count = %d \n", index, count);
	
	if(count > event_logs.log_count){
		count = event_logs.log_count;
	}
	
	pthread_mutex_lock(&(event_logs.mutex_lock));
	
	index = (event_logs.log_cursor + start_idx+event_logs.max_qty-event_logs.log_count) % event_logs.max_qty;

	destroy_cirArrList(event_logs.log_list);
	
	if(logs_buf == NULL){	
		event_logs.log_list = create_cirArrList(count, sizeof(log_record_st));
		logs_buf = (log_record_st *)event_logs.log_list->databuf;
	}else{
		event_logs.log_list = NULL;
	}
	
	event_logs.logs_buf = logs_buf;
	
	count = read_logs(logs_buf,  index, count);
	if(event_logs.log_list){
		event_logs.log_list->count = count;
	}

	logfile_unlock(log_fd);
	
	//pthread_mutex_unlock(&(event_logs.mutex_lock)); 
	
	return count;
}

//获取从 倒数第index 条开始的count 条日志,
//如果给定的logs_buf为NULL 则会创建一个环形数组列表,
//用于存放获取的日志记录
int get_logs_desc(log_record_st *logs_buf, int index, int count)
{
	
	char log_buf[16];
	int last_th = index;

	logfile_rdlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	bcopy(log_buf+4, &event_logs.last_sn, 4);
	bcopy(log_buf+8, &event_logs.log_count, 4);
	bcopy(log_buf+12, &event_logs.log_cursor, 4);
	
	if(index < 1 || index >event_logs.log_count){
		logfile_unlock(log_fd);
		return -1;
	}

	//printf("get logs by desc index : %d , count = %d \n", index, count);
	
	if(count > event_logs.log_count){
		count = event_logs.log_count;
	}

	//pthread_mutex_lock(&(event_logs.mutex_lock));

	index = (event_logs.max_qty + event_logs.log_cursor - last_th + 1) % event_logs.max_qty;
	
	destroy_cirArrList(event_logs.log_list);
	
	if(logs_buf == NULL){	
		event_logs.log_list = create_cirArrList(count, sizeof(log_record_st));
		logs_buf = (log_record_st *)event_logs.log_list->databuf;
	}else{
		event_logs.log_list = NULL;
	}

	event_logs.logs_buf = logs_buf;
	
	count = read_logs(logs_buf,  index, count);
	if(event_logs.log_list){
		event_logs.log_list->count = count;
	}

	logfile_unlock(log_fd);
	
	//pthread_mutex_unlock(&(event_logs.mutex_lock)); 

	return count;
}

//获取从 日志序号为log_sn 开始的count 条日志，
//如果给定的logs_buf为NULL 则会创建一个环形数组列表,
//用于存放获取的日志记录
int get_logs_by_sn(log_record_st *logs_buf, int log_sn, int count)
{
	char log_buf[16];
	int index, last_th;

	printf("---- get event logs by sn ---log_sn = %d, item_qty = %d \n", log_sn, count);

	if(count <= 0){
		return 0;
	}

	logfile_rdlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	bcopy(log_buf+4, &event_logs.last_sn, 4);
	bcopy(log_buf+8, &event_logs.log_count, 4);
	bcopy(log_buf+12, &event_logs.log_cursor, 4);

	if(count > event_logs.log_count){
		count = event_logs.log_count;
	}

	if(log_sn <= 0){
		last_th = count;
	}else{		
		last_th = event_logs.last_sn - log_sn + 1;
		if(last_th > event_logs.log_count){
			last_th = event_logs.log_count;
		}else if(last_th < 0){
			last_th = 0;
		}

		if(last_th < count){
			count = last_th;
		}
	}

	
	log_sn = event_logs.last_sn - last_th + 1;

	event_logs.gottenlogs_start_sn = log_sn;

	printf("get event logs : start_log_sn = %d, item_qty = %d, last_th = %d \n"
				, log_sn, count, last_th);

	index = (event_logs.max_qty + event_logs.log_cursor - last_th + 1) % event_logs.max_qty;
	
	destroy_cirArrList(event_logs.log_list);
	
	if(logs_buf == NULL){	
		event_logs.log_list = create_cirArrList(count, sizeof(log_record_st));
		logs_buf = (log_record_st *)event_logs.log_list->databuf;
	}else{
		event_logs.log_list = NULL;
	}

	event_logs.logs_buf = logs_buf;
	
	count = read_logs(logs_buf,  index, count);
	if(event_logs.log_list){
		event_logs.log_list->count = count;
	}

	logfile_unlock(log_fd);
	
	event_logs.gottenlogs_qty =  count;
	

	return event_logs.gottenlogs_qty;

}

//按所给条件获取最后top 条日志，其中参数为0 代表这一项没有条件，另外time 为闭区间,
//会创建一个环形数组列表,用于存放获取的日志记录
int get_logs_with_condition(int top, time_t s_time, time_t e_time, char event_obj, char action_type, char event_level)
{
	int i, index, count, last_th;
	static log_record_st logs_buf[1000];
	char log_buf[16];
		
	//pthread_mutex_lock(&(event_logs.mutex_lock));

	logfile_rdlock(log_fd);

	lseek(log_fd, 0, SEEK_SET);
	read(log_fd, log_buf, 16);

	bcopy(log_buf+4, &event_logs.last_sn, 4);
	bcopy(log_buf+8, &event_logs.log_count, 4);
	bcopy(log_buf+12, &event_logs.log_cursor, 4);

	if(top > event_logs.log_count){
		top = event_logs.log_count;
	}else if(top <=0){
		top = 1;
	}

	destroy_cirArrList(event_logs.log_list);	
	event_logs.log_list = create_cirArrList(top, sizeof(log_record_st));
	event_logs.logs_buf = (log_record_st *)event_logs.log_list->databuf;

	for(last_th=top; last_th<=event_logs.log_count; last_th+=top){
				
		index = (event_logs.max_qty + event_logs.log_cursor - last_th + 1) % event_logs.max_qty;

		printf("get logs with condition : index = %d , last_th = %d \n", index, last_th);
	
		count = read_logs(logs_buf,  index, top);

		for(i=count-1; i>=0; i--){

			printf("%#.8x : %#.8x\n", logs_buf[i].event_code, logs_buf[i].datatime);
			
			if(s_time && (difftime(logs_buf[i].datatime, s_time) < 0)){
				continue;
			}

			if(e_time && (difftime(logs_buf[i].datatime, e_time) > 0)){
				continue;
			}

			if(event_obj && (event_obj != ((logs_buf[i].event_code >> 24) & 0xff))){
				continue;
			}

			if(action_type && (action_type != ((logs_buf[i].event_code >> 16) & 0xff))){
				continue;
			}

			if(event_level && (event_level != ((logs_buf[i].event_code >> 14) & 0x3))){
				continue;
			}

			if(cirArrList_insert(event_logs.log_list, &logs_buf[i]) >= top){
				goto End_query;
			}
			
		}


		if(count < top){
			break;
		}

	}

	End_query :
	logfile_unlock(log_fd);
	//pthread_mutex_unlock(&(event_logs.mutex_lock)); 

	event_logs.gottenlogs_qty = cirArrList_size(event_logs.log_list);

	return event_logs.gottenlogs_qty;
}

/*
Severity
Information
Warning
major
Critical
Cleared
//*/
// 获取日志内容的文本
int get_log_text(log_record_st *p_log, char *str_buf)
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

	int event_param;	// 14bit, include obj_port and channel;

	event_obj = (p_log->event_code >> 24) & 0xff;

	if(event_obj=0){
		return -1;
	}
	
	action_type = (p_log->event_code >> 16) & 0xff;
	event_level = (p_log->event_code >> 14) & 0x3;
	if(event_obj&0x80){
		event_param = (p_log->event_code) & 0x3fff;
	}else{
		obj_port = (p_log->event_code >> 10) & 0xf;
		obj_channel = (p_log->event_code) & 0x3ff;
	}


	/*
	printf("\nget_log_event_msg, event_code = %#x\n", event_code);
	printf("event_obj = %#x\n", event_obj);
	printf("action_type = %#x\n", action_type);
	printf("obj_port = %#x\n", obj_port);
	printf("obj_channel = %#x\n", obj_channel);
	//*/

	
	p_buf = str_buf;
		
	len = time2str(&p_log->datatime, p_buf);
	//len = strftime(p_buf, 20, "%Y-%m-%d %H:%M:%S", localtime((time_t *)&p_log->datatime));
	
	sprintf(p_buf + len, " [%s] :", event_level_str[event_level]);

	pstr = get_property(event_logs.functype_maps, event_obj);
	if(pstr){
		sprintf(p_buf+strlen(p_buf), "%s", pstr);
	}else{
		sprintf(p_buf+strlen(p_buf), "%#.2x", event_obj);
	}

	if(event_obj&0x80){
		sprintf(p_buf+strlen(p_buf), " %d", event_param);
	}else{
		if(obj_port>0){
			sprintf(p_buf+strlen(p_buf), "-%d", obj_port);
		}

		if(obj_channel>0){
			sprintf(p_buf+strlen(p_buf), " Channel-%d", obj_channel);
		}
	}


	pstr = get_property(event_logs.actiontype_maps, action_type);
	if(pstr){
		sprintf(p_buf+strlen(p_buf), "%s ", pstr);
	}else{
		sprintf(p_buf+strlen(p_buf), "%#.2x ", action_type);
	}


	//printf("log_display_str = %s\n", str_buf);

	return strlen(str_buf);
}

void print_log(log_record_st *log_record)
{
	char strbuf[128];
	get_log_text(log_record, strbuf);
	printf("event_code = %#.8x \n", log_record->event_code);
	print_time((time_t)log_record->datatime);
	printf("text : %s \n", strbuf);
}

void get_eventlog_text(void)
{
	int i;
	log_record_st *log_record;
	char strbuf[100];


	//cirArrList_print(event_logs.log_list, (void(*)(void *))print_log);

	for(i=0; i<event_logs.log_list->count; i++){
		log_record = (log_record_st *)cirArrList_get(event_logs.log_list, i);
		print_log(log_record);
		//get_log_text(log_record, strbuf);
	}

	printf("get_evnetlog_text end \n");

}

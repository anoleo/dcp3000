#ifndef _EVENT_LOG_H_
#define _EVENT_LOG_H_

#include <sys/types.h>
#include <pthread.h>
#include "../utils/cirArrList.h"
#include "../utils/properties.h"

#define	EVENT_LOG_MAX_NUM	1000000



typedef struct _event_log_record_st{
	u_int	event_code;
	u_int	datatime;
}log_record_st;


typedef struct{
	u_int max_qty;		//最大日志数
	u_int log_cursor;	//当前最后一条日志的索引
	u_int log_count;	//当前日志的总数
	u_int last_sn;	//最后一条日志的序号
	pthread_mutex_t mutex_lock;
	

	hashmap_st	*devif_maps;
	hashmap_st	*functype_maps;
	hashmap_st	*actiontype_maps;
	
	log_record_st *logs_buf;
	int gottenlogs_start_sn;		//日志缓存中日志的开始索引
	int gottenlogs_qty;		//日志缓存中日志的数量
	cirArrList_st *log_list;		//条件查询所得的日志列表
	log_record_st irq_events[10];
}event_logs_st;
extern event_logs_st event_logs;


enum event_level_type_e{
	EVENT_LEVEL_INFO,
	EVENT_LEVEL_ERROR,
	EVENT_LEVEL_NORMAL,
	EVENT_LEVEL_WARNING,
};

#define EVENT_OBJECT(event_code)		((event_code>>24) & 0xff)
#define EVENT_ACTION(event_code)		((event_code>>16) & 0xff)
#define EVENT_LEVEL(event_code)		((event_code>>14) & 3)
#define EVENT_DEVIF(event_code)		((event_code>>10) & 0xf)
#define EVENT_CHANNEL(event_code)		((event_code>>0) & 0x3ff)
#define EVENT_PARAM(event_code)		((event_code>>0) & 0x3fff)

#define  EVENT_CODE(func_type,action_type, devif, channel) \
	(((func_type&0xff)<<24) | ((action_type&0xff)<<16) | ((devif&0xf)<<10) | ((channel&0x3ff)))

#define  EVENTCODE(func_type,action_type, param) \
	(((func_type&0xff)<<24) | ((action_type&0xff)<<16) | ((param&0x3fff)))


void get_loginfo(void);

void init_eventlogs(void);
void end_eventlogs(void);

void clean_logs(void);

void add_log(u_int event_code, int level);
log_record_st *get_log(int index);

int get_logs_asc(log_record_st *log_buf, int index, int count);
int get_logs_desc(log_record_st *log_buf, int index, int count);
int get_logs_by_sn(log_record_st *log_buf, int log_sn, int count);
int get_logs_with_condition(int top, time_t s_time, time_t e_time, char event_obj, char action_type, char event_level);

void print_log(log_record_st *log_record);
int get_log_text(log_record_st *p_log, char *str_buf);

#endif 


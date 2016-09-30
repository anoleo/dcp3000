#ifndef _BOARD_EVENT_LOG_H_
#define _BOARD_EVENT_LOG_H_

#include "eventlog.h"

extern event_logs_st board_event_logs[];

void init_board_logs(int slot);
void end_board_logs(int slot);

int clean_board_logs(int slot);
void free_board_logbuf(int slot);
int get_board_logs_by_sn(int slot, int log_sn, int count);
int get_board_logs_with_condition(int slot, int top, time_t s_time, time_t e_time, int event_obj, int action_type, int event_level);
int get_boardlog_text(int slot, log_record_st *p_log, char *str_buf);
int get_board_irq_events(int slot);
void print_board_log(int slot, log_record_st *log_record);

#endif //LOG_ENUM_H_H



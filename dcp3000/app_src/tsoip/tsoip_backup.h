#ifndef ___TSOIP_BACKUP_H___
#define ___TSOIP_BACKUP_H___

//#include <sys/types.h>
#include "tsoip.h"
#include "../utils/system.h"
#include "../utils/list.h"

typedef struct{
	u_int src_ipaddr[4];
	u_int src_mask;
	int lower_bitrate;
	int upper_bitrate;

	
	u_int bitrate[4];
	u_int channel_lock[4];
	
	
	int src_idx_0;
	int src_idx_1;
	
	int used_src_idx;
	int cur_src_status;
	int main_src_status;
	int backup_status;

	list_st user_list;
}tsoip_in_channel_backup_st;

typedef struct{
	int backup_mode;	// 0 not backup, 1 , 2
	int test_period;		// unit s
	int channel_qty;

	period_task_st periord_task;
	
	tsoip_in_channel_backup_st channel[MAX_TSOIP_IN_CHANNEL_QTY];
}tsoip_in_backup_st;


tsoip_in_backup_st tsoip_in_backup;
///------------------------------

int switch_ipin_src(int in_chan, int src_idx);

void update_tsoip_in_backup_test_period(int time);

void check_tsoip_in_main_src(void);


#define IP_BACKUP_ERR	-1

void monitor_tsoip_in(void);

void init_tsoip_in_backup(void);

void set_ipin_channel_with_backup(int channel);
	

#endif


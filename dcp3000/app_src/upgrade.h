
#ifndef __UPGRADE_H_
#define __UPGRADE_H_


#define	UPGRADE_FILES_DIR		"/var/upgrade"

typedef struct {
	u_char flag;			// 升级
	u_char status;		// 
	u_short progress_ratio;		// ##.##%  * 10000
	int input_value;
	int hasInput;	
	char status_msg[128];		//升级状态实时消息128byte
} upgrade_st;

upgrade_st *upgrade_status;

#define UPGRADE_STATUS_GLOBLE_SHM_OFFSET		512
#define UPGRADE_STATUS_GLOBLE_SHM_SPAN			(140*8)

#define UPGRADE_UNDO		0
#define UPGRADE_PREPARE		1
#define UPGRADE_START		2
#define UPGRADE_OVER		3

#define UPGRADE_NORMAL		1
#define UPGRADE_FILE_ERROR		2
#define UPGRADE_FILE_DONOT_MATCH		3
#define UPGRADE_FILE_VERSION_LOWER		4
#define UPGRADE_FAILED		5
#define UPGRADE_SECCESS		6
#define UPGRADE_EXIT		7


void wait_for_upgrade(void);

int upgrade_slot_board(int slot);
int upgrade_main_board(void);

void upgrade_input_value(int idx, int value);

int do_upgrade(int argc, char *argv[]);

#endif



#ifndef	__SYSTEM_H__
#define	__SYSTEM_H__
 
#include "ipcmsg/shmem_obj.h"
#include "utils/system.h"
#include "utils/thread_pool.h"
#include "utils/duList.h"
#include "utils/arraylist.h"
#include "utils/properties.h"

#include "transfer/spi.h"

#include "board_info.h"

#include "properties_define.h"

#define PRODUCT_NAME	"DCP-3000"
#define LOGIN_USER	"root"
#define LOGIN_PASSWD	"12345"

#define HOME_DIR	"/home/root"
#define OPT_DIR		"/opt"
#define TEMP_DIR	"/tmp"
#define WEBROOT		OPT_DIR"/webroot"
#ifndef DEBUG_IN_X86
#define SUBBOARD_DIR		OPT_DIR"/subboard"
#define DBFILE		OPT_DIR"/pbidhe.db"
#else
#define SUBBOARD_DIR		HOME_DIR"/subboard"
#define DBFILE		HOME_DIR"/pbidhe.db"
#endif

#define ADMIN_PASSWD	757149754	//0x2d21303a

#define MAX_SLOT_QTY	7
#define MAX_FAN_QTY	4
#define MAX_FAN_SPEED	20000	// unit : r/min

#define TEMPORATURE_HIGH		(80*4)
#define TEMPORATURE_LOW		(-10*4)



#define PWR1_TEST_GPIO	(0*32+12)		//gpio0_12
#define PWR2_TEST_GPIO	(0*32+13)		//gpio0_13
#define PWR1_FAN_GPIO	(0*32+14)		//gpio0_14
#define PWR2_FAN_GPIO	(0*32+15)		//gpio0_15

#define RTC_RESET_N_GPIO	(2*32+24)		//gpio2_24
#define BUZZER_GPIO	(3*32+0)		//gpio3_0



#define GLOBAL_SHM_SIZE	0x100000


#define GLOBAL_SHM_NAME		"globle_shm"
#define GLOBAL_SHM_KEY		999

	
#define WATCHDOG_GLOBLE_SHM_OFFSET		0
#define WATCHDOG_GLOBLE_SHM_SPAN			8


#define SYSINFO_GLOBLE_SHM_OFFSET		0x800
#define SYSINFO_GLOBLE_SHM_SPAN			0x400

#define MNG_NETWORK_GLOBLE_SHM_OFFSET		(SYSINFO_GLOBLE_SHM_OFFSET + SYSINFO_GLOBLE_SHM_SPAN)
#define MNG_NETWORK_GLOBLE_SHM_SPAN		(32)

#define TSOIP_NETWORK_GLOBLE_SHM_OFFSET		(MNG_NETWORK_GLOBLE_SHM_OFFSET + MNG_NETWORK_GLOBLE_SHM_SPAN)
#define TSOIP_NETWORK_GLOBLE_SHM_SPAN		(64*2)

#define TSOIP_IN_GLOBLE_SHM_OFFSET		(TSOIP_NETWORK_GLOBLE_SHM_OFFSET + TSOIP_NETWORK_GLOBLE_SHM_SPAN)
#define TSOIP_IN_GLOBLE_SHM_SPAN		(24*512)

#define TSOIP_OUT_GLOBLE_SHM_OFFSET		(TSOIP_IN_GLOBLE_SHM_OFFSET + TSOIP_IN_GLOBLE_SHM_SPAN)
#define TSOIP_OUT_GLOBLE_SHM_SPAN		(28*48)


#define SPIMASTER_GLOBLE_SHM_OFFSET		(TSOIP_OUT_GLOBLE_SHM_OFFSET + TSOIP_OUT_GLOBLE_SHM_SPAN)
#define SPIMASTER_GLOBLE_SHM_SPAN		(8*8)


#define TRANSCEIVER_GLOBLE_SHM_OFFSET		(SPIMASTER_GLOBLE_SHM_OFFSET + SPIMASTER_GLOBLE_SHM_SPAN)
#define TRANSCEIVER_GLOBLE_SHM_SPAN		(24*8)


#define BOARDINFO_GLOBLE_SHM_OFFSET		0x10000
#define BOARDINFO_SPAN			(1024)
#define BOARDINFO_GLOBLE_SHM_SPAN			(BOARDINFO_SPAN*8)

#define TRANSCEIVER_CHANNEL_GLOBLE_SHM_OFFSET		(BOARDINFO_GLOBLE_SHM_OFFSET + BOARDINFO_GLOBLE_SHM_SPAN)
#define TRANSCEIVER_CHANNEL_SPAN			(36)
#define TRANSCEIVER_CHANNEL_GLOBLE_SHM_SPAN			(0x1000*32)

#define TRANSCEIVER_ROUTE_GLOBLE_SHM_OFFSET		(TRANSCEIVER_CHANNEL_GLOBLE_SHM_OFFSET + TRANSCEIVER_CHANNEL_GLOBLE_SHM_SPAN)
#define TRANSCEIVER_ROUTE_SPAN			(24)
#define TRANSCEIVER_ROUTE_GLOBLE_SHM_SPAN			(0x1000*20)

#define TRANSCEIVER_ROUTE_REG_MAP_GLOBLE_SHM_OFFSET		(TRANSCEIVER_ROUTE_GLOBLE_SHM_OFFSET + TRANSCEIVER_ROUTE_GLOBLE_SHM_SPAN)
#define TRANSCEIVER_ROUTE_REG_MAP_GLOBLE_SHM_SPAN		(16*256*2)

#define TRANSCEIVER_TSIN_MAPPING_CHAN_GLOBLE_SHM_OFFSET		(TRANSCEIVER_ROUTE_REG_MAP_GLOBLE_SHM_OFFSET + TRANSCEIVER_ROUTE_REG_MAP_GLOBLE_SHM_SPAN)
#define TRANSCEIVER_TSIN_MAPPING_CHAN_GLOBLE_SHM_SPAN		(1024*2)
#define TRANSCEIVER_TSIN_TRANS_CHAN_GLOBLE_SHM_OFFSET		(TRANSCEIVER_TSIN_MAPPING_CHAN_GLOBLE_SHM_OFFSET + TRANSCEIVER_TSIN_MAPPING_CHAN_GLOBLE_SHM_SPAN)
#define TRANSCEIVER_TSIN_TRANS_CHAN_GLOBLE_SHM_SPAN		(1024*2)

extern u_int *shm_signal_status;

extern shmem_obj_t *global_shm;

#define SLOT_STATUS_NULL		0
#define SLOT_STATUS_ERROR		1
#define SLOT_STATUS_NORMAL	2
#define SLOT_STATUS_WARNING	3

#define 	BOARD_1_SLOT	0
#define 	BOARD_2_SLOT	1
#define 	BOARD_3_SLOT	2
#define 	BOARD_4_SLOT	3
#define 	BOARD_5_SLOT	4
#define 	BOARD_6_SLOT	5
#define 	EXTEND_BOARD_SLOT	6
#define 	MAIN_BOARD_SLOT	7


struct mng_network_param{
	u_int ipaddr;
	u_int netmask;
	u_int old_gw;
	u_int gateway;
	u_int trapip;	//SNMP Trap IP address
	char	macaddr[6];
};


typedef struct{
	u_int dev_type;
	u_int main_version;
	u_int fpga_version;
	u_int app_version;
	u_int firmware_version;
	u_int kernel_version;
	u_int web_version;
}program_version_st;

typedef struct {
	
	char last_link_status;
	char linked_count;
	char unlinked_count;
	char link_changed;
	
	char board_changed;
	char old_param_isvalid;
	char boardinfo_isok;
	char webfile_isok;
	
	char webfile_changed;
	char transceiver_flag;
	
	char last_slot;	//当前板卡上一次的插槽位置
	char run_flag;

	char properties_file_changed;
	char properties_file_isok;
	short boardinfo_len;
	
	int board_type;
	int board_version;
	int web_version;
	int properties_file_version;


	u_char tc_flag;		//  temperature controlled  flag	, 0 off, 1 on
	u_char tc_ratio;		// temperature controlled ratio	0 - 100 percent
	u_char temperature;		// -128 ~ 128 deg.c
	u_char temp_abnormal;		// 0 normal, 1 abnormal


}slotinfo_st;

struct system_info{

	int run_flag;
	int watchdog_enable;
	
	char product_name[20];
	char	device_name[20];
	char	device_sn[20];
	
	char loginuser[20];
	char loginpasswd[20];

	u_int admin_passwd;		// less than billion
	
	char netif_name[20];

	char web_configfile[256];
	
	char fan_qty;
	char fan_tempctrl;		//fan temperature controlled
	short fan_pwm;			
	int fan_speed[MAX_FAN_QTY];		//unit r/min

	int power_qty;
	short power_supply[2];
	
	int arm_temperature;		// unit 0.25 deg.c
	int fpga_temperature;	// unit 0.25 deg.c
	int temperature_high;
	int temperature_low;
	
	u_int firmware_version;
	u_int main_version;
	u_int fpga_version;
	u_int app_version;
	u_int kernel_version;
	u_int web_version;

	int timezone;
	u_int ntp_hostip;
	char ntp_hostname[128];

	u_int ipin_channels;
	u_int ipout_channels;
	u_int tsip_port;
	u_int tsin_channels;
	u_int mux_channels;
	

	int board_qty;
	int transceiver_qty;
	char slot_status[MAX_SLOT_QTY+1];
	slotinfo_st slot_info[MAX_SLOT_QTY+1];
	
	pid_t main_pid;

	pid_t webserver_pid;
	
};

typedef struct system_info sysinfo_st;
typedef struct mng_network_param mng_network_st;

extern program_version_st program_version;
extern sysinfo_st *sysinfo;
extern mng_network_st *mng_network;

extern board_st *main_board;
extern arraylist_st slot_board_list;

int set_manage_network(void);

void open_global_shm(void);
void close_global_shm(void);

int init_sysinfo(void);
void clean_sysinfo(void);

void init_database(void);


int Download_FPGA(char *filename);


void turnon_buzzer(void);
void turnoff_buzzer(void);

void check_power_supply(void);

void set_fans_speed(int pwm);
int read_fans_speed(void);

void set_board_fan_tc(int slot, int flag);
void set_fan_tempctrl(int flag);

int read_temperature(int idx);
void set_temperature_high(int t_high);
void set_temperature_low(int t_low);

void switch_slot_uart(int slot);

void suspend_slot_board(int slot);
void resume_slot_board(int slot);
void reset_slot_board(int slot);
void reset_slot_boards(int slot_mask);

void reset_slot_board_status(int slot);
	
int read_slots_status(void);
void check_slots_status(int slot_mask);


int receive_slot_boardinfo(spitask_st *p_spitask);
int receive_slot_webfile(spitask_st *p_spitask);

int update_slot_boardinfo(int slot);
int update_slot_webfile(int slot);

#define add_boardinfo_spitask(slot)	add_spi_transfer_task(slot, SLAVER_SPI_BOARDINFO_ADDR, NULL, -1, SPI_PROCEDURAL_READ)
#define add_webfile_spitask(slot)		add_spi_transfer_task(slot, SLAVER_SPI_WEBFILE_ADDR, NULL, -1, SPI_PROCEDURAL_READ)

#define get_slot_boardinfo(slot)	receive_slot_boardinfo(add_boardinfo_spitask(slot))
#define get_slot_webfile(slot)		receive_slot_webfile(add_webfile_spitask(slot))
int get_slot_board_properties_files(int slot);


int set_slot_board_slot(int slot);


int set_slot_board_systime(int slot);
void update_all_board_systime(struct tm *rtm);

typedef void (*taskHandler)(void *);

void init_async_task(int thread_qty, int stack_size);
int add_async_task(taskHandler handler, void *arg, int arg_size);

int get_slot_board_summary(int slot);

 void check_slot_in_status(int status, int slot);


#define get_boardinfo(slot)	(board_st *)arraylist_get(&slot_board_list, slot)

void set_timezone(int zone);
int set_ntp_host(char *hostname);
void set_ntp_hostip(u_int hostip);
void sync_board_systime(void);

int test_board_spi(int slot);

int watchdog_open(void);
void watchdog_close(void);
void watchdog_feed(void);
	
int check_ipaddr_network_isSame(void);

#endif

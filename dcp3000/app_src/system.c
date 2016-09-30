

#include <sys/ioctl.h>
#include <netinet/in.h>
//#include <linux/in.h>
#include <linux/if.h>
#include <linux/ip.h>
#include <linux/sockios.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "rtc.h"

#include "appref.h"
#include "sqlite3/sys_sql.h"

shmem_obj_t *global_shm;

program_version_st program_version;
sysinfo_st *sysinfo;
mng_network_st *mng_network;

static thread_pool_t *system_async_task_pool;

board_st *main_board;
arraylist_st slot_board_list;



void init_sim_encrypt(void);


int read_version_file(program_version_st *p_version, char *version_file)
{
	FILE *fp;
	char buf[128];
	char *pch;
	char *separator = "=: \t";
	u_int value;
	int ret;

	//p_printf(bin_pid,"read version file %s \n", version_file);

	fp = fopen(version_file, "r");
	if(fp==NULL){
		perror("read version file");
		return -1;
	}

	while(fgets(buf, 128, fp)){

		pch = strtok(buf, separator);		
		value =  (u_int)strtoul(strtrim(strtok(NULL, separator)), NULL, 16);

		if(strcmp("devtype", strtrim(pch)) == 0){
			p_version->dev_type = value;
		}else if(strcmp("main", strtrim(pch)) == 0){
			p_version->main_version = value;
		}else if(strcmp("app", strtrim(pch)) == 0){
			p_version->app_version = value;
		}else if(strcmp("fpga", strtrim(pch)) == 0){
			p_version->fpga_version = value;
		}else if(strcmp("kernel", strtrim(pch)) == 0){
			p_version->kernel_version = value;
		}else if(strcmp("firmware", strtrim(pch)) == 0){
			p_version->firmware_version= value;
		}else if(strcmp("web", strtrim(pch)) == 0){
			p_version->web_version = value;
		}
		
	}

	fclose(fp);

	return 0;
}

void  print_version_file(program_version_st *p_version, char *prefix)
{
	
	if(p_version==NULL){
		return;
	}

	if(prefix==NULL){
		prefix = "";
	}

	printf("%s device type \t: %#x \n", prefix, p_version->dev_type);
	printf("%s main version \t: %#x \n", prefix, p_version->main_version);
	printf("%s firmware version \t: %#x \n", prefix, p_version->firmware_version);
	printf("%s app version \t: %#x \n", prefix, p_version->app_version);
	printf("%s kernel version \t: %#x \n", prefix, p_version->kernel_version);
	printf("%s fpga version \t: %#x \n", prefix, p_version->fpga_version);
	printf("%s web version \t: %#x \n", prefix, p_version->web_version);
	
}


int check_ipaddr_network_isSame(void)
{
	int res = 0;
	u_int mng_netip, tsoip0_netip, tsoip1_netip;

	mng_netip = mng_network->ipaddr & mng_network->netmask;
	tsoip0_netip = tsoip_network[0].ipaddr &  tsoip_network[0].netmask;
	tsoip1_netip = tsoip_network[1].ipaddr &  tsoip_network[1].netmask;

	/*
	if(mng_netip == mng_network->ipaddr){
		res |= 1;
	}

	if(tsoip0_netip == tsoip_network[0].ipaddr){
		res |= 2;
	}

	if(tsoip1_netip == tsoip_network[1].ipaddr){
		res |= 4;
	}
	
	if(mng_netip != (mng_network->gateway & mng_network->netmask)){
		res |= 0x10;
	}

	if(tsoip0_netip != (tsoip_network[0].gateway & tsoip_network[0].netmask)){
		res |= 0x20;
	}
	
	if(tsoip1_netip != (tsoip_network[1].gateway & tsoip_network[1].netmask)){
		res |= 0x40;
	}
	//*/

	if(mng_netip == tsoip0_netip){
		res |= 0x100;
	}

	if(mng_netip == tsoip1_netip){
		res |= 0x200;
	}

	if(tsoip1_netip == tsoip0_netip){
		res |= 0x400;
	}
	
	return res;
}

int Download_FPGA(char *filename)
{
	int error = 0;
	FILE *FILE_Handle = NULL;
	char databuf[16];
	u_char *fpga_code;
	int code_len;
	int fd;

	if(filename){

		FILE_Handle=fopen(filename,"rb");
		if (FILE_Handle == NULL){
			printf("Can't find the file %s\n", filename);
			return(-1);
		}
		
		fseek(FILE_Handle,0,SEEK_END);
		code_len = ftell(FILE_Handle);
		fseek(FILE_Handle,0,SEEK_SET);
		
		fpga_code = (unsigned char *)xMalloc(code_len);

		fread(fpga_code, code_len, 1, FILE_Handle);

	}else{

		fpga_code = databuf;
		#ifdef USE_NOR_FLASH
		//read_flash(FPGA_CODE_BASE, fpga_code, 8);
		fd = mtd_open("/dev/mtd4");
		if(fd<0){
			return -1;
		}
		mtd_read(fd, 0, fpga_code, 8);

		if((fpga_code[0] != 'F') && (fpga_code[1] != 'P') && (fpga_code[2] != 'G') && (fpga_code[0] != 'A')){
			return -1;
		}
		
		memcpy(&code_len, fpga_code+4, 4);

		if(code_len<=0){
			return -1;
		}

		//printf("fpga code length  = %d [%#.8x]\n", code_len, code_len);
		fpga_code = (unsigned char *)xMalloc(code_len);
	
		//read_flash(FPGA_CODE_BASE+8, fpga_code, code_len);
		mtd_read(fd, 8, fpga_code, code_len);
		mtd_close(fd);
		#else
		printf("the fpga file is null , can't config fpga\n");
		return -1;
		#endif
		//fpga_code += 8;
		
	}
	
	error = FPGA_Config(fpga_code, code_len);
	
	if(error != -1){
		//printf("Config FPGA done!\n");
	}else{
		/*release the buffer*/
		//printf("Config FPGA Failure error=%x\n", error);
	}

	if(FILE_Handle && fpga_code){
		free(fpga_code);
		fclose(FILE_Handle);
		FILE_Handle=NULL;
	}
	
	return error;
}


void init_database(void)
{

	sqlite_Init(DBFILE);

	create_system_table();
}


int set_manage_network(void)
{
	//set_mac_addr(sysinfo.netif_name, mng_network.mac_addr);
	set_ip_addr(sysinfo->netif_name, mng_network->ipaddr, mng_network->netmask);
	//route_del(mng_network.netmask&mng_network.gateway, mng_network.netmask);
	//route_add_gw(mng_network.netmask&mng_network.gateway, mng_network.netmask, mng_network.gateway);
	if(mng_network->old_gw != mng_network->gateway){
		del_default_gw(mng_network->old_gw);
		mng_network->old_gw = mng_network->gateway;
		//save_mngnetwork_gateway(mng_network.gateway);
	}
	
	set_default_gw(mng_network->gateway);

	save_mngnetwork_param();

}


int read_temperature(int idx)
{
	#ifdef USE_TEMP_SENSOR
	return lm73_read_temprature(idx);
	#else
	return 30<<2;
	#endif
}

void set_temperature_high(int t_high)
{

	#ifdef USE_TEMP_SENSOR
	lm73_set_t_high(0, t_high);
	lm73_set_t_high(1, t_high);
	#else
	return;
	#endif
	sysinfo->temperature_high = t_high;
	printf("set_temperature_high = %d [ %#x ]", t_high, t_high);
	save_temperature_high(t_high);
}

void set_temperature_low(int t_low)
{

	#ifdef USE_TEMP_SENSOR
	lm73_set_t_low(0, t_low);
	lm73_set_t_low(1, t_low);
	#else
	return;
	#endif
	sysinfo->temperature_low = t_low;
	printf("set_temperature_low = %d [ %#x ]", t_low, t_low);
	save_temperature_low(t_low);
}

void turnon_buzzer(void)
{
	gpio_set(BUZZER_GPIO);
	printf("turnon_buzzer\n");
}

void turnoff_buzzer(void)
{
	gpio_clean(BUZZER_GPIO);
	printf("turnoff_buzzer\n");
}

void check_power_supply(void)
{
	int status, val;

	//left is No.0 , right is No.1
	
	sysinfo->power_supply[0] = gpio_get(PWR1_TEST_GPIO);
	sysinfo->power_supply[1] = gpio_get(PWR2_TEST_GPIO);
	//printf("check_power_supply : %#x \n", sysinfo.power_supply[0]);
	//printf("check_power_supply : %#x \n", sysinfo.power_supply[1]);

}

int check_power_fan(void)
{
	int status, val;
	printf("----check_power_fan-----\n");
	
	status = gpio_get(PWR1_FAN_GPIO);
	printf("check_power_fan : %#x \n", status);
	val = status;
	status = gpio_get(PWR2_FAN_GPIO);

	printf("check_power_fan : %#x \n", status);
	status = (status << 1) + val;
	return status;
}

void set_board_fan_tc(int slot, int flag)
{
	if(sysinfo->slot_info[slot].run_flag == 0){
		return;
	}
	if(sysinfo->slot_info[slot].tc_flag!=flag){
		sysinfo->slot_info[slot].tc_flag = flag;
		save_temperature_control_flag(slot, flag);
	}
}

void set_fan_tempctrl(int flag)
{

	if(sysinfo->fan_tempctrl != flag){
		sysinfo->fan_tempctrl = flag;
		save_fan_temperature_control(flag);
	}

	printf("set fan temperature contol switch : %d \n", flag);
}


void set_fans_speed(int pwm)
{
	int i;
	
	//theoretical value : max speed 18000 r/min (300 r/s)
	//field test : pwm max 2015(0x7DF), pulse  582Hz, speed 17460 r/min (291 r/s)
	pwm /= 10;
	if(sysinfo->fan_pwm != pwm){
		sysinfo->fan_pwm = pwm;
		FPGA_ARMIF_WRITE(REG_FPGA_FANS_PWM, pwm);
	}
	
	//printf("set_fans_speed : %d \n", pwm);
	for(i=0; i<=sysinfo->board_qty; i++){
		if(sysinfo->slot_info[i].tc_flag){
			return;
		}
	}
	
	save_fan_pwm(pwm);
}


/*
0x93 [15:0]  R  fan1_status
0x94 [15:0]  R  fan2_status
0x95 [15:0]  R  fan3_status
0x96 [15:0]  R  fan4_status
 风扇的频率反馈，以风扇的反馈脉冲信号进行频率计数。
//*/
int read_fans_speed(void)
{
	int i, speed;

	//fan number is in order From right to left

	for(i=0; i<sysinfo->fan_qty; i++){
		sysinfo->fan_speed[i] = FPGA_ARMIF_READ(REG_FPGA_FAN1_STATUS+i) * 30;
	}

	/*
	sysinfo.fan_speed[1] = FPGA_ARMIF_READ(REG_FPGA_FAN2_STATUS) * 30;
	sysinfo.fan_speed[2] = FPGA_ARMIF_READ(REG_FPGA_FAN3_STATUS) * 30;
	//*/

	/*
	printf("\n\nfan 0 : speed = %d , fan 1 : speed = %d, fan 2 : speed = %d \n"
		, sysinfo.fan_speed[0], sysinfo.fan_speed[1], sysinfo.fan_speed[2]);
	//*/

	//printf("fan 0 : speed = %d r/s  [%d r/min]\n", sysinfo.fan_speed[0]/2, sysinfo.fan_speed[0]*60/2);
	//printf("fan 1 : speed = %d r/s  [%d r/min]\n", sysinfo.fan_speed[1]/2, sysinfo.fan_speed[1]*60/2);
	//printf("fan 2 : speed = %d r/s  [%d r/min]\n", sysinfo.fan_speed[2]/2, sysinfo.fan_speed[2]*60/2);


	return speed;
}

void switch_slot_uart(int slot)
{
	FPGA_ARMIF_WRITE(REG_FPGA_SLOT_UART_CONFIG, slot);
	printf("switch_slot_uart : %d \n", slot);
}


void reset_slot_board_status(int slot)
{
	sysinfo->slot_status[slot] = SLOT_STATUS_NULL;
	sysinfo->slot_info[slot].run_flag = 0;
	sysinfo->slot_info[slot].last_link_status = 0;
	sysinfo->slot_info[slot].linked_count = 0;
	sysinfo->slot_info[slot].unlinked_count = 0;
	pause_spi_transfer_task(slot);
	disable_transceiver(slot);
}

static u_int slot_reset_control_mask = 0xffff;

void suspend_slot_board(int slot)
{

	slot_reset_control_mask &=  ~(1<<slot);
	FPGA_ARMIF_WRITE(REG_FPGA_SLOT_RESET_CONTROL, slot_reset_control_mask);

	usleep(200000);
	
	printf("suspend %d slot board \n", slot);

	reset_slot_board_status(slot);
}

void resume_slot_board(int slot)
{
	
	slot_reset_control_mask |=  (1<<slot);
	FPGA_ARMIF_WRITE(REG_FPGA_SLOT_RESET_CONTROL, slot_reset_control_mask);
	
	printf("resume %d slot board \n", slot);

}

void reset_slot_board(int slot)
{
	suspend_slot_board(slot);
	resume_slot_board(slot);
	
	add_log(EVENTCODE(FUNC_TYPE_CARD_SLOT, ACT_TYPE_RESET, slot+1), EVENT_LEVEL_INFO);	
	printf("reset_slot_board : %d \n", slot);

}

void reset_slot_boards(int slot_mask)
{
	int i;

	FPGA_ARMIF_WRITE(REG_FPGA_SLOT_RESET_CONTROL, ~(slot_mask));
	usleep(200000);
	FPGA_ARMIF_WRITE(REG_FPGA_SLOT_RESET_CONTROL, 0xffff);
	
	for(i=0; i<sysinfo->board_qty; i++){
		reset_slot_board_status(i);
		add_log(EVENTCODE(FUNC_TYPE_CARD_SLOT, ACT_TYPE_RESET, i+1), EVENT_LEVEL_INFO);
	}
	
	printf("reset_slot_boards : %#x \n", slot_mask);
	
}



 void check_slot_in_status(int status, int slot)
{
	int i = slot;
	
#define SLOT_LINK_CHANGED_CHECK_TIMES	1
	
	/*
		printf("%d slot : last_link_status = %d,  slot_status = %d, linked_count = %d, unlinked_count = %d \n", i
			, sysinfo.slot_info[i].last_link_status,sysinfo.slot_status[i], sysinfo.slot_info[i].linked_count, sysinfo.slot_info[i].unlinked_count);
		//*/
		
		if(check_maskbit(&status, i)){
			
			if(sysinfo->slot_info[i].last_link_status){
				//printf("%d slot : -------------last_status is linked-----------\n", i);
				if(!sysinfo->slot_status[i]){
					sysinfo->slot_info[i].linked_count++;
					//printf("%d slot : -------------linked_count = %d-----------\n", i, sysinfo.slot_info[i].linked_count);
				}
			}else{
				sysinfo->slot_info[i].linked_count = 1;
				//printf("%d slot : -------------linked_count = 0-----------\n", i);
			}
			
			if((!sysinfo->slot_status[i]) && (sysinfo->slot_info[i].linked_count > SLOT_LINK_CHANGED_CHECK_TIMES)){
				sysinfo->slot_status[i] = SLOT_STATUS_WARNING;
				sysinfo->slot_info[i].link_changed = 1;
				sysinfo->slot_info[i].linked_count = 0;
				printf("%d slot : -------------link_changed  and linked-----------\n", i);
			}
			
			sysinfo->slot_info[i].last_link_status = 1;
		}else{
			
			if(!sysinfo->slot_info[i].last_link_status){
				//printf("%d slot : =============last_status  is unlinked===========\n", i);
				if(sysinfo->slot_status[i]){
					sysinfo->slot_info[i].unlinked_count++;
					//printf("%d slot : =============unlinked_count = %d============\n", i, sysinfo.slot_info[i].unlinked_count);
				}
			}else{
				sysinfo->slot_info[i].unlinked_count = 1;
				//printf("%d slot : =============unlinked_count = 0============\n", i);
			}
			
			if((sysinfo->slot_status[i]) && (sysinfo->slot_info[i].unlinked_count >SLOT_LINK_CHANGED_CHECK_TIMES)){
				sysinfo->slot_info[i].link_changed = 1;
				sysinfo->slot_status[i] = SLOT_STATUS_NULL;
				sysinfo->slot_info[i].unlinked_count = 0;
				printf("%d slot : =============link_changed  and unlinked===========\n", i);
			}
			
			sysinfo->slot_info[i].last_link_status = 0;
		}
		
}

int read_slots_status(void)
{
	int i, status;


	status = FPGA_ARMIF_READ(REG_FPGA_SLOT_CONNECT_STATUS);

	//printf("\n\nread_slot_status : %#x \n", status);

	//*
	for(i=0; i<sysinfo->board_qty; i++){
		check_slot_in_status(status, i);
	}
	//*/

	return status;
}

void check_slots_status(int slot_mask)
{
	int  i, status;

	status = FPGA_ARMIF_READ(REG_FPGA_SLOT_CONNECT_STATUS);

	for(i=0; i<sysinfo->board_qty; i++){
		if(check_maskbit(&status, i)){
			check_slot_in_status(status, i);
		}
	}

}

void clean_main_board_info(void)
{
	arraylist_remove(&slot_board_list, 7);
	main_board = NULL;
	sysinfo->ipout_channels = 0;
	sysinfo->ipout_channels = 0;
	sysinfo->tsip_port = 0;
	sysinfo->transceiver_qty = 0;
}

int init_main_board_info(char *filename)
{
	int i, j, k;
	int board_len;
	board_st *board;
	funcmod_st *func;
	funcchan_st *fchan;
	devif_st *devif;
	sArrList_st *deviflist;
	sArrList_st *funclist;
	sArrList_st *chanlist;
	

	if(arraylist_get(&slot_board_list, 7)){
		return 0;
	}
	
	board = parse_board_info(filename);
	if(board){

		board_len = fill_board_info_into_buf(board, (char *)main_board);
		free_boardinfo(board);
		
		#ifndef DEBUG_IN_X86
		if(sysinfo->run_flag == 1){
			if(main_board->type != program_version.dev_type){
				printf("device type [%#x] doesn't match , exit process\n", main_board->type);
				return (-1);
			}
			if(main_board->version != program_version.firmware_version){
				printf("device firmware_version [%#x] doesn't match , exit process\n", main_board->version);
				return(-1);
			}
		}
		#endif

		sysinfo->slot_info[7].boardinfo_len = board_len;
		sysinfo->slot_info[7].boardinfo_isok = 1;
		sysinfo->slot_info[7].board_type = main_board->type;
		sysinfo->slot_info[7].board_version = main_board->version;

		deviflist = board_deviflist(main_board);
		funclist = board_funclist(main_board);
	
		for(i=0; i<deviflist->count; i++){
			devif = sArrList_get(deviflist, i);
			if(DEVIF_TYPE_TS_IP == devif->type){
				sysinfo->tsip_port = devif->count;
			}else if(DEVIF_TYPE_TRANSCEIVER == devif->type){
				sysinfo->transceiver_qty = devif->count;
			}
		}
		
		for(i=0; i<funclist->count; i++){
			func = sArrList_get(funclist, i);
			if(FUNC_TYPE_TS_IP_IN == func->type){

				sysinfo->ipin_channels = func->channel_count;
				chanlist = func_chanlist(func);
				for(j=0; j<chanlist->count; j++){
					
					fchan = sArrList_get(chanlist, j);

					if(fchan->type == ONE2MANY_CHANIF){
						tsoip_network[fchan->ifno].inchan_s = fchan->start_index;
						tsoip_network[fchan->ifno].inchan_qty =  fchan->end_index - fchan->start_index + 1;
						printf("%d tsoip in start channel = %d, qty = %d \n", fchan->ifno, tsoip_network[fchan->ifno].inchan_s, tsoip_network[fchan->ifno].inchan_qty);
					}else if(fchan->type == MANY2MANY_CHANIF){
						for(k=fchan->start_ifno; k<=fchan->end_ifno; k++){
							tsoip_network[k].inchan_s = fchan->start_index;
							tsoip_network[k].inchan_qty = fchan->end_index - fchan->start_index + 1;
							printf("%d tsoip in start channel = %d, qty = %d \n", k, tsoip_network[k].inchan_s, tsoip_network[k].inchan_qty);
						}
					}else if(fchan->type == ONE2ONE_CHANIF){
						for(k=fchan->start_ifno; k<=fchan->end_ifno; k++){
							tsoip_network[k].inchan_s = fchan->start_index;
							tsoip_network[k].inchan_qty = 1;
							printf("%d tsoip in start channel = %d, qty = %d \n", k, tsoip_network[k].inchan_s, tsoip_network[k].inchan_qty);
						}
					}

				}
				
			}else if(FUNC_TYPE_TS_IP_OUT == func->type){

				sysinfo->ipout_channels = func->channel_count;

				chanlist = func_chanlist(func);
				for(j=0; j<chanlist->count; j++){
					
					fchan = sArrList_get(chanlist, j);

					if(fchan->type == ONE2MANY_CHANIF){
						tsoip_network[fchan->ifno].outchan_s = fchan->start_index;
						tsoip_network[fchan->ifno].outchan_qty =  fchan->end_index - fchan->start_index + 1;
						printf("%d tsoip out start channel = %d, qty = %d \n", fchan->ifno, tsoip_network[fchan->ifno].outchan_s, tsoip_network[fchan->ifno].outchan_qty);
					}else if(fchan->type == MANY2MANY_CHANIF){
						for(k=fchan->start_ifno; k<=fchan->end_ifno; k++){
							tsoip_network[k].outchan_s = fchan->start_index;
							tsoip_network[k].outchan_qty = fchan->end_index - fchan->start_index + 1;
							printf("%d tsoip out start channel = %d, qty = %d \n", k, tsoip_network[k].outchan_s, tsoip_network[k].outchan_qty);
						}
					}else if(fchan->type == ONE2ONE_CHANIF){
						for(k=fchan->start_ifno; k<=fchan->end_ifno; k++){
							tsoip_network[k].outchan_s = fchan->start_index;
							tsoip_network[k].outchan_qty = 1;
							printf("%d tsoip out start channel = %d, qty = %d \n", k, tsoip_network[k].outchan_s, tsoip_network[k].outchan_qty);
						}
					}
					
				}
			}else if(FUNC_TYPE_TS_IN == func->type){
					sysinfo->tsin_channels = func->channel_count;
			}else if(FUNC_TYPE_REMUX == func->type){
					sysinfo->mux_channels = func->channel_count;
			}
		}
		
		
		arraylist_set(&slot_board_list, sysinfo->board_qty, main_board);

		return 0;
	}else{
		return -1;
	}

}

typedef struct {
	int enable;
	int timeout;	
} watchdog_st;

static watchdog_st *wdt = NULL;

/*
	OMAP Watchdog Timer Rev 0x01: initial timeout 60 sec
	software timeout 50 sec
*/
int watchdog_open(void)
{
	if(wdt){
		wdt->enable = 1;
		if(sysinfo->watchdog_enable == 0){
			sysinfo->watchdog_enable = 1;
			save_watchdog_enable(sysinfo->watchdog_enable);
		}
	}
	return sysinfo->watchdog_enable;
}

void watchdog_close(void)
{
	
	if(wdt){
		wdt->enable = 0;
	}
	if(sysinfo->watchdog_enable){
		sysinfo->watchdog_enable = 0;
		save_watchdog_enable(sysinfo->watchdog_enable);
	}
}

void watchdog_feed(void)
{
	if(wdt){
		wdt->timeout = 0;
	}
}

u_int *shm_signal_status;

void open_global_shm(void)
{

	char *shm_base;
	global_shm = shmem_init(GLOBAL_SHM_KEY, GLOBAL_SHM_SIZE, SHM_SYSTEMV_IF);

	shm_base = global_shm;

	// (global_shm->data_buf - (u_int)global_shm) = 88
	//printf("open_global_shm : %p, %p , %d\n", global_shm, global_shm->data_buf, (global_shm->data_buf - (u_int)global_shm));
	
	wdt = (watchdog_st *)(global_shm->data_buf + WATCHDOG_GLOBLE_SHM_OFFSET);
	
	#ifndef SUBROUTINE
	global_shm->ref_from.process_id = getpid();
	wdt->enable = 0;
	wdt->timeout = 0;
	#endif
	upgrade_status = (upgrade_st *)(shm_base + UPGRADE_STATUS_GLOBLE_SHM_OFFSET);
	
	sysinfo = (sysinfo_st *)(shm_base + SYSINFO_GLOBLE_SHM_OFFSET);
	main_board = (board_st *)(shm_base + BOARDINFO_GLOBLE_SHM_OFFSET);

	
	mng_network = (mng_network_st *)(shm_base + MNG_NETWORK_GLOBLE_SHM_OFFSET);

	tsoip_network = (tsoip_network_params_st *)(shm_base + TSOIP_NETWORK_GLOBLE_SHM_OFFSET);
	
	tsoip_in_channel = (tsoip_in_channel_st *)(shm_base + TSOIP_IN_GLOBLE_SHM_OFFSET);
	tsoip_out_channel = (tsoip_out_channel_st *)(shm_base + TSOIP_OUT_GLOBLE_SHM_OFFSET);
	
	spi_masters = (spi_master_st *)(shm_base + SPIMASTER_GLOBLE_SHM_OFFSET);
	transceiver = (transceiver_st *)(shm_base + TRANSCEIVER_GLOBLE_SHM_OFFSET);


	transceiver_channels = (trans_chan_st *)(shm_base + TRANSCEIVER_CHANNEL_GLOBLE_SHM_OFFSET);
	transceiver_routes = (trans_route_st *)(shm_base + TRANSCEIVER_ROUTE_GLOBLE_SHM_OFFSET);

	transceiver_route_reg_map = (u_short *)(shm_base + TRANSCEIVER_ROUTE_REG_MAP_GLOBLE_SHM_OFFSET);
	tsin_mapping_chan = (u_short *)(shm_base + TRANSCEIVER_TSIN_MAPPING_CHAN_GLOBLE_SHM_OFFSET);
	tsin_trans_chan =  (u_short *)(shm_base + TRANSCEIVER_TSIN_TRANS_CHAN_GLOBLE_SHM_OFFSET);
	
}

void close_global_shm(void)
{
	shmem_destroy(global_shm);
}

void init_workdir(void)
{
	int i;
	char *dir, *subdir;
	char buf[128];
	struct stat stbuf;

	dir = buf;
	sprintf(dir, SUBBOARD_DIR);

	if (stat(dir, &stbuf) == -1) {
		//fprintf(stderr, "stat: can't access %s\n", dir);
		mkdir(dir, 0644);
	}
	
	subdir = dir + strlen(dir);
	
	for(i=0; i<sysinfo->board_qty; i++){
		sprintf(subdir, "/slot%.2d", i);
		if (stat(dir, &stbuf) == -1) {
			fprintf(stderr, "stat: can't access %s\n", dir);
			mkdir(dir, 0644);
		}
	}
	
	
}

void clean_sysinfo(void)
{
	int i;
	
	sysinfo->run_flag = 0;
	
	thread_pool_destroy(system_async_task_pool);

	

	//hashmap_destroy(transceiver_channel_maps, NULL);
		
	arraylist_clean(&slot_board_list);

	free_nif_socket();
	
	//free_transceiver(7);
	
	xmlCleanupParser();
	

}

int init_sysinfo(void)
{
	int i,j;
	int thread_qty = 8;
	int res;
	int board_len;
	board_st *board, *slot_board;
	char filepath[100];
	
	sysinfo->run_flag = 1;
	sysinfo->watchdog_enable = 0;
	
	read_watchdog_enable(&sysinfo->watchdog_enable);
	printf("watchdog enable : %d \n", sysinfo->watchdog_enable);
	
	//*
	xmlInitParser();

	sysinfo->ipin_channels = 0;
	sysinfo->ipout_channels = 0;
	sysinfo->tsip_port = 0;
	sysinfo->tsin_channels = 0;
	sysinfo->mux_channels = 0;

	
	sysinfo->board_qty = MAX_SLOT_QTY;
	
	#ifndef DEBUG_TOOLS_MODE
	
	init_arraylist(&slot_board_list, sysinfo->board_qty+1, DATA_FLAG(ASSIGN_DATA,POINTER_DATA,sizeof(board_st *)));
	
	
	if(init_main_board_info(HOME_DIR"/board_info.xml")<0){		
		xmlCleanupParser();
		return -1;
	}

	#else
	sysinfo->ipin_channels = MAX_TSOIP_IN_CHANNEL_QTY;
	sysinfo->ipout_channels = MAX_TSOIP_OUT_CHANNEL_QTY;
	sysinfo->tsip_port = TSOIP_PORT_QTY;
	sysinfo->transceiver_qty = MAX_TRANSCEIVER_QTY;
	#endif
	
	printf("ipin qty = %d, ipout qty = %d , tsio port = %d \n", sysinfo->ipin_channels, sysinfo->ipout_channels, sysinfo->tsip_port);
	printf("transceiver  interface qty = %d \n", sysinfo->transceiver_qty);


	init_nif_socket();


	if(read_adminpasswd(&sysinfo->admin_passwd)){
		sysinfo->admin_passwd = 100000000;
	}

	init_sim_encrypt();

	strcpy(sysinfo->product_name, PRODUCT_NAME);
	strcpy(sysinfo->device_name, PRODUCT_NAME);
	
	if(read_devsn(sysinfo->device_sn)<0){
		//strcpy(sysinfo.device_sn, "0123456789ABCDEF");
	}

	#ifndef DEBUG_IN_X86
	strcpy(sysinfo->netif_name, "eth0");
	#else
	strcpy(sysinfo->netif_name, "eth2");
	#endif

	printf("sysinfo->netif_name = %s \n", sysinfo->netif_name);
	//mng_network.ipaddr = ip_a2i("10.10.110.93");
	//mng_network.netmask = ip_a2i("255.255.255.0");

	get_nif_ipaddr(sysinfo->netif_name, &mng_network->ipaddr);
	get_nif_netmask(sysinfo->netif_name, &mng_network->netmask);
	
	get_nif_mac(sysinfo->netif_name, mng_network->macaddr);
	
	mng_network->gateway = (mng_network->ipaddr & mng_network->netmask) + 1;
	//printf("mng_network.gateway : %#x \n", mng_network.gateway);
	mng_network->trapip = ip_a2i("10.10.110.91");

	mng_network->old_gw = mng_network->gateway;
	set_default_gw(mng_network->gateway);


	//*
	sysinfo->fan_qty = MAX_FAN_QTY;
	sysinfo->fan_tempctrl = 1;
	sysinfo->fan_pwm = 0;
	for(i=0; i<sysinfo->fan_qty; i++){
		sysinfo->fan_speed[i] = 0;
	}

	sysinfo->power_qty = 2;
	for(i=0; i<sysinfo->power_qty; i++){
		sysinfo->power_supply[i] = 0;
	}

	//*

	for(i=0; i<sysinfo->board_qty; i++){
		#ifdef SPI_MAIN_PROCESS
		init_spi_transfer(i);
		#endif
		//init_board_logs(i);
	}
	//*/
	
	for(i=0; i<=sysinfo->board_qty; i++){
		
		sysinfo->slot_status[i] = 0;
		
		sysinfo->slot_info[i].last_link_status = 0;
		sysinfo->slot_info[i].linked_count = 0;
		sysinfo->slot_info[i].unlinked_count = 0;
		sysinfo->slot_info[i].link_changed = 0;

		sysinfo->slot_info[i].board_type = 0;
		sysinfo->slot_info[i].board_version = 0;
		sysinfo->slot_info[i].web_version = 0;
		sysinfo->slot_info[i].properties_file_version = 0;

		sysinfo->slot_info[i].properties_file_changed = 0;
		sysinfo->slot_info[i].properties_file_isok = 0;
		
		sysinfo->slot_info[i].boardinfo_isok = 0;
		sysinfo->slot_info[i].board_changed = 0;
		sysinfo->slot_info[i].old_param_isvalid = 0;
		sysinfo->slot_info[i].webfile_isok = 0;
		sysinfo->slot_info[i].webfile_changed = 0;
		sysinfo->slot_info[i].transceiver_flag = 0;

		sysinfo->slot_info[i].boardinfo_len = 0;
			
		sysinfo->slot_info[i].last_slot = 0xff;
		sysinfo->slot_info[i].run_flag = 0;

		sysinfo->slot_info[i].tc_flag = 1;
		sysinfo->slot_info[i].tc_ratio = 0;
		sysinfo->slot_info[i].temperature = 0;
		sysinfo->slot_info[i].temp_abnormal = 0;

	}
	//*/

	//transceiver_channel_maps = hashmap_create(INTEGER_KEY, 0);
	memset(transceiver_channels, 0, TRANSCEIVER_CHANNEL_GLOBLE_SHM_SPAN);

	memset(transceiver_route_reg_map, 0, TRANSCEIVER_ROUTE_REG_MAP_GLOBLE_SHM_SPAN);
	memset(tsin_mapping_chan, 0xff, TRANSCEIVER_TSIN_MAPPING_CHAN_GLOBLE_SHM_SPAN);
	memset(tsin_trans_chan, 0xff, TRANSCEIVER_TSIN_TRANS_CHAN_GLOBLE_SHM_SPAN);
	
	memset(spi_masters, 0, SPIMASTER_GLOBLE_SHM_SPAN);
	

	//for(i=0; i<sysinfo.max_transceiver_qty; i++){
	for(i=0; i<=sysinfo->board_qty; i++){
		transceiver[i].run_flag = 0;
		transceiver[i].run_mode = 0;
		
		transceiver[i].tx_bitrate = 0;
		transceiver[i].rx_bitrate = 0;
		transceiver[i].channel_qty = 0;
		transceiver[i].route_qty = 0;

		if(i<6){
			transceiver[i].transif_s = i;
			transceiver[i].transif_qty = 1;
		}else if(i==6){
			transceiver[i].transif_s = i;
			transceiver[i].transif_qty = 3;
		}else if(i==sysinfo->board_qty){
			transceiver[i].transif_s = 9;
			transceiver[i].transif_qty = sysinfo->transceiver_qty - 9;
		}

		/*
		transceiver_mutex_init(&transceiver_cfg[i]);

		init_list(&transceiver_cfg[i].setroute_list, STRUCT_DATA);

		for(j=0; j<=sysinfo->board_qty; j++){
			init_list(&transceiver_cfg[i].sndchan_list[j], STRUCT_DATA);
		}
		//*/
		
		//printf("%d trans_idx = %d \n", i, trans_idx[i]);
	}
	
	//*/

	sysinfo->slot_info[sysinfo->board_qty].boardinfo_isok = 1;
	sysinfo->slot_info[sysinfo->board_qty].webfile_isok = 1;
	sysinfo->slot_info[sysinfo->board_qty].last_slot = MAX_SLOT_QTY;
	sysinfo->slot_info[sysinfo->board_qty].run_flag = 1;
	sysinfo->slot_info[sysinfo->board_qty].transceiver_flag = 1;
	
	#ifndef DEBUG_TOOLS_MODE
	init_transceiver(sysinfo->board_qty);
	//allocate_transceiver(sysinfo->board_qty);
	enable_transceiver(sysinfo->board_qty);
	#endif

	#ifndef DEBUG_TOOLS_MODE
	read_firmware_version(&sysinfo->firmware_version);
	
	if(main_board->version == sysinfo->firmware_version){
		
		sysinfo->slot_info[sysinfo->board_qty].board_changed = 0;
		sysinfo->slot_info[sysinfo->board_qty].old_param_isvalid = 1;

	}else{
		sysinfo->slot_info[sysinfo->board_qty].board_changed = 1;
		sysinfo->firmware_version = main_board->version;
		save_firmware_version(sysinfo->firmware_version);
	}
	#endif
	
	/*
	event_logs.devif_maps = read_properties(NULL, HOME_DIR"/properties/devif.properties", INTEGER_KEY);
	event_logs.functype_maps = read_properties(NULL, HOME_DIR"/properties/functype.properties", INTEGER_KEY);
	event_logs.actiontype_maps = read_properties(NULL, HOME_DIR"/properties/eventaction.properties", INTEGER_KEY);
	//*/
	
	init_async_task(thread_qty, SZ_1M);

	#ifndef DEBUG_IN_X86

	init_workdir();
	/*

	set_multicast_ttl(255);
	set_multicast_loop(1);

	//*/

	#ifndef DEBUG_TOOLS_MODE
	for(i=0; i<sysinfo->board_qty; i++){
		sprintf(filepath, OPT_DIR"/subboard/slot%.2d/board_info.xml", i);
		if(board = parse_board_info(filepath)){
			slot_board = (board_st *)((char *)main_board + BOARDINFO_SPAN*(i+1));
			board_len = fill_board_info_into_buf(board, slot_board);
			free_boardinfo(board);
			slot_board->slot = i;
			arraylist_set(&slot_board_list, i, slot_board);
			sysinfo->slot_info[i].board_version = slot_board->version;
			sysinfo->slot_info[i].board_type = slot_board->type;
			sysinfo->slot_info[i].boardinfo_len = board_len;
			select_slot_webversion(i, &sysinfo->slot_info[i].web_version);
			init_transceiver(i);
			//allocate_transceiver(i);
		}
	}
	#endif
	
	
	#endif
	usleep(100000);

	init_tsoip_networks();
	init_tsoip_in_channels();
	init_tsoip_out_channels();
	init_tsoip_in_backup();
	init_tsoip_env();

	return 0;
}


int receive_slot_boardinfo(spitask_st *p_spitask)
{
	int i, slot, len;
	int fd;
	int board_len;
	u_char *pdata;
	board_st *board, *slot_board;

	char filepath[100], *filename;

	if(p_spitask == NULL){
		return -1;
	}

	if(wait_spi_transfer_end(p_spitask) < 0){
		return -1;
	}
	
	len = p_spitask->len;

	if(len<=0){
		return -1;
	}
	
	pdata = p_spitask->data;
	slot = p_spitask->slot;
	

	//print_bytes(pdata, len);

	
	#if 0
	board = (board_st *)pdata;
	
	sysinfo->slot_info[slot].boardinfo_len = len;

	match_channel_devif(board);
	#else
	sprintf(filepath, SUBBOARD_DIR"/slot%.2d/board_info.xml", slot);
	
	printf("%s, filelen = %d \n", filepath, len);
	
	fd = open(filepath, O_RDWR | O_CREAT|O_TRUNC, 0644);
	if(fd>0){
		write(fd, pdata, len);
		close(fd);
		free(p_spitask->data);
	}else{
		printf("open %d failed \n", filepath);
		free(p_spitask->data);
		return -1;
	}

		
	board = parse_board_info(filepath);
	
	if(board == NULL){
		return -1;
	}
	#endif
	slot_board = (board_st *)((char *)main_board + BOARDINFO_SPAN*(slot+1));
	board_len = fill_board_info_into_buf(board, slot_board);
	free_boardinfo(board);
			
	slot_board->slot = slot;
	sysinfo->slot_info[slot].boardinfo_len = board_len;
	
	arraylist_set(&slot_board_list, slot, slot_board);


	DebugPrintf("receive_slot_boardinfo : %d slot board .......................\n", slot);

	
	
	sysinfo->slot_info[slot].boardinfo_isok = 1;

	
	return len;
}

//子板的web文件需要打包成一个，一次性传输
int receive_slot_webfile(spitask_st *p_spitask)
{
	int i, len, slot;
	char *filedata;
	char filepath[100];
	char tardir[100];
	int fd;


	if(p_spitask == NULL){
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){
		return -1;
	}

	filedata = p_spitask->data;
	len = p_spitask->len;
	
	//printf("get_webfile , file size = %d \n", len);
	if(len <=0){
		return -1;
	}

	slot = p_spitask->slot;
	//slot = 0;

	sprintf(filepath, TEMP_DIR"/web%.2d.tar.gz", slot);
	sprintf(tardir, OPT_DIR"/webroot/slot%.2d/", slot);
	
	fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0666);
	write(fd, filedata, len);
	close(fd);

	//print_bytes(filedata, len);
	
	free(filedata);


	if( access(tardir,F_OK) < 0){
		mkdir(tardir, 0644);
	}

	//sys_cmd("rm -rf "%s*", tardir);
	sys_cmd("tar -xf %s -C %s", filepath, tardir);

	
	unlink(filepath);
	//sys_cmd("rm -f %s", filepath);
	
	sysinfo->slot_info[slot].webfile_isok = 1;

	return 0;
}


int get_slot_board_properties_files(int slot)
{
	int fd;
	int datalen, filelen;
	char *filedata, *databuf;
	char filepath[100], *filename;
	
	
	
	datalen = spi_read_data(slot, SLAVER_SPI_BOARD_PROPERTIES_FILE_ADDR, (void **)&databuf);
	printf("get_slot_board_properties_files : %d slot, len = %d \n", slot, datalen);
	if(datalen<=0){
		printf("get %d slot_board_properties_files : failed \n", slot);
		return -1;
	}

	sprintf(filepath, SUBBOARD_DIR"/slot%.2d/", slot);
	filename = &filepath[strlen(filepath)];

	filedata = databuf;
	
	while(datalen>0){
		memcpy(&filelen, filedata+40, 4);

		strcpy(filename, filedata);
		filedata += 44;
		
		printf("properties_file %s, filelen = %d , datalen = %d\n", filename, filelen, datalen);
		//printf("properties_file filepath %s\n", filepath);
		
		fd = open(filepath, O_RDWR | O_CREAT|O_TRUNC, 0644);
		if(fd>0){
			write(fd, filedata, filelen);
			close(fd);
		}else{
			printf("open %d failed \n", filepath);
		}

		filedata += filelen;
		datalen -= filelen + 44;
	}
	//print_bytes(filedata, len);
	
	free(databuf);

	sysinfo->slot_info[slot].properties_file_isok = 1;

	

	return 0;
}

int update_slot_boardinfo(int slot)
{
	char databuf[20];
	char *pdata;
	int board_type, board_version, web_version, board_slot, properties_file_version;
	board_st *board;
	int i, board_changed = 0;

	add_async_task((taskHandler)set_slot_board_slot, (void *)slot, 4);

	printf("start update %d slot_boardinfo : ----\n", slot);

	pdata = databuf;
	for(i=0; i<3; i++){
		//if(spi_read_block(slot, SLAVER_SPI_BOARD_SUMMARY_ADDR, (void **)&pdata)> 0){
		if(spi_read_block(slot, SLAVER_SPI_BOARD_SUMMARY_ADDR, pdata, 16)> 0){
			goto Next_Step;
		}
	}

	printf("update_slot_boardinfo : read %d slot boardtype failed\n", slot);
	
	return -1;
	
	Next_Step :
		
	memcpy(&board_type, pdata, 4);
	memcpy(&board_version, pdata + 4, 4);
	memcpy(&web_version, pdata + 8, 4);
	memcpy(&board_slot, pdata + 12, 4);
	//memcpy(&properties_file_version, pdata + 16, 4);
	//free(pdata);

	
	printf("get  %d slot, type = %#x, board version = %#x, web version = %#x , board_slot = %d \n"
		, slot, board_type, board_version, web_version, board_slot);

	sysinfo->slot_info[slot].properties_file_changed = 1;
	add_async_task((taskHandler)get_slot_board_properties_files, (void *)slot, 4);

	
	printf("update_slot_boardinfo :  %d slot last case : board_type %#x, board_version = %#x, web_version = %#x \n", slot
		, sysinfo->slot_info[slot].board_type, sysinfo->slot_info[slot].board_version, sysinfo->slot_info[slot].web_version);

	
	

	

	if(board_version!=sysinfo->slot_info[slot].board_version){
		board_changed = 1;
		add_log(EVENTCODE(FUNC_TYPE_CARD_SLOT, ACT_TYPE_VERSION_CHANGED, slot+1), EVENT_LEVEL_INFO);
	}
	
	if((board_type>>8)!=(sysinfo->slot_info[slot].board_type>>8)){
		board_changed = 1;
	}

	sysinfo->slot_info[slot].board_changed = board_changed;

	if(board_changed){
		sysinfo->slot_info[slot].old_param_isvalid = 0;
	}else{
		sysinfo->slot_info[slot].old_param_isvalid = 1;	
	}
	
	if(board_type!=sysinfo->slot_info[slot].board_type){
		board_changed = 1;
		add_log(EVENTCODE(FUNC_TYPE_CARD_SLOT, ACT_TYPE_TYPE_CHANGED, slot+1), EVENT_LEVEL_INFO);
	}

	if(board_changed || (web_version!=sysinfo->slot_info[slot].web_version)){
		sysinfo->slot_info[slot].web_version = web_version;
		sysinfo->slot_info[slot].webfile_changed = 1;
		update_slot_webversion(slot, web_version);
		
	}else{
		sysinfo->slot_info[slot].webfile_changed = 0;
		sysinfo->slot_info[slot].webfile_isok = 1;
		printf("update_slot_boardinfo :  %d slot web version is not changed\n", slot);
	}
	update_slot_webfile(slot);

	if(board_changed){
				
		printf("update_slot_boardinfo :  %d slot is  changed, start to update \n", slot);

		arraylist_remove(&slot_board_list, slot);

		
		sysinfo->slot_info[slot].boardinfo_isok = 0;
		

		if(get_slot_boardinfo(slot) < 0){
			sysinfo->slot_status[slot] = SLOT_STATUS_ERROR;
			printf("get %d slot boardinfo error \n", slot);
			return -1;
		}
		
		sysinfo->slot_info[slot].board_type = board_type;
		sysinfo->slot_info[slot].board_version = board_version;

		printf("update_slot_boardinfo :  %d slot boardinfo success \n", slot);
	
	}else{
		sysinfo->slot_info[slot].board_changed = 0;
		sysinfo->slot_info[slot].boardinfo_isok = 1;
		sysinfo->slot_info[slot].old_param_isvalid = 1;
		
		printf("update_slot_boardinfo :  %d slot boardtype and version both are not changed\n", slot);
	}
	
	
	return 0;
}

int update_slot_webfile(int slot)
{
	
	printf("start to update %d slot webfile \n", slot);


	if(get_slot_webfile(slot) < 0){
		sysinfo->slot_status[slot] = SLOT_STATUS_ERROR;
		return -1;
	}

	//sysinfo.slot_info[slot].webfile_changed = 0;
	
	printf("update %d slot webfile success \n", slot);
	
	return 0;
}

int set_slot_board_slot(int slot)
{
	return spi_write_block(slot, SLAVER_SPI_BOARD_SLOT_ADDR, &slot, 4);
}

int test_board_spi(int slot)
{
	char databuf[20];

	set_spi_runflag(slot, RUNNING_FLAG);
	
	while(spi_read_block(slot, SLAVER_SPI_BOARD_SUMMARY_ADDR, databuf, 16)<= 0){
		usleep(1000);
	}

	if(sysinfo->slot_info[slot].run_flag == 1){
		sysinfo->slot_info[slot].run_flag = 2;
		sysinfo->slot_status[slot] = SLOT_STATUS_NORMAL;
		set_slot_board_systime(slot);
		printf("set %d slot board_systime  , run_flag = %d\n", slot, sysinfo->slot_info[slot].run_flag);
	}
	

	return 0;
}


int set_slot_board_systime(int slot)
{
	struct tm *ptm;
	ptm = get_systime();
	ptm->tm_sec += 1;
		
	#if 1
	spi_write_block(slot, SLAVER_SPI_SYSTIME_ADDR, ptm, sizeof(struct tm));
	#else
	while(spi_write_block(slot, SLAVER_SPI_SYSTIME_ADDR, (void *)get_systime(), sizeof(struct tm))< 0){
		usleep(1000);
	}

	if(sysinfo->slot_info[slot].run_flag == 1){
		sysinfo->slot_info[slot].run_flag = 2;
		sysinfo->slot_status[slot] = SLOT_STATUS_NORMAL;
		printf("set %d slot board_systime  , run_flag = %d\n", slot, sysinfo->slot_info[slot].run_flag);
	}
	#endif

	return 0;
}


void set_timezone(int zone)
{
	sysinfo->timezone = zone;
	save_timezone(sysinfo->timezone);
}

void set_ntp_hostip(u_int hostip)
{
	sysinfo->ntp_hostip = hostip;
	save_ntp_hostip(sysinfo->ntp_hostip);
}

int set_ntp_host(char *hostname)
{
	
	if(get_hostip(&sysinfo->ntp_hostip, hostname)<0){
		return 0;
	}

	strcpy(sysinfo->ntp_hostname, hostname);
	save_ntp_hostname(hostname);
	save_ntp_hostip(sysinfo->ntp_hostip);
}

void update_all_board_systime(struct tm *ptm)
{
	int slot;

	if(set_rtc_datetime(ptm)<0){
		return;
	}

	for(slot=0; slot<sysinfo->board_qty; slot++){
		ptm = get_systime();
		ptm->tm_sec += 1;
		//add_async_task((taskHandler)set_slot_board_systime, (void *)slot, 4);
		//add_spi_transfer_task(slot, SLAVER_SPI_SYSTIME_ADDR, (void *)ptm, sizeof(struct tm), SPI_BLOCK_WRITE);
		spi_write_block(slot, SLAVER_SPI_SYSTIME_ADDR, ptm, sizeof(struct tm));
	}
}

void sync_board_systime(void)
{
	int slot;
	struct tm *ptm;

	sync_ntp(sysinfo->ntp_hostip, sysinfo->timezone);
	
	if(set_rtc_datetime(NULL)<0){
		return;
	}

	for(slot=0; slot<sysinfo->board_qty; slot++){
		ptm= get_systime();
		ptm->tm_sec += 1;
		//add_async_task((taskHandler)set_slot_board_systime, (void *)slot, 4);
		//add_spi_transfer_task(slot, SLAVER_SPI_SYSTIME_ADDR, (void *)rtm, sizeof(struct tm), SPI_BLOCK_WRITE);
		spi_write_block(slot, SLAVER_SPI_SYSTIME_ADDR, ptm, sizeof(struct tm));
	}
}

int get_slot_board_summary(int slot)
{
	char databuf[20];
	char *pdata;
	int datalen;
	
	int board_type, board_version, web_version, board_slot;

	pdata = databuf;
	datalen = spi_read_block(slot, SLAVER_SPI_BOARD_SUMMARY_ADDR, pdata, 16);
	//datalen = spi_read_data(slot, SLAVER_SPI_BOARD_SUMMARY_ADDR, (void **)&pdata);
	printf("get_slot_board_summary : %d slot, len = %d \n", slot, datalen);
	if(datalen<=0){
		printf("get %d get_slot_board_summary : failed \n", slot);
		return -1;
	}

	memcpy(&board_type, pdata, 4);
	memcpy(&board_version, pdata + 4, 4);
	memcpy(&web_version, pdata + 8, 4);
	memcpy(&board_slot, pdata + 12, 4);

	printf("get  %d slot, type = %#x, board version = %#x, web version = %#x , board_slot = %d \n"
		, slot, board_type, board_version, web_version, board_slot);

	//free(pdata);

	return 0;
}

void init_async_task(int thread_qty, int stack_size)
{
	
	system_async_task_pool = create_thread_pool(thread_qty, stack_size);
}

int add_async_task(taskHandler handler, void *arg, int arg_size)
{
	int ret;
	task_arg_t taskarg;

	taskarg.mask_bit = -1;
	taskarg.arg = arg;
	taskarg.arg_size = arg_size;

	
	//printf("-------------------add_system_async_task : %p , arg_size %d   ---begin ------------------------\n", handler, arg_size);
	
	ret = thread_pool_add_task(system_async_task_pool, (proc_fun_t)handler, &taskarg);

	//printf("===============add_system_async_task : %p , arg_size %d   ===end ===================\n", handler, arg_size);

	return ret;
}

int add_board_irq_task(taskHandler handler, int slot)
{
	int ret;
	task_arg_t taskarg;

	taskarg.mask_bit = slot;
	taskarg.arg = (void *)slot;
	taskarg.arg_size = 4;
	
	ret = thread_pool_add_task(system_async_task_pool, (proc_fun_t)handler, &taskarg);

}




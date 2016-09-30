
 
#include "appref.h"

#include "web/webserver.h"

#include "rtc.h"


//#include "ipcmsgsrv.h"

#include <execinfo.h>

#include "mux/mux.h"

#define WEB_MAIN_VERSION 0x1018

#if 0

void slot_board_action(int slot)
{
	int i, ret;
	int event_level, event_obj, event_act, event_param;
	log_record_st *plog;
	u_char databuf[20];
	u_int v_u32;
	int v_s32;

	printf("%d slot board action , runflag = %d \n", slot, sysinfo->slot_info[slot].run_flag);

	clean_fpga_slot_irq(slot);

	if(sysinfo->slot_info[slot].run_flag == 1){
		//down_fpga_slot_irq(slot);
		//start_spi_transfer_task(slot);
	}

	/*
	else if(sysinfo.slot_info[slot].run_flag > 1){
		set_fpga_slot_irq(slot);
	}
	//*/

	#if 1
	for(i=0; i<3; i++){
		if(get_board_irq_events(slot)>0){
			break;
		}
	}

	/*
	if(sysinfo.slot_info[slot].run_flag == 1){
		set_fpga_slot_irq(slot);
	}
	//*/

	DebugPrintf("%d slot get irq events %d times\n", slot, i);
	
	if(i>=3){
		set_fpga_slot_irq(slot);
		return;
	}

	DebugPrintf("%d slot get irq events succss\n", slot);
	
	for(i=0; i<10; i++){
		plog = &board_event_logs[slot].irq_events[i];
		printf("%d board irq event : %#8x, %d\n", slot, plog->event_code, plog->datatime);
		if(plog->event_code == 0){
			break;
		}else{
			event_level = EVENT_LEVEL(plog->event_code);
			event_obj = EVENT_OBJECT(plog->event_code);
			event_act = EVENT_ACTION(plog->event_code);
			event_param = EVENT_PARAM(plog->event_code);
			
			if(event_level && event_obj && (event_obj !=FUNC_TYPE_UPGRADE)){
				sysinfo->slot_status[slot] = event_level;
			}
			
			switch(event_obj){
				case FUNC_TYPE_POWER:
					if(event_act == ACT_TYPE_RESET){
						reset_slot_board_status(slot);
					}
					break;
				case FUNC_TYPE_APPLACATION :
					if(event_act == ACT_TYPE_STARTOVER){
						if(sysinfo->slot_info[slot].run_flag){
							sysinfo->slot_status[slot] = SLOT_STATUS_NORMAL;
							
							sysinfo->slot_info[slot].run_flag = 2;
							set_slot_board_systime(slot);
						}
					}
					break;
				case FUNC_TYPE_UPGRADE :						
					spi_read_block(slot, SLAVER_SPI_UPGRADE_STATUS_ADDR, &upgrade_status[slot+1], sizeof(upgrade_st));
					if(upgrade_status[slot+1].status == UPGRADE_SECCESS){
						//sysinfo.slot_info[slot].run_flag = 0;
						//reset_slot_board_status(slot);
					}
					break;
				case FUNC_TYPE_TEMPERATURE :						
					
					sysinfo->slot_info[slot].temperature = event_param & 0x7f;
					sysinfo->slot_info[slot].tc_ratio = (event_param>>7) & 0x7f;
					
					if(event_level == EVENT_LEVEL_NORMAL){
						sysinfo->slot_info[slot].temp_abnormal = 0;
						if(sysinfo->slot_info[slot].run_flag == 0){
							resume_slot_board(slot);
						}
					}else if(event_level == EVENT_LEVEL_ERROR){
						suspend_slot_board(slot);
						sysinfo->slot_info[slot].temp_abnormal = 1;
					}else{
						sysinfo->slot_info[slot].temp_abnormal = 1;
					}

					printf("%d slot board temperature = %d, abnormal = %d, ratio = %d \n", slot, sysinfo->slot_info[slot].temperature, sysinfo->slot_info[slot].temp_abnormal, sysinfo->slot_info[slot].tc_ratio);
					
					break;
					
			}
		}
	}
	#endif

	set_fpga_slot_irq(slot);
}


int add_board_irq_task(taskHandler handler, int slot);
void fpga_sig_handler(int sig)
{
	int i;
	int status, irq_status;
	u_short irq_mask;

	irq_status = read_fpga_irq_status();
	
	printf("fpga_signal [%d] : irq status = %#x \n", sig, irq_status);

	#if (!defined(DEBUG_TOOLS_MODE) && !defined(INDEPENDENT_UPGRADE)) 

	status = FPGA_ARMIF_READ(REG_FPGA_SLOT_CONNECT_STATUS);
	
	printf(" slot connect status = %#x \n",  status);
	if(irq_status && status){
		for(i=0; i<sysinfo->board_qty; i++){

			if((irq_status & (1<<i)) && (status & (1<<i))){
				
				if(add_board_irq_task((taskHandler)slot_board_action, i) == 1){
					
				}
			}else{
				//set_fpga_slot_irq(i);
			}
			
		}
	}else{
		set_fpga_irq(0x7f);
	}

	#endif
}


void check_fpga_irq(void)
{
	int i;
	int status, irq_status;
	u_short irq_mask;

	irq_status = get_fpga_irq_status();
	
	//printf("fpga  irq status = %#x \n",  irq_status);

	#if (!defined(DEBUG_TOOLS_MODE) && !defined(INDEPENDENT_UPGRADE)) 

	status = FPGA_ARMIF_READ(REG_FPGA_SLOT_CONNECT_STATUS);
	
	irq_status &= 0x807f;
	
	if(irq_status){
		printf("fpga slot irq status : %#x \n", irq_status);
	}
	
	set_fpga_irq(0x0);


	for(i=0; i<sysinfo->board_qty; i++){
		
		check_slot_in_status(status, i);
		//*
		if(sysinfo->slot_info[i].run_flag == 0){
			sysinfo->slot_info[i].run_flag = 1;
		}
		
		//*/
		if((irq_status & (1<<i)) && sysinfo->slot_status[i]){
			//clean_fpga_slot_irq(i);
			if(add_board_irq_task((taskHandler)slot_board_action, i) == 1){
				//set_fpga_slot_irq(i);
			}
		}
		
	}

	set_fpga_irq(0x7f);

	#endif
}

#endif


static void *moniter_task(void *args)
{
	int i;
	int read_tsoip_link_count = 0;
	static int board_qty;
	
	char filepath[100], *filename;

	printf("moniter task start ... \n");
	
	board_qty = sysinfo->board_qty;
	
	while(sysinfo->run_flag){

		//wait_for_upgrade();

		//read_slots_status();
		#if 1
		for(i=0; i<board_qty; i++){

			/*
			if(sysinfo->slot_info[i].link_changed){
				
				sysinfo->slot_info[i].link_changed = 0;

				if(sysinfo->slot_status[i]){

					if(get_spi_runflag(i)==PAUSE_FLAG){
						resume_spi_transfer_task(i);
					}else{
						
						start_spi_transfer_task(i);
					}
					
					sysinfo->slot_info[i].run_flag = 1;
			
					add_log(EVENTCODE(FUNC_TYPE_CARD_SLOT, ACT_TYPE_PLUG_IN, i+1), EVENT_LEVEL_INFO);
					//add_async_task((taskHandler)test_board_spi, (void *)i, 4);

				}else{
					sysinfo->slot_info[i].run_flag = 0;
					pause_spi_transfer_task(i);
					disable_transceiver(i);
					add_log(EVENTCODE(FUNC_TYPE_CARD_SLOT, ACT_TYPE_PULL_OUT, i+1), EVENT_LEVEL_INFO);
				}
				
			}
			//*/
			
			if((sysinfo->slot_info[i].properties_file_changed) && (sysinfo->slot_info[i].properties_file_isok)){
				
				hashmap_destroy(board_event_logs[i].devif_maps, NULL);
				hashmap_destroy(board_event_logs[i].functype_maps, NULL);
				hashmap_destroy(board_event_logs[i].actiontype_maps, NULL);

				sprintf(filepath, SUBBOARD_DIR"/slot%.2d/", i);
				filename = &filepath[strlen(filepath)];

				strcpy(filename, "devif.properties");
				board_event_logs[i].devif_maps = read_properties(NULL, filepath, 1);
				strcpy(filename, "functype.properties");
				board_event_logs[i].functype_maps = read_properties(NULL, filepath, 1);
				strcpy(filename, "eventaction.properties");
				board_event_logs[i].actiontype_maps = read_properties(NULL, filepath, 1);

				sysinfo->slot_info[i].properties_file_changed = 0;
	
			}

			/*
			if((sysinfo->slot_info[i].run_flag == 2) && is_spi_running(i)){
				
				sysinfo->slot_info[i].boardinfo_isok = 0;
				sysinfo->slot_info[i].old_param_isvalid = 0;
				sysinfo->slot_info[i].webfile_isok = 0;

				add_async_task((taskHandler)update_slot_boardinfo, (void *)i, 4);
				sysinfo->slot_info[i].run_flag = 3;
				printf("moniter task : %d slot run_flag = 3 ... \n");
			}
			//*/

			//printf("%d slot runflag = %d, boardinfoisok %d \n", i, sysinfo->slot_info[i].run_flag, sysinfo->slot_info[i].boardinfo_isok);
			
			if((sysinfo->slot_info[i].run_flag == 3) && sysinfo->slot_info[i].boardinfo_isok){

				printf("update %d slot board transceiver data \n", i);
				
				if((sysinfo->slot_info[i].board_changed)){
					
					sysinfo->slot_info[i].board_changed = 0;
					
					if(sysinfo->slot_info[i].transceiver_flag == 0){
						sysinfo->slot_info[i].transceiver_flag = 1;
					}else{
						free_transceiver(i);
					}
		
					init_transceiver(i);
					allocate_transceiver(i);
					
					sysinfo->slot_info[i].transceiver_flag = 2;
				}else{
					if(sysinfo->slot_info[i].transceiver_flag == 0){
						sysinfo->slot_info[i].transceiver_flag = 1;
					}else{
						sysinfo->slot_info[i].transceiver_flag = 3;
					}
					add_async_task((taskHandler)sync_transceiver_route_tables, (void *)i, 4);
				}

				transceiver_reconfig(i);
				enable_transceiver(i);
				
				sysinfo->slot_info[i].run_flag = 4;
				
				printf("moniter task : %d slot run_flag = 4 ... \n", i);
			}

			
		}
		#endif
				
		read_tsoip_link_count++;
		if(read_tsoip_link_count>10){
			read_tsoip_link_count = 0;
			for(i=0; i<sysinfo->tsip_port; i++){
				//get_tsoip_netif_link_status(i);
			}
		}

		//*
		for(i=0; i<sysinfo->tsip_port; i++){
			read_tsoip_netif_in_bitrate(i);
			read_tsoip_netif_out_bitrate(i);
		}
		
		for(i=0; i<sysinfo->ipin_channels; i++){
			read_ipin_status(i);
		}
		for(i=0; i<sysinfo->ipout_channels; i++){
			read_ipout_status(i);
		}
		//*/

		//check_fpga_irq();
		
		sched_yield();
		usleep(10000);
		//sleep(1);

	}


	return (void *)0;
}

static void end_maintask(void)
{
	int i;

	stop_webserver();
	
	for(i=0; i<sysinfo->board_qty; i++){
		//end_spi_transfer_task(i);
	}
	
	sqlite_close();
	

	end_eventlogs();

	close_global_shm();

	free_nif_socket();
	free_shmmsg_client();

	#ifndef DEBUG_IN_X86
	close_rtc();
	close_fpga();
	close_fram();
	#endif
	

}


int init_prograce()
{
	int i,j, len;
	int thread_qty = 8;
	int res;
	int board_len;
	board_st *board, *slot_board;
	char filepath[100];
	


	init_nif_socket();


	init_arraylist(&slot_board_list, sysinfo->board_qty+1, DATA_FLAG(ASSIGN_DATA,POINTER_DATA,sizeof(board_st *)));
	arraylist_set(&slot_board_list, sysinfo->board_qty, main_board);

	
	
	init_sim_encrypt();


	//*

	for(i=0; i<sysinfo->board_qty; i++){
		//init_spi_transfer(i);
		init_board_logs(i);
	}
	//*/



	//for(i=0; i<sysinfo.max_transceiver_qty; i++){
	for(i=0; i<=sysinfo->board_qty; i++){


		transceiver_mutex_init(&transceiver_cfg[i]);

		init_list(&transceiver_cfg[i].setroute_list, STRUCT_DATA);

		for(j=0; j<=sysinfo->board_qty; j++){
			init_list(&transceiver_cfg[i].sndchan_list[j], STRUCT_DATA);
		}
		
		//printf("%d trans_idx = %d \n", i, trans_idx[i]);
	}
	
	//*/

	
	allocate_transceiver(sysinfo->board_qty);

	//*
	event_logs.devif_maps = read_properties(NULL, HOME_DIR"/properties/devif.properties", INTEGER_KEY);
	event_logs.functype_maps = read_properties(NULL, HOME_DIR"/properties/functype.properties", INTEGER_KEY);
	event_logs.actiontype_maps = read_properties(NULL, HOME_DIR"/properties/eventaction.properties", INTEGER_KEY);
	//*/
	
	
	init_async_task(thread_qty, SZ_1M);


	#ifndef DEBUG_IN_X86

	//*


	//*/

	for(i=0; i<sysinfo->board_qty; i++){
		
		if((sysinfo->slot_info[i].boardinfo_len > 0)){
			slot_board = (board_st *)((char *)main_board + BOARDINFO_SPAN*(i+1));		
			arraylist_set(&slot_board_list, i, slot_board);
			allocate_transceiver(i);
		}
	}

	#endif
	usleep(100000);

	
	set_multicast_ttl(255);
	set_multicast_loop(1);

	init_tsoip_in_backup();

	init_tsoip_env();

	select_db_blob("tsoip_in_backup", 0, &tsoip_in_backup, 12);
	if(tsoip_in_backup.backup_mode){
		tsoip_in_backup.channel_qty = tsoip_network[0].inchan_qty;
		len = sizeof(tsoip_in_backup.channel[0].src_ipaddr);
		for(i=0; i<sysinfo->ipin_channels; i++){
			select_db_blob("tsoip_in_channel_backup_src_ip", i, tsoip_in_backup.channel[i].src_ipaddr, len);
			select_db_blob("tsoip_in_channel_backup_threshold", 0, &tsoip_in_backup.channel[i].lower_bitrate, 4 * 2);
			select_db_integer("tsoip_in_channel_backup_src_mask", i, &tsoip_in_backup.channel[i].src_mask);
			tsoip_in_backup.channel[i].src_mask |= 1;
		}
	}else{
		tsoip_in_backup.channel_qty = sysinfo->ipin_channels;
	}
	

	select_db_blob("ipin_idx_ref", 0, ipin_idx_ref, 4 * sysinfo->ipin_channels);
	
	select_db_blob("ipout_idx_ref", 0, ipout_idx_ref, 4 * sysinfo->ipout_channels);

	return 0;
}

int main(int argc, char *argv[])
{	
	
	int argind;
	u_int v_u32;
	char *argp, *configFile = NULL;
	
	setsid();
		
	chdir(getenv("HOME"));
	
	setbuf(stdout, NULL);


	open_global_shm();
	
	sysinfo->web_version = WEB_MAIN_VERSION;
	sysinfo->webserver_pid = getpid();

	while(sysinfo->run_flag<4){
		usleep(100000);
	}
	
	open_rtc();
	
	init_eventlogs();
	
	open_fpga();
	
	open_fram();

	init_database();

	init_prograce();

	init_shmmsg_client();
	
	add_log(EVENTCODE(FUNC_TYPE_WEBSERVER, ACT_TYPE_START, 0), EVENT_LEVEL_NORMAL);

	
	init_mux(sysinfo->mux_channels, sysinfo->tsin_channels, 0, 0);

	if(sysinfo->run_flag<5){
		sysinfo->run_flag = 5;
		#ifndef DEBUG_IN_X86
		//init_tsoip();
		sync_transceiver_route_tables(sysinfo->board_qty);	
		#endif
	}
		
	
	#ifndef DEBUG_IN_X86
	
	if(sysinfo->run_flag<6){
		sysinfo->run_flag = 6;
		init_mux_fpga();
	}
	
	create_detached_pthread(moniter_task, NULL, SZ_1M);
	create_detached_pthread(mux_task, NULL, SZ_1M);
	create_detached_pthread(capture_task, NULL, SZ_1M);
	//create_detached_pthread(ipcmsgsrv_task, NULL, SZ_1M);
	
	
	#endif

	configFile = sysinfo->web_configfile;
	
	start_webserver(configFile);
	
	//while(sysinfo->run_flag){sched_yield();sleep(10);}

	end_maintask();

	return 0;
}





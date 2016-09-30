
 
#include "appref.h"
#include "front_panel/front_panel.h"

//#include "web/webserver.h"

#include "rtc.h"

#include "scan_mac.h"

#include <execinfo.h>

#include "ipcmsgsrv.h"
//#include "mux/mux.h"

#define  APP_VERSION  0x102d

#if 1

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

}


void check_fpga_irq(void)
{
	int i;
	int status, irq_status;
	u_short irq_mask;

	irq_status = get_fpga_irq_status();
	
	//printf("fpga  irq status = %#x \n",  irq_status);


	status = FPGA_ARMIF_READ(REG_FPGA_SLOT_CONNECT_STATUS);
	
	irq_status &= 0x807f;
	
	if(irq_status){
		printf("fpga slot irq status : %#x \n", irq_status);
	}
	
	set_fpga_irq(0x0);


	for(i=0; i<sysinfo->board_qty; i++){
		
		//check_slot_in_status(status, i);
		//*
	
		
		//*/
		if((irq_status & (1<<i))){
			//clean_fpga_slot_irq(i);
			if(sysinfo->slot_info[i].run_flag == 0){
				//continue;
				sysinfo->slot_info[i].run_flag = 1;
				start_spi_transfer_task(i);
				//sysinfo->slot_status[i] = SLOT_STATUS_WARNING;
			}
			if(add_board_irq_task((taskHandler)slot_board_action, i) == 1){
				//set_fpga_slot_irq(i);
			}
		}
		
	}

	set_fpga_irq(0x7f);

}

#endif

void *moniter_task(void *args)
{
	int i;
	int fan_pwm, fan_ratio;
	static int board_qty;

	char filepath[100], *filename;
	
	int read_temperature_count = 0;
	int read_fanspeed_count = 0;
	int read_supply_count = 0;

	printf("moniter task start ... \n");

	//while(sysinfo->run_flag<6){
		//usleep(10000);
	//}
	
	board_qty = sysinfo->board_qty;
	
	while(sysinfo->run_flag){

		//wait_for_upgrade();
		
		read_slots_status();
//*
		for(i=0; i<sysinfo->board_qty; i++){

			Extend_Led_Set(slot_led[i], sysinfo->slot_status[i]);

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


			/*
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
			//*/
			

			//*
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

			/*
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
			//*/
			
		}

		
		
	//*/
	
		//*
		#ifndef DEBUG_IN_X86
		if(read_fanspeed_count>100){
			read_fans_speed();
			
			read_fanspeed_count = 0;
		}else{
			read_fanspeed_count++;	
		}
		#endif
		//*/

		//*
		
		#ifndef DEBUG_IN_X86
		if(read_supply_count>200){
			check_power_supply();
			read_supply_count = 0;
		}else{
			read_supply_count++;	
		}
		#endif
		//*/

		//*
		if(read_temperature_count>100){
			
			sysinfo->arm_temperature = read_temperature(0);
			//printf("arm_temperature = %d \n", sysinfo.arm_temperature);
			sysinfo->fpga_temperature = read_temperature(1);
			//printf("fpga_temperature = %d \n", sysinfo.fpga_temperature);

			if(sysinfo->fan_tempctrl){
			
				fan_ratio = 0;
				
				if(sysinfo->slot_info[board_qty].tc_flag){

					sysinfo->slot_info[board_qty].temperature = sysinfo->arm_temperature/4;
					
					if(sysinfo->slot_info[board_qty].temperature < 25){
						sysinfo->slot_info[board_qty].tc_ratio = 100 * 5000/MAX_FAN_SPEED;	
					}else if(sysinfo->slot_info[board_qty].temperature < 55){
						sysinfo->slot_info[board_qty].tc_ratio = 100 * ((sysinfo->slot_info[board_qty].temperature - 25)* (MAX_FAN_SPEED - 5000)/(55-25)  + 5000) / MAX_FAN_SPEED;
			}else{
						//add_log(EVENTCODE(FUNC_TYPE_TEMPERATURE, ACT_TYPE_EXCEED_LIMITS, 1+1), EVENT_LEVEL_ERROR);
						sysinfo->slot_info[board_qty].tc_ratio = 100;
					}
					
					fan_ratio = sysinfo->slot_info[board_qty].tc_ratio;
				}

				for(i=0; i<board_qty; i++){
					if(sysinfo->slot_info[i].run_flag && sysinfo->slot_info[i].tc_flag){
						
						if(sysinfo->slot_info[i].temp_abnormal){
							if( (sysinfo->slot_info[i].tc_ratio > fan_ratio)){
								fan_ratio = sysinfo->slot_info[i].tc_ratio;
							}
			
						}else{
							if((sysinfo->slot_info[i].temperature > 60) && (sysinfo->slot_info[i].tc_ratio > fan_ratio)){
								fan_ratio = sysinfo->slot_info[i].tc_ratio;
							}
						}
					}
				}
				
				if(fan_ratio){
					fan_pwm =  fan_ratio * MAX_FAN_SPEED / 100;
					set_fans_speed(fan_pwm);
				}
			}
			
			read_temperature_count = 0;
		}else{
			read_temperature_count++;	
		}
		//*/
	

		/*
		for(i=0; i<sysinfo->tsip_port; i++){
			read_tsoip_netif_in_bitrate(i);
		}
		for(i=0; i<sysinfo->tsip_port; i++){
			read_tsoip_netif_out_bitrate(i);
		}
		for(i=0; i<sysinfo->ipin_channels; i++){
			read_ipin_status(i);
		}
		//*/

		if(sysinfo->watchdog_enable){
			watchdog_feed();
		}

		
		check_fpga_irq();

		/*
		if(kill(sysinfo->webserver_pid, 0)<0){
			printf("web server [%d] end and restart it :  [%d] %s \n", sysinfo->webserver_pid, errno, strerror(errno));
			start_webserver(sysinfo->web_configfile);
		}else{
			printf("web server [%d] process exist  yet \n", sysinfo->webserver_pid);
		}
		//*/
		sched_yield();
		usleep(10000);
		//sleep(1);

	}


	return (void *)0;
}


static void sigHandler(int signo)
{
	printf("%d signal is raised \n", signo);
	
	sysinfo->run_flag = 0;
	
	#ifdef USE_CMD_LINE
	exit_cmdline();
	#endif
}

static void forceExitHandler(int signo)
{
	int ret, status;
	
	printf("force exit main progress \n");

	//sysinfo->run_flag = 0;
	//signal(SIGCHLD ,SIG_DFL);
	//kill(sysinfo->webserver_pid, SIGKILL);

	sleep(3);
	
	//exit(1);
}


void dump(int signo)
{
        void *array[10];
        size_t size;
        char **strings;
        size_t i;

        size = backtrace (array, 10);
        strings = backtrace_symbols (array, size);

        printf ("signal %d : Obtained %zd stack frames.\n", signo, size);

        for (i = 0; i<size; i++){
		printf ("%s\n", strings[i]);
        }

        free (strings);

       // exit(0);
}


void masksignal(int signo)
{
	sigset_t sigset, osigset;
	
	sigemptyset(&sigset);
	
	sigaddset(&sigset, signo);

	sigprocmask(SIG_BLOCK, &sigset, &osigset);
}

void mask_signal(void)
{
	sigset_t sigset, osigset;
	
	sigemptyset(&sigset);
	
	//sigaddset(&sigset, SIGHUP);

	//block Ctrl + C signal
	sigaddset(&sigset, SIGINT);	

	//sigaddset(&sigset, SIGQUIT);
	//sigaddset(&sigset, SIGKILL);
	
	//sigaddset(&sigset, SIGTERM);
	//sigaddset(&sigset, SIGSTOP);
	

	sigprocmask(SIG_BLOCK, &sigset, &osigset);
}

static void init_platform_signal(void) 
{
	mask_signal();

	
	struct sigaction act, oldact;
   	act.sa_flags = 0;

	//signal(SIGINT, sigHandler);

	//signal(SIGILL, &dump);
	//signal(SIGABRT, &dump);
	//signal(SIGSEGV, &dump);
	 
	act.sa_handler = sigHandler;
  	sigaction(SIGTERM, &act, &oldact);

	//act.sa_handler = forceExitHandler;
  	//sigaction(SIGKILL, &act, &oldact);
  
	act.sa_handler = sigHandler;
  	sigaction(SIGQUIT, &act, &oldact);
	
	
}


void lm73_signal_handler(int idx, int status)
{
	//lm73_alert_reset(sig);
	add_log(EVENTCODE(FUNC_TYPE_TEMPERATURE, ACT_TYPE_EXCEED_LIMITS, idx+1), EVENT_LEVEL_ERROR);
}

u_int read_kernal_version(void)
{
	int fd;
	u_int kernel_version;
	char databuf[12];
	
	fd = open("/sys/kernel/sysver", O_RDONLY);
	
	if(fd>0){
		read(fd, databuf, 12);
		kernel_version = strtol(databuf, NULL, 16);
	}else{
		kernel_version = 0;
	}
	
	//printf("sysinfo.kernel_version = %#x \n", kernel_version);
	
	close(fd);

	return kernel_version;
}

void fpga_ctrl_init(void)
{

	init_fpga();
	
	FPGA_Enable_IRQ(0);
}


void init_gpio(void)
{
	
	gpio_init(PWR1_TEST_GPIO, "pwr_1_test");
	gpio_init(PWR2_TEST_GPIO, "pwr_2_test");

	//gpio_init(RTC_RESET_N_GPIO, "rtc_reset_n");

	gpio_init(BUZZER_GPIO, "buzzer");


	gpio_input(PWR1_TEST_GPIO);
	gpio_input(PWR2_TEST_GPIO);

	//gpio_output1(RTC_RESET_N_GPIO);

	gpio_output0(BUZZER_GPIO);
}


void free_gpio()
{
	gpio_free(PWR1_TEST_GPIO);
	gpio_free(PWR2_TEST_GPIO);

	//gpio_free(RTC_RESET_N_GPIO);

	gpio_free(BUZZER_GPIO);
}


static void end_maintask(void)
{
	int i;

	//masksignal(SIGCHLD);
	//signal_procmask(SIG_SETMASK, SIGCHLD, 0);
	
	
	sysinfo->run_flag = 0;
	
	#ifndef DEBUG_IN_X86
	if(sysinfo->watchdog_enable){
		watchdog_close();
	}
	#endif
	
	for(i=0; i<sysinfo->board_qty; i++){
		//disable_transceiver(i);
		end_spi_transfer_task(i);
		//end_board_logs(i);
	}


	end_eventlogs();
	
	system("rmmod altTse");
	//stop_webserver();
	
	kill(sysinfo->webserver_pid, SIGKILL);

	
	clean_sysinfo();
	
	
	sqlite_close();

	free_gpio();


	#ifndef DEBUG_IN_X86
	close_rtc();
	close_fpga();
	close_gpio();
	close_front_panel();

	#ifdef USE_NOR_FLASH
	close_flash();
	#endif
	close_fram();
	#ifdef USE_TEMP_SENSOR
	close_lm73();
	#endif

	#endif
	
	
	close_global_shm();
	
}

int read_version_file(program_version_st *p_version, char *version_file);
void  print_version_file(program_version_st *p_version, char *prefix);

int main(int argc, char *argv[])
{	
	
	int  i, k, result;
	int argind;
	u_int v_u32;
	char *argp, *configFile = NULL;
	char *fpgaFile = NULL;
	u_int kernel_version;
	
	struct timeval btime;

	//printf("HOME : %s \n", getenv("HOME"));
	chdir(getenv("HOME"));
	
		
	#ifdef  __STDC_VERSION__
	//printf("std c version : %i \n", __STDC_VERSION__);
	#endif

	setbuf(stdout, NULL);

	
	open_global_shm();
	
	for (argind = 1; argind < argc; argind++) {

		argp = argv[argind];
		
		if (*argp != '-') {
			break;
		}

		if ( !strcmp(argp, "-conf")) {
			configFile = argv[++argind];
		}else if ( !strcmp(argp, "-fpga") ) {
			fpgaFile = argv[++argind];
		}
	}

	if(argind>=argc){
		argp = NULL;
	}

	init_platform_signal();

	//sysinfo->run_flag = 1;
	sysinfo->main_pid = getpid();
	sysinfo->app_version = APP_VERSION;
	//sysinfo->web_version = WEB_MAIN_VERSION;
	
	
	
	#ifdef DEBUG_IN_X86
	
	if(read_version_file(&program_version, HOME_DIR"/version")==0){
		print_version_file(&program_version, "The program needs :");
	}

	signal(SIGUSR1, fpga_sig_handler);
	#else 
	open_rtc();
	
	sysinfo->kernel_version = read_kernal_version();

	if(read_version_file(&program_version, HOME_DIR"/version")<0){
		close_global_shm();
		return -1;
	}

	print_version_file(&program_version, "The program needs :");

	if((sysinfo->app_version != program_version.app_version)){
		printf("app version [%#x] doesn't match , exit process\n", sysinfo->app_version);
		close_global_shm();
		return -1;
	}

	/*
	if((sysinfo->web_version != program_version.web_version)){
		printf("web version [%#x] doesn't match , exit process\n", sysinfo->web_version);
		close_global_shm();
		return -1;
	}
	//*/

	if((sysinfo->kernel_version < program_version.kernel_version)){
		printf("kernel version [%#x] is lower, exit process\n", sysinfo->kernel_version);
		close_global_shm();
		return -1;
	}
	
	sysinfo->main_version = program_version.main_version;

	init_eventlogs();

	front_panel_init();

	
	open_fpga();
	
	#ifdef USE_NOR_FLASH
	open_flash();
	#endif
	#ifdef USE_TEMP_SENSOR
	open_lm73();

	lm73_register_signal_handler(lm73_signal_handler);

	
	#endif
	open_fram();
	open_gpio();
	init_gpio();
	#endif

	
	init_database();

	
	if(init_sysinfo()<0){
		printf("init sysinfo failed \n");
		close_global_shm();
		return -1;
	}
	
	#ifndef DEBUG_IN_X86
	create_detached_pthread(Lcd, NULL, SZ_1M);
	#endif

	init_tsoip_env();

	
	//*
	#ifndef DEBUG_IN_X86
	gettimeofday(&btime, NULL);
	
	if(argp && (*argp == '0')){
		printf("skip the configfpga step\n");
		}else{
		if(Download_FPGA(fpgaFile)<0){
			close_global_shm();
			return -1;
		}
	}
	
	
	show_exectime(&btime, "download fpga");

	
	/*
	printf("download fpga complite, press any key continue \n");
	getchar();
	//*/

	sleep(1);
	
	sysinfo->fpga_version = FPGA_ARMIF_READ(REG_FPGA_FPGA_VERSION);
	printf("fpga_version = %#x \n", sysinfo->fpga_version);

	if(sysinfo->fpga_version != program_version.fpga_version){
		printf("fpga version doesn't match , exit process\n");
		//clean_sysinfo();
		//return 1;
	}
//	fpga_register_signal(sysinfo->main_pid, FPGA_IRQ_SIGNAL, fpga_sig_handler);
	sleep(1);
	fpga_ctrl_init();
	
	#endif
	
	sysinfo->run_flag = 2;

	
	load_parameters();
	

	
	
	//*/


	//*

	#ifndef DEBUG_IN_X86

	system("insmod "HOME_DIR"/driver/altTse.ko &> /dev/null");
	
	init_tsoip();
	
	//init_mux(sysinfo->mux_channels, sysinfo->tsin_channels, 0, 0);
	
	//sync_transceiver_route_tables(sysinfo->board_qty);
	

	if(sysinfo->watchdog_enable){
		watchdog_open();
	}
	create_detached_pthread(moniter_task, NULL, SZ_1M);
	//create_detached_pthread(mux_task, NULL, SZ_1M);
	//create_detached_pthread(capture_task, NULL, SZ_1M);
	
	create_detached_pthread(ipcmsgsrv_task, NULL, SZ_1M);
	create_detached_pthread(SCANServiceRecv, NULL, SZ_4K);
	#endif
	
	//create_detached_pthread((void *(*)(void *))save_parameters_task, NULL);
	//add_async_task((taskHandler)sync_transceiver_route_tables, 7, 4);

	sysinfo->run_flag = 3;

	if(configFile){
		strcpy(sysinfo->web_configfile, configFile);
	}else{
		sysinfo->web_configfile[0] = 0;
	}
	
	//*/


	//*
	

	sysinfo->run_flag = 4;
	sync_delay_run(reset_tsoip_networks, NULL, 5000);


	
	//start_webserver(configFile);
	while(sysinfo->run_flag){sched_yield();sleep(10);}
	
	
	end_maintask();
	

	//*/
	
	return 0;
}





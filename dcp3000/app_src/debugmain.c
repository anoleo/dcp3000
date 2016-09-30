
 
#include "appref.h"

#include "rtc.h"

#include "cmdline/cmdline.h"

#include "debug.h"

#include "debug/debug_mux.h"
#include "debug/debug_chip.h"
#include "debug/debug_tsoip.h"
#include "debug/debug_other.h"

#define MSG_FRONTPANEL_LED		0x07
#define MSG_SPI_MASTER		0x08
#define MSG_SPI_TRANSFER		0x09

int debug_spi(int argc, char *argv[])
{
	int i,slot, len, ret;
	char databuf[1024];
	char *rcvdata;
	u_int v_u32;
	
	if(argc<3)return -1;
	
	slot = atoi(argv[1]);

	if(strcmp("getfile", argv[2]) == 0){		
		if(argc>3)
		spi_get_file(slot, argv[3]);
	 }else if(strcmp("sendfile", argv[2]) == 0){
		if(argc>3)
		spi_send_file(slot, argv[3]);
	 }else if(strcmp("cmd", argv[2]) == 0){
		if(argc<4){return -1;}

		if((*argv[3] == '\"') || (*argv[3] == '\'')){
			databuf[0] = 0;
			for(i=3; i<argc; i++){
				sprintf(&databuf[strlen(databuf)], "%s ", argv[i]);
			}

			len = strlen(databuf);
			if((databuf[len-2] == '\"') || (databuf[len-2] == '\'')){
				databuf[len-2] = 0;
			}else{
				databuf[len-1] = 0;
			}
			
			//printf("send cmd %s to %d spi \n", &databuf[1], slot);
			ret = spi_send_cmd(slot, &databuf[1], &rcvdata);
		}else{
			//printf("send cmd %s to %d spi \n", argv[3], slot);
			ret = spi_send_cmd(slot, argv[3], &rcvdata);
		}

		if(ret>0){
			printf("%s", rcvdata);
		}
		
	 }else if(strcmp("synctime", argv[2]) == 0){
		set_slot_board_systime(slot);
	 }else if(strcmp("setslot", argv[2]) == 0){
		set_slot_board_slot(slot);
	 }else if(strcmp("getslot", argv[2]) == 0){
		get_slot_board_summary(slot);
	 }else if(strcmp("boardinfo", argv[2]) == 0){
		get_slot_boardinfo(slot);
	 }else if(strcmp("getweb", argv[2]) == 0){
		get_slot_webfile(slot);
	 }else if(strcmp("properties", argv[2]) == 0){
		get_slot_board_properties_files(slot);
	 }else if(strcmp("status", argv[2]) == 0){
		//print_spi_status(slot);
		v_u32 = 3;
		memcpy(databuf, &slot, 4);
		memcpy(&databuf[4], &v_u32, 4);
		send_shmmsg(MSG_SPI_MASTER, databuf, 8);
	 }else if(strcmp("open", argv[2]) == 0){
		//start_spi_transfer_task(slot);
		//while(get_spi_runflag(slot)==0)usleep(1000);
		//set_spi_runflag(slot, RUNNING_FLAG);

		v_u32 = 1;
		memcpy(databuf, &slot, 4);
		memcpy(&databuf[4], &v_u32, 4);
		send_shmmsg(MSG_SPI_MASTER, databuf, 8);
			
	 }else if(strcmp("close", argv[2]) == 0){
		//end_spi_transfer_task(slot);
		v_u32 = 2;
		memcpy(databuf, &slot, 4);
		memcpy(&databuf[4], &v_u32, 4);
		send_shmmsg(MSG_SPI_MASTER, databuf, 8);
	 }else if(strcmp("enable", argv[2]) == 0){
		set_spi_runflag(slot, RUNNING_FLAG);
	 }else if(strcmp("disable", argv[2]) == 0){
		set_spi_runflag(slot, PAUSE_FLAG);
	 }

}


int debug_slot(int argc, char *argv[])
{
	int i, slot = 0;
	int status;

	if(argc>2)slot = atoi(argv[2]);
	
	if(strcmp("status", argv[1]) == 0){	
		status = read_slots_status();
		printf("slots board connect status = %#x\n", status);
	}else if(strcmp("reset", argv[1]) == 0){
		for(i=2; i<argc; i++){
			reset_slot_board(atoi(argv[i]));
		}
	}else if(strcmp("uart", argv[1]) == 0){
		switch_slot_uart(slot);
	}else if(strcmp("clean", argv[1]) == 0){

		printf("clean %d slot info and status \n", slot);

		sysinfo->slot_info[slot].board_type = 0;
		sysinfo->slot_info[slot].board_version = 0;
		sysinfo->slot_info[slot].web_version = 0;
		sysinfo->slot_info[slot].properties_file_version = 0;

		sysinfo->slot_info[slot].boardinfo_isok = 0;
		sysinfo->slot_info[slot].board_changed = 0;
		sysinfo->slot_info[slot].old_param_isvalid = 0;
		sysinfo->slot_info[slot].webfile_isok = 0;
		sysinfo->slot_info[slot].webfile_changed = 0;
		sysinfo->slot_info[slot].transceiver_flag = 0;

		sysinfo->slot_info[slot].last_slot = 0xff;
		sysinfo->slot_info[slot].run_flag = 2;
		
	}else if(strcmp("info", argv[1]) == 0){
		printf("---------------%d slot info : --------------\n", slot);
		printf("slot_status = %d \n", sysinfo->slot_status[slot]);
		printf("run_flag = %d \n", sysinfo->slot_info[slot].run_flag);
		printf("link_changed = %d \n", sysinfo->slot_info[slot].link_changed);
		printf("link_status = %d \n", sysinfo->slot_info[slot].last_link_status);
		printf("board_type = %#x \n", sysinfo->slot_info[slot].board_type);
		printf("board_version = %#x \n", sysinfo->slot_info[slot].board_version);
		printf("web_version = %#x \n", sysinfo->slot_info[slot].web_version);
		printf("boardinfo_isok = %d \n", sysinfo->slot_info[slot].boardinfo_isok);
		printf("webfile_isok = %d \n", sysinfo->slot_info[slot].webfile_isok);
		printf("board_changed = %d \n", sysinfo->slot_info[slot].board_changed);
		printf("webfile_changed = %d \n", sysinfo->slot_info[slot].webfile_changed);
		printf("transceiver_flag = %d \n", sysinfo->slot_info[slot].transceiver_flag);
		printf("tc_flag = %d \n", sysinfo->slot_info[slot].tc_flag);
		printf("tc_ratio = %d \n", sysinfo->slot_info[slot].tc_ratio);
		printf("temp_abnormal = %d \n", sysinfo->slot_info[slot].temp_abnormal);
		printf("temperature = %d \n", sysinfo->slot_info[slot].temperature);
		printf("==============%d slot info : ============\n", slot);
	 }
	 
	return 0;
}

int debug_fan(int argc, char *argv[])
{
	int i, v_s32 = 0;
	
	if(strcmp("show", argv[1]) == 0){	
		read_fans_speed();
		for(i=0; i<sysinfo->fan_qty; i++){
			printf("fan %d : speed = %d r/min\n", i, sysinfo->fan_speed[i]);
		}
	}else if(strcmp("set", argv[1]) == 0){
		if(argc>2){
			v_s32 = atoi(argv[2]);
			set_fans_speed(v_s32);
		}
	}else if(strcmp("tempctrl", argv[1]) == 0){
		if(argc>2){
			v_s32 = atoi(argv[2]);
			set_fan_tempctrl(v_s32);
		}
	}

	return 0;
}

int debug_ntp(int argc, char *argv[])
{
	u_int hostip = 0;
	int timezone = 0;
	
	if(argc>1){
		hostip = strtoul(argv[1], NULL, 16);
		if(argc>2){
			timezone = atoi(argv[2]);
		}
		sync_ntp(hostip, timezone);
	}

	return 0;
}

int debug_sysinfo(int argc, char *argv[])
{
	struct timeval btime;
	int timezone;
	
	if(argc<2){
		return 0;
	}
	
	if(strcmp(argv[1], "wdt") == 0){
		if(strcmp(argv[2], "open") == 0){
			watchdog_open();
			printf("debug open watchdog\n");
		}else if(strcmp(argv[2], "close") == 0){
			watchdog_close();
			printf("debug close watchdog\n");
		}else if(strcmp(argv[2], "feed") == 0){
			watchdog_feed();
			printf("debug feed dog\n");
		}else if(strcmp(argv[2], "stop") == 0){
			sysinfo->watchdog_enable = 0;
			printf("debug stop feed dog\n");
		}else if(strcmp(argv[2], "start") == 0){
			sysinfo->watchdog_enable = 1;
			printf("debug start feed dog\n");
		}
	}else
	if(strcmp(argv[1], "info") == 0){
		printf("product_name = %s \n", sysinfo->product_name);
		printf("device_name = %s \n", sysinfo->device_name);
		printf("device_sn = %s \n", sysinfo->device_sn);
		printf("process id = %u \n", sysinfo->main_pid);
		printf("run_flag = %d \n", sysinfo->run_flag);
		printf("watchdog_enable = %d \n", sysinfo->watchdog_enable);
		printf("arm_temperature = %f \n", (float)sysinfo->arm_temperature/4);
		printf("fpga_temperature = %f \n", (float)sysinfo->fpga_temperature/4);
		printf("temperature high = %f \n", (float)sysinfo->temperature_high/4);
		printf("temperature low = %f \n", (float)sysinfo->temperature_low/4);
		printf("fan temperature control = %d \n", sysinfo->fan_tempctrl);
		printf("main_version = %#x \n", sysinfo->main_version);
		printf("firmware_version = %#x \n", sysinfo->firmware_version);
		printf("fpga_version = %#x \n", sysinfo->fpga_version);
		printf("app_version = %#x \n", sysinfo->app_version);
		printf("kernel_version = %#x \n", sysinfo->kernel_version);
		printf("web_version = %#x \n", sysinfo->web_version);
		printf("loginuser = %s \n", sysinfo->loginuser);
		printf("loginpasswd = %s \n", sysinfo->loginpasswd);
		printf("tsip_port = %d \n", sysinfo->tsip_port);
		printf("ipin_channels = %d \n", sysinfo->ipin_channels);
		printf("ipout_channels = %d \n", sysinfo->ipout_channels);
		printf("board slot qty = %d \n", sysinfo->board_qty);
		printf("transceiver interface amount = %d \n", sysinfo->transceiver_qty);
		printf("main_board.name = %s \n", main_board->name);
		printf("main_board.version = %#x \n", main_board->version);
		printf("main_board.type = %#x \n", main_board->type);

	}
	else if(strcmp(argv[1], "loadfpga") == 0){
		gettimeofday(&btime, NULL);
		if(argc>2){
			Download_FPGA(argv[2]);
		}else{
			Download_FPGA(NULL);
		}
		show_exectime(&btime, "download fpga");
	}else if(strcmp(argv[1], "timezone") == 0){
		gettimeofday(&btime, NULL);
		if(argc>2){
			timezone = atoi(argv[2]);
		}
		set_timezone(timezone);
	}

	return 0;
}

int debug_boardinfo(int argc, char *argv[])
{
	int slot = 0;
	board_st *slot_board;
	
	if(argc>1){
		
		if(strcmp(argv[1], "init") == 0){
			if(argc>2){
				init_main_board_info(argv[2]);
			}else{
				init_main_board_info(HOME_DIR"/board_info.xml");
			}
		}else if(strcmp(argv[1], "clean") == 0){
			clean_main_board_info();
		}else{
			slot = atoi(argv[1]);
			slot_board = (board_st *)((char *)main_board + BOARDINFO_SPAN*(slot+1));
			if((slot_board->type) && (slot_board->func_list)){
				print_boardinfo(slot_board);
			}
		}
		
	}else{
		print_boardinfo(main_board);
	}
	
	return 0;
}


int debug_log(int argc, char *argv[])
{
	int slot = 0;
	u_int eventcode;
	int level;
	int i, index, count, log_sn;
	event_logs_st *p_eventlogs;
	log_record_st logs_buf[100];
	
	if(argc<2){
		return 0;
	}

	if(strcmp(argv[1], "add") == 0){
		if(argc<4){
			return 0;
		}
		eventcode = strtoul(argv[2], NULL, 16);
		level = atoi(argv[3]);
		add_log(eventcode, level);
	}else if(strcmp(argv[1], "info") == 0){
		get_loginfo();
	}else if(strcmp(argv[1], "clean") == 0){
		if(argc>2){
			slot = atoi(argv[2]);
			clean_board_logs(slot);
		}else{
			clean_logs();
		}

		print_log(get_log(index));
	}else if(strcmp(argv[1], "get") == 0){
		if(argc<3){
			return 0;
		}
		index = atoi(argv[2]);
		print_log(get_log(index));
	}else if(strcmp(argv[1], "getslot") == 0){
		if(argc<5){
			return 0;
		}
		slot = atoi(argv[2]);
		log_sn = atoi(argv[3]);
		count = atoi(argv[4]);
		count = get_board_logs_by_sn(slot, log_sn, count);
		p_eventlogs = &board_event_logs[slot];
		printf("--------------%d slot board logs--------------\n", slot);
		printf("log_count = %d, last_sn = %d \n", p_eventlogs->log_count, p_eventlogs->last_sn);
		printf("getlog start_sn = %d, qty = %d \n", p_eventlogs->gottenlogs_start_sn, p_eventlogs->gottenlogs_qty);

		for(i=0; i<count; i++){
			print_board_log(slot, &p_eventlogs->logs_buf[i]);
		}

		printf("==================================\n\n");
	}else if(strcmp(argv[1], "getsn") == 0){
		if(argc<4){
			return 0;
		}
		log_sn = atoi(argv[2]);
		count = atoi(argv[3]);
		count = get_logs_by_sn(logs_buf, log_sn, count);
		p_eventlogs = &event_logs;
		printf("--------------%d slot board logs--------------\n", slot);
		printf("log_count = %d, last_sn = %d \n", p_eventlogs->log_count, p_eventlogs->last_sn);
		printf("getlog start_sn = %d, qty = %d \n", p_eventlogs->gottenlogs_start_sn, p_eventlogs->gottenlogs_qty);

		for(i=0; i<count; i++){
			print_log(&logs_buf[i]);
		}

		printf("==================================\n\n");
	}else if(strcmp(argv[1], "getasc") == 0){
		if(argc<4){
			return 0;
		}
		index = atoi(argv[2]);
		count = atoi(argv[3]);
		count = get_logs_asc(logs_buf, index, count);
		p_eventlogs = &event_logs;
		printf("--------------%d slot board logs--------------\n", slot);
		printf("log_count = %d, last_sn = %d \n", p_eventlogs->log_count, p_eventlogs->last_sn);
		printf("getlog start_sn = %d, qty = %d \n", p_eventlogs->gottenlogs_start_sn, p_eventlogs->gottenlogs_qty);

		for(i=0; i<count; i++){
			print_log(&logs_buf[i]);
		}

		printf("==================================\n\n");
	}

	return 0;
}

int debug_led(int argc, char *argv[])
{
	int i;
	int color;
	char databuf[8];

	if(argc<3){
		return -1;
	}

	color = atoi(argv[2]);

	if(strcmp("a", argv[1]) == 0){
		i=10;
	}else if(strcmp("w", argv[1]) == 0){
		i=9;
	}else{
		i = atoi(argv[1]);
	}

	memcpy(databuf, &i, 4);
	memcpy(&databuf[4], &color, 4);
	send_shmmsg(MSG_FRONTPANEL_LED, databuf, 8);

	return 0;
}

#define MYSELF_SHMEM_PROGRAM_ID 	284
//#define OPPOSITE_SHMEM_PROGRAM_ID	220
//#define MSG_BUF_SIZE		0x800000


static void end_maintask(void)
{
	int i;
	
	sqlite_close();


	#ifndef DEBUG_IN_X86
	close_rtc();
	close_fpga();
	close_gpio();
	
	#ifdef USE_NOR_FLASH
	close_flash();
	#endif
	close_fram();
	#ifdef USE_TEMP_SENSOR
	close_lm73();
	#endif

	#endif
	
	free_nif_socket();
	
	free_shmmsg_client();
	close_global_shm();
	
}

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
	init_shmmsg_client();
	
	#ifdef DEBUG_IN_X86

	#else 
	open_rtc();
		
	open_fpga();
	
	#ifdef USE_NOR_FLASH
	open_flash();
	#endif
	#ifdef USE_TEMP_SENSOR
	open_lm73();
	
	#endif
	open_fram();
	open_gpio();
	#endif

	
	init_nif_socket();
	
	init_database();


	init_cmdline(argv[0]);



	//*
	
	#ifndef DEBUG_IN_X86
	regist_internal_command("led", debug_led, " led  [#idx|a|w] [#color] ");
	regist_internal_command("fpga", debug_fpga, "fpga [load|r|w|rf|wf|brf|bwf|bowf|unlockpage] [#addr|file] [#value|#size|#len|wdata...] [#size]");
	#ifdef USE_NOR_FLASH
	regist_internal_command("mtd", debug_mtd, " mtd [info|region|r|w|e|cp|] [#mtddev] [#addr|#regionidx] [#len|file]");
	regist_internal_command("flash", debug_flash, " flash [r|w|e|cp|prot|unprot|ppbs|mkimg] [#addr|#len|wdata...|file|#sector] [#len|#count]");
	#endif
	#ifdef USE_TEMP_SENSOR
	regist_internal_command("lm73", debug_lm73, " lm73 [rdtemp|rdthigh|rdtlow|rdcfg|rdstatus|setcfg|setctrl|setmode|reset|alert|setthigh|settlow] [#idx] [#value] ");
	#endif
	regist_internal_command("gpio", debug_gpio, " gpio [open|close|init|free|dir|set|clean|get] [#idx] [in|out|name] [#value]");
	regist_internal_command("fram", debug_fram, " fram [r|w|e|cp] [#addr] [#len|wdata...|file]");
	regist_internal_command("fan", debug_fan, " fan [show|set|tempctrl] [#speed|...] ");
	#endif
	regist_internal_command("ipbackup", debug_tsoip_backup, "ipbackup [info|channel|enable|disable|period] [#channel] [...]");
	regist_internal_command("tsoip", debug_tsoip, "tsoip [cfg|local|init|in|out|addmc|delmc] [#ifno|#channel] [...]");
	regist_internal_command("spi", debug_spi, "spi [#slot] [open|close|enable|disable|setslot|synctime|getfile|sendfile|cmd] [\"comand\"|filename|...]");
	//regist_internal_command("property", debug_properties, "property [read|reset|stat|walk] [file] [#keytype]");
	regist_internal_command("slot", debug_slot, " slot [status|reset|uart|info|clean] [#slot]");
	regist_internal_command("sys", debug_sysinfo, " [info|wdt|loadfpga|timezone] [open|close|feed|stop|start|fpgafile|#zone] ");
	regist_internal_command("boardinfo", debug_boardinfo, " print boardinfo data ");
	regist_internal_command("transceiver", debug_transceiver, " transceiver [rxrate|txrate|init|free|info|enable|disable|setroute|clrroute|sendchan|map|asiout] [#trans_if] [#...]");
	//regist_internal_command("mysql", debug_mysql, " mysql [...] ");
	regist_internal_command("cmd", debug_syscmd, " cmd [...] ");
	regist_internal_command("ntp", debug_ntp, " ntp [#hostip] ");

	//regist_internal_command("capture", debug_capture, " capture [ctrl] [#channel|#pid ... ] ");
	//regist_internal_command("mux", debug_mux, " mux [enable|disable|setpid|clrpid] [#out_channel|#in_chan][#in_pid] [#mux_pid] ");
	//regist_internal_command("ts", debug_ts, "ts  [capture|mux|iptv] [#mux_channel|#in_channel| ... ] [#in_channel] [#total_bitrate] [program_number ... ]");
	regist_internal_command("period", debug_period_task, "period  [create|start|pause|stop|update] [#timems] ");
	regist_internal_command("param", debug_parameter, "param  [sn|mac|default] [r|w] [#idx|data] [data]");


	start_cmdline("pbi");

	
	end_maintask();
	

	//*/
	
	return 0;
}





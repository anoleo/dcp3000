#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../basedef.h"
#include "../debug.h"

#include "../utils/utils.h"
#include "../utils/system.h"
#include "../utils/hashmap.h"
#include "../transfer/transceiver.h"

int debug_syscmd(int argc, char *argv[])
{
	int i;
	char cmdbuf[1024] = {0};
	struct timeval t_start; 


	for(i=1; i<argc; i++){
		sprintf(&cmdbuf[strlen(cmdbuf)], "%s ", argv[i]);
	}

	printf("syscmd : %s \n", cmdbuf);
	
	gettimeofday(&t_start, NULL); 
	
	system(cmdbuf);

	show_exectime(&t_start, "exec cmd");
	
	return 0;
}


void test_task(void)
{
	printf("test_task : cur time = %lu\n", time(NULL));
}

int debug_period_task(int argc, char *argv[])
{
	int i, v_s32 = 0;
	
	struct timeval t_start; 
	
	static period_task_st periord_task;

	
	if(strcmp("create", argv[1]) == 0){	
		
		periord_task.handler = test_task;
		periord_task.arg = NULL;
		periord_task.run_flag = 0;
		init_timeout(&periord_task.timeout, atoi(argv[2]));
		if(argc>3){
			v_s32 = atoi(argv[3]);
		}
		create_detached_pthread(start_period_task, &periord_task, v_s32);
	}else if(strcmp("start", argv[1]) == 0){
		periord_task.run_flag = RUNNING_FLAG;
	}else if(strcmp("pause", argv[1]) == 0){
		periord_task.run_flag = PAUSE_FLAG;
	}else if(strcmp("stop", argv[1]) == 0){
		periord_task.run_flag = STOP_FLAG;
	}else if(strcmp("update", argv[1]) == 0){
		update_timeout(&periord_task.timeout, atoi(argv[2]));
	}

	
	return 0;
}



int show_int_property(hashItem_st *item)
{
	printf("property : %u = %s \n", item->key, (char *)item->data);

	return 0;
}

int show_string_property(hashItem_st *item)
{
	printf("property : %s = %s \n", (char *)item->key, (char *)item->data);

	return 0;
}

int debug_properties(int argc, char *argv[])
{
	int keytype = 0;
	static hashmap_st *map = NULL;

	if(strcmp("read", argv[1]) == 0){
		if(argc>3)
			keytype = atoi(argv[3]);
		map = read_properties(NULL, argv[2], keytype);
	}else if(strcmp("stats", argv[1]) == 0){
		if(map==NULL){return -1;}
		hash_pstats(map->table, "property hashmap");
	}else if(strcmp("walk", argv[1]) == 0){
		if(keytype)
			hashmap_walk(map, show_int_property);
		else
			hashmap_walk(map, show_string_property);
	}else if(strcmp("reset", argv[1]) == 0){
		hashmap_destroy(map, NULL);
	}

	return 0;
}


int debug_parameter(int argc, char *argv[])
{
	int i;
	int addr, len, sector;
	u_short wdata;
	char data_buf[128];

	if(strcmp("mac", argv[1]) == 0){

		if(strcmp("w", argv[2]) == 0){
			strcpy(data_buf, argv[4]);
			fram_write_mac(atoi(argv[3]), data_buf);
		}else if(strcmp("r", argv[2]) == 0){
			fram_read_mac(atoi(argv[3]), data_buf);
			print_bytes(data_buf, 6);
		}
	 		
	}else if(strcmp("sn", argv[1]) == 0){
	
		if(strcmp("w", argv[2]) == 0){
			strcpy(data_buf, argv[3]);
			fram_write_sn(data_buf);
		}else if(strcmp("r", argv[2]) == 0){
			fram_read_sn(data_buf);
			printf("sn : %s \n", data_buf);
		}
	 	
	}else if(strcmp("default", argv[1]) == 0){
	
		factory_setting();
	 	
	}

	return 0;
	
}

int debug_transceiver(int argc, char *argv[])
{
	int i;
	int trans_if = 0;
	int trans_chan;
	trans_chan_st chan_param;
	spitask_st *p_spitask;
	u_int v_u32;
	
	if(argc < 2){
		return -1;
	}
	
	if(argc > 2){
		trans_if = atoi(argv[2]);
	}
	
	if(strcmp("rate", argv[1])==0){
		trans_chan = strtoul(argv[2], NULL, 16);
		v_u32 = read_transceiver_channel_byterate(trans_chan);
		printf("transceiver  channel %#x byterate : %d\n", trans_chan, v_u32);
	}else if(strcmp("validrate", argv[1])==0){
		trans_chan = strtoul(argv[2], NULL, 16);
		v_u32 = read_transceiver_channel_valid_byterate(trans_chan);
		printf("transceiver  channel %#x valid byterate : %d\n", trans_chan, v_u32);
	}else if(strcmp("ipoutrate", argv[1])==0){
		trans_chan = strtoul(argv[2], NULL, 16);
		v_u32 = read_transceiver_ipout_channel_byterate(trans_chan);
		printf("transceiver ipout channel %#x byterate : %d\n", trans_chan, v_u32);
	}else if(strcmp("muxrate", argv[1])==0){
		trans_chan = strtoul(argv[2], NULL, 16);
		v_u32 = read_transceiver_mux_channel_byterate(trans_chan);
		printf("transceiver mux channel %#x byterate : %d\n", trans_chan, v_u32);
	}else if(strcmp("info", argv[1])==0){
		print_transceiver_info(trans_if);
	}else if(strcmp("init", argv[1])==0){
		init_transceiver(trans_if);
	}
	/*
	else if(strcmp("free", argv[1])==0){
		free_transceiver(trans_if);
	}
	//*/
	else if(strcmp("enable", argv[1])==0){
		enable_transceiver(trans_if);
	}else if(strcmp("disable", argv[1])==0){
		disable_transceiver(trans_if);
	}else if(strcmp("reconfig", argv[1])==0){
		transceiver_reconfig(trans_if);
	}else if(strcmp("setroute", argv[1])==0){
		if(argc < 4){	return -1;}
		trans_if = strtoul(argv[3], NULL, 16);
		trans_chan = strtoul(argv[2], NULL, 16);
		set_transceiver_route_regs(trans_chan, trans_if);
	}else if(strcmp("clrroute", argv[1])==0){
		if(argc < 4){	return -1;}
		trans_if = strtoul(argv[3], NULL, 16);
		trans_chan = strtoul(argv[2], NULL, 16);
		clean_transceiver_route_regs(trans_chan, trans_if);
	}else if(strcmp("muxmap", argv[1])==0){
		if(argc < 4){	return -1;}
		transceiver_mux_channel_mapping(strtoul(argv[2], NULL, 16), strtoul(argv[3], NULL, 16));

	}else if(strcmp("ipoutmap", argv[1])==0){
		if(argc < 4){	return -1;}
		transceiver_ipout_channel_mapping(strtoul(argv[2], NULL, 16), strtoul(argv[3], NULL, 16));
	}else if(strcmp("asiout", argv[1])==0){
		trans_chan = strtoul(argv[2], NULL, 16);
		printf("transceiver set asi out %#x \n", trans_chan);
		set_asiout_transceiver_source(trans_chan);
	}else if(strcmp("tsinmap", argv[1])==0){
		for(i=0; i<sysinfo->tsin_channels; i++){
			printf("tsin_mapping_chan[%d] = %#x\n", i, tsin_mapping_chan[i]);
		}
	}else if(strcmp("tsinch", argv[1])==0){
		for(i=0; i<sysinfo->tsin_channels; i++){
			printf("tsin_trans_chan[%d] = %#x\n", i, tsin_trans_chan[i]);
		}
	}else{
		printf("command line arg error \n");
	}
	
	return 0;
}


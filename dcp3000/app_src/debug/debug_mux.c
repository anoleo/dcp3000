#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../debug.h"

#define MSG_TSIN_CAPTURE		0x10
#define MSG_TSIN_CAPTURE_CTRL		0x11
#define MSG_TSIN_CHANNEL_CAPTURE_CTRL		0x12
#define MSG_TSIN_CHANNEL_PRINTTABLE		0x13

#define MSG_REMUX		0x20
#define MSG_REMUX_ENABLE		0x21
#define MSG_REMUX_SETPIDMAPPING		0x22
#define MSG_REMUX_CLRPIDMAPPING		0x23
#define MSG_REMUX_PRINTDATA		0x24

int debug_capture(int argc, char *argv[])
{
	int i, wait_times;
	u_int v_u32;
	static int pid;
	static int in_chan = 0;
	int pid_count;
	u_short pid_list[100];
	char data_buf[192];
	char *strbuf = NULL;

	if(argc<2){
		return -1;
	}


	if(strcmp("ctrl", argv[1]) == 0){
		if(argc>3){
			in_chan = strtoul(argv[2], NULL, 16);
			v_u32 = strtoul(argv[3], NULL, 16);
			memcpy(data_buf, &in_chan, 4);
			memcpy(&data_buf[4], &v_u32, 4);
			send_shmmsg(MSG_TSIN_CHANNEL_CAPTURE_CTRL, data_buf, 8);
		}else{
			v_u32 = strtoul(argv[2], NULL, 16);
			memcpy(data_buf, &v_u32, 4);
			send_shmmsg(MSG_TSIN_CAPTURE_CTRL, data_buf, 4);
		}
	}else if(strcmp("print", argv[1]) == 0){
		if(argc>2){
			in_chan = strtoul(argv[2], NULL, 16);
			memcpy(data_buf, &in_chan, 4);
			send_shmmsg(MSG_TSIN_CHANNEL_PRINTTABLE, data_buf, 4);
		}
	}else{

		in_chan = strtol(argv[1], &strbuf,10);
		if((strbuf == NULL) || (strlen(strbuf) == 0)){
			memcpy(data_buf, &in_chan, 4);
			send_shmmsg(MSG_TSIN_CAPTURE, data_buf, 4);
		}
		//printf("in_chan %d, %s, %d \n", in_chan, strbuf, strlen(strbuf));
	}
		
	
}


int debug_mux(int argc, char *argv[])
{
	int i, wait_times;
	u_int v_u32;
	static int pid;
	static int out_chan = 0;
	int pid_count;
	u_short pid_list[100];
	char data_buf[192];
	char *strbuf = NULL;
	int in_chan, in_pid, mux_pid;

	if(argc<2){
		return -1;
	}


	if(strcmp("enable", argv[1]) == 0){
		if(argc>2){
			out_chan = strtol(argv[2], NULL, 16);
		}
		v_u32 = 1;
		memcpy(data_buf, &out_chan, 4);
		memcpy(&data_buf[4], &v_u32, 4);
		send_shmmsg(MSG_REMUX_ENABLE, data_buf, 8);

	}else if(strcmp("disable", argv[1]) == 0){
		if(argc>2){
			out_chan = strtol(argv[2], NULL, 16);
		}
		v_u32 = 0;
		memcpy(data_buf, &out_chan, 4);
		memcpy(&data_buf[4], &v_u32, 4);
		send_shmmsg(MSG_REMUX_ENABLE, data_buf, 8);
	}else if(strcmp("setpid", argv[1]) == 0){
		if(argc<6){
			return 0;
		}
		out_chan = strtol(argv[2], NULL, 16);
		in_chan = strtol(argv[3], NULL, 16);
		in_pid = strtol(argv[4], NULL, 16);
		mux_pid = strtol(argv[5], NULL, 16);

		memcpy(data_buf, &out_chan, 4);
		memcpy(&data_buf[4], &in_chan, 4);
		memcpy(&data_buf[8], &in_pid, 4);
		memcpy(&data_buf[12], &mux_pid, 4);
		send_shmmsg(MSG_REMUX_SETPIDMAPPING, data_buf, 16);
		
	}else if(strcmp("clrpid", argv[1]) == 0){
		if(argc<5){
			return 0;
		}
		out_chan = strtol(argv[2], NULL, 16);
		in_chan = strtol(argv[3], NULL, 16);
		in_pid = strtol(argv[4], NULL, 16);

		memcpy(data_buf, &out_chan, 4);
		memcpy(&data_buf[4], &in_chan, 4);
		memcpy(&data_buf[8], &in_pid, 4);
		send_shmmsg(MSG_REMUX_CLRPIDMAPPING, data_buf, 12);
		
	}else if(strcmp("info", argv[1]) == 0){
		if(argc<3){
			return 0;
		}
		out_chan = strtol(argv[2], NULL, 16);

		memcpy(data_buf, &out_chan, 4);
		send_shmmsg(MSG_REMUX_PRINTDATA, data_buf, 4);
		
	}
	
}


int debug_ts(int argc, char *argv[])
{
	int i, wait_times;
	u_int v_u32 = 0;
	u_int total_bitrate;
	static int pid;
	static int in_chan = 0;
	int mux_chan;
	char data_buf[192];
	char *strbuf = NULL;
	
	if(strcmp("capture", argv[1]) == 0){
		if(argc>2){
			in_chan = strtol(argv[2], NULL, 16);
			send_shmmsg(MSG_TSIN_CAPTURE, &in_chan, 4);
		}
	}else if(strcmp("mux", argv[1]) == 0){

		if(argc<5){
			return -1;
		}

		mux_chan = strtol(argv[2], NULL, 16);
		in_chan = strtol(argv[3], NULL, 16);		
		total_bitrate = strtol(argv[4], NULL, 10);

		if(argc>5){			
			v_u32 = strtol(argv[5], NULL, 10);
		}

		memcpy(data_buf, &mux_chan, 4);

		memcpy(&data_buf[4], &in_chan, 4);
		memcpy(&data_buf[8], &total_bitrate, 4);
		memcpy(&data_buf[12], &v_u32, 4);

		printf("ts  mux : mux_chan %d, in_chan %d, total_bitrate = %d \n", mux_chan, in_chan, v_u32);

		send_shmmsg(MSG_REMUX, data_buf, 16);
		
	}


	
}


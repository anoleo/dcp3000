#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "../utils/thread_pool.h"

#include "mux.h"


//----------------------------------------------------


tsin_capture_control_t capture_control = {
	.auto_capture = TSIN_AUTO_CAPTURE,
	.unlock_reset = TSIN_UNLOCK_RESET,
};


ts_in_channel_t ts_in_channels[ALL_TSIN_CHANNEL_QTY];

int tsin_capture_wait_times = TSIN_CAPTURE_WAIT_TIMES;

static pthread_mutex_t premux_mutex_lock;
#define premux_access_lock();		pthread_mutex_lock(&premux_mutex_lock)
#define premux_access_unlock()		pthread_mutex_unlock(&premux_mutex_lock)

//-----------------------------------------------------------------------


void init_tsin_all_channel(void)
{
	int i, j;
	ts_in_table_t *p_ts_table;

	pthread_mutex_init(&premux_mutex_lock, NULL);

	capture_control.auto_capture = 1;

	for(i=0; i<mux_info.total_tsin_channels; i++){
		
		ts_in_channels[i].channel = i;
		ts_in_channels[i].si_change_flag = 0;
		ts_in_channels[i].dowith_eit_pf = 0;
		ts_in_channels[i].dowith_eit_s = 0;
		ts_in_channels[i].dowith_nit = 0;
		ts_in_channels[i].mux_si_update_flag = 0;
		ts_in_channels[i].ts_flag = 0;
		ts_in_channels[i].capture_flag = 0;
		ts_in_channels[i].ts_rw_lock = 0;
		ts_in_channels[i].capture_buf_lock = UNLOCK;
		
		//ts_in_channels[i].capture_mask = 0x000000ff;
		ts_in_channels[i].capture_buf_offset = 0;
		ts_in_channels[i].ts_bitrate = 0;

		
		ts_in_channels[i].pat_crc = -1;

		ts_in_channels[i].original_encoding = 0;
		ts_in_channels[i].target_encoding = 0;
		ts_in_channels[i].lock_time = -1;

		init_ts_in_table(&ts_in_channels[i].ts_tables);

		
	}
	//init_ts_in_crc_monitor();
}


void free_ts_in_channel(int in_chan)
{

	if(in_chan>=mux_info.total_tsin_channels){
		printf("%free_ts_in_channel, in_chan = %d  over %d , return\n", in_chan, mux_info.total_tsin_channels);
	}
	
	
	reset_ts_in_table((&ts_in_channels[in_chan].ts_tables), 0xff);
	
	
	ts_in_channels[in_chan].ts_flag = 0;
	ts_in_channels[in_chan].ts_bitrate = 0;
	ts_in_channels[in_chan].capture_flag = 0;
	ts_in_channels[in_chan].ts_rw_lock = 0;
	ts_in_channels[in_chan].capture_buf_lock = UNLOCK;
	ts_in_channels[in_chan].capture_buf_offset = 0;

	ts_in_channels[in_chan].si_change_flag = 0;

}

u_int read_tsin_channel_byterate(int in_chan);
int read_tsin_bitrate(int in_chan)
{

	
	u_int bitrate;

	if(in_chan >= mux_info.total_tsin_channels){
		return EXEC_ERR;
	}

	
	bitrate = read_tsin_channel_byterate(in_chan);
	bitrate = (bitrate * 8)/1000;

	if(ts_in_channels[in_chan].ts_bitrate < 1){
		if((bitrate < 1) && (ts_in_channels[in_chan].ts_flag & TSIN_LOCK)){
			ts_in_channels[in_chan].ts_flag &= ~TSIN_LOCK;
			printf(" ts in  [%d] channel unlocked \n", in_chan);
			
		}
	}else{
		if((bitrate >= 1) && ((ts_in_channels[in_chan].ts_flag & TSIN_LOCK) == 0)){
			ts_in_channels[in_chan].ts_flag |= TSIN_LOCK;
			time(&ts_in_channels[in_chan].lock_time);
			printf(" ts in  [%d] channel locked , bitrate = %d \n", in_chan, bitrate);
			
		}
	}


	ts_in_channels[in_chan].ts_bitrate = bitrate;

	return EXEC_OK;
}


void lock_ts_in_channel(int in_chan, u_char rwflag)
{
	while(1)
	{
		if((rwflag == READ_FLAG && ts_in_channels[in_chan].ts_rw_lock >= 0) 
			|| (rwflag == WRITE_FLAG && ts_in_channels[in_chan].ts_rw_lock == 0))
		{
			break;
		}
		usleep(1000);
	}
	
	switch(rwflag){
		case READ_FLAG:
			ts_in_channels[in_chan].ts_rw_lock++;

			break;
		case WRITE_FLAG:
			ts_in_channels[in_chan].ts_rw_lock = -1;

			break;
		default :
			ts_in_channels[in_chan].ts_rw_lock = -1;

			break;
	}
	
}

void unlock_ts_in_channel(int in_chan, u_char rwflag)
{


	switch(rwflag){
		case READ_FLAG:
			ts_in_channels[in_chan].ts_rw_lock--;

			break;
		case WRITE_FLAG:
			ts_in_channels[in_chan].ts_rw_lock = 0;

			break;
		default :
			ts_in_channels[in_chan].ts_rw_lock = 0;

			break;
	}
	
}

void lock_capture_buf(int in_chan)
{
	while(ts_in_channels[in_chan].capture_buf_lock == LOCK)
	{
		usleep(1000);
	}
	
	ts_in_channels[in_chan].capture_buf_lock = LOCK;
	
}

void unlock_capture_buf(int in_chan)
{

	ts_in_channels[in_chan].capture_buf_lock = UNLOCK;
	
}


//==============================================================


u_char get_capture_flag(int in_chan, u_char flag_type)
{
	u_char flag_mask = 0;
		
	switch(flag_type){
		
		case CAPTURE_FLOW_FLAG	:
			flag_mask = 0x0f;
			break;
		case CAPTURE_STATU_FLAG	:
			flag_mask = 0xf0;
			break;
		case CAPTURE_ALL_FLAG	:
			flag_mask = 0xff;
			break;
		default	:
			flag_mask = 0xff;
			break;
	
	}

	return (ts_in_channels[in_chan].capture_flag & flag_mask);

}

u_char set_capture_flag(int in_chan, u_char flag_type, u_char flag)
{
	u_char flag_mask = 0;
	
	switch(flag_type){
		
		case CAPTURE_FLOW_FLAG	:
			flag_mask = 0xf0;
			break;
		case CAPTURE_STATU_FLAG	:
			flag_mask = 0x0f;
			break;
		case CAPTURE_ALL_FLAG	:
			flag_mask = 0x00;
			break;
		default	:
			flag_mask = 0x00;
			break;
	
	}
	
	ts_in_channels[in_chan].capture_flag &= flag_mask;
	ts_in_channels[in_chan].capture_flag |= flag;
		
	return (ts_in_channels[in_chan].capture_flag & (~flag_mask));
}

int check_capture_flag(int in_chan, u_char flag_type, u_char flag)
{
	u_char capture_flag;

	
	if((ts_in_channels[in_chan].ts_flag & TSIN_LOCK) == 0){
		printf("check_capture_flag : %d ts in isn't lock \n", in_chan);
		return 0;
	}
	
	capture_flag = get_capture_flag(in_chan, flag_type);
		
	return ((capture_flag == flag) ? 1 : 0);
}

int enable_pid_mapping(int in_chan,u_short pid)
{
	unsigned int page = 0, offset = 0;
	u_int value;

	if(in_chan >= mux_info.total_tsin_channels){
		printf("in_chan %d over than the max channel %d \n", in_chan, mux_info.total_tsin_channels);
		return EXEC_ERR;
	}

	page = DDR_Page_PidLookup(in_chan);
	capture_DebugPrintf("enable_pid_mapping:page = 0x%x\n", page);

	pid &= 0x1fff;
	
	WRITE_FPGA_RAM_PAGE(page);
	
	offset = DDR_Offset_PidLookup(in_chan, pid);
	offset += PID_MAPPING_SW_BIT/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
	capture_DebugPrintf("enable_pid_mapping:offset = 0x%x\n", offset);
	value = FPGA_MUX_DDR_READ(offset);

	value |= (1 << (PID_MAPPING_SW_BIT%DDR_BIT_WIDTH));
	FPGA_MUX_DDR_WRITE(offset, value);

	UNLOCK_FPGA_RAM_PAGE(page);

	capture_DebugPrintf(" %d in_chan , enable_pid_mapping , pid = 0x%04x \n", in_chan, pid);

	return EXEC_OK;
}

int disable_pid_mapping(int in_chan,u_short pid)
{
	unsigned int page = 0, offset = 0;
	u_int value;

	if(in_chan >= mux_info.total_tsin_channels){
		printf("in_chan %d over than the max channel %d \n", in_chan, mux_info.total_tsin_channels);
		return EXEC_ERR;
	}

	page = DDR_Page_PidLookup(in_chan);

	capture_DebugPrintf("disable_pid_mapping:page = 0x%x\n", page);

	pid &= 0x1fff;
	
	WRITE_FPGA_RAM_PAGE(page);
	
	offset = DDR_Offset_PidLookup(in_chan, pid);
	offset += PID_MAPPING_SW_BIT/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
	capture_DebugPrintf("disable_pid_mapping:offset = 0x%x\n", offset);
	value = FPGA_MUX_DDR_READ(offset);

	value &= ~(1 << (PID_MAPPING_SW_BIT%DDR_BIT_WIDTH));
	FPGA_MUX_DDR_WRITE(offset, value);

	UNLOCK_FPGA_RAM_PAGE(page);


	capture_DebugPrintf(" %d in_chan , disable_pid_mapping , pid = 0x%04x \n", in_chan, pid);

	return EXEC_OK;
}


void init_host_capture(int in_chan)
{
	int pidnum, i = 0;
	int offset;
	u_short page;
	u_int value;

	page = DDR_Page_PidLookup(in_chan);
	
	WRITE_FPGA_RAM_PAGE(page);
	
	for(pidnum=0; pidnum<8192; pidnum++){

		//init host capture
		offset =  DDR_Offset_PidLookup(in_chan, pidnum);

		offset += PID_CAPTURE_SW_BIT/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
		value = FPGA_MUX_DDR_READ(offset);

		value &= ~(1 << (PID_CAPTURE_SW_BIT%DDR_BIT_WIDTH));
		FPGA_MUX_DDR_WRITE(offset, value);

	}

	UNLOCK_FPGA_RAM_PAGE(page);
	
}

void init_pid_lookup(int in_chan)
{
	int pidnum, i = 0;
	int offset, start_off, end_off;
	u_short page;

	page = DDR_Page_PidLookup(in_chan);
	
	WRITE_FPGA_RAM_PAGE(page);
	
	for(pidnum=0; pidnum<8192; pidnum++){

		offset =  DDR_Offset_PidLookup(in_chan, pidnum);

		//init mux pid out switch and  host capture
		start_off = PID_MUX_OUT_SW_BIT_S/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
		end_off = PID_MUX_MAPPING_BIT_S/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
		for(i=start_off; i<=end_off; i+=(DDR_BIT_WIDTH/8)){
			FPGA_MUX_DDR_WRITE(offset+i, 0x0);
		}

		/*
		//init mux pid mapping
		start_off = PID_MUX_MAPPING_BIT_S/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
		end_off = PID_MUX_MAPPING_BIT_E/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
		for(i=start_off; i<=end_off; i+=(DDR_BIT_WIDTH/8)){
			FPGA_MUX_DDR_WRITE(offset+i, (-1UL >> (32-DDR_BIT_WIDTH)));
		}

		//init host capture
		start_off = PID_CAPTURE_SW_BIT/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
		end_off = (PID_MUX_MAPPING_BIT_S-1)/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
		for(i=start_off; i<=end_off; i+=(DDR_BIT_WIDTH/8)){
			FPGA_MUX_DDR_WRITE(offset+i, 0x0);
		}
		//*/
	}

	UNLOCK_FPGA_RAM_PAGE(page);
	
}

int set_host_capture(int in_chan,u_short pid)
{
	unsigned int page = 0, offset = 0;
	u_int value;

	if(in_chan >= mux_info.total_tsin_channels){
		printf("in_chan %d over than the max channel %d \n", in_chan, mux_info.total_tsin_channels);
		return EXEC_ERR;
	}

	page = DDR_Page_PidLookup(in_chan);
	capture_DebugPrintf("set_host_capture:page = 0x%x\n", page);

	pid &= 0x1fff;
	
	WRITE_FPGA_RAM_PAGE(page);
	
	offset = DDR_Offset_PidLookup(in_chan, pid);
	offset += PID_CAPTURE_SW_BIT/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
	capture_DebugPrintf("set_host_capture:offset = 0x%x\n", offset);
	value = FPGA_MUX_DDR_READ(offset);

	value |= (1 << (PID_CAPTURE_SW_BIT%DDR_BIT_WIDTH));
	FPGA_MUX_DDR_WRITE(offset, value);

	UNLOCK_FPGA_RAM_PAGE(page);

	capture_DebugPrintf(" %d in_chan , set_host_capture , pid = 0x%04x \n", in_chan, pid);

	return EXEC_OK;
}

int close_host_capture(int in_chan,u_short pid)
{
	unsigned int page = 0, offset = 0;
	u_int value;

	if(in_chan >= mux_info.total_tsin_channels){
		printf("in_chan %d over than the max channel %d \n", in_chan, mux_info.total_tsin_channels);
		return EXEC_ERR;
	}

	page = DDR_Page_PidLookup(in_chan);

	capture_DebugPrintf("close_host_capture:page = 0x%x\n", page);

	pid &= 0x1fff;
	
	WRITE_FPGA_RAM_PAGE(page);
	
	offset = DDR_Offset_PidLookup(in_chan, pid);
	offset += PID_CAPTURE_SW_BIT/DDR_BIT_WIDTH * (DDR_BIT_WIDTH/8);
	capture_DebugPrintf("close_host_capture:offset = 0x%x\n", offset);
	value = FPGA_MUX_DDR_READ(offset);

	value &= ~(1 << (PID_CAPTURE_SW_BIT%DDR_BIT_WIDTH));
	FPGA_MUX_DDR_WRITE(offset, value);

	UNLOCK_FPGA_RAM_PAGE(page);


	capture_DebugPrintf(" %d in_chan , close_host_capture , pid = 0x%04x \n", in_chan, pid);

	return EXEC_OK;
}


void enable_capture_irq(int in_chan)
{

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	FPGA_MUX_WRITE(PREMUX_IRQ_CAPTURE_ENABLE_REG, 1);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

}


void enable_capture_in_channel(int in_chan)
{
	u_int value;
	premux_access_lock();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	//FPGA_MUX_WRITE(PREMUX_IRQ_CAPTURE_ENABLE_REG, 1);
	value = FPGA_MUX_READ(PREMUX_TSIN_ENABLE_REG);
	value |= 1<<PREMUX_CHANNEL_ENABLE_BIT;
	value |= 1<<PREMUX_CHANNEL_CAPTURE_ENABLE_BIT;
	capture_DebugPrintf(" %d in_chan , set PREMUX_TSIN_ENABLE_REG %#x \n", in_chan, value);
	
	FPGA_MUX_WRITE(PREMUX_TSIN_ENABLE_REG, value);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	premux_access_unlock();

	
	capture_DebugPrintf(" %d in_chan , enable_capture_in_channel \n", in_chan);

}


void disable_capture_in_channel(int in_chan)
{
	u_int value;
	premux_access_lock();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	//FPGA_MUX_WRITE(PREMUX_IRQ_CAPTURE_ENABLE_REG, 0);
	value = FPGA_MUX_READ(PREMUX_TSIN_ENABLE_REG);
	//value &= ~(1<<PREMUX_CHANNEL_ENABLE_BIT);
	value &= ~(1<<PREMUX_CHANNEL_CAPTURE_ENABLE_BIT);
	
	capture_DebugPrintf(" %d in_chan , set PREMUX_TSIN_ENABLE_REG %#x \n", in_chan, value);
	FPGA_MUX_WRITE(PREMUX_TSIN_ENABLE_REG, value);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	premux_access_unlock();

	capture_DebugPrintf(" %d in_chan , disable_capture_in_channel \n", in_chan);

}


void enable_tsin_channel(int in_chan)
{
	u_int value;
	premux_access_lock();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	value = FPGA_MUX_READ(PREMUX_TSIN_ENABLE_REG);
	value |= 1<<PREMUX_CHANNEL_ENABLE_BIT;
	capture_DebugPrintf(" %d in_chan , set PREMUX_TSIN_ENABLE_REG %#x \n", in_chan, value);
	FPGA_MUX_WRITE(PREMUX_TSIN_ENABLE_REG, value);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	premux_access_unlock();

	
	capture_DebugPrintf(" enable ts in %d channel \n", in_chan);

}


void disable_tsin_channel(int in_chan)
{
	u_int value;
	premux_access_lock();

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);

	value = FPGA_MUX_READ(PREMUX_TSIN_ENABLE_REG);
	value &= ~(1<<PREMUX_CHANNEL_ENABLE_BIT);
	
	capture_DebugPrintf(" %d in_chan , set PREMUX_TSIN_ENABLE_REG %#x \n", in_chan, value);
	FPGA_MUX_WRITE(PREMUX_TSIN_ENABLE_REG, value);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	premux_access_unlock();

	capture_DebugPrintf(" disable ts in %d channel \n", in_chan);

}

int read_capture_packets_count(int in_chan)
{
	int packet_count;

	premux_access_lock();
	
	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	packet_count = FPGA_MUX_READ(PREMUX_BUF_PACKETS_REG);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	premux_access_unlock();
	
	capture_DebugPrintf(" %d in_chan , read_capture_packets_count, packets_count = %d \n"
		, in_chan, packet_count);

	return packet_count;
}


int decrease_capture_packets(int in_chan)
{
	int packet_count;

	premux_access_lock();
	
	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	packet_count = FPGA_MUX_READ(PREMUX_BUF_PACKETS_REG);
	if(packet_count>0){
		FPGA_MUX_WRITE(PREMUX_BUF_DECREASE_REG, 1);
		packet_count--;
	}
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	premux_access_unlock();

	//usleep(1000);

	capture_DebugPrintf(" %d in_chan , decrease_capture_packets, packets_count = %d \n"
		, in_chan, packet_count);

	return packet_count;
}


int clear_capture_buffer(int in_chan)
{

	int page, i=0, j;
	int offset;

	page = DDR_Page_CaptureBuffer(in_chan);
	
	lock_capture_buf(in_chan);

	premux_access_lock();
	
	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	FPGA_MUX_WRITE(PREMUX_BUF_CLEAR_REG, 0x1);
	//FPGA_MUX_WRITE(PREMUX_BUF_CLEAR_REG, 0x0);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	premux_access_unlock();
	
	//while(decrease_capture_packets(in_chan));

	offset = DDR_Offset_CaptureBuffer(in_chan);
	
	while(i<CAPTURE_BUF_BYTES_IN_CHAN){
	
		WRITE_FPGA_RAM_PAGE(page);
		for(j=0; j<0x100; j++){
			FPGA_MUX_DDR_WRITE(offset+i, (-1UL >> (32-DDR_BIT_WIDTH)));
			i+=(DDR_BIT_WIDTH/8);
		}
		UNLOCK_FPGA_RAM_PAGE(page);
		
	}

	
	ts_in_channels[in_chan].capture_buf_offset = offset;
	
		
	unlock_capture_buf(in_chan);

	capture_DebugPrintf(" %d in_chan , clear_capture_buffer, capture_buf_offset = %d \n"
		, in_chan, ts_in_channels[in_chan].capture_buf_offset);


	return EXEC_OK;
}

int reset_capture_buffer(int in_chan)
{
	int offset;
	
	capture_DebugPrintf(in_chan);

	premux_access_lock();
	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	FPGA_MUX_WRITE(PREMUX_BUF_CLEAR_REG, 0x1);
	//FPGA_MUX_WRITE(PREMUX_BUF_CLEAR_REG, 0x0);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	premux_access_unlock();

	//while(decrease_capture_packets(in_chan));

	offset = DDR_Offset_CaptureBuffer(in_chan);
	
	ts_in_channels[in_chan].capture_buf_offset = offset;
		
	unlock_capture_buf(in_chan);

	capture_DebugPrintf(" %d in_chan , reset_capture_buffer, capture_buf_offset = %d \n"
		, in_chan, ts_in_channels[in_chan].capture_buf_offset);
	
	return ts_in_channels[in_chan].capture_buf_offset;
}

int get_capture_buffer_offset(int in_chan)
{
	return ts_in_channels[in_chan].capture_buf_offset;
}

int shift_offset_to_next_packet(int in_chan)
{
	u_int offset;
	lock_capture_buf(in_chan);

	ts_in_channels[in_chan].capture_buf_offset += 192;
	offset = DDR_Offset_CaptureBuffer(in_chan);
	
	if(ts_in_channels[in_chan].capture_buf_offset >= (CAPTURE_BUF_BYTES_IN_CHAN+offset)){
		ts_in_channels[in_chan].capture_buf_offset = offset;
	}

	unlock_capture_buf(in_chan);

	capture_DebugPrintf(" %d in_chan , shift_offset_to_next_packet, capture_buf_offset = %d \n"
		, in_chan, ts_in_channels[in_chan].capture_buf_offset);

	return ts_in_channels[in_chan].capture_buf_offset;
}

void end_capture_packet(int in_chan)
{
	u_int value;
	
	lock_capture_buf(in_chan);

	premux_access_lock();
	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	FPGA_MUX_WRITE(PREMUX_TSIN_CHANNEL_REG, in_chan);
	
	//FPGA_MUX_WRITE(PREMUX_IRQ_CAPTURE_ENABLE_REG, 0);
	value = FPGA_MUX_READ(PREMUX_TSIN_ENABLE_REG);
	//value &= ~(1<<PREMUX_CHANNEL_ENABLE_BIT);
	value &= ~(1<<PREMUX_CHANNEL_CAPTURE_ENABLE_BIT);
	FPGA_MUX_WRITE(PREMUX_TSIN_ENABLE_REG, value);
	
	FPGA_MUX_WRITE(PREMUX_BUF_CLEAR_REG, 0x1);
	//FPGA_MUX_WRITE(PREMUX_BUF_CLEAR_REG, 0x0);
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
	premux_access_unlock();
		
	//ts_in_channels[in_chan].capture_flag = CAPTURE_END;

	usleep(1000);

	while(decrease_capture_packets(in_chan));
	unlock_capture_buf(in_chan);

	capture_DebugPrintf(" %d in_chan , end_capture_packet \n", in_chan);

}

int read_fpga_packet_2(int in_chan, char *packet_buf)
{
	int page, i , j;
	u_int tempval = 0;
	int offset;
	int packet_count;
	int ret = EXEC_ERR;
	char *p_buf;

	page = DDR_Page_CaptureBuffer(in_chan);

	
//	memset(packet_buf, 188, 0x0);
	lock_capture_buf(in_chan);
	
	offset = get_capture_buffer_offset(in_chan);

	
	WRITE_FPGA_RAM_PAGE(page);

	for(i=0; i<4; i+=(DDR_BIT_WIDTH/8)){
		tempval = FPGA_MUX_DDR_READ(offset+i);
		for(j=0; j<(DDR_BIT_WIDTH/8); j++){
			packet_buf[i+j] = tempval&0xff;
			tempval >>= 8;
		}
	}
	UNLOCK_FPGA_RAM_PAGE(page);

	p_buf = &packet_buf[0];
		
	if((p_buf[0] & 0xff) != SYNC_BYTE){
		capture_InfoPrintf("%d channel packet_buf[0] != SYNC_BYTE\n", in_chan);
		goto End_read_packet;
	}

	if(p_buf[1] & 0x80){
		capture_InfoPrintf("%d channel transport_error \n", in_chan);
		goto End_read_packet;
	}
			

	if(p_buf[3] & 0xc0){  
		capture_InfoPrintf("ts scrambling\n");
	}		 //scrambling 
	
	if(p_buf[3] & 0x10){// package has payload


		if(p_buf[3] & 0x20)	// jump out adaptation field
		{
			if(((p_buf[4] & 0xff) + 5) > PACKAGE_MAX_LEN){
				capture_InfoPrintf("%d channel adaptation field bytes count overflow \n", in_chan);
				goto End_read_packet;
			}
		}
		
	}else{
		capture_InfoPrintf("%d channel package has not payload \n", in_chan);
		goto End_read_packet;
	}

	WRITE_FPGA_RAM_PAGE(page);

	for(i=4; i<188; i+=(DDR_BIT_WIDTH/8)){
		tempval = FPGA_MUX_DDR_READ(offset+i);
		for(j=0; j<(DDR_BIT_WIDTH/8); j++){
			packet_buf[i+j] = tempval&0xff;
			tempval >>= 8;
		}
	}
	UNLOCK_FPGA_RAM_PAGE(page);

	ret = EXEC_OK;
		
	End_read_packet :
		
	packet_count = decrease_capture_packets(in_chan);
	
	unlock_capture_buf(in_chan);
	shift_offset_to_next_packet(in_chan);
	
	capture_DebugPrintf("read_fpga_packet  %d channel : page %d , offset %#x \n", in_chan, page, offset);
	capture_DebugPrintf(" %d in_chan , packet count = 0x%x\n", in_chan, packet_count);



	return ret;
}


int read_fpga_packet(int in_chan, char *packet_buf)
{
	int page, i , j;
	u_int tempval = 0;
	int offset;
	int packet_count;


	page = DDR_Page_CaptureBuffer(in_chan);
/*
	if(read_capture_packets_count(in_chan) == 0){
		printf(" %d in_chan , packet count = 0x%x\n", in_chan, FPGA_RAM_READ(PREMUX_BUF_PACKETS_REG));
		return EXEC_NULL;
	}
*/
	
	memset(packet_buf, 188, 0x0);
	lock_capture_buf(in_chan);
	
	offset = get_capture_buffer_offset(in_chan);

	
	WRITE_FPGA_RAM_PAGE(page);

	for(i=0; i<188; i+=(DDR_BIT_WIDTH/8)){
		tempval = FPGA_MUX_DDR_READ(offset+i);
		for(j=0; j<(DDR_BIT_WIDTH/8); j++){
			packet_buf[i+j] = tempval&0xff;
			tempval >>= 8;
		}
	}
	UNLOCK_FPGA_RAM_PAGE(page);
		
	packet_count = decrease_capture_packets(in_chan);
	
	unlock_capture_buf(in_chan);
	shift_offset_to_next_packet(in_chan);
	
	capture_DebugPrintf("read_fpga_packet  %d channel : page %d , offset %#x \n", in_chan, page, offset);
	capture_DebugPrintf(" %d in_chan , packet count = 0x%x\n", in_chan, packet_count);
	//print_bytes(packet_buf,188);


	return EXEC_OK;
}

void set_eit_section_num_maskbit(int in_chan, u_char section_number)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	eit_sub_table_t *p_sub_eit;
	int u8_bits;

	p_sub_eit = p_ts_table->cur_eit_sub_table;

	u8_bits = sizeof(u_char) * 8;
	p_sub_eit->section_num_mask[section_number/u8_bits] |= (1<<(section_number%u8_bits));
		
}
void set_section_num_maskbit(int in_chan, u_short pid, u_char section_number)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;

	capture_DebugPrintf(" %d in_chan , set_section_num_maskbit, pid = %#x, section_number = %#x \n"
		, in_chan, pid, section_number);
	
	switch(pid){
		case PAT_PID :
			p_ts_table->pat_section_num_mask |= (1<<section_number);
			break;
		case CAT_PID :
			p_ts_table->cat_section_num_mask |= (1<<section_number);
			break;
		case SDT_PID :
			p_ts_table->sdt_section_num_mask |= (1<<section_number);
			break;
		case NIT_PID :
			p_ts_table->nit_section_num_mask |= (1<<section_number);
			break;
		case EIT_PID :
			set_maskbit(p_ts_table->cur_eit_sub_table->section_num_mask, section_number);
		default :
			break;
	}
	
}


void clean_ts_table_section_num_maskbit(int in_chan, u_short pid, u_char section_number)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;

	capture_DebugPrintf(" %d in_chan , clean_section_num_maskbit,pid = %#x, section_number = %#x \n"
		, in_chan, pid, section_number);
	
	switch(pid){
		case PAT_PID :
			p_ts_table->pat_section_num_mask &= ~(1<<section_number);
			break;
		case CAT_PID :
			p_ts_table->cat_section_num_mask &= ~(1<<section_number);
			break;
		case SDT_PID :
			p_ts_table->sdt_section_num_mask &= ~(1<<section_number);
			break;
		case NIT_PID :
			p_ts_table->nit_section_num_mask &= ~(1<<section_number);
			break;
		case EIT_PID :
			//clean_eit_section_numb_maskbit(in_chan, section_number);
		default :
			break;
	}
}


int judge_in_the_eit_chose_range(int in_chan, u_short table_id)
{

	if((table_id == EIT_pf_TABLEID) && (ts_in_channels[in_chan].dowith_eit_pf)){
		//index = 0;
	}else if((table_id>=EIT_s_0_3_TABLEID && table_id<=EIT_s_60_63_TABLEID) && (ts_in_channels[in_chan].dowith_eit_s)){
		//index = p_section_head->table_id - EIT_s_0_3_TABLEID + 1;
	}else{
		capture_DebugPrintf("current eit table id [%#x] not in the range, not need deal!\n", table_id);
		return 0;
	}

	return 1;
}


eit_table_t *find_ts_table_eit(int in_chan, u_short service_id)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	program_t *p_program;

	capture_DebugPrintf("find_ts_table_eit : %d in_chan ,  service_id = %#x \n", in_chan, service_id);

	p_program = find_ts_in_program(p_ts_table->pmt_tables, service_id);

	while(p_program){
		if(service_id == p_program->program_number){
			return  p_program->eit_tables;
		}
		p_program = p_program->next;
	}

	return NULL;
}

eit_sub_table_t *find_eit_sub_table(eit_table_t *p_eit, u_char table_id)
{
	
	eit_sub_table_t *p_sub_table;


	if(p_eit==NULL){
		return NULL;
	}

	capture_DebugPrintf("find eit service_id = %d , table_id : %#x, \n", p_eit->service_id, table_id);
	
	p_sub_table = p_eit->sub_tables;
	while(p_sub_table){
		if(p_sub_table->table_id == table_id){
			return p_sub_table;
		}
		p_sub_table = p_sub_table->next;
	}

	return NULL;
}

eit_segment_t *find_eit_segment(eit_sub_table_t *p_sub_table, u_short section_number)
{
	eit_segment_t *p_segment;

	if(p_sub_table == NULL){
		return NULL;
	}

	p_segment = p_sub_table->segments;
	while(p_segment){
		if(p_segment->segment_num == section_number/8){
			return p_segment;
		}
		p_segment = p_segment->next;
	}

	return NULL;
}

int check_eit_seciton_isComplete(int in_chan)
{
	
	int i, is_finish = 1;
	eit_table_t *p_eit;
	eit_sub_table_t *p_sub_table;
	eit_segment_t *p_segment;
	ts_in_channel_t *p_tsin_chan;
	ts_in_table_t *p_ts_table;
	pmt_table_t *p_pmt_table;
	program_t *p_program;
	int last_table_id;

	p_tsin_chan = &ts_in_channels[in_chan];

	if((p_tsin_chan->dowith_eit_pf == 0) && (p_tsin_chan->dowith_eit_s == 0)){
		goto finish_end;
	}
	
	p_ts_table = &p_tsin_chan->ts_tables;

	p_pmt_table = p_ts_table->pmt_tables;

	while(p_pmt_table){
		
		p_program = p_pmt_table->p_program;

		while(p_program){
			//printf("............check eit completed : %d service_id, isComplete %d \n", p_eit->service_id, p_eit->isComplete);

			if((p_tsin_chan->dowith_eit_pf && p_program->EIT_pf_flag) || (p_tsin_chan->dowith_eit_pf && p_program->EIT_s_flag)){
				if(p_program->eit_tables == NULL){
					is_finish = 0;
					goto finish_end;
				}
			}else{
				p_program = p_program->next;
				continue;
			}

			p_eit = p_program->eit_tables;
			if(p_eit->isComplete==0){
				p_sub_table = p_eit->sub_tables;
				while(p_sub_table){

					//printf("////////////////check eit completed : %#x table_id, %d service_id , isComplete = %d \n", p_sub_table->table_id, p_eit->service_id, p_sub_table->isComplete);
					
					if(p_sub_table->isComplete){
						p_sub_table = p_sub_table->next;
						continue;
					}

					p_segment = p_sub_table->segments;
					while(p_segment){
						
						//printf("============check eit completed :  %d segment_num, section_count = %d, segment_last_section_number = %#x \n", p_segment->segment_num, p_segment->section_count, p_segment->segment_last_section_number);
						if(p_segment->section_count < p_segment->segment_last_section_number%8 + 1){
							//printf("================check eit completed :  ============0============\n");
				
							is_finish = 0;
							goto finish_end;
						}
						//printf("================check eit completed :  ============1============\n");
						p_segment = p_segment->next;
					}

					//printf("//////////////check eit completed :  %#x table_id, segment_count = %d, last_section_number = %#x \n", p_sub_table->table_id, p_sub_table->segment_count, p_sub_table->last_section_number);
				
					//print_bytes(p_sub_table->section_num_mask, SECTION_NUM_MASK_SIZE);
					if(p_sub_table->segment_count < (p_sub_table->last_section_number+1)/8 + 1){
						//printf("//////////////check eit completed :  ////////////// 0 /////////////\n");
				
						is_finish = 0;
						goto finish_end;
					}else{
					
						//printf("//////////////check eit completed :  ////////////// 1 /////////////\n");
						p_sub_table->isComplete = 1;
					}

					p_sub_table = p_sub_table->next;
				}

				last_table_id = p_eit->last_table_id;
				if(last_table_id > EIT_s_60_63_TABLEID){
					last_table_id = EIT_s_60_63_TABLEID;
				}

				//printf("--------------------check eit completed :  %d service_id, sub_table_count = %d, last_table_id = %#x \n", p_eit->service_id, p_eit->sub_table_count, p_eit->last_table_id);
				if((p_tsin_chan->dowith_eit_s == 0 || p_program->EIT_s_flag == 0) && (p_tsin_chan->dowith_eit_pf == 0 || p_program->EIT_pf_flag == 0)){
					p_eit->isComplete = 1;
					//printf("--------------------check eit completed :  ------0----------- \n");
				
				}else if((p_tsin_chan->dowith_eit_s == 0 || p_program->EIT_s_flag == 0) && (p_eit->sub_table_count == 1)){
					p_eit->isComplete = 1;
					//printf("--------------------check eit completed :  ------0----------- \n");
				
				}else if((p_program->EIT_s_flag && p_program->EIT_s_flag) && (p_eit->sub_table_count == last_table_id - EIT_s_TABLEID + 2)){
					p_eit->isComplete = 1;
					//printf("--------------------check eit completed :  ------1----------- \n");
				}else{
					//printf("--------------------check eit completed :  ------2----------- \n");
					is_finish = 0;
					p_eit->isComplete = 0;
					goto finish_end;
				}
				
			}
			p_program = p_program->next;
		}

		p_pmt_table = p_pmt_table->next;
	}
	
	

	//printf("%.............................check_eit_seciton_isComplete : check section complete end------------------------------------\n");

	finish_end:
	if(is_finish){
		printf("```````````capture eit  :  ````````is finish````````` \n");
			
		//*
		//if(p_ts_table->eit_wait_times > 250){
			set_maskbit(&p_ts_table->complete_mask, EIT_USED);
			close_host_capture(in_chan, EIT_PID);
		//}
		//*/
		//printf(".............................%d in_chan capture finished------------------------------------\n", in_chan);
	}else{
		//printf("```````````check eit completed :  ```````````isn't finish````````` \n");
		
	}

	return is_finish;
}

int isExist_ts_section(int in_chan, u_short pid, section_head_t *p_section_head)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	eit_segment_t *p_segment;
	eit_sub_table_t *p_sub_table;
	eit_table_t *p_eit;
	program_t *p_program;

	capture_DebugPrintf(" %d in_chan , isExist_section_num, pid = %#x, table_id = %#x, section_number = %#x \n"
		, in_chan, pid, p_section_head->table_id, p_section_head->section_number);
	
	switch(p_section_head->table_id){
		case PAT_TABLEID:
			return (p_ts_table->pat_section_num_mask & (1<<p_section_head->section_number));
			break;
		case CAT_TABLEID :
			return (p_ts_table->cat_section_num_mask & (1<<p_section_head->section_number));
			break;
		case SDT_TABLEID :
			return (p_ts_table->sdt_section_num_mask & (1<<p_section_head->section_number));
			break;
		case NIT_TABLEID :
			return (p_ts_table->nit_section_num_mask & (1<<p_section_head->section_number));
			break;
		case PMT_TABLEID :
			p_program = find_ts_in_program(p_ts_table->pmt_tables, p_section_head->sub_table_id);
			if(p_program == NULL || p_program->pmt_section == NULL){
				return 0;
			}
			break;
		/*
		case EIT_pf_TABLEID :

		case EIT_s_0_3_TABLEID :
		case EIT_s_4_7_TABLEID :
		case EIT_s_56_59_TABLEID :
		case EIT_s_60_63_TABLEID :
		//*/
			
		default :
			switch(pid){
				case SDT_PID :
					p_ts_table->sdt_cc = 0xff;
					break;
				case NIT_PID :
					p_ts_table->nit_cc = 0xff;
					break;
				case EIT_PID :
					if(judge_in_the_eit_chose_range(in_chan, p_section_head->table_id)){

						/*
						if(!check_eit_capture_cond(in_chan, p_section_head->table_id, p_section_head->sub_table_id)){
							return 1;
						}
						//*/

						p_eit = find_ts_table_eit(in_chan, p_section_head->sub_table_id);
						if(p_eit == NULL){
							#if EIT_CAPTURE
							printf("the  %d service_id  eit table not exist\n", p_section_head->sub_table_id);
							#endif
							return 0;
						}
						
						//if(p_section_head->table_id == EIT_pf_TABLEID || p_section_head->table_id <= p_eit->last_table_id// && 
							//p_section_head->section_number <= p_eit->segment_last_section_number
						//){
							p_sub_table = find_eit_sub_table(p_eit, p_section_head->table_id);
							if(p_sub_table){
								
								if(check_maskbit(p_sub_table->section_num_mask, p_section_head->section_number)){
									#if EIT_CAPTURE
									printf("%#x table_id, %d service_id, section_number[ %#x ] is arrival again \n",p_section_head->table_id, p_section_head->sub_table_id, p_section_head->section_number);
									#endif
									
									//*/
									return 1;
								}else{
									/*
									p_segment = find_eit_segment(p_sub_table, p_section_head->section_number);
									if(p_segment == NULL){
										return 0;
									}else if(p_section_head->section_number <= p_segment->segment_last_section_number){
										return 0;
									}
									//*/
									return 0;
								}
							}else{
								#if EIT_CAPTURE
								printf("the sub_table #x table_id, %d service_id in the eit table not exist\n", p_section_head->table_id, p_section_head->sub_table_id);
								#endif
								return 0;
							}

						/*
						}
						else{
							#if DEBUG_CAPTURE
							printf("current section table_id over than the eit last_table_id\n");// or section_number over than the segment_last_section_number\n");
							#endif
						}
						//*/
						
					}else{
						capture_DebugPrintf("current section table_id over than the eit chose range\n");
					}
					break;
				default :
					break;
			}
			return 1;
	}

}

section_t *prejudge_ts_table_section(int in_chan, u_short pid, u_char packet_cc)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	pmt_table_t *p_pmt;
	section_t *p_section = NULL;

	capture_DebugPrintf(" %d in_chan , prejudge_ts_table_section, pid = %#x, packet_cc = %d \n"
		, in_chan, pid, packet_cc);
	
	switch(pid){
		case PAT_PID :
			
			p_section = p_ts_table->cur_pat_section;

			if((p_ts_table->pat_cc != 0xff) && ((p_ts_table->pat_cc != packet_cc - 1) && (p_ts_table->pat_cc != packet_cc + 15))){

				if(p_section){
					p_ts_table->pat_section = p_section->next;
					free(p_section);
				}
				p_section = NULL;
				p_ts_table->cur_pat_section = NULL;
				
			}
			
			p_ts_table->pat_cc = packet_cc;
			break;
			
		case CAT_PID :

			p_section = p_ts_table->cur_cat_section;

			if((p_ts_table->cat_cc != 0xff) && ((p_ts_table->cat_cc != packet_cc - 1) && (p_ts_table->cat_cc != packet_cc + 15))){

				if(p_section){
					p_ts_table->cat_section = p_section->next;
					free(p_section);
				}
				p_section = NULL;
				p_ts_table->cur_cat_section = NULL;
			}
			
			p_ts_table->cat_cc = packet_cc;
			break;
			
		case SDT_PID :
			
			p_section = p_ts_table->cur_sdt_section;
				
			if((p_ts_table->sdt_cc != 0xff) && ((p_ts_table->sdt_cc != packet_cc - 1) && (p_ts_table->sdt_cc != packet_cc + 15))){

				capture_DebugPrintf(" %d in_chan , prejudge_ts_table_section, sdt cc is not continuous \n");

				if(p_section){
					p_ts_table->sdt_section = p_section->next;
					free(p_section);
					capture_DebugPrintf(" %d in_chan , prejudge_ts_table_section, delete sdt section \n");
				}
				p_section = NULL;
				p_ts_table->cur_sdt_section = NULL;
			}
			
			p_ts_table->sdt_cc = packet_cc;
			break;
			
		case NIT_PID :

			p_section = p_ts_table->cur_nit_section;

			if((p_ts_table->nit_cc != 0xff) && ((p_ts_table->nit_cc != packet_cc - 1) && (p_ts_table->nit_cc != packet_cc + 15))){

				if(p_section){
					p_ts_table->nit_section = p_section->next;
					free(p_section);
				}
				p_section = NULL;
				p_ts_table->cur_nit_section = NULL;
			}
			
			p_ts_table->nit_cc = packet_cc;
			break;
			
		case EIT_PID :

			if(p_ts_table->cur_eit_sub_table && p_ts_table->cur_eit_sub_table->cur_segment){
				
				p_section = (section_t *)(p_ts_table->cur_eit_sub_table->cur_segment->section);

				if((p_ts_table->eit_cc != 0xff) && ((p_ts_table->eit_cc != packet_cc - 1) && (p_ts_table->eit_cc != packet_cc + 15))){
					
					if(p_section){
						p_ts_table->cur_eit_sub_table->cur_segment->section = (eit_section_t *)p_section->next;
						free(p_section);
						p_ts_table->cur_eit_sub_table->cur_segment = NULL;
					}
					
					p_ts_table->cur_eit_sub_table = NULL;
					
				}
				
			}
		
			p_ts_table->eit_cc = packet_cc;
			break;
			
		default :
			
			p_pmt = find_ts_pmt_table(p_ts_table, pid);
			
			if(p_pmt && p_pmt->cur_program){

				p_section = p_pmt->cur_program->pmt_section;
				
				if((p_pmt->packet_cc != 0xff) && ((p_pmt->packet_cc != packet_cc - 1) && (p_pmt->packet_cc != packet_cc + 15))){
	
					if(p_section){
						p_pmt->cur_program->pmt_section = p_section->next;
						free(p_section);
					}
					p_section = NULL;
					p_pmt->cur_program = NULL;
				}
				
				p_pmt->packet_cc = packet_cc;
		
			}
			
			break;
	}

	return p_section;
}


void del_ts_table_eit_section(int in_chan)
{
	int index;
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	eit_sub_table_t *p_sub_eit = NULL;
	eit_section_t *p_section = NULL;

	#if EIT_CAPTURE
	printf("del_ts_table_eit_section   %d in_chan", in_chan);
	#endif

	p_sub_eit = p_ts_table->cur_eit_sub_table;
	if(p_sub_eit && p_sub_eit->cur_segment){
		p_section = p_sub_eit->cur_segment->section;
	}
	
	if(p_section){
		p_sub_eit->cur_segment->section = p_section->next;
		free(p_section);
		p_sub_eit->cur_segment = NULL;
	}
	
	p_ts_table->cur_eit_sub_table = NULL;
	
}

void del_ts_table_section(int in_chan, u_short pid)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	section_t *p_section;
	pmt_table_t *p_pmt;

	capture_DebugPrintf(" %d in_chan , del_ts_table_section, pid = 0x%04x \n"
		, in_chan, pid);
	
	switch(pid){
		case PAT_PID :
			p_section = p_ts_table->cur_pat_section;
			if(p_section){
				p_ts_table->pat_section = p_section->next;
				free(p_section);
			}
			p_ts_table->cur_pat_section = NULL;
			break;
		case CAT_PID :
			p_section = p_ts_table->cur_cat_section;
			if(p_section){
				p_ts_table->cat_section = p_section->next;
				free(p_section);
			}
			p_ts_table->cur_cat_section = NULL;
			break;
		case SDT_PID :
			p_section = p_ts_table->cur_sdt_section;
			if(p_section){
				p_ts_table->sdt_section = p_section->next;
				free(p_section);
			}
			p_ts_table->cur_sdt_section = NULL;
			break;
		case NIT_PID :
			p_section = p_ts_table->cur_nit_section;
			if(p_section){
				p_ts_table->nit_section = p_section->next;
				free(p_section);
			}
			p_ts_table->cur_nit_section = NULL;
			break;
		case EIT_PID :
			del_ts_table_eit_section(in_chan);
			break;
		default :
			p_pmt = find_ts_pmt_table(p_ts_table, pid);
			
			if(p_pmt && p_pmt->cur_program){

				p_section = p_pmt->cur_program->pmt_section;
				if(p_section){
					p_pmt->cur_program->pmt_section = p_section->next;
					free(p_section);
				}
				p_pmt->cur_program = NULL;
		
			}
			break;
	}

}


void add_ts_table_eit_section(int in_chan, section_head_t *p_section_head, section_t *p_section)
{
	u_char last_table_id;
	program_t *p_program;
	eit_table_t *p_eit;
	eit_sub_table_t *p_sub_table;
	eit_segment_t *p_segment;
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;

	if(p_section_head == NULL || p_section == NULL){
		return ;
	}

	#if EIT_CAPTURE
	printf("add_ts_table_eit_section   %d in_chan, table_id = %#x, section_number = %#x, section_len = %d\n"
		, in_chan, p_section_head->table_id, p_section_head->section_number, p_section->len);
	#endif


	last_table_id = p_section_head->p_buf[13];
	//p_eit = find_ts_table_eit(in_chan, p_section_head->sub_table_id);
	
	p_program = find_ts_in_program(p_ts_table->pmt_tables, p_section_head->sub_table_id);

	if(p_program==NULL){
		return;
	}

	p_eit = p_program->eit_tables;
	
	if(p_eit==NULL){
		p_eit = create_eit_table();
		p_eit->service_id = p_section_head->sub_table_id;
		p_eit->last_table_id = last_table_id;

		p_program->eit_tables = p_eit;
		
	}

	if(p_eit->last_table_id < last_table_id){
		p_eit->last_table_id = last_table_id;
	}

	p_sub_table = find_eit_sub_table(p_eit, p_section_head->table_id);
	if(p_sub_table == NULL){
		p_sub_table = create_eit_sub_table();

		p_sub_table->table_id = p_section_head->table_id;
		p_sub_table->last_section_number = p_section_head->last_section_number;
		
		p_sub_table->next = p_eit->sub_tables;
		p_eit->sub_tables = p_sub_table;
		
		p_eit->sub_table_count++;
	}

	p_segment = find_eit_segment(p_sub_table, p_section_head->section_number);
	if(p_segment == NULL){
		p_segment = create_eit_segment();
		p_segment->segment_num = p_section_head->section_number/8;
		p_segment->segment_last_section_number = p_section_head->p_buf[12];
		
		p_segment->next = p_sub_table->segments;
		p_sub_table->segments = p_segment;

		p_sub_table->segment_count++;
	}

	p_sub_table->cur_segment = p_segment;
	p_sub_table->now_section_number = p_section_head->section_number;
	
	if(p_sub_table->last_section_number == 0xffff){
		p_sub_table->last_section_number = p_section_head->last_section_number;
	}else if(p_sub_table->last_section_number != p_section_head->last_section_number){
		printf("the eit last one last section number [%#x] and the current last section number [%#x] isn't consistent ");
	}
	//add_section(&p_segment->section, p_section);
	add_eit_section(&p_segment->section, (eit_section_t *)p_section);

	p_ts_table->cur_eit_sub_table = p_sub_table;

}

void add_ts_table_pmt_section(int in_chan, section_head_t *p_section_head, section_t *p_section)
{
	ts_in_table_t *p_ts_table;
	pmt_table_t *p_pmt;
	program_t *p_program;

	if(p_section_head == NULL || p_section == NULL){
		return;
	}

	/*
	printf("add_ts_table_pmt_section : %d in_chan, program number : %d , section length = %d \n"
		, in_chan, p_section_head->sub_table_id, p_section_head->section_len);
	//*/
	
	p_ts_table = &ts_in_channels[in_chan].ts_tables;
	
	p_pmt = p_ts_table->pmt_tables;
	while(p_pmt){
		
		p_program = p_pmt->p_program;
		while(p_program){
			if(p_program->program_number == p_section_head->sub_table_id){
				goto End_Loop;
			}
			p_program = p_program->next;
		}
		
		p_pmt = p_pmt->next;
	}
	
	return;
	
	End_Loop:
	if(p_program){
		add_section(&p_program->pmt_section, p_section);
	}
	p_pmt->cur_program = p_program;
	
	return ;

}


section_t *add_ts_table_section(int in_chan, u_short pid, section_head_t *p_section_head, section_t *p_section)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	program_t *p_program;

	capture_DebugPrintf(" %d in_chan , add_ts_table_section, pid = 0x%04x \n"
		, in_chan, pid);

	if(p_section == NULL || p_section_head == NULL){
		return NULL;
	}
	
	p_section->len = p_section_head->section_len;
	p_section->version_number = p_section_head->version_number;
	p_section->section_number = p_section_head->section_number;
	
	switch(pid){
		case PAT_PID :
			p_ts_table->ts_id = p_section_head->sub_table_id;
			p_ts_table->pat_now_section_number = p_section_head->section_number;
			p_ts_table->pat_last_section_number = p_section_head->last_section_number;
			add_section(&p_ts_table->pat_section, p_section);
			p_ts_table->cur_pat_section = p_section;
			break;
		case CAT_PID :
			p_ts_table->cat_now_section_number = p_section_head->section_number;
			p_ts_table->cat_last_section_number = p_section_head->last_section_number;
			add_section(&p_ts_table->cat_section, p_section);
			p_ts_table->cur_cat_section = p_section;
			break;
		case SDT_PID :
			p_ts_table->original_network_id = get_bits(p_section_head->p_buf, 8, 0, 16);
			p_ts_table->sdt_now_section_number = p_section_head->section_number;
			p_ts_table->sdt_last_section_number = p_section_head->last_section_number;

			capture_DebugPrintf(" %d in_chan , add_sdt_section, section_num_mask = %#.8x, section_number = %d \n"
					, in_chan, p_ts_table->sdt_section_num_mask,p_section->section_number);

			add_section(&p_ts_table->sdt_section, p_section);
			p_ts_table->cur_sdt_section = p_section;
			break;
		case NIT_PID :
			p_ts_table->network_id = p_section_head->sub_table_id;
			p_ts_table->nit_now_section_number = p_section_head->section_number;
			p_ts_table->nit_last_section_number = p_section_head->last_section_number;
			add_section(&p_ts_table->nit_section, p_section);
			p_ts_table->cur_nit_section = p_section;
			break;
		case EIT_PID :
			add_ts_table_eit_section(in_chan, p_section_head, p_section);
			break;
		default :
			if(p_section_head->table_id == PMT_TABLEID){
				add_ts_table_pmt_section(in_chan, p_section_head, p_section);
			}
			break;
	}
	

	return p_section;
}

section_t *get_ts_table_cur_section(int in_chan, u_short pid)
{

	ts_in_table_t *p_ts_table;
	section_t *p_section = NULL;
	pmt_table_t *p_pmt;
	
	
	capture_DebugPrintf(" %d in_chan , get_ts_table_cur_section, pid = %#x \n", in_chan, pid);

	p_ts_table = &ts_in_channels[in_chan].ts_tables;

	switch(pid){
		case PAT_PID :
			p_section = p_ts_table->cur_pat_section;
			break;
		case CAT_PID :
			p_section = p_ts_table->cur_cat_section;
			break;
		case SDT_PID :
			p_section = p_ts_table->cur_sdt_section;
			break;
		case NIT_PID :
			p_section = p_ts_table->cur_nit_section;
			break;
		case EIT_PID :
			if(p_ts_table->cur_eit_sub_table  && p_ts_table->cur_eit_sub_table->cur_segment){
				p_section = (section_t *)p_ts_table->cur_eit_sub_table->cur_segment->section;
			#if EIT_CAPTURE
				printf(" cur_eit table_id = %#x, now_selction_number = %#x , section len = %d\n"
					, p_ts_table->cur_eit_sub_table->table_id, p_ts_table->cur_eit_sub_table->now_section_number
					, p_section->len);
				//print_bytes(packet_buf, PACKAGE_MAX_LEN);
			#endif
			}
			break;
		default :
			p_pmt = find_ts_pmt_table(p_ts_table, pid);
			if(p_pmt && p_pmt->cur_program){
				p_section = p_pmt->cur_program->pmt_section;
			}
			break;
	}

	return p_section;
}	



int copy_ts_table_section_data(section_t *p_section, u_char *p_data, u_char data_len)
{
	
	capture_DebugPrintf(" copy_ts_table_section_data,  data_len = %d\n", data_len);

	if((p_section == NULL) || (p_data == NULL) || (p_section->now_len == p_section->len)){
		return 0;
	}

	capture_DebugPrintf(" copy_ts_table_section_data, section total len = %d, now len = %d, data_len = %d\n"
		, p_section->len, p_section->now_len, data_len);

	capture_DebugPrintf("cur   section point addr : %p \n", p_section);
	capture_DebugPrintf("next  section point addr : %p \n", p_section->next);

	//print_bytes(p_section->data, p_section->now_len);

	if((p_section->len - p_section->now_len) > data_len){
		bcopy(p_data, (p_section->data + p_section->now_len), data_len);
		p_section->now_len += data_len;
		return 0;
	}else{
		bcopy(p_data, (p_section->data + p_section->now_len), p_section->len - p_section->now_len);
		p_section->now_len = p_section->len;
		return 1;
	}

	return 0;
}

int add_capture_eit_section(int in_chan)
{
	int i, capture_finished = 0;
	eit_table_t *p_eit;
	eit_sub_table_t *p_sub_table;
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;


	set_section_num_maskbit(in_chan, EIT_PID, p_ts_table->cur_eit_sub_table->now_section_number);
	
	p_ts_table->cur_eit_sub_table->cur_segment->section_count++;
	//p_ts_table->eit_cc = 0xff;
	
	//add_section(&p_ts_table->sdt_section, p_section);

	capture_finished = check_eit_seciton_isComplete(in_chan);
	
#if EIT_CAPTURE
	printf(" %d in_chan , add_capture_section, EIT_TABLE , eit_count = %d, now_section_number = %d, last_section_number = %d \n"
					, in_chan, p_ts_table->cur_eit_sub_table->segment_count, p_ts_table->cur_eit_sub_table->now_section_number, p_ts_table->cur_eit_sub_table->last_section_number);


	//print_bytes(p_ts_table->cur_eit_sub_table->section->data, p_ts_table->cur_eit_sub_table->section->len);
#endif

	p_ts_table->cur_eit_sub_table->cur_segment = NULL;
	p_ts_table->cur_eit_sub_table = NULL;


	return capture_finished;
}

int add_capture_pmt_section(int in_chan, u_short pmt_pid)
{
	int i, capture_finished = 0;
	pmt_table_t *p_pmt;
	program_t *p_program;
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;

	//printf("add_capture_pmt_section : %d in_chan , pmt_pid = %#x \n", in_chan, pmt_pid);
		
	p_pmt = find_ts_pmt_table(p_ts_table, pmt_pid);
	
	if(p_pmt == NULL || p_pmt->cur_program == NULL){
		return 0;
	}
	
	p_pmt->cur_program->isComplete = 1;
	p_pmt->cur_program = NULL;
	
	p_program = p_pmt->p_program;
	while(p_program){
		if(p_program->isComplete == 0){
			return 0;
		}
		p_program = p_program->next;
	}

	p_pmt->isComplete = 1;
	close_host_capture(in_chan, pmt_pid);

	return 1;

}


int add_capture_section(int in_chan, u_short pid)
{
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	int capture_finished = 0;
	
		
	switch(pid){
		case PAT_PID:
			set_section_num_maskbit(in_chan, pid, p_ts_table->pat_now_section_number);
			
			p_ts_table->pat_count++;
			//p_ts_table->pat_cc = 0xff;
			//add_section(&p_ts_table->pat_section, p_section);
			if((p_ts_table->pat_last_section_number == 0xff) || (p_ts_table->pat_count == p_ts_table->pat_last_section_number+1)){
				set_maskbit(&p_ts_table->complete_mask, PAT_USED);
				close_host_capture(in_chan, pid);
				capture_finished = 1;
			}
			p_ts_table->cur_pat_section = NULL;
		
			capture_DebugPrintf(" %d in_chan , add_capture_section, PAT_TABLE , pat_count = %d \n"
					, in_chan, p_ts_table->pat_count);
			break;
		case CAT_PID :
			set_section_num_maskbit(in_chan, pid, p_ts_table->cat_now_section_number);
			
			p_ts_table->cat_count++;
			//p_ts_table->cat_cc = 0xff;
			//add_section(&p_ts_table->cat_section, p_section);
			if(p_ts_table->cat_count == p_ts_table->cat_last_section_number+1){
				set_maskbit(&p_ts_table->complete_mask, CAT_USED);
				close_host_capture(in_chan, pid);
				capture_finished = 1;
			}
			p_ts_table->cur_cat_section = NULL;

			capture_DebugPrintf(" %d in_chan , add_capture_section, CAT_TABLE , cat_count = %d \n"
						, in_chan, p_ts_table->cat_count);

			break;
		case SDT_PID :
			set_section_num_maskbit(in_chan, pid, p_ts_table->sdt_now_section_number);
			
			p_ts_table->sdt_count++;
			//p_ts_table->sdt_cc = 0xff;

			
			//add_section(&p_ts_table->sdt_section, p_section);
			if((p_ts_table->sdt_last_section_number == 0xff) || (p_ts_table->sdt_count == p_ts_table->sdt_last_section_number+1)){
				set_maskbit(&p_ts_table->complete_mask, SDT_USED);
				close_host_capture(in_chan, pid);
				capture_finished = 1;
			}
			p_ts_table->cur_sdt_section = NULL;

			capture_DebugPrintf(" %d in_chan , add_capture_sdt section , section_number = %d, last_section_number = %d, count = %d \n"
					, in_chan, p_ts_table->sdt_now_section_number, p_ts_table->sdt_last_section_number, p_ts_table->sdt_count);

			break;
		case NIT_PID :
			set_section_num_maskbit(in_chan, pid, p_ts_table->nit_now_section_number);
			
			p_ts_table->nit_count++;
			//p_ts_table->nit_cc = 0xff;
			//add_section(&p_ts_table->nit_section, p_section);
			if(p_ts_table->nit_count == p_ts_table->nit_last_section_number+1){
				set_maskbit(&p_ts_table->complete_mask, NIT_USED);
				close_host_capture(in_chan, pid);
				capture_finished = 1;
			}
			p_ts_table->cur_nit_section = NULL;

			capture_DebugPrintf(" %d in_chan , add_capture_section, NIT_TABLE , nit_count = %d \n"
					, in_chan, p_ts_table->nit_count);

			break;
		case EIT_PID :
			capture_finished = add_capture_eit_section(in_chan);
			break;

			
		default :
			capture_finished = add_capture_pmt_section(in_chan, pid);			
			
			break;
	}

	return capture_finished;
}

int package_to_section(int in_chan, u_char *packet_buf, int wait_times)
{
	u_short pid_num;
	u_char payload_start_indicator;
	u_char packet_cc;
	u_char point_field;
	u_short offlen = 0;
	
	u_char section_finish = 0;
	u_char assemble_ok = 0;
	
	int result;

	ts_in_table_t *p_ts_tables;
	section_t *p_section;
	section_head_t section_head;

	p_ts_tables = &ts_in_channels[in_chan].ts_tables;

	payload_start_indicator = packet_buf[1] & 0x40;
	pid_num = (packet_buf[2] & 0xff) | ((packet_buf[1] & 0x1f) << 8);
	packet_cc = packet_buf[3] & 0xf;
	
	capture_InfoPrintf(" %d in_chan , package_to_section,  pid_num = 0x%04x, payload_start_indicator = %#.2x \n", in_chan, pid_num, payload_start_indicator);

	p_section = prejudge_ts_table_section(in_chan, pid_num, packet_cc);
	if(p_section == NULL && (payload_start_indicator == 0)){

		capture_DebugPrintf(" %d in_chan , package_to_section, the package isnot the section head and  section does not exist, pid[0x%04x], return\n", in_chan, pid_num);

		result = EXEC_ERR;
		goto End_Assemble_Section;
	}
	

	offlen = 4;
	
	if(packet_buf[3] & 0x20)	// jump out adaptation field
	{
		offlen += (packet_buf[4] & 0xff) + 1;
		
	}

	
	if(payload_start_indicator){	// first package

		point_field = (packet_buf[offlen++] & 0xff);
		
		capture_DebugPrintf(" %d in_chan , package_to_section, point_field = %#x \n", in_chan, point_field);

		if(p_section){
			section_finish = copy_ts_table_section_data(p_section, packet_buf + offlen, point_field);
			
			if(section_finish){
				if(calc_crc32(p_section->data, p_section->len) == 0){
					p_section->crc32 = get_bits(p_section->data + (p_section->len-4), 0, 0, 32);
					if(add_capture_section(in_chan, pid_num)){
						assemble_ok = 1;
						result = EXEC_OK;
						goto End_Assemble_Section;
					}
				}else{
					capture_DebugPrintf(" %d in_chan , package_to_section,  pid %d  table_id %d crc error \n", in_chan, pid_num, p_section->data[0]);
					del_ts_table_section(in_chan, pid_num);
				}
			}else{
				del_ts_table_section(in_chan, pid_num);
			}
			
			p_section = NULL;
		}
		
		offlen += point_field;

		section_head.p_buf = packet_buf + offlen;

		
		if(offlen >= PACKAGE_MAX_LEN - 4){
			capture_DebugPrintf(" %d in_chan , package_to_section, packet_len [%d] > PACKAGE_MAX_LEN , return\n", in_chan, offlen);
			result = EXEC_ERR;
			goto End_Assemble_Section;
		}

		Start_Assemble_Section:
		

		if((section_head.p_buf[5] & 0x1) == 0){//current table valid
			capture_DebugPrintf(" %d in_chan , package_to_section, current table invalid , return\n", in_chan);
			result = EXEC_NULL;
			goto End_Assemble_Section;

		}
		
		

		section_head.table_id = section_head.p_buf[0];
		section_head.sub_table_id = (section_head.p_buf[3]<<8) | section_head.p_buf[4];
		section_head.version_number = (section_head.p_buf[5]>>1)&0x1f;
		section_head.section_number = section_head.p_buf[6];
		
	//	catpute_DebugPrintf(" %d in_chan , package_to_section, table_id = %d\n", in_chan, section_head.table_id);
	//	catpute_DebugPrintf(" %d in_chan , package_to_section, section_number = %d\n", in_chan, section_head.section_number);
		

		if(isExist_ts_section(in_chan, pid_num, &section_head)){
			capture_DebugPrintf(" %d in_chan , package_to_section, current table exist , pid = 0x%04x, table_id = %#x, section_number = %d, return\n"
					, in_chan, pid_num, section_head.table_id, section_head.section_number);
			result = EXEC_NULL;
			goto End_Assemble_Section;
		}

		/*
		if(p_section){
			del_ts_table_section(in_chan, pid_num);
			p_section = NULL;
		}
		//*/
		
		section_head.section_len = ((section_head.p_buf[1] << 8) & 0xf00) + (section_head.p_buf[2] & 0xff) + 3;

		capture_DebugPrintf(" %d in_chan , package_to_section, section_len = %d\n", in_chan, section_head.section_len);
		if(section_head.section_len>MAX_SECTION_LEN){
			capture_DebugPrintf(" %d in_chan , package_to_section, section_len [%d] > MAX_SECTION_LEN, return\n", in_chan, section_head.section_len);
			result = EXEC_ERR;
			goto End_Assemble_Section;
		}
		
		section_head.last_section_number = section_head.p_buf[7] & 0xff;
		

		if(section_head.section_number > section_head.last_section_number){
			capture_DebugPrintf(" %d in_chan , package_to_section, PID=0x%x,tableid=0x%x,now_section_number >	max_section_number : [%d > %d] , return\n",
				pid_num, section_head.table_id,section_head.section_number,section_head.last_section_number);
			result = EXEC_ERR;
			goto End_Assemble_Section;
		}
		
		if(pid_num == EIT_PID){
			p_section = (section_t *)create_eit_section();
		}else{
			p_section = create_section();
		}
		
		add_ts_table_section(in_chan, pid_num, &section_head, p_section);
		
	}

	p_section = get_ts_table_cur_section(in_chan, pid_num);
	section_finish = copy_ts_table_section_data(p_section, packet_buf+offlen, PACKAGE_MAX_LEN - offlen);


	if(section_finish){
		if(calc_crc32(p_section->data, p_section->len) == 0){
			p_section->crc32 = get_bits(p_section->data + (p_section->len-4), 0, 0, 32);
			add_capture_section(in_chan, pid_num);
			assemble_ok = 1;
			result = EXEC_OK;
		}else{
			capture_InfoPrintf(" %d in_chan , package_to_section,  pid %d  table_id %d crc error \n", in_chan, pid_num, p_section->data[0]);
			del_ts_table_section(in_chan, pid_num);
			result = EXEC_ERR;
		}
	}

	End_Assemble_Section:

	if(assemble_ok){
		p_ts_tables->capture_error_count = 0;
	}else{
		p_ts_tables->capture_error_count++;
		//if(p_ts_tables->capture_error_count > CAPTURE_ERROR_LIMIT){
		if(p_ts_tables->capture_error_count > wait_times){
			printf("packet_to_section : %d in_chan can't get the requisite section, and timeout , end capture ", in_chan);
			if(p_section){
				del_ts_table_section(in_chan, pid_num);
			}
			result = EXEC_TIMEOUT;
		}
	}

	
	return result;

}

int check_capture_complete(int in_chan)
{
	ts_in_table_t *p_ts_tables;
	pmt_table_t *p_pmt;

	if((ts_in_channels[in_chan].ts_flag & TSIN_LOCK) == 0){
		return -1;
	}
	
	p_ts_tables = &ts_in_channels[in_chan].ts_tables;

		capture_DebugPrintf("check %d in_chan capture : capture_mask[ %#x ] , complete_mask[ %#x ]\n"
		, in_chan, p_ts_tables->capture_mask, p_ts_tables->complete_mask);

	if(p_ts_tables->capture_mask & PMT_MASK){
		p_pmt = p_ts_tables->pmt_tables;

		while(p_pmt){
			if(p_pmt->isComplete == 0){
				return 0;
			}
			p_pmt = p_pmt->next;
		}
		
		set_maskbit(&p_ts_tables->complete_mask, PMT_USED);
	}

	if(memcmp(&p_ts_tables->capture_mask, &p_ts_tables->complete_mask, 1) == 0){
		capture_InfoPrintf("%d in_chan, capture is complete\n", in_chan);
		//p_ts_tables->capture_mask = 0;
		//p_ts_tables->complete_mask = 0;
		return 1;
	}else{
		return 0;
	}
	
}

u_int check_capture_valid(int in_chan)
{
	
	u_int capture_valid, valid_mask;

	#ifdef MUX_REG_PAGE
	WRITE_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif
//	capture_valid = FPGA_MUX_READ(TSIN_CAPTURE_VALID_BASE_REG+((in_chan/CAPTURE_VALID_NUM_IN_ONE_REG)*(DDR_BIT_WIDTH/8)));
	#ifdef MUX_REG_PAGE
	UNLOCK_FPGA_RAM_PAGE(MUX_REG_PAGE);
	#endif

	valid_mask = (1<<(in_chan%CAPTURE_VALID_NUM_IN_ONE_REG));


	//catpute_InfoPrintf("capture_valid:0x%x, valid_mask:0x%x\n", capture_valid, valid_mask);

	return (capture_valid & valid_mask);
}

int read_fpga_capture_buffer(int in_chan, int times)
{

	int retval, wait_times;
	ts_in_channel_t *p_tsin_channel;
	u_char packet_buf[192], *p_buf;
	
	
	if(in_chan >= mux_info.total_tsin_channels){
		printf("read capture zone !, %d in_chan over than max channel\n", in_chan);
		return EXEC_ERR;
	}

	//printf("%d in_chan, read_fpga_capture_buffer, ----0----\n", in_chan);
	
	p_tsin_channel = &ts_in_channels[in_chan];

	wait_times = 5000;
	do{
		if(wait_times<=0){
			printf("%d in_chan haven't  capture packet \n", in_chan);	
			return EXEC_ERR;	
		}
		wait_times--;
		usleep(1000);
	}while((read_capture_packets_count(in_chan)<3));

	set_capture_flag(in_chan, CAPTURE_FLOW_FLAG, CAPTURE_READY);

	while(check_capture_flag(in_chan, CAPTURE_FLOW_FLAG, CAPTURE_READY)){

		if(check_maskbit(&p_tsin_channel->si_change_flag, PAT_USED) 
			&& !check_maskbit(&p_tsin_channel->ts_tables.capture_mask, PAT_USED)){
			printf("read_fpga_capture_buffer , %d in_chan ts already changed , escape the capture\n", in_chan);
			return EXEC_NULL;
		}

		wait_times = 1000;
		do{
			retval = check_capture_complete(in_chan);
			if(wait_times<=0 || retval){
				
				end_capture_packet(in_chan);

				if(retval<0){
					printf("%d in_chan is unlock , capture haven't complete ,end \n", in_chan);
					return EXEC_ERR;
				}
				if(wait_times<=0){
					printf("%d in_chan  capture ts, haven't  more packet ,end \n", in_chan);
				}
				
				return EXEC_OK;	
			}
			
			wait_times--;
			usleep(1000);
		}while((read_capture_packets_count(in_chan)<1));

		//monitor_capture_packets(in_chan);
		
		retval = read_fpga_packet_2(in_chan, packet_buf);
		
		if(retval == EXEC_ERR){
			printf("%d channel read_fpga_packet error, continue\n", in_chan);
			//disable_capture_in_channel(in_chan);
			//return EXEC_ERR;
			continue;
		}else if(retval == EXEC_NULL){
			printf("%d channel read_fpga_packet is empty\n", in_chan);
			continue;
		}

		#if 0
		p_buf = &packet_buf[0];
		
		if((p_buf[0] & 0xff) != SYNC_BYTE){
			capture_DebugPrintf("%d channel packet_buf[0] != SYNC_BYTE\n", in_chan);
			continue;
		}

		if(p_buf[1] & 0x80){
			capture_DebugPrintf("%d channel transport_error \n", in_chan);
			continue;
		}
				

		if(p_buf[3] & 0xc0){  
			capture_DebugPrintf("ts scrambling\n");
		}		 //scrambling 
		
		if(p_buf[3] & 0x10){// package has payload

			//printf("5.5 ... ...\n");

			if(p_buf[3] & 0x20)	// jump out adaptation field
			{
				if(((p_buf[4] & 0xff) + 5) > PACKAGE_MAX_LEN){
					capture_DebugPrintf("%d channel adaptation field bytes count overflow \n", in_chan);
					continue;
				}
			}
			
		}else{
			capture_DebugPrintf("%d channel package has not payload \n", in_chan);
			continue;
		}
	#endif
		if(package_to_section(in_chan, packet_buf, times) == EXEC_TIMEOUT){
			return EXEC_TIMEOUT;
		}


	}

	printf("capture ts : read %d ts in packet end\n", in_chan);
	//disable_capture_in_channel(in_chan);
	return EXEC_OK;

}


int isExist_eit_in_ts(int in_chan)
{
	
	ts_in_table_t *p_ts_table = &ts_in_channels[in_chan].ts_tables;
	pmt_table_t *p_pmt;
	program_t *p_program;

	p_pmt = p_ts_table->pmt_tables;
	
	while(p_pmt){
		
		p_program = p_pmt->p_program;
		
		while(p_program){
				
			if(p_program->EIT_pf_flag || p_program->EIT_s_flag){
				return 1;
			}
	
			p_program = p_program->next;
		}
		
		p_pmt = p_pmt->next;
	}
	
	return 0;
}


thread_pool_t *captureTs_thread_pool;


int capture_tsin_si(int in_chan, char capture_mask, u_short *pidlist, int pid_count, int wait_times)
{
	int i, result;
	
	ts_in_table_t *p_ts_table;

	struct timeval begintime, endtime;
	int sec_used, usec_used;

	p_ts_table = &ts_in_channels[in_chan].ts_tables;

	
	gettimeofday(&begintime, NULL);

	reset_capture_buffer(in_chan);

	//usleep(100000);
	
	p_ts_table->complete_mask = 0;
	p_ts_table->capture_mask = capture_mask;
	
	for(i=0; i<pid_count; i++){
		
		result = set_host_capture(in_chan, pidlist[i]);
		if(result != EXEC_OK){
			printf("%d Channel set_host_capture pid[ %#x ] Error\n", in_chan, pidlist[i]);
			break;
		}
	}

	if(result == EXEC_OK){
		enable_capture_in_channel(in_chan);

		/*
		result = getchar();
		if(result == 'n'){
			return EXEC_NULL;
		}
		//*/
		
		result = read_fpga_capture_buffer(in_chan, wait_times);
		if(result == EXEC_ERR){
			printf("%d Channel read_fpga_capture_buffer  Error\n", in_chan);
		}else if(result == EXEC_NULL){
			printf("%d Channel read_fpga_capture_buffer  Null\n", in_chan);
		}else if(result == EXEC_TIMEOUT){
			printf("%d Channel read_fpga_capture_buffer  Timeout\n", in_chan);
		}
	}
	
	gettimeofday(&endtime, NULL);
	sec_used = endtime.tv_sec - begintime.tv_sec;
	usec_used = endtime.tv_usec - begintime.tv_usec;
	
	if((mux_info.support_crc_monitor) || (pidlist[0] != PAT_PID)){
		printf("capture %d in_chan Ts, result :%d, used %8.6f second\n", in_chan, result, sec_used+(double)usec_used/1000000);
	}
	
	for(i=0; i<pid_count; i++){
		
		result = close_host_capture(in_chan, pidlist[i]);
	}

	return result;
}

int captureTS(int in_chan)
{
	int result;
	u_char si_mask;
	u_char table_id;
	
	ts_in_channel_t *p_ts_in_channel;
	ts_in_table_t *p_ts_table;
	pmt_table_t *p_pmt;
	program_t *p_program;
	int pid_count;
	u_short pid_list[100];

	if((capture_control.auto_capture == 0) && !check_capture_flag(in_chan, CAPTURE_FLOW_FLAG, CAPTURE_REDO)){
		return EXEC_OK;
	}

	if(in_chan >= mux_info.total_tsin_channels){
		return EXEC_ERR;
	}



#if INFO_CAPTURE	
	printf("begin capture %d channel ts !......\n", in_chan);
#endif


	p_ts_in_channel = &ts_in_channels[in_chan];

	
	if(mux_info.support_crc_monitor == 0){
		set_maskbit(&p_ts_in_channel->si_change_flag, PAT_USED);
		p_ts_in_channel->ts_flag &= ~TSIN_PSI_READY;
		p_ts_in_channel->ts_flag |= TSIN_PSI_CHANGE;
	}

	//clean_maskbit(&p_ts_in_channel->si_change_flag, PMT_USED);
	//clean_maskbit(&p_ts_in_channel->si_change_flag, CAT_USED);
	//clean_maskbit(&p_ts_in_channel->si_change_flag, NIT_USED);
	//clean_maskbit(&p_ts_in_channel->si_change_flag, EIT_USED);

	if(p_ts_in_channel->si_change_flag == 0){
		return EXEC_NULL;
	}


	lock_ts_in_channel(in_chan, WRITE_FLAG);
	
	
	set_capture_flag(in_chan, CAPTURE_FLOW_FLAG, CAPTURE_BEGIN);
	
	p_ts_table = &p_ts_in_channel->ts_tables;
	
	init_host_capture(in_chan);
	
	//p_ts_table->table_valid = INVALID;

	
	pid_count = 0;
	si_mask = 0;
	if(check_maskbit(&p_ts_in_channel->si_change_flag, PAT_USED)){
		
		if(mux_info.support_crc_monitor){
			printf("... %d Channel Capture PAT ... \n", in_chan);
		}
	
		si_mask = PAT_MASK;
		reset_ts_in_table(p_ts_table, si_mask);
	

		pid_count = 0;
		pid_list[pid_count++] = PAT_PID;

		
		result = capture_tsin_si(in_chan, si_mask, pid_list, pid_count, 50);
		if(result != EXEC_OK){
			goto TS_ERROR;	
		}

		if(p_ts_table->pat_count == 0){
			printf("%d Channel TS  capture pat empty\n", in_chan);
			goto TS_ERROR;
		}
		
		if(mux_info.support_crc_monitor == 0){
			if(p_ts_in_channel->pat_crc == p_ts_table->pat_section->crc32){
				p_ts_in_channel->ts_flag |= TSIN_PSI_READY;

				p_ts_table->table_valid = VALID;
				set_capture_flag(in_chan, CAPTURE_ALL_FLAG, CAPTURE_OK | CAPTURE_END);
				p_ts_in_channel->si_change_flag = 0;

				capture_DebugPrintf("%d channel in ts haven't changed yet \n", in_chan);

				unlock_ts_in_channel(in_chan, WRITE_FLAG);
				return EXEC_OK;
			}else{
				p_ts_in_channel->pat_crc = p_ts_table->pat_section->crc32;
				//*
				p_ts_in_channel->si_change_flag = ALL_SI_MASK;

				if(p_ts_in_channel->dowith_nit == 0){
					 p_ts_in_channel->si_change_flag &= (~NIT_MASK);
				}
				
				if(p_ts_in_channel->si_change_flag & SDT_MASK){
					p_ts_in_channel->ts_flag &= ~TSIN_SDT_READY;
					p_ts_in_channel->ts_flag |= TSIN_SDT_CHANGE;
					if((p_ts_in_channel->si_change_flag & PAT_MASK) == 0){
						free_all_service_eit_table(p_ts_table);
					}
				}
				if(p_ts_in_channel->si_change_flag & NIT_MASK){
					p_ts_in_channel->ts_flag &= ~TSIN_NIT_READY;
					p_ts_in_channel->ts_flag |= TSIN_NIT_CHANGE;
				}
				if(p_ts_in_channel->si_change_flag & EIT_MASK){
					p_ts_in_channel->ts_flag &= ~TSIN_EIT_READY;
					p_ts_in_channel->ts_flag |= TSIN_EIT_CHANGE;
				}
				//*/
			}
			
		}else{
			p_ts_in_channel->pat_crc = p_ts_table->pat_section->crc32;
		}


		

		//p_ts_in_channel->ts_flag &= ~TSIN_PSI_CHANGE;
		clean_maskbit(&p_ts_in_channel->si_change_flag, PAT_USED);

		//goto TS_CAPTURE_OK;
		
		
		p_ts_table->table_valid = INVALID;
		
		reset_ts_in_table(p_ts_table, PMT_MASK);
		
		result = parse_tsin_pat(p_ts_table);
		if(result == EXEC_ERR){
			printf("%d Channel parse pat Error\n", in_chan);
			goto TS_ERROR;	
		}

		printf("%d Channel program count = %d\n", in_chan, p_ts_table->program_count);

		//print_all_pmt_table(p_ts_table->pmt_tables);


		pid_count = 0;
		si_mask = 0;

		if(check_maskbit(&p_ts_in_channel->si_change_flag, PMT_USED)){
			//*
			p_pmt = p_ts_table->pmt_tables;
			if(p_pmt){si_mask |= PMT_MASK;}
			while(p_pmt){;
				pid_list[pid_count++] = p_pmt->pmt_pid;
				p_pmt = p_pmt->next;
			}
			
			printf("... %d Channel Capture PMT ... \n", in_chan);
			//*/

		}

		/*
		if(si_mask){
			result = capture_tsin_si(in_chan, si_mask, pid_list, pid_count, 200);
			if((result != EXEC_OK) && (result != EXEC_TIMEOUT)){
				goto TS_ERROR;	
			}
		}
		
		if(check_maskbit(&p_ts_table->complete_mask, PMT_USED)){
			parse_tsin_pmt(p_ts_table);
		}
		//*/

		if((p_ts_table->hasBissCA) || (p_ts_table->hasSimulCA)){
			//pid_count = 0;
			//si_mask = 0;
			if(check_maskbit(&p_ts_in_channel->si_change_flag, CAT_USED)){
				reset_ts_in_table(p_ts_table, CAT_MASK);
				pid_list[pid_count++] = CAT_PID;
				si_mask |= CAT_MASK;
				printf("... %d Channel Capture CAT ... \n", in_chan);
			}

			/*
			if(si_mask){
				result = capture_tsin_si(in_chan, si_mask, pid_list, pid_count, 100);
				if((result != EXEC_OK) && (result != EXEC_TIMEOUT)){
					//goto TS_ERROR;	
				}
			}

			if(check_maskbit(&p_ts_table->complete_mask, CAT_USED)){
				parse_tsin_cat(p_ts_table);
			}
			//*/

		}
	
	}

//	pid_count = 0;
//	si_mask = 0;	

	if(check_maskbit(&p_ts_in_channel->si_change_flag, SDT_USED)){
		printf("... %d Channel Capture SDT ... \n", in_chan);
		reset_ts_in_table(p_ts_table, SDT_MASK);
		pid_list[pid_count++] = SDT_PID;
		si_mask |= SDT_MASK;

	}
	if(si_mask){
		result = capture_tsin_si(in_chan, si_mask, pid_list, pid_count, 5000);
		if((result != EXEC_OK) && (result != EXEC_TIMEOUT)){
			//goto TS_ERROR;	
		}
	}
	
	//*
	if(check_maskbit(&p_ts_table->complete_mask, PMT_USED)){
		parse_tsin_pmt(p_ts_table);
	}
	
	if(check_maskbit(&p_ts_table->complete_mask, CAT_USED)){
		parse_tsin_cat(p_ts_table);
	}
	//*/
	
	if(check_maskbit(&p_ts_table->complete_mask, SDT_USED)){
		parse_tsin_sdt(p_ts_table);
		p_ts_in_channel->ts_flag |= TSIN_SDT_READY;
	}

	pid_count = 0;
	si_mask = 0;	
	
	if(check_maskbit(&p_ts_in_channel->si_change_flag, NIT_USED)){
		printf("... %d Channel Capture NIT ... \n", in_chan);
		reset_ts_in_table(p_ts_table, NIT_MASK);
		pid_list[pid_count++] = NIT_PID;
		si_mask |= NIT_MASK;
	}

	if(si_mask){
		result = capture_tsin_si(in_chan, si_mask, pid_list, pid_count, 5000);
		if((result != EXEC_OK) && (result != EXEC_TIMEOUT)){
			//goto TS_ERROR;	
		}
	}


	if(check_maskbit(&p_ts_table->complete_mask, NIT_USED)){
		//parse_tsin_nit(p_ts_table);
		p_ts_in_channel->ts_flag |= TSIN_NIT_READY;
	}


	p_ts_in_channel->ts_flag |= TSIN_PSI_READY;

	p_ts_table->table_valid = VALID;
	set_capture_flag(in_chan, CAPTURE_ALL_FLAG, CAPTURE_OK | CAPTURE_END);

	printf("%d Channel Capture TS ok ... ...!\n", in_chan);


	#if 1
	if((p_ts_in_channel->dowith_eit_pf || p_ts_in_channel->dowith_eit_s) && check_maskbit(&p_ts_in_channel->si_change_flag, EIT_USED)){

		if(isExist_eit_in_ts(in_chan)){
			printf("... %d Channel Capture EIT ... \n", in_chan);
						
			pid_count = 0;
			si_mask = EIT_MASK;
			reset_ts_in_table(p_ts_table, si_mask);
			pid_list[pid_count++] = EIT_PID;

			
			result = capture_tsin_si(in_chan, si_mask, pid_list, pid_count, tsin_capture_wait_times);
			//result = capture_tsin_si(in_chan, si_mask, pid_list, pid_count, 4000);
			if(result != EXEC_OK){
				//free_all_service_eit_table(p_ts_table);
				//goto TS_ERROR;
				printf("... %d Channel capture- EIT error... \n", in_chan);
			}else{
				printf("%d Channel Capture EIT ok ... !\n", in_chan);
			}
	
			p_ts_in_channel->ts_flag |= TSIN_EIT_READY;

			
		}else{
			printf("... %d Channel don't exist EIT ... \n", in_chan);
		}

		
	}

	#endif
	//printf("... %d Channel capture- EIT end... \n", in_chan);


	//print_ts_table(p_ts_table);

	p_ts_in_channel->si_change_flag = 0;


	unlock_ts_in_channel(in_chan, WRITE_FLAG);

	printf("%d Channel Capture  end ...ts_flag = %#x  \n", in_chan, p_ts_in_channel->ts_flag);
	printf("%d Channel Capture  end ... ..., return EXEC_OK\n", in_chan);



	
	//gettimeofday(&endtime, NULL);

	

	return EXEC_OK;
//----------------------------------------------------------------

TS_ERROR:
	printf("%d Channel Capture TS Error, return EXEC_ERR\n", in_chan);
	p_ts_table->table_valid = INVALID;
	reset_ts_in_table(&p_ts_in_channel->ts_tables, ALL_SI_MASK);
	set_capture_flag(in_chan, CAPTURE_ALL_FLAG, CAPTURE_ERROR | CAPTURE_END);
	p_ts_in_channel->si_change_flag = 0;
	unlock_ts_in_channel(in_chan, WRITE_FLAG);
	p_ts_in_channel->pat_crc = 0;
	
	return result;

}


int check_capture_task_is_ok(void)
{
	int in_chan;

	for(in_chan=0; in_chan<mux_info.total_tsin_channels; in_chan++){
		if(ts_in_channels[in_chan].ts_flag & TSIN_LOCK){
			if(!check_capture_flag(in_chan, CAPTURE_STATU_FLAG, CAPTURE_OK)){
				return 0;
			}
		}
	}

	return 1;
}


void add_captureTs_task(int in_chan)
{
	
	task_arg_t task_arg;
	
	task_arg.arg_size = 4;
	task_arg.mask_bit = in_chan;
	task_arg.arg = (void *)in_chan;
	
	thread_pool_add_task(captureTs_thread_pool, (proc_fun_t)captureTS, &task_arg);
}

void re_captureTs(int in_chan)
{
	ts_in_channels[in_chan].pat_crc = 0;
	set_capture_flag(in_chan, CAPTURE_FLOW_FLAG, CAPTURE_REDO);
	if(capture_control.auto_capture == 0){
		add_captureTs_task(in_chan);
	}
}

void *capture_task(void *args)
{

	int i, in_chan;
	int max_tread_num;
	task_arg_t task_arg;
	int now_time;
	
	struct timeval begintime, endtime;
	int sec_used, usec_used;

	printf("start capture_task ...\n");


	//sleep(10);
	
	printf("..... capture_task is running...\n");


	//enable_capture_irq();

#if 0

	max_tread_num = mux_info.total_tsin_channels;
	if(max_tread_num>16){
		max_tread_num = 16;
	}

	captureTs_thread_pool = create_thread_pool(max_tread_num, SZ_1M);


	for(in_chan=0; in_chan<mux_info.total_tsin_channels; in_chan++){
				
		for(i=0; i<5; i++){
			read_tsin_bitrate(in_chan);
			if(ts_in_channels[in_chan].ts_flag & TSIN_LOCK){
				
				task_arg.mask_bit = in_chan;
				task_arg.arg = (void *)in_chan;
				thread_pool_add_task(captureTs_thread_pool, (proc_fun_t)captureTS, &task_arg);
				break;
				
			}
			
		}

	}
	
	//thread_pool_destroy(captureTs_thread_pool);
	thread_pool_wait(captureTs_thread_pool);


#endif


	max_tread_num = 8;
	captureTs_thread_pool = create_thread_pool(max_tread_num, SZ_1M);


	task_arg.arg_size = 4;

	while(1){

		for(in_chan=0; in_chan<mux_info.total_tsin_channels; in_chan++){
					
			read_tsin_bitrate(in_chan);
			read_tsin_bitrate(in_chan);

			if(ts_in_channels[in_chan].ts_flag & TSIN_LOCK){
				task_arg.mask_bit = in_chan;
				task_arg.arg = (void *)in_chan;
				time(&now_time);
				if((now_time - ts_in_channels[in_chan].lock_time)>=1){
					//captureTS(in_chan);
					thread_pool_add_task(captureTs_thread_pool, (proc_fun_t)captureTS, &task_arg);
				}
			}else{
				set_capture_flag(in_chan, CAPTURE_FLOW_FLAG, CAPTURE_PEND);
				if(capture_control.unlock_reset)
					ts_in_channels[in_chan].pat_crc = -1;
			}

			usleep(10000);
			
		}

		sched_yield();
		sleep(2);
	}


	thread_pool_destroy(captureTs_thread_pool);


}



#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../basedef.h"
#include "../fpga.h"
#include "../fpga_reg.h"
#include "transceiver.h"

#include "../utils/utils.h"
#include "../utils/queue.h"

#include "../sqlite3/sys_sql.h"

#include "../tsoip/tsoip_backup.h"

/*
0:	可以理解为关闭所有transceiver输出
	初始化设置，将后面映射的ena全部设为0，
	为了避免误设置，该处需要设置0xaaaa 才能生效
1：原始需要修改的channel
2：设置映射的channel,bit[15]为使能,(只读上次设置)
3：设置transceiver所处的slot([3:0]rx_solot ? [7:4]tx_solot)
4：映射后的channel，即2中设置的channel值，不包括bit[15]使能设置
5：读取映射后channel的码率，包括channel位，byte每秒的数量
6：读取该组transceiver ?rx的码率，包括channel位，byte每秒的数量
7：读取该组transceiver ?tx的码率，包括channel位，byte每秒的数量

*/

#define REG_INIT_ALL_SETTING		0
#define REG_ORIGINAL_CHANNEL		1
#define REG_SET_CHANNEL		2
#define REG_SET_SLOT		3	// [3:0]rx ? [7:4]tx
#define REG_MAPPING_CHANNEL		4
#define REG_MAPPING_BYTE_COUNT		5
#define REG_ALL_RX_BYTE_COUNT		6
#define REG_ALL_TX_BYTE_COUNT		7

#define REG_MAPPING_VALID_BYTE_COUNT		0x15


u_short *transceiver_route_reg_map;

trans_chan_st *transceiver_channels;
trans_route_st *transceiver_routes;

transceiver_st *transceiver;

transceiver_cfg_st transceiver_cfg[MAX_TRANSCEIVER_QTY];

int transceiver_slot[MAX_TRANSCEIVER_QTY] = {
	0, 1, 2, 3, 4, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7
};

typedef struct {
	char flag;
	char src_slot;
	short route_id;
	int trans_chan;
}routeset_st;

#if 0
#define transceiver_channel_mutex_init(p_trans_chan)	pthread_mutex_init(&((p_trans_chan)->lock), NULL)
#define transceiver_channel_lock(p_trans_chan)		pthread_mutex_lock(&((p_trans_chan)->lock))
#define transceiver_channel_unlock(p_trans_chan)		pthread_mutex_unlock(&((p_trans_chan)->lock))
#else
#define transceiver_channel_mutex_init(p_trans_chan)	(p_trans_chan)->lock = 0
#define transceiver_channel_lock(p_trans_chan)	\
	do{	usleep(100); }while((p_trans_chan)->lock);	\
	(p_trans_chan)->lock = 1
	
#define transceiver_channel_unlock(p_trans_chan)		(p_trans_chan)->lock = 0
#endif

#define transceiver_route_mutex_init(p_trans_route)	transceiver_channel_mutex_init((trans_chan_st *)p_trans_route)
#define transceiver_route_lock(p_trans_route)		transceiver_channel_lock((trans_chan_st *)p_trans_route)
#define transceiver_route_unlock(p_trans_route)		transceiver_channel_unlock((trans_chan_st *)p_trans_route)


pthread_mutex_t transceiver_mux_map_lock;
pthread_mutex_t transceiver_ipout_map_lock;
pthread_mutex_t transceiver_mux_channel_lock;
pthread_mutex_t transceiver_ipout_channel_lock;
pthread_mutex_t transceiver_channel_rate_lock;


#define altrd_transceiver(base, reg)	FPGA_Readl((base)+((reg)<<2))
#define altwr_transceiver(base, reg, value)	FPGA_Writel((base)+((reg)<<2), value)



#define transceiver_channel_mapping(base, original_channel, target_channel)	\
				do{	\
					altwr_transceiver(base, REG_ORIGINAL_CHANNEL, original_channel);	\
					altwr_transceiver(base, REG_SET_CHANNEL, target_channel);	\
				}while(0)



int init_transceiver_all_setting(void)
{
	altwr_transceiver(REG_FPGA_TRANSCEIVER_MUX, REG_INIT_ALL_SETTING, 0xaaaa);
	altwr_transceiver(REG_FPGA_TRANSCEIVER_IPOUT, REG_INIT_ALL_SETTING, 0xaaaa);

	pthread_mutex_init(&(transceiver_mux_map_lock), NULL);
	pthread_mutex_init(&(transceiver_ipout_map_lock), NULL);
	pthread_mutex_init(&(transceiver_mux_channel_lock), NULL);
	pthread_mutex_init(&(transceiver_ipout_channel_lock), NULL);
	pthread_mutex_init(&(transceiver_channel_rate_lock), NULL);
}

u_int read_transceiver_mapping_channel_valid_byterate(u_int base, int channel)
{
	altwr_transceiver(base, REG_MAPPING_CHANNEL, channel);
	return altrd_transceiver(base, REG_MAPPING_VALID_BYTE_COUNT);
}


u_int read_transceiver_mapping_channel_byterate(u_int base, int channel)
{
	altwr_transceiver(base, REG_MAPPING_CHANNEL, channel);
	return altrd_transceiver(base, REG_MAPPING_BYTE_COUNT);
}

u_int read_transceiver_mux_channel_byterate(int channel)
{
	u_int byterate;

	pthread_mutex_lock(&transceiver_mux_channel_lock);
	byterate = read_transceiver_mapping_channel_byterate(REG_FPGA_TRANSCEIVER_MUX, channel);
	pthread_mutex_unlock(&transceiver_mux_channel_lock);

	return byterate;
}

u_int read_transceiver_ipout_channel_byterate(int channel)
{
	u_int byterate;

	pthread_mutex_lock(&transceiver_ipout_channel_lock);
	//byterate = read_transceiver_mapping_channel_byterate(REG_FPGA_TRANSCEIVER_IPOUT, channel);
	pthread_mutex_unlock(&transceiver_ipout_channel_lock);
	
	return byterate;
}

u_int read_transceiver_channel_valid_byterate(int channel)
{
	u_int byterate;
	
	pthread_mutex_lock(&transceiver_channel_rate_lock);
	byterate = read_transceiver_mapping_channel_valid_byterate(0x2e0000, channel);
	pthread_mutex_unlock(&transceiver_channel_rate_lock);

	
	return byterate;
}

u_int read_transceiver_channel_byterate(int channel)
{
	u_int byterate;
	
	pthread_mutex_lock(&transceiver_channel_rate_lock);
	byterate = read_transceiver_mapping_channel_byterate(0x2e0000, channel);
	pthread_mutex_unlock(&transceiver_channel_rate_lock);

	
	return byterate;
}

void transceiver_mux_channel_mapping(u_int original_channel, u_int target_channel)
{

	printf("transceiver mux channel mapping : %#x to %#x \n", original_channel, target_channel);
	pthread_mutex_lock(&transceiver_mux_map_lock);
	transceiver_channel_mapping(REG_FPGA_TRANSCEIVER_MUX, original_channel, target_channel);
	pthread_mutex_unlock(&transceiver_mux_map_lock);
}

void transceiver_ipout_channel_mapping(u_int original_channel, u_int target_channel)
{
	pthread_mutex_lock(&transceiver_ipout_map_lock);
	transceiver_channel_mapping(REG_FPGA_TRANSCEIVER_IPOUT, original_channel, target_channel);
	pthread_mutex_unlock(&transceiver_ipout_map_lock);
}


int set_transceiver_route_regs(int trans_chan, int dest_trans_if)
{
	u_int addr;
	u_int value;
	int i;

	//if(dest_trans_if)
	addr = REG_FPGA_TRANSCEIVER_ROUTE_TABLE + ((trans_chan)<<2);
	//addr = REG_FPGA_TRANSCEIVER_ROUTE_TABLE + ((src_trans_if*0x400+trans_chan)<<2);

	DebugPrintf("dest_trans_if : %d , trans_chan : %#x, addr = %#x \n"
		, dest_trans_if, trans_chan, addr);
	//*
	value = transceiver_route_reg_map[trans_chan];
	value |= 1<<dest_trans_if;
	//value |= 1<<trans_idx[dest_trans_if];
	transceiver_route_reg_map[trans_chan] = value;

	//usleep(1000);
	DebugPrintf("value : %#x  \n", value);

	FPGA_Writel(addr, value);
	//altwr_transceiver(REG_FPGA_TRANSCEIVER_ROUTE_TABLE, trans_chan, value);
	//*/
}

void clean_transceiver_route_regs(int trans_chan, int dest_trans_if)
{
	u_int addr;
	u_int value;
	int i;
	
	addr = REG_FPGA_TRANSCEIVER_ROUTE_TABLE + ((trans_chan)<<2);
	
	//addr = REG_FPGA_TRANSCEIVER_ROUTE_TABLE + ((src_trans_if*0x400+trans_chan)<<2);
	
	DebugPrintf("dest_trans_if : %d , trans_chan : %#x, addr = %#x \n"
		, dest_trans_if, trans_chan, addr);
	//*
	value = transceiver_route_reg_map[trans_chan];
	value &= ~(1<<dest_trans_if);
	//value &= ~(1<<trans_idx[dest_trans_if]);
	transceiver_route_reg_map[trans_chan] = value;

	//usleep(1000);
	DebugPrintf("value : %#x  \n", value);
	FPGA_Writel(addr, value);
	//altwr_transceiver(REG_FPGA_TRANSCEIVER_ROUTE_TABLE, trans_chan, value);
	//*/
}

void set_ipout_transceiver_source(int ipout_chan, int trans_chan)
{
	int addr;
	int value;

	
	DebugPrintf("ipout_chan : %d , trans_chan : %#x\n", ipout_chan, trans_chan);
	
	transceiver_ipout_channel_mapping(ipout_chan,   0x8000 | trans_chan);

}
void clean_ipout_transceiver_source(int ipout_chan)
{
	int addr;
	int value;

	
	DebugPrintf("ipout_chan : %d , trans_chan : %#x\n", ipout_chan);

	transceiver_ipout_channel_mapping(ipout_chan, 0);

}

void set_asiout_transceiver_source(int trans_chan)
{
	int addr;
	int value;

	
	DebugPrintf("asiout  trans_chan : %#x\n", trans_chan);
	
	FPGA_Writel(REG_FPGA_TRANSCEIVER_ASIOUT, trans_chan);

}



//Transceiver reconfig功能模块用于手动配置调整transceiver传输性能，优化硬件驱动性能
void transceiver_reconfig(int trans_if)
{

	u_int base_addr = REG_FPGA_TRANSCEIVER_RECONFIG;
	//transceiver_st *p_transceiv = &transceiver[trans_if];

	DebugPrintf("trans_if : %#x\n", trans_if);
	
	//transceiver_lock(p_transceiv);
	
	altwr_transceiver(base_addr,(0x8), trans_if*2);//set logical channel number register
	//VOD
	altwr_transceiver(base_addr,(0xc), 0x1e);//set data register 30=0x1e
	altwr_transceiver(base_addr,(0xb), 0);	//set pma offset register
	altwr_transceiver(base_addr,(0xa), 1);	//set control register in order to write data 

	//Pre-emphasis first post-tap
	altwr_transceiver(base_addr,(0xc), 3);
	altwr_transceiver(base_addr,(0xb), 2);
	altwr_transceiver(base_addr,(0xa), 1);

	//RX equalization DC gain
	altwr_transceiver(base_addr,(0xc), 1);
	altwr_transceiver(base_addr,(0xb), 0x10);
	altwr_transceiver(base_addr,(0xa), 1);

	//RX equalization control
	altwr_transceiver(base_addr,(0xc), 2);
	altwr_transceiver(base_addr,(0xb), 0x11);
	altwr_transceiver(base_addr,(0xa), 1);

//	transceiver_unlock(p_transceiv);
	
}


int enable_transceiver(int slot)
{
	int trans_if;

	trans_if = slot;

	transceiver[trans_if].run_flag = RUNNING_FLAG;
	printf(" enable %d transceiver  ... \n", trans_if);

}
int disable_transceiver(int slot)
{
	
	int trans_if;

	trans_if = slot;
	
	transceiver[trans_if].run_flag = PAUSE_FLAG;
	printf(" disable %d transceiver  ... \n", trans_if);

}

#ifdef SUBROUTINE
void free_transceiver(int slot)
{
	int i;


	if(transceiver[slot].run_flag != STOP_FLAG){
		transceiver[slot].run_flag = STOP_FLAG;

		destroy_arraylist(transceiver_cfg[slot].channel_list);
		transceiver_cfg[slot].channel_list = NULL;
			
		for(i=0; i<sysinfo->board_qty; i++){
			list_empty(&transceiver_cfg[slot].sndchan_list[i]);
		}

		delete_transceiver_routetable(slot);
		
		printf(" free %d transceiver  ... \n", slot);
		fflush(stdout);
	}

}
#endif

void print_transceiver_chan(trans_chan_st *chan_param)
{
	
	if(chan_param == NULL){
		return;
	}
	//*
	printf("src channel param : trans_if : %#x \n", chan_param->trans_if);
	printf("src channel param : trans_chan : %#x \n", chan_param->trans_chan);
	printf("src channel param : trans_mode : %#x \n", chan_param->trans_mode);
	printf("src channel param : functype : %#x \n", chan_param->func_type);
	printf("src channel param : func_chan : %d \n", chan_param->func_chan);
	printf("src channel param : ref_amount : %d \n", chan_param->ref_amount);
	printf("src channel param : enable : %d \n", chan_param->enable);
	//*/
}

void print_transceiver_route(trans_route_st *trans_route)
{
	
	if(trans_route == NULL){
		return;
	}
	
	printf("dest channel  : route_id : %d \n", trans_route->route_id);
	printf("dest channel param : trans_if : %#x \n", trans_route->trans_if);
	printf("dest channel param : trans_chan : %#x \n", trans_route->trans_chan);
	printf("dest channel param : trans_mode : %#x \n", trans_route->trans_mode);
	printf("dest channel param : functype : %#x \n", trans_route->func_type);
	printf("dest channel param : func_chan : %d \n", trans_route->func_chan);
	printf("dest channel param : enable : %d \n", trans_route->enable);
}

void print_transceiver_info(int slot)
{

	int idx = 0;
	int trans_if, trans_chan;;
	int trans_s, trans_e;
	trans_chan_st *chan_param;
	trans_route_st *trans_route;

	printf("%d transceiver run flag = %d \n", slot, transceiver[slot].run_flag);


	printf("%d transceiver : %d source channel \n", slot , transceiver[slot].channel_qty);

	trans_s = transceiver[slot].transif_s;
	trans_e = transceiver[slot].transif_qty+trans_s-1;
	for(trans_if=trans_s; trans_if<=trans_e; trans_if++){
		for(idx=0; idx<256; idx++){
			//chan_param = (trans_chan_st *)sArrList_get(transceiver[trans_if].channel_list, trans_chan);
			trans_chan = (trans_if<<8) | idx;
			chan_param = get_transceiver_channel(trans_chan);
			if((chan_param->board_slot==0xff) || (chan_param->trans_mode==0)){
				break;
			}
			print_transceiver_chan(chan_param);
		}
	}


	printf("%d transceiver : %d destination channel \n", slot , transceiver[slot].route_qty);
	
	for(idx=0; idx<transceiver[slot].route_qty; idx++){
		trans_route = get_transceiver_route(slot, idx);
		print_transceiver_route(trans_route);
	}

	
}


trans_chan_st *find_transceiver_channel(int slot, int idx)
{
	trans_chan_st *src_chan_param;
	/*
	transceiver_st *p_transceiver;
	int avg_qty, idx;

	p_transceiver = &transceiver[(trans_chan >> 8)];

	avg_qty = p_transceiver->channel_list->count / p_transceiver->transif_qty;
	idx = (trans_chan >> 8) - p_transceiver->transif_s;
	
	src_chan_param = sArrList_get(p_transceiver->channel_list, (idx *avg_qty) + (trans_chan&0xff));
	//*/
	DebugPrintf(" %#x \n", slot);
	if(slot > sysinfo->board_qty){
		return NULL;
	}

	src_chan_param = arraylist_get(transceiver_cfg[slot].channel_list,  idx);

	return src_chan_param;
}

trans_chan_st *get_transceiver_channel(u_short trans_chan)
{
	trans_chan_st *src_chan_param;
	/*
	transceiver_st *p_transceiver;
	int avg_qty, idx;

	p_transceiver = &transceiver[(trans_chan >> 8)];

	avg_qty = p_transceiver->channel_list->count / p_transceiver->transif_qty;
	idx = (trans_chan >> 8) - p_transceiver->transif_s;
	
	src_chan_param = sArrList_get(p_transceiver->channel_list, (idx *avg_qty) + (trans_chan&0xff));
	//*/
	DebugPrintf(" %#x \n", trans_chan);
	if(trans_chan == 0xffff){
		return NULL;
	}

	src_chan_param = &transceiver_channels[trans_chan];

	return src_chan_param;
}

trans_route_st *get_transceiver_route(int slot, int route_id)
{
	transceiver_st *p_transceiver = &transceiver[slot];
	trans_route_st *trans_route;

	if(route_id >= p_transceiver->route_qty){
		return NULL;
	}

	route_id = p_transceiver->transif_s*256 + route_id;
	trans_route = &transceiver_routes[route_id];

	return trans_route;
}

void init_transceiver(int slot)
{

	int i, j, k, idx;
	
	int max_chan = 0, max_route = 0;
	int trans_chan = 0, route_id = 0;
	int chan_count = 0, route_count = 0;
	int trans_if, trans_if_s, trans_if_e;
	
	board_st *board;
	funcmod_st *func;
	funcchan_st *chan;

	sArrList_st *deviflist;
	sArrList_st *funclist;
	sArrList_st *chanlist;
	
	transceiver_st *p_transceiver;
	transceiver_cfg_st *p_transceiver_cfg;
	trans_chan_st *chan_param;
	trans_route_st *trans_route;


	board = get_boardinfo(slot);

	if(board == NULL){
		return;
	}

	deviflist = board_deviflist(board);
	funclist = board_funclist(board);
		
	for(i=0; i<funclist->count; i++){
		
		func = (funcmod_st *)sArrList_get(funclist, i);
		chanlist = func_chanlist(func);
		
		for(j=0; j<chanlist->count; j++){
			
			chan = sArrList_get(chanlist, j);

			if(chan->trans_mode & TRANS_SRC){
				max_chan += chan->end_index - chan->start_index + 1;
			}
			
			if(chan->trans_mode & TRANS_DEST){
				max_route += chan->end_index - chan->start_index + 1;
			}

		}
	}

	
	printf("init_transceiver slot %d : max_route = %d , max_chan = %d \n", slot, max_route, max_chan);
	fflush(stdout);

	

	trans_if = slot;
	
	p_transceiver = &transceiver[slot];
	p_transceiver_cfg = &transceiver_cfg[slot];

	p_transceiver->channel_qty = max_chan;
	p_transceiver->route_qty = max_route;

	
	//p_transceiver_cfg->channel_list = create_arraylist(max_chan, DATA_FLAG(ASSIGN_DATA, POINTER_DATA, sizeof(trans_chan_st *)));

	//*
	if(slot==6){
		max_chan /= p_transceiver->transif_qty;
		max_route /= p_transceiver->transif_qty;
	}
	//*/
	

	for(i=0; i<funclist->count; i++){
		
		func = (funcmod_st *)sArrList_get(funclist, i);
		chanlist = func_chanlist(func);
		
		for(j=0; j<chanlist->count; j++){

			chan = (funcchan_st *)sArrList_get(chanlist, j);
			if((chan->trans_mode & TRANS_SRC)){
				for(k=chan->start_index; k<=chan->end_index;k++){

					//p_transceiver = &transceiver[slot];
					
					
					if(slot<6){
						trans_if = slot;
						trans_chan = chan_count;
					}else if(slot==6){
						trans_if = p_transceiver->transif_s + chan_count / max_chan;
						trans_chan = chan_count % max_chan;
					}else if(slot==7){
						switch(func->type){
							case FUNC_TYPE_TS_IP_IN:
								trans_if = 9 + chan->ifno;
								trans_chan = k-chan->start_index;
								//ipin_trans_srcidx[k] = chan_count;
								break;
							case FUNC_TYPE_REMUX:
								trans_if = 0xb;
								trans_chan = k;
								break;
							case FUNC_TYPE_ASI_IN:
								trans_if = 0xc;
								trans_chan = k;
								break;
						}
						

					}

					trans_chan |= (trans_if<<8);
	
					chan_param = get_transceiver_channel(trans_chan);
					
					chan_param->enable = DISABLE;
					chan_param->trans_mode = TRANS_SRC;
					chan_param->trans_chan =  trans_chan;

					
					chan_param->trans_if = trans_if;
					chan_param->board_slot = slot;


					chan_param->bitrate = 0;
					
					chan_param->func_type = func->type;
					chan_param->func_chan = k;
					chan_param->ref_amount = 0;
					memset(chan_param->ref_count, 0, sizeof(chan_param->ref_count));

					
					//arraylist_set(p_transceiver_cfg->channel_list, chan_count, chan_param);
					
					transceiver_channel_mutex_init(chan_param);
					chan_count++;

					//print_transceiver_chan(chan_param);
				}
			}
			
			if((chan->trans_mode & TRANS_DEST)){
				for(k=chan->start_index; k<=chan->end_index;k++){
					
					route_id = route_count;
					//p_transceiver = &transceiver[slot];
					
 
					if(slot<6){
						trans_if = slot;
						//route_id = route_count;
					}else if(slot==6){
						trans_if = p_transceiver->transif_s + route_count / max_route;
						//route_id = route_count % max_route;
					}else if(slot==7){
						switch(func->type){
							case FUNC_TYPE_TS_IP_OUT:
								trans_if = 0x9;
								//ipout_routeid[k] = route_id;
								break;
							case FUNC_TYPE_TS_IN:
								trans_if = 0xb;
								if(k==0){
								//	base_tsin_routeid = route_id;
								}
								break;
								
						}
						
					}
					
					trans_route = get_transceiver_route(slot, route_id);
					
					trans_route->route_id = route_id;
					trans_route->enable = DISABLE;
					trans_route->trans_mode = TRANS_DEST;
					trans_route->trans_chan = 0xffff;
					trans_route->trans_if = trans_if;
					trans_route->board_slot = slot;
					trans_route->func_type = func->type;
					trans_route->func_chan = k;
					trans_route->ref_amount = 0;
					trans_route->trans_chan_0 = 0xffff;
					trans_route->trans_chan_1 = 0xffff;
					
					transceiver_route_mutex_init(trans_route);
					
					route_count++;

					//print_transceiver_route(trans_route);
					
				}
			}
		
		}
	}

	
}

int base_tsin_routeid = 0;
u_short *tsin_mapping_chan;
u_short *tsin_trans_chan;


#ifdef SUBROUTINE
int allocate_transceiver(int slot)
{

	int i, j, k, idx;
	
	int max_chan = 0, max_route = 0;
	int trans_chan = 0, route_id = 0;
	int chan_count = 0, route_count = 0;
	int trans_if, trans_if_s, trans_if_e;
	
	board_st *board;
	funcmod_st *func;
	funcchan_st *chan;

	sArrList_st *deviflist;
	sArrList_st *funclist;
	sArrList_st *chanlist;
	
	transceiver_st *p_transceiver;
	transceiver_cfg_st *p_transceiver_cfg;
	trans_chan_st *chan_param;
	trans_route_st *trans_route;


	board = get_boardinfo(slot);

	if(board == NULL){
		return;
	}
	
	printf("init_transceiver slot %d : max_route = %d , max_chan = %d \n", slot, max_route, max_chan);
	fflush(stdout);

	trans_if = slot;
	
	p_transceiver = &transceiver[slot];
	p_transceiver_cfg = &transceiver_cfg[slot];

	max_chan = p_transceiver->channel_qty;
	max_route = p_transceiver->route_qty;

	p_transceiver_cfg->channel_list = create_arraylist(max_chan, DATA_FLAG(ASSIGN_DATA, POINTER_DATA, sizeof(trans_chan_st *)));

	//*
	if(slot==6){
		max_chan /= p_transceiver->transif_qty;
		max_route /= p_transceiver->transif_qty;
	}
	//*/

	deviflist = board_deviflist(board);
	funclist = board_funclist(board);

	for(i=0; i<funclist->count; i++){
		
		func = (funcmod_st *)sArrList_get(funclist, i);
		chanlist = func_chanlist(func);
		
		for(j=0; j<chanlist->count; j++){

			chan = (funcchan_st *)sArrList_get(chanlist, j);
			if((chan->trans_mode & TRANS_SRC)){
				for(k=chan->start_index; k<=chan->end_index;k++){
					
					if(slot<6){
						trans_if = slot;
						trans_chan = chan_count;
					}else if(slot==6){
						trans_if = p_transceiver->transif_s + chan_count / max_chan;
						trans_chan = chan_count % max_chan;
					}else if(slot==7){
						switch(func->type){
							case FUNC_TYPE_TS_IP_IN:
								trans_if = 9 + chan->ifno;
								trans_chan = k-chan->start_index;
								ipin_trans_srcidx[k] = chan_count;
								break;
							case FUNC_TYPE_REMUX:
								trans_if = 0xb;
								trans_chan = k;
								break;
							case FUNC_TYPE_ASI_IN:
								trans_if = 0xc;
								trans_chan = k;
								break;
						}
						

					}

					trans_chan |= (trans_if<<8);
	
					chan_param = get_transceiver_channel(trans_chan);
					arraylist_add(p_transceiver_cfg->channel_list, chan_param);
					
					
					chan_count++;

					//print_transceiver_chan(chan_param);
				}
			}
			
			if((chan->trans_mode & TRANS_DEST)){
				for(k=chan->start_index; k<=chan->end_index;k++){
					
					route_id = route_count;
					//p_transceiver = &transceiver[slot];
					
 
					if(slot<6){
						trans_if = slot;
						//route_id = route_count;
					}else if(slot==6){
						trans_if = p_transceiver->transif_s + route_count / max_route;
						//route_id = route_count % max_route;
					}else if(slot==7){
						switch(func->type){
							case FUNC_TYPE_TS_IP_OUT:
								trans_if = 0x9;
								ipout_routeid[k] = route_id;
								break;
							case FUNC_TYPE_TS_IN:
								trans_if = 0xb;
								
								if(k==0){
									base_tsin_routeid = route_id;
								}
								break;
								
						}
						
					}
	
					route_count++;
					
				}
			}
		
		}
	}

	
}


struct transceiver_channel_tx_param{
	char enable;
	char trans_if;
	char trans_mode;
	char func_type;
	short func_chan;
	short trans_chan;
};
typedef struct transceiver_channel_tx_param channel_tx_param_st;


static int add_transceiver_channel_param(int slot, trans_chan_st *chan_param)
{

	channel_tx_param_st chan_tx_param;
	
	transceiver_st *p_transceiver;
	transceiver_cfg_st *p_transceiver_cfg;
	list_st *ipin_used_list;

	if((chan_param == NULL)){
		return -1;
	}

	if((slot == 6) && (chan_param->enable == 0)){
		return 0;
	}

	p_transceiver = &transceiver[slot];
	p_transceiver_cfg = &transceiver_cfg[slot];
	
	
	//*

	DebugPrintf("chan_param->enable = %d \n", chan_param->enable);
	DebugPrintf("chan_param->trans_if = %d \n", chan_param->trans_if);
	DebugPrintf("chan_param->trans_chan = %#x \n", chan_param->trans_chan);
	DebugPrintf("chan_param->trans_mode = %d \n", chan_param->trans_mode);
	DebugPrintf("chan_param->functype = %d \n", chan_param->func_type);
	DebugPrintf("chan_param->func_chan = %d \n", chan_param->func_chan);
	//*/

		
	slot = transceiver_slot[chan_param->trans_if];

	DebugPrintf("trans_if = %d, slot = %d \n", chan_param->trans_if, slot);

	if(tsoip_in_backup.backup_mode){
		if((chan_param->trans_chan&0x900) || (chan_param->trans_chan&0xa00)){
			ipin_used_list = &tsoip_in_backup.channel[chan_param->trans_chan&0xff].user_list;
			if(chan_param->enable){
				list_add(ipin_used_list, chan_param, ASSIGN_FLAG);
			}else{
				list_del(ipin_used_list, chan_param, NULL);
			}
		}
	}
	
	if(slot<sysinfo->board_qty){

		chan_tx_param.enable = chan_param->enable;
		chan_tx_param.trans_if = chan_param->trans_if;
		chan_tx_param.trans_chan = chan_param->trans_chan;
		chan_tx_param.trans_mode = chan_param->trans_mode;
		chan_tx_param.func_type = chan_param->func_type;
		chan_tx_param.func_chan = chan_param->func_chan;
		DebugPrintf("list_add %d slot send channel \n", slot);
		
		list_enqueue(&p_transceiver_cfg->sndchan_list[slot], &chan_tx_param, sizeof(chan_tx_param));

	}else if(chan_param->func_type == FUNC_TYPE_TS_IP_OUT){
		if(chan_param->enable){
			tsoip_out_channel[chan_param->func_chan].src_flag |= 40;
			tsoip_out_channel[chan_param->func_chan].trans_src_chan = chan_param->trans_chan;
			set_ipout_transceiver_source(chan_param->func_chan, chan_param->trans_chan);
		}else{
			tsoip_out_channel[chan_param->func_chan].src_flag &= ~40;
			tsoip_out_channel[chan_param->func_chan].trans_src_chan = 0xffff;
			clean_ipout_transceiver_source(chan_param->func_chan);	
		}
		DebugPrintf("set main board ipout transceiver channel param \n");
	}else if(chan_param->func_type == FUNC_TYPE_TS_IN){
		if(chan_param->enable){
			tsin_mapping_chan[chan_param->func_chan] = chan_param->trans_chan;
			tsin_trans_chan[chan_param->func_chan] = chan_param->trans_chan;
			transceiver_mux_channel_mapping(chan_param->trans_chan, 0x8000|chan_param->func_chan);
		}else{
			tsin_mapping_chan[chan_param->func_chan] = 0xffff;
			transceiver_mux_channel_mapping(chan_param->trans_chan, 0);
		}
		DebugPrintf("set main board mux ts in transceiver channel param \n");
	}
	
	//save_trasceiver_channel(&chan_tx_param);

	DebugPrintf(" ......end.... \n");
	
	return 0;
	
}


int send_transceiver_params(int slot)
{
	//希望以任务队列的方式发送
	int msgid = 0x7f000001;
	channel_tx_param_st *chan_tx_params, *p_tx_param;
	
	spitask_st *p_spitask = NULL;
	transceiver_cfg_st *p_transceiver_cfg;
	list_st *psndchan_list;
	int i=0,  len, ret = 0;
	int chan_count;

	if((slot > sysinfo->board_qty)){
		return -1;
	}

	DebugPrintf("%d slot board \n", slot);

	p_transceiver_cfg = &transceiver_cfg[slot];

	for(slot=0; slot<sysinfo->board_qty; slot++){

		
		psndchan_list = &p_transceiver_cfg->sndchan_list[slot];
		chan_count = psndchan_list->count;

		DebugPrintf("%d slot send channel count %d \n", slot, chan_count);
		
		if(chan_count<=0){
			continue;
		}
		
		len = chan_count * sizeof(channel_tx_param_st);

		chan_tx_params = xMalloc(len);
	
		i=0;
		while(p_tx_param = list_dequeue(psndchan_list)){
			memcpy(&chan_tx_params[i++], p_tx_param, sizeof(channel_tx_param_st));
		}

		ret =spi_send_msg(slot, msgid, chan_tx_params, len);
		free(chan_tx_params);
		
		
	}
	
	
	
	return ret;
	
}

int clean_slot_transceiver_params(int slot)
{
	//希望以任务队列的方式发送
	int msgid = 0x7f000002;
	channel_tx_param_st *chan_tx_params, *p_tx_param;
	
	spitask_st *p_spitask = NULL;
	transceiver_st *p_transceiver;
	list_st *psndchan_list;
	int i=0,  len, ret = 0;
	int chan_count;

	if((slot >= sysinfo->board_qty)){
		return -1;
	}

	DebugPrintf("%d slot board \n", slot);

	
	ret =spi_send_msg(slot, msgid, NULL, 0);
	
	return ret;
	
}

int add_transceiver_route(int dest_slot, int route_id, int src_slot, int trans_chan)
{
	
	transceiver_cfg_st *dest_transceiver;
	routeset_st *routeset;

	DebugPrintf("dest_slot = %d, route_id = %d, src_slot = %d, src_trans_chan = %#x \n", dest_slot, route_id, src_slot, trans_chan);
	
	if((dest_slot>sysinfo->board_qty) || (src_slot>sysinfo->board_qty)){
		return -1;
	}
	
	dest_transceiver = &transceiver_cfg[dest_slot];
	routeset = xMalloc(sizeof(routeset_st));

	routeset->src_slot = src_slot;
	routeset->route_id = route_id;
	routeset->trans_chan = trans_chan;
	routeset->flag = 1;

	return list_enqueue(&dest_transceiver->setroute_list, routeset, ASSIGN_FLAG);
}

int del_transceiver_route(int slot, int route_id)
{
	transceiver_cfg_st *dest_transceiver;
	routeset_st *routeset;

	DebugPrintf("%d slot, route_id %d \n", slot, route_id);

	if(slot>sysinfo->board_qty){
		return -1;
	}
	
	dest_transceiver = &transceiver_cfg[slot];
	routeset = xMalloc(sizeof(routeset_st));

	routeset->route_id = route_id;
	routeset->flag = 0;

	return list_enqueue(&dest_transceiver->setroute_list, routeset, ASSIGN_FLAG);
}

int transceiver_route_add(int dest_slot, int route_id, int src_trans_chan)
{
	int trans_chan_id,  dest_trans_if;
	trans_chan_st *src_chan_param, *old_src_chan_param;
	trans_route_st *trans_route;
	spitask_st *p_spitask;

	if((dest_slot>sysinfo->board_qty)){
		return -1;
	}


	DebugPrintf("add transceiver route : dest_slot = %d, route_id = %d, src_trans_chan = %#x \n"
		,dest_slot, route_id, src_trans_chan);

	trans_route = get_transceiver_route(dest_slot, route_id);
	src_chan_param = get_transceiver_channel(src_trans_chan);

	DebugPrintf(" : %p , %p\n", trans_route, src_chan_param);

	if((trans_route == NULL) || (src_chan_param == NULL)){
		return -1;
	}

	if(trans_route->trans_chan == src_trans_chan){
		return 0;
	}

	transceiver_route_lock(trans_route);

	dest_trans_if = trans_route->trans_if;

	old_src_chan_param = get_transceiver_channel(trans_route->trans_chan);


	DebugPrintf(" --------------1------------\n");
#if 1	
	if(old_src_chan_param != NULL){
		transceiver_channel_lock(old_src_chan_param);
		DebugPrintf(" ----------------1.1----------\n");
		old_src_chan_param->ref_amount--;
		//DebugPrintf("add_transceiver_route : ---------------1.2-----------\n");
		if(old_src_chan_param->ref_amount <=0){
			DebugPrintf(" ------------1.2.2-----old--ref count %d-------\n", old_src_chan_param->ref_amount);
			old_src_chan_param->enable = DISABLE;
			add_transceiver_channel_param(dest_slot, old_src_chan_param);
		}
		
		old_src_chan_param->ref_count[dest_trans_if]--;
		//*
		if(old_src_chan_param->ref_count[dest_trans_if]<=0){
			clean_transceiver_route_regs(old_src_chan_param->trans_chan, dest_trans_if);
		}
		//*/
	
		DebugPrintf(" -------------1.3-------------\n");
		delete_transceiver_route(dest_slot, route_id);
		transceiver_channel_unlock(old_src_chan_param);
	}
#else
src_chan_param->enable = DISABLE;
#endif
	DebugPrintf(" ------------2--------------\n");
	
	transceiver_channel_lock(src_chan_param);
	trans_chan_id = src_chan_param->trans_chan;
	src_chan_param->ref_amount++;
	DebugPrintf(" -----------3-----rer count %d---------\n", src_chan_param->ref_amount);
	if(src_chan_param->enable == DISABLE){
		DebugPrintf("add_transceiver_route : -------------3.1-------------\n");
		src_chan_param->enable = ENABLE;
		add_transceiver_channel_param(dest_slot, src_chan_param);
	}
	transceiver_channel_unlock(src_chan_param);

	DebugPrintf(" --------------4------------\n");
	if(trans_route->enable == DISABLE){
		DebugPrintf(" ----------4.1----------------\n");
		trans_route->enable = ENABLE;
	}
	
	trans_route->trans_chan = trans_chan_id;
	DebugPrintf(" ---------5-----trans chan id %#x------------\n", trans_chan_id);
	add_transceiver_channel_param(dest_slot, (trans_chan_st *)trans_route);

	//*
	if(src_chan_param->ref_count[dest_trans_if]<=0){
		set_transceiver_route_regs(trans_chan_id, dest_trans_if);
		src_chan_param->ref_count[dest_trans_if] = 1;
	}else{
		set_transceiver_route_regs(trans_chan_id, dest_trans_if);
		src_chan_param->ref_count[dest_trans_if]++;
	}
	//*/

	if(sysinfo->slot_info[dest_slot].transceiver_flag == 2)
		save_transceiver_route(trans_route);

	transceiver_route_unlock(trans_route);

	return 0;
}

int transceiver_route_del(int slot, int route_id)
{
	int trans_if, src_trans_if, trans_chan_id;
	trans_route_st *trans_route;
	trans_chan_st *src_chan_param;
	spitask_st *p_spitask;

	if(slot>sysinfo->board_qty){
		return -1;
	}
		
	trans_route = get_transceiver_route(slot, route_id);
	if(trans_route->trans_chan == 0xffff){
		return -1;
	}
	
	DebugPrintf("del transceiver route : slot = %d, route_id = %d\n", slot, route_id);

	trans_if = trans_route->trans_if;

	transceiver_route_lock(trans_route);

	delete_transceiver_route(slot, route_id);

	src_chan_param = get_transceiver_channel(trans_route->trans_chan);
	
	transceiver_channel_lock(src_chan_param);
	src_trans_if = src_chan_param->trans_if;
	trans_chan_id = src_chan_param->trans_chan;
	src_chan_param->ref_amount--;
	if(src_chan_param->ref_amount <=0){	
		
		src_chan_param->enable = DISABLE;
		add_transceiver_channel_param(slot, src_chan_param);
	}
	transceiver_channel_unlock(src_chan_param);

	src_chan_param->ref_count[trans_if]--;
	//*
	if(src_chan_param->ref_count[trans_if]<=0){
		clean_transceiver_route_regs(trans_chan_id, trans_if);
	}
	//*/

	trans_route->enable = DISABLE;
	add_transceiver_channel_param(slot, (trans_chan_st *)trans_route);
	trans_route->trans_chan = 0xffff;
	
	
	transceiver_route_unlock(trans_route);

	return 0;
}

int empty_transceiver_route(int slot)
{
	int i;
	transceiver_st *dest_transceiver = &transceiver[slot];

	for(i=0; i<dest_transceiver->route_qty; i++){
		transceiver_route_del(slot, i);
	}
	send_transceiver_params(slot);
}


int config_transceiver(int slot)
{
	routeset_st *routeset;
	channel_tx_param_st  *p_tx_param;	
	transceiver_st *p_transceiver;
	transceiver_cfg_st *p_transceiver_cfg;
	int i=0, j=0, route_id = 0, max_route;
	int tran_idx, chan_count, avg_qty;
	trans_route_st *trans_route;
	int tsin_chan = 0;

	if((slot > sysinfo->board_qty)){
		return -1;
	}

	DebugPrintf("%d slot\n", slot);

	

	p_transceiver = &transceiver[slot];

	if(p_transceiver->run_flag != RUNNING_FLAG){
		return -1;
	}
	
	p_transceiver_cfg = &transceiver_cfg[slot];

	chan_count = p_transceiver_cfg->setroute_list.count;
	
	DebugPrintf("setroute_list count = %d \n", chan_count);
	
	if(chan_count<=0){
		//???????????????
		return clean_slot_transceiver_params(slot);
	}
	
	if(slot == 6){
		avg_qty = chan_count / p_transceiver->transif_qty;
		if(avg_qty==0){
			avg_qty = 1;
		}
		
		max_route = p_transceiver->route_qty/p_transceiver->transif_qty;
	}

	
	while(routeset = list_dequeue(&p_transceiver_cfg->setroute_list)){
				
		if(routeset->flag){
			if(slot == 6){
				tran_idx = i/avg_qty;
				route_id = i%avg_qty;

				if(tran_idx>=p_transceiver->transif_qty){
					tran_idx = p_transceiver->transif_qty - 1;
					route_id += avg_qty;
				}
				
				route_id += tran_idx*max_route;

				DebugPrintf("routeid = %d, tran_idx  = %d, avg_qty = %d, max_route = %d \n", route_id, tran_idx, avg_qty, max_route);

				i++;
			}
			#if 1
			else if((slot== 7) && (routeset->route_id>=base_tsin_routeid)){
				
				for(j=0; j<sysinfo->tsin_channels; j++){
					trans_route = get_transceiver_route(slot, j+base_tsin_routeid);
					if(trans_route->trans_chan == routeset->trans_chan){
						break;
					}else if(tsin_trans_chan[j] == routeset->trans_chan){
						route_id = j+base_tsin_routeid;
						TagPrintf(j);
						goto Route_Add;
					}
				}
				if(j<sysinfo->tsin_channels){
					printf("hashmap get tsin trans routeid = %d, trans_chan = %#x \n", trans_route->route_id, trans_route->trans_chan);
					continue;
				}

				for(;tsin_chan<sysinfo->tsin_channels; tsin_chan++){
					//trans_route = get_transceiver_route(slot, tsin_chan+base_tsin_routeid);
					//printf("tsin_mapping_chan[%d] : %#x \n", tsin_chan, trans_route->trans_chan);
					//if(trans_route->trans_chan==0xffff){
					if(tsin_trans_chan[tsin_chan] == 0xffff){
						//route_id = trans_route->route_id;
						route_id = tsin_chan+base_tsin_routeid;
						TagPrintf(tsin_chan);
						break;
					}
				}
				
			}
			#endif
			else {
				route_id = routeset->route_id;
			}
			Route_Add:
			transceiver_route_add(slot, route_id, routeset->trans_chan);
		}else{
			route_id = routeset->route_id;
			transceiver_route_del(slot, route_id);
		}
	}

	return send_transceiver_params(slot);

}

int sync_transceiver_route_tables(int slot)
{
	int i;
	transceiver_st *p_transceiver;
	spitask_st *p_spitask;
	trans_chan_st *psrc_chan;
	trans_route_st *trans_route;

	int trans_chan, route_id;



	printf("sync_transceiver_route_tables %d slot \n", slot);


	p_transceiver = &transceiver[slot];

	//sysinfo.slot_info[slot].old_param_isvalid = 1;
	//sysinfo.slot_info[slot].transceiver_flag = 1;
	//enable_transceiver(slot);

	if(sysinfo->slot_info[slot].transceiver_flag == 1){
		select_transceiver_routetable(slot);
		//config_transceiver(slot);
	}else{

		
		for(i=0; i<p_transceiver->route_qty; i++){
			trans_route = get_transceiver_route(slot, i);
			if(trans_route->trans_chan != 0xffff){
				//add_transceiver_route(trans_if, trans_route->route_id, trans_route->trans_if, trans_route->trans_chan);
				set_transceiver_route_regs(trans_route->trans_chan, trans_route->trans_if);
				add_transceiver_channel_param(slot, (trans_chan_st *)trans_route);
			}
		}
		

	}

	for(i=0; i<p_transceiver->channel_qty; i++){
		//psrc_chan = sArrList_get(p_transceiver->channel_list, i);
		psrc_chan = get_transceiver_channel((slot<<8) | i);
		if(psrc_chan->enable){
			add_transceiver_channel_param(slot, psrc_chan);
		}
	}

	send_transceiver_params(slot);

	usleep(1000);



	//if(sysinfo.slot_info[slot].transceiver_flag == 1)
		sysinfo->slot_info[slot].transceiver_flag = 2;
	
	//sched_yield();
	//sleep(1);

	return 0;
}


void monitor_transceiver_channel(void)
{
	int i, trans_if;
	transceiver_st *p_transceiver;
	trans_route_st *trans_route;
	int byterate;

	while(sysinfo->run_flag){
	
		for(trans_if=0; trans_if<=sysinfo->board_qty; trans_if++){
			p_transceiver = &transceiver[trans_if];

			if(p_transceiver->run_mode == 0){
				usleep(100);
				continue;
			}

			for(i=0; i<p_transceiver->route_qty; i++){
				trans_route = get_transceiver_route(trans_if, i);
				
				byterate = read_tsin_channel_byterate(trans_route->trans_chan);
				if(trans_route->trans_chan_0 == trans_route->trans_chan){
					if(byterate < 10){
						trans_route->test_count = 0;
						transceiver_route_add(trans_route->board_slot, trans_route->route_id, trans_route->trans_chan_1);
					}
				}else{
					if(byterate < 10){
						byterate = read_tsin_channel_byterate(trans_route->trans_chan_0);
						if(byterate >= 10){
							transceiver_route_add(trans_route->board_slot, trans_route->route_id, trans_route->trans_chan_0);
						}
					}else{
						byterate = read_tsin_channel_byterate(trans_route->trans_chan_0);
						if(byterate>=10){
							trans_route->test_count++;
						}else{
							trans_route->test_count = 0;
						}

						if(trans_route->test_count>3){
							transceiver_route_add(trans_route->board_slot, trans_route->route_id, trans_route->trans_chan_0);
						}
					}
				}
				
			}
			
			send_transceiver_params(trans_if);

			usleep(100);
			
		}

		sched_yield();
		usleep(10000);
		
	}
}
#endif


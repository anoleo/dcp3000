#ifndef __TRANSCEIVER__H_
#define __TRANSCEIVER__H_

#include "../system.h"
#include "../utils/list.h"
#include "../utils/sArrList.h"
#include "../utils/arraylist.h"

#include "spi.h"

#define MAX_TRANSCEIVER_QTY	16


#define TRANS_NONE	0
#define TRANS_SRC	1
#define TRANS_DEST	2
#define TRANS_BOTH	3

extern int transceiver_slot[];

struct transceiver_channel_param{
	char trans_if;
	char board_slot;
	short ref_amount;
	char lock;
	char enable;
	char trans_mode;
	char func_chan;
	u_short trans_chan;
	u_short func_type;


	int bitrate;
	char ref_count[MAX_TRANSCEIVER_QTY];
};

typedef struct transceiver_channel_param trans_chan_st;

struct transceiver_route{
	char trans_if;
	char board_slot;
	short ref_amount;
	char lock;
	char enable;
	char trans_mode;
	char func_chan;
	u_short trans_chan;
	u_short func_type;

	short route_id;
	short trans_chan_0;
	short trans_chan_1;
	short test_count;
};

typedef struct transceiver_route trans_route_st;


struct transceiver_config{
	arraylist_st *channel_list;
	list_st setroute_list;
	list_st sndchan_list[MAX_SLOT_QTY+1];
	pthread_mutex_t lock;
};

typedef struct transceiver_config transceiver_cfg_st;

struct transceiver_module{
	
	char transif_s;
	char transif_qty;
	
	short run_flag;
	int run_mode;		// 0 normal, 1 backup
	int tx_bitrate;
	int rx_bitrate;
	int channel_qty;
	int route_qty;
	
};

typedef struct transceiver_module transceiver_st;

extern transceiver_st *transceiver;

extern transceiver_cfg_st transceiver_cfg[MAX_TRANSCEIVER_QTY];

extern trans_chan_st *transceiver_channels;
extern trans_route_st *transceiver_routes;

extern u_short *transceiver_route_reg_map;

extern u_short  *tsin_mapping_chan;
extern u_short *tsin_trans_chan;

#define transceiver_mutex_init(p_transceiver)	pthread_mutex_init(&((p_transceiver)->lock), NULL)
#define transceiver_lock(p_transceiver)		pthread_mutex_lock(&((p_transceiver)->lock))
#define transceiver_unlock(p_transceiver)		pthread_mutex_unlock(&((p_transceiver)->lock))


void init_transceiver(int slot);
void free_transceiver(int slot);
int allocate_transceiver(int slot);

void transceiver_reconfig(int trans_if);
int config_transceiver(int slot);

int add_transceiver_route(int desct_slot, int route_id, int src_slot, int trans_chan);
int del_transceiver_route(int desct_slot, int route_id);
int empty_transceiver_route(int desct_slot);

int send_transceiver_params(int slot);
int clean_slot_transceiver_params(int slot);

int sync_transceiver_route_tables(int slot);

trans_chan_st *find_transceiver_channel(int slot, int idx);

trans_chan_st *get_transceiver_channel(u_short trans_chan);
trans_route_st *get_transceiver_route(int slot, int route_id);

int enable_transceiver(int slot);
int disable_transceiver(int slot);

u_int read_transceiver_channel_byterate(int channel);
u_int read_transceiver_channel_valid_byterate(int channel);

int set_transceiver_route_regs(int trans_chan, int dest_trans_if);
void clean_transceiver_route_regs(int trans_chan, int dest_trans_if);

void set_asiout_transceiver_source(int trans_chan);

void set_ipout_transceiver_source(int ipout_chan, int trans_chan);
void clean_ipout_transceiver_source(int ipout_chan);
void transceiver_ipout_channel_mapping(u_int original_channel, u_int target_channel);
void transceiver_mux_channel_mapping(u_int original_channel, u_int target_channel);

int debug_transceiver(int argc, char *argv[]);

#endif


#ifndef ___TSOIP_H___
#define ___TSOIP_H___

#include <sys/types.h>
#include "../utils/system.h"

#define TSOIP_PORT_QTY		2

#define MAX_TSOIP_IN_CHANNEL_QTY	512
#define MAX_TSOIP_OUT_CHANNEL_QTY	48



#define TS_RECOVERY_MOTHOD_RTP	2
#define TS_RECOVERY_MOTHOD_VBR	1
#define TS_RECOVERY_MOTHOD_CBR	0

extern int ipin_channel_ref[];
extern int ipout_channel_ref[];

extern int ipin_idx_ref[];
extern int ipout_idx_ref[];
extern int ipout_routeid[];
extern int ipin_trans_srcidx[];

typedef struct
{
	char channel_license;
	char input_format;
	u_short ts_packets;
	u_short cfec_packets;
	u_short rfec_packets;
	u_int input_bitrate;		//kbit in second
}ipout_status_st;

typedef struct
{
	char channel_license;
	char format;				//bit[0] = rtp/udp,1=rtp,bit[1]=2d_fec,bit[2] = fec_lock,
	char lsize;
	char dsize;
	u_int src_ipaddr;
	u_int ts_byterrate;
	u_short udp_length;
	u_short recovery_packets;
	u_int channel_lock;	//0=not lock,1=si not ready,2=locked
	char ts_flag;
	char SI_ok;

	u_int lost_packets;
	
	unsigned int bitrate;		//bitrate

}ipin_status_st;


extern ipin_status_st ipin_status[];
extern ipout_status_st ipout_status[];

//extern unsigned char monitor_ch;


typedef struct {
	
	

} tsoip_nif_status_st;

typedef struct {
	
	

} tsoip_nif_params_st;



typedef struct {
	
	u_int ipaddr;
	u_int netmask;
	u_int gateway;

	u_char ttl;
	u_char tos;
	u_char packets_per_udp;
	u_char igmp_mode;	// 0 auto, 1=v2, 2=v3

	
	u_char macaddr[6];
	
	u_int link_status;
	
	u_short inchan_qty;
	u_short outchan_qty;
	u_short inchan_s;
	u_short outchan_s;
	
	u_int in_bitrate;	//kbps
	u_int out_bitrate;	//kbps
	
	char nif_name[20];

} tsoip_network_params_st;

extern tsoip_network_params_st *tsoip_network;






typedef struct {
	u_char	enable;
	u_char	mode; //0 unicast, 1 mutilcast
	u_char	ts_recovery_method;	// 2--RTP stamp 1--VBR,0--CBR
	u_char	src_filter_enable;
	
	u_int	src_ipaddr;
	
	u_int	target_ip;
	u_short	target_port;
	
	u_short	reserver0;
	
	u_short	reserver1;
	u_char	channel;
	u_char	nif_no;;			//0 = A,1 =B
	u_int	last_target_ip;
} tsoip_in_channel_st;

extern tsoip_in_channel_st *tsoip_in_channel;


typedef struct _tsoip_out_one_channel_t {
	
	u_char channel;
	u_char enable_mode; // bit[0] = enable, [2:1]=1:port A,2:port B,3:port A & B
	char fec_idx;
	char test_lost;
	u_char packets_per_udp; // 1-7
	u_char src_flag;
	u_short trans_src_chan;
	u_short	reserver;
	u_char encapsul_type[TSOIP_PORT_QTY];	// 1:udp,0:RTP
	
	u_int target_ip[TSOIP_PORT_QTY]; //portA, portB
	u_short target_port[TSOIP_PORT_QTY]; //1024 - 65535

	u_char tos[TSOIP_PORT_QTY];  // 0-255
	u_char ttl[TSOIP_PORT_QTY]; // 0- 255

} tsoip_out_channel_st;
extern tsoip_out_channel_st *tsoip_out_channel;




void init_tsoip_networks(void);


void init_tsoip_in_channels(void);

void init_tsoip_out_channels(void);

void set_tsoip_network(int ifno);
void reset_tsoip_networks(void);

void set_tsoip_in_channel(int channel);
void set_ipin_channel(int nif_no, int channel, tsoip_in_channel_st *ip2ts, u_int src_ipaddr, int enable);

void set_tsoip_out_channel(int channel);



u_int get_tsoip_netif_link_status(int ifno);

int read_tsoip_netif_in_bitrate(int ifno);
 
int read_tsoip_netif_out_bitrate(int ifno);

void read_ipin_status(int channel);
void read_ipout_status(int channel);

int read_ipin_channel_bitrate(int ifno, int channel);
u_int read_ipin_channel_srcip(int ifno, int channel);

void tsoip_add_task(u_short cmd_type, u_short idx);

iproute_st *find_iproute(int route_type, u_int dest_ip, u_int netmask, u_int nif_gw);
int add_iproute(int route_type, u_int dest_ip, u_int netmask, u_int nif_gw);
int del_iproute(int route_type, u_int dest_ip, u_int netmask, u_int nif_gw);

void init_tsoip_env(void);

void init_tsoip(void);


#endif


#ifndef _WEB_MUX_MSG_H_
#define _WEB_MUX_MSG_H_

#define WEB_MSG_IP_PORT 	2
#define CA_SERVER_QTY		4
#define BISS_1_EVENTKEY_LEN	6
#define BISS_E_EVENTID_LEN		7
#define BISS_E_EVENTKEY_LEN	8
#define MAX_PROGRAM_NAME_LEN		48
#define ES_PID_QTY_IN_A_PROGRAM		10

#define ECM_PID_QTY_IN_A_PROGRAM		10
#define EMM_PID_QTY_IN_A_TS		10

#define AC_DATA_LEN	256
#define	CA_DESC_DATA_LEN		256

#define _WEB_MUX_MSG_H_

//SUPPORT
#define SUPPORT_MODULATOR (1<<0)
#define SUPPORT_MUX  (1<<1)
#define SUPPORT_CA  (1<<2)

//PID MASK FIST 3 BIT
#define VIDEO_STREAM		0x02
#define AUDIO_STREAM		0x04
#define OTHER_STREAM		0x06

#define ECM_STREAM		0x01
#define EMM_STREAM		0x03
#define PCR_STREAM		0x05
#define PMT_PACKET		0x07


typedef struct{
 unsigned int ipin_channels;
 unsigned int asiin_channels;
 unsigned int asiout_channels;
 unsigned int total_tsin_channels;
 unsigned int ipout_channels;
 unsigned int mux_channels;
 unsigned int qam_channels;
 unsigned int tsip_port;   // 1 or 2
  u_short device_type;  // 0=MODULATOR,1=MUX,2=CA
 
 unsigned char modulator_type; // 0 maxim, 1 blocklogic

 u_char ca_server_qty;
 char device_name[20];
 char device_sn[20];
 u_char rom_sn[8];
 u_char license[8];
 u_short auth_code;
 u_char support_crc_monitor;
 u_char reserved;
} sys_info_t;

#if 1

typedef struct {
	u_char service_type;
	u_char service_name[MAX_PROGRAM_NAME_LEN];
	u_char provider_name[MAX_PROGRAM_NAME_LEN];
}msg_tx_service_info_t;

#endif

typedef struct {
	int index;
	char time_str[20];
	char msg_str[48];
} msg_tx_eventlog_t;


typedef struct {
	u_char	ttl;
	u_char	tos;
	u_char	packets_per_udp;
	u_char	reserved;
}msg_tx_tsoip_protocol_t;


typedef struct {
	u_int ip_addr;
	u_int net_mask;
	u_int gate_way;
	u_int trap_ip;
	char mac_addr[6];
} msg_tx_mng_network_t;


//TSOIP_NETWORK_DATA_TYPE
typedef struct {
	u_int ip_addr;
	u_int net_mask;
	u_int gate_way;
	u_int mac_high;
	u_int mac_low;
	u_char port_mode; // 0-rh45,1-SFP
	u_char IGMP_Version; // 0-version 2,1-version 3
	u_short port_num; // 
} msg_tx_tsoip_network_t;

//TSOIP_IN_CHANNEL_DATA_TYPE
typedef struct {
	u_short channel;//channel
	u_short  port_num;//Port
	u_int target_ip;//IP Address
	u_char enable;//enable
	u_char multi_flag;//uni/Multicast
	u_short	reserved;
} msg_tx_ipin_channel_t;

//TSOIP_OUT_CHANNEL_DATA_TYPE
typedef struct {
	u_short chan_num;
	u_char	enable_mode; // [2:0]=1:port A,2:port B,3:port A & B
	u_char	packets_per_udp;// 1-7
	
	u_char reserved[WEB_MSG_IP_PORT];
	
	u_char	encapsul_type[WEB_MSG_IP_PORT];//--1:udp,0:RTP
	
	u_int	target_ipaddr[WEB_MSG_IP_PORT];
	u_short	dest_port[WEB_MSG_IP_PORT];
	
	u_char	tos[WEB_MSG_IP_PORT];//
	u_char	ttl[WEB_MSG_IP_PORT];//
	//u_char	output_mode;
}msg_tx_ipout_channel_t;


//TSIN_PROGRAM_INFO_DATA_TYPE
typedef struct {

	u_short program_number;
	u_short	pmt_pid;
	u_short pcr_pid;
	u_char	free_CA_mode;
	u_char	service_type;
	char	service_name[MAX_PROGRAM_NAME_LEN];
	char	provider_name[MAX_PROGRAM_NAME_LEN];
	u_short	es_pid[ES_PID_QTY_IN_A_PROGRAM];
	u_short	ecm_pid[ECM_PID_QTY_IN_A_PROGRAM];
	
}msg_tx_program_info_t;

typedef struct {
	short	mux_chan;
	
	char 	enable; // 0:disable, 1:enable
	char	pid_set_auto;
	
	char	insert_sdt_flag; //0:disable, 1:enable
	char	insert_nit_flag; //0:disable, 1:enable
	char	insert_eit_flag; //0:disable, 1:enable
	u_char	packet_length; 	 //0:188 bytes, 1:204 bytes
	
	u_short	ts_id;
	u_short	network_id;
	u_short	ori_network_id;

	u_short	reserved;
	
	u_int	total_bitrate; // bps
}msg_tx_mux_parameter_t;


typedef struct {
	u_char	enable;
	u_char	ac_index;
	u_short	ecm_pid;
} msg_tx_ca_usage_t;

typedef struct {
	u_char biss_e_id[BISS_E_EVENTID_LEN*2 + 2];
	u_char biss_e_key[BISS_E_EVENTKEY_LEN*2];
	u_char biss_1_key[BISS_1_EVENTKEY_LEN*2];
}msg_tx_biss_key_t;

typedef struct {
	int ac_index;
	u_char	ac_name[AC_DATA_LEN/4];	// string
	u_char	ac_data[AC_DATA_LEN];	// hex string
	u_char	ecm_desc_data[CA_DESC_DATA_LEN];	// hex string
} msg_tx_ac_t;

//MUX_PROGRAM_INFO_DATA_TYPE
typedef struct __mux_program_t_{
	u_short in_chan;
	u_short	reserved;
	
	u_short old_pn;
	u_short new_pn;
	u_short in_pmt_pid;
	u_short mux_pmt_pid;
	u_short in_pcr_pid;
	u_short mux_pcr_pid;
	u_short es_pid[10][2];

	msg_tx_service_info_t service_info;

	u_int cas_mode;
	msg_tx_ca_usage_t	ca_usage[CA_SERVER_QTY];
	msg_tx_biss_key_t biss_key;
	
//struct _mux_program_t *next;
}msg_tx_mux_program_t;


typedef struct {
	u_char	ca_server_no;
	u_char	enable;
	
	u_short ca_system_id;
	u_short ca_sub_id;
	
	u_short	emm_pid;
	
	u_char	protocol_version;	//Version of ECMG protocol
	u_char	emmg_protocol;		//tcp or udp


	u_short	ecmg_port;
	u_short	emmg_tcp_port;
	u_short	emmg_udp_port;
	u_int	ecmg_ip;
	u_int	emmg_ip;
	u_int	emmg_bandwidth;//++
	
	u_char	emm_private_data[CA_DESC_DATA_LEN];// hex string
	
	u_char	ca_server_name[48];
} msg_tx_ca_server_t;

typedef struct {
	u_int cas_mode;
	msg_tx_ca_usage_t	ca_usage[CA_SERVER_QTY];
	msg_tx_biss_key_t biss_key;
}msg_tx_program_scramble_t;


typedef struct {
	unsigned char channel;
	unsigned char connected;
	unsigned char protocol_version;
	u_char reserved0;
 	u_short reserved1;
 	u_short ip_port;
	u_int ip_addr;
	unsigned long messages_received;
	unsigned int protocol_errors;
	unsigned int parameter_errors;
	unsigned int internal_errors;
	unsigned int channel_tests;
	unsigned int channel_errors;
	unsigned int stream_tests;
	unsigned int stream_errors;
	unsigned int connect_timeout_errors;
	unsigned int cw_provision_timeout_errors;
	unsigned long cw_provision_counts;
}msg_tx_ecmg_status_st;

typedef struct {
	unsigned char channel;
	unsigned char connected;
	u_short ip_port;
	u_int ip_addr;
	unsigned long messages_received;
	unsigned int protocol_errors;
	unsigned int parameter_errors;
	unsigned int internal_errors;
	unsigned int channel_tests;
	unsigned int channel_errors;
	unsigned int stream_tests;
	unsigned int stream_errors;
	unsigned long tcp_data_provisions;
	unsigned long udp_data_provisions;
}msg_tx_emmg_status_st;



//////--------------------

typedef struct{
	u_char chan_num;
	u_char freq_idx;//#
	u_char rf_sw;//#
	u_char modulate_sw;//#

	u_int frequence;	//kHz   #
	u_int symbolrate;	//kbaud/s  #


	u_char qam_constel;	//6 : 64qam, 8 : 256qam 
	u_char annex_abc;	//0 : annex A, 1 : annex B, 2 : annex C, 3 : annex A 6M 
	u_char roll_off;		//0 : 0.12, 1 : 0.13, 2 : 0.15, 3 : 0.18
	u_char bandwidth;	//6-8M 
	
	u_char icw;
	u_char spec_inv;	//?
	u_char ts_src;//***
	u_char reserved;
} msg_tx_qam_channel_t;

typedef struct {
	
	u_int group_start_freq[4];
	
	u_int symbolrate;	//kbaud/s
	
	u_char qam_constel;	//6 : 64qam, 8 : 256qam
	u_char annex_abc;	//0 : annex A, 1 : annex B, 2 : annex C, 3 : annex A 6M 
	u_char rolloff;		//0 : 0.12, 1 : 0.13, 2 : 0.15, 3 : 0.18
	u_char bandwidth;	//6-8M
	
	u_char icw;		//Interleaver Control Word(annex B)
	u_char spec_inv;	//Spectrum Inversion
	u_char main_sw;		//0 turn off, 1 turn on

	u_char reserved;

} msg_tx_qam_globle_param_t;

//u_int rf_level;	//75 - 105 //

typedef struct {
	u_char enable_flag;	
	u_char out_mode;	//0 backup, 1 master
	u_short reserved;
}msg_tx_rf_backup_t;

typedef struct {	
	u_char type;	
	u_char flag;	
	u_char name[20];	
	u_short in_channel_count;
	u_short out_channel_count;
}msg_card_status_t;

typedef struct {	
	u_char type;	
	u_char name[20];	
	u_char card_count;	
	msg_card_status_t subcard_info[7];
}msg_subcard_info_t;
#endif



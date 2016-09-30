
#ifndef ___MUX_H___
#define ___MUX_H___

#include <unistd.h>
#include "capture.h"

#include "table.h"
#include "psi_si.h"

#define DEBUG_CAPTURE	0
#define INFO_CAPTURE	0
#define	EIT_CAPTURE		0


#define DEBUG_SI_CRC_MONITOR 0
#define INFO_SI_CRC_MONITOR 0

#define MUX_INFO	1
#define MUX_DEBUG	1


#if DEBUG_CAPTURE
#define capture_DebugPrintf(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#else
#define capture_DebugPrintf(fmt, ...)
#endif
#if INFO_CAPTURE
#define capture_InfoPrintf(fmt, ...)		printf(fmt, ##__VA_ARGS__)
#else
#define capture_InfoPrintf(fmt, ...)
#endif

#if MUX_INFO
#define mux_InfoPrintf(fmt, ...)		printf(fmt, ##__VA_ARGS__)
#else
#define mux_InfoPrintf(fmt, ...)
#endif
#if MUX_DEBUG
#define mux_DebugPrintf(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#else
#define mux_DebugPrintf(fmt, ...)
#endif


#define MAX_MUX_CHANNEL_QTY	48


#define	MUX_NORMAL		0
#define	MUX_SHUTDOWN		1
#define	MUX_NOT_TS		2
#define	MUX_OVERFLOW		3


#define	INTERNAL_27M_CLK		0
#define	EXTERNAL_16M_CLK		1


#define MIN_SI_TICKER	2500

#define SI_ZONE_0_TICKER	10000
#define SI_ZONE_1_TICKER	20000
#define SI_ZONE_2_TICKER	50000
#define SI_ZONE_3_TICKER	2000000

#define MUX_SI_ZONE_NUM	4//20
#define PSISI_SI_ZONE_QTY	4

#define MUX_SI_OUTPUT_BYTES_IN_CHAN		SZ_1M


#if DDR_PAGING_ACCESS
#define DDR_Page_MUX_SI_Buf(out_chan)		 ((DDR_SIOUT_BASE + (out_chan)*MUX_SI_OUTPUT_BYTES_IN_CHAN)/FPGA_PAGE_SIZE)
#define DDR_Offset_MUX_SI_Buf(out_chan)		 ((DDR_SIOUT_BASE + (out_chan)*MUX_SI_OUTPUT_BYTES_IN_CHAN)%FPGA_PAGE_SIZE)
#else
#define DDR_Page_MUX_SI_Buf(out_chan)			0
#define DDR_Offset_MUX_SI_Buf(out_chan)		 (DDR_SIOUT_BASE + (out_chan)*MUX_SI_OUTPUT_BYTES_IN_CHAN)
#endif



#define MAX_MUX_PACKET_NUMBER	(MUX_SI_OUTPUT_BYTES_IN_CHAN/(192<<1))		//2730	// (1MB / (192*2))


#define MUX_TOP_MUX_CHANNEL		(MUX_TOP_BASE_REG+(0x1ff*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_MUX_ENABLE		(MUX_TOP_BASE_REG+(0x100*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_EDGEQAM_MODE		(MUX_TOP_BASE_REG+(0x102*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_SI_ADDRESS_LO		(MUX_TOP_BASE_REG+(0xf02*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_SI_ADDRESS_HI		(MUX_TOP_BASE_REG+(0xf03*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_NCO2_LOW			(MUX_TOP_BASE_REG+(0xf04*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_NCO2_HIGH			(MUX_TOP_BASE_REG+(0xf05*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_NCO2_PLL_SEL		(MUX_TOP_BASE_REG+(0xf06*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_BUF_OUT_DEPTH	(MUX_TOP_BASE_REG+(0xf07*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_SI_COUNT			(MUX_TOP_BASE_REG+(0xf08*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_SI_CHANNEL		(MUX_TOP_BASE_REG+(0xf09*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_SI_TICKLE_LO		(MUX_TOP_BASE_REG+(0xf0a*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_SI_TICKLE_HI		(MUX_TOP_BASE_REG+(0xf0b*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_SI_PACKET_NUM		(MUX_TOP_BASE_REG+(0xf0c*(MUXREG_BIT_WIDTH/8)))

//PCR restamp
#define MUX_TOP_PCR_MODE			(MUX_TOP_BASE_REG+(0X8e00*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_NEW_PCRPID		(MUX_TOP_BASE_REG+(0X8e01*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_PCR_INTERVAL		(MUX_TOP_BASE_REG+(0X8e02*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_STREAM_NUM		(MUX_TOP_BASE_REG+(0X8e04*(MUXREG_BIT_WIDTH/8)))
#define MUX_TOP_STREAM_RAM		(MUX_TOP_BASE_REG+(0X8f00*(MUXREG_BIT_WIDTH/8)))


extern char the_first_run_flag[MAX_MUX_CHANNEL_QTY];

typedef struct{

	u_int	total_tsin_channels;
	u_short	mux_channels;
	u_char	support_crc_monitor;
	u_char	support_ca;
	int base_routeid;
	
} mux_info_t;
extern mux_info_t mux_info;

typedef struct{
	u_short pid;
	u_short siid;
}siidmap_st;

typedef struct{
	int count;
	int offset;
	siidmap_st *buf;
	
} pid_hash_buf_t;



typedef struct _pid_mapping_s pid_mapping_t;
struct _pid_mapping_s{
	u_short in_chan;
	u_short pid_type;
	u_short in_pid;
	u_short mux_pid;
	pid_mapping_t *next;
};
#define MUX_PID_OUT_ON	0x4000



typedef struct _mux_si_zone_s mux_si_zone_t;
struct _mux_si_zone_s{
	u_char zone_num;
	u_char mutex;
	u_char enable;
	int packet_point;
	mux_packet_t *p_packet;
	mux_packet_t *cur_packet;
	mux_si_zone_t *next;
};


#define CAS_SI_ZONE_BASE	PSISI_SI_ZONE_QTY


#define	MUX_SI_PACKET_SIZE	0x180//192*2
#define	MUX_SI_SECTION_SIZE	0x900//192*2*6
#define	MUX_ECM_SI_ZONE_OFFSET	0xf0000//0x180*0xaa//0x18000//0xe1000//400*192*2*6


typedef struct _tsin_within_mux_st{
	u_short	in_chan;
	u_short	operated_mask;
	struct _tsin_within_mux_st *next;
}tsin_within_mux_t;



typedef struct _mux_program_t{

	u_short	in_chan;
	u_short old_pn;
	u_short new_pn;
	
	u_short in_pmt_pid;
	u_short mux_pmt_pid;
	
	u_short in_pcr_pid;
	u_short mux_pcr_pid;
	
	u_char	cas_mode;
	u_char	free_CA_mode;

	int char_type;


	u_short es_pid_mapping[ES_PID_QTY_IN_A_PROGRAM][3];

	
	service_info_t old_service_info;
	service_info_t service_info;
	mux_section_t *p_section;
	
	struct _mux_program_t *next;
	struct _mux_program_t *link;
}mux_program_t;


typedef struct {
	char	mux_chan;
	char 	enable; // 0:disable, 1:enable
	
	char	insert_sdt_flag; //0:disable, 1:enable
	char	insert_nit_flag; //0:disable, 1:enable
	char	insert_eit_flag; //0:disable, 1:enable
	u_char	packet_length; 	 //0:188 bytes, 1:204 bytes
	
	u_short	ts_id;
	u_short	network_id;
	u_short	ori_network_id;
	
	u_int	total_bitrate; // kbps
	char channel_name[48];
}mux_parameter_t;

typedef struct _mux_channel_s{

	char	mux_chan;
	char 	enable; // 0:disable, 1:enable
	
	char remove_ca_flag; //0:don't, 1:do
	char	insert_sdt_flag; //0:disable, 1:enable
	char	insert_nit_flag; //0:disable, 1:enable
	char	insert_eit_flag; //0:disable, 1:enable
	u_char	packet_length; 	 //0:188 bytes, 1:204 bytes
	
	u_short	ts_id;
	u_short	network_id;
	u_short	ori_network_id;
	
	u_int	total_bitrate; // kbps

	char channel_name[48];
	//----------------------
	u_short flag;
	u_short run_status;
	
	char	reperform_flag;
	char	free_CA_mode;
	char	pid_set_auto;
	
	u_short	pcr_interval;	// pcr间隔，以10us为单位

	u_short tsin_change_flag;
	
	u_int valid_bitrate;	//kbps

	tsin_within_mux_t *used_tsin;

	
	int pass_pid_count;
	pid_mapping_t *pid_mapping_list;
	pid_mapping_t *pass_pids;
	pid_mapping_t *last_pid_mapping_list;
	

	int program_count;
	mux_program_t	*mux_programs;
	mux_program_t	*mux_programs_raw;
	mux_program_t	*mux_programs_tail;

	int section_count;
	mux_section_t	*mux_sections;


	//-----------------------------

	//mux_si_zone_t *si_zone;
	mux_si_zone_t si_zone[MUX_SI_ZONE_NUM];		//0-2, psi-si; 
	pid_hash_buf_t *si_id_buf;



} mux_channel_t;

#define MUX_BE_READY		(1 << 0)
#define MUX_PID_MAP_FINISH	(1 << 1)
#define MUX_PID_WRITED_FPGA	(1 << 2)
#define MUX_PACKET_FINISH	(1 << 3)
#define MUX_PACKET_WRITED_FPGA	(1 << 5)
#define MUX_SET_FPGA_OK		(1 << 6)


#define MUX_SI_UPDATE_END_OK		(1 << 7)


extern mux_channel_t mux_channels[MAX_MUX_CHANNEL_QTY]; 



void lock_fpga_mux_channel(void);
void unlock_fpga_mux_channel(void);

void unlock_mux_channel(int mux_chan);
void lock_mux_channel(int mux_chan);



void disable_fpga_mux_channel(int mux_chan);
void enable_fpga_mux_channel(int mux_chan);
void disable_all_mux_channel(void);
void enable_all_mux_channel(void);

void reset_all_mux_pll(void);

void set_mux_pll(int mux_chan);


void set_mux_external_clk(int mux_chan, int j83_annex, int qam_constel, int symbolrate);


void print_mux_channel(int mux_chan);



void init_mux_channels(void);

int add_packet_to_mux_si_zone(mux_si_zone_t *si_zone, mux_packet_t *p_packet, int si_id);

int write_packet_to_fpga_si_zone(int page, u_int *addr_base, mux_si_zone_t *p_si_zone);

void write_mapping_pid(int mux_chan, int in_chan, int in_pid, int mux_pid);
void clean_mapping_pid(int mux_chan, int in_chan, int in_pid);

void remove_all_mux_pid_filters(int mux_chan);
void remove_all_mux_program(int mux_chan);

int check_mux_status(int mux_chan);
int read_mux_validrate(int mux_chan);

void clean_mux_config(int mux_chan);
void clean_all_mux_data(void);

void init_mux_fpga(void);

void free_all_mux_program(mux_program_t *head_mux_program);
mux_program_t *create_mux_program(void);


pid_mapping_t *create_pid_mapping(void);

mux_program_t *add_program_mapping(int mux_chan, int in_chan, u_short old_pn, u_short new_pn);
int add_pid_mapping(int mux_chan, int in_chan, u_short in_pid, u_short mux_pid);
mux_section_t *add_mux_section(int mux_chan, u_short pid, u_short section_len, char *section_data);

void add_section_to_mux(int mux_chan, mux_section_t *p_mux_section);

void reset_mux_programs(int mux_chan);
void reset_mux_pid_mapping(int mux_chan);
void reset_mux_sections(int mux_chan);

void init_mux(int mux_channel_qty, int tsin_channel_qty, int crc_monitor, int mux_clk_type);

int remux(int mod_channel);


void *mux_task(void *args);

#endif

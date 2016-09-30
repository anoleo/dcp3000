
#ifndef ___PSI_SI_H___
#define ___PSI_SI_H___

#include <sys/types.h>


#define	ENCODING_


#define BISS_CA_SYSTEM_ID	0x2600

/* pid define */
#define PAT_PID		0x0000
#define CAT_PID		0x0001
#define NIT_PID		0x0010
#define SDT_PID		0x0011
#define BAT_PID		0x0011
#define EIT_PID		0x0012
#define RST_PID		0x0013
#define TDT_PID		0x0014
#define TOT_PID		0x0014

/* table ID define */
#define PAT_TABLEID		0x00
#define CAT_TABLEID		0x01
#define PMT_TABLEID		0x02
#define NIT_TABLEID		0x40
#define ONIT_TABLEID	0x41
#define SDT_TABLEID		0x42
#define OSDT_TABLEID	0x46
#define BAT_TABLEID		0x4a

#define EIT_TABLEID		0x4e

#define OEIT_TABLEID	0x4f

#define TDT_TABLEID		0x70
#define RST_TABLEID		0x71
#define ST_TABLEID		0x72
#define TOT_TABLEID		0x73 



#define EIT_pf_TABLEID		0x4e
#define EIT_s_TABLEID		0x50


#define EIT_s_0_3_TABLEID		0x50

#define EIT_s_4_7_TABLEID		0x51
#define EIT_s_56_59_TABLEID		0x5e

#define EIT_s_60_63_TABLEID		0x5f




#define NO_USE		0	 
#define PAT_USED	1
#define PMT_USED	2
#define CAT_USED	3
#define SDT_USED	4
#define NIT_USED	5
#define EIT_USED	6
#define DATA_USED	7

#define SDT_ACTUAL_USED	0x4
#define NIT_ACTUAL_USED	0x5
#define SDT_OTHER_USED	0x8
#define NIT_OTHER_USED	0x9
#define EIT_PF_ACTUAL_USED	0x0a
#define EIT_S_ACTUAL_USED	0x0b
#define EIT_PF_OTHER_USED	0x0c
#define EIT_S_OTHER_USED	0x0d



#define PAT_MASK	(1<<PAT_USED)
#define PMT_MASK	(1<<PMT_USED)
#define CAT_MASK	(1<<CAT_USED)
#define SDT_MASK	(1<<SDT_USED)
#define NIT_MASK	(1<<NIT_USED)
#define EIT_MASK	(1<<EIT_USED)
#define	ALL_SI_MASK 0x7e



#define SYNC_BYTE 				0x47
#define PACKAGE_MAX_LEN			188
#define MAX_SECTION_LEN			1024
#define EIT_SECTION_LEN			4096


#define MAX_PROGRAM_NAME_LEN 48

//#define MAX_PROGRAM_ONE_CHANNEL 64
//#define MAX_PROGRAM_ONE_CHANNEL_VOD 64

#define MAX_PID_COUNT_ONE_CHANNEL	75

#define MAX_SECTION_LENGTH	4096

#define DESCRAMBLE			0
#define SCRAMBLE 			1

#define NOPASS_FLAG		0x04
#define PMT_PACKET		0x07


#define  UNUSED_STREAM             0x00		
#define  Mpeg1_Video_STREAM        0x01		
#define  Mpeg2_Video_STREAM        0x02		
#define  Mpeg1_Audio_STREAM        0x03		
#define  Mpeg2_Audio_STREAM        0x04		
#define  Private_Data_STREAM       0x05		
#define  Private_Data_STREAM       0x06		
#define  MHEG_Data_STREAM          0x07		
#define  DSM_CC_STREAM             0x08		
#define  Auxiliary_Data_STREAM     0x09		
#define  DSM_CC_STREAM             0x0a		
#define  DSM_CC_STREAM             0x0b		
#define  DSM_CC_STREAM             0x0c		
#define  DSM_CC_STREAM             0x0d		
#define  Auxiliary_Data_STREAM     0x0e		
#define  Mpeg2_AAC_STREAM         0x0f		
#define  Mpeg4_Video_STREAM        0x10		
#define  Mpeg4_AAC_STREAM          0x11		
#define  MPEG4_FlexMux_STREAM     0x12		
#define  MPEG4_FlexMux_STREAM     0x13		
#define  DSM_CC_STREAM             0x14		
#define  Meta_Data_STREAM          0x15		
#define  Meta_Data_STREAM          0x16		
#define  DSM_CC_Meta_STREAM        0x17		
#define  DSM_CC_Meta_STREAM        0x18		
#define  Download_Meta_STREAM      0x19		
#define  IPMP_STREAM               0x1a		
#define  H264_Video_STREAM        0x1b		
#define  H265_Video_STREAM        0x24		
#define  AVS_PULS_STREAM               0x42		
#define  AVS_PULS_STREAM               0x43		
#define  DigiCipher_II_STREAM      0x80		
#define  Dolby_Audio_STREAM        0x81		
#define  DTS_6CH_Audio_STREAM      0x82		
#define  Dolby_TrueHD_STREAM       0x83		
#define  Dolby_D_PULS_STREAM           0x84		
#define  DTS_8CH_Audio_STREAM      0x85		
#define  DTS_8CH_Audio_STREAM      0x86		
#define  Dolby_D_PULS_STREAM           0x87		
#define  subtitle_STREAM           0x90		
#define  ATSC_DSM_CC_STREAM        0x95		
#define  DigiCipher_II_STREAM      0xc0		
#define  Dolby_D_SC_STREAM         0xc1		
#define  ATSC_DSM_CC_STREAM        0xc2		
#define  AAC_SC_STREAM             0xcf		
#define  Ultra_HD_video_STREAM     0xd1		
#define  H264_SC_STREAM           0xdb		
#define  WMV_STREAM                0xea		
                               
#define  PCR_STREAM                0xfa		
#define  PMT_STREAM                0xfb		
#define  GHOST_STREAM         0xfc		
#define  EMM_STREAM           0xfd		
#define  ECM_STREAM           0xfe		
  


typedef struct _packet_t packet_t;
struct _packet_t{
	unsigned short	len;
	unsigned char	data[PACKAGE_MAX_LEN];
	packet_t *next;
};


typedef struct _mux_packet_t mux_packet_t;
struct _mux_packet_t {
	u_char	len;
	u_char	si_id;
	u_short index;	//from 1 to N
 	u_char	buf[PACKAGE_MAX_LEN]; 
	mux_packet_t	*next;
}; 


typedef struct _section_t section_t;
struct _section_t{
	u_char	version_number;
	u_char	section_number;
	u_short	len;
	u_short	now_len;
	u_int	crc32;
	section_t *next;
	u_char	data[MAX_SECTION_LEN];
};

typedef struct _eit_section_t eit_section_t;
struct _eit_section_t{
	u_char	version_number;
	u_char	section_number;
	u_short	len;
	u_short	now_len;
	u_int	crc32;
	eit_section_t *next;
	u_char	data[EIT_SECTION_LEN];
};


typedef struct _mux_section_t mux_section_t;
struct _mux_section_t {
	u_short	pid;
	u_short	len;
 	u_char	data[MAX_SECTION_LEN];	
	mux_section_t	*next;
};


typedef struct {
	u_char	table_id;
	u_char	version_number;
	u_char	section_number;
	u_char	last_section_number;
	u_short	sub_table_id;
	u_short	section_len;
	u_char	*p_buf;
} section_head_t;

struct pmt_ecm_st{
	u_short ca_system_id;
	u_short ecm_pid;
};



#define SECTION_NUM_MASK_SIZE	32

typedef struct _ts_sub_table_t ts_sub_table_t;
struct _ts_sub_table_t {
	u_char	table_id;
	u_char	isComplete;
	u_short	sub_table_id;
	u_char	section_count;
	u_char	now_section_number;
	u_short	last_section_number;
	u_char	section_num_mask[SECTION_NUM_MASK_SIZE];
	section_t *section;
	ts_sub_table_t *next;
};


typedef struct _eit_segment_st eit_segment_t;
struct _eit_segment_st{
	u_char	section_count;
	u_char	segment_num;
	u_char	segment_last_section_number;
	eit_section_t *section;
	eit_segment_t *next;
};

typedef struct _eit_sub_table_st eit_sub_table_t;
struct _eit_sub_table_st {
	u_char	table_id;
	u_char	isComplete;
	u_char	segment_count;
	u_char	now_section_number;
	u_short	last_section_number;
	u_char	section_num_mask[SECTION_NUM_MASK_SIZE];
	eit_segment_t *segments;
	eit_segment_t *cur_segment;
	eit_sub_table_t *next;
};



typedef struct _eit_table_st eit_table_t;
struct _eit_table_st {
	u_char	isComplete;
	u_char	sub_table_count;
	u_char	last_table_id;
	u_short	service_id;
	eit_sub_table_t *sub_tables;
	struct _eit_table_st *next;
};


typedef struct {
	u_char	packet_cc;
	ts_sub_table_t *cur_sub_table;
	ts_sub_table_t *sub_tables;
}ts_table_t;

typedef struct {
	u_int service_type;
	u_char service_name[MAX_PROGRAM_NAME_LEN];
	u_char provider_name[MAX_PROGRAM_NAME_LEN];
}service_info_t;


#define ES_PID_QTY_IN_A_PROGRAM		10

#define ECM_PID_QTY_IN_A_PROGRAM		10
#define EMM_PID_QTY_IN_A_TS		10


typedef struct _program_t{
	u_char	isComplete;
	u_short pmt_pid;
	u_short program_number;
	u_short pcr_pid;
	
	section_t *pmt_section;
	eit_table_t *eit_tables;

	
	u_int es_pid[ES_PID_QTY_IN_A_PROGRAM];
	u_short ecm_pid[ECM_PID_QTY_IN_A_PROGRAM];

	
	//*
	u_char EIT_s_flag;
	u_char EIT_pf_flag;
	u_char running_status;
	u_char free_CA_mode;
	//*/

	service_info_t service_info;
	
	
	struct _program_t *next;
}program_t;

typedef struct _pmt_table_s{
	u_char	packet_cc;
	u_char	isComplete;
	u_short pmt_pid;
	program_t *p_program;
	program_t *cur_program;
	struct _pmt_table_s *next;
}pmt_table_t;


typedef struct _mux_pmt_section_s{
	u_short	program_number;
	u_short	pmt_pid;
	mux_section_t section;
	struct _mux_pmt_section_s *next;
}mux_pmt_section_t;

typedef struct _ts_in_table_t {
	
	u_char	table_valid;
	u_char	hasBissCA;
	u_char	hasSimulCA;

	u_int	capture_error_count;
	
	u_char	capture_mask;
	u_char	complete_mask;
	
	u_short ts_id;
	u_short network_id;
	u_short original_network_id;
	
	/* PAT table */
	u_char	pat_count;
	u_char	pat_cc;
	u_char	pat_now_section_number;
	u_char	pat_last_section_number;
	u_int	pat_section_num_mask;
	section_t *pat_section;
	section_t *cur_pat_section;

	/*PMT Table */
	u_char	program_count;
	pmt_table_t *pmt_tables;

	/* SDT Table */
	u_char	sdt_count;
	u_char	sdt_cc;
	u_char	sdt_now_section_number;
	u_char	sdt_last_section_number;
	u_int	sdt_section_num_mask;
	section_t *sdt_section;
	section_t *cur_sdt_section;

	/*CAT Table */
	u_char	cat_count;
	u_char	cat_cc;
	u_char	cat_now_section_number;
	u_char	cat_last_section_number;
	u_int	cat_section_num_mask;
	section_t *cat_section;
	section_t *cur_cat_section;

	/*NIT table */
	u_char	nit_count;
	u_char	nit_cc;
	u_char	nit_now_section_number;
	u_char	nit_last_section_number;
	u_int	nit_section_num_mask;
	section_t *nit_section;
	section_t *cur_nit_section;

	/*EIT table */
	//u_char	eit_cond_enable;
	u_char	eit_cc;
	
	eit_sub_table_t *cur_eit_sub_table;
	//eit_qualifier_t *eit_capture_cond;

	u_short emm_pid[EMM_PID_QTY_IN_A_TS];
	//u_short pid_point;
	//u_short in_pids[120];
}ts_in_table_t;


#endif

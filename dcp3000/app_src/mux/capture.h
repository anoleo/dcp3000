#ifndef ___CAPTURE_H___
#define ___CAPTURE_H___

#include <sys/types.h>
#include "mux_fpga.h"
#include "psi_si.h"


#define ALL_TSIN_CHANNEL_QTY	256


#define PIDLOOKUP_BYTES_PERPID		SZ_128
#define PIDLOOKUP_BYTES_IN_CHAN		SZ_1M

#if DDR_PAGING_ACCESS
#define DDR_Page_PidLookup(in_chan)		((DDR_PREMUX_LOOKUP_BASE + (in_chan)*PIDLOOKUP_BYTES_IN_CHAN) / FPGA_PAGE_SIZE)
#define DDR_Offset_PidLookup(in_chan, pid)		((DDR_PREMUX_LOOKUP_BASE + (in_chan)*PIDLOOKUP_BYTES_IN_CHAN + \
														(pid)*PIDLOOKUP_BYTES_PERPID) % FPGA_PAGE_SIZE)
#else
#define DDR_Page_PidLookup(in_chan)		0
#define DDR_Offset_PidLookup(in_chan, pid)		((DDR_PREMUX_LOOKUP_BASE + (in_chan)*PIDLOOKUP_BYTES_IN_CHAN + (pid)*PIDLOOKUP_BYTES_PERPID) )
#endif



#define PID_MUX_OUT_SW_BIT_S		0
#define PID_MUX_OUT_SW_BIT_E		71
#define PID_CAPTURE_SW_BIT		72
#define PID_MAPPING_SW_BIT		73
#define PID_MUX_MAPPING_BIT_S		80
#define PID_MUX_MAPPING_BIT_E		1015


#define CAPTURE_BUF_BYTES_IN_CHAN	0x10000

#if DDR_PAGING_ACCESS
#define DDR_Page_CaptureBuffer(in_chan)			((DDR_CAPTURE_BASE + (in_chan)*CAPTURE_BUF_BYTES_IN_CHAN) / FPGA_PAGE_SIZE)
#define DDR_Offset_CaptureBuffer(in_chan)		((DDR_CAPTURE_BASE + (in_chan)*CAPTURE_BUF_BYTES_IN_CHAN) % FPGA_PAGE_SIZE)
#else
#define DDR_Page_CaptureBuffer(in_chan)			0
#define DDR_Offset_CaptureBuffer(in_chan)		(DDR_CAPTURE_BASE + (in_chan)*CAPTURE_BUF_BYTES_IN_CHAN)
#endif


//#define TSIN_CAPTURE_IRQ_ENABlE_REG	(TSIN_1_BASE_REG + (0x03*(DDR_BIT_WIDTH/8)))
//#define TSIN_CAPTURE_VALID_BASE_REG	(PREMUX_BASE_REG + (0xf10*(DDR_BIT_WIDTH/8)))


#define	PREMUX_TSIN_CHANNEL_REG		(PREMUX_BASE_REG + (0xfff*(MUXREG_BIT_WIDTH/8)))
#define	PREMUX_TSIN_ENABLE_REG		(PREMUX_BASE_REG + (0xf00*(MUXREG_BIT_WIDTH/8)))
#define	PREMUX_BUF_DECREASE_REG		(PREMUX_BASE_REG + (0xf01*(MUXREG_BIT_WIDTH/8)))
#define	PREMUX_BUF_PACKETS_REG		(PREMUX_BASE_REG + (0xf01*(MUXREG_BIT_WIDTH/8)))
#define	PREMUX_BUF_CLEAR_REG			(PREMUX_BASE_REG + (0xf02*(MUXREG_BIT_WIDTH/8)))
#define	PREMUX_IRQ_CAPTURE_ENABLE_REG	(PREMUX_BASE_REG + (0xf05*(MUXREG_BIT_WIDTH/8)))


#define PREMUX_CHANNEL_ENABLE_BIT	0
#define PREMUX_CHANNEL_CAPTURE_ENABLE_BIT	1
#define PREMUX_PID_CC_CHECK_ENABLE_BIT	2
#define PREMUX_PID_CC_COUNTER_ENABLE_BIT	3


#define CAPTURE_VALID_NUM_IN_ONE_REG		32


#define CAPTURE_ERROR_LIMIT		2100


#define	TSIN_UNLOCK_RESET		1
#define	TSIN_AUTO_CAPTURE		1
#define	TSIN_CAPTURE_WAIT_TIMES		2500

#define TS_UNLOCK	0
#define TS_NOT_READY	1
#define TS_LOCKED	2
#define TS_CHANGED	3



#define TSIN_LOCK			(1 << 0)
#define TSIN_PSI_READY		(1 << 1)
#define TSIN_PSI_CHANGE		(1 << 2)
#define TSIN_SDT_READY		(1 << 3)
#define TSIN_SDT_CHANGE		(1 << 4)
#define TSIN_NIT_READY		(1 << 5)
#define TSIN_NIT_CHANGE		(1 << 6)
#define TSIN_EIT_READY		(1 << 7)
#define TSIN_EIT_CHANGE		(1 << 8)




#define CAPTURE_NULL	0
//#define TS_NOT_CHANGE	(2 << 4)
#define CAPTURE_OK		(5 << 4)
#define CAPTURE_ERROR	(9 << 4)


#define CAPTURE_PEND	0
#define CAPTURE_BEGIN	1
#define CAPTURE_REDO	2
#define CAPTURE_READY	3
#define CAPTURE_END		9

#define CAPTURE_FLOW_FLAG	1
#define CAPTURE_STATU_FLAG	2
#define CAPTURE_ALL_FLAG	9




typedef struct{
	char auto_capture;
	char unlock_reset;
	short reserve2;
}tsin_capture_control_t;

extern tsin_capture_control_t capture_control;



typedef struct{

	u_short		channel;
	u_short		ts_flag;//bit0 lock,1 psi  ready, 2 psi change, 3 sdt ready, 4 sdt sdt

	u_short		mux_si_update_flag;

	u_char		dowith_eit_pf;
	u_char		dowith_eit_s;
	
	u_char		dowith_nit;
	
	u_char		si_change_flag;
	u_char		capture_flag;
	u_char		capture_buf_lock;
	
	int		ts_rw_lock;
	u_int		capture_buf_offset;
	u_int		ts_bitrate; //kbps
	u_int		pat_crc;
	ts_in_table_t ts_tables;
	u_int lock_time;

	u_char	original_encoding;	//the original encoding of the service name 
	u_char	target_encoding;	//the target encoding of the service name

}ts_in_channel_t;

extern ts_in_channel_t ts_in_channels[ALL_TSIN_CHANNEL_QTY];


int read_tsin_bitrate(int in_chan);

void init_tsin_all_channel(void);

void free_ts_in_channel(int in_chan);


int check_capture_flag(int in_chan, u_char flag_type, u_char flag);
int check_capture_task_is_ok(void);

#define check_capture_isEnd(in_chan)	 check_capture_flag(in_chan, CAPTURE_FLOW_FLAG, CAPTURE_END)
#define check_capture_isOk(in_chan)	check_capture_flag(in_chan, CAPTURE_STATU_FLAG, CAPTURE_OK)


void init_pid_lookup(int in_chan);


void lock_ts_in_channel(int in_chan, u_char rwflag);
void unlock_ts_in_channel(int in_chan, u_char rwflag);

void enable_tsin_channel(int in_chan);
void disable_tsin_channel(int in_chan);


int  clear_capture_buffer(int in_chan);

void add_captureTs_task(int in_chan);


void *capture_task(void *args);
void re_captureTs(int in_chan);

#endif


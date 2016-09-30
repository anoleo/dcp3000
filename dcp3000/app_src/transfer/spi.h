#ifndef	__SPI_H__
#define	__SPI_H__

#include <sys/types.h>



#define TRANSFER_WAIT	0
#define TRANSFER_START	1
#define TRANSFER_BUSY	2
#define TRANSFER_END	3


#define TRANSFER_OK		1
#define TRANSFER_ERR		2
#define TRANSFER_TIMEOUT	3

#define SPI_READ_MODE		(0 << 1)
#define SPI_WRITE_MODE		(1 << 1)


#define SPI_BLOCK_READ		(SPI_READ_MODE | 0)
#define SPI_BLOCK_WRITE		(SPI_WRITE_MODE | 0)

#define SPI_PROCEDURAL_READ	(SPI_READ_MODE | 1)
#define SPI_PROCEDURAL_WRITE	(SPI_WRITE_MODE | 1)


#define SLAVER_SPI_TRANS_FLAG_ADDR	0X1000
#define SLAVER_SPI_SYSTIME_ADDR	0X1004
#define SLAVER_SPI_MESSAGE_ADDR	0X4000


#define SLAVER_SPI_BOARDINFO_ADDR	0X5000
#define SLAVER_SPI_BOARD_SUMMARY_ADDR	0X5004
#define SLAVER_SPI_BOARD_SLOT_ADDR 0X5008
#define SLAVER_SPI_BOARD_PROPERTIES_FILE_ADDR 0X500c

#define SLAVER_SPI_WEBFILE_ADDR	0X5100

#define SLAVER_SPI_UPGRADE_ADDR	0X6000
#define SLAVER_SPI_SEND_FILE_ADDR	0X6004
#define SLAVER_SPI_GET_IRQ_EVNET_ADDR	0X6008
#define SLAVER_SPI_UPGRADE_STATUS_ADDR	0X6100
#define SLAVER_SPI_UPGRADE_INPUT_ADDR	0X6104
#define SLAVER_SPI_GET_TEMPRATURE_ADDR	0X6200		// 16bit  bit[15:8] ratio, bit[7:0] temprature

#define SPI_MSG_GET_FILE	0x7fe00001
#define SPI_MSG_EXEC_CMD	0x7fe00002

typedef struct {
	int run_flag;
	//int status;
	int lock;
}spi_master_st;

extern spi_master_st *spi_masters;

struct _spi_transfer_task{
	u_int addr;
	int len;
	int transfer_len;
	void *data;
	char transfer_mode;
	char transfer_flag;
	char transfer_status;
	char slot;
};

typedef struct _spi_transfer_task spitask_st;

int spi_master_transfer(spitask_st *spitask);

void init_spi_transfer(int slot);
void start_spi_transfer_task(int slot);
void end_spi_transfer_task(int slot);
void pause_spi_transfer_task(int slot);
void resume_spi_transfer_task(int slot);

int	get_spi_runflag(int slot);
void set_spi_runflag(int slot, int runflag);
int is_spi_running(int slot);

spitask_st *add_spi_transfer_task(int slot, u_int addr, void *data, int datalen, int transfer_mode);
spitask_st *insert_spi_transfer_task(int slot, u_int addr, void *data, int datalen, int transfer_mode);
int wait_spi_transfer_end(spitask_st *spitask);
int check_spi_transfer_status(spitask_st *spitask);
void print_spi_status(int slot);

spitask_st *spi_transfer_msg(int slot, int msgid, void *data, int datalen, int stransfer_mode);

#define spi_write_msg(slot, msgid, pdata, datalen)	spi_transfer_msg(slot, msgid, pdata, datalen, SPI_PROCEDURAL_WRITE)
#define spi_read_msg(slot, msgid, pdata, datalen)	spi_transfer_msg(slot, msgid, pdata, datalen, SPI_PROCEDURAL_READ)

//spitask_st * spi_write_msg(int slot, int msgid, void *data, int datalen);
//spitask_st * spi_read_msg(int slot, int msgid, void *data, int datalen);

int spi_read_data(int slot, u_int addr, void **ppdata);
int spi_write_data(int slot, u_int addr, void *data, int len);

//int spi_read_word(int slot, u_int addr);
int spi_write_word(int slot, u_int addr, u_short data);

int spi_read_block(int slot, u_int addr, void *data, int len);
int spi_write_block(int slot, u_int addr, void *data, int len);

int spi_send_json(int slot, int msgid, char *json_txt);
int spi_rcv_json(int slot, int msgid, char *json_txt, char **pp_rcvdata);

int spi_send_msg(int slot, int msgid, void *data, int len);
int spi_rcv_msg(int slot, int msgid, void *data, int len, char **pp_rcvdata);

int spi_send_file(int slot, char *filename);
int spi_get_file(int slot, char *filename);

int spi_send_cmd(int slot, char *cmd, char **result_data);

int debug_spi(int argc, char *argv[]);

#endif

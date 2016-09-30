
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "../system.h"
#include "../utils/queue.h"
#include "../utils/utils.h"

#include "spi.h"

#include "../basedef.h"

#include "../shmmsg_client.h"
//#include "../ipcmsg/shmem_msg.h"

spi_master_st *spi_masters;


#define SPI_MAX_DATASIZE		(0X800000)

#define SPI_BUFFER_SIZE		(2000-8) // ((SPI_XYF_BUF_SIZE<<1) - 8)

#ifdef SPI_MAIN_PROCESS

#include "spi_xyf.h"

typedef struct {
	int run_flag;
	//int status;
	list_st task_queue;
	pthread_mutex_t lock;
}spi_master_mod_st;


static spi_master_mod_st spi_master_module[MAX_SLOT_QTY];

#if 1

#define spi_transfer_mutex_init(spi_master)	pthread_mutex_init(&((spi_master)->lock), NULL);
#define spi_transfer_lock(spi_master)		pthread_mutex_lock(&((spi_master)->lock))
#define spi_transfer_unlock(spi_master)		pthread_mutex_unlock(&((spi_master)->lock))
#else
#define spi_transfer_mutex_init(spi_master)	(spi_master)->lock = 0
#define spi_transfer_lock(spi_master)	\
	do{	usleep(100); }while((spi_master)->lock);	\
	(spi_master)->lock = 1
	
#define spi_transfer_unlock(spi_master)		(spi_master)->lock = 0
#endif



#define MASTER_SPI_READ_LEN_FIXED	(1 << 9)


#define MASTER_SPI_START_READ	(1 << 10)
#define MASTER_SPI_END_READ	(2 << 10)

#define MASTER_SPI_START_WRITE	(1 << 12)
#define MASTER_SPI_END_WRITE	(2 << 12)

#define MASTER_SPI_START_TRANS	(1 << 14)
#define MASTER_SPI_END_TRANS	(2 << 14)


static int spi_procedural_transfer_data(spitask_st *spitask)
{
	u_int addr, v_u32;
	int status, wrlen = 0, rdlen = 0, len = 0;
	int s_m_flag, r_w_flag;
	char *wr_data = NULL, *rd_data = NULL, *pdata;
	int rddata_isalloc = 0;
	u_int reg_base;
	int ret = 0;
	int msgwd_flag;

	DebugPrintf("=================spi_procedural_transfer_data start=====================\n");

	spitask->transfer_flag = TRANSFER_START;
	
	addr = spitask->addr;
	len = spitask->len;
	pdata = spitask->data;
	r_w_flag = spitask->transfer_mode & 2;

	msgwd_flag = spitask->transfer_mode & 0x10;  // 写入的消息数据是否是动态分配的，0 为是
	
	if(spitask->slot >= 7){
		reg_base = REG_FPGA_SPI_MASTER_BASE+((0x10 * 0x10)<<2);
	}else{
		reg_base = REG_FPGA_SPI_MASTER_BASE+((spitask->slot * 0x10)<<2);
	}


	if((r_w_flag == SPI_WRITE_MODE) || (addr == SLAVER_SPI_MESSAGE_ADDR)){
		
		v_u32 = (MASTER_SPI_START_TRANS | MASTER_SPI_START_WRITE);

		status = spi_master_write(reg_base, SLAVER_SPI_TRANS_FLAG_ADDR, &v_u32, 4);

		if(status < 0){
			spitask->transfer_status = TRANSFER_ERR;
			if(addr == SLAVER_SPI_MESSAGE_ADDR){
				if(msgwd_flag==0){
					free(pdata);
				}
			}
			return -1;
		}
		
		wr_data = pdata;
		wrlen = len;	

		status = spi_master_write(reg_base, addr, &wrlen, 4);
		if(status < 0){
			spitask->transfer_status = TRANSFER_ERR;
			if(addr == SLAVER_SPI_MESSAGE_ADDR){
				if(msgwd_flag==0){
					free(pdata);
				}
			}
			return -1;
		}

		while(wrlen>0){
			if(wrlen > SPI_BUFFER_SIZE){
				status = spi_master_write(reg_base, addr, wr_data, SPI_BUFFER_SIZE);
				wrlen -= SPI_BUFFER_SIZE;
				wr_data += SPI_BUFFER_SIZE;
			}else{
				status = spi_master_write(reg_base, addr, wr_data, wrlen);
				wrlen = 0;
			}

			spitask->transfer_len += wrlen;
			
			if(status < 0){
				spitask->transfer_status = TRANSFER_ERR;
				
				if(addr == SLAVER_SPI_MESSAGE_ADDR){
					if(msgwd_flag==0){
						free(pdata);
					}
				}
				return -1;
			}
		}
		if(addr == SLAVER_SPI_MESSAGE_ADDR){
			if(msgwd_flag==0){
				free(pdata);
			}
			pdata = NULL;
		}
	}
	

	if(r_w_flag == SPI_WRITE_MODE){
		v_u32 = (MASTER_SPI_END_TRANS | MASTER_SPI_END_WRITE);
	}else if(addr == SLAVER_SPI_MESSAGE_ADDR){
		v_u32 = (MASTER_SPI_END_WRITE | MASTER_SPI_START_READ);
	}else{
		rdlen = len;
		rd_data = pdata;
		if(rdlen > 0){
			v_u32 = (MASTER_SPI_START_TRANS | MASTER_SPI_START_READ | MASTER_SPI_READ_LEN_FIXED);
		}else{
			v_u32 = (MASTER_SPI_START_TRANS | MASTER_SPI_START_READ);
		}
	}

	status = spi_master_write(reg_base, SLAVER_SPI_TRANS_FLAG_ADDR, &v_u32, 4);

	if(status < 0){
		spitask->transfer_status = TRANSFER_ERR;
		return -1;
	}
	
	if(r_w_flag == SPI_WRITE_MODE){
		spitask->transfer_status = TRANSFER_OK;
		return 0;
	}

	if(rdlen > 0){
		status = spi_master_write(reg_base, addr, &rdlen, 4);
	}else{
		status = spi_master_read(reg_base, addr, &rdlen, 4);
	}


	if(status < 0){
		spitask->transfer_status = TRANSFER_ERR;
		ret = -1;
		goto End_spi_tans;
	}	

	if(rdlen > SPI_MAX_DATASIZE){
		spitask->transfer_status = TRANSFER_ERR;
		printf("spi master read data , len %d over than the max size %d \n", rdlen, SPI_MAX_DATASIZE);
		ret = -1;
		goto End_spi_tans;
	}

	len = rdlen;

	DebugPrintf("spi transfer sequence data , read data, len = %d \n", len);

	if(rd_data == NULL){
		rd_data = malloc(rdlen+1);
		if(rd_data == NULL){
			perror("spi rd_data");
			exit(-1);
		}
		pdata = rd_data;
		rddata_isalloc = 1;
	}


	while(rdlen>0){
		if(rdlen > SPI_BUFFER_SIZE){
			status = spi_master_read(reg_base, addr, rd_data, SPI_BUFFER_SIZE);
			rdlen -= SPI_BUFFER_SIZE;
			rd_data += SPI_BUFFER_SIZE;
		}else{
			status = spi_master_read(reg_base, addr, rd_data, rdlen);
			rdlen = 0;
		}

		spitask->transfer_len += rdlen;
		
		if(status < 0){
			spitask->transfer_status = TRANSFER_ERR;
			if(rddata_isalloc){
				free(pdata);
				pdata = NULL;
				rddata_isalloc = 0;
			}
			ret = -1;
			goto End_spi_tans;
		}
	}

	End_spi_tans:

	v_u32 = (MASTER_SPI_END_TRANS | MASTER_SPI_END_READ);
	status = spi_master_write(reg_base, SLAVER_SPI_TRANS_FLAG_ADDR, &v_u32, 4);

	if(status < 0){
		spitask->transfer_status = TRANSFER_ERR;
		if(rddata_isalloc){
			free(pdata);
		}
		return -1;
	}

	if(ret==0){
		spitask->transfer_status = TRANSFER_OK;
	
		if(pdata){
			pdata[len] = 0;
		}
		spitask->len = len;
		spitask->data = pdata;
	}

	InfoPrintf("=================spi_procedural_transfer_data end=====================\n");
	
	return ret;
}


int spi_master_transfer(spitask_st *spitask)
{
	int slot;
	int status;
	int reg_base;
	//struct timeval btime;
	spi_master_mod_st *spi_mod;
	//char buf[40];
	int try_times = 1;

	if(spitask == NULL){
		return -1;
	}

	slot = spitask->slot;
	
	if(slot >= 7){
		reg_base = REG_FPGA_SPI_MASTER_BASE+((0x10 * 0x10)<<2);
	}else{
		reg_base = REG_FPGA_SPI_MASTER_BASE+((slot * 0x10)<<2);
	}
	
	spi_mod = &spi_master_module[slot];

	InfoPrintf("=================%d spi master transfer  wait=====================\n", slot);

	spi_transfer_lock(spi_mod);
	
	
	spitask->transfer_flag = TRANSFER_BUSY;

	InfoPrintf("=================%d spi master transfer  begin=====================\n", slot);

	//gettimeofday(&btime, NULL);

	Start_Transfer:

		if(spitask->transfer_mode&1){
			status = spi_procedural_transfer_data(spitask);
		}else{

			spitask->addr |= 1<<31;	//地址最高位为1 时直接读写数据
			
			if(spitask->transfer_mode & 2){
				status = spi_master_write(reg_base, spitask->addr, spitask->data, spitask->len);
				InfoPrintf("%d slot write  block : status = %d \n", slot, status);
			}else{
				status = spi_master_read(reg_base, spitask->addr, spitask->data, spitask->len);
				InfoPrintf("%d slot read  block : status = %d \n", slot, status);
			}

			
			
			if(status < 0){
				spitask->transfer_status = TRANSFER_ERR;
			}else{
				spitask->transfer_status = TRANSFER_OK;
				status = 0;
			}
		}

		if(status<0){
			spitask->data = NULL;
			printf("slot %d spi master transfer failed \n", slot);
			try_times--;
			//goto Start_Transfer;
		}else{
			//break;
			//sprintf(buf, "slot %d spi master transfer", spitask->slot);
			//show_exectime(&btime, buf);
		}

	End_transfer:
	spitask->transfer_flag = TRANSFER_END;
	spi_transfer_unlock(spi_mod);
	InfoPrintf("=================%d spi master transfer  end=====================\n", slot);
	
	return status;
}

static void spi_master_transfer_task(int slot)
{
	spi_master_st *spi_master;
	spi_master_mod_st *spi_mod;
	list_st *spi_queue;
	spitask_st *p_spitask;


	InfoPrintf("%d slot starting ... \n", slot);
	
	spi_master = &spi_masters[slot];
	spi_mod =  &spi_master_module[slot];
	spi_queue = &spi_mod->task_queue;

	spi_mod->run_flag = 1;

	while(spi_mod->run_flag){

		if(spi_queue->count > 0){
			
			InfoPrintf("spi_queue->count = %d \n", spi_queue->count);
			
			if(spi_master->run_flag == RUNNING_FLAG){
				p_spitask = (spitask_st *)list_dequeue(spi_queue);
				spi_master_transfer(p_spitask);
			}else if(spi_master->run_flag == PAUSE_FLAG){
				list_empty(spi_queue);
			}
			
		}else{
			sched_yield();
		}
		
		usleep(1000);
	}

	list_empty(spi_queue);
	pthread_rwlock_destroy(&(spi_queue->rwlock));	
	pthread_mutex_destroy(&(spi_mod->lock));



}

void init_spi_transfer(int slot)
{
	spi_master_mod_st *spi_mod;
	list_st *spi_queue;

	printf("..........init %d slot spi transfer module ................\n", slot);
	
	spi_mod = &spi_master_module[slot];
	spi_queue = &spi_mod->task_queue;

	init_list(spi_queue, STRUCT_FLAG);

	spi_transfer_mutex_init(spi_mod)

	spi_mod->run_flag = 0;	
	//spi_master->run_flag = RUNNING_FLAG;
	create_detached_pthread( (void *(*)(void *))spi_master_transfer_task, (void *)slot, SZ_2M);
	
}


void start_spi_transfer_task(int slot)
{
	while(spi_master_module[slot].run_flag == 0){
		usleep(1000);
	}
	printf("..........start %d slot spi transfer task ................\n", slot);
	spi_masters[slot].run_flag = RUNNING_FLAG;
}

void end_spi_transfer_task(int slot)
{
	spi_masters[slot].run_flag = STOP_FLAG;
	spi_master_module[slot].run_flag = STOP_FLAG;
	printf("..........end %d slot spi transfer task ................\n", slot);
}

void print_spi_status(int slot)
{
	spi_master_mod_st *spi_mod = &spi_master_module[slot];
	printf(" %d slot runflag = %d, taskqueue  %d \n", slot, spi_masters[slot].run_flag, spi_mod->task_queue.count);
}

#endif

void pause_spi_transfer_task(int slot)
{
	spi_masters[slot].run_flag = PAUSE_FLAG;
	printf("..........pause %d slot spi transfer task ................\n", slot);
}

void resume_spi_transfer_task(int slot)
{
	spi_masters[slot].run_flag = RUNNING_FLAG;
	printf("..........resume %d slot spi transfer task ................\n", slot);
}

void set_spi_runflag(int slot, int runflag)
{
	spi_masters[slot].run_flag = runflag;
}

int get_spi_runflag(int slot)
{
	return  spi_masters[slot].run_flag;
}

int is_spi_running(int slot)
{
	return (spi_masters[slot].run_flag == RUNNING_FLAG);
}


#ifdef IPC_CLIENT
#define MSG_SPI_TRANSFER		0x09
spitask_st *add_spi_transfer_task(int slot, u_int addr, void *data, int datalen, int transfer_mode)
{
	int ret;
	char *databuf;
	spitask_st *p_spitask;

	if(spi_masters[slot].run_flag != RUNNING_FLAG){
		printf("%d slot spi master transfer task is not running\n", slot);
		return NULL;
	}
	
	InfoPrintf("%d slot, addr %#x, %d len, mode : %#x \n", slot, addr, datalen, transfer_mode);

	if(datalen>0){
		databuf = malloc(datalen+4*4);
	}else{
		databuf = malloc(4*4);
	}
	if(databuf == NULL){
		perror("msgbuf");
		exit(-1);
	}

	memcpy(databuf, &slot, 4);
	memcpy(&databuf[4], &addr, 4);
	memcpy(&databuf[8], &transfer_mode, 4);
	memcpy(&databuf[12], &datalen, 4);
	if(datalen>0){
		if(transfer_mode&0xffff){
			memcpy(&databuf[16], data, datalen);
		}
	}else{
		datalen = 0;
	}
	
	ret = send_shmmsg(MSG_SPI_TRANSFER, databuf, (datalen+4*4));
	free(databuf);

	if(ret<0){
		return NULL;
	}
	
	p_spitask = malloc(sizeof(spitask_st));
	if(p_spitask == NULL){
		perror("spitask_st");
		exit(-1);
	}
	
	p_spitask->slot = slot;
	p_spitask->addr = addr;
	p_spitask->len = datalen;
	p_spitask->data = data;
	p_spitask->transfer_mode = transfer_mode;
	p_spitask->transfer_flag = TRANSFER_WAIT;
	p_spitask->transfer_status = 0;
	p_spitask->transfer_len = 0;

	return p_spitask;
}

spitask_st *insert_spi_transfer_task(int slot, u_int addr, void *data, int datalen, int transfer_mode)
{
	int ret;
	char *databuf;
	spitask_st *p_spitask;

	if(spi_masters[slot].run_flag != RUNNING_FLAG){
		printf("%d slot spi master transfer task is not running\n", slot);
		return NULL;
	}
	
	InfoPrintf("%d slot, addr %#x, %d len, mode : %#x \n", slot, addr, datalen, transfer_mode);

	if(datalen>0){
		databuf = malloc(datalen+4*4);
	}else{
		databuf = malloc(4*4);
	}
	if(databuf == NULL){
		perror("msgbuf");
		exit(-1);
	}
	
	transfer_mode |= 0x10000;

	memcpy(databuf, &slot, 4);
	memcpy(&databuf[4], &addr, 4);
	memcpy(&databuf[8], &transfer_mode, 4);
	memcpy(&databuf[12], &datalen, 4);
	if(datalen>0){
		if(transfer_mode&3){
			memcpy(&databuf[16], data, datalen);
		}
	}else{
		datalen = 0;
	}
	
	ret = send_shmmsg(MSG_SPI_TRANSFER, databuf, (datalen+4*4));
	free(databuf);

	if(ret<0){
		return NULL;
	}
	
	p_spitask = malloc(sizeof(spitask_st));
	if(p_spitask == NULL){
		perror("spitask_st");
		exit(-1);
	}
	
	p_spitask->slot = slot;
	p_spitask->addr = addr;
	p_spitask->len = datalen;
	p_spitask->data = data;
	p_spitask->transfer_mode = transfer_mode;
	p_spitask->transfer_flag = TRANSFER_WAIT;
	p_spitask->transfer_status = 0;
	p_spitask->transfer_len = 0;

	return p_spitask;
}

int wait_spi_transfer_end(spitask_st *spitask)
{
	int retval;

	if((spitask->transfer_mode&3)){
		spitask->len = rcv_shmmsg(NULL);
		spitask->data = get_shmmsg_rcvdata();
	}else{
		spitask->len = rcv_shmmsg(spitask->data);
	}

	if(spitask->len > 0){
		if(spitask->transfer_mode&2){
			memcpy(&retval, spitask->data, 4);
			free(spitask->data);
			if(retval){
				return 0;
			}else{
				return -1;
			}
		}else{
			return 0;
		}
	}else{
		return -1;
	}
	
}

//返回值-1 传输错误，正常为传输的进度是一个精度为0.01 的完成比例的10000 倍，0 表示没开始传输，
int check_spi_transfer_status(spitask_st *spitask)
{
	if((spitask->transfer_len== TRANSFER_END)){
		if(spitask->transfer_status == TRANSFER_OK){
			return 10000;
		}else{
			return -1;
		}
	}else{
		if(spitask->len>0){
			return (spitask->transfer_len * 10000 / spitask->len);
		}else{
			return 0;	
		}
	}
}
#else
spitask_st *add_spi_transfer_task(int slot, u_int addr, void *data, int datalen, int transfer_mode)
{
	list_st *spi_queue;
	
	spitask_st *p_spitask;

	if(spi_masters[slot].run_flag != RUNNING_FLAG){
		printf("%d slot spi master transfer task is not running\n", slot);
		return NULL;
	}
	
	InfoPrintf("%d slot, addr %#x, %d len, mode : %#x \n", slot, addr, datalen, transfer_mode);
	spi_queue = &spi_master_module[slot].task_queue;

	p_spitask = malloc(sizeof(spitask_st));
	if(p_spitask == NULL){
		perror("spitask_st");
		exit(-1);
	}

	
	p_spitask->slot = slot;
	p_spitask->addr = addr;
	p_spitask->len = datalen;
	p_spitask->data = data;
	p_spitask->transfer_mode = transfer_mode;
	p_spitask->transfer_flag = TRANSFER_WAIT;
	p_spitask->transfer_status = 0;
	p_spitask->transfer_len = 0;

	list_enqueue(spi_queue, p_spitask, ASSIGN_FLAG);

	InfoPrintf("%d slot, addr %#x ------end \n", slot, addr);


	return p_spitask;
}

spitask_st *insert_spi_transfer_task(int slot, u_int addr, void *data, int datalen, int transfer_mode)
{
	list_st *spi_queue;
	
	spitask_st *p_spitask;

	if(spi_masters[slot].run_flag != RUNNING_FLAG){
		printf("%d slot spi master transfer task is not running\n", slot);
		return NULL;
	}
	
	InfoPrintf("%d slot, addr %#x, %d len, mode : %#x \n", slot, addr, datalen, transfer_mode);
	spi_queue = &spi_master_module[slot].task_queue;

	p_spitask = malloc(sizeof(spitask_st));
	if(p_spitask == NULL){
		perror("spitask_st");
		exit(-1);
	}

	
	p_spitask->slot = slot;
	p_spitask->addr = addr;
	p_spitask->len = datalen;
	p_spitask->data = data;
	p_spitask->transfer_mode = transfer_mode;
	p_spitask->transfer_flag = TRANSFER_WAIT;
	p_spitask->transfer_status = 0;
	p_spitask->transfer_len = 0;

	list_insert(spi_queue, p_spitask, ASSIGN_FLAG);

	InfoPrintf("%d slot, addr %#x ------end \n", slot, addr);


	return p_spitask;
}

int wait_spi_transfer_end(spitask_st *spitask)
{
	if(spitask == NULL){
		return 0;
	}
	
	while(spitask->transfer_flag != TRANSFER_END){
		if(spi_masters[spitask->slot].run_flag != RUNNING_FLAG){
			return -1;
		}
		usleep(1000);
	}

	if(spitask->transfer_status == TRANSFER_OK){
		return 0;
	}else{
		return -1;
	}
	
}

//返回值-1 传输错误，正常为传输的进度是一个精度为0.01 的完成比例的10000 倍，0 表示没开始传输，
int check_spi_transfer_status(spitask_st *spitask)
{
	if((spitask->transfer_len== TRANSFER_END)){
		if(spitask->transfer_status == TRANSFER_OK){
			return 10000;
		}else{
			return -1;
		}
	}else{
		if(spitask->len>0){
			return (spitask->transfer_len * 10000 / spitask->len);
		}else{
			return 0;	
		}
	}
}
#endif



spitask_st *spi_transfer_msg(int slot, int msgid, void *data, int datalen, int stransfer_mode)
{
	u_int addr;
	char *msgdata = NULL;
	u_short crc16;

	spitask_st *p_spitask;
	

	InfoPrintf(" %d slot, msgid = %#x, %d len , stransfer_mode = %d \n", slot, msgid, datalen, stransfer_mode);

	if(datalen<0){
		datalen = 0;
	}

	msgdata = malloc(datalen + 4*2);
	if(msgdata == NULL){
		perror("spimsgdata");
		exit(-1);
	}

	memcpy(msgdata, &msgid, 4);
	memcpy(msgdata+4, &datalen, 4);

	if(data && (datalen>0)){
		memcpy(msgdata+8, data, datalen);
	}else{
		datalen = 0;
	}
		
	
//	crc16 = calc_crc16(msgdata, datalen + 8);
//	mempy(msgdata+datalen +8, crc16, 2);

	p_spitask = add_spi_transfer_task(slot, SLAVER_SPI_MESSAGE_ADDR, msgdata, datalen+8, stransfer_mode);

	if(p_spitask == NULL){
		free(msgdata);
		return NULL;
	}

	return p_spitask;
}

int spi_read_data(int slot, u_int addr, void **ppdata)
{
	spitask_st *p_spitask;
	int rd_len;

	if(ppdata == NULL){
		//return -1;
	}

	InfoPrintf("spi_read_data : %d slot, addr = %#x \n", slot, addr);

	p_spitask = add_spi_transfer_task(slot, addr, NULL, -1, SPI_PROCEDURAL_READ);

	if(p_spitask == NULL){
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){
		rd_len = -1;
	}else{
		if(ppdata){
			*ppdata = p_spitask->data;
		}else{
			free(p_spitask->data);
		}
		rd_len = p_spitask->len;

		InfoPrintf("spi_read_data : datalen = %d \n", rd_len);
	}
	
	free(p_spitask);

	return rd_len;
}


int spi_write_data(int slot, u_int addr, void *data, int len)
{
	int ret = 0;
	spitask_st *p_spitask;

	InfoPrintf("spi_write_data : %d slot, addr = %#x, %d len \n", slot, addr, len);

	p_spitask = add_spi_transfer_task(slot, addr, data, len, SPI_PROCEDURAL_WRITE);

	if(p_spitask == NULL){
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){       
		ret = -1;
	}

	free(p_spitask);

	return ret;
}


int spi_write_word(int slot, u_int addr, u_short data)
{
	int ret = 0;
	spitask_st *p_spitask;

	p_spitask = add_spi_transfer_task(slot, addr, &data, 2, SPI_BLOCK_WRITE);

	if(p_spitask == NULL){
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){
		ret = -1;
	}

	free(p_spitask);

	return ret;
}

int spi_read_block(int slot, u_int addr, void *data, int len)
{
	spitask_st *p_spitask;

	if(len > SPI_BUFFER_SIZE){
		printf("spi_read_block : data len %d over than the spi buffer[%d] \n", len, SPI_BUFFER_SIZE);
	}
	
	p_spitask = add_spi_transfer_task(slot, addr, data, len, SPI_BLOCK_READ);

	if(p_spitask == NULL){
		printf("spi_read_block : spitask == NULL \n");
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){
		printf("spi_read_block : wait failed \n");
		len = -1;
	}else{
		InfoPrintf("spi_read_block : end len = %d \n", p_spitask->len);
		len = p_spitask->len;
	}

	free(p_spitask);

	return len;
}

int spi_write_block(int slot, u_int addr, void *data, int len)
{
	int ret = 0;
	spitask_st *p_spitask;

	if(len > SPI_BUFFER_SIZE){
		printf("spi_write_block : data len %d over than the spi buffer[%d] \n", len, SPI_BUFFER_SIZE);
	}

	p_spitask = add_spi_transfer_task(slot, addr, data, len, SPI_BLOCK_WRITE);

	if(p_spitask == NULL){
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){
		ret = -1;
	}

	free(p_spitask);

	return 0;
}

int spi_send_msg(int slot, int msgid, void *data, int len)
{
	int ret = 0;
	spitask_st *p_spitask;

	p_spitask = spi_write_msg(slot, msgid, data, len);

	if(p_spitask == NULL){
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){
		ret = -1;
	}
	
	free(p_spitask);

	return ret;
}

int spi_rcv_msg(int slot, int msgid, void *data, int len, char **pp_rcvdata)
{
	spitask_st *p_spitask;
	
	p_spitask = spi_read_msg(slot, msgid, data, len);

	if(p_spitask == NULL){
		return -1;
	}
	
	if(wait_spi_transfer_end(p_spitask) < 0){
		*pp_rcvdata = NULL;
		len = -1;
	}else{

		len = p_spitask->len;
		*pp_rcvdata = p_spitask->data;

		InfoPrintf(" ---%d slot, msgid = %#x, rcv datalen = %d ------------\n\n", slot, msgid, len);
	}

	free(p_spitask);

	return len;
}

int spi_send_json(int slot, int msgid, char *json_txt)
{
	int len;

	if(json_txt){
		len = strlen(json_txt);
	}else{
		len = 0;
	}
	
	return spi_send_msg(slot, msgid, json_txt, len);

}

int spi_rcv_json(int slot, int msgid, char *json_txt, char **pp_rcvdata)
{
	int len;
	char *pdata;

	if(json_txt){
		len = strlen(json_txt);
	}else{
		len = 0;
	}
	
	len = spi_rcv_msg(slot, msgid, json_txt, len, pp_rcvdata);

	/*
	if(len>0){
		printf("%s\n\n", *pp_rcvdata);
	}
	//*/
	
	return len;
}

#define FILENAME_MAX_LEN	40

//return copy total bytes, contain filename and the byte of filelen
int read_and_copy_file(char *databuf, char *file, int max_filename_len)
{
	int len, rdlen;
	int fd;
	char *filename;
	char *pdata;
	
	printf("read_and_copy_file ... %s ...  \n", file);

	pdata = databuf;
	len = max_filename_len+4;

	filename = strrchr(file, '/');
	if(filename){
		filename++;
	}else{
		filename = file;
	}
	
	//printf("read_and_copy_file ... %s ...  \n", filename);

	strcpy(pdata, filename);
	pdata[strlen(filename)] = 0;
	pdata += len;
	
	fd = open(file, O_RDONLY);
	if(fd<0){
		printf(" open %s failed \n", file);
		return 0;
	}

	do{
		rdlen = read(fd, pdata, 1024);
		//printf("rdlen = %d \n", rdlen);
		if(rdlen>0){
			pdata += rdlen;
			len += rdlen;
		}
	}while(rdlen>0);

	close(fd);
	
	rdlen = len-4-max_filename_len;
	memcpy(databuf+max_filename_len, &rdlen, 4);

	printf("read_and_copy_file ... %d bytes ...  \n", len);

	return len;
}

//return write file bytes and data header bytes
int copy_and_write_file(char *databuf,  int max_filename_len)
{
	int ret;
	int len, wrlen;
	int fd;
	char *pdata;
	char *filename;
		

	pdata = databuf;
	filename = pdata;
	
	pdata += max_filename_len;

	memcpy(&len, pdata, 4);
	pdata += 4;

	ret = len + max_filename_len + 4;

	
	printf("copy_and_write_file ... %s ...%d bytes  \n", filename, len);
	
	fd = open(filename, O_RDWR | O_TRUNC| O_CREAT, 0644);
	if(fd<0){
		printf(" open %s failed \n", filename);
		return 0;
	}

	while(len>0){
		wrlen = write(fd, pdata, len);
		//printf("wrlen = %d \n", wrlen);
		if(wrlen>0){
			pdata += wrlen;
			len -= wrlen;
		}else{
			ret = -1;
			break;
		}
	}

	close(fd);
	
	return ret;
}



int spi_send_file(int slot, char *filename)
{

	int rdlen, i = 0;
	char *data;
	int len;
	int ret;
	
	printf("%d slot spi_send_file ... %s ...  \n", slot, filename);

	len = calc_filesize(filename);
	len += FILENAME_MAX_LEN + 4;
	data = xMalloc(len);

	rdlen = read_and_copy_file(data, filename, FILENAME_MAX_LEN);

	if(len==rdlen){
		printf("%d slot  spi_send_file %d bytes \n", slot, len);
		ret = spi_write_data(slot, SLAVER_SPI_SEND_FILE_ADDR, data, len);
	}else{
		ret = -1;
	}
	
	free(data);

	return ret;
	
}

int spi_get_file(int slot, char *filename)
{

	int wrlen;
	int datalen;
	char *databuf;

	printf("%d slot spi_get_file ... %s ...  \n", slot, filename);

	datalen = spi_rcv_msg(slot, SPI_MSG_GET_FILE, filename, strlen(filename), &databuf);
	printf("spi_get_files : %d slot, len = %d \n", slot, datalen);
	if(datalen<=0){
		printf("get %d slot files : failed \n", slot);
		wrlen = -1;
	}else{
		wrlen = copy_and_write_file(databuf, FILENAME_MAX_LEN);
	}
	
	free(databuf);
			
	return wrlen;
}



int spi_send_cmd(int slot, char *cmd, char **result_data)
{
	int len;
	char *pdata;

	if((cmd == NULL) || ((len = strlen(cmd)) <= 0)){
		return 0;
	}

	printf("send cmd \"%s\" to %d board by spi \n", cmd, slot);

	len = spi_rcv_msg(slot, SPI_MSG_EXEC_CMD, cmd, len, result_data);
	
	return len;
}


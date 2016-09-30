
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "fpga.h"
#include "fpga_reg.h"

#include "utils/utils.h"

#ifdef DEBUG_IN_X86
#define ioctl(fd, cmd, arg)		0
#else
#include <sys/ioctl.h>
#endif

#define MY_CMD_MAGIC			0xf
#define FPGA_FILE_LENGTH		_IOW(MY_CMD_MAGIC, 1, int)
#define FPGA_CONFIG				_IOW(MY_CMD_MAGIC, 2, int)
#define FPGA_PHY_ADDRESS		_IOW(MY_CMD_MAGIC, 3, int)
#define FPGA_READ_8BIT			_IOW(MY_CMD_MAGIC, 4, int)
#define FPGA_WRITE_8BIT			_IOW(MY_CMD_MAGIC, 5, int)
#define FPGA_READ_16BIT			_IOW(MY_CMD_MAGIC, 6, int)
#define FPGA_WRITE_16BIT		_IOW(MY_CMD_MAGIC, 7, int)
#define FPGA_READ_32BIT			_IOW(MY_CMD_MAGIC, 8, int)
#define FPGA_WRITE_32BIT		_IOW(MY_CMD_MAGIC, 9, int)
#define FPGA_DMA_READ			_IOW(MY_CMD_MAGIC, 10, int)
#define FPGA_DMA_WRITE			_IOW(MY_CMD_MAGIC, 11, int)


#define FPGA_RESET			_IOW(MY_CMD_MAGIC, 12, int)
#define FPGA_ENABLE_IRQ			_IOW(MY_CMD_MAGIC, 13, int)
#define FPGA_GET_OWNER			_IOW(MY_CMD_MAGIC, 14, int)



#define FPGA_DEVICE			"/dev/misc/fpga"
//#define FPGA_DEVICE			"/dev/fpga"
//#define FPGA_FILENAME		"/home/root/top.rbf"


/*
register INTERRUPT_ENABLE   = 32'h00000010    wr  high active; 
register INTERRUPT_STATUS   = 32'h00000014    rd  bit0-bit15;
register INTERRUPT_CLEAR    = 32'h00000018    wr  high active; 
 
INTERRUPT_STATUS : bit6-bit0 子板发出的中断，bit0为子板1，bit1为子板2，，，，
                   bit13-bit8 子板插拔发出的中断，bit8为子板1，bit9为子板2，，，，

//*/
#define REG_INTERRUPT_ENABLE	0x10
#define REG_INTERRUPT_STATUS	0x14
#define REG_INTERRUPT_CLEAR	0x18


//This register address deponds on fpga settings
/*
0x2	PAGE_CTRL_REG	r,w
bit[15]:16bit ena?
bit[14:8]:reserve
bit[7:0]page
0x8	AVALON_CH_REG	rw	avalon channel
0xa	MAPPING_PAGE1	rw	mapping_page1[15:0]
0xc	MAPPING_PAGE2	rw	mapping_page2[15:0]
*/
#define FPGA_PAGE_CNTL_REG	0x02
#define FPGA_AVALON_CHAN_REG	0x08
#define FPGA_MAPPING_PAGE1	0x0a
#define FPGA_MAPPING_PAGE2	0x0c

#define FPGA_PAGING_0_BASE_ADDR	(SZ_32M * 3)
#define FPGA_PAGING_1_BASE_ADDR	SZ_32M
#define FPGA_PAGING_2_BASE_ADDR	(SZ_64M * 2)

#define FPGA_PAGING_BASE_ADDR	FPGA_PAGING_0_BASE_ADDR

#define FPGA_RW_FLAG_BIT		(1<<8)
#define AVALON_CHAN_FLAG_BIT	(1<<9)
#define FPGA_PAGE_FLAG_BIT		(1<<10)
#define FPGA_MODE_FLAG_BIT		(1<<11)

#define IRQ_BIT_SLOT_0		(1<<0)
#define IRQ_BIT_SLOT_1		(1<<1)
#define IRQ_BIT_SLOT_2		(1<<2)
#define IRQ_BIT_SLOT_3		(1<<3)
#define IRQ_BIT_SLOT_4		(1<<4)
#define IRQ_BIT_SLOT_5		(1<<5)
#define IRQ_BIT_SLOT_6		(1<<6)



static int fpga_fd = 0;
static u_short fpga_irq_mask = 0x7f;

typedef struct gpmc_arg{
	u_int addr;
	u_int data;
	int len;
	int err;
}gpmc_arg_t;

struct {
	int current_page;
	int access_count;
	pthread_rwlock_t rw_lock;
	pthread_mutex_t mutex_lock;
} fpga_ctrl = {
	.current_page = 0,
	.access_count = 0,
	.rw_lock =  PTHREAD_RWLOCK_INITIALIZER,
	.mutex_lock = PTHREAD_MUTEX_INITIALIZER,
};


#define fpga_access_lock()		pthread_mutex_lock(&fpga_ctrl.mutex_lock)
#define fpga_access_trylock()		pthread_mutex_trylock(&fpga_ctrl.mutex_lock)
#define fpga_access_unlock()		do{pthread_mutex_trylock(&fpga_ctrl.mutex_lock);pthread_mutex_unlock(&fpga_ctrl.mutex_lock);}while(0)

#define fpga_rd_lock()		pthread_rwlock_rdlock(&fpga_ctrl.rw_lock)
#define fpga_wr_lock()		pthread_rwlock_wrlock(&fpga_ctrl.rw_lock)
#define fpga_rw_unlock()		pthread_rwlock_unlock(&fpga_ctrl.rw_lock)


//#define FPGA_Reset()		ioctl(fpga_fd, FPGA_RESET, NULL)
int FPGA_Reset(void)
{
	int ret = 0;
	ret = ioctl(fpga_fd, FPGA_RESET, NULL);

	printf("FPGA reset : %d \n", ret);

	return ret;
}

int FPGA_Get_Owner(void)
{
	int ret = 0;
	u_int pid;
	
	ret = ioctl(fpga_fd, FPGA_GET_OWNER, &pid);

	printf(" FPGA_Get_Owner  : pid = %d \n", pid);

	return ret;
}

int FPGA_Enable_IRQ(int enable)
{
	int ret = 0;
	
	ret = ioctl(fpga_fd, FPGA_ENABLE_IRQ, enable);

	printf("FPGA enable IRQ : %d \n", enable);

	return ret;
}

int read_fpga_irq_status(void)
{
	int ret = 0;
	read(fpga_fd, &ret, 4);

	return ret;
}


u_int get_fpga_irq_status(void)
{
	return  FPGA_Readl(REG_INTERRUPT_STATUS);
}

void set_fpga_irq(u_short irq_mask)
{
	int v_s32;
	
	do{
		
		FPGA_Writel(REG_INTERRUPT_ENABLE, irq_mask);
		v_s32 = FPGA_Readl(REG_INTERRUPT_ENABLE);
		//printf("set fpga irq : %#x\n",  v_s32);
		
	}while(v_s32 != irq_mask);
	
}

void clean_fpga_slot_irq(int slot)
{

	fpga_wr_lock();
	fpga_irq_mask &= ~(1<<slot);
	//FPGA_Writew(REG_INTERRUPT_ENABLE, fpga_irq_mask);
	printf("clean fpga %d slot irq : %#x\n", slot , fpga_irq_mask);
	fpga_rw_unlock();
}

void down_fpga_slot_irq(int slot)
{
	clean_fpga_slot_irq(slot);
	//set_fpga_irq(fpga_irq_mask);
}

void set_fpga_slot_irq(int slot)
{

	fpga_wr_lock();
	if(fpga_irq_mask & (1<<slot)){
		fpga_rw_unlock();
		return;
	}
	
	printf("set fpga %d slot irq : %#x\n", slot , fpga_irq_mask);
	fpga_irq_mask |= (1<<slot);
	//set_fpga_irq(fpga_irq_mask);

	fpga_rw_unlock();
}


int FPGA_Config(char *fpga_code, int code_len)
{
	int error = 0;

	error = ioctl(fpga_fd, FPGA_FILE_LENGTH, &code_len);
	error = ioctl(fpga_fd, FPGA_CONFIG, fpga_code);
	
	if(error != -1){
		printf("Config FPGA done!\n");
	}else{
		/*release the buffer*/
		printf("Config FPGA Failure error=%x\n", error);
	}

	return error;
}



unsigned char FPGA_Readb(unsigned int address)
{
	gpmc_arg_t arg;

	arg.addr = address;
	ioctl(fpga_fd, FPGA_READ_8BIT, &arg);

	//printf("Read 8Bit Data from FPGA:address = 0x%x, data = 0x%02x\n", address, (unsigned char)arg.data);

	return arg.data;
}

void FPGA_Writeb(unsigned int address, unsigned char data)
{	
	gpmc_arg_t arg;

	arg.addr = address;
	arg.data = data;
	ioctl(fpga_fd, FPGA_WRITE_8BIT, &arg);

	//printf("Write 8Bit Data to FPGA:address = 0x%x, data = 0x%02x\n", address, (unsigned char)data);
}

unsigned short FPGA_Readw(unsigned int address)
{
	gpmc_arg_t arg;

	arg.addr = address;
	ioctl(fpga_fd, FPGA_READ_16BIT, &arg);

	//printf("Read 16Bit Data from FPGA:address = 0x%x, data = 0x%04x\n", address, (unsigned short)arg.data);

	return arg.data;
}

void FPGA_Writew(unsigned int address, unsigned short data)
{
	gpmc_arg_t arg;

	arg.addr = address;
	arg.data = data;
	ioctl(fpga_fd, FPGA_WRITE_16BIT, &arg);

	//printf("Write 16Bit Data to FPGA:address = 0x%x, data = 0x%04x\n", address, (unsigned short)data);
}

unsigned int FPGA_Readl(unsigned int address)
{

	gpmc_arg_t arg;
	
	arg.addr = address;
	ioctl(fpga_fd, FPGA_READ_32BIT, &arg);
	
	//printf("Read 32Bit Data from FPGA:address = 0x%x, data = 0x%08x\n", address,  arg.data);

	return  arg.data;
}

void FPGA_Writel(unsigned int address, unsigned int data)
{

	gpmc_arg_t arg;

	arg.addr = address;
	arg.data = data;
	ioctl(fpga_fd, FPGA_WRITE_32BIT, &arg);
	
	//printf("Write 32Bit Data to FPGA:address = 0x%x, data = 0x%08x\n", address, (unsigned int)data);
}

int FPGA_DMA_Read(unsigned int address, void *data, int len)
{
	gpmc_arg_t arg;

	arg.addr = address;
	arg.data = (u_int)data;
	arg.len = len;

	ioctl(fpga_fd, FPGA_DMA_READ, &arg);

	return arg.err;
}

int FPGA_DMA_Write(unsigned int address, void *data, int len)
{
	gpmc_arg_t arg;

	arg.addr = address;
	arg.data = (u_int)data;
	arg.len = len;

	ioctl(fpga_fd, FPGA_DMA_WRITE, &arg);

	return arg.err;
}


void FPGA_Set_Page(int page)
{
	u_short v_u16;

	//TagPrintf(page);
	//TagPrintf(fpga_ctrl.current_page);

	#if 1	
	fpga_access_lock();
	if(fpga_ctrl.current_page != page){
		//TagPrintf(page);
		fpga_ctrl.current_page = page;
		v_u16 = FPGA_Readw(FPGA_PAGE_CNTL_REG);
		
		v_u16 &= 0xff00;
		v_u16 |= page;

		FPGA_Writew(FPGA_PAGE_CNTL_REG, v_u16);
	}
	#else
	if(fpga_ctrl.current_page != page){
		fpga_access_lock();
		fpga_ctrl.current_page = page;
		fpga_ctrl.access_count = 1;

		TagPrintf(fpga_ctrl.access_count);

		v_u16 = FPGA_Readw(FPGA_PAGE_CNTL_REG);
		
		v_u16 &= 0xff00;
		v_u16 |= page;

		FPGA_Writew(FPGA_PAGE_CNTL_REG, v_u16);
	}else{
		fpga_access_trylock();
		fpga_ctrl.access_count++;
		TagPrintf(fpga_ctrl.access_count);
	}
	#endif
		

	//printf("FPGA_Set_Page:: page = 0x%x\n", page);
}

void FPGA_End_Page(int page)
{
	u_short v_u16;

	//TagPrintf(page);
	//TagPrintf(fpga_ctrl.current_page);

	#if 1
	fpga_access_unlock();
	#else
	if(fpga_ctrl.current_page != page){
		TagPrintf(fpga_ctrl.current_page);
		while(fpga_ctrl.current_page != page){usleep(1000);}
		fpga_access_unlock();
		TagPrintf(fpga_ctrl.access_count);
		TagPrintf(fpga_ctrl.current_page);
		fpga_ctrl.access_count = 0;
	}else{
		fpga_ctrl.access_count--;
		TagPrintf(fpga_ctrl.access_count);
		if(fpga_ctrl.access_count<=0){
			TagPrintf(fpga_ctrl.access_count);
			fpga_access_unlock();
		}
	}
	#endif
	//printf("FPGA_End_Page:: page = 0x%x\n", page);
}

void FPGA_Set_16bit_mode(void)
{
	u_short v_u16;

	v_u16 = FPGA_Readw(FPGA_PAGE_CNTL_REG);

	if((v_u16&0x8000) == 0){
		v_u16 |= 0x8000;	
		FPGA_Writew(FPGA_PAGE_CNTL_REG, v_u16);
	}

	printf("FPGA_Set_16bit_mode:: \n");
}

void FPGA_End_16bit_mode(void)
{
	u_short v_u16;

	//v_u16 = FPGA_Readw(FPGA_PAGE_CNTL_REG);
	//v_u16 &= ~FPGA_MODE_FLAG_BIT;

	//FPGA_Writew(FPGA_PAGE_CNTL_REG, v_u16);

	printf("FPGA_End_16bit_mode:: \n");
}


void FPGA_Set_32bit_mode(void)
{
	u_short v_u16;

	v_u16 = FPGA_Readw(FPGA_PAGE_CNTL_REG);
 
	if((v_u16&0x8000)){
		v_u16 &= ~0x8000;
 
		FPGA_Writew(FPGA_PAGE_CNTL_REG, v_u16);
	}

	printf("FPGA_Set_32bit_mode:: \n");
}

void FPGA_End_32bit_mode(void)
{
	u_short v_u16;

	//v_u16 = FPGA_Readw(FPGA_PAGE_CNTL_REG);
	//v_u16 &= ~FPGA_MODE_FLAG_BIT;
	
	//FPGA_Writew(FPGA_PAGE_CNTL_REG, v_u16);

	printf("FPGA_End_32bit_mode:: \n");
}

void FPGA_Set_Avalon_Channel(int channel)
{
	u_short v_u16;

	v_u16 = FPGA_Readw(FPGA_AVALON_CHAN_REG);

	if(v_u16 != channel){
		FPGA_Writew(FPGA_AVALON_CHAN_REG, channel);	
	}
	
	printf("FPGA_Set_Avalon_Channel::  0x%x\n", channel);
}

void FPGA_End_Avalon_Channel(int channel)
{
	u_short v_u16;

	//v_u16 = FPGA_Readw(FPGA_PAGE_CNTL_REG);
	//v_u16 &= ~AVALON_CHAN_FLAG_BIT;

	//FPGA_Writew(FPGA_PAGE_CNTL_REG, v_u16);

	printf("FPGA_End_Avalon_Channel:: \n");
}


unsigned short FPGA_Readw_DDR(unsigned int address)
{
	
	unsigned short data;

	data = FPGA_Readw(address+FPGA_PAGING_BASE_ADDR);

	//printf("FPGA_Readw_DDR::address = 0x%x, data = 0x%08x\n", address, (unsigned int)data);

	return data;
}

void FPGA_Writew_DDR(unsigned int address, unsigned short data)
{
	 
	FPGA_Writew(address+FPGA_PAGING_BASE_ADDR, data);

	//printf("FPGA_Writew_DDR::address = 0x%x, data = 0x%08x\n", address, (unsigned int)data);
}

unsigned int FPGA_Readl_DDR(unsigned int address)
{
	
	unsigned int data;

	data = FPGA_Readl(address+FPGA_PAGING_BASE_ADDR);

	//printf("FPGA_Readl_DDR::address = 0x%x, data = 0x%08x\n", address, (unsigned int)data);

	return data;
}

void FPGA_Writel_DDR(unsigned int address, unsigned int data)
{
	 
	FPGA_Writel(address+FPGA_PAGING_BASE_ADDR, data);

	//printf("FPGA_Writel_DDR::address = 0x%x, data = 0x%08x\n", address, (unsigned int)data);
}



int fpga_register_signal(pid_t pid, int signo, void (*sig_handler)(int))
{
	#if (!defined(INDEPENDENT_UPGRADE)) 

	printf("fpga init : pid = %u \n", pid);
	if (fcntl(fpga_fd, F_SETOWN, pid) < 0){
		printf("fpga fcntl F_SETOWN faild\n");
		return -1;
	}
	
	signal(signo, sig_handler);
	#endif
}

void init_fpga(void)
{
	FPGA_Writew(FPGA_PAGE_CNTL_REG, 0);
	FPGA_Writew(FPGA_AVALON_CHAN_REG, 0);
	
	#if FPGA_ACCESS_MODE
	FPGA_Set_16bit_mode();
	#else
	FPGA_Set_32bit_mode();
	#endif

	set_fpga_irq(fpga_irq_mask);
}

int open_fpga(void)
{
	int fpga_phy_addr;

	
	fpga_fd = open(FPGA_DEVICE, O_RDWR);

	if(fpga_fd < 0){
		printf("***Can't open the fpga driver handle!\r\n");
		return -1;
	}

	return fpga_fd;
}

void close_fpga(void)
{
	close(fpga_fd);
}



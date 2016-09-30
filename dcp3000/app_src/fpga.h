#ifndef __FPGA_H__
#define __FPGA_H__

#include <sys/types.h>
#include "basedef.h"

#define DDR_BASE_ADDRESS			SZ_1G

#define MAX_FPGA_PAGE		(128/8)
#define FPGA_PAGE_SIZE		SZ_32M 


#define DDR_PAGE_BASE		(DDR_BASE_ADDRESS/FPGA_PAGE_SIZE)

#define FPGA_ACCESS_MODE		0		// 1 16bit, 0 32bit

#define FPGA_IRQ_SIGNAL	SIGUSR1

extern int open_fpga(void);
extern void close_fpga(void);
void init_fpga(void);


int read_fpga_irq_status(void);
u_int get_fpga_irq_status(void);
void set_fpga_irq(u_short irq_mask);
void set_fpga_slot_irq(int slot);
void clean_fpga_slot_irq(int slot);
void down_fpga_slot_irq(int slot);

int fpga_register_signal(pid_t pid, int signo, void (*sig_handler)(int));

int FPGA_Config(char *fpga_code, int code_len);

extern unsigned char FPGA_Readb(unsigned int address);
extern void FPGA_Writeb(unsigned int address, unsigned char data);

extern unsigned short FPGA_Readw(unsigned int address);
extern void FPGA_Writew(unsigned int address, unsigned short data);

extern unsigned int FPGA_Readl(unsigned int address);
extern void FPGA_Writel(unsigned int address, unsigned int data);

extern unsigned short FPGA_Readw_DDR(unsigned int address);
extern void FPGA_Writew_DDR(unsigned int address, unsigned short data);

extern unsigned int FPGA_Readl_DDR(unsigned int address);
extern void FPGA_Writel_DDR(unsigned int address, unsigned int data);

extern int FPGA_DMA_Read(unsigned int address, void *data, int len);
extern int FPGA_DMA_Write(unsigned int address, void *data, int len);

extern void FPGA_Set_Page(int page);
extern void FPGA_End_Page(int page);

extern void FPGA_Set_16bit_mode(void);
extern void FPGA_End_16bit_mode(void);

extern void FPGA_Set_32bit_mode(void);
extern void FPGA_End_32bit_mode(void);

extern void FPGA_Set_Avalon_Channel(int channel);
extern void FPGA_End_Avalon_Channel(int channel);


#endif


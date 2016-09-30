#ifndef __FLASH_H__
#define __FLASH_H__

#include "sys/types.h"
#include "../basedef.h"


#define FLASH_SECTOR_SIZE	 0x20000

#define KERNEL_MTD_BASE_ADDR		(0xe0000)
#define KERNEL_MTD_BASE_SECTOR		(KERNEL_MTD_BASE_ADDR / FLASH_SECTOR_SIZE)
#define KERNEL_MTD_SIZE		(SZ_4M)
#define KERNEL_MTD_SECTOR_COUNT		(KERNEL_MTD_SIZE / FLASH_SECTOR_SIZE)

#define ROOTFS_MTD_BASE_ADDR		(0x4e0000)
#define ROOTFS_MTD_BASE_SECTOR		(ROOTFS_MTD_BASE_ADDR / FLASH_SECTOR_SIZE)
#define ROOTFS_MTD_SIZE		(0x00b20000)
#define ROOTFS_MTD_SECTOR_COUNT		(ROOTFS_MTD_SIZE / FLASH_SECTOR_SIZE)

#define FPGA_MTD_BASE_ADDR		(SZ_16M)
#define FPGA_MTD_BASE_SECTOR		(FPGA_MTD_BASE_ADDR / FLASH_SECTOR_SIZE)
#define FPGA_MTD_SIZE		(SZ_16M-SZ_2M)
#define FPGA_MTD_SECTOR_COUNT		(FPGA_MTD_SIZE / FLASH_SECTOR_SIZE)

#define HOME_MTD_BASE_ADDR		(SZ_32M-SZ_2M)
#define HOME_MTD_BASE_SECTOR		(HOME_MTD_BASE_ADDR / FLASH_SECTOR_SIZE)
#define HOME_MTD_SIZE		(SZ_2M+SZ_8M)
#define HOME_MTD_SECTOR_COUNT		(HOME_MTD_SIZE / FLASH_SECTOR_SIZE)

#define OPT_MTD_BASE_BASE		(SZ_32M+SZ_8M)
#define OPT_MTD_BASE_SECTOR		(OPT_MTD_BASE_BASE / FLASH_SECTOR_SIZE)
#define OPT_MTD_SIZE		(SZ_32M-SZ_8M)
#define OPT_MTD_SECTOR_COUNT		(OPT_MTD_SIZE / FLASH_SECTOR_SIZE)


#define FPGA_CODE_BASE		FPGA_MTD_BASE_ADDR
#define FPGA_CODE_SIZE		FPGA_MTD_SIZE


#define APP_CODE_BASE_ADDR		HOME_MTD_BASE_ADDR
#define APP_CODE_BASE_SECTOR		(APP_CODE_BASE_ADDR / FLASH_SECTOR_SIZE)
#define APP_CODE_SIZE		HOME_MTD_SIZE
#define APP_CODE_SECTOR_COUNT		(APP_CODE_SIZE / FLASH_SECTOR_SIZE)

int open_flash(void);
void close_flash(void);

int protect_flash(int sector, int count);
int unprotect_flash(int sector, int count);


int erase_flash(int sector, int count);

int read_flash(u_int addr, void *rdbuf, int rdlen);
int write_flash(u_int addr,void *wrdata, int wrlen);
int writew_flash(u_int addr, u_short wdata);

int read_flash_to_file(char *filename, int addr, int size);
int write_file_to_flash(u_int addr, u_char *data_prefix,char *file);

int write_file_to_flash_with_fn(u_int addr, u_char *data_prefix,char *file, void (*func)(double , void *), void *arg);
int erase_flash_with_fn(int sector, int count, void (*func)(double , void *), void *arg);


#endif //__FLASH_H__

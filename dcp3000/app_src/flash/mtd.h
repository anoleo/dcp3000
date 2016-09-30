#ifndef __MTD_H__
#define __MTD_H__

#include "sys/types.h"
//#include "flash.h"



struct mtd_info_user {
	u_char type;
	u_int flags;
	u_int size;	/* Total size of the MTD */
	u_int erasesize;
	u_int writesize;
	u_int oobsize;	/* Amount of OOB data per block (e.g. 16) */
	unsigned long long padding;	/* Old obsolete field; do not use */
};

typedef struct mtd_info_user mtdinfo_st;

mtdinfo_st *mtd_get_info(int mtd_fd);

int mtd_open(char *mtd_dev);
void mtd_close(int mtd_fd);

int mtd_erase(int mtd_fd, u_int addr, int count);

int mtd_read(int mtd_fd, u_int addr, void *rdbuf, int rdlen);
int mtd_write(int mtd_fd, u_int addr,void *wrdata, int wrlen);

int mtd_write_file(int mtd_fd, u_int addr, u_char *data_prefix,char *file);

int mtd_write_file_with_fn(int mtd_fd, u_int addr, u_char *data_prefix,char *file, void (*func)(double , void *), void *arg);
int mtd_erase_with_fn(int mtd_fd, u_int addr, int count, void (*func)(double , void *), void *arg);


#endif //__FLASH_H__

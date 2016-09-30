
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>


#include  "mtd.h"

//#include "../utils/system.h"

#define FLASH_SECTOR_SIZE	 0x20000
#define printchar(ch) do{putchar(ch);fflush(stdout);}while(0)

struct region_info_user {
	u_int offset;		/* At which this region starts,
				 * from the beginning of the MTD */
	u_int erasesize;	/* For this region */
	u_int numblocks;	/* Number of blocks in this region */
	u_int regionindex;
};


struct erase_info_user {
	u_int start;
	u_int length;
};


/*
 * Note, the following ioctl existed in the past and was removed:
 * #define MEMSETOOBSEL           _IOW('M', 9, struct nand_oobinfo)
 * Try to avoid adding a new ioctl with the same ioctl number.
 */

/* Get basic MTD characteristics info (better to use sysfs) */
#define MEMGETINFO		_IOR('M', 1, struct mtd_info_user)
/* Erase segment of MTD */
#define MEMERASE		_IOW('M', 2, struct erase_info_user)
/* Get the number of different erase regions */
#define MEMGETREGIONCOUNT	_IOR('M', 7, int)
/* Get information about the erase region for a specific index */
#define MEMGETREGIONINFO	_IOWR('M', 8, struct region_info_user)






int mtd_open(char *mtd_dev)
{

	return open(mtd_dev, O_RDWR);
}

void mtd_close(int mtd_fd)
{
	close(mtd_fd);
}

mtdinfo_st *mtd_get_info(int mtd_fd)
{
	int ret;
	static struct mtd_info_user mtd_info;
	
	if( (ret = ioctl(mtd_fd, MEMGETINFO, &mtd_info))!=0){
		perror("MTD get info failure");
		return NULL;
	}

	printf("mtd info : type = %#x, flags = %#x\n", mtd_info.type, mtd_info.flags);
	printf("mtd info : size = %#x, oobsize = %#x\n", mtd_info.size, mtd_info.oobsize);
	printf("mtd info : erasesize = %#x, writesize = %#x\n", mtd_info.erasesize, mtd_info.writesize);

	return &mtd_info;
}

int mtd_get_region_info(int mtd_fd, int region_idx)
{
	int ret;
	struct region_info_user region_info;
	region_info.regionindex = region_idx;
	if( (ret = ioctl(mtd_fd, MEMGETREGIONINFO, &region_info))!=0){
		perror("MTD get region info failure");
		return ret;
	}

	printf("region %d: offset = %#x, erasesize = %#x, numblocks = %#x\n", region_info.regionindex, region_info.offset, region_info.erasesize, region_info.numblocks);

	return ret;
}

int mtd_erase_with_fn(int mtd_fd, u_int addr, int count, void (*func)(double , void *), void *arg)
{
	int i, ret;
	struct erase_info_user erase_info;
	
	if(addr%FLASH_SECTOR_SIZE){
		return 0;
	}

	printf("erase flash : %#x sectors at offset %#x  \n", count, addr);

	printf("nor flash erase ");

	for(i=0; i<count; i++){
		erase_info.start =  addr+i*FLASH_SECTOR_SIZE;
		erase_info.length = FLASH_SECTOR_SIZE;
		if( (ret = ioctl(mtd_fd, MEMERASE, &erase_info))!=0){
			perror("MTD Erase failure");
			return ret;
		}

		printchar('.');
		if(func){
			func((double)(i+1)/count, arg);
		}
	}

	
	printf(" ok\n");
	
	return 0;
}


int mtd_erase(int mtd_fd, u_int addr, int count)
{

	return mtd_erase_with_fn(mtd_fd, addr, count, NULL, NULL);
}


int mtd_read(int mtd_fd, u_int addr, void *rdbuf, int len)
{


	if(lseek(mtd_fd, addr, SEEK_SET)<0){
		return -1;
	}
	
	len = read(mtd_fd, rdbuf, len);

	return len;
}

int mtd_write(int mtd_fd, u_int addr, void *wrdata, int len)
{

	int i;
	u_short *wbuf = (u_short *)wrdata;
	u_short wdata;

	int remain_bytes;
	int wbuf_count;
	int wbufoff;

	
	if((wrdata ==NULL) || (len<=0)){
		return -1;
	}

	if(addr%2){
		printf("write_flash :  %#x is a even address and it's invalid \n", addr);
		return -1;
	}

	if(lseek(mtd_fd, addr, SEEK_SET)<0){
		perror("MTD lseek");
		return -1;
	}

	len = write(mtd_fd, wrdata, len);

	 
	return len;
}


int mtd_write_file_with_fn(int mtd_fd, u_int addr, u_char *data_prefix,char *file, void (*func)(double , void *), void *arg)
{
	char wrbuf[4096];
	int filelen,len = 0, offset = 0;
	int i=0, fd;
	
	fd = open(file, O_RDONLY);
	if(fd<0){
		printf("write_file_to_mtd : open %s failed \n", file);
		return -1;
	}

	filelen = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	printf("write file %s %d bytes to mtd offset %#x \n", file, filelen, addr);

	if(lseek(mtd_fd, addr, SEEK_SET)<0){
		perror("MTD lseek");
		return -1;
	}
		
	if(data_prefix){
		len = strlen(data_prefix);
		if(len>4){
			len = 4;
		}
		for(i=0; i<len; i++){
			wrbuf[i] = data_prefix[i];
		}
		for(; i<4; i++){
			wrbuf[i] = 0xdc;
		}
		
		memcpy(&wrbuf[4], &filelen, 4);
		len = read(fd, &wrbuf[8], 4096-8) + 8;
		if(write(mtd_fd, wrbuf, len) < 0){
			close(fd);
			return -1;
		}
		printchar('.');
		if(func){
			func((double)offset/filelen, arg);
		}
		offset = len;	
	}else{
		offset = 0;
	}

	do{
		len = read(fd, wrbuf, 4096);
		//print_bytes(wrbuf, len);
		if(write(mtd_fd, wrbuf, len) < 0){
			break;
		}
		printchar('.');
		offset += len;
		if(func){
			func((double)offset/filelen, arg);
		}
	}while(len>0);

	close(fd);
	
	printf("ok\n");
	
	return 0;
}

int mtd_write_file(int mtd_fd, u_int addr, u_char *data_prefix,char *file)
{
	return mtd_write_file_with_fn(mtd_fd, addr, data_prefix, file, NULL, NULL);
	
}



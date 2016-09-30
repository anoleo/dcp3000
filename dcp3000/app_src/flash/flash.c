
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include  "flash.h"


//u_char flash_data_buf[FLASH_SECTOR_SIZE*2];


#define MY_CMD_MAGIC			0xf2

#define NOR_FLASH_READ			_IOW(MY_CMD_MAGIC, 1, int)
#define NOR_FLASH_WRITE			_IOW(MY_CMD_MAGIC, 2, int)
#define NOR_FLASH_WRITE_WORD			_IOW(MY_CMD_MAGIC, 3, int)
#define NOR_FLASH_ERASE			_IOW(MY_CMD_MAGIC, 4, int)
#define NOR_FLASH_ERASE_SECTOR			_IOW(MY_CMD_MAGIC, 5, int)
#define NOR_FLASH_PROTECT			_IOW(MY_CMD_MAGIC, 6, int)
#define NOR_FLASH_UNPROTECT			_IOW(MY_CMD_MAGIC, 7, int)
#define NOR_FLASH_PPBS			_IOW(MY_CMD_MAGIC, 8, int)


#define printchar(ch) do{putchar(ch);fflush(stdout);}while(0)


#define CONFIG_SYS_MAX_FLASH_SECT	512

typedef struct {
	volatile u_short *base;		//FLASH  virtual base address
	u_int	size;			/* total bank size in bytes		*/
	u_short	sector_count;		/* number of erase units		*/

	u_short	buffer_size;		/* # of bytes in write buffer		*/
	u_int	sector_size;		/* # of bytes in a sector		*/
	
	u_char	protect[CONFIG_SYS_MAX_FLASH_SECT]; /* sector protection status	*/


}flashinfo_st;

static flashinfo_st norinfo;


static int flash_fd;
#define FLASH_DEVICE			"/dev/misc/norflash"

typedef struct{
	u_int addr;
	char *data;
	int len;
	int sector;
	int count;
}norflash_args_st;

int protect_flash(int sector, int count)
{
	norflash_args_st norflash_args;

	norflash_args.sector = sector;
	norflash_args.count = count;
	
	return ioctl(flash_fd, NOR_FLASH_PROTECT, &norflash_args);
}

int unprotect_flash(int sector, int count)
{

	norflash_args_st norflash_args;

	norflash_args.sector = sector;
	norflash_args.count = count;
	
	return ioctl(flash_fd, NOR_FLASH_UNPROTECT, &norflash_args);
	
}


int read_flash_ppbs(int sector, int count)
{
	int i, ret = 0;
	flashinfo_st *info = &norinfo;

	norflash_args_st norflash_args;

	norflash_args.data = &info->protect[sector];
	norflash_args.sector = sector;
	norflash_args.count = count;
	
	ret = ioctl(flash_fd, NOR_FLASH_ERASE, &norflash_args);
	for(i=0; i<count; i++){
		printf("nor flash %d sector : ppb %d \n", sector+i, info->protect[sector+i]);
	}

	return ret;
}

int erase_flash_sector(int sector)
{
	norflash_args_st norflash_args;

	norflash_args.sector = sector;
	
	return ioctl(flash_fd, NOR_FLASH_ERASE_SECTOR, &norflash_args);
}

int erase_flash(int sector, int count)
{
	norflash_args_st norflash_args;

	norflash_args.sector = sector;
	norflash_args.count = count;
	
	return ioctl(flash_fd, NOR_FLASH_ERASE, &norflash_args);
}

int erase_flash_with_fn(int sector, int count, void (*func)(double , void *), void *arg)
{
	int i;
	flashinfo_st *info = &norinfo;
	
	if(count<=0){
		return 0;
	}

	printf("erase flash :   %d - %d  sectors\n", sector, sector+count-1);

	if((sector < 0) && (sector >= info->sector_count)){
		printf("erase_flash : the nor flash sector %d don't exist  \n", sector);
		return -1;
	}
	

	count += sector;
	if((count) > info->sector_count){
		printf("erase_flash :  %d sectors  exceed  the nor flash sector range\n", count - info->sector_count);
		count =  info->sector_count;
	}

	printf("nor flash erase ");

	count -= sector;
	for(i=0; i<count; i++){
		if(erase_flash_sector(i+sector)==0){
			printchar('.');
			func((double)(i+1)/count, arg);
		}else{
			return -i;
		}
	}

	
	printf(" ok\n");
	
	return 0;
}


int read_flash(u_int addr, void *rdbuf, int len)
{
	norflash_args_st norflash_args;

	norflash_args.addr = addr;
	norflash_args.data = rdbuf;
	norflash_args.len = len;
	
	return ioctl(flash_fd, NOR_FLASH_READ, &norflash_args);

}

int writew_flash(u_int addr, u_short wdata)
{

	norflash_args_st norflash_args;

	norflash_args.addr = addr;
	norflash_args.data = (char *)wdata;
	norflash_args.len = 2;
	
	return ioctl(flash_fd, NOR_FLASH_WRITE_WORD, &norflash_args);
	
}

int write_flash(u_int addr, void *wrdata, int len)
{

	norflash_args_st norflash_args;

	norflash_args.addr = addr;
	norflash_args.data = wrdata;
	norflash_args.len = len;
	
	return ioctl(flash_fd, NOR_FLASH_WRITE, &norflash_args);
	 
}

int open_flash(void)
{
	int i;
	flashinfo_st *info = &norinfo;
	
	info->buffer_size = 64;
	info->sector_size = FLASH_SECTOR_SIZE;
	info->sector_count = CONFIG_SYS_MAX_FLASH_SECT;
	info->size = info->sector_size * info->sector_count;


	flash_fd = open(FLASH_DEVICE, O_RDWR);

	if(flash_fd < 0){
		printf("***Can't open the norflash driver handle!\r\n");
		return -1;
	}
	
	return flash_fd;

}

void close_flash(void)
{
	close(flash_fd);
}

int read_flash_to_file(char *filename, int addr, int size)
{
	int fd, ret = 0;
	int wrlen, bufsize;
	char rdbuf[0x20000];
	
	bufsize = 0x20000;
	
	fd = open(filename, O_RDWR | O_TRUNC| O_CREAT, 0644);
	if(fd<0){
		printf(" open %s failed \n", filename);
		return 0;
	}

	while(size>0){
		
		if(bufsize > size){
			bufsize = size;
		}
		
		read_flash(addr, rdbuf, bufsize);

		wrlen = write(fd, rdbuf, bufsize);
		//printf("wrlen = %d \n", wrlen);
		if(wrlen>0){
			addr += wrlen;
			size -= wrlen;
		}else{
			ret = -1;
			break;
		}
	}

	close(fd);

	return ret;
}

int write_file_to_flash_with_fn(u_int addr, u_char *data_prefix,char *file, void (*func)(double , void *), void *arg)
{
	char wrbuf[4096];
	int filelen,len = 0, offset = 0;
	int i=0, fd;
	
	fd = open(file, O_RDONLY);
	if(fd<0){
		printf("write_file_to_flash : open %s failed \n", file);
		return -1;
	}

	filelen = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	
	printf("write file %s %d bytes to nor flash %#x \n", file, filelen, addr);
		
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
		if(write_flash(addr, wrbuf, len) < 0){
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
		if(write_flash(addr+offset, wrbuf, len) < 0){
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
int write_file_to_flash(u_int addr, u_char *data_prefix,char *file)
{
	char wrbuf[4096];
	int len = 0, offset = 0;
	int i=0, fd;
	
	

	fd = open(file, O_RDONLY);
	if(fd<0){
		printf("write_file_to_flash : open %s failed \n", file);
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
			
		len = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);

		printf("write file %s %d bytes to nor flash %#x \n", file, len, addr);

		memcpy(&wrbuf[4], &len, 4);
		len = read(fd, &wrbuf[8], 4096-8) + 8;
		if(write_flash(addr, wrbuf, len) < 0){
			close(fd);
			return -1;
		}
		printchar('.');
		
		offset = len;	
	}else{
		printf("write file %s to nor flash %#x \n", file, addr);
		offset = 0;
	}

	do{
		len = read(fd, wrbuf, 4096);
		//print_bytes(wrbuf, len);
		if(write_flash(addr+offset, wrbuf, len) < 0){
			break;
		}
		printchar('.');
		offset += len;
	}while(len>0);

	close(fd);
	
	printf("ok\n");
	
	return 0;
}



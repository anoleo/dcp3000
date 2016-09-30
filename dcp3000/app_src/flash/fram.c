#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fram.h"

//#include "../utils/utils.h"

#define FM31256_FRAM_READ		0x3101	/* read FM31256 FRAM */
#define FM31256_FRAM_WRITE	0x3102	/* write FM31256 FRAM */

#define FRAM_DEVICE	"/dev/misc/fram"

static int fram_fd;

struct io_args{
	int offset;
	int len;
	int ret;
	void *data;
};

int open_fram(void)
{
	int fpga_phy_addr;
	fram_fd = open(FRAM_DEVICE, O_RDWR);

	if(fram_fd < 0)
	{
		printf("***Can't open the fram driver handle!\r\n");
		return -1;
	}
	
	return 0;
}

void close_fram(void)
{

	close(fram_fd);
	
}

int read_fram(unsigned  int addr, void *buffer, int len)
{	
	struct io_args arg;


	arg.offset = addr;
	arg.data = buffer;
	arg.len = len;

	ioctl(fram_fd, FM31256_FRAM_READ, &arg);

	printf("read_fram addr %#x, len %d, ret = %#x \n", addr, len, arg.ret);

	return arg.ret;
}

int write_fram(unsigned int addr, void *buffer, int len)
{	
	struct io_args arg;


	arg.offset = addr;
	arg.data = buffer;
	arg.len = len;

	ioctl(fram_fd, FM31256_FRAM_WRITE, &arg);

	printf("write_fram addr %#x, len %d, ret = %#x \n", addr, len, arg.ret);

	return arg.ret;
}



//#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>


#include "gpio.h"


#define MY_CMD_MAGIC			0xf1

#define AM335X_GPIO_INIT			_IOW(MY_CMD_MAGIC, 1, int)
#define AM335X_GPIO_FREE			_IOW(MY_CMD_MAGIC, 2, int)
#define AM335X_GPIO_INPUT			_IOW(MY_CMD_MAGIC, 3, int)
#define AM335X_GPIO_OUTPUT0			_IOW(MY_CMD_MAGIC, 4, int)
#define AM335X_GPIO_OUTPUT1			_IOW(MY_CMD_MAGIC, 5, int)
#define AM335X_GPIO_SET			_IOW(MY_CMD_MAGIC, 6, int)
#define AM335X_GPIO_CLEAR			_IOW(MY_CMD_MAGIC, 7, int)
#define AM335X_GPIO_GET			_IOW(MY_CMD_MAGIC, 8, int)



#define GPIO_DEVICE			"/dev/misc/armgpio"

typedef struct{
	int idx;
	char *name;
}gpio_st;

static int gpio_fd = 0;


int gpio_get(int index)
{
	int value;
	
	value = index;
	ioctl(gpio_fd, AM335X_GPIO_GET, &value);
	//ioctl(gpio_fd, TEST_GET_GPIO_VALUE, &value);
	//printf("Get %d GPIO value %d\n", index, value);

	return (value?1:0);
}

void gpio_set(int index)
{

	ioctl(gpio_fd, AM335X_GPIO_SET, &index);
	//ioctl(gpio_fd, TEST_GPIO_SET, &index);
	//printf("Set %d GPIO \n", index);

}

void gpio_clean(int index)
{

	ioctl(gpio_fd, AM335X_GPIO_CLEAR, &index);
	//ioctl(gpio_fd, TEST_GPIO_CLEAR, &index);
	//printf("Clean %d GPIO \n", index);
}


void gpio_init(int index, char *name)
{
	gpio_st gpio_args;
	
	gpio_args.idx = index;
	gpio_args.name = name;
	
	ioctl(gpio_fd, AM335X_GPIO_INIT, &gpio_args);
	printf("init %d GPIO : name %s \n", index, name);
}


void gpio_free(int index)
{

	ioctl(gpio_fd, AM335X_GPIO_FREE, &index);
	printf("free %d GPIO \n", index);

}

void gpio_output0(int index)
{
	ioctl(gpio_fd, AM335X_GPIO_OUTPUT0, &index);
	printf("set %d GPIO to output mode, default pull down\n", index);
}

void gpio_output1(int index)
{
	ioctl(gpio_fd, AM335X_GPIO_OUTPUT1, &index);
	printf("set %d GPIO to output mode, default pull up\n", index);
}

void gpio_input(int index)
{
	ioctl(gpio_fd, AM335X_GPIO_INPUT, &index);
	printf("set %d GPIO to input mode\n", index);
}

int open_gpio(void)
{

	gpio_fd = open(GPIO_DEVICE, O_RDWR);

	if(gpio_fd < 0){
		printf("***Can't open the gpio driver handle!\r\n");
		return -1;
	}

	return 0;
}


void close_gpio(void)
{
	
	close(gpio_fd);
}


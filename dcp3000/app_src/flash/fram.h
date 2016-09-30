#ifndef __FRAM_H__
#define __FRAM_H__

#include "sys/types.h"



#define FRAM_SIZE 	0x8000	// 32K


int open_fram(void);
void close_fram(void);
	
int read_fram(unsigned  int addr, void *buffer, int len);
int write_fram(unsigned int addr, void *buffer, int len);


#endif //__FLASH_H__

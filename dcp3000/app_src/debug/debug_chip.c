#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../debug.h"

#include "../gpio.h"
#include "../fpga.h"
#include "../lm73.h"

#include "../flash/mtd.h"
#include "../flash/flash.h"
#include "../flash/fram.h"

//实验证明通过文件系统和mtd 访问flash 要比直接通过驱动访问效率要高
int debug_flash(int argc, char *argv[])
{
	int i, count = 1;
	u_int addr = 0;
	int len, sector = 0;
	u_short wdata;
	char data_buf[0x10000];
	char *filename = NULL, *prefix = NULL;
	struct timeval t_start; 


	//gettimeofday(&t_start, NULL); 
	if(strcmp("r", argv[1]) == 0){

		if(argc<4){return -1;}

		addr = strtoul(argv[2], NULL, 16);
		len = atoi(argv[3]);
		
		read_flash(addr, data_buf, len);
		print_bytes(data_buf, len);
		
	}else if(strcmp("e", argv[1]) == 0){

		if(argc<3){return -1;}
		
		sector = atoi(argv[2]);
		if(argc>3)
			count = atoi(argv[3]);

		erase_flash(sector, count);

	}else if(strcmp("w", argv[1]) == 0){

		if(argc<4){return -1;}

		addr = strtoul(argv[2], NULL, 16);
		wdata = strtoul(argv[3], NULL, 16);
		
		writew_flash(addr, wdata);
		
	}else if(strcmp("cp", argv[1]) == 0){

		if(argc<4){return -1;}

		addr = strtoul(argv[2], NULL, 16);

		if(argc>4){
			prefix = argv[4];
		}
		
		write_file_to_flash(addr, prefix, argv[3]);

	}else if(strcmp("prot", argv[1]) == 0){

		if(argc<3){return -1;}
		
	 	sector = atoi(argv[2]);
		if(argc>3)
			count = atoi(argv[3]);
		
		protect_flash(sector, count);

	}else if(strcmp("unprot", argv[1]) == 0){
	 
		if(argc<3){return -1;}

		sector = atoi(argv[2]);
		if(argc>3)
			count = atoi(argv[3]);

		unprotect_flash(sector, count);

	}else if(strcmp("ppbs", argv[1]) == 0){

	 	if(argc<3){return -1;}

		sector = atoi(argv[2]);
		if(argc>3)
			count = atoi(argv[3]);

		read_flash_ppbs(sector, count);

	}else if(strcmp("mkimg", argv[1]) == 0){
	 
		
		
	 	if(argc>2)
			addr = strtol(argv[2], NULL, 16);
		
	 	if(argc>3)
			len = strtol(argv[3], NULL, 16);
		else
			len = 0x1000000;

		if(argc>4)
			filename = argv[4];
		else
			filename = "/var/ftp/pub/norflash.img";
		
		read_flash_to_file(filename, addr, len);
	
	}
	
	//show_exectime(&t_start, "flash operation");


	 return 0;
	
}

int debug_fpga( int argc, char *argv[ ] )
{
	u_int reg, page = 0, addr, value = 0;
	int i, len, size = 4, v_s32 =  0;
	struct timeval btime;

	short data_buf[0x100000];
	
	if(argc < 2){
		return 0;
	}

	if(strcmp("load", argv[1]) == 0){
		//gettimeofday(&btime, NULL);
		if(argc>2){
			//Config_FPGA(argv[2]);
		}else{
			//Config_FPGA(NULL);
		}
		//show_exectime(&btime, "download fpga");
	}else if(strcmp("owner", argv[1]) == 0){
		FPGA_Get_Owner();
	}else if(strcmp("reset", argv[1]) == 0){
		FPGA_Reset();
	}else if(strcmp("irq", argv[1]) == 0){
		if(argc>2){
			v_s32 = atoi(argv[2]);
		}
		FPGA_Enable_IRQ(v_s32);
	}else if(strcmp("r", argv[1]) == 0){

		if(argc>2){
			reg = strtoul(argv[2], NULL, 16);
		}

		if(argc>3){
			size = strtoul(argv[3], NULL, 16);
		}

		
		page = reg/FPGA_PAGE_SIZE;
		addr = reg%FPGA_PAGE_SIZE;

		if(page>0){
			FPGA_Set_Page(page);
			if(size == 2){
				value = FPGA_Readw_DDR(addr);
			}else if(size == 4){
				value = FPGA_Readl_DDR(addr);
			}	
			FPGA_End_Page(page);
		}else{
			if(size == 1){
				value = FPGA_Readb(reg);
			}else if(size == 2){
				value = FPGA_Readw(reg);
			}else if(size == 4){
				value = FPGA_Readl(reg);
			}
		}
		
		printf("read fpga %#x byte, %#x addr[ %d page, %#x offset ], value = %#x \n", size, reg, page, addr, value);
		
	}
	else if(strcmp("w", argv[1]) == 0){

		if(argc>2){
			reg = strtoul(argv[2], NULL, 16);
		}
		
		if(argc>3){
			value = strtoul(argv[3], NULL, 16);
		}

		if(argc>4){
			size = strtoul(argv[4], NULL, 16);
		}

		page = reg/FPGA_PAGE_SIZE;
		addr = reg%FPGA_PAGE_SIZE;

		if(page>0){
			FPGA_Set_Page(page);
			if(size == 2){
				FPGA_Writew_DDR(addr, value);
			}else if(size == 4){
				FPGA_Writel_DDR(addr, value);
			}	
			FPGA_End_Page(page);
		}else{
			if(size == 1){
				FPGA_Writeb(reg, value);
			}else if(size == 2){
				FPGA_Writew(reg, value);
			}else if(size == 4){
				FPGA_Writel(reg, value);
			}
		}

		printf("write fpga %#x byte, %#x addr[ %d page, %#x offset ], value = %#x \n", size, reg, page, addr, value);
		
	}
	else if(strcmp("rf", argv[1]) == 0){

		if(argc>2){
			reg = strtoul(argv[2], NULL, 16);
		}

		if(argc>3){
			size = strtoul(argv[3], NULL, 16);
		}
		
		if(size == 1){
			value = FPGA_Readb(reg);
		}else if(size == 2){
			value = FPGA_Readw(reg);
		}else if(size == 4){
			value = FPGA_Readl(reg);
		}
		
		printf("read fpga %#x byte, %#x addr, value = %#x \n", size, reg, value);
		
	}
	else if(strcmp("wf", argv[1]) == 0){

		if(argc>2){
			reg = strtoul(argv[2], NULL, 16);
		}
		
		if(argc>3){
			value = strtoul(argv[3], NULL, 16);
		}

		if(argc>4){
			size = strtoul(argv[4], NULL, 16);
		}

		if(size == 1){
			FPGA_Writeb(reg, value);
		}else if(size == 2){
			FPGA_Writew(reg, value);
		}else if(size == 4){
			FPGA_Writel(reg, value);
		}

		printf("write fpga %#x byte, %#x addr, value = %#x \n", size, reg, value);
		
	}


	return 0;
}


int debug_gpio(int argc, char *argv[])
{
	int idx, value = 0;
		
	if(argc < 2){
		return 0;
	}


	if(strcmp("open", argv[1]) == 0){
		open_gpio();
	}else if(strcmp("close", argv[1]) == 0){
		close_gpio();
	}else if(strcmp("init", argv[1]) == 0){
		if(argc < 4){	return 0;}
		idx = atoi(argv[2]);
		gpio_init(idx, argv[3]);
	}else if(strcmp("free", argv[1]) == 0){
		if(argc < 3){	return 0;}	
		idx = atoi(argv[2]);
		gpio_free(idx);
	}else if(strcmp("dir", argv[1]) == 0){
		if(argc < 4){	return 0;}	
		idx = atoi(argv[2]);
		if(strcmp("in", argv[3]) == 0){
			gpio_input(idx);
		}else if(strcmp("out", argv[3]) == 0){
			if(argc>4)
				value = atoi(argv[4]);
			if(value)
				gpio_output1(idx);
			else
				gpio_output0(idx);
		}
	}else if(strcmp("get", argv[1]) == 0){
		if(argc < 3){	return 0;}	
		idx = atoi(argv[2]);
		value = gpio_get(idx);
		printf("gpio [%d] input %d \n", idx, value);
	}else if(strcmp("set", argv[1]) == 0){
		if(argc < 3){	return 0;}	
		idx = atoi(argv[2]);
		gpio_set(idx);
		printf("gpio [%d] set \n", idx);
	}else if(strcmp("clean", argv[1]) == 0){
		if(argc < 3){	return 0;}	
		idx = atoi(argv[2]);
		gpio_clean(idx);
		printf("gpio [%d] clean\n", idx);
	}

}


int fram_write_file(unsigned int addr, char *file)
{
	char wrbuf[1024];
	int len, offset = 0;
	int fd;

	printf("fram write file %s to %#x \n", file, addr);

	fd = open(file, O_RDONLY);
	if(fd<0){
		printf("fram_write_file : open %s failed \n", file);
		return -1;
	}

	do{
		len = read(fd, wrbuf, 1024);
		//print_bytes(wrbuf, len);
		if(write_fram(addr+offset, wrbuf, len) < 0){
			break;
		}
		offset += len;
	}while(len>0);

	close(fd);

	return 0;
}

int debug_fram(int argc, char *argv[])
{
	int i;
	int addr, len, sector;
	u_short wdata;
	char data_buf[8192];

	if(strcmp("r", argv[1]) == 0){
		
	 	if(argc<4){return -1;}
		
		addr = strtol(argv[2], NULL, 16);
		len = atoi(argv[3]);
		
		read_fram(addr, data_buf, len);
		print_bytes(data_buf, len);
	
	}else if(strcmp("w", argv[1]) == 0){
	
		if(argc<4){return -1;}
		
	 	addr = strtol(argv[2], NULL, 16);
		wdata = strtol(argv[3], NULL, 16);
		
		write_fram(addr, &wdata, 2);
	}else if(strcmp("e", argv[1]) == 0){
	
		if(argc<4){return -1;}
		
	 	addr = strtol(argv[2], NULL, 16);
		len = atoi(argv[3]);
		
		memcpy(data_buf, 0, len);
		
		write_fram(addr, data_buf, len);
	}else if(strcmp("cp", argv[1]) == 0){
	
		if(argc<3){return -1;}
		
	 	addr = strtol(argv[2], NULL, 16);
		
		fram_write_file(addr, argv[3]);
	}


	return 0;
	
}


int debug_lm73( int argc, char *argv[ ] )
{
	int  value = 0;
	int reg;
	int idx;
	
	if(argc < 3){
		return 0;
	}

	idx = atoi(argv[2]);
	
	if(strcmp("rdtemp", argv[1]) == 0){
		value = lm73_read_temprature(idx);
		printf("lm73_read_temprature : %#x, %d \n", value, value/4);
	}else if(strcmp("rdthigh", argv[1]) == 0){
		value = lm73_read_t_high(idx);
		printf("lm73_read_t_high : %#x, %d \n", value, value/4);
	}else if(strcmp("rdtlow", argv[1]) == 0){
		value = lm73_read_t_low(idx);
		printf("lm73_read_t_low : %#x, %d \n", value, value/4);
	}else if(strcmp("rdcfg", argv[1]) == 0){
		value = lm73_read_configuration(idx);
		printf("lm73_read_configuration : %#x,\n", value);
	}else if(strcmp("rdstatus", argv[1]) == 0){
		value = lm73_read_status(idx);
		printf("lm73_read_status : %#x,\n", value);
	}else if(strcmp("setcfg", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_set_configuration(idx, strtol(argv[3], NULL, 16));
	}else if(strcmp("setctrl", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_set_control(idx, strtol(argv[3], NULL, 16));
	}else if(strcmp("setmode", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_set_mode(idx, atoi(argv[3]));
	}else if(strcmp("reset", argv[1]) == 0){
		lm73_alert_reset(idx);
	}else if(strcmp("polarity", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_alert_polarity(idx, atoi(argv[3]));
	}else if(strcmp("alert", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_alert_enable(idx, atoi(argv[3]));
	}else if(strcmp("timeout", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_TimeOut_Disable(idx, atoi(argv[3]));
	}else if(strcmp("setresol", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_set_resolution(idx, atoi(argv[3]));
	}else if(strcmp("setthigh", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_set_t_high(idx, atoi(argv[3]));
	}else if(strcmp("settlow", argv[1]) == 0){
		if(argc< 4){return -1;	}
		lm73_set_t_low(idx, atoi(argv[3]));
	}


	return 0;
}


//实验证明通过文件系统和mtd 访问flash 要比直接通过驱动访问效率要高
int debug_mtd(int argc, char *argv[])
{
	int i = 0, mtd_fd = 0;
	u_int addr = 0, len = 0;
	u_short wdata;
	char data_buf[0x10000];
	char *filename = NULL, *prefix = NULL;

	struct timeval t_start; 

	if(argc<3){
		return -1;
	}

	mtd_fd = mtd_open(argv[2]);

	//gettimeofday(&t_start, NULL); 
	
	  if(strcmp("info", argv[1]) == 0){

		mtd_get_info(mtd_fd);
		
	 }else if(strcmp("region", argv[1]) == 0){
	 	if(argc>3)
		i = strtoul(argv[3], NULL, 16);
		mtd_get_region_info(mtd_fd, i);
		
	 }else if(strcmp("r", argv[1]) == 0){

		if(argc<5){goto End_debug;}

		addr = strtoul(argv[3], NULL, 16);
		len = strtoul(argv[4], NULL, 16);
		
		mtd_read(mtd_fd, addr, data_buf, len);
		print_bytes(data_buf, len);
		
	 }else if(strcmp("e", argv[1]) == 0){

		if(argc<4){goto End_debug;}
		
		addr = strtoul(argv[3], NULL, 16);
		if(argc>4)
			len = strtoul(argv[4], NULL, 16);

		mtd_erase(mtd_fd, addr, len);

	 }else if(strcmp("w", argv[1]) == 0){

		if(argc<4){goto End_debug;}

		addr = strtoul(argv[3], NULL, 16);
		wdata = strtoul(argv[4], NULL, 16);
		
		mtd_write(mtd_fd, addr, wdata, 2);
		
	 }else if(strcmp("cp", argv[1]) == 0){

		if(argc<4){goto End_debug;}

		addr = strtoul(argv[3], NULL, 16);

		if(argc>5){
			prefix = argv[5];
		}
		
		mtd_write_file(mtd_fd, addr, prefix, argv[4]);

	 }
	
	//show_exectime(&t_start, "flash operation");

End_debug:
	
	close(mtd_fd);

	 return 0;
	
}


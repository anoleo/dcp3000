
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "spi_xyf.h"


#ifndef __ALT_TYPES_H__
typedef signed char  alt_8;
typedef unsigned char  alt_u8;
typedef signed short alt_16;
typedef unsigned short alt_u16;
typedef signed long alt_32;
typedef unsigned long alt_u32;
typedef long long alt_64;
typedef unsigned long long alt_u64;
#endif

#ifndef	_SYS_TYPES_H
typedef alt_u32 u_int;
#endif



/*******************************************************************/
#define SPI_XYF_STATUS			0
#define SPI_XYF_CONTROL			1
#define SPI_XYF_ADDRESS			2
#define SPI_XYF_LENGTH			3
#define SPI_XYF_WDATA			4
#define SPI_XYF_RDATA			5
#define SPI_XYF_R_CHS_OFFSET	6
#define SPI_XYF_CHECKSUM		7

//==============SPI_XYF_STATUS===========================
#define SPI_XYF_STATUS_LINK			0x10
#define SPI_XYF_STATUS_OVER			0x08
#define SPI_XYF_STATUS_ERROR		0x04
#define SPI_XYF_STATUS_BUSY			0x02
#define SPI_XYF_STATUS_TMT			0x01

//==============SPI_XYF_CONTROL=========================
#define SPI_XYF_CONTROL_RESET		0x02
#define SPI_XYF_CONTROL_SEND		0x01

//==============SPI_XYF_LENGTH=========================
#define SPI_XYF_LENGTH_RW			0x8000
#define SPI_XYF_LENGTH_SM			0x4000
#define SPI_XYF_LENGTH_LEN			0x3fff

//=========================================
#define SPI_XYF_W_FLAG				0x8000
#define SPI_XYF_R_FLAG				0x0000

#define SPI_XYF_M_FLAG				0x4000
#define SPI_XYF_S_FLAG				0x0000

#define SPI_XYF_SEND_FLAG			0x0001
#define SPI_XYF_RESET_FLAG			0x0002

/*******************************************************************/



#if		SPI_XYF_DEBUG
#define SPI_XYF_DEBUG_PRINT(fmt,...)			printf(fmt ,__VA_ARGS__)
#define SPI_XYF_DEBUG_N_PRINT(fmt)				printf(fmt)
#else
#define SPI_XYF_DEBUG_PRINT(fmt,...)
#define SPI_XYF_DEBUG_N_PRINT(fmt)
#endif


#if		SPI_XYF_DATA_DEBUG
#define SPI_XYF_D_DEBUG_PRINT(fmt,...) printf(fmt ,__VA_ARGS__)
#define SPI_XYF_D_DEBUG_N_PRINT(fmt) printf(fmt)
#else
#define SPI_XYF_D_DEBUG_PRINT(fmt,...)
#define SPI_XYF_D_DEBUG_N_PRINT(fmt)
#endif

int is_spi_running(int slot);

static int spi_reset(u_int dev_base);
static int spi_check_busy(u_int dev_base,char * string);

int spi_master_write(u_int dev_base, u_int spi_addr, void *wr_data, u_int wr_len){
	int i, slot;
	alt_u32 resend_cnt = 0;
	alt_u32 data_size =0;
	alt_u32 busy_cnt =0;
	alt_u32 status = 0;
	alt_u32 c_checksum = 0;
	alt_u32 fpga_checksum = 0;
	alt_u32 offset = 0;
	alt_u32 crc32;
	alt_u16 temp_data = 0;
	alt_u16 length = (wr_len%2)?(wr_len/2 + 1):(wr_len/2);
	alt_u8 wr_buf[SPI_XYF_BUF_SIZE*2];

	slot = (dev_base-REG_FPGA_SPI_MASTER_BASE)/0x40;
	
	data_size = sizeof(temp_data);
	
#if SPI_XYF_BUSY_CLEAR
	if(spi_reset(dev_base)==SPI_XYF_RESET_FAIL){
		return SPI_XYF_RESET_FAIL;
	}
#endif
	SPI_XYF_DEBUG_N_PRINT("=================master write begin =====================\n");

	SPI_XYF_DEBUG_PRINT("dev base=0x%0.8x,spi_addr=0x%0.8x,wr_len=%d\n",dev_base,spi_addr,wr_len);


	if(length==1){
		do {
			if(!is_spi_running(slot)){
				return SPI_XYF_LINK_ERROR;
			}
			
			resend_cnt++;
			IOWR_SPI_XYF_32DIRECT(dev_base, SPI_XYF_ADDRESS, spi_addr);
			IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_LENGTH, (SPI_XYF_W_FLAG|SPI_XYF_S_FLAG|1));
			memcpy(&temp_data, wr_data, data_size);
			SPI_XYF_D_DEBUG_PRINT("-------------------send data begin add=0x%0.8x cnt=%d-------------------\n",spi_addr,resend_cnt);
			SPI_XYF_D_DEBUG_PRINT("0x%0.4x\n",temp_data);
			SPI_XYF_D_DEBUG_N_PRINT("-------------------send data end-------------------\n");
			IOWR_SPI_XYF_16DIRECT(dev_base,SPI_XYF_WDATA, temp_data);
			fpga_checksum = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_CHECKSUM);
			SPI_XYF_DEBUG_PRINT("w fpga_checksum=%0.4x\n", fpga_checksum);
		} while ((fpga_checksum ^ temp_data) != 0);

		IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_CONTROL, SPI_XYF_CONTROL_SEND);

		busy_cnt = spi_check_busy(dev_base,"sw");
		#if SPI_XYF_BUSY_CLEAR
		if(busy_cnt==SPI_XYF_SEND_FAIL){
			SPI_XYF_DEBUG_N_PRINT("send fail!!!\n");
			return SPI_XYF_SEND_FAIL;
		}
		#endif
		SPI_XYF_DEBUG_N_PRINT("\r\n");
	}
	else if(length>1){
		
		wr_buf[0] = 0x9d;
		wr_buf[1] = 0x91;
		wr_buf[2] = 0xb0;
		wr_buf[3] = 0xba;

		memcpy(&wr_buf[4], wr_data, wr_len);

		length += 2;
		
		wr_len = length<<1;
		crc32 = calc_crc32(wr_buf, wr_len);
		wr_buf[wr_len++] = (crc32 >> 24) & 0xff;
		wr_buf[wr_len++] = (crc32 >> 16) & 0xff;
		wr_buf[wr_len++] = (crc32 >> 8) & 0xff;
		wr_buf[wr_len++] = (crc32 >> 0) & 0xff;

		length = wr_len>>1;
		
		do {

			if(!is_spi_running(slot)){
				return SPI_XYF_LINK_ERROR;
			}
			
			resend_cnt++;
			c_checksum = 0;
			IOWR_SPI_XYF_32DIRECT(dev_base,SPI_XYF_ADDRESS, spi_addr);
			IOWR_SPI_XYF_16DIRECT(dev_base,SPI_XYF_LENGTH, (SPI_XYF_W_FLAG|SPI_XYF_M_FLAG|length));
			SPI_XYF_D_DEBUG_PRINT("-------------------send data begin add=0x%0.8x cnt=%d-------------------\n",spi_addr,resend_cnt);
			for (i = 0; i < length; i++) {

				if(!is_spi_running(slot)){
					return SPI_XYF_LINK_ERROR;
				}
				memcpy(&temp_data, &wr_buf[i*data_size], data_size);
				IOWR_SPI_XYF_16DIRECT(dev_base,SPI_XYF_WDATA, temp_data);
				SPI_XYF_D_DEBUG_PRINT("0x%0.4x,",temp_data);
				c_checksum += temp_data;
				while (c_checksum >> 16) {
					c_checksum = (c_checksum & 0xffff) + (c_checksum >> 16);
//					SPI_XYF_DEBUG_PRINT("#%d:checksum = %x\n",i,c_checksum);
				}
#if SPI_XYF_DATA_DEBUG
				if ((i + 1) % 20 == 0) {
					SPI_XYF_D_DEBUG_N_PRINT("\r\n");
				}
#endif
			}
			SPI_XYF_D_DEBUG_N_PRINT("\r\n");
			SPI_XYF_D_DEBUG_N_PRINT("-------------------send data end-------------------\n");
			
			offset = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_R_CHS_OFFSET);
			SPI_XYF_DEBUG_PRINT("w offset=%d,", offset);
			SPI_XYF_DEBUG_PRINT("w c_checksum=%0.4x,", c_checksum);
			fpga_checksum = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_CHECKSUM);
			SPI_XYF_DEBUG_PRINT("w fpga_checksum=%0.4x\n", fpga_checksum);

		} while ((offset != length) || ((c_checksum ^ fpga_checksum) != 0));

		
		do{
			if(!is_spi_running(slot)){
				return SPI_XYF_LINK_ERROR;
			}
			IOWR_SPI_XYF_16DIRECT(dev_base,SPI_XYF_STATUS, 0);
			IOWR_SPI_XYF_16DIRECT(dev_base,SPI_XYF_CONTROL, SPI_XYF_CONTROL_SEND);
			status = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_STATUS);
			SPI_XYF_DEBUG_PRINT("status = %x\n",status);
		}while(status & SPI_XYF_STATUS_ERROR);

		busy_cnt = spi_check_busy(dev_base,"mw");
		#if SPI_XYF_BUSY_CLEAR
		if(busy_cnt==SPI_XYF_SEND_FAIL){
			SPI_XYF_DEBUG_N_PRINT("send fail!!!\n");
			return SPI_XYF_SEND_FAIL;
		}
		#endif
		SPI_XYF_DEBUG_N_PRINT("\r\n");
	}
	else{
		SPI_XYF_DEBUG_N_PRINT("you write length is 0\n");
	}

	SPI_XYF_DEBUG_N_PRINT("=================master write end =====================\n");


	return resend_cnt;
}

int spi_master_read(u_int dev_base, u_int spi_addr, void *rd_data, u_int rd_len){
	int i;
	alt_u32 check_over = 1;
	alt_u32 data_size =0;
	alt_u32 resend_cnt = 0;
	alt_u32 busy_cnt =0;
	alt_u32 status = 0;
	alt_u32 c_checksum = 0;
	alt_u32 fpga_checksum = 0;
	alt_u32 offset = 0;
	alt_u16 temp_data = 0;
	alt_u16 length;
	alt_u16 length_flag = 0;
	char * rstr_1;
	alt_u8 rd_buf[SPI_XYF_BUF_SIZE*2];
	int slot;

	slot = (dev_base-REG_FPGA_SPI_MASTER_BASE)/0x40;

	length = (rd_len%2)?(rd_len/2 + 1):(rd_len/2);

	if(length==1){
		rstr_1 = "sr 1";
		length_flag = SPI_XYF_R_FLAG|SPI_XYF_S_FLAG|1;
	}else{
		length += 4;
		rstr_1 = "mr 1";
		length_flag = SPI_XYF_R_FLAG|SPI_XYF_M_FLAG|length;
	}
	

	data_size = sizeof(temp_data);
	
#if SPI_XYF_BUSY_CLEAR
	if(spi_reset(dev_base)==SPI_XYF_RESET_FAIL){
		return SPI_XYF_RESET_FAIL;
	}
#endif
		
	SPI_XYF_DEBUG_N_PRINT("=================master read begin =====================\n");
	SPI_XYF_DEBUG_PRINT("dev base=0x%0.8x,spi_addr=0x%0.8x,rd_len=%d\n",dev_base,spi_addr,rd_len);
	if(length>0){
		
		do {
			
			if(resend_cnt > 0){
				if(spi_reset(dev_base)==SPI_XYF_RESET_FAIL){
					return SPI_XYF_RESET_FAIL;
				}
				if(resend_cnt > SPI_XYF_BUSY_CNT){
					IOWR_SPI_XYF_16DIRECT(dev_base,SPI_XYF_STATUS, 0);
					return SPI_XYF_RESET_FAIL;
				}
			}

			SPI_XYF_DEBUG_N_PRINT("first read send\n");
			IOWR_SPI_XYF_32DIRECT(dev_base, SPI_XYF_ADDRESS, spi_addr);
			IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_LENGTH, length_flag);
			IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_CONTROL, SPI_XYF_CONTROL_SEND);

			check_over = 1;


			do {
				busy_cnt = spi_check_busy(dev_base,rstr_1);
				if(busy_cnt == SPI_XYF_SEND_FAIL){
					spi_reset(dev_base);
					return SPI_XYF_RESET_FAIL;
				}
				//SPI_XYF_DEBUG_N_PRINT("\n");
				status = IORD_SPI_XYF_16DIRECT(dev_base, SPI_XYF_STATUS);
				if (((status & SPI_XYF_STATUS_OVER) == SPI_XYF_STATUS_OVER) & check_over) {
					if ((status & ( SPI_XYF_STATUS_ERROR)) == (SPI_XYF_STATUS_ERROR)) {
						if(spi_reset(dev_base)==SPI_XYF_SEND_FAIL){
							return SPI_XYF_RESET_FAIL;
						}
						IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_LENGTH, length_flag);
						IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_CONTROL, SPI_XYF_CONTROL_SEND);
						check_over = 1;
					}else{
						IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_STATUS, 0);
						IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_CONTROL, SPI_XYF_CONTROL_SEND);
						status = IORD_SPI_XYF_16DIRECT(dev_base, SPI_XYF_STATUS);
						check_over = 0;
						SPI_XYF_DEBUG_PRINT("end over,status = %x,check = %d\n", status, check_over);
					}
				}
			} while ((status & SPI_XYF_STATUS_BUSY) || check_over);

			
			c_checksum = 0;
			resend_cnt++;
			IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_R_CHS_OFFSET, 0);
			SPI_XYF_D_DEBUG_PRINT("-------------------receive data begin add=0x%0.8x cnt=%d-------------------\n",spi_addr,resend_cnt);

			for (i = 0; i < length; i++) {
				
				if(!is_spi_running(slot)){
					return SPI_XYF_LINK_ERROR;
				}
				temp_data = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_RDATA);

				memcpy(&rd_buf[i*data_size], &temp_data, data_size);
				SPI_XYF_D_DEBUG_PRINT("0x%0.4x,",temp_data);
				c_checksum += temp_data;
				
				while (c_checksum >> 16) {
					c_checksum = (c_checksum & 0xffff) + (c_checksum >> 16);
				}

#if SPI_XYF_DATA_DEBUG
				if ((i + 1) % 20 == 0) {
					SPI_XYF_D_DEBUG_N_PRINT("\r\n");
				}
#endif
			}
			SPI_XYF_D_DEBUG_N_PRINT("\r\n");
			SPI_XYF_D_DEBUG_N_PRINT("-------------------receive data end-------------------\n");
			fpga_checksum = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_CHECKSUM);

			if((c_checksum ^ fpga_checksum) != 0){
				SPI_XYF_DEBUG_PRINT("fpga_checksum = %x, c_checksum = %x\n", fpga_checksum, c_checksum);
				resend_cnt = SPI_XYF_BUSY_CNT + 1;
				continue;
			}

			//*
			if((rd_buf[0] != 0x9d) || (rd_buf[1] != 0x91) || (rd_buf[2] != 0xb0) || (rd_buf[3] != 0xba)){
				SPI_XYF_DEBUG_PRINT("the prefix of spi rcv data is error [%#.2x %#.2x %#.2x %#.2x]\n", rd_buf[0], rd_buf[1], rd_buf[2], rd_buf[3]);
				resend_cnt = SPI_XYF_BUSY_CNT + 1;
				continue;
			}

			if(calc_crc32(rd_buf, length<<1)!=0){
				SPI_XYF_DEBUG_N_PRINT("spi rcv data crc32 error\n");
				resend_cnt = SPI_XYF_BUSY_CNT + 1;
				continue;
			}else{		
				memcpy(rd_data, &rd_buf[4], rd_len);
				break;
			}

			//*/

		} while (1);
		
		IOWR_SPI_XYF_16DIRECT(dev_base,SPI_XYF_STATUS, 0);
		

	}
	else{
		SPI_XYF_DEBUG_N_PRINT("you read length is 0\n");
	}
	SPI_XYF_DEBUG_N_PRINT("=================master read end =====================\n");
#if SPI_XYF_BUSY_CLEAR
	if(busy_cnt==SPI_XYF_SEND_FAIL){
		SPI_XYF_DEBUG_N_PRINT("send fail!!!\n");
		return SPI_XYF_SEND_FAIL;
	}
#endif

	return resend_cnt;
}

static int spi_reset(u_int dev_base){
	alt_u32 status=0;
	int reset_cnt=0;
	int slot;

	slot = (dev_base-REG_FPGA_SPI_MASTER_BASE)/0x40;

	//printf("%d spi reset \n", slot);

	IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_CONTROL, (SPI_XYF_CONTROL_SEND|SPI_XYF_CONTROL_RESET));
	do{
		if(!is_spi_running(slot)){
			return SPI_XYF_RESET_FAIL;
		}
		
		if(status & SPI_XYF_STATUS_LINK){
			IOWR_SPI_XYF_16DIRECT(dev_base, SPI_XYF_CONTROL, (SPI_XYF_CONTROL_SEND|SPI_XYF_CONTROL_RESET));
		}
		status = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_STATUS);
		++reset_cnt;
		SPI_XYF_D_DEBUG_PRINT("reset_cnt = %d",reset_cnt);
#if SPI_XYF_BUSY_CLEAR
		if(reset_cnt>SPI_XYF_BUSY_CNT){
			return SPI_XYF_RESET_FAIL;
		}
		usleep(10);
#endif
	}while((status & ( SPI_XYF_STATUS_TMT | SPI_XYF_STATUS_BUSY)) != SPI_XYF_STATUS_TMT);

	return 0;
}

static int spi_check_busy(u_int dev_base,char * string){
	int busy_cnt=0;
	alt_u32 status=0;
	int slot;

	slot = (dev_base-REG_FPGA_SPI_MASTER_BASE)/0x40;

	do{

		if(!is_spi_running(slot)){
			return SPI_XYF_SEND_FAIL;
		}
		
		status = IORD_SPI_XYF_16DIRECT(dev_base,SPI_XYF_STATUS);
		++busy_cnt;
		SPI_XYF_D_DEBUG_PRINT("%s busy_cnt = %d",string,busy_cnt);
#if SPI_XYF_BUSY_CLEAR
		if(busy_cnt>SPI_XYF_BUSY_CNT){
			return SPI_XYF_SEND_FAIL;
		}
		usleep(10);
#endif
	}while((status & (SPI_XYF_STATUS_TMT | SPI_XYF_STATUS_BUSY)) != SPI_XYF_STATUS_TMT);

	return busy_cnt;
}
	

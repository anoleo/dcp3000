#ifndef	__SPI_XYF_H__
#define	__SPI_XYF_H__

/***************head include***************/
#include <unistd.h>
#include "../fpga.h"
#include "../fpga_reg.h"
#include "../utils/utils.h"
/****************************************/

/******************************************************************/
//==============NEED RECONFIG===========================

#define SPI_XYF_BUF_SIZE					1000

#define SPI_XYF_BUSY_CLEAR					1
#define SPI_XYF_BUSY_CNT					100000
#define SPI_XYF_SEND_FAIL					-1
#define SPI_XYF_RESET_FAIL					-2
#define SPI_XYF_LINK_ERROR					-9


#define IORD_SPI_XYF_16DIRECT(base,reg)			FPGA_Readl(base+((reg)<<2))
#define IOWR_SPI_XYF_16DIRECT(base,reg,data)		FPGA_Writel(base+((reg)<<2),data)

#define IORD_SPI_XYF_32DIRECT(base,reg)			FPGA_Readl(base+((reg)<<2))
#define IOWR_SPI_XYF_32DIRECT(base,reg,data)		FPGA_Writel(base+((reg)<<2),data)

#define SLAVE_SERVER_WAIT_ENA				0
#if SLAVE_SERVER_WAIT_ENA
#define SLAVE_SERVER_WAIT()		OSTimeDlyHMSM(0, 0, 3, 0)
#else
#define SLAVE_SERVER_WAIT()
#endif

//==============SPI_DEBUG===========================
#define SPI_XYF_DEBUG					0
#define SPI_XYF_DATA_DEBUG				0
/******************************************************************/

int spi_master_write(u_int dev_base, u_int spi_addr, void *wr_data, u_int wr_len);
int spi_master_read(u_int dev_base, u_int spi_addr, void *rd_data, u_int rd_len);


#endif

#ifndef __MUX_FPGA_H__
#define __MUX_FPGA_H__

#include <sys/types.h>

#include "../fpga.h"

#if FPGA_ACCESS_MODE
#define FPGA_BIT_WIDTH		16
#define DDR_BIT_WIDTH		16
#else
#define FPGA_BIT_WIDTH		32
#define DDR_BIT_WIDTH		32
#endif
#define MUXREG_BIT_WIDTH		32

#define DDR_PAGING_ACCESS		1

//#define MUX_REG_PAGE	1


/*
DDR3_A 基址： 0x4000_0000; 
lookup	0x1000000	0x30000000	每pid 128bytes，
每TS=1048576bytes=0x100000, 
768路TS=805306368bytes=0x30000000
Capture_zone	0x32000000	0x3000000	每路TS用0x10000=64Kbytes,最大存储340个SI包，若mcu来不及取走，FPGA将停止俘获。
SI Out Buffer	0x35000000	0x3000000	每路mux用1Mbytes=0x100000,48路mux共计0x3000000
Mux buffer	0x38000000	0x6000000	每路mux用2Mbytes=0x200000,48路mux共计0x6000000
//*/

#define DDR_PREMUX_LOOKUP_BASE 	(DDR_BASE_ADDRESS + (0x000<<16))
#define DDR_PREMUX_LOOKUP_END 	(DDR_PREMUX_LOOKUP_BASE + (0x1000<<16) - (DDR_BIT_WIDTH/8))
#define DDR_CAPTURE_BASE	 (DDR_BASE_ADDRESS + (0x1100<<16))
#define DDR_CAPTURE_END	 (DDR_CAPTURE_BASE + (0x300<<16) - (DDR_BIT_WIDTH/8))
#define DDR_SIOUT_BASE	 (DDR_BASE_ADDRESS + (0x1400<<16))
#define DDR_SIOUT_END	 (DDR_SIOUT_BASE + (0x300<<16) - (DDR_BIT_WIDTH/8))
#define DDR_MUXBUF_BASE	 (DDR_BASE_ADDRESS + (0x1700<<16))
#define DDR_MUXBUF_END	 (DDR_MUXBUF_BASE + (0x600<<16) - (DDR_BIT_WIDTH/8))


#define MUX_CFG_BASE_ADDR			0x0


/*
page	offset	
IPIN(Port1):	0x1000-0x13ff 
IPIN(Port2):	0x4000-0x4300
IPOUT_FEC:	0x40000-0x7ffff
IPOUTIPTV:	0x80000-0xbffff
PRE_MUX:	0xC0000-0xfffff

//*/
#define	REG_DDR_TSIN_BASE		(0x1000)
#define	REG_DDR_PREMUX_BASE		(0xC0000)
#define	REG_DDR_MUXTOP_BASE		(0x80000)

#define	TSIN_1_BASE_REG		(REG_DDR_TSIN_BASE + MUX_CFG_BASE_ADDR)
#define	PREMUX_BASE_REG		(REG_DDR_PREMUX_BASE + MUX_CFG_BASE_ADDR)
#define	MUX_TOP_BASE_REG		(REG_DDR_MUXTOP_BASE + MUX_CFG_BASE_ADDR)


#define WRITE_FPGA_RAM_PAGE(page)		FPGA_Set_Page(page)
#define UNLOCK_FPGA_RAM_PAGE(page)	FPGA_End_Page(page)

#define MUX_ARMIF_READ(dataaddr)			FPGA_Readl(MUX_ARMIF_BASE + (dataaddr<<2))
#define MUX_ARMIF_WRITE(dataaddr,data)		FPGA_Writel((MUX_ARMIF_BASE + (dataaddr<<2)), data)

#if (FPGA_BIT_WIDTH == 32)
#define FPGA_MUX_READ(dataaddr)				FPGA_Readl((dataaddr))
#define FPGA_MUX_WRITE(dataaddr, data)		FPGA_Writel((dataaddr), data)
#else
#define FPGA_MUX_READ(dataaddr)				FPGA_Readw((dataaddr))
#define FPGA_MUX_WRITE(dataaddr, data)		FPGA_Writew((dataaddr), data)
#endif

#if (DDR_BIT_WIDTH == 32)
#define FPGA_MUX_DDR_READ(dataaddr)				FPGA_Readl_DDR(dataaddr)
#define FPGA_MUX_DDR_WRITE(dataaddr, data)		FPGA_Writel_DDR(dataaddr, data)
#else
#define FPGA_MUX_DDR_READ(dataaddr)				FPGA_Readw_DDR(dataaddr)
#define FPGA_MUX_DDR_WRITE(dataaddr, data)		FPGA_Writew_DDR(dataaddr, data)
#endif

#endif


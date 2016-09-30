

#ifndef _FPGA_REG_H_
#define _FPGA_REG_H_



#define MUX_ARMIF_BASE		0x40000
#define TSOIP_ARMIF_BASE		0x400000	

/*
transceiver register group


*/

#define REG_FPGA_TRANSCEIVER_MUX		0x100000
#define REG_FPGA_TRANSCEIVER_IPOUT		0x200000

#define REG_FPGA_TRANSCEIVER_RECONFIG	0x300000

#define REG_FPGA_TRANSCEIVER_ROUTE_TABLE		0x140000
#define REG_FPGA_TRANSCEIVER_ASIOUT	0x17fffc


#define REG_FPGA_SPI_MASTER_BASE	0x10000


//-------------------------------------------------------------------------------------------------
//===========================================================================

/*
ARM_IF

Address in word	ARMRW	Name	Note

0	R		FPGA_Version	FPGA�汾��
1	R		IP_INPUT_PORTS	IP������·������16����ʾÿ��PHY֧��8·IP input
2	R		MUX_Ports	Mux��·���� gateway��֧�֡�
3	R		IP_OUTPUT_PORTS	IP�����·������16����ʾÿ��PHY֧��16·IP output��ע���INPUT��ͬ
4	R	W	Nios_ver	Nios�汾�š�Niosд�룬arm9��
5	R/W		Irq_enable	Bit[15] IRQ Enable
Bit[1] IPIN SI changed irq enable
Bit[0] link changed irq enable

Default��00
6				
7	R	W	PHY1_link	R:Bit[7:0] Phy1 link status,Bit[15:8] Phy2 link status
	W: Bit[7:0] Phy1 link status
0x27		W	PHY2_link	Bit[7:0] Phy2 link status
0x18		R	ASI_INPUT_PORTS	ASI ����˿���
0x19		R	ASI_OUTPUT_PORTS	ASI ����˿���

*/

#define	REG_FPGA_FPGA_VERSION		0x00
#define	REG_FPGA_IP_INPUT_PORTS	0x01
#define	REG_FPGA_MUX_PORTS			0x02
#define	REG_FPGA_IP_OUTPUT_PORTS	0x03
#define	REG_FPGA_NIOS_VERSION		0x04

#define	REG_FPGA_IRQ_ENABLE		0x05
#define	REG_FPGA_IRQ_STATUS		0x06

#define REG_FPGA_SI_CHANGE_STATUS_S       0x08 
#define REG_FPGA_SI_CHANGE_STATUS_E       0x17


#define	REG_FPGA_PHY_LINK			0x07

#define	REG_FPGA_PHY1_LINK_WR		0x07
#define	REG_FPGA_PHY2_LINK_WR		0x27

/*
0x38		R		FPGA_REG_IP1TX_BITRATE_LO	PHY1 �ܵ��������[15��0]
0x39		R		FPGA_REG_IP1TX_BITRATE_HI	PHY1 �ܵ��������[31��16]
0x3a		R		FPGA_REG_IP2TX_BITRATE_LO	PHY2 �ܵ��������[15��0]
0x3b		R		FPGA_REG_IP2TX_BITRATE_HI	PHY2 �ܵ��������[31��16]
0x3c		R		FPGA_REG_IP1RX_BITRATE_LO	PHY1 �ܵ���������[15��0]
0x3d		R		FPGA_REG_IP1RX_BITRATE_HI	PHY1 �ܵ���������[31��16]
0x3e		R		FPGA_REG_IP2RX_BITRATE_LO	PHY2 �ܵ���������[15��0]
0x3f		R		FPGA_REG_IP2RX_BITRATE_HI	PHY2 �ܵ���������[31��16]

*/
#define	REG_FPGA_PHY1_TX_BITRATE_LO		0x38
#define	REG_FPGA_PHY1_TX_BITRATE_HI		0x39
#define	REG_FPGA_PHY2_TX_BITRATE_LO		0x3a
#define	REG_FPGA_PHY2_TX_BITRATE_HI		0x3b

#define	REG_FPGA_PHY1_RX_BITRATE_LO		0x3c
#define	REG_FPGA_PHY1_RX_BITRATE_HI		0x3d
#define	REG_FPGA_PHY2_RX_BITRATE_LO		0x3e
#define	REG_FPGA_PHY2_RX_BITRATE_HI		0x3f



/*
0x2800	R		FPGA_REG_IPIN_BITERATE_BASE	PHY 1��ÿ·IP input�����ʡ�
W[1:0] ipin ch0 byterate,
W[3:2] ch1 byterate,��
0x5000	R		FPGA_REG_IPIN2_BITERATE_BASE	PHY 2��ÿ·IP input�����ʡ�
W[1:0] ipin ch0 byterate,
W[3:2] ch1 byterate,��
0x4000	R		FPGA_REG_ASIIN_BITERATE_BASE	ASI input��ÿ·����
W[1:0]asi ch0 byterate,
W[3:2] ch1 byterate,��
0x3000	R		FPGA_REG_MUX_BITERATE_BASE	TBD
0x3800	R		FPGA_REG_IPIN_SRCADDR_BASE	PHY1��ÿ·IP input��ip source address
0x5800	R		FPGA_REG_IPIN2_SRCADDR_BASE	PHY2��ÿ·IP input��ip source address
0x7000	R		FPGA_REG_IPOUT_BITERATE_BASE	IPOUT ��ÿ·����
W[1:0] ipout ch0 byterate
W[3:2] ch1 byterate

//*/
#define	REG_FPGA_IPIN1_BITRATE_BASE		0x2800
#define	REG_FPGA_IPIN2_BITRATE_BASE		0x5000

#define	REG_FPGA_IPIN1_SRCADDR_BASE		0x3800
#define	REG_FPGA_IPIN2_SRCADDR_BASE		0x5800

#define	REG_FPGA_IPOUT_BITRATE_BASE		0x7000

#define REG_FPGA_TSIP1_RTPERR_BASE		0x8000
#define REG_FPGA_TSIP2_RTPERR_BASE		0x8800




//-------------------------------------------------------------------------------

#define REG_FPGA_FANS_PWM       0x23

/*

//*/
#define REG_FPGA_SLOT_UART_CONFIG		0x90
#define REG_FPGA_SLOT_RESET_CONTROL	0x91
#define REG_FPGA_SLOT_CONNECT_STATUS	0x92
#define REG_FPGA_FAN1_STATUS		0x93
#define REG_FPGA_FAN2_STATUS		0x94
#define REG_FPGA_FAN3_STATUS		0x95
#define REG_FPGA_FAN4_STATUS		0x96




#define FPGA_ARMIF_READ(dataaddr)			FPGA_Readl(TSOIP_ARMIF_BASE + (dataaddr<<2))
#define FPGA_ARMIF_WRITE(dataaddr,data)		FPGA_Writel((TSOIP_ARMIF_BASE + (dataaddr<<2)), data)

#endif


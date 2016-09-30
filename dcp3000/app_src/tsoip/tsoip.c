

#include <sys/ioctl.h>
#include <netinet/in.h>
//#include <linux/in.h>
//#include <linux/if.h>
#include <linux/ip.h>
#include <linux/sockios.h>
#include <net/if.h>

#include "../appref.h"
#include "tsoip.h"


#define MAC_BASE_ADDR		0x210000
#define MAC_SPAN		0x10000

#define IPIN_BASE	 0x230000
#define IPIN_SPAN	 0x10000

#define IPOUT_BASE 0x280000



tsoip_network_params_st *tsoip_network;

tsoip_in_channel_st *tsoip_in_channel;
tsoip_out_channel_st *tsoip_out_channel;


static duList_st tsoip_iproute_list;

ipin_status_st ipin_status[MAX_TSOIP_IN_CHANNEL_QTY];
ipout_status_st ipout_status[MAX_TSOIP_OUT_CHANNEL_QTY];

int ipin_channel_ref[MAX_TSOIP_IN_CHANNEL_QTY];
int ipout_channel_ref[MAX_TSOIP_OUT_CHANNEL_QTY];

int ipin_idx_ref[MAX_TSOIP_IN_CHANNEL_QTY];
int ipout_idx_ref[MAX_TSOIP_OUT_CHANNEL_QTY];

int ipout_routeid[MAX_TSOIP_OUT_CHANNEL_QTY];
int ipin_trans_srcidx[MAX_TSOIP_IN_CHANNEL_QTY];

static pthread_mutex_t ipout_channel_mutex_lock;
#define ipout_channel_lock()		pthread_mutex_lock(&ipout_channel_mutex_lock)
#define ipout_channel_unlock()		pthread_mutex_unlock(&ipout_channel_mutex_lock)


static pthread_mutex_t ipin_channel_mutex_lock;
#define ipin_channel_lock()		pthread_mutex_lock(&ipin_channel_mutex_lock)
#define ipin_channel_unlock()		pthread_mutex_unlock(&ipin_channel_mutex_lock)


#define altwr_ipout(reg, data)	FPGA_Writel(((reg) << 2)+IPOUT_BASE, (data))
#define altrd_ipout(reg)	FPGA_Readl(((reg) << 2)+IPOUT_BASE)
#define altwr_ipin(base, reg, data)	FPGA_Writel(((reg) << 2)+(base), (data))
#define altrd_ipin(base, reg)	FPGA_Readl(((reg) << 2)+(base))
#define altwr_ipin_channel(base, reg, data)	FPGA_Writel(((reg) << 2)+(base) + 0x200, (data))
#define altrd_ipin_channel(base, reg)	FPGA_Readl(((reg) << 2)+(base) + 0x200)

///------------------------------


void set_ipin_channel(int nif_no, int channel, tsoip_in_channel_st *ip2ts, u_int src_ipaddr, int enable)
{

	iproute_st route;

	u_int target_ip;


	u_int val32, dest_ipaddr, dest_port;

	u_int base;


	printf("set %d tsoip in channel %d :  src_ipaddr %#.8x, enable %d ....\n", nif_no, channel, src_ipaddr, enable);

	base = IPIN_BASE + nif_no * IPIN_SPAN;

	ipin_channel_lock();

	altwr_ipin_channel(base, 5, channel);   //RX channel select register,

	if (enable){
		dest_port = ip2ts->target_port;
		dest_port +=  0x80000000;
		altwr_ipin_channel(base, 6, dest_port); //bit[15:0] UDP destination port for selected channel.[31]   Enable for selected channel (1 to enable)
	
		altwr_ipin_channel(base, 7, ip2ts->target_ip);                        //

		altwr_ipin(base, 0xc, channel);
		altwr_ipin(base, 0xe, ip2ts->ts_recovery_method);

		if(src_ipaddr){
			altwr_ipin_channel(base, 10,src_ipaddr);
			altwr_ipin_channel(base, 11, 1); 	// source ip address check , if enable ,write addr[10] & '1'
		}else{
			altwr_ipin_channel(base, 11, 0); 
		}
	}else{
		altwr_ipin_channel(base, 6, 0); 
	}


	//dest_ipaddr =  altrd_ipin_channel(base, 7);
	//dest_port =  altrd_ipin_channel(base, 6);

	ipin_channel_unlock();


	#ifndef DEBUG_TOOLS_MODE
	//IPROUTE(&route, IPROUTE_STATIC_NIF, tsoip_network[nif_no].ipaddr, tsoip_network[nif_no].netmask, tsoip_network[nif_no].nif_name);
		
	if(enable == 0){

		//del_ip_route(&route);
		//route_del_dev(tsoip_network[nif_no].ipaddr, 0xffffffff, tsoip_network[nif_no].nif_name);
		imr_drop_membership(tsoip_network[nif_no].ipaddr, tsoip_network[nif_no].nif_name, ip2ts->last_target_ip);
		
	}else{
	
		if(ip2ts->last_target_ip && (ip2ts->target_ip != ip2ts->last_target_ip))
			imr_drop_membership(tsoip_network[nif_no].ipaddr, tsoip_network[nif_no].nif_name, ip2ts->last_target_ip);
		
		//add_ip_route(&route);
		//route_add_dev(tsoip_network[nif_no].ipaddr, 0xffffffff, tsoip_network[nif_no].nif_name);
		imr_add_membership(tsoip_network[nif_no].ipaddr, tsoip_network[nif_no].nif_name, ip2ts->target_ip);
		ip2ts->last_target_ip = ip2ts->target_ip;
	}
	#endif

	//if(&tsoip_in_channel[channel] != ip2ts)
	//	memcpy(&tsoip_in_channel[channel], ip2ts, sizeof(*ip2ts));
	//auto_igmp_sendreport(channel, &myPara, &myNetwork, &myNetwork_b);


	//printf("set tsip in : target_ip : %s , port : %u \n", ip_i2a(ip2ts->target_ip), dest_port&0xffff);
			
}


void set_tsoip_in_channel(int channel)
{
	tsoip_in_channel_st *ip2ts;
	u_int src_ipaddr = 0;
	
	printf("set tsoip in channel %d....\n", channel);
	
	ip2ts = &tsoip_in_channel[channel];
	//channel = ip2ts->channel % sysinfo.ipin_channels;
	if(ip2ts->src_filter_enable){
		src_ipaddr = ip2ts->src_ipaddr;
	}

	set_ipin_channel(ip2ts->nif_no, ip2ts->channel,ip2ts, src_ipaddr, ip2ts->enable);
	
	save_db_blob(tsoip_in_param, channel, &tsoip_in_channel[channel]);

			
}


void set_tsoip_mac(int ifno, u_char *macaddr)
{

	set_mac_addr(tsoip_network[ifno].nif_name, macaddr);
	memcpy(tsoip_network[ifno].macaddr, macaddr, 6);

}

void set_tsoip_ipaddr(int ifno, u_int ipaddr, u_int netmask)
{

	set_ip_addr(tsoip_network[ifno].nif_name, ipaddr, netmask);
	tsoip_network[ifno].ipaddr = ipaddr;
	tsoip_network[ifno].netmask = netmask;

	save_db_blob(tsoip_network_param, ifno, &tsoip_network[ifno]);

}

void set_tsoip_gateway(int ifno, u_int gateway)
{

	//route_del(tsoip_network[ifno].gateway&tsoip_network[ifno].netmask, tsoip_network[ifno].netmask);
	route_add_gw(gateway&tsoip_network[ifno].netmask, tsoip_network[ifno].netmask, gateway);
	tsoip_network[ifno].gateway = gateway;

	save_db_blob(tsoip_network_param, ifno, &tsoip_network[ifno]);
}

void reset_tsoip_networks(void)
{
	//*
	//tsoip_network_params_st tsoip_network;

	printf("reset tsoip network  \n");

	sys_cmd("ifconfig %s down", tsoip_network[0].nif_name);
	sys_cmd("ifconfig %s down", tsoip_network[1].nif_name);
	sleep(1);
	set_ip_addr(tsoip_network[0].nif_name, tsoip_network[0].ipaddr, tsoip_network[0].netmask);
	set_ip_addr(tsoip_network[1].nif_name, tsoip_network[1].ipaddr, tsoip_network[1].netmask);
	
}


void set_tsoip_network(int ifno)
{
	//*
	//tsoip_network_params_st tsoip_network;

	printf("set_tsoip_network....%d.....\n", ifno);

	//set_mac_addr(tsoip_network[ifno].nif_name, tsoip_network[ifno].macaddr);

	sys_cmd("ifconfig %s down", tsoip_network[ifno].nif_name);
	sleep(1);
	set_ip_addr(tsoip_network[ifno].nif_name, tsoip_network[ifno].ipaddr, tsoip_network[ifno].netmask);

	//route_del(tsoip_network[ifno].ipaddr, tsoip_network[ifno].netmask);
	//route_add_gw(tsoip_network[ifno].ipaddr&tsoip_network[ifno].netmask, tsoip_network[ifno].netmask, tsoip_network[ifno].gateway);

	//*/

	save_db_blob(tsoip_network_param, ifno, &tsoip_network[ifno]);

	
}

void set_tsoip_out_channel(int channel)
{
	int i;
	u_char channel_enable;
	u_char encapsul_type;
	u_int offset;
	u_short t16;
	tsoip_out_channel_st *p_ipout;

	u_char fec_size[9][3] = {
			{0,0,0},				// 1:1D/2D/0D, 2:lsize, 3:dsize
			{1,5,5},
			{1,5,10},
			{1,10,10},
			{1,5,20},
			{3,5,5},
			{3,5,10},
			{3,10,10},
			{3,5,20}
	};

	p_ipout = &tsoip_out_channel[channel];
	
	
	//p_ipout->packets_per_udp = tsoip_network[0].packets_per_udp;
	
	
	channel_enable = p_ipout->enable_mode & 0x7;
	
	printf("setup %d ipout,pkts=%d,ena=0x%x\n",channel,p_ipout->packets_per_udp, channel_enable);
	printf("fec_idx = %d, ip A= %#.8x, ip B= %#.8x\n", p_ipout->fec_idx, p_ipout->target_ip[0],p_ipout->target_ip[1]);
	printf(" port A= %d, port B= %d\n", p_ipout->fec_idx, p_ipout->target_port[0],p_ipout->target_port[1]);

	ipout_channel_lock();
	
	altwr_ipout(0xfff, channel+1);	//channel + 1

	
	if(channel_enable){
		channel_enable <<= 1;
		channel_enable |= 1;
	}else{	
		altwr_ipout(0xf00, 0);
		ipout_channel_unlock();
		save_db_blob(tsoip_out_param, channel, &tsoip_out_channel[channel]);
		return;
	}

	for(i=0; i<sysinfo->tsip_port; i++){

		if((channel_enable & (2<<i)) == 0){
			continue;
		}
		
		offset = 0x8000*i;
	
		encapsul_type = p_ipout->encapsul_type[i];

		printf("set ip out : %d port encapsul_type = %#x\n", i, encapsul_type);

		if(encapsul_type){
			altwr_ipout(0+offset,0x3);//encap type=UDP
		}else{
			altwr_ipout(0+offset,0x7);//encap type=RTP
		}

		
		altwr_ipout(1+offset,0x11);  //
		altwr_ipout(2+offset,0x45);  //
		altwr_ipout(3+offset,0);  //

		//printf("%d tsoip out %d port ipaddr[3] : %#x\n", channel, i,  ((p_ipout->target_ip[i]>>24)&0xe0));

		if(((p_ipout->target_ip[i]>>24)&0xe0) == 0xe0){
			altwr_ipout(4+offset, p_ipout->target_ip[i]&0xff); 	 // MAC DA
			altwr_ipout(5+offset, (p_ipout->target_ip[i]>>8)&0xff);
			altwr_ipout(6+offset, (p_ipout->target_ip[i]>>16)&0x7f);
			altwr_ipout(7+offset, 0x5e);
			altwr_ipout(8+offset, 0x00);
			altwr_ipout(9+offset, 0x01);
			printf("%d tsoip out %d port ipaddr is muticast \n", channel, i);
		}else{
			//channel_enable &= ~(2<<i);
			//printf("%d tsoip out %d port ipaddr is unicast \n", channel, i);
		}


		p_ipout->tos[i] = tsoip_network[i].tos;
		p_ipout->ttl[i] = tsoip_network[i].ttl;


		altwr_ipout(10+offset,p_ipout->tos[i]);				//TOS
		altwr_ipout(11+offset,p_ipout->ttl[i]);				//TTL
	
		altwr_ipout(12+offset,  (tsoip_network[i].ipaddr)&0xff);  // IP source address
		altwr_ipout(13+offset, (tsoip_network[i].ipaddr>>8)&0xff);
		altwr_ipout(14+offset, (tsoip_network[i].ipaddr>>16)&0xff);
		altwr_ipout(15+offset, (tsoip_network[i].ipaddr>>24)&0xff);


		altwr_ipout(16+offset, (p_ipout->target_ip[i]>>0)&0xff);	// IP destination address
		altwr_ipout(17+offset, (p_ipout->target_ip[i]>>8)&0xff);
		altwr_ipout(18+offset, (p_ipout->target_ip[i]>>16)&0xff);
		altwr_ipout(19+offset, (p_ipout->target_ip[i]>>24)&0xff);

		altwr_ipout(20+offset, (p_ipout->target_port[i]>>0)&0xff);	// UDP source port
		altwr_ipout(21+offset, (p_ipout->target_port[i]>>8)&0xff);
		altwr_ipout(22+offset, (p_ipout->target_port[i]>>0)&0xff); 	 // UDP destination port
		altwr_ipout(23+offset, (p_ipout->target_port[i]>>8)&0xff); 


		if(!encapsul_type){
			t16 = p_ipout->target_port[i] + 4;
			altwr_ipout(24+offset, (t16 & 0xff));	// source port
			altwr_ipout(25+offset, ((t16>>8) & 0xff));
			altwr_ipout(26+offset, (t16 & 0xff));	// destination FEC row port
			altwr_ipout(27+offset, ((t16>>8) & 0xff));

			t16 = p_ipout->target_port[i] + 2;
			altwr_ipout(28+offset, (t16 & 0xff));	//  source port
			altwr_ipout(29+offset, ((t16>>8) & 0xff));
			altwr_ipout(30+offset, ((t16)& 0xff)); 	 // destination FEC column port
			altwr_ipout(31+offset, (((t16)>>8)& 0xff)); 
		}

	}
	
	//if  ((channel_enable & 0x1)== 1){

		//fec mode
		if (encapsul_type) {//udp has no fec
			altwr_ipout(0xf03,0);			
		}else{
			altwr_ipout(0xf03, fec_size[p_ipout->fec_idx][0]);
			altwr_ipout(0xf04, fec_size[p_ipout->fec_idx][1]);
			altwr_ipout(0xf05, fec_size[p_ipout->fec_idx][2]);
		}
		

		
		altwr_ipout(0xf01, p_ipout->packets_per_udp);	//set packets per frame

		if (p_ipout->test_lost == 1)
			channel_enable += 0x8;
		else if (p_ipout->test_lost == 2)
			channel_enable += 0x10;

		//printf("set %d tsoip out : enable = %#x \n", channel, channel_enable);
				
		altwr_ipout(0xf00, channel_enable);	//enable & port
	
		ipout_channel_unlock();
	//}
	save_db_blob(tsoip_out_param, channel, &tsoip_out_channel[channel]);
	
}


#define	GETLINKSTATUS		(SIOCDEVPRIVATE + 0)
#define	SETTSOIPINCHANNEL		(SIOCDEVPRIVATE + 1)

u_int get_tsoip_netif_link_status(int ifno)
{
	int i;
	u_short val16;
	//u_short link_status;
	char link_status;

	struct ifreq ifr_ip;
	
	memset(&ifr_ip, 0, sizeof(ifr_ip));
	
	strcpy(ifr_ip.ifr_name, tsoip_network[ifno].nif_name);
	
	if(ioctl(nif_socket_fd, GETLINKSTATUS, &ifr_ip) < 0 ) {
	        perror("ioctl : GETLINKSTATUS");
	        return -1;
	}

	link_status = ((u_int)ifr_ip.ifr_data);
	
//	printf("%s : netif_link_status = %#x \n", nif_name, ifr_ip.ifr_data);

	//tsoip_network[ifno].link_status = link_status;

	return link_status;
	#if 0

	//tsoip_network[0].link_status = link_status & 0xff;
	//tsoip_network[1].link_status = link_status >> 8;
	
	for(i=0; i<2; i++){
		//link_status = (val16 >> (i*4)) & 0x7;
		link_status = (val16 >> (i*8)) & 0xff;
		if ((tsoip_network[i].link_status) != (link_status)){
			printf("detect TSIP link changed:%x,%x ",tsoip_network[i].link_status, link_status);
			if (link_status & 0x01){
				
				printf("up,");
				
				if((link_status)&(1<<4)){
					
					printf("sfp : 1000M : full duplex\n");
					//add_log(EVENT_CODE(TSIP_SFP_EVENT_OBJ,EVENT_ACT_LINK_UP,1+i,0));
				}else{
					//add_log(EVENT_CODE(TSIP_RJ45_EVENT_OBJ,EVENT_ACT_LINK_UP,1+i,0));
					if ((link_status & 0x6) == 0){
						printf("speed:10M\n");
						
						if(i==0){
							//add_log(EVENT_TSIP_LNKUP10);
						}else{
							//add_log(EVENT_TSIPB_LNKUP10);
						}
					}
					else if ((link_status & 0x6) == 0x2){
						printf("speed:100M\n");
						if(i==0){
							//add_log(EVENT_TSIP_LNKUP100);
						}else{
							//add_log(EVENT_TSIPB_LNKUP100);
						}
					}
					else {
						printf("speed:1000M\n");
						if(i==0){
							//add_log(EVENT_TSIP_LNKUP1000);
						}else{
							//add_log(EVENT_TSIPB_LNKUP1000);
						}
					}

					if((link_status & 0x08)){
						printf("full duplex\n");
					}else{
						printf("half duplex\n");
					}
				}
				
			}else{
				printf("down \n");
				//add_log(EVENT_CODE(TSIP_RJ45_EVENT_OBJ,EVENT_ACT_LINK_DOWN,1+i,0));
			}

			tsoip_network[i].link_status = (link_status);
		}
		
	}
	#endif

}


int read_tsoip_netif_in_bitrate(int ifno)
{
	u_int bitrate;

	if(ifno == 0){
		bitrate = FPGA_ARMIF_READ(REG_FPGA_PHY1_RX_BITRATE_LO)&0xffff;
		bitrate |= (FPGA_ARMIF_READ(REG_FPGA_PHY1_RX_BITRATE_HI)&0xffff) << 16;
	}else if(ifno == 1){
		bitrate = FPGA_ARMIF_READ(REG_FPGA_PHY2_RX_BITRATE_LO)&0xffff;
		bitrate |= (FPGA_ARMIF_READ(REG_FPGA_PHY2_RX_BITRATE_HI)&0xffff) << 16;
	}else{
		return EXEC_ERR;
	}
	
	tsoip_network[ifno].in_bitrate = (bitrate + 500)/1000;
	
	if(tsoip_network[ifno].in_bitrate>0){
		tsoip_network[ifno].link_status|=0x1;
	}
	
	return EXEC_OK;
}

int read_tsoip_netif_out_bitrate(int ifno)
{
	u_int bitrate;

	if(ifno >= sysinfo->tsip_port){
		return EXEC_ERR;
	}

	if(tsoip_network[ifno].link_status&0x1){
		if(ifno == 0){
			bitrate = FPGA_ARMIF_READ(REG_FPGA_PHY1_TX_BITRATE_LO);
			bitrate |= FPGA_ARMIF_READ(REG_FPGA_PHY1_TX_BITRATE_HI) << 16;
		}else if(ifno == 1){
			bitrate = FPGA_ARMIF_READ(REG_FPGA_PHY2_TX_BITRATE_LO);
			bitrate |= FPGA_ARMIF_READ(REG_FPGA_PHY2_TX_BITRATE_HI) << 16;
		}
	}else{
		bitrate = 0;
	}


	tsoip_network[ifno].out_bitrate = (bitrate + 500)/1000;

	return EXEC_OK;
}

void init_ipin_status(void)
{
	int i;
	
	for(i=0; i<sysinfo->ipin_channels; i++){
		
		ipin_status[i].channel_lock = 0;

		ipin_status[i].bitrate = 0;
		ipin_status[i].format = 0;
		ipin_status[i].lsize = 0;
		ipin_status[i].dsize = 0;
		ipin_status[i].src_ipaddr = 0;
		ipin_status[i].ts_byterrate = 0;
		ipin_status[i].udp_length = 0;
		ipin_status[i].recovery_packets = 0;
		ipin_status[i].lost_packets = 0;
		
	}

}


void init_ipout_status(void)
{
	int i;
	
	for(i=0; i<sysinfo->ipout_channels; i++){
		
		ipout_status[i].input_bitrate = 0;
		ipout_status[i].input_format = 0;
		ipout_status[i].ts_packets = 0;
		ipout_status[i].cfec_packets = 0;
		ipout_status[i].rfec_packets = 0;
	
	}

}



void init_tsoip_networks(void)
{
	int i;
	u_int ipaddr, gateway, netmask;

	ipaddr = ip_a2i("192.168.0.10");
	gateway = ip_a2i("192.168.0.1");
	netmask = ip_a2i("255.255.255.0");

	for(i=0; i<sysinfo->tsip_port; i++){


		sprintf(tsoip_network[i].nif_name, "altTse%d", i);
		
		tsoip_network[i].ipaddr = ipaddr + (i<<8);
		tsoip_network[i].gateway = gateway + (i<<8);
		tsoip_network[i].netmask = netmask;

		//get_nif_mac(tsoip_network[i].nif_name, tsoip_network[i].macaddr);
		/*
		tsoip_network[i].macaddr[0] = 0x00;
		tsoip_network[i].macaddr[1] = 0x08;
		tsoip_network[i].macaddr[2] = 0x15;
		tsoip_network[i].macaddr[3] = 0x6b;
		tsoip_network[i].macaddr[4] = 0x25;
		tsoip_network[i].macaddr[5] = 0x2a + i;
		//*/
		
		
		tsoip_network[i].ttl = 0xff;
		tsoip_network[i].tos = 0;
		tsoip_network[i].packets_per_udp = 7;
		tsoip_network[i].igmp_mode = 0;

		tsoip_network[i].link_status = 0;	
		tsoip_network[i].link_status|=0x1;
		
		//set_tsoip_network(i);
	}

	
}

void init_tsoip_in_channels(void)
{
	int i,k;
	int channel, chan_qty;

	u_int target_ip = ip_a2i("224.1.1.1");

	//chan_qty = sysinfo.ipin_channels/2;

	for(k=0; k<sysinfo->tsip_port; k++){
		//for(i=k*sysinfo.tsip_port; i<sysinfo.ipin_channels*(k+1); i++){

		chan_qty = tsoip_network[k].inchan_qty;
		
		for(i=0; i<chan_qty; i++){
			
			//channel = i + chan_qty*k;

			channel = i + tsoip_network[k].inchan_s;
			
			tsoip_in_channel[channel].channel = i;
			tsoip_in_channel[channel].enable = 0;
			tsoip_in_channel[channel].nif_no = k;
			tsoip_in_channel[channel].last_target_ip = 0;
			tsoip_in_channel[channel].target_ip = target_ip+i+(k<<8);
			tsoip_in_channel[channel].target_port = 1234;
			tsoip_in_channel[channel].src_ipaddr = 0;
			tsoip_in_channel[channel].mode = 1;
			tsoip_in_channel[channel].ts_recovery_method = 0;
			//set_tsoip_in_channel(&tsoip_in_channel[i]);
		}
	}

}

void init_tsoip_out_channels(void)
{
	int i,j;

	u_int target_ip = ip_a2i("225.1.1.1");

	for(i=0; i<sysinfo->ipout_channels; i++){
		
		tsoip_out_channel[i].channel = i;
		tsoip_out_channel[i].enable_mode = 0;
		tsoip_out_channel[i].packets_per_udp = 7;
		
		for(j=0; j<sysinfo->tsip_port; j++){
			tsoip_out_channel[i].tos[j] = tsoip_network[j].tos;
			tsoip_out_channel[i].ttl[j] = tsoip_network[j].ttl;
			tsoip_out_channel[i].encapsul_type[j] = 1;
			tsoip_out_channel[i].target_ip[j] = target_ip + (j<<16) + i;
			tsoip_out_channel[i].target_port[j] = 5220;
			tsoip_out_channel[i].src_flag = 0;
			tsoip_out_channel[i].trans_src_chan = 0xffff;
			
		}
		
		//set_tsoip_out_channel(i);
	}

	
}

void init_tsoip_env(void)
{
	int i;
	init_duList(&tsoip_iproute_list, STRUCT_DATA);

	pthread_mutex_init(&ipin_channel_mutex_lock, NULL);
	pthread_mutex_init(&ipout_channel_mutex_lock, NULL);

	init_ipin_status();

	
	for(i=0; i<sysinfo->ipin_channels; i++){
		ipin_idx_ref[i] = -1;
	}

	
	for(i=0; i<sysinfo->ipout_channels; i++){
		ipout_channel_ref[i] = -1;
		ipout_idx_ref[i] = -1;
	}

}

void init_tsoip(void)
{
	int i, qty_per_port;
	int channel;
	
	
	for(i=0; i<sysinfo->tsip_port; i++){	
		set_tsoip_network(i);
		//route_add_dev(ip_a2i("224.0.0.0"), ip_a2i("224.0.0.0"), tsoip_network[i].nif_name);
		if(read_macaddr(i+1, tsoip_network[i].macaddr)<0){
			get_nif_mac(tsoip_network[i].nif_name, tsoip_network[i].macaddr);
		}else{
			set_tsoip_mac(i, tsoip_network[i].macaddr);
		}
	}
	
	//qty_per_port = sysinfo.ipin_channels/sysinfo.tsip_port;
//	for(i=0; i<sysinfo.ipin_channels; i++){
	for(i=0; i<tsoip_in_backup.channel_qty; i++){

		channel = ipin_idx_ref[i];
		if(channel<0){
			continue;
		}
	
		printf("%d tsoip_in_channel : nifno %d, channel %d \n", channel, tsoip_in_channel[channel].nif_no, tsoip_in_channel[channel].channel);
		set_ipin_channel_with_backup(channel);
	}

	
	#if 1
	
	for(i=0; i<sysinfo->ipout_channels; i++){
		
		channel = ipout_idx_ref[i];
		if(channel<0){
			continue;
		}
		
		set_tsoip_out_channel(channel);
	}
	
	#endif


}


int read_ipin_channel_bitrate(int ifno, int channel)
{

	int start_chan, offset;
	
	u_int bitrate;

	#if 1

	start_chan = channel;
	
	if(ifno == 0){
		offset = REG_FPGA_IPIN1_BITRATE_BASE;
	}else if(ifno == 1){
		offset = REG_FPGA_IPIN2_BITRATE_BASE;
	}

	offset += start_chan<<1;

	if(tsoip_network[ifno].link_status&0x1){
			bitrate = FPGA_ARMIF_READ(offset)&0xffff;
			bitrate |= (FPGA_ARMIF_READ(offset + 1)&0xffff) << 16;
	}else{
		bitrate = 0;
	}

	//bitrate *= 8;
	bitrate /= 1000;


	//printf(" tsoip in  [%d] channel  bitrate = %d \n", in_chan, bitrate);
	
	#endif

	return bitrate;
}


u_int read_ipin_channel_srcip(int ifno, int channel)
{

	int offset;
	
	u_int src;
	
	if(ifno == 0)
		offset = REG_FPGA_IPIN1_SRCADDR_BASE;
	else if(ifno == 1)
		offset = REG_FPGA_IPIN2_SRCADDR_BASE;

	offset += channel<<1;

	src = FPGA_ARMIF_READ(offset)&0xffff;
	src |= (FPGA_ARMIF_READ(offset + 1)&0xffff) << 16;


	return src;
}

void read_ipout_status(int channel)
{

	int i;
	i = channel;
	
	//for (i=0;i<sysinfo.ipout_channels;i++){
	
		ipout_channel_lock();
		
		altwr_ipout(0xfff, i);		//channel 
		ipout_status[i].input_format = altrd_ipout(0xf02);		// 1=188,0=204
		ipout_status[i].ts_packets = altrd_ipout(0xf04);
		ipout_status[i].cfec_packets = altrd_ipout(0xf05);
		ipout_status[i].rfec_packets = altrd_ipout(0xf06);
		
		ipout_channel_unlock();

	//}
}

void read_ipin_status(int channel)
{
	unsigned int src = 0;
	int i;
	int ifno;
	int start_chan, offset;
	u_short t16;
	u_int bitrate;

	u_int event_code;
		
	///printf("read ipin channel %d status : \n", channel);
	
	if(channel >= sysinfo->ipin_channels){
		return EXEC_ERR;
	}
		
	ifno = tsoip_in_channel[channel].nif_no;
	start_chan = tsoip_in_channel[channel].channel;

	bitrate = read_ipin_channel_bitrate(ifno, start_chan);

	if(ipin_status[channel].bitrate< 1){
		if((bitrate < 1) && (ipin_status[channel].channel_lock)){
			ipin_status[channel].channel_lock = 0;
			//printf(" ts in  [%d] channel unlocked \n", in_chan);

			add_log(EVENT_CODE(FUNC_TYPE_TS_IP_IN,ACT_TYPE_UNLOCK,ifno+1, start_chan+1), EVENT_LEVEL_WARNING);
		}
	}else{
		if((bitrate >= 1) && (ipin_status[channel].channel_lock == 0)){
			ipin_status[channel].channel_lock = 2;
			//printf(" ts in  [%d] channel locked , bitrate = %d \n", in_chan, bitrate);

			add_log(EVENT_CODE(FUNC_TYPE_TS_IP_IN,ACT_TYPE_LOCK,ifno+1, start_chan+1), EVENT_LEVEL_NORMAL);
		}
	}


	ipin_status[channel].bitrate = bitrate;

	if(ipin_status[channel].channel_lock){
		src = read_ipin_channel_srcip(ifno, start_chan);
	}
	ipin_status[channel].src_ipaddr = src;

#if 1
		ipin_channel_lock();

		offset = IPIN_BASE + IPIN_SPAN*ifno;

		
		altwr_ipin(offset, 0x0c, channel);		//channel 
		t16 = altrd_ipin(offset, 0x11);
		ipin_status[channel].lost_packets = (t16<<16) + altrd_ipin(offset, 0x10);

		t16 = altrd_ipin(offset, 0x13);
		ipin_status[channel].recovery_packets = (t16<<16) + altrd_ipin(offset, 0x12);

		t16 = altrd_ipin(offset , 0x14);
		ipin_status[channel].udp_length = t16 & 0x7ff;
		ipin_status[channel].format = (t16 >> 11) & 0x7;

		t16 = altrd_ipin(offset, 0x15);
		ipin_status[channel].lsize = t16 & 0x1f;
		ipin_status[channel].dsize = (t16>>5) & 0x1f;

		t16 = altrd_ipin(offset, 0x17);
		ipin_status[channel].ts_byterrate = (t16<<16) + altrd_ipin(offset, 0x16);

		ipin_channel_unlock();
	#endif
	
	//printf(" tsoip in  [%d] channel  bitrate = %d \n", in_chan, bitrate);
		
}

void print_tsoip_local(int nif_no)
{
	int i = nif_no;
	char * nif_name;
	u_char *macaddr;


	nif_name = tsoip_network[i].nif_name;
	macaddr = tsoip_network[i].macaddr;
	
	printf("tsoip %d : nifname = %s \n", i, tsoip_network[i].nif_name);
	printf("tsoip %d : ipaddr = %#.8x \n", i,  tsoip_network[i].ipaddr);
	printf("tsoip %d : gateway = %#.8x \n", i,  tsoip_network[i].gateway);
	printf("tsoip %d : netmask = %#.8x \n", i,  tsoip_network[i].netmask);
	printf("tsoip %d : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x \n", i, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	printf("tsoip %d : packets_per_udp = %d \n", i,  tsoip_network[i].packets_per_udp);
	printf("tsoip %d : tos = %#x \n", i,  tsoip_network[i].tos);
	printf("tsoip %d : ttl = %#x \n", i,  tsoip_network[i].ttl);
	printf("tsoip %d : igmp_mode = %#x \n", i, tsoip_network[i].igmp_mode);
	printf("tsoip %d :  in channel start with %d , qty = %d \n", i,  tsoip_network[i].inchan_s, tsoip_network[i].inchan_qty);
	printf("tsoip %d :  out channel start with %d , qty = %d \n", i,  tsoip_network[i].outchan_s, tsoip_network[i].outchan_qty);
	printf("tsoip %d : link_status = %#x \n", i,  tsoip_network[i].link_status);

}


void print_tsoip_in_channel(int channel)
{

	printf("tsoip in %d channel : nif_no = %d \n", channel, tsoip_in_channel[channel].nif_no);
	printf("tsoip in %d channel : channel = %d \n", channel, tsoip_in_channel[channel].channel);
	printf("tsoip in %d channel : enable = %d \n", channel, tsoip_in_channel[channel].enable);
	printf("tsoip in %d channel : target_ip = %#.8x \n", channel, tsoip_in_channel[channel].target_ip);
	printf("tsoip in %d channel : src_ipaddr = %#.8x \n", channel, tsoip_in_channel[channel].src_ipaddr);
	printf("tsoip in %d channel : ip_port = %d \n", channel, tsoip_in_channel[channel].target_port);
	printf("tsoip in %d channel : uni/multi cast = %d \n", channel, tsoip_in_channel[channel].mode);
	printf("tsoip in %d channel : ts_recovery_method = %d \n", channel, tsoip_in_channel[channel].ts_recovery_method);

}

void print_tsoip_out_channel(int channel)
{
	int j;

	printf("tsoip out %d channel : enable_mode = %#x \n", channel, tsoip_out_channel[channel].enable_mode);
	printf("tsoip out %d channel : packets_per_udp = %d \n", channel, tsoip_out_channel[channel].packets_per_udp);
		
	for(j=0; j<sysinfo->tsip_port; j++){
		printf("tsoip out %d channel : %d port , target_ip = %#.8x \n", channel, j, tsoip_out_channel[channel].target_ip[j]);
		printf("tsoip out %d channel : %d port , target_port = %d \n", channel, j, tsoip_out_channel[channel].target_port[j]);
		printf("tsoip out %d channel : %d port , encapsul_type = %#x \n", channel, j, tsoip_out_channel[channel].encapsul_type[j]);
		printf("tsoip out %d channel : %d port , tos = %#.2x \n", channel, j, tsoip_out_channel[channel].tos[j]);
		printf("tsoip out %d channel : %d port , ttl = %#.2x \n", channel, j, tsoip_out_channel[channel].ttl[j]);

	}

}


int compare_iproute(iproute_st *route1, iproute_st *route2)
{
	if(route1 && route2){
		if(route1->type == route2->type){
			switch(route1->type){
				
				case IPROUTE_DEFAULT_GW:
					if(route1->next_dev.gateway == route2->next_dev.gateway){
						return 0;
					}
					break;
				case IPROUTE_STATIC_GW:
					if((route1->next_dev.gateway == route2->next_dev.gateway) && (route1->netmask == route2->netmask) 
						&& ((route1->destip & route1->netmask) == (route2->destip & route2->netmask))){
						return 0;
					}
					break;
				case IPROUTE_STATIC_NIF:
					if((strcmp(route1->next_dev.nif_name, route2->next_dev.nif_name) == 0) && (route1->netmask == route2->netmask) 
						&& ((route1->destip & route1->netmask) == (route2->destip & route2->netmask))){
						return 0;
					}
					break;
				default :
					return -1;
				
			}
		}
	}

	return -1;
}

iproute_st *find_iproute(int route_type, u_int dest_ip, u_int netmask, u_int nif_gw)
{
	iproute_st route;

	IPROUTE(&route, route_type, dest_ip, netmask, nif_gw);

	return (iproute_st *)duList_find(&tsoip_iproute_list, &route, (compare_func)compare_iproute);

}

int add_iproute(int route_type, u_int dest_ip, u_int netmask, u_int nif_gw)
{
	iproute_st route, *p_route;
	dnode_st *pnode;

	
	IPROUTE(&route, route_type, dest_ip, netmask, nif_gw);
	
	pnode = duList_find_node(&tsoip_iproute_list, &route, (compare_func)compare_iproute);
	if(pnode==NULL){
		if(route_type == IPROUTE_STATIC_NIF){
			route.next_dev.nif_name = cpstring((char *)nif_gw);
		}
		add_ip_route(&route);
		duList_add(&tsoip_iproute_list, &route, sizeof(route));
	}
}


int del_iproute(int route_type, u_int dest_ip, u_int netmask, u_int nif_gw)
{
	iproute_st route, *p_route;
	dnode_st *pnode;

	IPROUTE(&route, route_type, dest_ip, netmask, nif_gw);
	
	pnode = duList_find_node(&tsoip_iproute_list, &route, (compare_func)compare_iproute);
	if(pnode){
		del_ip_route(&route);
		if(route_type == IPROUTE_STATIC_NIF){
			p_route = (iproute_st *)(pnode->data);
			free(p_route->next_dev.nif_name);
		}
		duList_del_node(&tsoip_iproute_list, pnode);
	}
}


#if 0

/*
void *tsoip_task(void *arg)
{
	u_short cmd_type, idx;
	int tsoip_task_arg;

	tsoip_task_arg = (int)arg;

	cmd_type = tsoip_task_arg >> 16;
	idx = tsoip_task_arg & 0xffff;
	
	switch(cmd_type){

		case SET_ONE_TSOIP_IN :
			set_tsoip_in_channel(idx);
			break;
		case SET_ALL_TSOIP_IN :
			set_tsoip_in_all_channels(idx);
			break;
		case SET_LOCAL_TSOIP :
			set_tsoip_network(idx);
			break;
		case SET_TSOIP_OUT :
			set_tsoip_out_channel(idx);
			break;
	}


	return (void *)0;
}
//*/

extern thread_pool_t *tsoip_thread_pool;

void tsoip_add_task(u_short cmd_type, u_short idx)
{
	task_arg_t task_arg;
	int tsoip_task_arg;

	tsoip_task_arg = (cmd_type << 16) + idx;
	
	task_arg.arg = (void *)tsoip_task_arg;
	task_arg.arg_size = 4;

	switch(cmd_type){

		case SET_ONE_TSOIP_IN :
			task_arg.mask_bit = idx;
			break;
		case SET_ALL_TSOIP_IN :
			if(idx == SET_ALL_FLAG){
				idx = sysinfo->tsip_port;
			}
			task_arg.mask_bit = sysinfo->total_tsin_channels + idx;
			break;
		case SET_LOCAL_TSOIP :
			task_arg.mask_bit = sysinfo->total_tsin_channels + 8 + idx;
			break;
		case SET_TSOIP_OUT :
			task_arg.mask_bit = sysinfo->total_tsin_channels + 8 + 8 + idx;
			break;
	}
	
	thread_pool_add_task(tsoip_thread_pool, tsoip_task, &task_arg);
}


#endif




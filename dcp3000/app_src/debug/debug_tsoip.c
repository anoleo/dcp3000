#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "../debug.h"

#include "../system.h"
#include "../parameter.h"
#include "../tsoip/tsoip.h"
#include "../tsoip/tsoip_backup.h"


int debug_tsoip( int argc, char *argv[ ] )
{
	int i, idx, nif_no = 0;
  	int channel = 0;
	u_int v_u32;
	tsoip_in_channel_st ip2ts, *p_ip2ts;
	tsoip_out_channel_st *p_ts2ip;

	if(argc < 2){
		return 1;
	}

	if(strcmp("init", argv[1]) == 0){
		init_tsoip();
	}else if(strcmp("cfg", argv[1]) == 0){

		if(argc < 5){	return 1; }
	 
		nif_no = atoi(argv[2]);

		tsoip_network[nif_no].ipaddr = ip_a2i(argv[3]);
		tsoip_network[nif_no].netmask = ip_a2i(argv[4]);
		
		set_tsoip_ipaddr(nif_no, tsoip_network[nif_no].ipaddr, tsoip_network[nif_no].netmask);
		//set_tsoip_gateway(nif_no, u_int gateway);
		
	}else if(strcmp("local", argv[1]) == 0){
		if(argc>2)
			channel = atoi(argv[2]);
		print_tsoip_local(channel);
	}else if(strcmp("in", argv[1]) == 0){
		if(argc == 3){
			channel = atoi(argv[2]);
			print_tsoip_in_channel(channel);
		}else if(argc == 4){
			channel = atoi(argv[2]);
			p_ip2ts = &tsoip_in_channel[channel];
			p_ip2ts->enable = atoi(argv[3]);
			set_tsoip_in_channel(channel);
		}else if(argc < 7){
			nif_no = atoi(argv[2]);
			channel = atoi(argv[3]);
			channel += nif_no*sysinfo->ipin_channels/sysinfo->tsip_port;
			print_tsoip_in_channel(channel);
		}else{
			nif_no = atoi(argv[2]);
			channel = atoi(argv[3]);
			p_ip2ts = &tsoip_in_channel[channel + nif_no*sysinfo->ipin_channels/sysinfo->tsip_port];
			
			p_ip2ts->target_ip = ip_a2i(argv[4]);
			p_ip2ts->target_port = atoi(argv[5]);
			p_ip2ts->nif_no = nif_no;
			p_ip2ts->channel = channel;

			p_ip2ts->enable = atoi(argv[6]);

			p_ip2ts->mode = 1;

			set_tsoip_in_channel(channel + nif_no*sysinfo->ipin_channels/sysinfo->tsip_port);
		}

	}else if(strcmp("out", argv[1]) == 0){

		if(argc < 7){
			if(argc>2)
				channel = atoi(argv[2]);
			
			if(argc>3){
				p_ts2ip = &tsoip_out_channel[channel];
				p_ts2ip->enable_mode = atoi(argv[3]);
				set_tsoip_out_channel(channel);
			}
			
			print_tsoip_out_channel(channel);

		}else{
			nif_no = atoi(argv[2]);
		
		 	channel = atoi(argv[3]);
		 	p_ts2ip = &tsoip_out_channel[channel];
			p_ts2ip->target_ip[nif_no] = ip_a2i(argv[4]);
			p_ts2ip->target_port[nif_no] = atoi(argv[5]);
			if(atoi(argv[6]))
				p_ts2ip->enable_mode |= nif_no+1;
			else
				p_ts2ip->enable_mode &= ~(nif_no+1);

			printf("%d p_ts2ip->enable_mode %d\n", nif_no, p_ts2ip->enable_mode);
			
			p_ts2ip->encapsul_type[nif_no] = 1;
			p_ts2ip->fec_idx = 0;
			p_ts2ip->test_lost = 0;
			p_ts2ip->packets_per_udp = 7;
			
			set_tsoip_out_channel(channel);
		}
		
	 }else if(strcmp("addmc", argv[1]) == 0){
	 	if(argc < 4){	return 1; }
		
		nif_no = atoi(argv[2]);
		imr_add_membership(tsoip_network[nif_no].ipaddr, tsoip_network[nif_no].nif_name, ip_a2i(argv[3]));
	 }else if(strcmp("delmc", argv[1]) == 0){
		if(argc < 4){	return 1; }
		nif_no = atoi(argv[2]);
		imr_drop_membership(tsoip_network[nif_no].ipaddr, tsoip_network[nif_no].nif_name, ip_a2i(argv[3]));
	 }else if(strcmp("addmcsrc", argv[1]) == 0){
	 	if(argc < 5){	return 1; }
		
		nif_no = atoi(argv[2]);
		imr_add_src_membership(tsoip_network[nif_no].ipaddr, ip_a2i(argv[3]), ip_a2i(argv[4]));
	 }else if(strcmp("delmcsrc", argv[1]) == 0){
	 	if(argc < 5){	return 1; }
		
		nif_no = atoi(argv[2]);
		imr_drop_src_membership(tsoip_network[nif_no].ipaddr, ip_a2i(argv[3]), ip_a2i(argv[4]));
	 }else if(strcmp("status", argv[1]) == 0){
		if(argc < 4){	return 1; }
		
		channel = atoi(argv[3]);
		if (strcmp("in", argv[2]) == 0){
			read_ipin_status(channel);
			printf("%d tsoip in channel ; src ipaddr = %#.8x, bitrate = %d\n", channel, ipin_status[channel].src_ipaddr, ipin_status[channel].bitrate);
			printf("%d tsoip in channel ;  lsize = %d, dsize = %d\n", channel, ipin_status[channel].lsize, ipin_status[channel].dsize);
			printf("%d tsoip in channel ;  format = %#x, SI_ok = %d\n", channel, ipin_status[channel].format, ipin_status[channel].SI_ok);
			printf("%d tsoip in channel ;  lost_packets = %d, recovery_packets = %d\n", channel, ipin_status[channel].lost_packets, ipin_status[channel].recovery_packets);
			printf("%d tsoip in channel ;  udp_length = %d, ts_byterrate = %d\n", channel, ipin_status[channel].udp_length, ipin_status[channel].ts_byterrate);
		}else if (strcmp("out", argv[2]) == 0){
			read_ipout_status(channel);
			
			printf("ipout %d channel : input_format = %#x, ts_packets = %d, cfec_packets = %d, rfec_packets = %d \n", channel
				, ipout_status[channel].input_format, ipout_status[channel].ts_packets, ipout_status[channel].cfec_packets, ipout_status[channel].rfec_packets);
		
		}else if (strcmp("if", argv[2]) == 0){
			read_tsoip_netif_in_bitrate(channel);
			read_tsoip_netif_out_bitrate(channel);
			printf("%d tsoip netif : in bitrate %d, out bitrate %d \n", channel, tsoip_network[nif_no].in_bitrate, tsoip_network[nif_no].out_bitrate);
			v_u32 = get_tsoip_netif_link_status(channel);
			printf("%d tsoip linkstatus : %#x \n", channel, v_u32);
		}
	 }else if(strcmp("idx", argv[1]) == 0){
	 	if(argc < 3){	return 1; }

		if (strcmp("in", argv[2]) == 0){
			select_db_blob("ipin_idx_ref", 0, ipin_idx_ref, 4 * sysinfo->ipin_channels);
			for(i=0; i<sysinfo->ipin_channels; i++){
				channel = ipin_idx_ref[i];
				printf("%d tsoip in channel :  %d \n", i, channel);
			}
		}else if (strcmp("out", argv[2]) == 0){
			select_db_blob("ipout_idx_ref", 0, ipout_idx_ref, 4 * sysinfo->ipout_channels);
			for(i=0; i<sysinfo->ipout_channels; i++){
				channel = ipout_idx_ref[i];
				printf("%d tsoip out channel :  %d \n", i, channel);
			}
		}
		
	}

}


int debug_tsoip_backup( int argc, char *argv[ ] )
{
	int i,idx, nif_no = 0;
  	int v_s32, channel = 0;
	u_int v_u32;
	char *strbuf = NULL;
	u_int bitrate = 0, srcip = 0;
	tsoip_in_channel_st ip2ts, *p_ip2ts;

	if(argc < 2){
		return 1;
	}

	if(strcmp("info", argv[1]) == 0){

		printf("tsoip_in_backup.backup_mode = %#x \n", tsoip_in_backup.backup_mode);
		printf("tsoip_in_backup.channel_qty = %#d \n", tsoip_in_backup.channel_qty);
		printf("tsoip_in_backup.test_period = %#d\n", tsoip_in_backup.test_period);
		
	}else if(strcmp("channel", argv[1]) == 0){

		if(argc < 3){	return 1; }
	 
		channel = atoi(argv[2]);

		printf("tsoip_in_backup.channel[%d].used_src_idx = %d \n",  channel, tsoip_in_backup.channel[channel].used_src_idx);
		printf("tsoip_in_backup.channel[%d].src_mask = %#x \n",  channel, tsoip_in_backup.channel[channel].src_mask);
		printf("tsoip_in_backup.channel[%d].backup_status = %#x \n",  channel, tsoip_in_backup.channel[channel].backup_status);
		printf("tsoip_in_backup.channel[%d].main_src_status = %#x \n",  channel, tsoip_in_backup.channel[channel].main_src_status);
		printf("tsoip_in_backup.channel[%d].cur_src_status = %#x \n",  channel, tsoip_in_backup.channel[channel].cur_src_status);
		printf("tsoip_in_backup.channel[%d].lower_bitrate = %d \n",  channel, tsoip_in_backup.channel[channel].lower_bitrate);
		printf("tsoip_in_backup.channel[%d].upper_bitrate = %d \n",  channel, tsoip_in_backup.channel[channel].upper_bitrate);
		printf("tsoip_in_backup.channel[%d].src_idx_0 = %#x \n",  channel, tsoip_in_backup.channel[channel].src_idx_0);
		printf("tsoip_in_backup.channel[%d].src_idx_1 = %#x \n",  channel, tsoip_in_backup.channel[channel].src_idx_1);
		printf("tsoip_in_backup.channel[%d].src_bitrate[0] = %d \n",  channel, tsoip_in_backup.channel[channel].bitrate[0]);
		printf("tsoip_in_backup.channel[%d].src_bitrate[1] = %d \n",  channel, tsoip_in_backup.channel[channel].bitrate[1]);
		printf("tsoip_in_backup.channel[%d].src_bitrate[2] = %d \n",  channel, tsoip_in_backup.channel[channel].bitrate[2]);
		printf("tsoip_in_backup.channel[%d].src_bitrate[3] = %d \n",  channel, tsoip_in_backup.channel[channel].bitrate[3]);
		printf("tsoip_in_backup.channel[%d].src_ipaddr[0] = %#x \n",  channel, tsoip_in_backup.channel[channel].src_ipaddr[0]);
		printf("tsoip_in_backup.channel[%d].src_ipaddr[1] = %#x \n",  channel, tsoip_in_backup.channel[channel].src_ipaddr[1]);
		printf("tsoip_in_backup.channel[%d].src_ipaddr[2] = %#x \n",  channel, tsoip_in_backup.channel[channel].src_ipaddr[2]);
		printf("tsoip_in_backup.channel[%d].src_ipaddr[3] = %#x \n",  channel, tsoip_in_backup.channel[channel].src_ipaddr[3]);
	}else if(strcmp("enable", argv[1]) == 0){

		tsoip_in_backup.backup_mode = 1;
		tsoip_in_backup.channel_qty = tsoip_network[0].inchan_qty;
		save_ipin_backup();
				
	}else if(strcmp("disable", argv[1]) == 0){

		tsoip_in_backup.backup_mode = 0;
		tsoip_in_backup.channel_qty = sysinfo->ipin_channels;
		save_ipin_backup();
				
	}else if(strcmp("threshold", argv[1]) == 0){
		if(argc<5){
			return 1;
		}
		channel = atoi(argv[2]);
		tsoip_in_backup.channel[channel].lower_bitrate = atoi(argv[3]);
		tsoip_in_backup.channel[channel].upper_bitrate = atoi(argv[4]) ;
		
		save_ipin_channel_backup_threshold(channel);
				
	}else if(strcmp("srcmask", argv[1]) == 0){
		if(argc<4){
			return 1;
		}
		channel = atoi(argv[2]);
		tsoip_in_backup.channel[channel].src_mask = atoi(argv[3]);
		
		save_ipin_channel_backup_src_mask(channel);
				
	}else if(strcmp("in", argv[1]) == 0){

		if(argc<7){
			return 1;
		}
	
		v_s32 = strtol(argv[3], &strbuf,10);
		if(strbuf && (strlen(strbuf) > 0)){
			channel = atoi(argv[2]);
			p_ip2ts = &tsoip_in_channel[channel];
			p_ip2ts->target_ip = ip_a2i(argv[3]);
			p_ip2ts->target_port = atoi(argv[4]);
			p_ip2ts->enable = atoi(argv[5]);
			for(i=6; i<argc; i++){
				idx = i-6;
				tsoip_in_backup.channel[channel].src_ipaddr[idx] = ip_a2i(argv[i]);
				if(tsoip_in_backup.channel[channel].src_ipaddr[idx]){
					tsoip_in_backup.channel[channel].src_mask |= 1<<idx;
				}
			}
			save_ipin_channel_backup_src_ip(channel);
			save_ipin_channel_backup_src_mask(channel);
			
			set_ipin_channel_with_backup(channel);
		}else{
			
			nif_no = atoi(argv[2]);
			channel = v_s32;
			p_ip2ts = &ip2ts;
		
			p_ip2ts->target_ip = ip_a2i(argv[4]);
			p_ip2ts->target_port = atoi(argv[5]);
			p_ip2ts->nif_no = nif_no;
			p_ip2ts->channel = channel;

			p_ip2ts->enable = atoi(argv[6]);

			p_ip2ts->mode = 1;
			if(argc>7){
				p_ip2ts->src_ipaddr = ip_a2i(argv[7]);
			}else{
				p_ip2ts->src_ipaddr = 0;
			}

			set_ipin_channel(nif_no, channel, p_ip2ts, p_ip2ts->src_ipaddr, p_ip2ts->enable);
		}
		
		
	}else if(strcmp("status", argv[1]) == 0){

		if(argc<4){
			return 1;
		}
		
		nif_no = atoi(argv[2]);
		channel = atoi(argv[3]);
		bitrate = read_ipin_channel_bitrate(nif_no, channel);
		srcip = read_ipin_channel_srcip(nif_no, channel);

		printf("%d nif %d channel bitrate = %d , srcip = %#.8x\n", nif_no, channel, bitrate, srcip);
	}
	else if(strcmp("period", argv[1]) == 0){
		if(argc<3){
			return 1;
		}
		//update_tsoip_in_backup_test_period(atoi(argv[2]));
		save_ipin_backup();
				
	}
	else if(strcmp("monitor", argv[1]) == 0){

		//create_detached_pthread(monitor_tsoip_in, NULL,  SZ_1M);
		
	}

	 return 0;
}




#include "../appref.h"
//#include "tsoip.h"
//#include "tsoip_backup.h"


void init_tsoip_in_backup(void)
{
	int i;

	tsoip_in_backup.backup_mode = 0;
	tsoip_in_backup.test_period = 600;

	for(i=0; i<tsoip_in_backup.channel_qty; i++){
		
		tsoip_in_backup.channel[i].backup_status = 0;
		tsoip_in_backup.channel[i].used_src_idx = 0;
		tsoip_in_backup.channel[i].cur_src_status = 1;
		tsoip_in_backup.channel[i].main_src_status = 1;
		tsoip_in_backup.channel[i].src_mask = 1;
		
		tsoip_in_backup.channel[i].lower_bitrate = 0;
		tsoip_in_backup.channel[i].upper_bitrate = 0;

		init_list(&tsoip_in_backup.channel[i].user_list, POINTER_DATA);
		
	}
	
}


void set_ipin_channel_with_backup(int channel)
{
	int  nif_no, ref_chan;
	tsoip_in_channel_backup_st *p_backup_channel;
	tsoip_in_channel_st *ipin_channel;

		
	ipin_channel = &tsoip_in_channel[channel];


	if(tsoip_in_backup.backup_mode){
		
		p_backup_channel = &tsoip_in_backup.channel[channel];

		ref_chan =  channel+tsoip_network[0].inchan_qty;
	
	//	if(p_backup_channel->src_idx_0 == 0)
	//	if(p_backup_channel->src_idx_1 == 0)
	
		p_backup_channel->used_src_idx = 0;

		if(ipin_channel->enable){
			set_ipin_channel(0, channel, ipin_channel, p_backup_channel->src_ipaddr[0], 1);
			set_ipin_channel(0, ref_chan, ipin_channel, p_backup_channel->src_ipaddr[1], p_backup_channel->src_mask&(1<<1));
			set_ipin_channel(1, channel, ipin_channel, p_backup_channel->src_ipaddr[2], p_backup_channel->src_mask&(1<<2));
			set_ipin_channel(1, ref_chan, ipin_channel, p_backup_channel->src_ipaddr[3], p_backup_channel->src_mask&(1<<3));
		}else{
			set_ipin_channel(0, channel, ipin_channel, 0, 0);
			set_ipin_channel(0, ref_chan, ipin_channel, 0, 0);
			set_ipin_channel(1, channel, ipin_channel, 0, 0);
			set_ipin_channel(1, ref_chan, ipin_channel, 0, 0);
		}
		
	}else{
		set_tsoip_in_channel(channel);
	}
	
	save_db_blob(tsoip_in_param, channel, &tsoip_in_channel[channel]);
	
}


#ifdef SUBROUTINE
///------------------------------

void switch_transceiver_ipin_src(int in_chan, int src_nif, int to_nif)
{
	list_st *ipin_used_list;
	trans_route_st *trans_route;

	ipin_used_list = &tsoip_in_backup.channel[in_chan].user_list;

	TagPrintf(in_chan);
	TagPrintf(src_nif);
	TagPrintf(to_nif);


	trans_route = list_access(ipin_used_list, 0);
	while(trans_route){
		if((trans_route->trans_chan>>8) == (9+src_nif)){
			del_transceiver_route(trans_route->board_slot, trans_route->route_id);
			add_transceiver_route(trans_route->board_slot, trans_route->route_id, sysinfo->board_qty, ((to_nif+9)<<8)+in_chan);
			config_transceiver(trans_route->board_slot);
		}
		trans_route = list_next(ipin_used_list);
	}
	list_access_end(ipin_used_list);
}

int switch_ipin_src(int in_chan, int src_idx)
{
	int nif_no, last_src_idx, last_nif_no;
	tsoip_in_channel_backup_st *p_backup_channel;
	tsoip_in_channel_st *ipin_channel;

	
	p_backup_channel = &tsoip_in_backup.channel[in_chan];

	TagPrintf(in_chan);
	TagPrintf(src_idx);
	
	do{
		if(src_idx>3){
			return -1;
		}

		if(p_backup_channel->src_mask&(1<<src_idx)){
			break;
		}else{
			src_idx++;
		}
	}while(1);

	
	TagPrintf(in_chan);
	TagPrintf(src_idx);

	nif_no = src_idx/2;
	last_src_idx = p_backup_channel->used_src_idx;
	last_nif_no = last_src_idx/2;

	ipin_channel = &tsoip_in_channel[in_chan];
		
	if(tsoip_in_backup.backup_mode == 2){
		imr_drop_src_membership(tsoip_network[last_nif_no].ipaddr, tsoip_in_channel[in_chan].target_ip
				, p_backup_channel->src_ipaddr[last_src_idx]);

		set_ipin_channel(nif_no, in_chan, ipin_channel, p_backup_channel->src_ipaddr[src_idx], 1);
		if(src_idx==1){
			p_backup_channel->src_idx_0 = 1;
			set_ipin_channel(nif_no, in_chan+tsoip_network[nif_no].inchan_qty, ipin_channel, p_backup_channel->src_ipaddr[0], 1);
		}

		imr_add_src_membership(tsoip_network[nif_no].ipaddr, tsoip_in_channel[in_chan].target_ip
			, p_backup_channel->src_ipaddr[src_idx]);

	
	}else if(tsoip_in_backup.backup_mode == 1){
	
		if(src_idx==1){
			set_ipin_channel(nif_no, in_chan, ipin_channel, p_backup_channel->src_ipaddr[src_idx], 1);
			set_ipin_channel(nif_no, in_chan+tsoip_network[nif_no].inchan_qty, ipin_channel, p_backup_channel->src_ipaddr[0], 1);
			p_backup_channel->src_idx_0 = 1;
		}else if(src_idx==2){
			if(p_backup_channel->src_idx_1==1){
				set_ipin_channel(nif_no, in_chan, ipin_channel, p_backup_channel->src_ipaddr[src_idx], 1);
				set_ipin_channel(nif_no, in_chan+tsoip_network[nif_no].inchan_qty, ipin_channel, p_backup_channel->src_ipaddr[3], 1);
				p_backup_channel->src_idx_1==0;
			}
		}else if(src_idx==3){
			if(p_backup_channel->src_idx_1==0){
				set_ipin_channel(nif_no, in_chan, ipin_channel, p_backup_channel->src_ipaddr[src_idx], 1);
				set_ipin_channel(nif_no, in_chan+tsoip_network[nif_no].inchan_qty, ipin_channel, p_backup_channel->src_ipaddr[2], 1);
				p_backup_channel->src_idx_1==1;
			}
		}
	}
	
	p_backup_channel->used_src_idx = src_idx;
	
	if(last_nif_no != nif_no){
		switch_transceiver_ipin_src(in_chan, last_nif_no, nif_no);
	}

	return 0;
}


void check_tsoip_in_main_src(void)
{
	int in_chan, nif_no, ref_chan;
	tsoip_in_channel_backup_st *p_backup_channel;
	tsoip_in_channel_st *ipin_channel;
	struct timeval stime;

	gettimeofday(&stime, NULL);
	
	
	//TagPrintf(0);
	
	for(in_chan=0; in_chan<tsoip_in_backup.channel_qty; in_chan++){
		
		p_backup_channel = &tsoip_in_backup.channel[in_chan];

		if(p_backup_channel->used_src_idx == 0){
			continue;		
		}

		nif_no = p_backup_channel->used_src_idx/2;

		if(tsoip_in_backup.backup_mode == 2){
			imr_add_src_membership(tsoip_network[0].ipaddr, tsoip_in_channel[in_chan].target_ip
				, p_backup_channel->src_ipaddr[0]);
		}

		//--------------
		ref_chan =  in_chan+tsoip_network[0].inchan_qty;
		
		ipin_channel = &tsoip_in_channel[in_chan];
		

		if(tsoip_in_backup.backup_mode == 2){
			
				//set_ipin_channel(nif_no, ref_chan, ipin_channel, p_backup_channel->src_ipaddr[0], 1);
				//sleep(1);

			
		}

		p_backup_channel->bitrate[0] = read_ipin_channel_bitrate(0, ref_chan);

		if((p_backup_channel->bitrate[0]>p_backup_channel->lower_bitrate) && (p_backup_channel->bitrate[0]<p_backup_channel->upper_bitrate)){
			p_backup_channel->main_src_status = 1;
		}else{
			p_backup_channel->main_src_status = 0;
		}
		
		if(p_backup_channel->main_src_status){

			printf("check %d channel  main src  go back to normal \n", in_chan);
	
			if(tsoip_in_backup.backup_mode == 2){
				imr_drop_src_membership(tsoip_network[nif_no].ipaddr, tsoip_in_channel[in_chan].target_ip
					, p_backup_channel->src_ipaddr[p_backup_channel->used_src_idx]);
			}

			if(tsoip_in_backup.backup_mode == 2){
				set_ipin_channel(0, in_chan, ipin_channel, p_backup_channel->src_ipaddr[0], 1);
				//set_ipin_channel(0, ref_chan, ipin_channel, p_backup_channel->src_ipaddr[1], 1);
				TagPrintf(in_chan);
			}else if(tsoip_in_backup.backup_mode == 1){
				//if(p_backup_channel->used_src_idx == 1){
					set_ipin_channel(0, in_chan, ipin_channel, p_backup_channel->src_ipaddr[0], 1);
					set_ipin_channel(0, ref_chan, ipin_channel, p_backup_channel->src_ipaddr[1], 1);
					
					TagPrintf(in_chan);
				//}
			}

			p_backup_channel->src_idx_0 = 0;
			
			if(p_backup_channel->used_src_idx>1){
				switch_transceiver_ipin_src(in_chan, 1, 0);
			}
			
			p_backup_channel->used_src_idx = 0;
			
		}else{
			
			imr_drop_src_membership(tsoip_network[nif_no].ipaddr, tsoip_in_channel[in_chan].target_ip
				, p_backup_channel->src_ipaddr[0]);
		}

	}

	//show_exectime(&stime, "check_tsoip_in_main_src");
	
}

void update_tsoip_in_backup_test_period(int time)
{
	TagPrintf(time);
	tsoip_in_backup.test_period = time;
	update_period_task(&(tsoip_in_backup.periord_task), tsoip_in_backup.test_period*1000);
}


void monitor_tsoip_in(void)
{
	int in_chan, ref_chan, nif_no;
	int i, src_idx;
	tsoip_in_channel_backup_st *p_backup_channel;

	tsoip_in_backup.periord_task.handler = check_tsoip_in_main_src;
	tsoip_in_backup.periord_task.arg = NULL;
	tsoip_in_backup.periord_task.run_flag = 0;
	init_timeout(&tsoip_in_backup.periord_task.timeout, tsoip_in_backup.test_period);

	create_detached_pthread(start_period_task, &tsoip_in_backup.periord_task, SZ_1M);

	printf("start monitor_tsoip_in task\n");

	while(sysinfo->run_flag){

		if(tsoip_in_backup.backup_mode == 0){
				usleep(100000);
				continue;
		}
		
		for(in_chan=0; in_chan<tsoip_in_backup.channel_qty; in_chan++){

			if(tsoip_in_channel[in_chan].enable == 0){
				continue;
			}

			p_backup_channel = &tsoip_in_backup.channel[in_chan];

			src_idx = p_backup_channel->used_src_idx;
			nif_no = src_idx / 2;
			ref_chan = in_chan + tsoip_network[nif_no].inchan_qty;

			if(tsoip_in_backup.backup_mode == 2){
					p_backup_channel->bitrate[src_idx] = read_ipin_channel_bitrate(nif_no, in_chan);
			}else if(tsoip_in_backup.backup_mode == 1){
				if(p_backup_channel->src_idx_0 == 0){
					p_backup_channel->bitrate[0] = read_ipin_channel_bitrate(0, in_chan);
					p_backup_channel->bitrate[1] = read_ipin_channel_bitrate(0, ref_chan);
				}else{
					p_backup_channel->bitrate[1] = read_ipin_channel_bitrate(0, in_chan);
					p_backup_channel->bitrate[0] = read_ipin_channel_bitrate(0, ref_chan);
				}

				if(p_backup_channel->src_idx_1 == 0){
					p_backup_channel->bitrate[2] = read_ipin_channel_bitrate(1, in_chan);
					p_backup_channel->bitrate[3] = read_ipin_channel_bitrate(1, ref_chan);
				}else{
					p_backup_channel->bitrate[3] = read_ipin_channel_bitrate(1, in_chan);
					p_backup_channel->bitrate[2] = read_ipin_channel_bitrate(1, ref_chan);
				}
			}else if(tsoip_in_backup.backup_mode ==0){
					break;
			}

			if((p_backup_channel->bitrate[src_idx]>p_backup_channel->lower_bitrate) && (p_backup_channel->bitrate[src_idx]<p_backup_channel->upper_bitrate)){
				p_backup_channel->cur_src_status = 1;
			}else{
				p_backup_channel->cur_src_status = 0;
			}

			if(src_idx == 0){
				p_backup_channel->main_src_status = p_backup_channel->cur_src_status;
			}
			
			if(p_backup_channel->cur_src_status){

			}else{
				
				if(switch_ipin_src(in_chan, p_backup_channel->used_src_idx+1)<0){
					p_backup_channel->backup_status = IP_BACKUP_ERR;
				}
				
			}
			
			
		}

		sched_yield();
		sleep(tsoip_in_backup.test_period);
		
	}
}
#endif



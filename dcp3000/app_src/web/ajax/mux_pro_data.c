
#include "../../appref.h"
#include "../../event/board_event.h"
#include "../../event/eventlog.h"

#include "mux.h"
#include "mux_pro_data.h"

#include <sys/time.h>
#include <time.h>
#include <iconv.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../mux/mux.h"

#include "../../utils/convert_code.h"

#define value_len 1024
char *web_json_text;
/*
成功返回0
*/
int my_system(const char * cmd) 
{ 
	FILE * fp; 
	int res; 
	char buf[1024] = {0};
	if(cmd == NULL) 
	{ 
		printf("my_system cmd is NULL!\n");
		return -1;
	} 
	if((fp = popen(cmd, "r")) == NULL)
	{ 
		perror("popen");
 		printf("popen error: %s\n", strerror(errno)); 
 		return -1; 
	} 
	else
 	{
 		while(fgets(buf, sizeof(buf), fp)) 
		{ 
			printf("%s\n", buf); 
		} 
		if((res = pclose(fp)) == -1)
		{ 
			printf("close popen file pointer fp error!\n"); 
			return res;
 		} 
		else if(res == 0) 
		{
 			return res;
 		} 
		else 
		{ 
			printf("popen res is :%d\n", res); 
			return res; 
		} 
	}
} 
v_32 Get_Hardware_Status(cJSON *fmt){
	int value_item[MAX_FAN_QTY];
	int i;
	v_u32 temp_32;
	cJSON *fmt1, *dim1, *dim2;
		
	cJSON_AddItemToObject(fmt, "Hardware_status", fmt1=cJSON_CreateObject());
		
	cJSON_AddItemToObject(fmt1, "value", dim1=cJSON_CreateObject());
 	cJSON_AddNumberToObject(dim1, "arm_temperature", sysinfo->arm_temperature);
 	cJSON_AddNumberToObject(dim1, "fpga_temperature", sysinfo->fpga_temperature);
 	for(i=0; i<sysinfo->fan_qty; i++)
 	{
 		value_item[i] = sysinfo->fan_speed[i];
 	}
	cJSON_AddItemToObject(dim1, "fan_speed", cJSON_CreateIntArray(value_item, sysinfo->fan_qty));
 	
 	memset(value_item, 0, sizeof(value_item));
 	for(i=0; i<sysinfo->power_qty; i++){
		value_item[i] = sysinfo->power_supply[i];
 	}
	cJSON_AddItemToObject(dim1, "power_status", cJSON_CreateIntArray(value_item, sysinfo->power_qty));
	return 0;
}

v_32 Get_IPIN_Status(cJSON *fmt){
	int i, j, nifno,srcidx;
	int idx;
	cJSON *fmt1, *dim1, *dim2;

	cJSON_AddItemToObject(fmt, "IPIN_Staus", fmt1=cJSON_CreateObject());

	
	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateArray());


	for(idx=0; idx<tsoip_in_backup.channel_qty; idx++){

		i = ipin_idx_ref[idx];
		if(i<0){
			break;
		}

		if(tsoip_in_backup.backup_mode == 1){
			
			for(j=0; j<4; j++){
				if(tsoip_in_backup.channel[i].src_mask & (1<<j)){

					cJSON_AddItemToArray(dim1,dim2=cJSON_CreateObject());
					cJSON_AddNumberToObject(dim2, "channel", idx);
					cJSON_AddNumberToObject(dim2, "channel_license", ipin_status[i].channel_license);
					cJSON_AddNumberToObject(dim2, "format", ipin_status[i].format);
					cJSON_AddNumberToObject(dim2, "lsize", ipin_status[i].lsize);
					cJSON_AddNumberToObject(dim2, "dsize", ipin_status[i].dsize);

					srcidx = j;
					nifno = srcidx/2;
					
					
					cJSON_AddStringToObject(dim2, "src_ipaddr", ip_i2a(tsoip_in_backup.channel[i].src_ipaddr[srcidx]));
					cJSON_AddNumberToObject(dim2, "bitrate", tsoip_in_backup.channel[i].bitrate[srcidx]);

					
					cJSON_AddNumberToObject(dim2, "ts_byterrate", ipin_status[i].ts_byterrate);
					cJSON_AddNumberToObject(dim2, "udp_length", ipin_status[i].udp_length);
					cJSON_AddNumberToObject(dim2, "recovery_packets", ipin_status[i].recovery_packets);
					cJSON_AddNumberToObject(dim2, "ts_flag", ipin_status[i].ts_flag);
					cJSON_AddNumberToObject(dim2, "lost_packets", ipin_status[i].lost_packets);
					cJSON_AddNumberToObject(dim2, "channel_lock", tsoip_in_backup.channel[i].channel_lock[srcidx]);

					cJSON_AddNumberToObject(dim2, "SI_ok", ipin_status[i].SI_ok);

					cJSON_AddNumberToObject(dim2, "nif_no", nifno);
					cJSON_AddNumberToObject(dim2, "target_port", tsoip_in_channel[i].target_port);
					cJSON_AddNumberToObject(dim2, "enable", (tsoip_in_backup.channel[i].src_mask & (1<<srcidx))?1:0);
					
				}
			}
		
		}else if(tsoip_in_backup.backup_mode == 2){

			cJSON_AddItemToArray(dim1,dim2=cJSON_CreateObject());
			cJSON_AddNumberToObject(dim2, "channel", idx);
			cJSON_AddNumberToObject(dim2, "channel_license", ipin_status[i].channel_license);
			cJSON_AddNumberToObject(dim2, "format", ipin_status[i].format);
			cJSON_AddNumberToObject(dim2, "lsize", ipin_status[i].lsize);
			cJSON_AddNumberToObject(dim2, "dsize", ipin_status[i].dsize);

			srcidx = tsoip_in_backup.channel[i].used_src_idx;
			nifno = srcidx/2;
			
			//TagPrintf(srcidx);
			
			cJSON_AddStringToObject(dim2, "src_ipaddr", ip_i2a(tsoip_in_backup.channel[i].src_ipaddr[srcidx]));
			cJSON_AddNumberToObject(dim2, "bitrate", tsoip_in_backup.channel[i].bitrate[srcidx]);

			
			cJSON_AddNumberToObject(dim2, "ts_byterrate", ipin_status[i].ts_byterrate);
			cJSON_AddNumberToObject(dim2, "udp_length", ipin_status[i].udp_length);
			cJSON_AddNumberToObject(dim2, "recovery_packets", ipin_status[i].recovery_packets);
			cJSON_AddNumberToObject(dim2, "ts_flag", ipin_status[i].ts_flag);
			cJSON_AddNumberToObject(dim2, "lost_packets", ipin_status[i].lost_packets);
			cJSON_AddNumberToObject(dim2, "channel_lock", tsoip_in_backup.channel[i].channel_lock[srcidx]);

			cJSON_AddNumberToObject(dim2, "SI_ok", ipin_status[i].SI_ok);

			cJSON_AddNumberToObject(dim2, "nif_no", nifno);
			cJSON_AddNumberToObject(dim2, "target_port", tsoip_in_channel[i].target_port);
			cJSON_AddNumberToObject(dim2, "enable", (tsoip_in_backup.channel[i].src_mask & (1<<srcidx))?1:0);
			
		}else{
			//TagPrintf(i);
		
			cJSON_AddItemToArray(dim1,dim2=cJSON_CreateObject());
			cJSON_AddNumberToObject(dim2, "channel", idx);
			cJSON_AddNumberToObject(dim2, "channel_license", ipin_status[i].channel_license);
			cJSON_AddNumberToObject(dim2, "format", ipin_status[i].format);
			cJSON_AddNumberToObject(dim2, "lsize", ipin_status[i].lsize);
			cJSON_AddNumberToObject(dim2, "dsize", ipin_status[i].dsize);

			
			
			cJSON_AddNumberToObject(dim2, "ts_byterrate", ipin_status[i].ts_byterrate);
			cJSON_AddNumberToObject(dim2, "udp_length", ipin_status[i].udp_length);
			cJSON_AddNumberToObject(dim2, "recovery_packets", ipin_status[i].recovery_packets);
			cJSON_AddNumberToObject(dim2, "ts_flag", ipin_status[i].ts_flag);
			cJSON_AddNumberToObject(dim2, "lost_packets", ipin_status[i].lost_packets);
			cJSON_AddNumberToObject(dim2, "channel_lock", ipin_status[i].channel_lock);

			cJSON_AddNumberToObject(dim2, "SI_ok", ipin_status[i].SI_ok);

			cJSON_AddNumberToObject(dim2, "nif_no", tsoip_in_channel[i].nif_no);
			cJSON_AddNumberToObject(dim2, "target_port", tsoip_in_channel[i].target_port);
			cJSON_AddNumberToObject(dim2, "enable", tsoip_in_channel[i].enable);

			
			cJSON_AddStringToObject(dim2, "src_ipaddr", ip_i2a(ipin_status[i].src_ipaddr));
			cJSON_AddNumberToObject(dim2, "bitrate", ipin_status[i].bitrate);
			
		}
		
		
	}
	return 0;
}

v_32 Get_IPOUT_Status(cJSON *fmt){
	int value_item[MAX_TSOIP_OUT_CHANNEL_QTY];
	int i;
	v_u32 temp_32;
	cJSON *fmt1, *dim1, *dim2;
		
	cJSON_AddItemToObject(fmt, "IPOUT_Staus", fmt1=cJSON_CreateObject());
	for(i=0; i<sysinfo->ipout_channels; i++)
		value_item[i] = 1;
	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateIntArray(value_item, sysinfo->ipout_channels));

	//cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateArray());
	for(i=0; i<sysinfo->ipout_channels; i++)
	{
  	        //cJSON_ReplaceItemInArray(dim1, i, dim2=cJSON_CreateObject());
		cJSON_AddItemToArray(dim1, dim2=cJSON_CreateObject());
		cJSON_AddNumberToObject(dim2, "channel_license", ipout_status[i].channel_license);
		cJSON_AddNumberToObject(dim2, "input_format", ipout_status[i].input_format);
		cJSON_AddNumberToObject(dim2, "ts_packets", ipout_status[i].ts_packets);
		cJSON_AddNumberToObject(dim2, "cfec_packets", ipout_status[i].cfec_packets);
		cJSON_AddNumberToObject(dim2, "rfec_packets", ipout_status[i].rfec_packets);
		cJSON_AddNumberToObject(dim2, "input_bitrate", ipout_status[i].input_bitrate);
		
		cJSON_AddNumberToObject(dim2, "encapsul_type", tsoip_out_channel[i].encapsul_type[0]);
	}	
	return 0;
}
v_32 Get_Transceiver_Status(cJSON *fmt){
	int value_item[MAX_TRANSCEIVER_QTY];
	int i;
	v_u32 temp_32;
	cJSON *fmt1, * dim1, *dim2;
		
	cJSON_AddItemToObject(fmt, "Transceiver_Staus", fmt1=cJSON_CreateObject());
	for(i=0; i<sysinfo->board_qty; i++)
		value_item[i] = 1;
//	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateIntArray(value_item, sysinfo.board_qty));

	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateArray());
	for(i=0; i<sysinfo->board_qty; i++)
	{
	//	cJSON_ReplaceItemInArray(dim1, i, dim2=cJSON_CreateObject());
		cJSON_AddItemToArray(dim1, dim2=cJSON_CreateObject());
		cJSON_AddNumberToObject(dim2, "trans_id", i);
		cJSON_AddNumberToObject(dim2, "run_flag", transceiver[i].run_flag);
		cJSON_AddNumberToObject(dim2, "tx_bitrate", transceiver[i].tx_bitrate);
		cJSON_AddNumberToObject(dim2, "rx_bitrate", transceiver[i].rx_bitrate);
	}
	return 0;
}

v_32 Get_Deviceinfo_func(cJSON *fmt){
	int value_item[20];
	v_u32 temp_32;
	cJSON *fmt1, *dim1;
	
	DEBUG_N_PRINT("#### form_RRev_Init\n");
	
	cJSON_AddItemToObject(fmt, "INIT", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", INIT);

	cJSON_AddItemToObject(fmt1, "value", dim1=cJSON_CreateObject());
 	cJSON_AddStringToObject(dim1, "device_name", sysinfo->device_name);
 	cJSON_AddStringToObject(dim1, "SN", sysinfo->device_sn);

	DEBUG_PRINT("dev name =%s\n", sysinfo->device_name);
	DEBUG_PRINT("dev sn =%s\n", sysinfo->device_sn);
	
	DEBUG_N_PRINT("#### form_RRev_Init end\n\n");
	
	return 0;
}

v_32 Get_Upgrade_Percent_func(cJSON *fmt, int board_id){ // board_id: 0-mainboard  1~7-subboard
	v_u32 temp_32;
	cJSON *fmt1, *dim1;
	static int flag = -1;
	static int status = -1;
	int status_change = 0;
	if(upgrade_status != NULL){
		cJSON_AddItemToObject(fmt, "UPGRADE_PERCENT", fmt1=cJSON_CreateObject());
		
		cJSON_AddItemToObject(fmt1, "value", dim1=cJSON_CreateObject());
	 	cJSON_AddNumberToObject(dim1, "flag", upgrade_status[board_id].flag);
	 	cJSON_AddNumberToObject(dim1, "status", upgrade_status[board_id].status);
	 	cJSON_AddNumberToObject(dim1, "progress_ratio", upgrade_status[board_id].progress_ratio);
		
		cJSON_AddStringToObject(dim1, "status_msg", upgrade_status[board_id].status_msg);

		if(status != upgrade_status[board_id].status){
			status = upgrade_status[board_id].status;
			status_change = 1;
		}
		
		if(flag != upgrade_status[board_id].flag){
			status_change = 1;
			flag = upgrade_status[board_id].flag;
		}
	 	cJSON_AddNumberToObject(dim1, "status_change", status_change);
	 	if(board_id > 0){
	 		cJSON_AddNumberToObject(dim1, "run_flag", sysinfo->slot_info[board_id-1].run_flag);
	 		cJSON_AddNumberToObject(dim1, "slot_status", sysinfo->slot_status[board_id-1]);
	 	}else{
	 		cJSON_AddNumberToObject(dim1, "run_flag", sysinfo->run_flag);
	 	}
	 	//printf("flag = %d, status = %d, progress_ratio = %d\n", upgrade_status[0].flag, upgrade_status[0].status, upgrade_status[0].progress_ratio);
	}
	return 0;
}
//#define random(x) (rand()%x)

v_32 Get_BoardInfo_func(cJSON *fmt){
	int value_item[value_len];
	v_u32 temp_32 ,i ,j, k, m;
	cJSON *fmt1, * dim1, *dim2, *dim3;
	cJSON *fuc_list_fmt, *channel_list_fmt, *dim4, *dim5;
	board_st *board;
	funcmod_st *func;
	funcchan_st *chan;
	int trans_chan = 0;
	sArrList_st *deviflist;
	sArrList_st *funclist;
	sArrList_st *chanlist;
	
	cJSON_AddItemToObject(fmt, "BOARD_INFO", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", CARD_INFO);
	cJSON_AddItemToObject(fmt1, "value", dim1=cJSON_CreateObject());
 	cJSON_AddStringToObject(dim1, "name", "card_status");
	cJSON_AddNumberToObject(dim1, "count", CARD_LEN);

	memset(value_item, 0, sizeof(value_item));
        //cJSON_AddItemToObject(dim1, "messages", dim2=cJSON_CreateIntArray(value_item, CARD_LEN));
	cJSON_AddItemToObject(dim1, "messages", dim2 = cJSON_CreateArray());
	for(i=0; i<CARD_LEN; i++)
	{
                //cJSON_ReplaceItemInArray(dim2, i, dim3=cJSON_CreateObject()); 
		cJSON_AddItemToArray(dim2, dim3=cJSON_CreateObject());
		board = get_boardinfo(i);

		if(board == NULL){
			cJSON_AddNumberToObject(dim3, "slot_id", i+1);
	    		cJSON_AddNumberToObject(dim3, "status_flag", 0);
	    		cJSON_AddNumberToObject(dim3, "type", 0);
	    		cJSON_AddStringToObject(dim3, "name", ""); 
		}else{
		
			cJSON_AddNumberToObject(dim3, "slot_id", board->slot+1);

	    		cJSON_AddNumberToObject(dim3, "status_flag", sysinfo->slot_status[i]);
	    		cJSON_AddNumberToObject(dim3, "type", board->type);
	    		cJSON_AddStringToObject(dim3, "name", board->name);
	    		cJSON_AddNumberToObject(dim3, "version", board->version);

			deviflist = board_deviflist(board);
			funclist = board_funclist(board);
			
	    		if(funclist != NULL)
	    		{
		    		cJSON_AddNumberToObject(dim3, "func_count", funclist->count);
				
				for(m=0; m<funclist->count; m++)
					value_item[m] = 1;
				trans_chan = 0;
                                //cJSON_AddItemToObject(dim3, "func_list", dim4=cJSON_CreateIntArray(value_item, board->func_list->count)); // ...
				cJSON_AddItemToObject(dim3, "func_list", dim4 = cJSON_CreateArray());
				for(m=0; m<funclist->count; m++)
				{
	                                //cJSON_ReplaceItemInArray(dim4, m, fuc_list_fmt=cJSON_CreateObject()); 
					cJSON_AddItemToArray(dim4, fuc_list_fmt=cJSON_CreateObject());
					func = (funcmod_st *)sArrList_get(funclist, m);
					chanlist = func_chanlist(func);
					
					if((func != NULL) && (chanlist!= NULL))
					{
			    		cJSON_AddNumberToObject(fuc_list_fmt, "count", chanlist->count);  
						for(j=0; j<chanlist->count; j++)
							value_item[j] = 1;
					//	cJSON_AddItemToObject(fuc_list_fmt, "channle_list", dim5=cJSON_CreateIntArray(value_item, func->channel_list->count));  
						cJSON_AddItemToObject(fuc_list_fmt, "channle_list", dim5 = cJSON_CreateArray());
						for(j=0; j<chanlist->count; j++)
						{
							//cJSON_ReplaceItemInArray(dim5, j, channel_list_fmt=cJSON_CreateObject()); 
                                                 cJSON_AddItemToArray(dim5, channel_list_fmt=cJSON_CreateObject());
							chan = sArrList_get(chanlist, j);
							if(chan != NULL)
							{
								cJSON_AddNumberToObject(channel_list_fmt, "trans_mode", chan->trans_mode); 
								cJSON_AddNumberToObject(channel_list_fmt, "type", chan->type);   
						    		cJSON_AddNumberToObject(channel_list_fmt, "start_index", chan->start_index);
						    		cJSON_AddNumberToObject(channel_list_fmt, "end_index", chan->end_index);

/*
								if((chan->trans_mode & TRANS_SRC)){
									for(k=chan->start_index; k<chan->end_index;k++){
								
										value_item[k] = (i<<8) | trans_chan;
										trans_chan++;
									}
									cJSON_AddItemToObject(channel_list_fmt, "trans_chan", cJSON_CreateIntArray(value_item,k));
								}
*/
							}
						}
					}
				}
			}
		}
	}
		
	return 0;
}


v_32 Get_SysPara_func(cJSON *fmt){
	int value_item[2];
	v_u32 temp_32;
	cJSON  *fmt1,* dim1;
		
	cJSON_AddItemToObject(fmt, "SYS_PARA", fmt1=cJSON_CreateObject());   	
	cJSON_AddNumberToObject(fmt1, "form_id", SYS_PARA);  
	cJSON_AddItemToObject(fmt1, "value", dim1=cJSON_CreateObject());
     //	cJSON_AddStringToObject(dim1,"device_name", cJSON_CreateString(name));   
 	cJSON_AddStringToObject(dim1, "device_name", sysinfo->device_name);   
	 	
	return 0;
}


v_32 Set_SysPara_func(v_u32 id, cJSON *val_arr_wrap){
	int size;
	char name[DEV_NAME_LENGTH];
	cJSON *dim1;
			
	size = cJSON_GetArraySize(val_arr_wrap);

	dim1 = cJSON_GetObjectItem(val_arr_wrap, "device_name");			
	sprintf(name,"%s", dim1->valuestring);
	name[DEV_NAME_LENGTH-1] = 0;
	DE_GIVE_PRINT("DEV_name=%s,", name);
	sprintf(sysinfo->device_name,"%s", dim1->valuestring);
	sysinfo->device_name[DEV_NAME_LENGTH-1] = 0;

	return size;
}

v_32  Get_ManNet_func(cJSON *fmt){
	int value_item[6],i=0;
	cJSON *fmt1, *dim1, *dim2,*dim3;
	
	cJSON_AddItemToObject(fmt, "MAN_NET", fmt1=cJSON_CreateObject());   	
	cJSON_AddNumberToObject(fmt1, "form_id", MAN_NET);  	
/*
	memset(&t_mng_network, 0 ,sizeof(msg_tx_mng_network_t));	
	 t_mng_network.trap_ip = mng_network.trap_ip;
	 t_mng_network.mac_high = (mng_network.mac_addr[0] << 8) | mng_network.mac_addr[1] ;
	 t_mng_network.mac_low = (mng_network.mac_addr[2] << 24) | (mng_network.mac_addr[3] << 16)  | (mng_network.mac_addr[4] << 8) | (mng_network.mac_addr[5] << 0);
	t_mng_network.ip_addr = mng_network.ipaddr;  
	t_mng_network.net_mask = mng_network.netmask;
	t_mng_network.gate_way = mng_network.gateway;	
*/	
	cJSON_AddItemToObject(fmt1, "value", dim1=cJSON_CreateObject());		

	value_item[IPS_IP0] = (mng_network->ipaddr>>24)&0xff;
	value_item[IPS_IP1] = (mng_network->ipaddr>>16)&0xff;
	value_item[IPS_IP2] = (mng_network->ipaddr>>8)&0xff;
	value_item[IPS_IP3] = mng_network->ipaddr &0xff;
	cJSON_AddItemToObject(dim1, "ip_addr", cJSON_CreateIntArray(value_item, 4));

	value_item[MACS_MAC0] = mng_network->macaddr[0]&0xff;
	value_item[MACS_MAC1] = mng_network->macaddr[1]&0xff;
	value_item[MACS_MAC2] = mng_network->macaddr[2]&0xff;
	value_item[MACS_MAC3] = mng_network->macaddr[3]&0xff;
	value_item[MACS_MAC4] = mng_network->macaddr[4]&0xff;
	value_item[MACS_MAC5] = mng_network->macaddr[5]&0xff;
	cJSON_AddItemToObject(dim1, "mac_addr", cJSON_CreateIntArray(value_item, MACS_LEN)); 

	value_item[IPS_IP0] = mng_network->netmask>>24&0xff;
	value_item[IPS_IP1] = mng_network->netmask>>16&0xff;
	value_item[IPS_IP2] = mng_network->netmask>>8&0xff;
	value_item[IPS_IP3] = mng_network->netmask&0xff;
	cJSON_AddItemToObject(dim1, "net_mask", cJSON_CreateIntArray(value_item, 4)); 

	value_item[IPS_IP0] = mng_network->gateway>>24&0xff;
	value_item[IPS_IP1] = mng_network->gateway>>16&0xff;
	value_item[IPS_IP2] = mng_network->gateway>>8&0xff;
	value_item[IPS_IP3] = mng_network->gateway&0xff;
	cJSON_AddItemToObject(dim1, "gw_addr", cJSON_CreateIntArray(value_item, 4)); 

	value_item[IPS_IP0] = mng_network->trapip>>24&0xff;
	value_item[IPS_IP1] = mng_network->trapip>>16&0xff;
	value_item[IPS_IP2] = mng_network->trapip>>8&0xff;
	value_item[IPS_IP3] = mng_network->trapip&0xff;
	cJSON_AddItemToObject(dim1, "trap_addr", cJSON_CreateIntArray(value_item, 4)); 
	
	value_item[IPS_IP0] = sysinfo->ntp_hostip>>24&0xff;
	value_item[IPS_IP1] = sysinfo->ntp_hostip>>16&0xff;
	value_item[IPS_IP2] = sysinfo->ntp_hostip>>8&0xff;
	value_item[IPS_IP3] = sysinfo->ntp_hostip&0xff;
	cJSON_AddItemToObject(dim1, "ntp_hostip", cJSON_CreateIntArray(value_item, 4)); 

	cJSON_AddNumberToObject(dim1, "watchdog", sysinfo->watchdog_enable);  

	cJSON_AddItemToObject(dim1, "localIP", dim2 = cJSON_CreateArray());  


	for(i=0;i<sysinfo->tsip_port;i++)
	{  
		cJSON_AddItemToObject(dim2, "", dim3=cJSON_CreateObject());  
		value_item[IPS_IP0] = ( tsoip_network[i].ipaddr >>24)&0xff;
		value_item[IPS_IP1] = (tsoip_network[i].ipaddr>>16)&0xff;
		value_item[IPS_IP2] = (tsoip_network[i].ipaddr>>8)&0xff;
		value_item[IPS_IP3] = tsoip_network[i].ipaddr &0xff;
		cJSON_AddItemToObject(dim3, "ip_addr", cJSON_CreateIntArray(value_item, 4));

		value_item[IPS_IP0] = (tsoip_network[i].netmask >>24)&0xff;
		value_item[IPS_IP1] = (tsoip_network[i].netmask>>16)&0xff;
		value_item[IPS_IP2] = (tsoip_network[i].netmask>>8)&0xff;
		value_item[IPS_IP3] = tsoip_network[i].netmask &0xff;
		cJSON_AddItemToObject(dim3, "net_mask", cJSON_CreateIntArray(value_item, 4));
	}


	return 0;
}

v_32 Set_ManNet_func(v_u32 id, cJSON *val_arr_wrap){
	int size;
	msg_tx_mng_network_t t_mng_network;
	cJSON *dim1, *dim2, *dim3;
				
	memset(&t_mng_network, 0 ,sizeof(msg_tx_mng_network_t));			
	size = cJSON_GetArraySize(val_arr_wrap);

	dim1 = cJSON_GetObjectItem(val_arr_wrap, "ip_addr");
	//dim1 = cJSON_GetArrayItem(p_json_info->val_arr_wrap, MAN_NET_IP);
	t_mng_network.ip_addr = GET_INT_JARRAY(dim1, IPS_IP0)<<24;
	t_mng_network.ip_addr |= GET_INT_JARRAY(dim1, IPS_IP1)<<16;
	t_mng_network.ip_addr |= GET_INT_JARRAY(dim1, IPS_IP2)<<8;
	t_mng_network.ip_addr |= GET_INT_JARRAY(dim1, IPS_IP3);

/*		
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "mac_addr");
	t_mng_network.mac_addr[0] = GET_INT_JARRAY(dim1,MACS_MAC0);
	t_mng_network.mac_addr[1] = GET_INT_JARRAY(dim1,MACS_MAC1);
	t_mng_network.mac_addr[2] = GET_INT_JARRAY(dim1,MACS_MAC2);
	t_mng_network.mac_addr[3] = GET_INT_JARRAY(dim1,MACS_MAC3);
	t_mng_network.mac_addr[4] = GET_INT_JARRAY(dim1,MACS_MAC4);
	t_mng_network.mac_addr[5] = GET_INT_JARRAY(dim1,MACS_MAC5);
*/			

	//dim1 = cJSON_GetArrayItem(p_json_info->val_arr_wrap, MAN_NET_MASK);
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "net_mask");
	t_mng_network.net_mask = GET_INT_JARRAY(dim1, IPS_IP0)<<24;
	t_mng_network.net_mask |= GET_INT_JARRAY(dim1, IPS_IP1)<<16;
	t_mng_network.net_mask |= GET_INT_JARRAY(dim1, IPS_IP2)<<8;
	t_mng_network.net_mask |= GET_INT_JARRAY(dim1, IPS_IP3);

	//dim1 = cJSON_GetArrayItem(p_json_info->val_arr_wrap, MAN_NET_GATEWAY);
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "gw_addr");
	t_mng_network.gate_way = GET_INT_JARRAY(dim1, IPS_IP0)<<24;
	t_mng_network.gate_way |= GET_INT_JARRAY(dim1, IPS_IP1)<<16;
	t_mng_network.gate_way |= GET_INT_JARRAY(dim1, IPS_IP2)<<8;
	t_mng_network.gate_way |= GET_INT_JARRAY(dim1, IPS_IP3);

	//dim1 = cJSON_GetArrayItem(p_json_info->val_arr_wrap, MAN_NET_TRAP);
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "trap_addr");
	t_mng_network.trap_ip = GET_INT_JARRAY(dim1, IPS_IP0)<<24;
	t_mng_network.trap_ip |= GET_INT_JARRAY(dim1, IPS_IP1)<<16;
	t_mng_network.trap_ip |= GET_INT_JARRAY(dim1, IPS_IP2)<<8;
	t_mng_network.trap_ip |= GET_INT_JARRAY(dim1, IPS_IP3);

	DEBUG_PRINT("[t_mng_network] ip_addr=%x,mac_high=%x,mac_low=%x,mask=%x,gateway=%x,trap_ip=%x\n", t_mng_network.ip_addr, t_mng_network.mac_high,t_mng_network.mac_low,
t_mng_network.net_mask,t_mng_network.gate_way,t_mng_network.trap_ip);
	mng_network->trapip = t_mng_network.trap_ip;
	mng_network->ipaddr = t_mng_network.ip_addr;  
	mng_network->netmask = t_mng_network.net_mask;
	mng_network->gateway = t_mng_network.gate_way;			
//	attach_msg_data(p_ipcmsg, &t_mng_network, sizeof(msg_tx_mng_network_t));
	set_manage_network();
	return size;
}
v_32 Set_NTP_func(v_u32 id, cJSON *val_arr_wrap){
	int size;
	u_int ntp_hostip = 0;
	cJSON *dim1, *dim2, *dim3;
				
	size = cJSON_GetArraySize(val_arr_wrap);

	if((dim1 = cJSON_GetObjectItem(val_arr_wrap, "ntp_hostip")) != NULL){
		ntp_hostip = GET_INT_JARRAY(dim1, IPS_IP0)<<24;
		ntp_hostip |= GET_INT_JARRAY(dim1, IPS_IP1)<<16;
		ntp_hostip |= GET_INT_JARRAY(dim1, IPS_IP2)<<8;
		ntp_hostip |= GET_INT_JARRAY(dim1, IPS_IP3);
		DebugPrintf("start:%#x........\n", ntp_hostip);
		set_ntp_hostip(ntp_hostip);
		sync_board_systime();
		DebugPrintf("end:%#x........\n", ntp_hostip);
	}
	return size;
}
v_32 Set_DOG_func(v_u32 id, cJSON *val_arr_wrap){
	cJSON *dim1;
	dim1 = cJSON_GetArrayItem(val_arr_wrap, 0);
	printf("1----watchdog %d\n", dim1->valueint);
	if(dim1->valueint){
		watchdog_open();
	}else{
		watchdog_close();
	}
}

v_32 Get_Clk_func(cJSON *fmt){
	v_u32 year;
	v_16 month,day,hour,minute,second;
	v_u32 temp_32;
	cJSON *fmt1,  * dim1;
	struct tm *time_temp;
	time_t cur_time;		

	cJSON_AddItemToObject(fmt, "CLOCK", fmt1=cJSON_CreateObject());   	
	cJSON_AddNumberToObject(fmt1, "form_id", CLK_SET);  	

	cur_time = time(NULL);
	time_temp = localtime(&cur_time);	
	year = 1900 + time_temp->tm_year;
	month = 1 + time_temp->tm_mon;
	day = time_temp->tm_mday;
	hour = time_temp->tm_hour;
	minute = time_temp->tm_min;
				
	cJSON_AddItemToObject(fmt1,"value", dim1=cJSON_CreateObject());
	
	cJSON_AddNumberToObject(dim1, "year", year); 
	cJSON_AddNumberToObject(dim1, "month", month); 
	cJSON_AddNumberToObject(dim1, "day", day); 
	cJSON_AddNumberToObject(dim1, "hour", hour); 
	cJSON_AddNumberToObject(dim1, "minute", minute); 
	
	return 0;
}

v_32 Set_Clk_func(v_u32 id, cJSON *val_arr_wrap){
	int size;
	v_u32 year;
	v_16 month, day, hour, minute, second;
	cJSON *dim1,*dim2,*dim3;

	struct tm time_temp;	
		
	size = cJSON_GetArraySize(val_arr_wrap);

	dim1 = cJSON_GetObjectItem(val_arr_wrap, "year");
	year = dim1->valueint;
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "month");
	month = dim1->valueint;			
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "day");
	day = dim1->valueint;
	
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "hour");
	hour = dim1->valueint;
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "minute");
	minute = dim1->valueint;			
	
	time_temp.tm_year = year;
	time_temp.tm_mon = month;
	time_temp.tm_mday = day;
	time_temp.tm_hour = hour;
	time_temp.tm_min = minute;
	time_temp.tm_sec = 0;
	DebugPrintf("[time] %d-%d-%d %d:%d\n", year, month, day, hour, minute);

	update_all_board_systime(&time_temp);

	return size;
}


v_32 Get_Version_func(cJSON *fmt){
	int value_item[8] = {0}, i;
	cJSON *dim1, *dim2, *dim3, *dim4;
	
	cJSON_AddItemToObject(fmt, "VERSION", dim1=cJSON_CreateObject());
	cJSON_AddNumberToObject(dim1, "form_id", 11);
	cJSON_AddItemToObject(dim1, "value", dim2=cJSON_CreateObject());
	cJSON_AddNumberToObject(dim2, "main_ver", sysinfo->main_version);
	
	cJSON_AddNumberToObject(dim2, "web_ver", sysinfo->web_version);
	cJSON_AddNumberToObject(dim2, "APP_ver", sysinfo->app_version);
	cJSON_AddNumberToObject(dim2, "firm_ver", sysinfo->firmware_version);
	cJSON_AddNumberToObject(dim2, "FGPA_ver", sysinfo->fpga_version);
	cJSON_AddNumberToObject(dim2, "kernel_ver", sysinfo->kernel_version);

	memset(value_item, 0, sizeof(value_item));
        //cJSON_AddItemToObject(dim2, "card_ver", dim3=cJSON_CreateIntArray(value_item, CARD_LEN));
	cJSON_AddItemToObject(dim2, "card_ver", dim3 = cJSON_CreateArray());
	for(i=0; i<sysinfo->board_qty; i++)
	{
                //cJSON_ReplaceItemInArray(dim3, i, dim4=cJSON_CreateObject());
		cJSON_AddItemToArray(dim3, dim4=cJSON_CreateObject());
		cJSON_AddNumberToObject(dim4, "index", i);
		cJSON_AddNumberToObject(dim4, "FPGA", sysinfo->slot_info[i].web_version);
		cJSON_AddNumberToObject(dim4, "software_ver", sysinfo->slot_info[i].board_version);
	}	
	
	return 0;
}

v_32 Set_Login_func(cJSON *val_arr_wrap){
	int size;
	int web_set_login_flag = 0;
	v_u8 old_usename[DEV_NAME_LENGTH], old_passwd[DEV_NAME_LENGTH];
	v_u8 usename[DEV_NAME_LENGTH], passwd[DEV_NAME_LENGTH];
	cJSON *dim1;
			
	size = cJSON_GetArraySize(val_arr_wrap);

	dim1 = cJSON_GetObjectItem(val_arr_wrap, "old_usename");
	sprintf(old_usename, "%s", dim1->valuestring);
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "old_passwd");
	sprintf(old_passwd, "%s", dim1->valuestring);

	dim1 = cJSON_GetObjectItem(val_arr_wrap, "usename");
	sprintf(usename, "%s", dim1->valuestring);
	dim1 = cJSON_GetObjectItem(val_arr_wrap, "passwd");
	sprintf(passwd, "%s", dim1->valuestring);

	if(strcmp(sysinfo->loginuser, old_usename) == 0 && strcmp(sysinfo->loginpasswd, old_passwd) == 0){
		if(strcmp(sysinfo->loginuser, usename) != 0){
			strcpy(sysinfo->loginuser, usename);
			save_loginuser(sysinfo->loginuser);
		}
		
		if(strcmp(sysinfo->loginpasswd,passwd) != 0){
			strcpy(sysinfo->loginpasswd,passwd);
			save_loginpasswd(sysinfo->loginpasswd);
		}
		web_set_login_flag = 1;
	}
	return web_set_login_flag;
}	

v_32 Get_Iplocal_func(cJSON *fmt){
	int i;
	int value_item[7] = {0};
	cJSON *dim1, *dim2, *fmt1,*dim3, *dim4;
	
	
	cJSON_AddItemToObject(fmt, "SET_IPLOCAL", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", TSIP_LOCAL_SET);

	cJSON_AddItemToObject(fmt1, "localIP", dim3 = cJSON_CreateArray());
	cJSON_AddItemToObject(dim3, "", dim4=cJSON_CreateObject());
	value_item[IPS_IP0] = (mng_network->ipaddr>>24)&0xff;
	value_item[IPS_IP1] = (mng_network->ipaddr>>16)&0xff;
	value_item[IPS_IP2] = (mng_network->ipaddr>>8)&0xff;
	value_item[IPS_IP3] = mng_network->ipaddr &0xff;
	cJSON_AddItemToObject(dim4, "ip_addr", cJSON_CreateIntArray(value_item, 4));

	value_item[IPS_IP0] = mng_network->netmask>>24&0xff;
	value_item[IPS_IP1] = mng_network->netmask>>16&0xff;
	value_item[IPS_IP2] = mng_network->netmask>>8&0xff;
	value_item[IPS_IP3] = mng_network->netmask&0xff;
	cJSON_AddItemToObject(dim4, "net_mask", cJSON_CreateIntArray(value_item, 4)); 




	for(i=0; i<sysinfo->tsip_port; i++)
		value_item[i] = 1;
	//cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateIntArray(value_item, sysinfo.tsip_port));

	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateArray());
	for(i=0; i<sysinfo->tsip_port; i++)
	{
		//cJSON_ReplaceItemInArray(dim1, i, dim2=cJSON_CreateObject());
                cJSON_AddItemToArray(dim1, dim2=cJSON_CreateObject());

		value_item[IPS_IP0] = (tsoip_network[i].ipaddr >>24)&0xff;
		value_item[IPS_IP1] = (tsoip_network[i].ipaddr>>16)&0xff;
		value_item[IPS_IP2] = (tsoip_network[i].ipaddr>>8)&0xff;
		value_item[IPS_IP3] = tsoip_network[i].ipaddr &0xff;
		cJSON_AddItemToObject(dim2, "ipaddr", cJSON_CreateIntArray(value_item, 4));	
		
		value_item[IPS_IP0] = (tsoip_network[i].gateway >>24)&0xff;
		value_item[IPS_IP1] = (tsoip_network[i].gateway>>16)&0xff;
		value_item[IPS_IP2] = (tsoip_network[i].gateway>>8)&0xff;
		value_item[IPS_IP3] = tsoip_network[i].gateway &0xff;
		cJSON_AddItemToObject(dim2, "gateway", cJSON_CreateIntArray(value_item, 4));

		value_item[IPS_IP0] = (tsoip_network[i].netmask >>24)&0xff;
		value_item[IPS_IP1] = (tsoip_network[i].netmask>>16)&0xff;
		value_item[IPS_IP2] = (tsoip_network[i].netmask>>8)&0xff;
		value_item[IPS_IP3] = tsoip_network[i].netmask &0xff;
		cJSON_AddItemToObject(dim2, "netmask", cJSON_CreateIntArray(value_item, 4));

		value_item[MACS_MAC0] = tsoip_network[i].macaddr[0]&0xff;
		value_item[MACS_MAC1] = tsoip_network[i].macaddr[1]&0xff;
		value_item[MACS_MAC2] = tsoip_network[i].macaddr[2]&0xff;
		value_item[MACS_MAC3] = tsoip_network[i].macaddr[3]&0xff;
		value_item[MACS_MAC4] = tsoip_network[i].macaddr[4]&0xff;
		value_item[MACS_MAC5] = tsoip_network[i].macaddr[5]&0xff;
		cJSON_AddItemToObject(dim2, "macaddr", cJSON_CreateIntArray(value_item, 6));
		
		cJSON_AddNumberToObject(dim2, "ttl", tsoip_network[i].ttl);
     	cJSON_AddNumberToObject(dim2, "tos", tsoip_network[i].tos);
		cJSON_AddNumberToObject(dim2, "packets_per_udp", tsoip_network[i].packets_per_udp);
		cJSON_AddNumberToObject(dim2, "igmp_mode", tsoip_network[i].igmp_mode);
		cJSON_AddNumberToObject(dim2, "link_status", tsoip_network[i].link_status); 
		cJSON_AddNumberToObject(dim2, "in_bitrate", tsoip_network[i].in_bitrate);

		cJSON_AddNumberToObject(dim2, "out_bitrate", tsoip_network[i].out_bitrate);
	}
	return 0;
}

v_32 Set_Iplocal_func(v_u32 id, cJSON *val_arr_wrap){
	int size,i,channel=0;
	v_u32 temp32;
	cJSON *dim1, *dim2, *dim3;
		
	size = cJSON_GetArraySize(val_arr_wrap);

	for(i=0; i<size; i++)
	{
		dim1 = cJSON_GetArrayItem(val_arr_wrap, i);
		
		if((dim2 = cJSON_GetObjectItem(dim1, "channel")) != NULL)
			channel = dim2->valueint;
			
		if((dim2 = cJSON_GetObjectItem(dim1, "ipaddr")) != NULL)
		{
			temp32 = GET_INT_JARRAY(dim2, 0)<<24;
			temp32 |= GET_INT_JARRAY(dim2, 1)<<16;
			temp32 |= GET_INT_JARRAY(dim2, 2)<<8;
			temp32 |= GET_INT_JARRAY(dim2, 3);
			tsoip_network[channel].ipaddr = temp32;
		}
		if((dim2 = cJSON_GetObjectItem(dim1, "gateway")) != NULL)
		{
			temp32 = GET_INT_JARRAY(dim2, 0)<<24;
			temp32 |= GET_INT_JARRAY(dim2, 1)<<16;
			temp32 |= GET_INT_JARRAY(dim2, 2)<<8;
			temp32 |= GET_INT_JARRAY(dim2, 3);
			tsoip_network[channel].gateway = temp32;
		}
		if((dim2 = cJSON_GetObjectItem(dim1, "netmask")) != NULL)
		{
			temp32 = GET_INT_JARRAY(dim2, 0)<<24;
			temp32 |= GET_INT_JARRAY(dim2, 1)<<16;
			temp32 |= GET_INT_JARRAY(dim2, 2)<<8;
			temp32 |= GET_INT_JARRAY(dim2, 3);
			tsoip_network[channel].netmask = temp32;
		}
		if((dim2 = cJSON_GetObjectItem(dim1, "ttl")) != NULL)
			tsoip_network[channel].ttl = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "tos")) != NULL)
			tsoip_network[channel].tos = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "packets_per_udp")) != NULL)
			tsoip_network[channel].packets_per_udp = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "igmp_mode")) != NULL)
			tsoip_network[channel].igmp_mode = dim2->valueint;

		set_tsoip_network(channel);
	}

	return size;
}



/*
*lxzheng 2016/05/16 make function for Tsoip Out -> IP In real channel .
*/
v_32 Get_ipin_idx_ref_func(cJSON *fmt)
{
	int i=0;
	cJSON *dim1, *dim2;
	cJSON_AddItemToObject(fmt, "IPIN_IDX_CHANNEL", dim1=cJSON_CreateObject());
	cJSON_AddNumberToObject(dim1, "form_id", IPIN_IDX_CHANNEL_GET);

	cJSON_AddItemToObject(dim1, "value", dim2 = cJSON_CreateArray());
	for(i=0;i<tsoip_in_backup.channel_qty;i++)
	{
		cJSON_AddNumberToObject(dim2,"",ipin_idx_ref[i]);
		//cJSON_ReplaceItemInArray(dim2,i,ipin_idx_ref[i]);
	}
}


v_32 Get_IpOut_func(cJSON *fmt){
	int i,j, idx;
	static int value_item[value_len] = {0};
	cJSON *dim1, *dim2, *dim3, *fmt1, *tsip_port_fmt;
	trans_route_st *route;
	int trans_if, route_id;
		
	cJSON_AddItemToObject(fmt, "SET_IPOUT", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", TSIP_OUT_SET);
	cJSON_AddNumberToObject(fmt1, "tsip_port_number", TSOIP_PORT_QTY);
	cJSON_AddNumberToObject(fmt1, "total_ipout", 24);

		
	//cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateIntArray(value_item, sysinfo.ipout_channels));
	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateArray());	
	//printf("sysinfo.ipout_channels = %d\n", sysinfo.ipout_channels);

	trans_if = 7;

	for(idx=0; idx<sysinfo->ipout_channels; idx++){
		i = ipout_idx_ref[idx];
		if(i<0){
			break;
		}


		//i = idx;

	//	printf("%d ipout route id = %d \n", i, ipout_routeid[i]);
		
		route = get_transceiver_route(trans_if,  ipout_routeid[i]);
		if(route ==NULL){
			continue;
		}
		/*
		if(route->func->type!=FUNC_TYPE_TS_IP_OUT){
			continue;
		}
		i = route->func_chan;
		//*/

		//printf("i = %d\n", i);
		       //cJSON_ReplaceItemInArray(dim1, i, dim2=cJSON_CreateObject());
		cJSON_AddItemToArray(dim1, dim2=cJSON_CreateObject());
		cJSON_AddNumberToObject(dim2, "func_chan", i);
		cJSON_AddNumberToObject(dim2, "trans_if", trans_if);
		cJSON_AddNumberToObject(dim2, "route_id", route->route_id);
		cJSON_AddNumberToObject(dim2, "Ethernet_card", tsoip_out_channel[i].enable_mode);
		cJSON_AddNumberToObject(dim2, "FEC", tsoip_out_channel[i].fec_idx);
		cJSON_AddNumberToObject(dim2, "packets_per_udp", tsoip_out_channel[i].packets_per_udp);
		// cJSON_AddNumberToObject(dim2, "channle_enable", tsoip_out_channel[i].enable_mode);
		cJSON_AddNumberToObject(dim2, "Lost_Packet", tsoip_out_channel[i].test_lost);
		for(j=0; j<sysinfo->tsip_port; j++)
		   value_item[j] = 1;
		       //cJSON_AddItemToObject(dim2, "tsip", dim3 = cJSON_CreateIntArray(value_item, sysinfo.tsip_port));
		cJSON_AddItemToObject(dim2, "tsip", dim3 = cJSON_CreateArray());
		for(j=0; j<sysinfo->tsip_port; j++){
		  // cJSON_ReplaceItemInArray(dim3, j, tsip_port_fmt=cJSON_CreateObject());
		   cJSON_AddItemToArray(dim3, tsip_port_fmt=cJSON_CreateObject());   
		   cJSON_AddNumberToObject(tsip_port_fmt, "tos", tsoip_out_channel[i].tos[j]);	 
		   cJSON_AddNumberToObject(tsip_port_fmt, "ttl", tsoip_out_channel[i].ttl[j]);
		   cJSON_AddNumberToObject(tsip_port_fmt, "encapsul_type", tsoip_out_channel[i].encapsul_type[j] ); 
		   value_item[IPS_IP0] = ((tsoip_out_channel[i].target_ip[j])>>24)&0xff;
		   value_item[IPS_IP1] = ((tsoip_out_channel[i].target_ip[j])>>16)&0xff;
		   value_item[IPS_IP2] = ((tsoip_out_channel[i].target_ip[j])>>8)&0xff;
		   value_item[IPS_IP3] = (tsoip_out_channel[i].target_ip[j]) &0xff;
		   cJSON_AddItemToObject(tsip_port_fmt, "target_ip", cJSON_CreateIntArray(value_item, 4));
		   cJSON_AddNumberToObject(tsip_port_fmt, "target_port", tsoip_out_channel[i].target_port[j]);
		   cJSON_AddNumberToObject(tsip_port_fmt, "rtp_udp", tsoip_out_channel[i].encapsul_type[j]);
		}

	}

	return 0;
}

v_32 Set_Ipout_func(v_u32 id, cJSON *val_arr_wrap){
	int size,i,j,k, channel = 0;
	int src_card = 0, src_channel = 0; 
	v_u32 temp32 = 0;
	cJSON *dim1, *dim2, *dim3, *dim4;
	
	trans_route_st *route;
	int trans_if = 7, route_id = 0;
		
	size = cJSON_GetArraySize(val_arr_wrap);
	DebugPrintf("form_id=%d, value_length=%d\n", id, size);
	for(i=0; i<size; i++)
	{
		dim1 = cJSON_GetArrayItem(val_arr_wrap, i);
		if((dim2 = cJSON_GetObjectItem(dim1, "func_chan")) != NULL)
		{
			channel = dim2->valueint;
		}
		if((dim2 = cJSON_GetObjectItem(dim1, "route_id")) != NULL)
			route_id = dim2->valueint; // channel < sysinfo.ipout_channels
			
		if((dim2 = cJSON_GetObjectItem(dim1, "trans_if")) != NULL)
			trans_if = dim2->valueint; // channel < sysinfo.ipout_channels

		if((dim2 = cJSON_GetObjectItem(dim1, "Ethernet_card")) != NULL)
			tsoip_out_channel[channel].enable_mode = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "packets_per_udp")) != NULL)
			tsoip_out_channel[channel].packets_per_udp = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "FEC")) != NULL)
			tsoip_out_channel[channel].fec_idx = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "Lost_Packet")) != NULL)
			tsoip_out_channel[channel].test_lost = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "src_card")) != NULL)
			src_card = dim2->valueint;
		if((dim2 = cJSON_GetObjectItem(dim1, "src_channel")) != NULL)
			src_channel = dim2->valueint;

		dim2 = cJSON_GetObjectItem(dim1, "tsip");	
		//k = cJSON_GetArraySize(dim2);
		k = tsoip_out_channel[channel].enable_mode; // 0:both off, 1:port1, 2:port2, 3:both on

		if(tsoip_out_channel[channel]. enable_mode){
			set_ipout_transceiver_source(channel, tsoip_out_channel[channel].trans_src_chan);
		}else{
			clean_ipout_transceiver_source(channel);
		}
		
		for(j=0; j<TSOIP_PORT_QTY; j++){
			dim3 = cJSON_GetArrayItem(dim2, j);
			if((dim4 = cJSON_GetObjectItem(dim3, "rtp_udp")) != NULL)
				tsoip_out_channel[channel].encapsul_type[j] = dim4->valueint;
/*
			if(k==0)
				break;
			else if((k==1 && j!=0) || (k==2 && j!=1))
				continue;
*/
			if((dim4 = cJSON_GetObjectItem(dim3, "target_port")) != NULL)
				tsoip_out_channel[channel].target_port[j] = dim4->valueint;
			temp32 = 0;
			if((dim4 = cJSON_GetObjectItem(dim3, "target_ip")) != NULL)
			{
				temp32 = GET_INT_JARRAY(dim4, 0)<<24;
				temp32 |= GET_INT_JARRAY(dim4, 1)<<16;
				temp32 |= GET_INT_JARRAY(dim4, 2)<<8;
				temp32 |= GET_INT_JARRAY(dim4, 3);
				tsoip_out_channel[channel].target_ip[j] = temp32;
			}
			if((dim4 = cJSON_GetObjectItem(dim3, "tos")) != NULL)
				tsoip_out_channel[channel].tos[j] = dim4->valueint;
			if((dim4 = cJSON_GetObjectItem(dim3, "ttl")) != NULL)
				tsoip_out_channel[channel].ttl[j] = dim4->valueint;
		}
		DebugPrintf("trans_if = %d, route_id = %d, src_card = %d, src_channel = %d\n", trans_if, route_id, src_card, src_channel);				
		if((src_card == -1) || (src_channel == -1))
		{
			del_transceiver_route(trans_if, route_id);
		}
		else
		{
			add_transceiver_route(trans_if, route_id, src_card, src_channel); // IP->7
		}
		set_tsoip_out_channel(channel);
	}

	config_transceiver(trans_if);
	return size;
}


v_32 Get_Ipin_func(cJSON *fmt){
	int i=0,j=0, idx, n;
	int value_item[value_len];
	cJSON *dim1, *dim2, *fmt1,*dim3;
		
	cJSON_AddItemToObject(fmt, "SET_IPIN", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", TSIP_IN_SET);
	cJSON_AddNumberToObject(fmt1, "total_ipin", tsoip_in_backup.channel_qty);

	for(i=0; i<sysinfo->ipin_channels; i++)
		value_item[i] = 1;
	
	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateArray());

	for(idx=0; idx<tsoip_in_backup.channel_qty; idx++)
	{
		//i = idx;
		i = ipin_idx_ref[idx];
		DebugPrintf("Get_Ipin_func : ipin_idx_ref[%d] = %d \n", idx, ipin_idx_ref[idx]);
		if(i<0){
			break;
		}
		cJSON_AddItemToArray(dim1, dim2=cJSON_CreateObject());

		cJSON_AddNumberToObject(dim2, "channel", i);
     		cJSON_AddNumberToObject(dim2, "channel_enable", tsoip_in_channel[i].enable);
		cJSON_AddNumberToObject(dim2, "mode", tsoip_in_channel[i].mode);
	     	cJSON_AddNumberToObject(dim2, "rtp_udp", tsoip_in_channel[i].ts_recovery_method);

     //	cJSON_AddNumberToObject(dim2, "nif_no", tsoip_in_channel[i].nif_no);
		value_item[IPS_IP0] = ((tsoip_in_channel[i].target_ip)>>24)&0xff;
		value_item[IPS_IP1] = ((tsoip_in_channel[i].target_ip)>>16)&0xff;
		value_item[IPS_IP2] = ((tsoip_in_channel[i].target_ip)>>8)&0xff;
		value_item[IPS_IP3] = (tsoip_in_channel[i].target_ip) &0xff;
		cJSON_AddItemToObject(dim2, "target_ip", cJSON_CreateIntArray(value_item, 4));
     	cJSON_AddNumberToObject(dim2, "target_port", tsoip_in_channel[i].target_port);

		cJSON_AddNumberToObject(dim2, "select_src", tsoip_in_backup.channel[i].used_src_idx);

		for(j=0;j<4;j++)
		{
			value_item[IPS_IP0] = (tsoip_in_backup.channel[i].src_ipaddr[j]>>24)&0xff;
			value_item[IPS_IP1] = (tsoip_in_backup.channel[i].src_ipaddr[j]>>16)&0xff;
			value_item[IPS_IP2] = (tsoip_in_backup.channel[i].src_ipaddr[j]>>8)&0xff;
			value_item[IPS_IP3] = tsoip_in_backup.channel[i].src_ipaddr[j] &0xff;
			
			switch(j)
			{
				case 0:
					cJSON_AddItemToObject(dim2, "source_ip_1", cJSON_CreateIntArray(value_item, 4));
					break;
				case 1:
					cJSON_AddItemToObject(dim2, "source_ip_2", cJSON_CreateIntArray(value_item, 4));
					break;
				case 2:
					cJSON_AddItemToObject(dim2, "source_ip_3", cJSON_CreateIntArray(value_item, 4));
					break;
				case 3:
					cJSON_AddItemToObject(dim2, "source_ip_4", cJSON_CreateIntArray(value_item, 4));
					break;
				default:
					break;
			}
		}

		for(n=0; n<4; n++){
			if(tsoip_in_backup.channel[i].src_mask & (1<<n))
				value_item[n] = 1;
			else{
				value_item[n] = 0;
			}
		}

		cJSON_AddItemToObject(dim2, "source_ip_enable", cJSON_CreateIntArray(value_item, 4));
		cJSON_AddNumberToObject(dim2, "max_bitrate", tsoip_in_backup.channel[i].upper_bitrate);
		cJSON_AddNumberToObject(dim2, "min_bitrate", tsoip_in_backup.channel[i].lower_bitrate);
	//	cJSON_AddNumberToObject(dim2, "SFP_switch", 0);
	}
	
	cJSON_AddItemToObject(fmt1, "setup_guide", dim3 = cJSON_CreateObject());
	cJSON_AddNumberToObject(dim3, "port1_ipin_total", tsoip_network[0].inchan_qty);
	cJSON_AddNumberToObject(dim3, "port2_ipin_total", tsoip_network[1].inchan_qty);
	cJSON_AddNumberToObject(dim3, "redundancy_mode", tsoip_in_backup.backup_mode);
	cJSON_AddNumberToObject(dim3, "interval_min", 200);
	cJSON_AddNumberToObject(dim3, "interval_valid", tsoip_in_backup.test_period);
	cJSON_AddNumberToObject(dim3, "IGMP_mode", tsoip_network[0].igmp_mode);
		
	return 0;
}

v_32 Set_Tsipin_Basic_Infos_func(cJSON *val_arr_wrap)
{
	cJSON *child_json, *p_json;
	int v_s32;
	child_json = val_arr_wrap->child;
	
	while(child_json)
	{

		DebugPrintf("child_json [%p] : %d %s \n ", child_json, child_json->type, child_json->string);

		
		if(strcmp("redundancy_mode", child_json->string) == 0)
		{
			if(child_json, child_json->type == cJSON_String){
				v_s32 = atoi(child_json->valuestring);
			}else if(child_json, child_json->type == cJSON_Number){
				v_s32 = child_json->valueint;
			}
		
			DebugPrintf("redundancy_mode : %d\n", v_s32);
			tsoip_in_backup.backup_mode = v_s32;
			if(tsoip_in_backup.backup_mode){
				tsoip_in_backup.channel_qty = tsoip_network[0].inchan_qty;
			}else{
				tsoip_in_backup.channel_qty = sysinfo->ipin_channels;
			}
		
		}else if(strcmp("interval_valid", child_json->string) == 0)
		{
			if(child_json, child_json->type == cJSON_String){
				v_s32 = atoi(child_json->valuestring);
			}else if(child_json, child_json->type == cJSON_Number){
				v_s32 = child_json->valueint;
			}
			
			DebugPrintf("interval_valid : %d\n", v_s32);
			if(tsoip_in_backup.test_period != v_s32){
				update_tsoip_in_backup_test_period(v_s32);
			}
		
		}
		else if(strcmp("IGMP_mode", child_json->string) == 0)	
		{
			if(child_json, child_json->type == cJSON_String){
				v_s32 = atoi(child_json->valuestring);
			}else if(child_json, child_json->type == cJSON_Number){
				v_s32 = child_json->valueint;
			}
			
			DebugPrintf("IGMP_mode : %d\n", v_s32);
			tsoip_network[0].igmp_mode = v_s32;
			tsoip_network[1].igmp_mode = tsoip_network[0].igmp_mode;
			save_tsoip_igmp_mode(tsoip_network[0].igmp_mode);
		}

		child_json = child_json->next;
	}

	save_ipin_backup();

}

v_32 Set_Ipout_idx_ref_func(v_u32 id, cJSON *val_arr_wrap)
{
	cJSON *child_json;
	int i=0, channel;

	int channel_idx[MAX_TSOIP_OUT_CHANNEL_QTY];

	if(val_arr_wrap && val_arr_wrap->type == cJSON_Array)
	{
		for(i=0;i<sysinfo->ipout_channels;i++)
		{
			channel_idx[i] = -1;
			ipout_idx_ref[i] = -1;
		}
		i = 0;
		channel = 0;
		child_json = val_arr_wrap->child;

		while(child_json)
		{			
			ipout_idx_ref[i] = child_json->valueint;
			TagPrintf(ipout_idx_ref[i]);
			if(ipout_idx_ref[i]>=0){			
				channel_idx[ipout_idx_ref[i]] = i;
			}
			i++;
			child_json = child_json->next;
		}

		save_ipout_idx_ref();
			
		for(channel=0;channel<sysinfo->ipout_channels;channel++){
			TagPrintf(channel);
			if(channel_idx[channel]<0){
				TagPrintf(channel);
				if(tsoip_out_channel[channel].enable_mode){
					TagPrintf(channel);
					tsoip_out_channel[channel].enable_mode = 0;
					set_tsoip_out_channel(channel);
					del_transceiver_route(sysinfo->board_qty, ipout_routeid[channel]);
				}
			}
			
		}
		config_transceiver(sysinfo->board_qty);
	}
}

v_32 Set_Ipin_idx_ref_func(v_u32 id, cJSON *val_arr_wrap)
{
	cJSON *child_json;
	int i=0, channel;

	int channel_idx[MAX_TSOIP_IN_CHANNEL_QTY];
	
	if(val_arr_wrap->type == cJSON_Array)
	{

		for(i=0;i<sysinfo->ipin_channels;i++){
			channel_idx[i] = -1;
			ipin_idx_ref[i] = -1;
		}
	
		i = 0;
		channel = 0;
		child_json = val_arr_wrap->child;

		while(child_json)
		{
			
			ipin_idx_ref[i] = child_json->valueint;
			channel_idx[ipin_idx_ref[i]] = i;
			TagPrintf(ipin_idx_ref[i]);
			i++;
			child_json = child_json->next;
		}

		for(channel=0;channel<tsoip_in_backup.channel_qty;channel++){
			TagPrintf(channel);
			if(channel_idx[channel]<0){
				TagPrintf(channel);
				if(tsoip_in_channel[channel].enable){
					TagPrintf(channel);
					tsoip_in_channel[channel].enable = 0;
					set_ipin_channel_with_backup(channel);
				}
			}
			
		}

		
	}
		

	for(i=0; i<tsoip_in_backup.channel_qty; i++){
		DebugPrintf("ipin_idx_ref[%d] = %d \n", i, ipin_idx_ref[i]);
	}
	
	save_ipin_idx_ref();
}

// peifei modify SetIpin
v_32 Set_Ipin_func(v_u32 id, cJSON *val_arr_wrap){
	int size, i=0,j=0, channel = 0, nif_no = 0;
	v_u32 temp32, v_s32;
	cJSON *p_json;
	cJSON *dim1, *dim2, *dim3;

	size = cJSON_GetArraySize(val_arr_wrap);
	DebugPrintf("form_id=%d, value_length=%d\n", id, size);

	for(i=0; i<size; i++)
	{
		if((dim1 = cJSON_GetArrayItem(val_arr_wrap, i)) != NULL)
		{
			if((dim2 = cJSON_GetObjectItem(dim1, "channel")) != NULL)
				channel = dim2->valueint;
			DebugPrintf("Set_Ipin_func : channel = %d \n", channel);
			//tsoip_in_channel[channel].channel = channel;
				
		//	if((dim2 = cJSON_GetObjectItem(dim1, "nif_no")) != NULL)
		//		nif_no = dim2->valueint;
		//	printf("Set_Ipin_func : nif_no = %d \n", nif_no);
			
			//channel += nif_no*sysinfo.ipin_channels/sysinfo.tsip_port;
			
			if((dim2 = cJSON_GetObjectItem(dim1, "channle_enable")) != NULL)
				tsoip_in_channel[channel].enable = dim2->valueint;
			if((dim2 = cJSON_GetObjectItem(dim1, "mode")) !=NULL)
				tsoip_in_channel[channel].mode = dim2->valueint;
			if((dim2 = cJSON_GetObjectItem(dim1, "rtp_udp")) != NULL){
				if(dim2->type == cJSON_String){
					v_s32 = atoi(dim2->valuestring);
				}else if(dim2->type == cJSON_Number){
					v_s32 = dim2->valueint;
				}
				tsoip_in_channel[channel].ts_recovery_method = v_s32;
			}
			
			if((dim2 = cJSON_GetObjectItem(dim1, "target_ip")) != NULL)
			{
				temp32 = GET_INT_JARRAY(dim2, IPS_IP0)<<24;
				DebugPrintf("ip=[%d.", GET_INT_JARRAY(dim2, IPS_IP0));
				temp32 |= GET_INT_JARRAY(dim2, IPS_IP1)<<16;
				DebugPrintf("%d.", GET_INT_JARRAY(dim2, IPS_IP1));
				temp32 |= GET_INT_JARRAY(dim2, IPS_IP2)<<8;
				DebugPrintf("%d.", GET_INT_JARRAY(dim2, IPS_IP2));
				temp32 |= GET_INT_JARRAY(dim2, IPS_IP3);
				DebugPrintf("%d]\n", GET_INT_JARRAY(dim2, IPS_IP3));
				tsoip_in_channel[channel].target_ip = temp32;
			}
			if((dim2 = cJSON_GetObjectItem(dim1, "target_port")) != NULL)
				tsoip_in_channel[channel].target_port = dim2->valueint;

			if(tsoip_in_backup.backup_mode){
				if((dim2 = cJSON_GetObjectItem(dim1, "source_ip_enable")) != NULL)
				{

					tsoip_in_backup.channel[channel].src_mask = 1;
					DebugPrintf("ip_src_1_enable= %d \n", v_s32 = GET_INT_JARRAY(dim2, IPS_IP0));
					if(v_s32){
						//tsoip_in_backup.channel[channel].src_mask = (1<<0);
					}
					DebugPrintf("ip_src_2_enable= %d \n", v_s32 = GET_INT_JARRAY(dim2, IPS_IP1));
					if(v_s32){
						tsoip_in_backup.channel[channel].src_mask |= (1<<1);
					}
					DebugPrintf("ip_src_3_enable= %d \n", v_s32 = GET_INT_JARRAY(dim2, IPS_IP2));
					if(v_s32){
						tsoip_in_backup.channel[channel].src_mask |= (1<<2);
					}
					DebugPrintf("ip_src_4_enable= %d \n", v_s32 = GET_INT_JARRAY(dim2, IPS_IP3));
					if(v_s32){
						tsoip_in_backup.channel[channel].src_mask |= (1<<3);
					}
					save_ipin_channel_backup_src_mask(channel);
				}
				if((dim2 = cJSON_GetObjectItem(dim1, "max_bitrate")) != NULL){
					if(dim2->type == cJSON_String){
						v_s32 = atoi(dim2->valuestring);
					}else if(dim2->type == cJSON_Number){
						v_s32 = dim2->valueint;
					}
					DebugPrintf("max_bitrate : %d\n", v_s32);
					tsoip_in_backup.channel[channel].upper_bitrate = v_s32;
				}
				if((dim2 = cJSON_GetObjectItem(dim1, "min_bitrate")) != NULL){
					if(dim2->type == cJSON_String){
						v_s32 = atoi(dim2->valuestring);
					}else if(dim2->type == cJSON_Number){
						v_s32 = dim2->valueint;
					}
					DebugPrintf("min_bitrate : %d\n", v_s32);
					tsoip_in_backup.channel[channel].lower_bitrate = v_s32;
				}
			}
			if((dim2 = cJSON_GetObjectItem(dim1, "SFP_switch")) != NULL){
				DebugPrintf("SFP_switch : %d\n", dim2->valueint);
			}

			save_ipin_channel_backup_threshold(channel);
			
			p_json = dim1->child;

			while(p_json){
				
				p_json = p_json->next;
			}

			if(tsoip_in_backup.backup_mode){
				for(j=0;j<4;j++){
				
					switch(j){
						case 0:
							dim2 = cJSON_GetObjectItem(dim1, "source_ip_1");
							break;
						case 1:
							dim2 = cJSON_GetObjectItem(dim1, "source_ip_2");
							break;
						case 2:
							dim2 = cJSON_GetObjectItem(dim1, "source_ip_3");
							break;
						case 3:
							dim2 = cJSON_GetObjectItem(dim1, "source_ip_4");
							break;
						default:
							break;
					}
					if(dim2 != NULL ){
						temp32 = GET_INT_JARRAY(dim2, IPS_IP0)<<24;
						temp32 |= GET_INT_JARRAY(dim2, IPS_IP1)<<16;
						temp32 |= GET_INT_JARRAY(dim2, IPS_IP2)<<8;
						temp32 |= GET_INT_JARRAY(dim2, IPS_IP3);
						tsoip_in_backup.channel[channel].src_ipaddr[j] = temp32;
					}

				}
				
				save_ipin_channel_backup_src_ip(channel);
				tsoip_in_channel[channel].src_ipaddr = tsoip_in_backup.channel[channel].src_ipaddr[ tsoip_in_backup.channel[channel].used_src_idx];
			}else{
				dim2 = cJSON_GetObjectItem(dim1, "source_ip_1");
				if(dim2 != NULL ){
					temp32 = GET_INT_JARRAY(dim2, IPS_IP0)<<24;
					temp32 |= GET_INT_JARRAY(dim2, IPS_IP1)<<16;
					temp32 |= GET_INT_JARRAY(dim2, IPS_IP2)<<8;
					temp32 |= GET_INT_JARRAY(dim2, IPS_IP3);
					tsoip_in_channel[channel].src_ipaddr = temp32;
				}
			}

		}
		set_ipin_channel_with_backup(channel);

		
	}

	
	return size;
}


/*
dest:"func_list": [[1, "CI", CI-1, trans_if, route_id(显示), trans_chan(要提交的)],[]]
src:"func_list": [[1, "Tuner", Tuner-1, trans_chan],[]]
*/
v_32 Get_Transceiver_func(cJSON * fmt){
	static int value_item[value_len]={0};
	int i, j, m;
	int dest_module = -1, dest_funciton = 0;
	int src_module = 0, src_fuction = 0;
	cJSON *fmt1, *dim1, *dim2, *dim3, *dim4, *dim5;
	transceiver_st *src_transceiver;
	transceiver_st *dest_transceiver;
	trans_chan_st *src_chan_param;
	trans_route_st *trans_route;
	funcmod_st *func;
	
	cJSON_AddItemToObject(fmt, "SET_TRANS", fmt1=cJSON_CreateObject());   	
	cJSON_AddNumberToObject(fmt1, "form_id", 24);
	
	cJSON_AddNumberToObject(fmt1, "dest_module", 24);
	
	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateObject());
	for(m=0; m<=sysinfo->board_qty; m++)
		value_item[m] = 1;
//	cJSON_AddItemToObject(dim1, "dest_func", dim2 = cJSON_CreateIntArray(value_item, sysinfo.board_qty+1));
	cJSON_AddItemToObject(dim1, "dest_func", dim2 = cJSON_CreateArray());
	// dest
	for(i=0; i<=sysinfo->board_qty; i++)
	{
		dest_transceiver = &transceiver[i];
         //	cJSON_ReplaceItemInArray(dim2, i, dim3=cJSON_CreateObject());
		cJSON_AddItemToArray(dim2, dim3=cJSON_CreateObject());
		cJSON_AddNumberToObject(dim3, "module", i);
		cJSON_AddNumberToObject(dim3, "run_flag", dest_transceiver->run_flag);
		DebugPrintf("i=%d, run_flag = %d\n", i, dest_transceiver->run_flag);
		if(dest_transceiver->run_flag == 1)
		{
			if(dest_transceiver->route_qty > 0)
			{
				cJSON_AddNumberToObject(dim3, "count", dest_transceiver->route_qty);
				memset(value_item, 0, sizeof(value_item));
				for(m=0; m<dest_transceiver->route_qty; m++)
					value_item[m] = 1;
                                //cJSON_AddItemToObject(dim3, "func_list", dim4 = cJSON_CreateIntArray(value_item, dest_transceiver->route_list->count));
				cJSON_AddItemToObject(dim3, "func_list", dim4 = cJSON_CreateArray());
				DebugPrintf("dest_transceiver->route_list->count = %d\n", dest_transceiver->route_qty);
				for(j=0; j<dest_transceiver->route_qty; j++)
				{
					memset(value_item, 0, sizeof(value_item));
					for(m=0; m<7; m++)
						value_item[m] = -1;
                                        //cJSON_ReplaceItemInArray(dim4, j, dim5=cJSON_CreateIntArray(value_item, 6));
					cJSON_AddItemToArray(dim4, dim5=cJSON_CreateIntArray(value_item, 7));
					trans_route = get_transceiver_route(i, j);

					//printf("dest_transceiver %d trans route %p\n", i, trans_route);
					
					if(trans_route != NULL)
					{
						DebugPrintf("trans_route->functype : %#x \n", trans_route->func_type);
						func = get_board_funcmod(get_boardinfo(trans_route->board_slot), trans_route->func_type);
						if(func != NULL)
						{
							//func = transchan_func(trans_route);
							//cJSON_ReplaceItemInArray(dim5, 0, cJSON_CreateNumber(1));

							
							cJSON_ReplaceItemInArray(dim5, 0, cJSON_CreateNumber(func->type));
							DebugPrintf("-------------route_id = %d,  type = %#x, name %s \n",trans_route->route_id, func->type, func->name);
							cJSON_ReplaceItemInArray(dim5, 1, cJSON_CreateString(func->name));
							cJSON_ReplaceItemInArray(dim5, 2, cJSON_CreateNumber(trans_route->func_chan));
							cJSON_ReplaceItemInArray(dim5, 6, cJSON_CreateNumber(trans_route->route_id));
						}
						
						if(trans_route->trans_chan== 0xffff)
						{
							cJSON_ReplaceItemInArray(dim5, 3, cJSON_CreateNumber(-1));
							cJSON_ReplaceItemInArray(dim5, 4, cJSON_CreateNumber(0));
						}
						else
						{
							src_chan_param = get_transceiver_channel(trans_route->trans_chan);
							DebugPrintf("trans_route->psrc_chan->trans_if = %d\n", src_chan_param->board_slot);
							DebugPrintf("trans_route->psrc_chan->func_chan = %d\n", src_chan_param->func_chan);
							DebugPrintf("trans_route->psrc_chan->trans_chan = %d\n", src_chan_param->trans_chan);
							cJSON_ReplaceItemInArray(dim5, 3, cJSON_CreateNumber(src_chan_param->board_slot));
							cJSON_ReplaceItemInArray(dim5, 4, cJSON_CreateNumber(src_chan_param->func_chan));
							cJSON_ReplaceItemInArray(dim5, 5, cJSON_CreateNumber(src_chan_param->trans_chan));
						}
					}
					
				}
			}
		}
	}	
	// src
	//cJSON_AddItemToObject(dim1, "src_func", dim2 = cJSON_CreateIntArray(value_item, sysinfo.board_qty+1));
	cJSON_AddItemToObject(dim1, "src_func", dim2 = cJSON_CreateArray());

	for(i=0; i<=sysinfo->board_qty; i++)
	{
		src_transceiver = &transceiver[i];
		//cJSON_ReplaceItemInArray(dim2, i, dim3=cJSON_CreateObject());
		cJSON_AddItemToArray(dim2, dim3=cJSON_CreateObject());
		cJSON_AddNumberToObject(dim3, "module", i);
		cJSON_AddNumberToObject(dim3, "run_flag", src_transceiver->run_flag);
		if(src_transceiver->run_flag == 1)
		{
			if(src_transceiver->channel_qty > 0)
			{

				cJSON_AddItemToObject(dim3, "func_list", dim4 = cJSON_CreateArray());


				if(i == sysinfo->board_qty){

					for(j=0; j<tsoip_in_backup.channel_qty; j++){
						m = ipin_idx_ref[j];
						if(m<0){
							break;
						}
						//src_chan_param = sArrList_get(src_transceiver->channel_list, j);
						if(tsoip_in_backup.backup_mode && (tsoip_in_backup.channel[m].used_src_idx>=2)){
							m += tsoip_network[0].inchan_qty;
						}

						DebugPrintf(" ipin_idx_ref[%d] = %d, used_src_idx = %d, %d \n", j, m, tsoip_in_backup.channel[m].used_src_idx, ipin_trans_srcidx[m]);

						src_chan_param = find_transceiver_channel(i, ipin_trans_srcidx[m]);
						TagPrintf(ipin_trans_srcidx[m]);
						DebugPrintf("src_chan_param : %p\n", src_chan_param);
						if((src_chan_param != NULL) )
						{
							func =  get_board_funcmod(get_boardinfo(src_chan_param->board_slot), src_chan_param->func_type);
							if(func == NULL){continue;}
							TagPrintf(func->type);
							//func = transchan_func(src_chan_param);
							cJSON_AddItemToArray(dim4,  dim5=cJSON_CreateIntArray(value_item, 4));
							cJSON_ReplaceItemInArray(dim5, 0, cJSON_CreateNumber(func->type));
							cJSON_ReplaceItemInArray(dim5, 1, cJSON_CreateString(func->name));
							//cJSON_ReplaceItemInArray(dim5, 2, cJSON_CreateNumber(ipin_trans_srcidx[m]));
							cJSON_ReplaceItemInArray(dim5, 2, cJSON_CreateNumber(j));
							cJSON_ReplaceItemInArray(dim5, 3, cJSON_CreateNumber(src_chan_param->trans_chan));
						}
					}

					for(j=0; j<src_transceiver->channel_qty; j++){
						//cJSON_ReplaceItemInArray(dim4, j, dim5=cJSON_CreateObject());

						//src_chan_param = sArrList_get(src_transceiver->channel_list, j);
						src_chan_param = find_transceiver_channel(i,  j);
						TagPrintf(j);
						DebugPrintf("src_chan_param : %p\n", src_chan_param);
						if((src_chan_param != NULL))
						{
							
							if(src_chan_param->func_type == FUNC_TYPE_TS_IP_IN){
								continue;
							}
							func =  get_board_funcmod(get_boardinfo(src_chan_param->board_slot), src_chan_param->func_type);

							DebugPrintf("-------------trans_chan = %d,  type = %#x, name %s \n",src_chan_param->trans_chan, func->type, func->name);
							
							//func = transchan_func(src_chan_param);
							if(func == NULL){continue;}
							cJSON_AddItemToArray(dim4,  dim5=cJSON_CreateIntArray(value_item, 4));
						
							cJSON_ReplaceItemInArray(dim5, 0, cJSON_CreateNumber(func->type));
							cJSON_ReplaceItemInArray(dim5, 1, cJSON_CreateString(func->name));
							cJSON_ReplaceItemInArray(dim5, 2, cJSON_CreateNumber(src_chan_param->func_chan));
							cJSON_ReplaceItemInArray(dim5, 3, cJSON_CreateNumber(src_chan_param->trans_chan));
						}
					}
				}
				else{

					for(j=0; j<src_transceiver->channel_qty; j++){
						//cJSON_ReplaceItemInArray(dim4, j, dim5=cJSON_CreateObject());

						//src_chan_param = sArrList_get(src_transceiver->channel_list, j);
						src_chan_param = find_transceiver_channel(i,  j);
						TagPrintf(j);
						if((src_chan_param != NULL))
						{
							func =  get_board_funcmod(get_boardinfo(src_chan_param->board_slot), src_chan_param->func_type);
							DebugPrintf("-------------trans_chan = %d,  type = %#x, name %s \n",src_chan_param->trans_chan, func->type, func->name);
							if(func == NULL){continue;}
							//func = transchan_func(src_chan_param);
							cJSON_AddItemToArray(dim4,  dim5=cJSON_CreateIntArray(value_item, 4));
							cJSON_ReplaceItemInArray(dim5, 0, cJSON_CreateNumber(func->type));
							cJSON_ReplaceItemInArray(dim5, 1, cJSON_CreateString(func->name));
							cJSON_ReplaceItemInArray(dim5, 2, cJSON_CreateNumber(src_chan_param->func_chan));
							cJSON_ReplaceItemInArray(dim5, 3, cJSON_CreateNumber(src_chan_param->trans_chan));
						}
					}
					
				}
				
				cJSON_AddNumberToObject(dim3, "count", cJSON_GetArraySize(dim4));
				
			}
		}
	}
	return 0;
}
/*
**单路提交tranceiver func
**此功能为了兼容之前的json格式:(子板不存在此格式后可删除)
"form_id":25, // form_id固定

"value":{"dest_card":2, "dest_channel":0, "src_card":7, "src_channel":1}
或者
"value":[
	{"dest_card":2, "dest_channel":0, "src_card":7, "src_channel":1},
	{"dest_card":2, "dest_channel":1, "src_card":2, "src_channel":3},
	{"dest_card":2, "dest_channel":2, "src_card":3, "src_channel":1},
	.,
	.
]
*/
v_32 Set_Transceiver_func(v_u32 id, cJSON *val_arr_wrap){
	int size, i;
	int dest_trans_if = 0, route_id = 0, src_trans_if = 0, src_trans_chan = 0;
	int dest_trans_card = -1;
	cJSON *dim1, *dim2;
	DebugPrintf("%s:%d:in Set_Transceiver_func", __FUNCTION__, __LINE__);
	if((dim1 = cJSON_GetObjectItem(val_arr_wrap, "dest_card")) != NULL){
		dest_trans_if = dim1->valueint;
		dest_trans_card = dest_trans_if;
		if((dim1 = cJSON_GetObjectItem(val_arr_wrap, "dest_channel")) != NULL)
			route_id = dim1->valueint;
		if((dim1 = cJSON_GetObjectItem(val_arr_wrap, "src_card")) != NULL)
			src_trans_if = dim1->valueint;
		if((dim1 = cJSON_GetObjectItem(val_arr_wrap, "src_channel")) != NULL)
			src_trans_chan = dim1->valueint;

		if((src_trans_if == -1) || (src_trans_chan == -1)){
			del_transceiver_route(dest_trans_if, route_id);
		}else{
			DebugPrintf("old:dest_trans_if = %d, route_id = %d, src_trans_if = %d, src_trans_chan = %d\n", dest_trans_if, route_id,src_trans_if, src_trans_chan);
			add_transceiver_route(dest_trans_if, route_id, src_trans_if, src_trans_chan);
		}
	}else{ // 多参数提交,不存在清空源列表,只是提交更改的源
		
		DebugPrintf("%s:%d:in new Set_Transceiver_func", __FUNCTION__, __LINE__);
		size = cJSON_GetArraySize(val_arr_wrap);
		if((dim1 = cJSON_GetArrayItem(val_arr_wrap, 0)) != NULL){
			if((dim2 = cJSON_GetObjectItem(dim1, "dest_card")) != NULL){
				dest_trans_card = dim2->valueint; // 每次提交保持此参数一致
				//empty_transceiver_route(dest_trans_card);
			}
		}
		
		for(i=0; i<size; i++){
			if((dim1 = cJSON_GetArrayItem(val_arr_wrap, i)) != NULL){
				if((dim2 = cJSON_GetObjectItem(dim1, "dest_card")) != NULL){
					dest_trans_if = dim2->valueint;
					if((dim2 = cJSON_GetObjectItem(dim1, "dest_channel")) != NULL)
						route_id = dim2->valueint;
					if((dim2 = cJSON_GetObjectItem(dim1, "src_card")) != NULL)
						src_trans_if = dim2->valueint;
					if((dim2 = cJSON_GetObjectItem(dim1, "src_channel")) != NULL)
						src_trans_chan = dim2->valueint;
					
					if((src_trans_if == -1) || (src_trans_chan == -1)){
						del_transceiver_route(dest_trans_if, route_id);
					}else{
						DebugPrintf("new:dest_trans_if = %d, route_id = %d, src_trans_if = %d, src_trans_chan = %d\n", dest_trans_if, route_id,src_trans_if, src_trans_chan);
						add_transceiver_route(dest_trans_if, route_id, src_trans_if, src_trans_chan);
					}
				}
			}
		}
	}
	if(dest_trans_card != -1){	
		config_transceiver(dest_trans_card);
	}
	return 0;
}

extern int base_tsin_routeid;
/* tranceiver多路参数提交
** 目前作为mux设置tranceiver专用
** 功能描述:清空某板卡的源,(可同时提交新的源)
json格式:
"form_id":28 // form_id固定
"dest_card":8
"value":[
	{"dest_channel":0, "src_card":7, "src_channel":1},
	{"dest_channel":1, "src_card":2, "src_channel":3},
	{"dest_channel":2, "src_card":3, "src_channel":1},
	.,
	.
]
*/

v_32 Set_Transceiver_Multi_func_Clean(int dest_trans_card, cJSON *val_arr_wrap){
	int size, i, j;
	int route_id = 0, src_trans_if = 0, src_trans_chan = 0;
	cJSON *dim1, *dim2;
	int doneflag = 1;
	trans_route_st *trans_route;
	u_short tsin_trans_chan[ALL_TSIN_CHANNEL_QTY];
		
	DebugPrintf(":in ........\n");

	if(dest_trans_card>sysinfo->board_qty){
		return 0;
	}
	
	size = cJSON_GetArraySize(val_arr_wrap);
	if(dest_trans_card<sysinfo->board_qty){
		empty_transceiver_route(dest_trans_card);
	}else{
		
	}

	for(i=0; i<size; i++){
		if((dim1 = cJSON_GetArrayItem(val_arr_wrap, i)) != NULL){
			if((dim2 = cJSON_GetObjectItem(dim1, "dest_channel")) != NULL)
				route_id = dim2->valueint;
			if((dim2 = cJSON_GetObjectItem(dim1, "src_card")) != NULL)
				src_trans_if = dim2->valueint;
			if((dim2 = cJSON_GetObjectItem(dim1, "src_channel")) != NULL)
				src_trans_chan = dim2->valueint;

		
			DebugPrintf("%d : %#x \n", i, src_trans_chan);
			if(dest_trans_card== 7){
				tsin_trans_chan[i] = src_trans_chan;
				
				for(j=0; j<mux_info.total_tsin_channels; j++){
					trans_route = get_transceiver_route(dest_trans_card, j+base_tsin_routeid);
					if(trans_route->trans_chan == src_trans_chan){
						break;
					}
				}
				if(j==mux_info.total_tsin_channels){
					add_transceiver_route(dest_trans_card, route_id, src_trans_if, src_trans_chan);
				}
				
			}else{
				if((src_trans_if == -1) || (src_trans_chan == -1)){
					del_transceiver_route(dest_trans_card, route_id);
				}else{
					DebugPrintf("dest_trans_card = %d, route_id = %d, src_trans_if = %d, src_trans_chan = %d\n", dest_trans_card, route_id,src_trans_if, src_trans_chan);
					
					add_transceiver_route(dest_trans_card, route_id, src_trans_if, src_trans_chan);
				}
			}
		}
	}

	if(dest_trans_card== 7){
		for(i=0; i<mux_info.total_tsin_channels; i++){
			
			trans_route = get_transceiver_route(dest_trans_card, i+base_tsin_routeid);
			DebugPrintf("tsin_mapping_chan[%d] : %#x \n", i, trans_route->trans_chan);
			if(trans_route->trans_chan == 0xffff){
				continue;
			}
			for(j=0; j<size; j++){
				if(trans_route->trans_chan == tsin_trans_chan[j]){
					break;
				}
			}
			if(j==size){
				del_transceiver_route(dest_trans_card, i+base_tsin_routeid);
			}
		}
	}
	config_transceiver(dest_trans_card);
			
	return 0;
}

/* tranceiver多路参数提交
** 目前没有使用，只是作为预留 2016.01.22,但期望以后功能符合时用这种
** 功能描述:不清空某板卡的源,只是更改的提交源
json格式:
"form_id":25 // form_id固定
"dest_card":2
"value":[
	{"dest_channel":0, "src_card":7, "src_channel":1},
	{"dest_channel":1, "src_card":2, "src_channel":3},
	{"dest_channel":2, "src_card":3, "src_channel":1},
	.,
	.
]
*/
v_32 Set_Transceiver_Multi_func_NoClean(int dest_trans_card, cJSON *val_arr_wrap){
	int size, i;
	int route_id = 0, src_trans_if = 0, src_trans_chan = 0;
	cJSON *dim1, *dim2;
		
	DebugPrintf("%s:%d:in ..... \n", __FUNCTION__, __LINE__);
	size = cJSON_GetArraySize(val_arr_wrap);
	
	for(i=0; i<size; i++){
		if((dim1 = cJSON_GetArrayItem(val_arr_wrap, i)) != NULL){
			if((dim2 = cJSON_GetObjectItem(dim1, "dest_channel")) != NULL)
				route_id = dim2->valueint;
			if((dim2 = cJSON_GetObjectItem(dim1, "src_card")) != NULL)
				src_trans_if = dim2->valueint;
			if((dim2 = cJSON_GetObjectItem(dim1, "src_channel")) != NULL)
				src_trans_chan = dim2->valueint;
			
			if((src_trans_if == -1) || (src_trans_chan == -1)){
				del_transceiver_route(dest_trans_card, route_id);
			}else{
				DebugPrintf("dest_trans_card = %d, route_id = %d, src_trans_if = %d, src_trans_chan = %d\n", dest_trans_card, route_id,src_trans_if, src_trans_chan);
				add_transceiver_route(dest_trans_card, route_id, src_trans_if, src_trans_chan);
			}
		}
	}
			
	config_transceiver(dest_trans_card);

			
	return 0;
}

v_32 Get_Main_Log_Event(cJSON *fmt){
	char strbuf[100];
	int i;
	log_record_st *log_record;
	get_logs_desc(NULL, 1, 100);
	DebugPrintf("event_logs.log_count = %d\n", event_logs.log_count);
	if(event_logs.log_list != NULL){
		for(i=0; i<event_logs.log_list->count; i++){
			log_record = (log_record_st *)cirArrList_get(event_logs.log_list, i);
			//print_evnet_log(log_record);
			memset(strbuf, 0, sizeof(strbuf));
			get_log_text(log_record, strbuf);
			DebugPrintf("strbuf = %s..\n", strbuf);
		}
		DebugPrintf("event_logs.log_list->count = %d\n", event_logs.log_list->count);
	}else{
		DebugPrintf("event_logs.log_list == NULL\n");
	}
	return 0;
}

v_32 Get_Log_Event(cJSON *fmt){
	int i, j, ret;
	int slot, log_sn,  count,  top;
	int event_obj, action_type,  event_level;
	time_t s_time,  e_time;
	board_st *board;
	log_record_st *log_buf;
	event_logs_st *p_eventlogs;
	char databuf[100];
		
	int value_item[MAX_SLOT_QTY*10] = {0};
	cJSON *dim1, *dim2, *fmt1;
	cJSON_AddItemToObject(fmt, "LOG_MSG", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", LOG_MSG);
	for(j=0; j<MAX_SLOT_QTY*LOG_COUNT; j++)
		value_item[j] = 1;
	cJSON_AddItemToObject(fmt1, "value", dim1 = cJSON_CreateIntArray(value_item, MAX_SLOT_QTY*LOG_COUNT));
	i = 0;
#if 1
	
#else
	for(slot=0; slot<MAX_SLOT_QTY; slot++)
	{
		board = (board_st *)arraylist_get(&slot_board_list, slot);
		//printf("sysinfo.slot_status[%d] = %d......\n", slot, sysinfo.slot_status[slot]);
		if((board != NULL)&&(sysinfo->slot_status[slot] != 0))
		{
			p_eventlogs = &board_event_logs[slot];
			ret = get_board_logs_by_sn(slot, START_LOG, LOG_COUNT);
			printf("ret = %d\n", ret);
			if(ret > 0)
			{
				log_buf = p_eventlogs->logs_buf;
				for(j=0; j<ret; j++)
				{
					memset(databuf, 0, sizeof(databuf));
					get_boardlog_text(slot, log_buf+j, databuf);
					cJSON_ReplaceItemInArray(dim1, i, cJSON_CreateString(databuf));
					i++;
				}
				free_board_logbuf(slot);
			}
		}
	}
//#else
	memset(databuf, 0, sizeof(databuf));
	strcpy(databuf, "ssssssssssssssssssssssssssssssss");
	for(slot=0; slot<MAX_SLOT_QTY; slot++)
	{
		for(j=0; j<LOG_COUNT; j++)
		{
			cJSON_ReplaceItemInArray(dim1, i, cJSON_CreateString(databuf));
			i++;
		}
	}
#endif
	cJSON_AddNumberToObject(fmt1, "log_count", i);

	return 0;
}

v_32 ResetSonBoard(cJSON *val_arr_wrap)
{
	cJSON *SonBoardValue;

	if(val_arr_wrap->type == cJSON_Array)
	{
		SonBoardValue = val_arr_wrap->child;
		while(SonBoardValue)
		{
			printf("--SonBoardValue : %d \n",SonBoardValue->valueint);
			reset_slot_board(SonBoardValue->valueint);
			SonBoardValue = SonBoardValue->next;
		}
	}
}
v_32 CaptureInfos(cJSON *val_arr_wrap)
{
	cJSON *CaptureValue,*ParamObj;
	int in_chan;
	u_int ctrlflag;
	
	if(val_arr_wrap->type == cJSON_Array)
	{
		CaptureValue = val_arr_wrap->child;
		while(CaptureValue)
		{
			ParamObj = CaptureValue->child;
			while(ParamObj)
			{
				if(strcmp(ParamObj->string,"tsin_ch") == 0)
				{
					printf("tsin_ch : %d \n",ParamObj->valueint);
					in_chan = ParamObj->valueint;
					
				}
				else if(strcmp(ParamObj->string,"setparam") == 0)
				{
					printf("setparam : %d \n",ParamObj->valueint);
					ctrlflag = ParamObj->valueint;
				}
				ParamObj = ParamObj->next;
			
			}

			ts_in_channels[in_chan].dowith_eit_pf = (ctrlflag>>8)&1;
			ts_in_channels[in_chan].dowith_eit_s = (ctrlflag>>9)&1;
			ts_in_channels[in_chan].dowith_nit = (ctrlflag>>10)&1;

			if((ctrlflag>>0)&1){
				re_captureTs(in_chan);
			}

			save_channel_capture_ctrl(in_chan);
			
			CaptureValue = CaptureValue->next;
		}
		sleep(1);
	}
}


v_32 Clear_Log_Message(v_u32 id, cJSON *val_arr_wrap){

	int i, slot;

	for(slot=0; slot<MAX_SLOT_QTY; slot++)
	{	

		if((sysinfo->slot_info[slot].boardinfo_isok)&&(sysinfo->slot_status[slot] != 0))
		{
			clean_board_logs(slot);
		}
	}
	return 0;
}
/* lxzheng 2016/05/06 for main board Remux */

/*********************************************************************************************
*Authors & Time:_lixuezheng_2016/03/22_make function_
*Function :  NameTran_For_Background
*Parameter:pageName
*Return :backgroundName
*********************************************************************************************/
void NameTran_For_Background(u_char * pageName,u_char * backgroundName,u_int character_sw)
{
	u_int len=0,resultSize=0,i;
	u_char obj[64];
	memset(obj, 0, sizeof(obj));
	
	printf("character_sw : %d \n",character_sw);
	len = strlen(pageName);                    //service_name从页面中抓取的值
    	resultSize = URLDecode(pageName, len, obj, 64);                 //将service_name 解码到obj 中
	if(resultSize >= 0)
	{
	    //将值转化真正的编码并存进结构体.
	    utf8_to_other_convert(character_sw,obj,backgroundName);      

	}
}
/*********************************************************************************************
*Authors & Time:_lixuezheng_2016/03/22_make function_
*Function :  NameTran_For_Page
*Parameter:backgroundName,ageName
*Return : 
*********************************************************************************************/
void NameTran_For_Page(u_char *backgroundName ,u_char * pageName)
{
	u_int len=0,resultSize=0;
	u_char name_buf[64];
	memset(name_buf, 0, sizeof(name_buf));
	
	name_convert_to_utf8(backgroundName,name_buf);
	Special_Symbol_Convert(name_buf,pageName,strlen(name_buf));
}

/* lxzheng 2016/03/21  BasicInfo for MUX */
v_32 Get_Remux_BasicInfo_func(cJSON *fmt)
{	
      cJSON *fmt1,*fmt1_value_obj;

	cJSON_AddItemToObject(fmt, "REMUX_BASIC_INFO", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", BASIC_INFO_FORM_ID);
	cJSON_AddItemToObject(fmt1, "value", fmt1_value_obj=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1_value_obj, "mux_total", mux_info.mux_channels);
	cJSON_AddNumberToObject(fmt1_value_obj, "ary_sw", 0); //十进制 

}

v_32 Get_Input_Tree_func(cJSON *fmt)
{
	v_u32 temp_32;
	int i,j,k=0;
	char bitrates_str[11];	
	cJSON *fmt1, *fmt1_value,*fmt1_value_obj,*dim_InPro,*dim_InPro_ever,*dim_Emmpids,*dim_Espids_Num,*dim_Ecmpids_Num,*dim_Ips;
	cJSON *EspidEverObj;
	ts_in_channel_t *p_tsin_channel;
	ts_in_table_t *p_ts_table;
	pmt_table_t *p_pmt;
	program_t *p_program;
	service_info_t *p_server;
	u_char chNameBuf[64];

	cJSON_AddItemToObject(fmt, "INPUT_TREE", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", INIT_INPUT_TREE_FORM_ID);
	cJSON_AddItemToObject(fmt1, "value", fmt1_value=cJSON_CreateArray());

 
	for(i=0; i<mux_info.total_tsin_channels; i++)
	{
		//TagPrintf(i);
		p_tsin_channel = &ts_in_channels[i];
		p_ts_table = &p_tsin_channel->ts_tables;

		
		
		
		if((tsin_mapping_chan[i] != 0xffff) ){

			//TagPrintf( tsin_mapping_chan[i]);
		cJSON_AddItemToArray(fmt1_value,fmt1_value_obj=cJSON_CreateObject());
		//cJSON_AddNumberToObject(fmt1_value_obj, "in_channel", i );
		cJSON_AddNumberToObject(fmt1_value_obj, "src_num", tsin_mapping_chan[i] );
	
		cJSON_AddItemToObject(fmt1_value_obj, "In_Pro_infos", dim_InPro=cJSON_CreateArray());


			//TagPrintf(read_transceiver_channel_byterate(tsin_mapping_chan[i]));

			#if 0
			if(read_transceiver_channel_byterate(tsin_mapping_chan[i])>0){
				//TagPrintf(p_tsin_channel->ts_bitrate);
				while(1){
					read_tsin_bitrate(i);
					if(p_tsin_channel->ts_flag & TSIN_LOCK){
						break;
					}
				}
			}else{
				continue;
			}
					
			while(check_capture_flag(i, CAPTURE_FLOW_FLAG, CAPTURE_END) == 0){
				usleep(1000);
			}
			#endif

			lock_ts_in_channel(i, READ_FLAG);
		
			if(p_ts_table->table_valid == INVALID){
				
				unlock_ts_in_channel(i, READ_FLAG);
				continue;
			}


			for(p_pmt = p_ts_table->pmt_tables; p_pmt; p_pmt = p_pmt->next)
			{

				for(p_program = p_pmt->p_program; p_program; p_program = p_program->next){
					cJSON_AddItemToArray(dim_InPro,dim_InPro_ever=cJSON_CreateObject());

					

					p_server = &p_program->service_info;
					cJSON_AddNumberToObject(dim_InPro_ever, "C_Type", get_codetype(p_server->service_name));
					cJSON_AddNumberToObject(dim_InPro_ever, "Program_Type", p_server->service_type);

					InfoPrintf("%d : %s \n", p_server->service_type, p_server->service_name);

					memset(chNameBuf,0,sizeof(chNameBuf));
					NameTran_For_Page(p_server->service_name,chNameBuf);
					cJSON_AddStringToObject(dim_InPro_ever, "Ser_Name",chNameBuf);

					memset(chNameBuf,0,sizeof(chNameBuf));
					NameTran_For_Page(p_server->provider_name,chNameBuf);
					cJSON_AddStringToObject(dim_InPro_ever, "Ser_Pro_Name",chNameBuf);
					
					cJSON_AddNumberToObject(dim_InPro_ever, "Pro_Num", p_program->program_number);
					cJSON_AddNumberToObject(dim_InPro_ever, "Pmt_pid", p_program->pmt_pid);
					cJSON_AddNumberToObject(dim_InPro_ever, "Pcr_pid", p_program->pcr_pid);
					cJSON_AddNumberToObject(dim_InPro_ever, "Fca_Mode",p_program->free_CA_mode);
					cJSON_AddNumberToObject(dim_InPro_ever, "Ser_Type", p_server->service_type); 

					cJSON_AddItemToObject(dim_InPro_ever, "Espids_Num", dim_Espids_Num=cJSON_CreateArray());
		
					for(j=0; j<ES_PID_QTY_IN_A_PROGRAM; j++)
					{	
						if((p_program->es_pid[j] & 0x1fff) == 0x1fff){
							break;
						}
						cJSON_AddItemToArray(dim_Espids_Num,EspidEverObj=cJSON_CreateObject());
						cJSON_AddNumberToObject(EspidEverObj, "pid", p_program->es_pid[j]& 0x1fff);
						cJSON_AddNumberToObject(EspidEverObj, "type",p_program->es_pid[j]>>16);
					}
					
					cJSON_AddItemToObject(dim_InPro_ever, "Ecmpids_Num", dim_Ecmpids_Num=cJSON_CreateArray());
					for(j=0; j<ECM_PID_QTY_IN_A_PROGRAM; j++)
					{	
						if((p_program->ecm_pid[j] & 0x1fff) == 0x1fff){
							break;
						}
						cJSON_AddNumberToObject(dim_Ecmpids_Num,"",p_program->ecm_pid[j] & 0x1fff);
					}
				}
			}
					
			cJSON_AddNumberToObject(fmt1_value_obj, "Channel_Bitrates", p_tsin_channel->ts_bitrate);
			cJSON_AddItemToObject(fmt1_value_obj, "Emmpids_Num", dim_Emmpids=cJSON_CreateArray());
			cJSON_AddItemToObject(fmt1_value_obj, "Ips_Num", dim_Ips=cJSON_CreateObject());
			
			unlock_ts_in_channel(i, READ_FLAG);

			
		}
		
		
	}
}
	
v_32 Get_FILTER_Infos_func(cJSON *fmt)
{
	int i=0;
	char mux_key[10];
	mux_channel_t *p_mux_channel;
	cJSON *fmt1,*MuxSetInfosValueObj,*vauleItemObj;
	
	cJSON_AddItemToObject(fmt, "MUX_SET_INFOS", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", Get_MUX_SET_INFOS_FORM_ID);
	cJSON_AddItemToObject(fmt1, "value", MuxSetInfosValueObj=cJSON_CreateObject());
	for(i=0; i<mux_info.mux_channels; i++)
	{
		p_mux_channel = &mux_channels[i];
		sprintf(mux_key, "mux_%d", i+1);
		cJSON_AddItemToObject(MuxSetInfosValueObj,mux_key,vauleItemObj=cJSON_CreateObject());
		cJSON_AddNumberToObject(vauleItemObj, "muxSwitch", p_mux_channel->enable);
		cJSON_AddNumberToObject(vauleItemObj, "removeCA", p_mux_channel->remove_ca_flag);
		cJSON_AddNumberToObject(vauleItemObj, "outBitrate", p_mux_channel->total_bitrate);
		cJSON_AddStringToObject(vauleItemObj, "chName", p_mux_channel->channel_name);
		cJSON_AddNumberToObject(vauleItemObj, "setId", p_mux_channel->ts_id);
		cJSON_AddNumberToObject(vauleItemObj, "networkId", p_mux_channel->network_id);		 
	}
}
v_32 Get_Mux_Refresh_Infos_func(cJSON *fmt)
{
	int i=0;
	char mux_key[10];
	mux_channel_t *p_mux_channel;
	cJSON *fmt1,*MuxRefreshInfosValueObj,*vauleItemObj;
	
	cJSON_AddItemToObject(fmt, "Mux_Refresh_Infos", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", Get_Mux_Refresh_Infos_FORM_ID);
	cJSON_AddItemToObject(fmt1, "value", MuxRefreshInfosValueObj=cJSON_CreateObject());
	for(i=0; i<mux_info.mux_channels; i++)
	{
		sprintf(mux_key, "mux_%d", i+1);
		cJSON_AddItemToObject(MuxRefreshInfosValueObj,mux_key,vauleItemObj=cJSON_CreateObject());
		cJSON_AddNumberToObject(vauleItemObj, "validBitrate", mux_channels[i].valid_bitrate);
	}
}

v_32 Get_Edit_Prog_Infos_func(cJSON *fmt)
{
	int i=0,k=0,j=0;
	char mux_key[10];
	char prog_key[10];
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	trans_chan_st *p_trans_chan;
	char src_channel_name[50];
	cJSON *fmt1,*EditProgInfosValueObj,*vauleItemObj,*muxProsInfosItemObj;
	cJSON *OriginalProgInfosObj,*MuxProgInfosObj,*MuxPidInfosObj;
	cJSON *muxPidInfosItemObj;
	funcmod_st *func;
	u_char chNameBuf[64];
	cJSON_AddItemToObject(fmt, "Edit_Program_Infos", fmt1=cJSON_CreateObject());
	cJSON_AddNumberToObject(fmt1, "form_id", INIT_Edit_Prog_Infos_FORM_ID);
	cJSON_AddItemToObject(fmt1, "value", EditProgInfosValueObj=cJSON_CreateObject());
	TagPrintf(0);
	k=0;
	for(i=0; i<mux_info.mux_channels; i++)
	{
		TagPrintf(i);	

		p_mux_channel = &mux_channels[i];

		if(p_mux_channel->program_count<=0){
			continue;
		}

		sprintf(mux_key, "mux_%d", i+1);

		cJSON_AddItemToObject(EditProgInfosValueObj,mux_key,vauleItemObj=cJSON_CreateObject());
		 
		p_mux_program = p_mux_channel->mux_programs_raw;
		for(; p_mux_program; p_mux_program=p_mux_program->link)
		{		
			sprintf(prog_key, "%d", ++k);
			
			TagPrintf(k);
			
			TagPrintf(p_mux_program->in_chan);
			
			TagPrintf(p_mux_program->old_pn);

			if(p_mux_program->in_chan>=mux_info.total_tsin_channels){
				continue;
			}			
			p_trans_chan = get_transceiver_channel(tsin_mapping_chan[p_mux_program->in_chan]);
			if(p_trans_chan){
				func =  get_board_funcmod(get_boardinfo(p_trans_chan->board_slot), p_trans_chan->func_type);
				//func = transchan_func(p_trans_chan);
				sprintf(src_channel_name, "Mode %d #%s-%d", p_trans_chan->board_slot+1, func->name, p_trans_chan->func_chan+1);
			}
			DebugPrintf("%s \n", src_channel_name);
			cJSON_AddItemToObject(vauleItemObj,prog_key, muxProsInfosItemObj=cJSON_CreateObject());
			cJSON_AddNumberToObject(muxProsInfosItemObj, "dest_func_channel", p_mux_program->in_chan);
			cJSON_AddNumberToObject(muxProsInfosItemObj, "Trans_channel", tsin_mapping_chan[p_mux_program->in_chan]);

			cJSON_AddItemToObject(muxProsInfosItemObj,"Original_prog_infos", OriginalProgInfosObj=cJSON_CreateObject());
			cJSON_AddNumberToObject(OriginalProgInfosObj, "prog_num",p_mux_program->old_pn);
			cJSON_AddNumberToObject(OriginalProgInfosObj, "PMT_pid", p_mux_program->in_pmt_pid);
			cJSON_AddNumberToObject(OriginalProgInfosObj, "PCR_pid",p_mux_program->in_pcr_pid);

			memset(chNameBuf,0,sizeof(chNameBuf));
			NameTran_For_Page(p_mux_program->old_service_info.service_name,chNameBuf);		
			cJSON_AddStringToObject(OriginalProgInfosObj, "prog_name", chNameBuf);
			cJSON_AddStringToObject(OriginalProgInfosObj, "Src_name", src_channel_name);

			cJSON_AddItemToObject(muxProsInfosItemObj,"Mux_prog_infos", MuxProgInfosObj=cJSON_CreateObject());
			cJSON_AddNumberToObject(MuxProgInfosObj, "prog_num",p_mux_program->new_pn);
			cJSON_AddNumberToObject(MuxProgInfosObj, "PMT_pid", p_mux_program->mux_pmt_pid);
			cJSON_AddNumberToObject(MuxProgInfosObj, "PCR_pid",p_mux_program->mux_pcr_pid);

			memset(chNameBuf,0,sizeof(chNameBuf));
			NameTran_For_Page(p_mux_program->service_info.service_name,chNameBuf);
			cJSON_AddStringToObject(MuxProgInfosObj, "prog_name",chNameBuf);

			memset(chNameBuf,0,sizeof(chNameBuf));
			NameTran_For_Page(p_mux_program->service_info.provider_name,chNameBuf);
			cJSON_AddStringToObject(MuxProgInfosObj, "provider_name",chNameBuf);

			cJSON_AddNumberToObject(MuxProgInfosObj, "Fca_Mode", p_mux_program->free_CA_mode);
			cJSON_AddNumberToObject(MuxProgInfosObj, "S_TYPE", p_mux_program->service_info.service_type);
			cJSON_AddNumberToObject(MuxProgInfosObj, "C_TYPE", p_mux_program->char_type);
			cJSON_AddNumberToObject(MuxProgInfosObj, "PMT_version", p_mux_program->service_info.service_type);


			cJSON_AddItemToObject(muxProsInfosItemObj,"Mux_pid_infos", MuxPidInfosObj=cJSON_CreateArray());

			TagPrintf(k);
			
			for(j=0; j<ES_PID_QTY_IN_A_PROGRAM; j++)
			{			
				DebugPrintf("%#x pid, %#x type \n", p_mux_program->es_pid_mapping[j][0], p_mux_program->es_pid_mapping[j][2]);
				if((p_mux_program->es_pid_mapping[j][0]&0x1fff) == 0x1fff){
					break;
				}
				cJSON_AddItemToArray(MuxPidInfosObj,muxPidInfosItemObj=cJSON_CreateObject());
				cJSON_AddNumberToObject(muxPidInfosItemObj, "pid", p_mux_program->es_pid_mapping[j][0]&0x1fff);
				cJSON_AddNumberToObject(muxPidInfosItemObj, "type", p_mux_program->es_pid_mapping[j][2]);
				cJSON_AddNumberToObject(muxPidInfosItemObj, "mux_pid", p_mux_program->es_pid_mapping[j][1]&0x1fff);
			}

			
		}
	}
	
}

v_32 Set_Edit_Program_Infos_func(cJSON *val_arr_wrap)
{
	int value_size_2=0,j=0,i;
	
	cJSON *muxkey_json,*progkey_json,*progChild;
	cJSON *OriginalProgInfos_json,*MuxProgInfos_json,*MuxPidInfos_json,*MuxPidInfosItem_json,*PidChild_json;

	int mux_chan, in_chan, old_pn, new_pn, in_pmtpid, in_pcrpid, out_pmtpid, out_pcrpid;
	int service_type, char_type, Fca_Mode;
	//u_char *old_prog_name, *prog_name = NULL, *provider_name = NULL;
	u_char  prog_name[48], provider_name[48];
	u_char old_prog_name_temp[48],provider_name_temp[48],prog_name_temp[48];	

	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	program_t *p_program;
	
	if(val_arr_wrap->type == cJSON_Object)
	{
		muxkey_json = val_arr_wrap->child;
		while(muxkey_json)
		{
			//printf("Set_Edit_Program : %s \n", muxkey_json->string);
			mux_chan = strtol(muxkey_json->string+4, NULL, 10) - 1;
			DebugPrintf("Set_Edit_Program : %d mux channel \n", mux_chan);
			p_mux_channel= &mux_channels[mux_chan];

			remove_all_mux_program(mux_chan);
			free_all_mux_section(p_mux_channel->mux_sections);
			p_mux_channel->mux_sections = NULL;
			
			progkey_json = muxkey_json->child;
			while(progkey_json){
				progChild = progkey_json->child;
				while(progChild)
				{ 
					if(strcmp(progChild->string,"dest_func_channel") == 0)
					{
						printf("dest_func_channel : %d \n",progChild->valueint);
						in_chan = progChild->valueint;
						if((in_chan>=mux_info.total_tsin_channels)){
							break;
						}
					}
					else if(strcmp(progChild->string,"Trans_channel") == 0)
					{
						printf("Trans_channel : %d \n",progChild->valueint);
					}
					else if(strcmp(progChild->string,"Original_prog_infos") == 0)
					{
						OriginalProgInfos_json = progChild->child;
						while(OriginalProgInfos_json)
						{
							if(strcmp(OriginalProgInfos_json->string,"prog_num") == 0)
							{
								printf("prog_num old : %d \n",OriginalProgInfos_json->valueint);
								old_pn = OriginalProgInfos_json->valueint;
							}
							else if(strcmp(OriginalProgInfos_json->string,"PMT_pid") == 0)
							{
								printf("PMT_pid old : %d \n",OriginalProgInfos_json->valueint);
								in_pmtpid = OriginalProgInfos_json->valueint;
								
							}
							else if(strcmp(OriginalProgInfos_json->string,"PCR_pid") == 0)
							{
								printf("PCR_pid old : %d \n",OriginalProgInfos_json->valueint);
								in_pcrpid = OriginalProgInfos_json->valueint;
							}
							else if(strcmp(OriginalProgInfos_json->string,"prog_name") == 0)
							{
								//memset(old_prog_name,0,sizeof(old_prog_name));
								//strcpy(old_prog_name, OriginalProgInfos_json->valuestring);
								//old_prog_name_temp = OriginalProgInfos_json->valuestring;
								//printf("prog_name old : %s \n",old_prog_name);
							}
							else if(strcmp(OriginalProgInfos_json->string,"Src_name") == 0)
							{
								printf("Src_name old : %s \n",OriginalProgInfos_json->valuestring);
							}
							OriginalProgInfos_json= OriginalProgInfos_json->next;
						}
					}
					else if(strcmp(progChild->string,"Mux_prog_infos") == 0)
					{
						MuxProgInfos_json = progChild->child;
						while(MuxProgInfos_json)
						{
							if(strcmp(MuxProgInfos_json->string,"prog_num") == 0)
							{
								printf("prog_num new : %d \n",MuxProgInfos_json->valueint);
								new_pn = MuxProgInfos_json->valueint;
							}
							else if(strcmp(MuxProgInfos_json->string,"PMT_pid") == 0)
							{
								printf("PMT_pid new : %d \n",MuxProgInfos_json->valueint);
								out_pmtpid = MuxProgInfos_json->valueint;
							}
							else if(strcmp(MuxProgInfos_json->string,"PCR_pid") == 0)
							{
								printf("PCR_pid new : %d \n",MuxProgInfos_json->valueint);
								out_pcrpid = MuxProgInfos_json->valueint;
							}
							else if(strcmp(MuxProgInfos_json->string,"prog_name") == 0)
							{
								memset(prog_name_temp,0,sizeof(prog_name_temp));
								strcpy(prog_name_temp, MuxProgInfos_json->valuestring);
								//prog_name_temp = MuxProgInfos_json->valuestring;
								printf("prog_name new : %s \n",prog_name_temp);
							}
							else if(strcmp(MuxProgInfos_json->string,"provider_name") == 0)
							{
								memset(provider_name_temp,0,sizeof(provider_name_temp));
								strcpy(provider_name_temp, MuxProgInfos_json->valuestring);
								//provider_name_temp = MuxProgInfos_json->valuestring;
								printf("provider_name new : %s \n",provider_name_temp);
							}
							else if(strcmp(MuxProgInfos_json->string,"S_TYPE") == 0)
							{
								printf("S_TYPE new : %d \n",MuxProgInfos_json->valueint);
								service_type = MuxProgInfos_json->valueint;
							}
							else if(strcmp(MuxProgInfos_json->string,"C_TYPE") == 0)
							{
								printf("C_TYPE new : %d \n",MuxProgInfos_json->valueint);
								char_type = MuxProgInfos_json->valueint;
							}
							else if(strcmp(MuxProgInfos_json->string,"PMT_version") == 0)
							{
								printf("PMT_version new : %d \n",MuxProgInfos_json->valueint);
							}
							else if(strcmp(MuxProgInfos_json->string,"Fca_Mode") == 0)
							{
								printf("Fca_Mode new : %d \n",MuxProgInfos_json->valueint);
								Fca_Mode = MuxProgInfos_json->valueint;
							}

							MuxProgInfos_json = MuxProgInfos_json->next;
						
						}
						
						memset(provider_name,0,sizeof(provider_name));
						memset(prog_name,0,sizeof(prog_name));
						
						NameTran_For_Background(provider_name_temp,provider_name,char_type);
						NameTran_For_Background(prog_name_temp,prog_name,char_type);
#if 0
	for(i=0;i<10;i++)
	{
		printf("__33_____%X____\n",provider_name[i]);
	}
#endif
						
						p_mux_program = update_program_mapping(mux_chan, in_chan, old_pn,  new_pn);
						if(p_mux_program){
							p_mux_program->in_pmt_pid = in_pmtpid;
							p_mux_program->mux_pmt_pid = out_pmtpid;
							p_mux_program->in_pcr_pid = in_pcrpid;
							p_mux_program->mux_pcr_pid = out_pcrpid;
							p_mux_program->service_info.service_type = service_type;

							p_mux_program->char_type = char_type;
							p_mux_program->free_CA_mode = Fca_Mode;
							p_program = find_ts_in_program(ts_in_channels[in_chan].ts_tables.pmt_tables, p_mux_program->old_pn);
							if(p_program){
								strcpy(p_mux_program->old_service_info.service_name, p_program->service_info.service_name);
							}
							//printf("regen pmt : %s, %s \n", p_program->service_info.service_name, p_mux_program->old_service_info.service_name);
							//strcpy(p_mux_program->old_service_info.service_name, old_prog_name);
							//strcpy(p_mux_program->service_info.service_name, &prog_name[1]);
							memcpy(p_mux_program->service_info.service_name, prog_name, 20);
						//	strcpy(p_mux_program->service_info.provider_name, provider_name);
							memcpy(p_mux_program->service_info.provider_name, provider_name, 20);
							p_mux_program->service_info.provider_name[21] = '\0';

#if 0
	for(i=0;i<10;i++)
	{
		printf("__9999_____%X____\n",p_mux_program->service_info.provider_name[i]);
	}
#endif
						}
						
						

					}
					else if(strcmp(progChild->string,"Mux_pid_infos") == 0)
					{
						j = 0;
						MuxPidInfosItem_json=progChild->child;
						while(MuxPidInfosItem_json)
						{	
							PidChild_json = MuxPidInfosItem_json->child;
							while(PidChild_json)
							{
								if(strcmp(PidChild_json->string,"pid") == 0)
								{
									printf("pid  : %d \n",PidChild_json->valueint);
									if(p_mux_program)
									p_mux_program->es_pid_mapping[j][0] = PidChild_json->valueint;
								}else if(strcmp(PidChild_json->string,"type") == 0)
								{
									printf("type  : %d \n",PidChild_json->valueint);
									if(p_mux_program)
									p_mux_program->es_pid_mapping[j][2] = PidChild_json->valueint;
								}else if(strcmp(PidChild_json->string,"mux_pid") == 0)
								{
									printf("mux_pid  : %d \n",PidChild_json->valueint);
									if(p_mux_program)
									p_mux_program->es_pid_mapping[j][1] = PidChild_json->valueint;
								}
								PidChild_json = PidChild_json->next;
							}
							j++;
							MuxPidInfosItem_json = MuxPidInfosItem_json->next;
						}	
						
					}			
					progChild= progChild->next;
				}
				progkey_json = progkey_json->next;
			}
			//remux(mux_chan);
			save_remux_programs(mux_chan);
			p_mux_channel->reperform_flag = 1;
			remux(mux_chan);
			save_remux_pid_filter(mux_chan);
			save_remux_section(mux_chan, p_mux_channel->mux_sections);
	
			muxkey_json = muxkey_json->next;
		}		
	}
}

v_32 Set_Mux_Infos_func(cJSON *val_arr_wrap)
{
	u_int i=0,j=0,k=0,t=0,m=0;

	cJSON *MuxTreeValueArrayItemObj;
	cJSON *IndexObj;
	cJSON *SetInfosObj,*muxSwitchObj,*outBitrateObj,*chNameObj,*setIdObj,*networkIdObj;
	cJSON *MuxProsInfosObj, *MuxProsInfosArrayItemObj;
	cJSON *SrcIndexObj,*SrcNameObj,*ProgramTypeObj,*ProgNumObj,*PmtPidObj,*PcrPidObj,*ProIndexObj,*FcaModeObj; 	
	cJSON *ProgNameObj,*ProgNameNewObj,*ProviderNameObj;
	cJSON *MuxPidInfosObj,*MuxPidInfosArrayItemobj,*PidObj,*TypeObj,*MuxPidObj,*SetEnaObj;

	u_int value_size=0,value_size_2=0,value_size_3=0,value_size_4=0;
	u_int dim_size1=0,dim_size2=0,dim_size3=0,dim_size4=0,dim_size5=0;
	u_int index;

	int mux_chan, in_chan, old_pn, new_pn;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;
	
	value_size = cJSON_GetArraySize(val_arr_wrap);
	printf("111_size : %d \n",value_size);
	for(i=0;i<value_size;i++)
	{
		if(i>=sysinfo->mux_channels)
			break;
		
		MuxTreeValueArrayItemObj = cJSON_GetArrayItem(val_arr_wrap, i);

		/*--index Value[i][0] part--*/
		if((IndexObj = cJSON_GetObjectItem(MuxTreeValueArrayItemObj, "index")) != NULL)
		{
			printf("index : 0x%04X \n",IndexObj->valueint);
			mux_chan = IndexObj->valueint - 1;
			p_mux_channel= &mux_channels[mux_chan];

			remove_all_mux_program(mux_chan);
			//remove_all_mux_pid_filters(mux_chan);
		}
		else
		{
			printf("mux index is NULL,i = %d\n",i);	
			return -1;
		}
		
		/*--mux_infos Value[i][2] part--*/

		if((MuxProsInfosObj = cJSON_GetObjectItem(MuxTreeValueArrayItemObj, "mux_infos")) != NULL)
		{
			value_size_2 = cJSON_GetArraySize(MuxProsInfosObj);
			for(j=0;j<value_size_2;j++)
			{
				if((MuxProsInfosArrayItemObj = cJSON_GetArrayItem(MuxProsInfosObj, j)) != NULL)
				{						

					if((SrcIndexObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "dest_func_channel"))!=NULL)
					{
						printf("dest_func_channel : 0x%04X \n",SrcIndexObj->valueint);
						in_chan = SrcIndexObj->valueint;
						if((in_chan>=mux_info.total_tsin_channels)){
								continue;
						}
					}
					
					if((SrcIndexObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "Src_index"))!=NULL)
					{
						printf("Src_index : 0x%04X \n",SrcIndexObj->valueint);
						//in_chan = SrcIndexObj->valueint;
					}
					if((ProgNumObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "prog_num"))!=NULL)
					{
						printf("ProgNumObj : %d \n",ProgNumObj->valueint);
						old_pn = ProgNumObj->valueint;
						
					}
					
					p_mux_program = update_program_mapping(mux_chan, in_chan, old_pn,  old_pn);
					
					if((SrcNameObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "Src_name"))!=NULL)
					{
						printf("SrcNameObj : %s \n",SrcNameObj->valuestring);
					}
					if((ProgNameNewObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "prog_name_new"))!=NULL)
					{
						printf("ProgNameNewObj : %s \n",ProgNameNewObj->valuestring); 
						strcpy(p_mux_program->service_info.service_name, ProgNameNewObj->valuestring);
					}
					if((ProgNameObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "prog_name"))!=NULL)
					{
						printf("ProgNameObj : %s \n",ProgNameObj->valuestring); 
						//strcpy(p_mux_program->old_service_info.service_name, ProgNameNewObj->valuestring);
					}
					if((ProviderNameObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "provider_name"))!=NULL)
					{
						printf("ProviderNameObj : %s \n",ProviderNameObj->valuestring);
						strcpy(p_mux_program->service_info.provider_name, ProviderNameObj->valuestring);
					}
					if((ProgramTypeObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "Program_Type"))!=NULL)
					{
						printf("Program_Type : %d \n",ProgramTypeObj->valueint);	
						p_mux_program->service_info.service_type = ProgramTypeObj->valueint;
					}
					if((PmtPidObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "PMT_pid"))!=NULL)
					{
						printf("PmtPidObj : %d \n",PmtPidObj->valueint);
						p_mux_program->in_pmt_pid = PmtPidObj->valueint;
						p_mux_program->mux_pmt_pid = PmtPidObj->valueint;
					}
					if((PcrPidObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "PCR_pid"))!=NULL)
					{
						printf("PcrPidObj : %d \n",PcrPidObj->valueint);
						p_mux_program->in_pcr_pid = PcrPidObj->valueint;
						p_mux_program->mux_pcr_pid = PcrPidObj->valueint;
					}
					p_mux_program->cas_mode = 0;
					if((FcaModeObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "Fca_Mode"))!=NULL)
					{
						printf("Fca_Mode : %d \n",FcaModeObj->valueint);							
					}						
					if((ProIndexObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "Pro_index"))!=NULL)
					{
						printf("ProIndexObj : %d \n",ProIndexObj->valueint);							
					}


					//remux(mux_chan);
					if((MuxPidInfosObj = cJSON_GetObjectItem(MuxProsInfosArrayItemObj, "Mux_pid_infos"))!=NULL)
					{
						k=0;
						m = 0;
						dim_size3 = cJSON_GetArraySize(MuxPidInfosObj);
						for(k=0;k<dim_size3;k++)
						{
							if((MuxPidInfosArrayItemobj = cJSON_GetArrayItem(MuxPidInfosObj, k)) != NULL)
							{
								if((PidObj = cJSON_GetObjectItem(MuxPidInfosArrayItemobj, "pid"))!=NULL)
								{
									printf("PidObj : 0x%04x \n", PidObj->valueint);			
									p_mux_program->es_pid_mapping[k][0] = PidObj->valueint;	
									p_mux_program->es_pid_mapping[k][1] = PidObj->valueint;				
								}
								if((TypeObj = cJSON_GetObjectItem(MuxPidInfosArrayItemobj, "type"))!=NULL)
								{
									printf("TypeObj : 0x%02x \n", TypeObj->valueint);
									p_mux_program->es_pid_mapping[k][2] = TypeObj->valueint;
								}
							}
						}
					}
				}
			}
		}

		save_remux_programs(mux_chan);
			
	}

	return value_size;
}

v_32 Set_FILTER_Infos_func(cJSON *val_arr_wrap)
{
	cJSON *FilterIndexObj,*FilterInfoObj,*muxSwitchObj,*removeCAObj,*outBitrateObj,*chNameObj,*setIdObj,*networkIdObj;
	int index;
	int mux_chan, in_chan, old_pn, new_pn;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	if((FilterIndexObj = cJSON_GetObjectItem(val_arr_wrap, "FilterIndex")) != NULL)
	{
		printf("mux index outof range index = %d\n",FilterIndexObj->valueint);	
		mux_chan = FilterIndexObj->valueint ;
		p_mux_channel= &mux_channels[mux_chan];
		
		free_all_mux_section(p_mux_channel->mux_sections);
		p_mux_channel->mux_sections = NULL;
	}
	else
	{
		printf("mux index is NULL\n");	
		return -1;
	}
	
	if((FilterInfoObj = cJSON_GetObjectItem(val_arr_wrap, "SetFilterInfo")) != NULL)
	{
		if((muxSwitchObj = cJSON_GetObjectItem(FilterInfoObj, "muxSwitch"))!=NULL)
		{
			printf("muxSwitch : %d \n",muxSwitchObj->valueint);
			p_mux_channel->enable = muxSwitchObj->valueint;
		}	
		
		if((removeCAObj = cJSON_GetObjectItem(FilterInfoObj, "removeCA"))!=NULL)
		{
			printf("removeCA : %d \n",removeCAObj->valueint);
			p_mux_channel->remove_ca_flag =  removeCAObj->valueint;
		}
				
		if((outBitrateObj = cJSON_GetObjectItem(FilterInfoObj, "outBitrate"))!=NULL)
		{
			printf("outBitrate : %s \n",outBitrateObj->valuestring);
			p_mux_channel->total_bitrate = atoi(outBitrateObj->valuestring);
		}
		if((chNameObj = cJSON_GetObjectItem(FilterInfoObj, "chName"))!=NULL)
		{	
			printf("chName : %s \n",chNameObj->valuestring);	
			strcpy(p_mux_channel->channel_name, chNameObj->valuestring);
		}
		if((setIdObj = cJSON_GetObjectItem(FilterInfoObj, "setId"))!=NULL)
		{
			printf("setId : %d \n",setIdObj->valueint);
			p_mux_channel->ts_id = setIdObj->valueint;
		}
		if((networkIdObj = cJSON_GetObjectItem(FilterInfoObj, "networkId"))!=NULL)
		{
			printf("networkIdObj : %d \n",networkIdObj->valueint);
			p_mux_channel->network_id = networkIdObj->valueint;
		}		
	}

	save_remux_parameters(mux_chan);

	p_mux_channel->reperform_flag = 1;
	remux(mux_chan);
	
	save_remux_pid_filter(mux_chan);
	save_remux_section(mux_chan, p_mux_channel->mux_sections);

	return FilterIndexObj->valueint;
}

/* lxzheng 2016/03/10  Prog_Infos */
v_32 Set_Prog_Infos_func(cJSON *val_arr_wrap)
{
	u_int value_size_1=0;
	u_int j=0;
	cJSON *IndexObj,*ChnameObj,*MuxProsinfosObj,*MuxProinfoItemObj;
	cJSON *SrcObj,*OldProgInfoObj,*NewProgInfoObj;
	cJSON *FcaModeObj,*ProgramTypeObj,*ProgNumOldObj,*PMTPidOldObj,*ProgNameOldObj,*ProviderNameOldObj;
	cJSON *ProgNumNewObj,*PMTPidNewObj,*ProgNameNewObj,*ProviderNameNewsObj,*RemoveCAObj;	 	  
	int mux_chan, in_chan, old_pn, new_pn;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	/*--index Value[i][0] part--*/
	if((IndexObj = cJSON_GetObjectItem(val_arr_wrap, "Index")) != NULL)
	{
		printf("index : 0x%04X \n",IndexObj->valueint);	
		mux_chan = IndexObj->valueint;
		p_mux_channel= &mux_channels[mux_chan];
	}
	
	if((ChnameObj = cJSON_GetObjectItem(val_arr_wrap, "Chname"))!=NULL)
	{
		printf("ChnameLast : %s \n",ChnameObj->valuestring);
	}

	if((MuxProsinfosObj = cJSON_GetObjectItem(val_arr_wrap, "MuxProsinfos")) != NULL)
	{
		value_size_1 = cJSON_GetArraySize(MuxProsinfosObj);
		for(j=0;j<value_size_1;j++)
		{
			MuxProinfoItemObj = cJSON_GetArrayItem(MuxProsinfosObj, j);
			if((SrcObj = cJSON_GetObjectItem(MuxProinfoItemObj, "Src"))!=NULL)
			{
				printf("Src : %d \n",SrcObj->valueint);
				in_chan = SrcObj->valueint;
				if((in_chan>=mux_info.total_tsin_channels)){
					continue;
				}
			}

			if((OldProgInfoObj = cJSON_GetObjectItem(MuxProinfoItemObj, "Old_prog_infos"))!=NULL)
			{
				continue;			
			}
				
			if((NewProgInfoObj = cJSON_GetObjectItem(MuxProinfoItemObj, "NewProgInfoObj"))!=NULL)
			{
				continue;
			}
		
			if((ProgNumOldObj = cJSON_GetObjectItem(OldProgInfoObj, "prog_num"))!=NULL)
			{
				printf("old prog_num : %d \n",ProgNumOldObj->valueint);
				old_pn = ProgNumOldObj->valueint;
			}		
			
			if((ProgNumNewObj = cJSON_GetObjectItem(NewProgInfoObj, "prog_num"))!=NULL)
			{
				printf("new prog_num : %d \n",ProgNumNewObj->valueint);
				new_pn = ProgNumNewObj->valueint;
			}	

			p_mux_program = update_program_mapping(mux_chan, in_chan, old_pn,  old_pn);

			if((FcaModeObj = cJSON_GetObjectItem(OldProgInfoObj, "Fca_Mode"))!=NULL)
				{
					printf("Fca_Mode : %d \n",FcaModeObj->valueint);
					p_mux_program->cas_mode = FcaModeObj->valueint;
					
				}					
				if((ProgramTypeObj = cJSON_GetObjectItem(OldProgInfoObj, "Program_Type"))!=NULL)
				{
					printf("Program_Type : %d \n",ProgramTypeObj->valueint);
					p_mux_program->service_info.service_type = ProgramTypeObj->valueint;
				}
				
				if((PMTPidOldObj = cJSON_GetObjectItem(OldProgInfoObj, "PMT_pid"))!=NULL)
				{
					printf("old PMT_pid : %d \n",PMTPidOldObj->valueint);
					p_mux_program->in_pmt_pid = PMTPidOldObj->valueint;
				}	
				
				if((ProgNameOldObj = cJSON_GetObjectItem(OldProgInfoObj, "prog_name"))!=NULL)
				{
					printf(" ProgNameOldLast : %s \n",ProgNameOldObj->valuestring);
				}
				if((ProviderNameOldObj = cJSON_GetObjectItem(OldProgInfoObj, "provider_name"))!=NULL)
				{
					printf("ProviderNameOldLast : %s \n",ProviderNameOldObj->valuestring);
				}					

				
				if((PMTPidNewObj = cJSON_GetObjectItem(NewProgInfoObj, "PMT_pid"))!=NULL)
				{
					printf("new PMT_pid : %d \n",PMTPidNewObj->valueint);
				}	
				
				if((ProgNameNewObj = cJSON_GetObjectItem(NewProgInfoObj, "prog_name"))!=NULL)
				{
					printf("ProgNameNewLast : %s \n",ProgNameNewObj->valuestring);
					strcpy(p_mux_program->service_info.service_name, ProgNameNewObj->valuestring);
				}
				if((ProviderNameNewsObj = cJSON_GetObjectItem(NewProgInfoObj, "provider_name"))!=NULL)
				{
					printf("ProviderNameNewLast : %s \n",ProviderNameNewsObj->valuestring);
					strcpy(p_mux_program->service_info.provider_name, ProviderNameNewsObj->valuestring);
				}
				if((RemoveCAObj = cJSON_GetObjectItem(NewProgInfoObj, "remove_ca"))!=NULL)
				{
					printf("new remove_ca : %d \n",RemoveCAObj->valueint);
				}
				
			
		}

		//remux(mux_chan);
	}
}

v_32 Set_Remap_Infos_func(cJSON *val_arr_wrap)
{
	u_int value_size_1=0,value_size_2=0,value_size_3=0,value_size_4=0,value_size_5=0;
	u_int j=0,k=0,m=0;
	cJSON *IndexObj,*ChnameObj,*RemapinfosObj,*OtherpidsObj;
	cJSON *RemapItemObj,*SrcObj,*OriProgNameObj,*ProgramTypeObj,*MuxPidinfosObj;
	cJSON *MuxPidItemObj,*PIDObj,*TypeObj,*MuxPidObj,*LockFlagObj,*ReleFlagObj,*ReleMuxchObj,*ReleMuxchItemObj;
	cJSON *OtherpidItemObj,*PIDObj2,*TypeObj2,*MuxPidObj2,*LockFlagObj2,*ReleFlagObj2,*ReleMuxchObj2,*ReleMuxchItemObj2;
	cJSON *PidPassObj,*PidPassItemObj,*PassIndexObj,*PassSrcObj,*PassSrcNameObj,*PassPidObj,*PassRemapPidObj;
	int mux_chan, in_chan, old_pn, new_pn, in_pid, mux_pid;
	mux_channel_t *p_mux_channel;
	mux_program_t *p_mux_program;

	
	/*--index Value[i][0] part--*/
	if((IndexObj = cJSON_GetObjectItem(val_arr_wrap, "Index")) != NULL)
	{
		printf("index : 0x%04X \n",IndexObj->valueint);	
		mux_chan = IndexObj->valueint;
		p_mux_channel= &mux_channels[mux_chan];
	}
	
	if((ChnameObj = cJSON_GetObjectItem(val_arr_wrap, "Chname"))!=NULL)
	{
		printf("ChnameLast : %s \n",ChnameObj->valuestring);
	}
	
	if((RemapinfosObj = cJSON_GetObjectItem(val_arr_wrap, "Remapinfos")) != NULL)
	{
		value_size_1 = cJSON_GetArraySize(RemapinfosObj);
		for(j=0;j<value_size_1;j++)
		{
			RemapItemObj = cJSON_GetArrayItem(RemapinfosObj, j);
			if((SrcObj = cJSON_GetObjectItem(RemapItemObj, "Src"))!=NULL)
			{
				printf("Src : %d \n",SrcObj->valueint);
				in_chan = SrcObj->valueint;
			}

			p_mux_program = find_mux_program_by_newpn(mux_chan, new_pn);
			
			if((OriProgNameObj = cJSON_GetObjectItem(RemapItemObj, "ori_prog_name"))!=NULL)
			{			
				printf("OriProgNameLast : %s \n",OriProgNameObj->valuestring);
			}
			
			if((ProgramTypeObj = cJSON_GetObjectItem(RemapItemObj, "Program_Type"))!=NULL)
			{
				printf("Program_Type : %d \n", ProgramTypeObj->valueint);
			}
			
			if((MuxPidinfosObj = cJSON_GetObjectItem(RemapItemObj, "MuxPidinfos"))!=NULL)
			{
				value_size_3 = cJSON_GetArraySize(MuxPidinfosObj);
				for(k=0;k<value_size_3;k++)
				{
					MuxPidItemObj = cJSON_GetArrayItem(MuxPidinfosObj, k);
					if((PIDObj = cJSON_GetObjectItem(MuxPidItemObj, "pid"))!=NULL)
					{
						printf("pid : %d \n",PIDObj->valueint);
						p_mux_program->es_pid_mapping[k][0] = PIDObj->valueint;
					}
					if((TypeObj = cJSON_GetObjectItem(MuxPidItemObj, "type"))!=NULL)
					{
						printf("type : %d \n",TypeObj->valueint);
						p_mux_program->es_pid_mapping[k][2] = TypeObj->valueint;
					}
					if((MuxPidObj = cJSON_GetObjectItem(MuxPidItemObj, "mux_pid"))!=NULL)
					{
						printf("mux_pid : %d \n",MuxPidObj->valueint);
						p_mux_program->es_pid_mapping[k][1] = MuxPidObj->valueint;
					}
					if((LockFlagObj = cJSON_GetObjectItem(MuxPidItemObj, "lock_flag"))!=NULL)
					{
						printf("lock_flag : %d \n",LockFlagObj->valueint);
					}
					if((ReleFlagObj = cJSON_GetObjectItem(MuxPidItemObj, "rele_flag"))!=NULL)
					{
						printf("rele_flag : %d \n",ReleFlagObj->valueint);
					}
					if((ReleMuxchObj = cJSON_GetObjectItem(MuxPidItemObj, "rele_muxch"))!=NULL)
					{
						value_size_4 = cJSON_GetArraySize(ReleMuxchObj);
						for(m=0;m<value_size_4;m++)
						{
							ReleMuxchItemObj = cJSON_GetArrayItem(ReleMuxchObj, m);
							printf("ReleMuxch : %d \n",ReleMuxchItemObj->valueint);
						}
					}
				}		
			}	
		}
	}
	
	if((OtherpidsObj = cJSON_GetObjectItem(val_arr_wrap, "Other_pids")) != NULL)
	{
		value_size_2 = cJSON_GetArraySize(OtherpidsObj);
		for(j=0;j<value_size_2;j++)
		{
			OtherpidItemObj = cJSON_GetArrayItem(OtherpidsObj, j);

			if((PIDObj2 = cJSON_GetObjectItem(OtherpidItemObj, "pid"))!=NULL)
			{
				printf("pid2 : %d \n",PIDObj2->valueint);
			}
			if((TypeObj2 = cJSON_GetObjectItem(OtherpidItemObj, "type"))!=NULL)
			{
				printf("type : %d \n",TypeObj2->valueint);
			}
			if((MuxPidObj2 = cJSON_GetObjectItem(OtherpidItemObj, "mux_pid"))!=NULL)
			{
				printf("mux_pid2 : %d \n",MuxPidObj2->valueint);
			}
			if((LockFlagObj2 = cJSON_GetObjectItem(OtherpidItemObj, "lock_flag"))!=NULL)
			{
				printf("lock_flag2 : %d \n",LockFlagObj2->valueint);
			}
			if((ReleFlagObj2 = cJSON_GetObjectItem(OtherpidItemObj, "rele_flag"))!=NULL)
			{
				printf("rele_flag2 : %d \n",ReleFlagObj2->valueint);
			}
			if((ReleMuxchObj2 = cJSON_GetObjectItem(OtherpidItemObj, "rele_muxch"))!=NULL)
			{
				value_size_4 = cJSON_GetArraySize(ReleMuxchObj2);
				for(m=0;m<value_size_4;m++)
				{
					ReleMuxchItemObj2 = cJSON_GetArrayItem(ReleMuxchObj2, m);
					printf("ReleMuxch2 : %d \n",ReleMuxchItemObj2->valueint);
				}
			}

		}

	}	

	//lxzheng 2016/04/25
	if((PidPassObj = cJSON_GetObjectItem(val_arr_wrap, "pid_pass")) != NULL)
	{
		value_size_5 = cJSON_GetArraySize(PidPassObj);
		for(j=1;j<value_size_5;j++) //数组【0】是 源，所以 从1开始 
		{
			PidPassItemObj = cJSON_GetArrayItem(PidPassObj, j);
			if((PassIndexObj = cJSON_GetObjectItem(PidPassItemObj, "index"))!=NULL)
			{
				printf("index : %d \n",PassIndexObj->valueint);
			}
			
			if((PassSrcObj = cJSON_GetObjectItem(PidPassItemObj, "Src"))!=NULL)
			{
				printf("Src : %d \n",PassSrcObj->valueint);
				in_chan = PassSrcObj->valueint;
			}
			
			if((PassSrcNameObj = cJSON_GetObjectItem(PidPassItemObj, "Src_name"))!=NULL)
			{
				printf("Src : %s \n",PassSrcNameObj->valuestring);
			}
			
			if((PassPidObj = cJSON_GetObjectItem(PidPassItemObj, "pid"))!=NULL)
			{
				printf("pid : %d \n",PassPidObj->valueint);
				in_pid = PassPidObj->valueint;
			}
			
			if((PassRemapPidObj = cJSON_GetObjectItem(PidPassItemObj, "remap_pid"))!=NULL)
			{
				printf("remap_pid : %d \n",PassRemapPidObj->valueint);
				mux_pid = PassRemapPidObj->valueint;
			}		

			add_pid_mapping(mux_chan, in_chan, in_pid, mux_pid);
		}		
	}
	//remux(mux_chan);
}
/* End lxzheng 2016/05/06 for main board Remux */





 
#include "appref.h"

// version, name, size
blob_parameter_st tsoip_in_param = {0x04, "tsoip_in_channels", 16};
blob_parameter_st tsoip_out_param = {0x01, "tsoip_out_channels", sizeof(tsoip_out_channel_st)};
//blob_parameter_st tsoip_network_param = {0x01, "tsoip_network", sizeof(tsoip_network_params_st)};
blob_parameter_st tsoip_network_param = {0x02, "tsoip_network", 16};



#ifndef SAVE_IN_DB
#define FRAM_SN_OFFSET		0x0
#define FRAM_MAC_OFFSET	0x18

#define FRAM_MAC0_OFFSET	(FRAM_MAC_OFFSET + (8*0))
#define FRAM_MAC1_OFFSET	(FRAM_MAC_OFFSET + (8*1))
#define FRAM_MAC2_OFFSET	(FRAM_MAC_OFFSET + (8*2))


#define FRAM_MNGNETWORK_OFFSET	0x40
#define FRAM_ADMINPASSWD_OFFSET	0x60
#define FRAM_WEBACCOUNT_OFFSET	0x70
#define FRAM_TEMPORATURE_OFFSET	0xa0

#define FRAM_OEM_OFFSET	0xb0

//#define FRAM_END_OFFSET	0xc0


#define FRAM_OTHER_OFFSET	0x100


int fram_write_sn(char *sn)
{
	char databuf[24];

	databuf[0] = 'm';
	databuf[1] = 'a';
	databuf[2] = 'p';
	databuf[3] = 'z';

	memcpy(&databuf[4], sn, 20);

	return write_fram(FRAM_SN_OFFSET, databuf, 24);
}

int fram_read_sn(char *sn)
{
	char databuf[24];
	
	if(read_fram(FRAM_SN_OFFSET, databuf, 24)<0){
		return -1;
	}

	if((databuf[0] == 'm') && (databuf[1] == 'a') && (databuf[2] == 'p') && (databuf[3] == 'z')){
		memcpy(sn, &databuf[4], 20);
		return 0;
	}else{
		return -1;
	}
}

int fram_write_mac(int idx, char *mac)
{
	char databuf[8];
	
	databuf[0] = 0xf6;
	databuf[1] = 0x04;

	memcpy(&databuf[2], mac, 6);

	return write_fram(FRAM_MAC_OFFSET+8*idx, databuf, 8);

	
}

int fram_read_mac(int idx, char *mac)
{
	char databuf[8];

	if(read_fram(FRAM_MAC_OFFSET+8*idx, databuf, 8)<0){
		return -1;
	}

	if((databuf[0] == 0xf6) && (databuf[1] == 0x04)){
		memcpy(mac, &databuf[2], 6);
		return 0;
	}else{
		return -1;
	}
	
}

int fram_write_network(void)
{
	char databuf[20];
	
	databuf[0] = 0xf6;
	databuf[1] = 0x04;

	memcpy(&databuf[2], &mng_network->ipaddr, 4);
	memcpy(&databuf[6], &mng_network->netmask, 4);
	memcpy(&databuf[10], &mng_network->gateway, 4);
	memcpy(&databuf[14], &mng_network->trapip, 4);

	return write_fram(FRAM_MNGNETWORK_OFFSET, databuf, 18);
	
}

int fram_read_network(void)
{
	char databuf[20];

	if(read_fram(FRAM_MNGNETWORK_OFFSET, databuf, 18)<0){
		return -1;
	}

	if((databuf[0] == 0xf6) && (databuf[1] == 0x04)){
		memcpy(&mng_network->ipaddr, &databuf[2], 4);
		memcpy(&mng_network->netmask, &databuf[6], 4);
		memcpy(&mng_network->gateway, &databuf[10], 4);
		memcpy(&mng_network->trapip, &databuf[14], 4);
		return 0;
	}else{
		return -1;
	}
	
}


int fram_write_ipaddr(u_int ipaddr)
{
	return write_fram(FRAM_MNGNETWORK_OFFSET+2, &ipaddr, 4);
}

int fram_write_netmask(u_int netmask)
{
	return write_fram(FRAM_MNGNETWORK_OFFSET+6, &netmask, 4);
}

int fram_write_gateway(u_int gateway)
{
	return write_fram(FRAM_MNGNETWORK_OFFSET+10, &gateway, 4);
}

int fram_write_trapip(u_int trapip)
{
	return write_fram(FRAM_MNGNETWORK_OFFSET+14, &trapip, 4);
}

int fram_write_adminpasswd(u_int passwd)
{
	char databuf[0x10];
	
	databuf[0] = 0xf6;
	databuf[1] = 0x04;

	memcpy(&databuf[2], &passwd, 4);

	return write_fram(FRAM_ADMINPASSWD_OFFSET, databuf, 6);
	
}

int fram_read_adminpasswd(u_int *p_passwd)
{
	char databuf[0x10];

	if(read_fram(FRAM_ADMINPASSWD_OFFSET, databuf, 6)<0){
		return -1;
	}

	if((databuf[0] == 0xf6) && (databuf[1] == 0x04)){
		memcpy(p_passwd, &databuf[2], 4);
		return 0;
	}else{
		return -1;
	}
	
}


int fram_write_webaccount(void)
{
	char databuf[44];
	
	databuf[0] = 0xf6;
	databuf[1] = 0x04;

	memcpy(&databuf[2], sysinfo->loginuser, 20);
	memcpy(&databuf[22], sysinfo->loginpasswd, 20);

	return write_fram(FRAM_WEBACCOUNT_OFFSET, databuf, 42);
	
}

int fram_read_webaccount(void)
{
	char databuf[44];

	if(read_fram(FRAM_WEBACCOUNT_OFFSET, databuf, 42)<0){
		return -1;
	}

	if((databuf[0] == 0xf6) && (databuf[1] == 0x04)){
		memcpy(sysinfo->loginuser, &databuf[2], 20);
		memcpy(sysinfo->loginpasswd, &databuf[22], 20);
		return 0;
	}else{
		return -1;
	}
	
}

int fram_write_webuser(char *username)
{
	return write_fram(FRAM_WEBACCOUNT_OFFSET+2, username, 20);
}

int fram_write_webpasswd(char *passwd)
{
	return write_fram(FRAM_WEBACCOUNT_OFFSET+22, passwd, 20);
}

short int2short(int v_32)
{
	short v_s16;
	
	if(v_32<0){
		v_s16 = -v_32;
		v_s16 |= (1<<15);
	}else{
		v_s16 = v_32;
	}
	
	return v_s16;
}

int short2int(short v_s16)
{
	int v_32;

	if(v_s16&(1<<15)){
		v_32 = -(v_s16 & 0x7fff);
	}else{
		v_32 = v_s16;
	}
	
	return v_32;
}

int fram_write_temperature_sensor(void)
{
	short v_s16;
	char databuf[8];
	
	databuf[0] = 0xf6;
	databuf[1] = 0x04;

	v_s16 = int2short(sysinfo->temperature_high);
	memcpy(&databuf[2], &v_s16, 2);
	v_s16 = int2short(sysinfo->temperature_low);
	memcpy(&databuf[4], &v_s16, 2);

	return write_fram(FRAM_TEMPORATURE_OFFSET, databuf, 6);
	
}

int fram_read_temperature_sensor(void)
{
	short v_s16;
	char databuf[8];

	if(read_fram(FRAM_TEMPORATURE_OFFSET, databuf, 6)<0){
		return -1;
	}

	if((databuf[0] == 0xf6) && (databuf[1] == 0x04)){
		memcpy(&v_s16, &databuf[2], 2);
		sysinfo->temperature_high = short2int(v_s16);
		memcpy(&v_s16, &databuf[4], 2);
		sysinfo->temperature_low = short2int(v_s16);
			if(sysinfo->temperature_high>150*4){
				sysinfo->temperature_high=150*4;
			}else if(sysinfo->temperature_high<60*4){
				sysinfo->temperature_high = 60*4;
			}
			if(sysinfo->temperature_low>20*4){
				sysinfo->temperature_low=20*4;
			}else if(sysinfo->temperature_low<-40*4){
				sysinfo->temperature_low = -40*4;
			}
	
		return 0;
	}else{
		return -1;
	}
	
}

int fram_write_t_high(int t_high)
{
	short v_s16 = int2short(t_high);

	return write_fram(FRAM_TEMPORATURE_OFFSET+2, &v_s16, 2);
}

int fram_write_t_low(int t_low)
{
	short v_s16 = int2short(t_low);
	return write_fram(FRAM_TEMPORATURE_OFFSET+4, &v_s16, 2);
}

int fram_write_oem(void)
{
	char databuf[44];
	
	databuf[0] = 0xf6;
	databuf[1] = 0x04;

	memcpy(&databuf[2], sysinfo->product_name, 20);

	return write_fram(FRAM_OEM_OFFSET, databuf, 22);
	
}

int fram_read_oem(void)
{
	char databuf[44];

	if(read_fram(FRAM_OEM_OFFSET, databuf, 22)<0){
		return -1;
	}

	if((databuf[0] == 0xf6) && (databuf[1] == 0x04)){
		memcpy(sysinfo->product_name, &databuf[2], 20);
		return 0;
	}else{
		return -1;
	}
	
}

int fram_write_productname(char *productname)
{
	return write_fram(FRAM_OEM_OFFSET, productname, strlen(productname)+1);
}
#endif

void save_mngnetwork_param(void)
{
	save_mngnetwork_ipaddr(mng_network->ipaddr);
	save_mngnetwork_netmask(mng_network->netmask);
	save_mngnetwork_gateway(mng_network->gateway);
	save_mngnetwork_trapip(mng_network->trapip);


	//fram_write_mac(0, mng_network.mac_addr);

	
	//save_parameters(INTEGER_PARAM, 3, "mng_network_ipaddr", 0, mng_network.ipaddr);
}



typedef struct{
	int type;
	int argc;
	u_int argv[];
}passed_parameters_st;


static int save_parameters_task(	passed_parameters_st *parameter)
{
	u_int v_u32;
	int len;

	if(parameter == NULL){
		return -1;
	}

	switch(parameter->type){
		case NETWORK_PARAM	:
			save_mngnetwork_ipaddr(mng_network->ipaddr);
			save_mngnetwork_netmask(mng_network->netmask);
			save_mngnetwork_gateway(mng_network->gateway);
			save_mngnetwork_trapip(mng_network->trapip);

			break;
		default :
			break;
	}
	
	free(parameter);
	
}

int save_parameters(int type, int argc, ...)
{
	int i;
	u_int v_u32 = 0;
	passed_parameters_st *parameter;
		
	va_list ap;    

	if(argc<=0){
		return -1;
	}
	
	parameter = xMalloc(sizeof(passed_parameters_st) + argc*sizeof(u_int));
	parameter->type = type;
	parameter->argc = argc;

	va_start(ap, argc);

	for(i=0; i<argc; i++){
		v_u32 = va_arg(ap, u_int);
		parameter->argv[i] = v_u32;
	}

	va_end(ap); 

	switch(type){
		case INTEGER_PARAM	:
			replace_db_integer((char *) parameter->argv[0],  parameter->argv[1], parameter->argv[2]);
			free(parameter);
			break;
		case FLOAT_PARAM	:
			replace_db_float((char *) parameter->argv[0],  parameter->argv[1], *((double *)parameter->argv[2]));
			free(parameter);
			break;
		case TEXT_PARAM	:
			replace_db_text((char *) parameter->argv[0],  parameter->argv[1], (char *)parameter->argv[2]);
			free(parameter);
			break;
		case BLOB_PARAM	:
			replace_db_blob((char *) parameter->argv[0],  parameter->argv[1], (void *)parameter->argv[2], parameter->argv[3]);
			free(parameter);
			break;
		default :
			add_async_task((taskHandler)save_parameters_task, parameter, 4);

	}

	return 0;
}

#if 1
int download_parameters(char *databuf)
{
	//目前需要下载的参数有
	//下载的参数应包括参数的版本
	int i, len, rdlen;

	len = read_from_file(DBFILE, databuf);

	//data_encrypt(databuf, , )
	
	return len;
}

int get_parameters(char *filename, char *databuf)
{
printf("------ get_parameters---- \n");
	int i, len, rdlen;

	len = read_from_file(filename, databuf);

	//data_encrypt(databuf, , )
	
	return len;
}



int upload_parameters(char *databuf, int datalen)
{
	int i, len, offset;
	u_int func_type;

	//unlink(DBFILE);
	remove(DBFILE);
	write_to_file(DBFILE,  databuf, datalen);

	return 0;
}
#endif

void default_parameters(void)
{
	int i;
	
	//strcpy(sysinfo.device_name, PRODUCT_NAME);
	
	//init_tsoip_networks();
	//init_tsoip_in_channels();
	//init_tsoip_out_channels();

	remove(DBFILE);
	system("reboot");
}

void default_setting(void)
{
	int i;
	
	//save_devicename(sysinfo.device_name);
	
	default_parameters();

	//init_tsoip();

	//for(i=0; i<=sysinfo->board_qty; i++){
		//empty_transceiver_route(i);
	//}
}

// 恢复出厂设置是除了产品序号和3个网口的mac，其它的都要恢复，
// 只能在前面板执行，并且需要管理员口令
// 板卡信息和transceiver route 不确定是否要恢复，并且还不确定要怎么恢复
void factory_setting(void)
{

	#ifndef SAVE_IN_DB
	sysinfo->admin_passwd = 100000000;
	save_adminpasswd(sysinfo->admin_passwd);

	strcpy(sysinfo->product_name, PRODUCT_NAME);

	strcpy(sysinfo->loginuser, LOGIN_USER);
	strcpy(sysinfo->loginpasswd, LOGIN_PASSWD);

	fram_write_oem();
	fram_write_webaccount();
	#endif
	
	unlink(DBFILE);
	
}

int load_parameters(void)
{
	int v_s32, tmpval;
	char *text_val;
	int i, len;

	strcpy(sysinfo->loginuser, LOGIN_USER);
	strcpy(sysinfo->loginpasswd, LOGIN_PASSWD);

	sysinfo->temperature_high = TEMPORATURE_HIGH;
	sysinfo->temperature_low = TEMPORATURE_LOW;
	
	select_db_text("device_name", 0, sysinfo->device_name);

	//#if 0
	#ifdef SAVE_IN_DB
	select_db_text("product_name", 0, sysinfo->product_name);
	
	select_db_integer("mng_network_ipaddr", 0, &mng_network->ipaddr);
	select_db_integer("mng_network_netmask", 0, &mng_network->netmask);
	select_db_integer("mng_network_gateway", 0, &mng_network->gateway);
	select_db_integer("mng_network_trap_ip", 0, &mng_network->trapip);

	
	select_db_integer("temperature_high", 0, &sysinfo->temperature_high);
	select_db_integer("temperature_low", 0, &sysinfo->temperature_low);
	
	/*
	select_db_blob("mng_network_mac_addr", mng_network.mac_addr, 6);
	//*/
	
	select_db_text("loginuser", 0, sysinfo->loginuser);
	select_db_text("loginpasswd", 0, sysinfo->loginpasswd);

	#else

	if(fram_read_oem()<0){
		fram_write_oem();
	}

	if(fram_read_temperature_sensor()<0){
		fram_write_temperature_sensor();
	}


	if(fram_read_network()<0){
		fram_write_network();
	}

	if(fram_read_webaccount()<0){
		fram_write_webaccount();
	}
	
	#endif

	tmpval = 0;
	for(i=0; i<=sysinfo->board_qty; i++){
		if(read_temperature_control_flag(i, &v_s32)==0){
			sysinfo->slot_info[i].tc_flag = v_s32;
			if(v_s32){
				tmpval = 1;
			}
		}
	}

	if(read_fan_temperature_control(&v_s32)==0){
		sysinfo->fan_tempctrl = v_s32;
	}

	if((tmpval==0) && (sysinfo->fan_tempctrl==0)){
		if(read_fan_pwm(&v_s32)==0){
			set_fans_speed(v_s32);
		}
	}

	set_temperature_high(sysinfo->temperature_high);
	set_temperature_low(sysinfo->temperature_low);
	
	if(read_macaddr(0, mng_network->macaddr)==0){
		set_mac_addr(sysinfo->netif_name, mng_network->macaddr);
	}
	
	set_ip_addr(sysinfo->netif_name, mng_network->ipaddr, mng_network->netmask);
	del_default_gw(mng_network->old_gw);
	set_default_gw(mng_network->gateway);;

	select_db_text("ntp_host_name", 0, sysinfo->ntp_hostname);
	select_db_integer("ntp_host_ipaddr", 0, &sysinfo->ntp_hostip);
	select_db_integer("timezone", 0, &sysinfo->timezone);


	
	
	//*
	select_db_blob("tsoip_in_backup", 0, &tsoip_in_backup, 12);
	if(tsoip_in_backup.backup_mode){
		tsoip_in_backup.channel_qty = tsoip_network[0].inchan_qty;
		len = sizeof(tsoip_in_backup.channel[0].src_ipaddr);
		for(i=0; i<sysinfo->ipin_channels; i++){
			select_db_blob("tsoip_in_channel_backup_src_ip", i, tsoip_in_backup.channel[i].src_ipaddr, len);
			select_db_blob("tsoip_in_channel_backup_threshold", 0, &tsoip_in_backup.channel[i].lower_bitrate, 4 * 2);
			select_db_integer("tsoip_in_channel_backup_src_mask", i, &tsoip_in_backup.channel[i].src_mask);
			tsoip_in_backup.channel[i].src_mask |= 1;
		}
	}else{
		tsoip_in_backup.channel_qty = sysinfo->ipin_channels;
	}
	
	//*/
	//tsoip_in_backup.channel_qty = tsoip_network[0].inchan_qty;
	

	//*
	if((select_db_integer("tsoip_network_version", 0, &v_s32)==0) && (v_s32 == tsoip_network_param.version)){
		for(i=0; i<sysinfo->tsip_port; i++){
			select_db_blob(tsoip_network_param.name, i, &tsoip_network[i], tsoip_network_param.size);
		}
	}else{
		replace_db_integer("tsoip_network_version", 0, tsoip_network_param.version);
		delete_db_blob("tsoip_network%", -1);
	}
	//*/

	if(select_db_integer("tsoip_igmp_mode", 0, &v_s32)==0){
		tsoip_network[0].igmp_mode = v_s32;
		tsoip_network[1].igmp_mode = v_s32;
	}


	//*
	if((select_db_integer("tsoip_in_version", 0, &v_s32)==0) && (v_s32 == tsoip_in_param.version)){
		select_db_blob("ipin_idx_ref", 0, ipin_idx_ref, 4 * sysinfo->ipin_channels);

		for(i=0; i<sysinfo->ipin_channels; i++){
			select_db_blob(tsoip_in_param.name, i, &tsoip_in_channel[i], tsoip_in_param.size);
		}
		
	}else{
		replace_db_integer("tsoip_in_version", 0, tsoip_in_param.version);
		delete_db_blob("tsoip_in_channels%", -1);
	}
	//*/


	//*
	if((select_db_integer("tsoip_out_version", 0, &v_s32)==0) && (v_s32 == tsoip_out_param.version)){
		for(i=0; i<sysinfo->ipout_channels; i++){
			select_db_blob(tsoip_out_param.name, i, &tsoip_out_channel[i], tsoip_out_param.size);
		}
		select_db_blob("ipout_idx_ref", 0, ipout_idx_ref, 4 * sysinfo->ipout_channels);
	}else{
		replace_db_integer("tsoip_out_version", 0, tsoip_out_param.version);
		delete_db_blob("tsoip_out_channels%", -1);
	}
	//*/

	return 0;
}




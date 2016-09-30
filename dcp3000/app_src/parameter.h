
#ifndef ___PARAMETER_H___
#define ___PARAMETER_H___

#include "sqlite3/app_sql.h"

//#define SAVE_IN_DB


typedef struct{
	int version;
	char *name;
	int size;
}blob_parameter_st;


blob_parameter_st tsoip_in_param;
blob_parameter_st tsoip_out_param;
blob_parameter_st tsoip_network_param;


int save_parameters(int type, int argc, ...);

void default_parameters(void);

void factory_setting(void);

int load_parameters(void);

int download_parameters(char *databuf);
int upload_parameters(char *databuf, int datalen);

void save_mngnetwork_param(void);

#define save_db_blob(blob_param, idx, data)	replace_db_blob(blob_param.name, idx, data, blob_param.size)

#define save_devicename(devicename)		replace_db_text("device_name", 0, devicename)

#define save_firmware_version(version)		replace_db_integer("firmware_version", 0, version)
#define read_firmware_version(p_version)		select_db_integer("firmware_version", 0, p_version)

#define read_fan_temperature_control(p_ctrl)		select_db_integer("fan_temperature_control", 0, p_ctrl)
#define save_fan_temperature_control(ctrl)		replace_db_integer("fan_temperature_control", 0, ctrl)
#define read_temperature_control_flag(idx, p_flag)		select_db_integer("temperature_control_flag", idx, p_flag)
#define save_temperature_control_flag(idx, flag)		replace_db_integer("temperature_control_flag", idx, flag)

#define read_fan_pwm(p_pwm)		select_db_integer("fan_pwm", 0, p_pwm)
#define save_fan_pwm(pwm)		replace_db_integer("fan_pwm", 0, pwm)

#ifdef SAVE_IN_DB

#define save_temperature_high(t_high)		replace_db_integer("temperature_high", 0, t_high)
#define save_temperature_low(t_low)		replace_db_integer("temperature_low", 0, t_low)

#define save_devsn(sn)		replace_db_text("device_sn", 0, sn)
#define save_macaddr(idx, mac)	replace_db_blob("macaddr", idx, mac, 6)
#define read_devsn(sn)		select_db_text("device_sn", 0, sn)
#define read_macaddr(idx, mac)	select_db_blob("macaddr", idx, mac, 6)

#define save_productname(productname)		replace_db_text("product_name", 0, productname)

#define save_adminpasswd(adminpasswd)		replace_db_integer("admin_passwd", 0, adminpasswd)
#define read_adminpasswd(p_adminpasswd)		select_db_integer("admin_passwd", 0, p_adminpasswd)

#define save_loginuser(loginuser)		replace_db_text("loginuser", 0, loginuser)
#define save_loginpasswd(loginpasswd)		replace_db_text("loginpasswd", 0, loginpasswd)


#define save_mngnetwork_ipaddr(ipaddr)		replace_db_integer("mng_network_ipaddr", 0, ipaddr)
#define save_mngnetwork_netmask(netmask)		replace_db_integer("mng_network_netmask", 0, netmask)
#define save_mngnetwork_gateway(gateway)		replace_db_integer("mng_network_gateway", 0, gateway)
#define save_mngnetwork_trapip(trapip)		replace_db_integer("mng_network_trap_ip", 0, trapip)
#else
int fram_write_sn(char *sn);
int fram_write_mac(int idx, char *mac);
int fram_read_sn(char *sn);
int fram_read_mac(int idx, char *mac);

int fram_write_t_high(int t_high);
int fram_write_t_low(int t_low);

#define save_temperature_high(t_high)		fram_write_t_high(t_high)
#define save_temperature_low(t_low)		fram_write_t_low(t_low)

#define save_devsn(sn)		fram_write_sn(sn)
#define save_macaddr(idx, mac)	fram_write_mac(idx, mac)
#define read_devsn(sn)		fram_read_sn(sn)
#define read_macaddr(idx, mac)	fram_read_mac(idx, mac)

int fram_write_ipaddr(u_int ipaddr);
int fram_write_netmask(u_int netmask);
int fram_write_gateway(u_int gateway);
int fram_write_trapip(u_int trapip);

int fram_write_adminpasswd(u_int passwd);
int fram_read_adminpasswd(u_int *p_passwd);

int fram_write_webuser(char *username);
int fram_write_webpasswd(char *passwd);

int fram_write_productname(char *productname);

#define save_productname(productname)		fram_write_productname(productname)

#define save_adminpasswd(adminpasswd)		fram_write_adminpasswd(adminpasswd)
#define read_adminpasswd(adminpasswd)		fram_read_adminpasswd(adminpasswd)

#define save_loginuser(loginuser)		fram_write_webuser(loginuser)
#define save_loginpasswd(loginpasswd)		fram_write_webpasswd(loginpasswd)


#define save_mngnetwork_ipaddr(ipaddr)		fram_write_ipaddr(ipaddr)
#define save_mngnetwork_netmask(netmask)		fram_write_netmask(netmask)
#define save_mngnetwork_gateway(gateway)		fram_write_gateway(gateway)
#define save_mngnetwork_trapip(trapip)		fram_write_trapip(trapip)


#endif

#define save_ipin_backup()	 replace_db_blob("tsoip_in_backup", 0, &tsoip_in_backup, 4 * 3)
#define save_ipin_channel_backup_src_ip(idx)	 replace_db_blob("tsoip_in_channel_backup_src_ip", idx, tsoip_in_backup.channel[idx].src_ipaddr, 4 * 4)
#define save_ipin_channel_backup_threshold(idx)	 replace_db_blob("tsoip_in_channel_backup_threshold", idx, &tsoip_in_backup.channel[idx].lower_bitrate, 4 * 2)
#define save_ipin_channel_backup_src_mask(idx)	 replace_db_integer("tsoip_in_channel_backup_src_mask", idx, tsoip_in_backup.channel[idx].src_mask);
#define save_tsoip_igmp_mode(igmp_mode)	 replace_db_integer("tsoip_igmp_mode", 0, igmp_mode);

#define save_ipin_idx_ref()	 replace_db_blob("ipin_idx_ref", 0, ipin_idx_ref, 4 * sysinfo->ipin_channels)
#define save_ipout_idx_ref()	 replace_db_blob("ipout_idx_ref", 0, ipout_idx_ref, 4 * sysinfo->ipout_channels)


#define save_timezone(zone)		replace_db_integer("timezone", 0, zone);
#define save_ntp_hostip(ipaddr)		replace_db_integer("ntp_host_ipaddr", 0, ipaddr);
#define save_ntp_hostname(hostname)		replace_db_text("ntp_host_name", 0, hostname);

#define read_watchdog_enable(p_enable)		select_db_integer("watchdog_enable", 0, p_enable)
#define save_watchdog_enable(enable)		replace_db_integer("watchdog_enable", 0, enable)


#endif

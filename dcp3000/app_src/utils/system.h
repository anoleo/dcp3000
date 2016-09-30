#ifndef _UTILS_SYSTEM_H_
#define _UTILS_SYSTEM_H_

#include <pthread.h>
#include <sys/types.h>

#include "utils.h"

void *mem_map(u_int base_addr, u_int mem_size);
int create_detached_pthread(void *(*start_routine) (void *), void *arg, int stacksize);

u_int ip_a2i(char *ipstr);
char *ip_i2a(u_int ipaddr);
char *ip_i2str(u_int ipaddr, char *ipstr);
char *ip_i2strxxx(u_int ipaddr, char *ipstr);
char *mac_arr2str(char *macaddr, char *macstr);
char *mac_str2arr(char *macstr, char *macaddr);


#if 0
int sys_cmd(char *str_format, ...);
#else
extern pthread_mutex_t system_cmd_lock;
extern char system_cmd_buf[];

#define sys_cmd(str_fmt, ...)	do{ \
		pthread_mutex_lock(&system_cmd_lock); \
		sprintf(system_cmd_buf, str_fmt, ##__VA_ARGS__); \
		system(system_cmd_buf);\
		pthread_mutex_unlock(&system_cmd_lock);\
	}while(0)
#endif

extern int nif_socket_fd;

#define	IPROUTE_DEFAULT_GW	0
#define	IPROUTE_STATIC_GW	1
#define	IPROUTE_STATIC_NIF	2

typedef struct {
	int type;		// 0 default gw, 1  gw, 2  dev 
	u_int destip;
	u_int netmask;
	union {
		u_int gateway;
		char *nif_name;
	} next_dev;
}iproute_st;

#define IPROUTE(proute, route_type, dest_ip, net_mask, nif_gw)	do{ \
	(proute)->type = (route_type);	\
	(proute)->destip = (dest_ip);	\
	(proute)->netmask = (net_mask);	\
	if((route_type) == IPROUTE_STATIC_NIF)	\
		(proute)->next_dev.nif_name = (char *)(nif_gw);	\
	else		\
		(proute)->next_dev.gateway = (u_int)(nif_gw);	\
	}while(0)


#define set_mac_addr(nif_name, mac)	sys_cmd("ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x",  nif_name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5])
void set_ip_addr(char *nif_name, u_int ip_addr, u_int net_mask);
void route_add_gw(u_int dest_ip, u_int net_mask, u_int gateway);
void route_add_dev(u_int dest_ip, u_int net_mask, char *devname);
void route_del_gw(u_int dest_ip, u_int net_mask, u_int gateway);
void route_del_dev(u_int dest_ip, u_int net_mask, char *devname);
void route_del(u_int dest_ip, u_int net_mask);
void reset_default_gw(u_int gateway);
void set_default_gw(u_int gateway);
void del_default_gw(u_int gateway);

int add_ip_route(iproute_st *route);
int del_ip_route(iproute_st *route);

int init_nif_socket(void);
void free_nif_socket(void);

int get_nif_mac(char *nif_name, u_char *macaddr);
int get_nif_ipaddr(char *nif_name, u_int *ipaddr);
int get_nif_netmask(char *nif_name, u_int *netmask);


int set_multicast_ttl(unsigned char ttl);
int set_multicast_loop(int loop);
int imr_drop_membership(u_int nif_addr, char *nif_name, u_int multi_addr);
int imr_add_membership(u_int nif_addr, char *nif_name, u_int multi_addr);

int imr_unblock_src(u_int nif_addr,  u_int multi_addr, u_int src_addr);
int imr_block_src(u_int nif_addr,  u_int multi_addr, u_int src_addr);
int imr_add_src_membership(u_int nif_addr,  u_int multi_addr, u_int src_addr);
int imr_drop_src_membership(u_int nif_addr,  u_int multi_addr, u_int src_addr);

int set_nif_igmp_version(char *nif_name, int version);

int get_hostip(u_int *hostip, char *hostname);



typedef struct{
	pthread_mutex_t mutex;    /*mutex of condition variable wait for*/
	pthread_cond_t cond;      /*when thread idle, thread wait for this*/
	int time_ms;		// ms
}timeout_st;

typedef struct {
	timeout_st timeout;
	void (*handler)(void *);
	void *arg;
	int run_flag;
}period_task_st;


void init_timeout(timeout_st *p_timeout, int time_ms);
void break_timeout(timeout_st *p_timeout);
void update_timeout(timeout_st *p_timeout, int time_ms);
int wait_timeout(timeout_st *p_timeout);
period_task_st *create_period_task(void (*handler)(void *), void *arg, int time_ms);
int start_period_task(period_task_st  *task);


#define update_period_task(task, time_ms)	update_timeout(&((task)->timeout), time_ms)

int delay_run_task(void (*handler)(void *), void *arg, int time_ms);
 void sync_delay_run(void (*handler)(void *), void *arg, int time_ms);

/*
其中参数 how可设置的参数为：SIG_BLOCK， SIG_UNBLOCK，SIG_SETMASK
 
   SIG_BLOCK：
        按照参数  set 提供的屏蔽字，屏蔽信号。并将原信号屏蔽保存到oldset中。
           
   SIG_UNBLOCK：
     按照参数  set 提供的屏蔽字进行信号的解除屏蔽。针对Set中的信号进行解屏。
    
   SIG_SETMASK:
     按照参数  set 提供的信号设置重新设置系统信号设置。
//*/
void signal_procmask(int how, ...);
	 

int FileRecord_GetLock(int fd, struct flock *p_lock);
int FileRecord_SetLock(int fd, int type, int cmd, int whence, int start, int len);

#define file_getLock(fd, p_lock)		FileRecord_GetLock(fd, p_lock)

#define file_setLock(fd, type, whence, start, len)		FileRecord_SetLock(fd, type, F_SETLK, whence, start, len)
#define file_setLock_wait(fd, type, whence, start, len)		FileRecord_SetLock(fd, type, F_SETLKW, whence, start, len)

#define file_rdlock(fd, whence, start, len)		file_setLock(fd, F_RDLCK, whence, start, len)
#define file_wrlock(fd, whence, start, len)		file_setLock(fd, F_WRLCK, whence, start, len)
#define file_unlock(fd, whence, start, len)		file_setLock(fd, F_UNLCK, whence, start, len)
#define file_rdlock_wait(fd, whence, start, len)		file_setLock_wait(fd, F_RDLCK, whence, start, len)
#define file_wrlock_wait(fd, whence, start, len)		file_setLock_wait(fd, F_WRLCK, whence, start, len)


#endif

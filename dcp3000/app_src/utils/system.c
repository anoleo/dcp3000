#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <netdb.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include <linux/sockios.h>
#include <net/if.h>  

#include <sys/ioctl.h>

#include "system.h"
#include "../system.h"
#include "../basedef.h"


#define SYSTEM_CMD_BUF_SIZE	128

#if 0
int sys_cmd(char *str_fmt, ...)
{
	static char cmdstr[SYSTEM_CMD_BUF_SIZE];

	sprintf(cmdstr, str_fmt, ##__VA_ARGS__);

	return system(cmdstr);
}
#else
pthread_mutex_t system_cmd_lock = PTHREAD_MUTEX_INITIALIZER;
char system_cmd_buf[SYSTEM_CMD_BUF_SIZE];
#endif

int nif_socket_fd;

int create_detached_pthread(void *(*start_routine) (void *), void *arg, int stacksize)
{
	pthread_t thread;
	pthread_attr_t attr;

	if (pthread_attr_init(&attr) != 0){
		printf( "pthread_attr_init failure\n");
		return -1;
	}

	//*
	if(stacksize>0){
		if (pthread_attr_setstacksize(&attr, stacksize) != 0){
			printf( "pthread_attr_setstacksize failure\n");
			return -1;
		}
	}
	//*/

	//*
	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0){
		printf( "pthread_attr_setdetachstate failure\n");
		return -1;
	}
	//*/
	

	if (pthread_create(&thread, &attr, start_routine, arg) != 0){
		printf( "Create deteched thread failure\n");
		exit(1);
	}

	return 0;
}


void *mem_map(u_int base_addr, u_int mem_size)
{
	int memfd;
	void *map_addr;
	
	memfd = open("/dev/mem", O_RDWR | O_SYNC);	  //open mem device 
	if (memfd < 0) {
		printf("Error open memfd /dev/mem, exit\n");
		exit(1);
	}

	
	//MAP_PRIVATE
	map_addr = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, base_addr); 	//remap address
	if (map_addr == MAP_FAILED) {
		perror("mmap");
		exit(1);
	} else {
		printf("Map addr %08x to %08x!\n", base_addr, (int)map_addr);
	}
	
	
	close(memfd);

	return map_addr;
}

//little end
char *ip_i2strxxx(u_int ipaddr, char *ipstr)
{
	char *pch_ip;

	pch_ip = (char *)(&ipaddr);

	sprintf(ipstr, "%.3u.%.3u.%.3u.%.3u", pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]);

	return ipstr;
}

//little end
char *ip_i2str(u_int ipaddr, char *ipstr)
{
	char *pch_ip;

	pch_ip = (char *)(&ipaddr);

	sprintf(ipstr, "%u.%u.%u.%u", pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]);

	return ipstr;
}

//little end
char *ip_i2a(u_int ipaddr)
{
	char *pch_ip;
	static char ipstr[16];

	pch_ip = (char *)(&ipaddr);

	sprintf(ipstr, "%u.%u.%u.%u", pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]);

	return ipstr;
}

//little end
u_int ip_a2i(char *ipstr)
{
	char *p_ch, *pch_ip;
	u_int ipaddr;

	pch_ip = (char *)(&ipaddr);

	p_ch = ipstr;
	pch_ip[3] = (strtol(p_ch, &p_ch, 10) & 0xff);
	p_ch++;
	pch_ip[2] = (strtol(p_ch, &p_ch, 10) & 0xff);
	p_ch++;
	pch_ip[1] = (strtol(p_ch, &p_ch, 10) & 0xff);
	p_ch++;
	pch_ip[0] = (strtol(p_ch, &p_ch, 10) & 0xff);

	//printf("ip_a2i :  %s [%#.8x] \n", ipstr, ipaddr);

	return ipaddr;
}

char *mac_arr2str(char *macaddr, char *macstr)
{
	sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X"
		, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);
	return macstr;
}

char *mac_str2arr(char *macstr, char *macaddr)
{
	macaddr[0] = ((macstr[0]) << 4 & 0xf0) |((macstr[1]) & 0x0f);
	macaddr[1] = ((macstr[3]) << 4 & 0xf0) |((macstr[4]) & 0x0f);
	macaddr[2] = ((macstr[6]) << 4 & 0xf0) |((macstr[7]) & 0x0f);
	macaddr[3] = ((macstr[9]) << 4 & 0xf0) |((macstr[10]) & 0x0f);
	macaddr[4] = ((macstr[12]) << 4 & 0xf0) |((macstr[13]) & 0x0f);
	macaddr[5] = ((macstr[15]) << 4 & 0xf0) |((macstr[16]) & 0x0f);

	return macaddr;
}

//little end
void set_ip_addr(char *nif_name, u_int ip_addr, u_int net_mask)
{
	char *pch_ip, *pch_mask;

	pch_ip = (char *)(&ip_addr);
	pch_mask = (char *)(&net_mask);

	sys_cmd("ifconfig %s %u.%u.%u.%u netmask %u.%u.%u.%u", nif_name
		, pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0], pch_mask[3], pch_mask[2], pch_mask[1], pch_mask[0]);

	
}


void del_default_gw(u_int gateway)
{
	sys_cmd("route del default gw %s", ip_i2a(gateway));
}

void set_default_gw(u_int gateway)
{
	sys_cmd("route add default gw %s", ip_i2a(gateway));
}


//little end
void reset_default_gw(u_int gateway)
{
	sys_cmd("route del default");
	sys_cmd("route add default gw %s", ip_i2a(gateway));
}

//little end
void route_add_dev(u_int dest_net, u_int net_mask, char *devname)
{
	char *pch_ip, *pch_mask;

	pch_ip = (char *)(&dest_net);
	pch_mask = (char *)(&net_mask);

	sys_cmd(" route add -net %u.%u.%u.%u netmask %u.%u.%u.%u dev %s"
		, pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]
		, pch_mask[3], pch_mask[2], pch_mask[1], pch_mask[0]
		, devname);

}

//little end
void route_add_gw(u_int dest_ip, u_int net_mask, u_int gateway)
{
	char *pch_ip, *pch_mask, *pch_gw;

	pch_ip = (char *)(&dest_ip);
	pch_mask = (char *)(&net_mask);
	pch_gw = (char *)(&gateway);

	if(net_mask==0xffffffff){
		sys_cmd(" route add -host %u.%u.%u.%u  gw %u.%u.%u.%u"
		, pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]
		, pch_gw[3], pch_gw[2], pch_gw[1], pch_gw[0]);
	}else{
		sys_cmd(" route add -net %u.%u.%u.%u netmask %u.%u.%u.%u gw %u.%u.%u.%u"
			, pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]
			, pch_mask[3], pch_mask[2], pch_mask[1], pch_mask[0]
			, pch_gw[3], pch_gw[2], pch_gw[1], pch_gw[0]);
	}

}

//little end
void route_del_dev(u_int dest_net, u_int net_mask, char *devname)
{
	char *pch_ip, *pch_mask;

	pch_ip = (char *)(&dest_net);
	pch_mask = (char *)(&net_mask);

	sys_cmd(" route del -net %u.%u.%u.%u netmask %u.%u.%u.%u dev %s"
		, pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]
		, pch_mask[3], pch_mask[2], pch_mask[1], pch_mask[0]
		, devname);

}

//little end
void route_del_gw(u_int dest_ip, u_int net_mask, u_int gateway)
{
	char *pch_ip, *pch_mask, *pch_gw;

	pch_ip = (char *)(&dest_ip);
	pch_mask = (char *)(&net_mask);
	pch_gw = (char *)(&gateway);

	sys_cmd(" route add -net %u.%u.%u.%u netmask %u.%u.%u.%u gw %u.%u.%u.%u"
		, pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]
		, pch_mask[3], pch_mask[2], pch_mask[1], pch_mask[0]
		, pch_gw[3], pch_gw[2], pch_gw[1], pch_gw[0]);

}


//little end
void route_del(u_int dest_net, u_int net_mask)
{
	char *pch_ip, *pch_mask;

	pch_ip = (char *)(&dest_net);
	pch_mask = (char *)(&net_mask);

	sys_cmd(" route del -net %u.%u.%u.%u netmask %u.%u.%u.%u"
		, pch_ip[3], pch_ip[2], pch_ip[1], pch_ip[0]
		, pch_mask[3], pch_mask[2], pch_mask[1], pch_mask[0]);
}


#if 0
void set_mac_addr(char *nif_name, char *mac)
{

	 sys_cmd("ifconfig %s hw ether %02x:%02x:%02x:%02x:%02x:%02x",  nif_name, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

}
#endif

int init_nif_socket(void)
{
	nif_socket_fd =  socket(AF_INET, SOCK_DGRAM, 0);
	if (nif_socket_fd < 0) {
		perror("socket()");
		return -1;
	}

	return nif_socket_fd;
}

void free_nif_socket(void)
{
	close(nif_socket_fd);
}

int get_nif_mac(char *nif_name, u_char *macaddr)
{
	struct ifreq ifr_ip;
	
	memset(&ifr_ip, 0, sizeof(ifr_ip));
	
	strcpy(ifr_ip.ifr_name, nif_name);
	
	if(ioctl(nif_socket_fd, SIOCGIFHWADDR, &ifr_ip) < 0 ) {
	        perror("ioctl : SIOCGIFHWADDR");
	        return -1;
	}

	memcpy(macaddr, ifr_ip.ifr_hwaddr.sa_data, 6);

	printf("%s : %x:%x:%x:%x:%x:%x \n", nif_name, macaddr[0], macaddr[1], macaddr[2], macaddr[3], macaddr[4], macaddr[5]);

	return 0;
}

int get_nif_ipaddr(char *nif_name, u_int *ipaddr)
{

	struct ifreq ifr_ip;
	struct sockaddr_in *sin;
	
	memset(&ifr_ip, 0, sizeof(ifr_ip));
	
	strcpy(ifr_ip.ifr_name, nif_name);
	printf("nif_socket_fd= %d\n", nif_socket_fd);
	if(ioctl(nif_socket_fd, SIOCGIFADDR, &ifr_ip) < 0) {
	        perror("ioctl : SIOCGIFADDR");
	        return -1;
	}

	sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;

	*ipaddr = ntohl(sin->sin_addr.s_addr);

	printf("get_nif_ipaddr : %#x \n", *ipaddr);

	return 0;
}

int get_nif_netmask(char *nif_name, u_int *netmask)
{

	struct ifreq ifr_ip;
	struct sockaddr_in *sin;
	
	memset(&ifr_ip, 0, sizeof(ifr_ip));
	
	strcpy(ifr_ip.ifr_name, nif_name);
	
	if(ioctl(nif_socket_fd, SIOCGIFNETMASK, &ifr_ip) < 0) {
	        perror("ioctl : SIOCGIFNETMASK");
	        return -1;
	}

	sin = (struct sockaddr_in *)&ifr_ip.ifr_netmask;

	*netmask = ntohl(sin->sin_addr.s_addr);

	printf("get_nif_netmask : %#x \n", *netmask);
	
	return 0;
}


int set_multicast_ttl(unsigned char ttl)
{
	if( setsockopt(nif_socket_fd,IPPROTO_IP,IP_MULTICAST_TTL,&ttl,sizeof(ttl)) < 0) {
		perror("setsockopt():IP_MULTICAST_TTL");
		return -1;
	}
}

int set_multicast_loop(int loop)
{
	if(setsockopt(nif_socket_fd,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop)) < 0) {
		perror("setsockopt():IP_MULTICAST_LOOP");
		return -1;
	}
}

int imr_add_membership(u_int nif_addr, char *nif_name, u_int multi_addr)
{

	  struct ip_mreqn mreqn; /* 多播组*/


	 mreqn.imr_multiaddr.s_addr = htonl(multi_addr); /*多播地址*/
	 mreqn.imr_address.s_addr = htonl(nif_addr);	/*网络接口地址*/
	 mreqn.imr_ifindex = if_nametoindex(nif_name);

	 /*将本机加入多播组*/
	 if (setsockopt(nif_socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreqn, sizeof(mreqn)) < 0){
		 perror("setsockopt():IP_ADD_MEMBERSHIP");
		 return -1;
	 }

	 
	 return 0;
}


int imr_drop_membership(u_int nif_addr, char *nif_name, u_int multi_addr)
{

	struct ip_mreqn mreqn; /*多播组*/


	mreqn.imr_multiaddr.s_addr = htonl(multi_addr); /*多播地址*/
	mreqn.imr_address.s_addr = htonl(nif_addr);	/*网络接口地址*/
	mreqn.imr_ifindex = if_nametoindex(nif_name);

	 /*退出多播组*/
	 if (setsockopt(nif_socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreqn, sizeof(mreqn)) < 0){
		 perror("setsockopt():IP_DROP_MEMBERSHIP");
		 return -1;
	 }
	 
	 return 0;
}

int imr_unblock_src(u_int nif_addr,  u_int multi_addr, u_int src_addr)
{

	struct ip_mreq_source ims; 
   
	ims.imr_multiaddr.s_addr  =  htonl(multi_addr); /*多播地址*/;
	ims.imr_sourceaddr.s_addr = htonl(src_addr);
	ims.imr_interface.s_addr  = htonl(nif_addr);	/*网络接口地址*/

	  /*为多播组移除要阻塞的源ip 地址*/
	 if (setsockopt(nif_socket_fd, IPPROTO_IP, IP_UNBLOCK_SOURCE,&ims, sizeof(ims)) < 0){
		 perror("setsockopt():IP_UNBLOCK_SOURCE");
		 return -1;
	 }

	 
	 return 0;
}

int imr_block_src(u_int nif_addr,  u_int multi_addr, u_int src_addr)
{

	struct ip_mreq_source ims; 
   
	ims.imr_multiaddr.s_addr  =  htonl(multi_addr); /*多播地址*/;
	ims.imr_sourceaddr.s_addr = htonl(src_addr);
	ims.imr_interface.s_addr  = htonl(nif_addr);	/*网络接口地址*/

	 /*为多播组添加要阻塞的源ip 地址*/
	 if (setsockopt(nif_socket_fd, IPPROTO_IP, IP_BLOCK_SOURCE,&ims, sizeof(ims)) < 0){
		 perror("setsockopt():IP_BLOCK_SOURCE");
		 return -1;
	 }

	 
	 return 0;
}

int imr_add_src_membership(u_int nif_addr,  u_int multi_addr, u_int src_addr)
{

	struct ip_mreq_source ims; 
   
	ims.imr_multiaddr.s_addr  =  htonl(multi_addr); /*多播地址*/;
	ims.imr_sourceaddr.s_addr = htonl(src_addr);
	ims.imr_interface.s_addr  = htonl(nif_addr);	/*网络接口地址*/

	 /*为多播组添加源ip 地址*/
	 if (setsockopt(nif_socket_fd, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP,&ims, sizeof(ims)) < 0){
		 perror("setsockopt():IP_ADD_SOURCE_MEMBERSHIP");
		 return -1;
	 }

	 
	 return 0;
}

int imr_drop_src_membership(u_int nif_addr,  u_int multi_addr, u_int src_addr)
{

	struct ip_mreq_source ims; 
   
	ims.imr_multiaddr.s_addr  =  htonl(multi_addr); /*多播地址*/;
	ims.imr_sourceaddr.s_addr = htonl(src_addr);
	ims.imr_interface.s_addr  = htonl(nif_addr);	/*网络接口地址*/

	 /*为多播组移除源ip 地址*/
	 if (setsockopt(nif_socket_fd, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP,&ims, sizeof(ims)) < 0){
		 perror("setsockopt():IP_DROP_SOURCE_MEMBERSHIP");
		 return -1;
	 }

	 
	 return 0;
}

int set_nif_igmp_version(char *nif_name, int version)
{
	char pathbuf[128];
	sprintf(pathbuf, "echo %d > /proc/sys/net/ipv4/conf/%s/force_igmp_version", version, nif_name);
	return system(pathbuf);
}

int get_hostip(u_int *hostip, char *hostname)
{
	struct hostent *host;
	u_char       *pch1, *pch2;
	
	host=gethostbyname(hostname);
	if (host == NULL) {
		herror(hostname);
		return -1;
	}
	if (host->h_length != 4) {
		/* IPv4 only, until I get a chance to test IPv6 */
		fprintf(stderr,"oops %d\n",host->h_length);
		return -1;
	}

	pch1 = (u_char *) hostip;
	pch2 = (u_char *) host->h_addr_list[0];

	pch1[0] = pch2[3];
	pch1[1] = pch2[2];
	pch1[2] = pch2[1];
	pch1[3] = pch2[0];
	

	return 0;
}

int add_ip_route(iproute_st *route)
{
	if(route == NULL){
		return -1;
	}

	switch(route->type){
		case IPROUTE_DEFAULT_GW:
			set_default_gw(route->next_dev.gateway);
			break;
		case IPROUTE_STATIC_GW:
			route_add_gw(route->destip, route->netmask, route->next_dev.gateway);
			break;
		case IPROUTE_STATIC_NIF:
			route_add_dev(route->destip, route->netmask, route->next_dev.nif_name);
			break;
		default :
			return -1;
		
	}

	return 0;
}


int del_ip_route(iproute_st *route)
{
	if(route == NULL){
		return -1;
	}

	switch(route->type){
		case IPROUTE_DEFAULT_GW:
			del_default_gw(route->next_dev.gateway);
			break;
		case IPROUTE_STATIC_GW:
			route_del_gw(route->destip, route->netmask, route->next_dev.gateway);
			break;
		case IPROUTE_STATIC_NIF:
			route_del_dev(route->destip, route->netmask, route->next_dev.nif_name);
			break;
		default :
			return -1;
	}

	return 0;
}


void init_timeout(timeout_st *p_timeout, int time_ms)
{
	
	pthread_condattr_t condattr;

	pthread_mutex_init(&(p_timeout->mutex), NULL);
	
	if (pthread_condattr_init(&condattr) != 0) {
		exit(1);
	}
	if (pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC) != 0) {
		exit(1);
	}
	if (pthread_cond_init(&(p_timeout->cond), &condattr) != 0) {
		exit(1);
	}
	
	p_timeout->time_ms = time_ms;
}


void break_timeout(timeout_st *p_timeout)
{


//	pthread_mutex_lock(&p_timeout->mutex);

	pthread_cond_signal(&p_timeout->cond);

//	pthread_mutex_unlock(&p_timeout->mutex);

}

void update_timeout(timeout_st *p_timeout, int time_ms)
{
	p_timeout->time_ms = time_ms;
	printf("update period_task timeout : %d ms \n", time_ms);
	break_timeout(p_timeout);
}

int wait_timeout(timeout_st *p_timeout)
{
	int status;
	int sec;
	int nsec;
	int waitms;
	static struct timespec tmspec;


	if(p_timeout == NULL){
		return -1;
	}

	waitms = p_timeout->time_ms;

	clock_gettime(CLOCK_MONOTONIC, &tmspec);

	//printf("GetKeyValue : tv_sec = %d\n", tmspec.tv_sec);
	
	sec = waitms / 1000;
	//*
	waitms = waitms % 1000;
	
	nsec =  waitms * 1000000 + tmspec.tv_nsec;

	tmspec.tv_nsec = nsec % 1000000000;
	tmspec.tv_sec += nsec / 1000000000;
	//*/
	tmspec.tv_sec += sec;

	
	pthread_mutex_lock(&p_timeout->mutex);

	while (1) {

		//*
		status = pthread_cond_timedwait(&p_timeout->cond, &p_timeout->mutex, &tmspec);
		//printf( "get key status = %x----\n", status);
		if (status != 0){
			//printf( "get key wait cond failure\n");
			if(status == ETIMEDOUT){
				status = 9;
				break;
			}

		}else{
			break;
		}
		//*/

	}

	pthread_mutex_unlock(&p_timeout->mutex);

	return status;
}

period_task_st *create_period_task(void (*handler)(void *), void *arg, int time_ms)
{
	period_task_st  *task;

	task = xMalloc(sizeof(period_task_st));
	task->handler = handler;
	task->arg = arg;
	init_timeout(&task->timeout, time_ms);
	task->run_flag = STOP_FLAG;

	return task;
}

int start_period_task(period_task_st  *task)
{
	int status;
	int value;

	task->run_flag = RUNNING_FLAG;
	TagPrintf(task->timeout.time_ms);
	while(task->run_flag){
		
		while(task->run_flag == PAUSE_FLAG){
			usleep(1000);
		}
		//TagPrintf(0);
		
		status = wait_timeout(&task->timeout);
		//TagPrintf(status);
		if (status < 0){
			usleep(10000);
			continue;
		}else if(status == 0){
			
		}else if(status == 9){
			task->handler(task->arg);
		}
	
	}
	
	TagPrintf(task->run_flag);
	
}

typedef struct{
	void (*handler)(void *);
	void *arg;
	timeout_st timeout;
} waitTask_st;

static int wait_run_task(waitTask_st *p_task)
{
	int status;
	
	status = wait_timeout(&p_task->timeout);
	if(status == 9){
		p_task->handler(p_task->arg);
		status = 0;
	}else{
		status = -1;
	}

	free(p_task);
	return status;
}


int delay_run_task(void (*handler)(void *), void *arg, int time_ms)
{
	int status;
	timeout_st timeout;
	
	init_timeout(&timeout, time_ms);
	
	status = wait_timeout(&timeout);
	if(status == 9){
		handler(arg);
		return 0;
	}

	return -1;
}


 void sync_delay_run(void (*handler)(void *), void *arg, int time_ms)
{
	
	waitTask_st *ptask = xMalloc(sizeof(waitTask_st));
	
	init_timeout(&ptask->timeout, time_ms);
	ptask->arg = arg;
	ptask->handler = handler;

	create_detached_pthread(wait_run_task,  ptask, SZ_1M);
	
}


void signal_procmask(int how, ...)
{
	int signo;
	sigset_t sigset, osigset;
	va_list ap;
	
	sigemptyset(&sigset);

	va_start(ap, how);

	do{
		signo = va_arg(ap, int);
		if(signo <= 0){
			break;
		}
		
		sigaddset(&sigset, signo);
		
	}while(signo);

	va_end(ap); 
	
	sigprocmask(how, &sigset, &osigset);
}

int FileRecord_GetLock(int fd, struct flock *p_lock)
{
	return fcntl(fd, F_GETLK, p_lock);
}

int FileRecord_SetLock(int fd, int type, int cmd, int whence, int start, int len)
{
	struct flock lock;
	
	lock.l_type = type;
	lock.l_whence = whence;
	lock.l_start = start;
	lock.l_len = len;

	return fcntl(fd, cmd, &lock);
}



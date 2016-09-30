
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <signal.h>
#include <linux/watchdog.h>

#include <errno.h>

#include "fpga.h"
#include "fpga_reg.h"
#include "ipcmsg/shmem_obj.h"

#include "system.h"

#define WDT_DEVICE			"/dev/misc/watchdog"


#define WEBSERVER_APP	"httpd"

void reset_all_slot_boards(void)
{
	FPGA_ARMIF_WRITE(REG_FPGA_SLOT_RESET_CONTROL, 0);
	usleep(200000);
	FPGA_ARMIF_WRITE(REG_FPGA_SLOT_RESET_CONTROL, 0xffff);
	
	printf("reset all slot boards\n");
}


static int run_flag;
static pid_t webserver_pid;
sysinfo_st *sysinfo;

static void sigHandler(int signo){
	printf("watchdog : %d signal is raise \n", signo);
	sysinfo->run_flag = 0;
	run_flag = 0;
	signal(SIGCHLD ,SIG_DFL);
	kill(sysinfo->webserver_pid, SIGKILL);
}


#define GLOBAL_SHM_SIZE	0x100000

#define GLOBAL_SHM_NAME		"globle_shm"
#define GLOBAL_SHM_KEY		999

shmem_obj_t *global_shm;

/*
	OMAP Watchdog Timer Rev 0x01: initial timeout 60 sec
	software timeout 50s
*/

#define WDT_HW_TIMEOUT	60
#define WDT_SW_TIMEOUT	40

static int wdt_hw_timeout = WDT_HW_TIMEOUT;
static int wdt_sw_timeout = WDT_SW_TIMEOUT;

typedef struct {
	int enable;
	int timeout;	
} watchdog_st;

watchdog_st *wdt;
	
#define WATCHDOG_GLOBLE_SHM_OFFSET		0
#define WATCHDOG_STATUS_GLOBLE_SHM_SPAN			8

void open_global_shm(void)
{
	char *shm_base;
	
	global_shm = shmem_init(GLOBAL_SHM_KEY, GLOBAL_SHM_SIZE, SHM_SYSTEMV_IF);
	shm_base = global_shm;
	
	wdt = (watchdog_st *)(global_shm->data_buf + WATCHDOG_GLOBLE_SHM_OFFSET);
	sysinfo = (sysinfo_st *)(shm_base + SYSINFO_GLOBLE_SHM_OFFSET);

}

void close_global_shm(void)
{
	wdt = NULL;
	shmem_destroy(global_shm);
	
}


static int watchdog_fd = -1;
	
int open_watchdog(void)
{

	watchdog_fd = open(WDT_DEVICE, O_WRONLY);
	if(watchdog_fd == -1) {  
		printf("\n!!! FAILED to open "WDT_DEVICE", errno: %d, %s\n", errno, strerror(errno));  
	} 

	printf("open watchdog\n");
	
	return watchdog_fd;
}

void close_watchdog(void)
{
	if(watchdog_fd<0){
		return;
	}

	if(watchdog_fd>0){
		close(watchdog_fd);
	}
	
	printf("close watchdog\n");
	
	watchdog_fd = -1;
}

/*
如果设备的超时值的粒度只能到分钟，则这个例子可能实际打印"The timeout was set to 60 seconds"。
*/
void set_wdt_timeout(int sec)
{
	int timeout = sec;

	ioctl(watchdog_fd, WDIOC_SETTIMEOUT, &timeout);

	printf("The timeout was set to %d seconds\n", timeout);

	wdt_hw_timeout = timeout;
}

void feed_dog(void)
{
	if(write(watchdog_fd, "\0", 1) != 1) {  
		puts("\n!!! FAILED feeding watchdog");  
	}  
}



int start_webserver(void)
{
	int ret;
	pid_t pid;

	printf("create and start web server process \n");
	
	pid = fork();
	 if (pid < 0) {
		perror("fork failed");
	 	return -1;
	 }else if (pid == 0){
		execl(WEBSERVER_APP, NULL);
		exit(0);
	 }else{
		//setpgid(pid,pid);
	 	webserver_pid = pid;
		printf("pork web server process success , pid : %d \n", pid);
				
	 }
}


static void SIGCHLD_Handler(int signo)
{
	int ret, status;
	
	//printf("%d signal is raised \n", signo);

	ret = wait(&status);

	printf("progress [%d] end and wait status %d , %s exit %d \n", ret, status
		, WIFEXITED(status)?"normal":"abnormal", WEXITSTATUS(status));

	if((sysinfo->run_flag > 4) && (ret == webserver_pid)){
		start_webserver();
	}
	 
}


int main(int argc, char *argv[])
{

	int argind;	u_int v_u32;	char *argp;
	
	struct sigaction act, oldact;
   	act.sa_flags = 0;
	 
	act.sa_handler = sigHandler;
  	sigaction(SIGTERM, &act, &oldact);

	act.sa_handler = sigHandler;
  	sigaction(SIGKILL, &act, &oldact);

	
	act.sa_handler = SIGCHLD_Handler;
  	sigaction(SIGCHLD, &act, &oldact);
	
	//signal(SIGTERM, sigHandler);
	//signal(SIGKILL, sigHandler);
	
	
	open_global_shm();

	run_flag = 1;
	sysinfo->run_flag = 1;

	for (argind = 1; argind < argc; argind++) {
		argp = argv[argind];
		if (*argp != '-') {
			break;
		}
		if ( !strcmp(argp, "-st")) {
			wdt_sw_timeout = atoi(argv[++argind]);
		}else if ( !strcmp(argp, "-ht") ) {
			wdt_hw_timeout = atoi(argv[++argind]);
		}
	}
	

	start_webserver();

	
	while(run_flag){
		if(wdt->enable){
			if(watchdog_fd<0){
				wdt->timeout == 0;
				if(open_watchdog()<0){
					wdt->enable = 0;
					watchdog_fd = 0;
					printf("watchdog timeout %d second\n", wdt_sw_timeout);
					continue;
				}

				set_wdt_timeout(wdt_hw_timeout);
				printf("watchdog timeout %d:%d second\n", wdt_sw_timeout, wdt_hw_timeout);
			
			}else{
				if(wdt->timeout == 0){
					feed_dog();
				}else if(wdt->timeout > wdt_sw_timeout){
					printf("watchdog timeout, reboot ! \n");
					sysinfo->run_flag = 0;
					run_flag = 0;
					system("reboot");
					break;
				}
				wdt->timeout++;
			}
		}else{
			close_watchdog();
		}
		
		sleep(1);
	}

	printf("wdg : %d, %d \n", run_flag, sysinfo->run_flag);
	
	if(open_fpga()>0){
		reset_all_slot_boards();
		close_fpga();
	}

	close_global_shm();
	close_watchdog();
	
	return 0;
}



//#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "lm73.h"



#define LM73_ALARM0_GPIO	(2*32+25)	//gpio2_25	temperature sensor  alarm 1 near by arm
#define LM73_ALARM1_GPIO	(0*32+29)	//gpio0_29	temperature sensor alarm 2 near by fpga



#define LM73_SMBUS_ADDR		0x49	// 0x4a



#define LM73_IDENTIFICATION	0x0190

#define Alert_Pin_Status_Bit			(1<<3)
#define Temperature_High_Flag_Bit	(1<<2)
#define Temperature_Low_Flag_Bit	(1<<1)
#define Data_Available_Flag_Bit		(1<<0)


//temprature unit 0.25 deg c, is a signed value

#define LM73_READ_TEMPERATURE		0x3101	/* read LM73 temperature */
#define LM73_READ_T_HIGH		0x3102	/* read LM73 temperature upper-limit */
#define LM73_READ_T_LOW		0x3103	/* read LM73 temperature lower-limit */
#define LM73_READ_CONFIGURATION		0x3104	/* read LM73 configuration */
#define LM73_READ_CTRL_STATUS		0x3105	/* read LM73 control/status */
#define LM73_READ_IDENTIFICATION		0x3106	/* read LM73 identification */

#define LM73_WRITE_T_HIGH		0x3112	/* write LM73 temperature upper-limit */
#define LM73_WRITE_T_LOW		0x3113	/* write LM73 temperature lower-limit */
#define LM73_WRITE_CONFIGURATION		0x3114	/* write LM73 configuration */
#define LM73_WRITE_CTRL_STATUS		0x3115	/* write LM73 control/status */

#define LM73_SET_MODE		0x3121
#define LM73_ALERT_ENABLE		0x3122
#define LM73_ALERT_POLARITY		0x3123
#define LM73_ALERT_RESET		0x3124
#define LM73_SET_RESOLUTION		0x3125
#define LM73_TIMEOUT_DISABLE		0x3126


static char *lm73_name[2] = {"/dev/misc/lm73_0", "/dev/misc/lm73_1"};
static int lm73_fd[2] = {0,0};
static void (*lm73_signal_handler)(int , int ) = NULL;

void lm73_register_signal_handler(void (*sig_handler)(int , int ))
{
	lm73_signal_handler = sig_handler;
}

static void lm73_alert_handler(int sig)
{
	int status;
	printf("lm73_alert_handler : %d \n", sig);

	sig -= 34;

	if(read(lm73_fd[sig], &status, 4)>0){
		printf("lm73[%d] status : %#x \n", sig, status);
	}

	if(lm73_signal_handler){
		lm73_signal_handler (sig, status);
	}
	
	
}

int open_lm73(void)
{
	int i=0, k; 
	int ret;
	
	for(i=0; i<2; i++){
		
		lm73_fd[i] = open(lm73_name[i], O_RDWR);

		if(lm73_fd[i] < 0){
			printf("***Can't open the device %s  driver handle!\r\n", lm73_name[i]);
			for(k=0; k<i; k++){
				close(lm73_fd[k]);
			}
			return -1;
		}

		signal(34+i, lm73_alert_handler);

		if ((ret = fcntl(lm73_fd[i], F_SETOWN, getpid())) < 0)
			printf("lm73_%d fcntl F_SETOWN faild\n", i);

		

	}

	//SIGRTMIN 34, SIGRTMAX 64
	//printf("SIGRTMIN %d , SIGRTMAX %d \n", SIGRTMIN, SIGRTMAX);
	
	
	return 0;
}

void close_lm73(void)
{
	int i;
	
	for(i=0; i<2; i++){
		close(lm73_fd[i]);
	}
}


int lm73_set_mode(int idx, int mode)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_SET_MODE, &mode);
		
	return ret;
}

int lm73_alert_enable(int idx, int enable)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_ALERT_ENABLE, &enable);
	
	return ret;
}

int lm73_alert_polarity(int idx, int polarity)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_ALERT_POLARITY, &polarity);
	
	return ret;
}

int lm73_alert_reset(int idx)
{

	int ret;

	ret =ioctl(lm73_fd[idx], LM73_ALERT_RESET, NULL);
	
	return ret;
}

int lm73_set_resolution(int idx, int resolution)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_SET_RESOLUTION, &resolution);
	
	
	return ret;
}

int lm73_TimeOut_Disable(int idx, int Disable)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_TIMEOUT_DISABLE, &Disable);
	
	
	return ret;
}


int lm73_read_temprature(int idx)
{

	int ret, value;
	
	ret =ioctl(lm73_fd[idx], LM73_READ_TEMPERATURE, &value);
	if(ret<0){
		return -0x10000;
	}
	
	return value;
}


int lm73_read_t_high(int idx)
{

	int ret, value;
	
	ret =ioctl(lm73_fd[idx], LM73_READ_T_HIGH, &value);
	if(ret<0){
		return -0x10000;
	}
	
	return value;
}


int lm73_read_t_low(int idx)
{

	int ret, value;
	
	ret =ioctl(lm73_fd[idx], LM73_READ_T_LOW, &value);
	if(ret<0){
		return -0x10000;
	}
	
	return value;
}


int lm73_read_configuration(int idx)
{

	int ret, value;
	
	ret =ioctl(lm73_fd[idx], LM73_READ_CONFIGURATION, &value);
	if(ret<0){
		return ret;
	}
	
	return value;
}


int lm73_read_status(int idx)
{

	int ret, value;
	
	ret =ioctl(lm73_fd[idx], LM73_READ_CTRL_STATUS, &value);
	if(ret<0){
		return ret;
	}
	
	return value;
}

int lm73_set_t_high(int idx, int t_high)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_WRITE_T_HIGH, &t_high);
	
	
	return ret;
}

int lm73_set_t_low(int idx, int t_low)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_WRITE_T_LOW, &t_low);
	
	
	return ret;
}

int lm73_set_configuration(int idx, int configuration)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_WRITE_CONFIGURATION, &configuration);
	
	
	return ret;
}

int lm73_set_control(int idx, int control)
{

	int ret;
	
	ret =ioctl(lm73_fd[idx], LM73_WRITE_CTRL_STATUS, &control);
	
	
	return ret;
}


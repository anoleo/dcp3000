
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include "front_panel.h"


static int front_panel_fd;

key_response_t key_response;

char *Wait_Dot_Str = ".........................";

//idx range 1 - 8
void Extend_Led_Set(int idx, int color)
{
	int data;
	static int led_idx = 0, led_status = 0;

	//printf("Extend_Led_Set : idx = %d, color = %d \n", idx, color);

	if((led_idx == idx) && (led_status == color)){
		return;
	}
	
	led_status = color;
	led_idx = idx;

	data = color | (idx<<8);
	
	ioctl(front_panel_fd, EXTENDLEDDISPLY, &data);	
}

void Led_Alarm_Set(int color)
{
	int data;
	static int alarm_led_status = 0;

	//printf("Led_Alarm_Set : color = %d \n", color);

	if(alarm_led_status == color){
		return;
	}
	alarm_led_status = color;

	data = color | (2<<8);

	ioctl(front_panel_fd, LEDDISPLY, &data);	
	
}

void Led_Warning_Set(int color)
{
	int data;
	static int warning_led_status = 0;

	//printf("Led_Warning_Set : color = %d \n", color);
	
	if(warning_led_status == color){
		return;
	}
	warning_led_status = color;

	data = color | (1<<8);

	ioctl(front_panel_fd, LEDDISPLY, &data);
}


void keyhandler(int signum)
{
	int status;
	int keyvalue;
	static char ret[2];

	read(front_panel_fd, ret, 1);
	if (ret[0] != NO_KEY) {
		printf("key = %d \n", ret[0]);

		keyvalue = ret[0];

		//mxprintf(DEBUG_INGO1, "into keyhandler\n");

		status = pthread_mutex_lock(&key_response.mutex);
		if (status != 0)
			printf( "keyhandler lock mutex failure\n");

		//mxprintf(DEBUG_INFO2, "keyhandler, lock mutex\n");

		key_response.key_value = keyvalue;

		//mxprintf(DEBUG_INFO2, "key press, value[%d]\n", key_response.key_value);

		//key_response.key_flag = 1;

		status = pthread_cond_signal(&key_response.cond);
		if (status != 0)
			printf( "keyhandler unlock mutex failure\n");

		//mxprintf(DEBUG_INFO2, "keyhandler, signal capture cond, key_response.key_flag %d\n", key_response.key_flag);

		status = pthread_mutex_unlock(&key_response.mutex);
		if (status != 0)
			printf( "keyhandler unlock mutex failure\n");

		//mxprintf(DEBUG_INFO1, "exit keyhandler\n");
	}
	
	signal(SIGIO, keyhandler);
	
    return;
}


void timeouthandle(int signum)
{
	int status;
	int result;

	//mxprintf(DEBUG_INFO1, "into timeouthandle\n");

	status = pthread_mutex_lock(&key_response.mutex);
	if (status != 0)
		printf( "timeouthandle lock mutex failue\n");

	//mxprintf(DEBUG_INFO2, "timeouthandle, lock mutex\n");

	key_response.key_value = KEY_TIMEOUT;

	key_response.key_flag = 1;

	status = pthread_cond_signal(&key_response.cond);
	if (status != 0)
		printf( "timeouthandle signal capture cond failure\n");

	//mxprintf(DEBUG_INFO2, "timeouthandle, signal capture cond, key_response.key_flag %d\n", key_response.key_flag);

	status = pthread_mutex_unlock(&key_response.mutex);
	if (status != 0)
		printf( "timeouthandle unlock mutex failure\n");

	//mxprintf(DEBUG_INFO1, "exit timeouthandle\n");

	//signal(SIGALRM, timeouthandle);
}


int GetKeyValue(int waitms)
{
	int status;
	int value;
	int sec;
	int nsec;
	static struct timespec tmspec;

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

	
	status = pthread_mutex_lock(&key_response.mutex);
	if (status != 0)
		printf( "get lock mutex failure\n");

	//mxprintf(DEBUG_INFO2, "get key mutex\n");

	//key_response.key_flag = 0;

	//alarm(2);/*timeout second*/
	//alarm(timeout);

	while ((key_response.key_flag == 0)) {
		//mxprintf(DEBUG_INFO2, "wait key\n");

		//*
		
		status = pthread_cond_timedwait(&key_response.cond, &key_response.mutex, &tmspec);
		//printf( "get key status = %x----\n", status);
		if (status != 0){
			//printf( "get key wait cond failure\n");
			if(status == ETIMEDOUT){
				pthread_mutex_unlock(&key_response.mutex);
				return KEY_TIMEOUT;
			}
			//return NO_KEY;
		}else{
			break;
		}
		//*/

		
	}

	//mxprintf(DEBUG_INFO2, "key wakeup\n");
	//alarm(0);
	
	value = key_response.key_value;
	key_response.key_value = NO_KEY;

	if(value != NO_KEY){
		key_response.key_flag = 0;
	}
	//printf("get key will exit lock, key_value %X\n", value);

	status = pthread_mutex_unlock(&key_response.mutex);
	if (status != 0)
		printf( "get key unlock mutex failure\n");

	//mxprintf(DEBUG_INFO2, "get key exit lock\n");

		
	
	return value;
}


/* display string lcd */
void LcdDisplay(int line, const char *s)
{
	u_char buf[25];
	int len;

	len = strlen(s);
	if(len>20)
	{
		memcpy(buf, s,20);
		buf[20] = 0x00;
	}
	else
		sprintf(buf, "%s", s);

	if (line == 1)
		ioctl(front_panel_fd, DISPLAYSTRING1, buf);
	else if (line == 2)
		ioctl(front_panel_fd, DISPLAYSTRING2, buf);
}

void lcd_brightness(int brightness)
{

	brightness |= 0x38;
	ioctl(front_panel_fd, LCDWRINST, &brightness);
	//printf("brightness = 0x%02X\n",brightness);
}

void LcdFlashCursor(int line, int location)
{
	int i;
	i = location;
	if (line == 1)
		ioctl(front_panel_fd, DISPPAYCURSOR1, &i);
	else if (line == 2)
		ioctl(front_panel_fd, DISPPAYCURSOR2, &i);
}

int slot_led[MAX_LED_QTY] = {
 	SLOT0_LED,  SLOT1_LED, SLOT2_LED, SLOT3_LED,
	SLOT4_LED, SLOT5_LED, MUX_A_LED, CAS_B_LED
};

void open_front_panel(void)
{
		
	front_panel_fd = open("/dev/misc/front_panel", O_RDWR);
	if (front_panel_fd < 0)
		printf("can't open /dev/misc/front_panel device\n");

}

void close_front_panel(void)
{	
	close(front_panel_fd);
}

void front_panel_init(void)
{
	int temp, val;
	int i;
	
	pthread_condattr_t condattr;

	printf("init  front_panel\n");

	open_front_panel();

	signal(SIGIO, keyhandler);

	if ((val = fcntl(front_panel_fd, F_SETOWN, getpid())) < 0)
			printf("mxkey fcntl F_SETOWN");
	temp = fcntl(front_panel_fd, F_GETFL);
	if ((val = fcntl(front_panel_fd, F_SETFL, temp | FASYNC)) < 0)
			printf( "mxkey fcntl F_SETFL");

	pthread_mutex_init(&(key_response.mutex), NULL);

	if (pthread_condattr_init(&condattr) != 0) {
		exit(1);
	}
	if (pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC) != 0) {
		exit(1);
	}
	if (pthread_cond_init(&(key_response.cond), &condattr) != 0) {
		exit(1);
	}

	key_response.key_flag = 0;


	for(i=1; i<=MAX_LED_QTY; i++){
		Extend_Led_Set(i, 0);
	}
	
	printf("front panel init succeed \n");
}


void start_front_panel(void *(*handler)(void *))
{
	int status;
	pthread_t thread_lcd;
	pthread_attr_t attr;
	size_t size;

	size = 0x100000;   // 1M

	front_panel_init();

	status = pthread_attr_init(&attr);
	if (status != 0)
		printf( "pthread_attr_init failure\n");

	status = pthread_attr_setstacksize(&attr,size);
	if (status != 0)
		printf( "pthread_attr_setstacksize failure\n");
	
	status = pthread_create(&thread_lcd, &attr, handler, NULL);
	if (status != 0)
		printf( "Create thread_lcd failure\n");
	
}


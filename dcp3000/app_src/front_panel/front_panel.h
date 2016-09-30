#ifndef _FRONT_PANEL_DRIVER_H_
#define _FRONT_PANEL_DRIVER_H_

/*driver  ioctl control */
#define DISPLAYSTRING1   0x6010
#define DISPLAYSTRING2   0x6011
#define DISPLAYBYTE1     0x6012
#define DISPLAYBYTE2     0x6013
#define DISPLAYCLEAR     0x6014
#define DISPPAYCURSOR1   0x6015
#define DISPPAYCURSOR2   0x6016
#define DISPLYINIT       0x6017
#define LEDDISPLY        0x6018
#define EXTENDLEDDISPLY     0x6019
#define LCDBACKLIGHT    0x601a
#define LCDWRINST       0x601b

#define LCDWR4094		0x60ff

#define LED_OFF			0
#define LED_RED			1
#define LED_GREEN		2
#define LED_ORANGE		3

#define SLOT0_LED	5
#define SLOT1_LED	6
#define SLOT2_LED	7
#define SLOT3_LED	1
#define SLOT4_LED	2
#define SLOT5_LED	3
#define MUX_A_LED	8
#define CAS_B_LED	4

#define MAX_LED_QTY	8

extern int slot_led[MAX_LED_QTY];


/*define key value*/
#define NO_KEY    0
#define EXIT_KEY  6
#define ENTER_KEY 5
#define RIGHT_KEY 4
#define DOWN_KEY  3
#define UP_KEY    2
#define LEFT_KEY  1

#define KEY_TIMEOUT 0xff

#define BACKBEFORE 0xba
#define SAVERESULT 0x990


#define FIRST_LINE 1
#define SECOND_LINE 2

#define TIME_OUT_COUNT 120000

#define TIME_OUT_RETURN_FLAG 0xFFFF
#define EXIT_KEY_PRESS_FLAG 0xFFFB


extern char *Wait_Dot_Str;
#define SPRINT_WAIT_STR(p_str, p_prefix, n) \
	do{ \
		strcpy(p_str, p_prefix); \
		strncat(p_str, Wait_Dot_Str, n); \
	}while(0)




/*define 5000tm key value sync structure*/
typedef struct _key_response_t {
	pthread_mutex_t mutex;    /*mutex of condition variable wait for*/
	pthread_cond_t cond;      /*when thread idle, thread wait for this*/
	int key_value;            /*key value define: 1 = exit; 2 = enter; 3 = right; 4 = down; 5 = up; 6 = left*/
	int key_flag;             /*when thread idle, wait for change*/
} key_response_t;

extern key_response_t key_response;

void Led_Alarm_Set(int color);
void Led_Warning_Set(int color);

void Extend_Led_Set(int idx, int color);

int GetKeyValue(int timeout);
void keyhandler(int keyvalue);

void timeouthandle(int signum);

void LcdDisplay(int line, const char *s);
void LcdFlashCursor(int line, int location);
void lcd_brightness(int brightness);

void open_front_panel(void);
void close_front_panel(void);
void front_panel_init(void);
void start_front_panel(void *(*handler)(void *));

void *Lcd(void *arg);

#endif


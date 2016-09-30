#ifndef __LM73_H__
#define __LM73_H__

#include <sys/types.h>

/*
Temperature Accuracy
-10	to 80	¡À1.0 deg.C (max)
-25	to 115	¡À1.5 deg.C (max)
-40	to 150	¡À2.0 deg.C (max)
//*/

#define LM73_NORMAL_MODE	0
#define LM73_SHUTDOWN_MODE	1

#define LM73_CTRL_11bits	0	// resolution	0.25 deg C/LSB
#define LM73_CTRL_12bits	1	// resolution	0.125 deg C/LSB
#define LM73_CTRL_13bits	2	// resolution	0.0625 deg C/LSB
#define LM73_CTRL_14bits	3	// resolution	0.03125 deg C/LSB


int open_lm73(void);
void close_lm73(void);

int lm73_read_status(int idx);
int lm73_read_temprature(int idx);
int lm73_read_t_high(int idx);
int lm73_read_t_low(int idx);
int lm73_set_t_high(int idx, int t_high);
int lm73_set_t_low(int idx, int t_low);
int lm73_set_control(int idx, int control);
int lm73_set_configuration(int idx, int configuration);
int lm73_read_configuration(int idx);
int lm73_set_resolution(int idx, int resolution);
int lm73_alert_reset(int idx);
int lm73_alert_polarity(int idx, int polarity);
int lm73_alert_enable(int idx, int enable);
int lm73_set_mode(int idx, int mode);
int lm73_TimeOut_Disable(int idx, int Disable);



void lm73_register_signal_handler(void (*sig_handler)(int , int ));


#endif


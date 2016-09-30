#ifndef NTPCLIENT_H
#define NTPCLIENT_H



/* global tuning parameter */
extern double min_delay;

/* prototype for function defined in phaselock.c */
int contemplate_data(unsigned int absolute, double skew, double errorbar, int freq);

int sync_ntp(unsigned int host, int timezone);

#endif

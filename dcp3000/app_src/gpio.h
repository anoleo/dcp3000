#ifndef __GPIO_H__
#define __GPIO_H__

#include <sys/types.h>


int open_gpio(void);
void close_gpio(void);

void gpio_init(int index, char *name);
void gpio_free(int index);

void gpio_input(int index);
void gpio_output0(int index);
void gpio_output1(int index);


void gpio_clean(int index);
void gpio_set(int index);
int gpio_get(int index);

#endif


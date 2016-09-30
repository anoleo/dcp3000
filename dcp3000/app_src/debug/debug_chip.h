

#ifndef _DEBUG_CHIP_H_
#define _DEBUG_CHIP_H_


int debug_flash(int argc, char *argv[]);
   
int debug_fpga( int argc, char *argv[ ] );
int debug_gpio(int argc, char *argv[]);


int debug_fram(int argc, char *argv[]);
int debug_lm73( int argc, char *argv[ ] );
int debug_mtd(int argc, char *argv[]);

#endif

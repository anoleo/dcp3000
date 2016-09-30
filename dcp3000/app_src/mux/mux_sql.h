
#ifndef __MUX_SQL_H__
#define __MUX_SQL_H__

#include "psi_si.h"


void create_remux_table(void);

int select_remux_section(int mux_chan, int type, mux_section_t **pp_mux_section);

int select_remux_programs(int mux_chan);
int select_remux_pid_filter(int mux_chan);

int select_remux_parameters(int mux_chan);

int select_capture_control_params(void);


int save_remux_section(int mux_chan, mux_section_t *head_mux_section);

void save_remux_programs(int mux_chan);
void save_remux_pid_filter(int mux_chan);
void save_remux_parameters(int mux_chan);

void save_capture_control_params(void);
int save_capture_ctrl(void);
int save_channel_capture_ctrl(int idx);


void read_remux_parameters(void);
void read_pid_filter_table(void);
void read_mux_programs(void);

void read_all_remux_data(void);


void save_remux_data(int mux_chan);


#endif


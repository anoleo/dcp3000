#ifndef _WEB_MUX_H_
#define _WEB_MUX_H_


#include "psi_si.h"


mux_program_t *find_mux_program_by_oldpn(int mux_chan, int in_chan, u_short old_pn);
mux_program_t *find_mux_program_by_newpn(int mux_chan, u_short new_pn);
mux_program_t *find_mux_program_by_pid(int mux_chan, u_short pid);

int check_newpn_in_mapping(int mux_chan, u_short new_pn);
u_short find_newpn_in_mapping(int mux_chan, int in_chan, u_short old_pn);
int add_pn_mapping(int mux_chan, int in_chan, u_short old_pn, u_short new_pn);
mux_program_t *update_program_mapping(int mux_chan, int in_chan, u_short old_pn, u_short new_pn);



int check_muxpid_in_mapping(int mux_chan, u_short mux_pid);
u_short find_muxpid_in_mapping(int mux_chan, int in_chan, u_short in_pid);
int update_pid_mapping(int mux_chan, int in_chan, u_short in_pid, u_short mux_pid);


mux_section_t *regen_pat_section(int mux_chan);

mux_section_t *regen_pmt_section(int mux_chan);

mux_section_t *regen_cat_section(int mux_chan);

mux_section_t *regen_sdt_section(int mux_chan);

mux_section_t *regen_nit_section(int mux_chan);

int regen_mux_pid_mapping(int mux_chan);

int prepare_mux_programs(int mux_chan);

void print_all_mux_program(int mux_chan);

void *WebMuxMsgTask(void *arg);

#endif




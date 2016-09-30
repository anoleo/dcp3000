
#include "descriptor.h"
#include "psi_si.h"


u_short get_sub_table_id(u_char *section_data);

program_t *create_program(void);
void free_all_program(program_t *head_program);

pmt_table_t * create_pmt_table(void);
void free_all_pmt_table(pmt_table_t *head_pmt_section);
pmt_table_t *find_ts_pmt_table(ts_in_table_t *p_ts_table, u_short pmt_pid);
program_t *find_ts_in_program(pmt_table_t *head_pmt_section, u_short program_number);


section_t *create_section(void);
void free_all_section(section_t *head_section);
void add_section(section_t **pp_head_section, section_t *p_section);

eit_section_t *create_eit_section(void);
void free_all_eit_section(eit_section_t *head_section);
void add_eit_section(eit_section_t **pp_head_section, eit_section_t *p_section);


eit_table_t *create_eit_table(void);
ts_sub_table_t *create_ts_sub_table(void);
void free_all_ts_sub_table(ts_sub_table_t *head_sub_table);
void free_all_eit_table(eit_table_t *head_eit);



void free_all_eit_sub_table(eit_sub_table_t *head_sub_table);
eit_sub_table_t *create_eit_sub_table(void);
void free_all_eit_segment(eit_segment_t *head_eit_segment);
eit_segment_t *create_eit_segment(void);

void free_ts_eit_table(ts_in_table_t *p_ts_table, u_char table_id, u_short service_id);

void free_all_service_eit_table(ts_in_table_t *p_ts_table);


eit_table_t *find_ts_table_eit(int in_chan, u_short service_id);

void print_all_pmt_table(pmt_table_t *head_pmt_section);


void init_ts_in_table(ts_in_table_t *p_ts_table);
ts_in_table_t *create_ts_in_table(void);
void free_ts_table(ts_in_table_t *p_ts_table);
void print_ts_table(ts_in_table_t *p_ts_table);
void reset_ts_in_table(ts_in_table_t *p_ts_table, int resetpat_flag);
void reset_pat_in_ts_table(ts_in_table_t *p_ts_table);

mux_packet_t * packetData2packets(u_char *p_data, int data_len);
mux_packet_t *sectionData2packets(u_char *p_data, int data_len, u_short pid, int *cc_index);

void free_all_mux_packet(mux_packet_t *head_packet);
mux_packet_t *create_mux_packet();

mux_section_t *create_mux_section(void);
void free_all_mux_section(mux_section_t *head_mux_section);



int parse_tsin_pat(ts_in_table_t *p_ts_table);
int parse_tsin_pmt(ts_in_table_t *p_ts_table);
int parse_tsin_sdt(ts_in_table_t *p_ts_table);
int parse_tsin_cat(ts_in_table_t *p_ts_table);






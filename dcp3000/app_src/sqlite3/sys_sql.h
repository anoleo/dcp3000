
#ifndef __SYSTEM_SQL_H__
#define __SYSTEM_SQL_H__

#include "../transfer/transceiver.h"

void create_system_table(void);


int select_slot_webversion(int slot, int *version);
int update_slot_webversion(int slot, int version);

void delete_transceiver_route(int trans_if, int route_id);
void save_transceiver_route(trans_route_st*p_route);
int select_transceiver_routetable(int trans_if);
void delete_transceiver_routetable(int trans_if);

#endif


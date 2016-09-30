#ifndef ___MY_SHM_MSG_CLIENT_H___
#define ___MY_SHM_MSG_CLIENT_H___

#include "ipcmsg/shmem_msg.h"

char *get_shmmsg_rcvdata(void);

void init_shmmsg_client(void);
void free_shmmsg_client(void);

int send_shmmsg(int msgid, char *data, int len);
int rcv_shmmsg(char *data);

#endif

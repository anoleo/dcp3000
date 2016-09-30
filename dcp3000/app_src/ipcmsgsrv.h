
#ifndef _IPC_MSG_SRV_H_
#define _IPC_MSG_SRV_H_



#define IPCMSG_BUF_SIZE		0x800000

#define MYSELF_SHMEM_PROGRAM_ID 	220


#define IPCMSG_DO_EXEC	0x9


void *ipcmsgsrv_task(void *arg);

#endif



#include "trapmsg.h"
#include "../appref.h"


extern u_char unit_name[50];

key_t msg_key = 20;
int msg_id;

trap_msg_buf_ trap_msg_buf;

void trapCreate(void)
{

	msg_id = msgget(msg_key, IPC_CREAT| IPC_EXCL | 0x666);
	if(msg_id < 0){
		printf("Creat trap Message queue failure, %d\n", msg_id);
		if(errno == EEXIST){
			msg_id = msgget(msg_key, 0);
			if(msg_id < 0){
				printf("Recreat trap Message Queue Failure, %d\n", msg_id);
				return;
			}else
				printf("Recreat trap Message queue Ok, %d\n", msg_id);
		}
		
	}else
		printf("Creat Message queue success, %d\n", msg_id);
		
	trap_msg_buf.mtype = 1;	
}

void trapSend(u_int event_index)
{
	int len,status;
	trap_msg_para_	trap_msg_para;	
	
	memset(&trap_msg_para, 0, sizeof(trap_msg_para));
	trap_msg_para.device_ip = mng_network->ipaddr;
	trap_msg_para.event_index = event_index;
	sprintf(trap_msg_para.device_name,"%s",sysinfo->device_name);

	memcpy(trap_msg_para.device_mac, mng_network->macaddr, 6);
	
	len = sizeof(trap_msg_para);
	
	memset(trap_msg_buf.mtext, 0, sizeof(trap_msg_buf.mtext));	
	bcopy(&trap_msg_para,&trap_msg_buf.mtext[0], len);	
	if(msg_id >= 0){	
		status = msgsnd(msg_id, &trap_msg_buf, sizeof(trap_msg_buf.mtext), 0);
		if(status < 0){
		//	printf("send message failure, %d\n", status);
			printf( "send message failed, %d, %d\n", status, event_index);
		}else{
			printf( "send message success, %d, %d\n", status, event_index);
		}
	}	
	usleep(20000);
}


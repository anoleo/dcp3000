

#include "shmmsg_client.h"


typedef struct {
	shmem_message_t *shm_msg;
	msg_buf_t msg_buf;
} shmmsgclient_st;


#define MYSELF_SHMEM_PROGRAM_ID 	284
#define OPPOSITE_SHMEM_PROGRAM_ID	220
#define MSG_BUF_SIZE		0x800000

static shmmsgclient_st mymsgclient;
//char shmclient_databuf[MSG_BUF_SIZE];

char *get_shmmsg_rcvdata(void)
{
	return mymsgclient.msg_buf.msgdata;
}

int send_shmmsg(int msgid, char *data, int len)
{
	mymsgclient.msg_buf.msgid = msgid;
		
	mymsgclient.msg_buf.msglen = len;
	mymsgclient.msg_buf.msgdata = data;

	return shmem_msg_send(mymsgclient.shm_msg, &mymsgclient.msg_buf, SHM_WRITE_TIME_OUT_MS);
}

int rcv_shmmsg(char *data)
{
	int ret;

	//if(data){
		mymsgclient.msg_buf.msgdata = data;
	//}else{
	//	mymsgclient.msg_buf.msgdata = shmclient_databuf;
	//}

	ret = shmem_msg_rcv(mymsgclient.shm_msg, &mymsgclient.msg_buf, 10000);
	if(ret > 0){
		/*
		printf("shm msg rcv : msgid = %d \n", mymsgclient.msg_buf.msgid);
		printf("shm msg rcv : program_id = %d \n", mymsgclient.msg_buf.program_id);
		printf("shm msg rcv : process_id = %d \n", mymsgclient.msg_buf.process_id);
		print_bytes(mymsgclient.msg_buf.msgdata, mymsgclient.msg_buf.msglen);
		//*/
		return mymsgclient.msg_buf.msglen;
	}else{
		printf("shm msg rcv : failed \n");
		return 0;
	}
}


void init_shmmsg_client(void)
{

	shmem_obj_t *p_shmem;


	//char *shm_name = "my_shm";
	p_shmem = shmem_init(723, MSG_BUF_SIZE, SHM_SYSTEMV_IF);
	mymsgclient.shm_msg = shmem_msg_init(p_shmem, MYSELF_SHMEM_PROGRAM_ID);

		
	init_msg_buf(&mymsgclient.msg_buf, NULL);
	
	mymsgclient.msg_buf.program_id = OPPOSITE_SHMEM_PROGRAM_ID;
	mymsgclient.msg_buf.process_id = 0;

	mymsgclient.msg_buf.buf_size = -1;		// buf_size < 0, 假若msgdata == NULL, 则自动分配接收的数据空间

		
}

void free_shmmsg_client(void)
{
	shmem_obj_t *p_shmem = mymsgclient.shm_msg->p_shmem;
	
	shmem_msg_destroy(mymsgclient.shm_msg);
	shmem_destroy(p_shmem);
}



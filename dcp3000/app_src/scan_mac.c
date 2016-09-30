
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "scan_mac.h"

#include "appref.h"


int ClientSend(char *SendBuf);
/********************add by yjtang define for setup Mac  29/11/2010**************/

char *pMacData[10] = {NULL};	
char sendIP[20] = "10.10.70.181";
u_char SetMacFlag = 0x00;


/*****************************************************************
*Function:HexChar									               	    *
*parameters:													    *
*		char							 	             				    *
*Return:														    *
*		char											 		    *
*Description:													    *
*		change the hex char to decimal 
*Created:yjtang	2010/12/03				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/
char HexChar(char c)
{
	if((c>='0')&&(c<='9'))
		return c-0x30;
	else if((c>='A')&&(c<='F'))
		return c-'A'+10;
	else if((c>='a')&&(c<='f'))
		return c-'a'+10;
	else 
		return 0x10;
}

/*****************************************************************
*Function:Str2Hex								               	    *
*parameters:													    *
*		char*str 	 	source data
*		char*data 	store buf
*Return:														    *
*		int 											 		    *
*Description:													    *
*		change string to hex 
*Created:yjtang	2010/12/03				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/

int Str2Hex(char* str,char* data)
{
	int t,t1,i;
	int rlen=0,len=strlen(str);
	for(i=0;i<len;)
	{
		char l;
		char	h=str[i];
		if(h==' ')
		{
			i++;
			continue;
		}
		i++;
		if(i>=len)
			break;
		l=str[i];
		t=HexChar(h);
		t1=HexChar(l);
		if((t==16)||(t1==16))
			break;
		else 
			t=t*16+t1;
		i++;
		data[rlen]=(char)t;
		rlen++;
	}
	return rlen;
}

/*****************************************************************
*Function:SaveMac								               	           *
*parameters:													    *
*		void 							 	             		           *
*Return:														    *
*		int 										 		    	    *
*Description:													    *
*		save mac paramter
*Created:yjtang	2010/12/03				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/
int SaveMac()
{
	int i = 0,j;
	char MacBuf[50] = "";

	j = 0;
	for(i = 0; i < SETMAC_MACNUMBER; i++)
	{
		Str2Hex(pMacData[i+2],&MacBuf[j]);
		j = j + 6;
	}

	memcpy(mng_network->macaddr, &MacBuf[0], 6);
	memcpy(tsoip_network[0].macaddr, &MacBuf[6], 6);
	memcpy(tsoip_network[1].macaddr, &MacBuf[12], 6);
	
	strcpy(sysinfo->device_sn,pMacData[SETMAC_MACNUMBER+2]);	
	

	save_devsn(sysinfo->device_sn);
	save_macaddr(0, mng_network->macaddr);
	save_macaddr(1, tsoip_network[0].macaddr);
	save_macaddr(2, tsoip_network[1].macaddr);


	set_mac_addr(sysinfo->netif_name, mng_network->macaddr);
	set_mac_addr(tsoip_network[0].nif_name, tsoip_network[0].macaddr);
	set_mac_addr(tsoip_network[1].nif_name, tsoip_network[1].macaddr);
	
	
	sleep(3);
	
	return 0;	
}

/*****************************************************************
*Function:ProcessString							               	           *
*parameters:													    *
*		char*RecvBuf		received data from socket  				   *
*Return:														    *
*		int 										 		    	    *
*Description:													    *
*		process received data(strtok)
*Created:yjtang	2010/12/03				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/
int ProcessString(char *RecvBuf)
{
	char* str = ":";
	char* token;
	int i = 0,j;
	for(j = 0; j < 10; j++)
	{
		pMacData[j] = NULL;
	}
	token = strtok(RecvBuf,str);
	pMacData[i] = token;

	while(token != NULL)
	{
		i++;
		token = strtok(NULL,str);
		pMacData[i] = token;
	}
	return 0;
}

/*****************************************************************
*Function:ClientSend							               	           *
*parameters:													    *
*		char* SendBuf		send data			            		           *
*Return:														    *
*		int										 		    	    *
*Description:													    *
*		client send msg
*Created:yjtang	2010/12/03				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/

int ClientSend(char *SendBuf)
{
	struct sockaddr_in s_addr;
	int sock;
	int addr_len;
	int len;	

	if((sock = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
		printf("create socket error!!\n");
		return 1;
	}

	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(SETMAC_PORT);
	s_addr.sin_addr.s_addr = inet_addr(sendIP);

	addr_len = sizeof(s_addr);

	len = sendto(sock,SendBuf,strlen(SendBuf)+1,0,(struct sockaddr*)&s_addr,addr_len);

	if(len <0)
	{
		printf("send error!\n");
		return 1;
	}
	//printf("send success.\n\r");
	return 0;
}

/*****************************************************************
*Function:ConfirmSetupMac										    *
*parameters:													    *
*		void								 					    *
*Return:														    *
*		void											 		    *
*Description:													    *
*		if change mac address by scanner then send success msg		    *
*		else return
*Created:yjtang	2010/12/03				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/

void ConfirmSetupMac()
{
	char SendBuf[128] = "";
	
	if(SetMacFlag == 0x01)
	{
		SetMacFlag = 0x00;
		sprintf(SendBuf,"%d:%d:",SETMAC_HEADER,SETMAC_SUCCESS);
		ClientSend(SendBuf);
		return;
	}
	else
	{
		return;
	}
}

void PING_Test(u_int ipaddr)
{
	struct in_addr sIn;
	unsigned char uBuf[50] = {0};

	//little endian
	sIn.s_addr = ipaddr;
	sprintf((char *)uBuf, "ping -c 3 %s -W 2 -q > /dev/null &", inet_ntoa(sIn));
	system((char *)uBuf);
}


/*****************************************************************
*Function:ServiceRecv							               	           *
*parameters:													    *
*		void							 	             		           *
*Return:														    *
*		void										 		    	    *
*Description:													    *
*		thread ,server recv data
*Created:yjtang	2010/12/03				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/

void *SCANServiceRecv(void *arg)
{
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;	
	int sock;	
	socklen_t addr_len;	
	int len;	
	char buff[256];
	char sendbuf[256];
	int header,recvType,sendType;
	int i=0;
	//printf("socket function!!\n");
	if((sock = socket(AF_INET,SOCK_DGRAM,0)) == -1)	
	{		
		printf("socket error!\n");		
		return;
	}	
	
	memset(&s_addr,0, sizeof(struct sockaddr_in));	
	s_addr.sin_family = AF_INET;	
	s_addr.sin_port = htons(SETMAC_PORT);	
	s_addr.sin_addr.s_addr = INADDR_ANY;	

	if((bind(sock,(struct sockaddr *)&s_addr,sizeof(s_addr))) == -1)	
	{		
		printf("bind error!!\n");		
		return;	
	}	
	else	
	{
		printf("Create socket and bind address to socket.\n");	
	}
	
	addr_len = sizeof(c_addr);	

	while(1)	
	{
		for(i = 0; i < 10; i++)
		{
			pMacData[i] = NULL;
		}
		
		len = recvfrom(sock,buff,sizeof(buff)-1,0,(struct sockaddr *)&c_addr,&addr_len);			

		SetMacFlag = 0x01;
		if(len < 0)		
		{			
			printf("recvfrom error!!\n");	
			sprintf(sendbuf,"%d:%d:",SETMAC_HEADER,SETMAC_FAILURE);
			ClientSend(sendbuf);
			continue;		
		}
		buff[len] = '\0';
		
		//printf("recv  %s :%d message:%s len:%d\n\r",inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port),buff,strlen(buff));

		len = strlen(buff);

		ProcessString(buff);

		if(pMacData[0] == NULL)
		{
			sleep(1);
			continue;
		}
		
		if((header = atoi(pMacData[0])) == SETMAC_HEADER)
		{
			recvType = atoi(pMacData[1]);
			strcpy( sendIP,inet_ntoa(c_addr.sin_addr) ); 
			
			switch(recvType)
			{
				case SETMAC_ONLINE:
					{
						sendType = SETMAC_ONLINE;
						sprintf(sendbuf,"%d:%d:%d:",SETMAC_HEADER,sendType,SETMAC_MACNUMBER);
						ClientSend(sendbuf);
					}
					break;
				
				case SETMAC_MACDATA:
					{
						if(len != SETMAC_BUFLEN)
						{
							sendType = SETMAC_FAILURE;
							sprintf(sendbuf,"%d:%d:",SETMAC_HEADER,sendType);
							ClientSend(sendbuf);
							break;
						}
						else
						{
							SaveMac();
							ConfirmSetupMac();
							//PING_Test(c_addr.sin_addr);
						}
					}
					break;

				default:
					break;
			}
		}
	}	
	return ;
}


/*****************************************************************
*Function:Pthread_ScanMac							                        *
*parameters:													    *
*		void									            		           *
*Return:														    *
*		void										 		    	    *
*Description:													    *
*		create ScanMac pthread
*Created:yjtang	2011/4/27				         				    *
*															    *
*History of Modification:										    *
*														           *
*****************************************************************/


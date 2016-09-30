
#ifndef _SCAN_MAC_H_
#define _SCAN_MAC_H_

/*****************add by yjtang for setup mac 29/11/2010************************/

#define SETMAC_HEADER				0x0080  //packet header
#define SETMAC_ONLINE				0x0001  //ȷ������
#define SETMAC_OFFLINE				0x0002
#define SETMAC_SUCCESS				0x0003  //���óɹ�
#define SETMAC_FAILURE				0x0004  //����ʧ��
#define SETMAC_QUERY_MACNUMBER	0x0005
#define SETMAC_MACDATA			0x0006
#define SETMAC_RECVDATA			0x0007  //���յ�MAC��ַ����

#define SETMAC_MACNUMBER			3	//�豸�����mac ��ַ����
#define SETMAC_PORT				0x2125   // 8485�˿�
#define SETMAC_BUFLEN				(SETMAC_MACNUMBER*17+4+13+SETMAC_MACNUMBER+2)

/****************end define******************************************/


void *SCANServiceRecv(void *arg);

#endif /*_SCAN_MAC_H_*/


 

/*******************JSON INIT******************/
#define DEV_NAME							0
#define DEV_SN								1


#define TSIP_PORT							2
#define ASIIN_CH								3
#define ASIOUT_CH							4
#define IPIN_CH								5
#define TOTAL_TSIN							6
#define MUX_CH								7
#define UPGRADE_FLAG						8

#define WEB_USE								9

#define DEVICE_TYPE							10

#define INIT_LEN								11
/****************************************************/
/*******************JSON UPGRADE******************/
#define UPGRADE_STATUS_FLAG 0
#define UPGRADE_OVERALL_PROGRESS_RATE 1
#define UPGRADE_MESSAGE 2
	
#define UPGRADE_LEN 3

//UP_FLAG
#define DEFAULT			0
#define PREPARING 		1
#define READY_AND_DO 	2 
#define END 				3


/****************************************************/
/*******************JSON DEFAULT IPS******************/
#define IPS_IP0								0
#define IPS_IP1								1
#define IPS_IP2								2
#define IPS_IP3								3
#define IPS_PORT								4

#define IPS_LEN								5
/****************************************************/
/*******************JSON DEFAULT MACS******************/
#define MACS_MAC0							0
#define MACS_MAC1							1
#define MACS_MAC2							2
#define MACS_MAC3							3
#define MACS_MAC4							4
#define MACS_MAC5							5

#define MACS_LEN							6
/****************************************************/
/*************JSON_SET_IPIN_VALUE*************/
//dim1
#define IPIN_INDEX									0
#define IPIN_IP0										1
#define IPIN_IP1										2
#define IPIN_IP2										3
#define IPIN_IP3										4
#define IPIN_PORT									5
#define IPIN_ENA										6

#define IPIN_PARAS_LEN								7
/*********************************************/

/***********JSON_PORT_STATUS_VALUE***************/
//dim1
#define CH_STATUS_S								0

//dim2
#define CH_FLAG									0
#define CH_BITRATE								1
#define CH_PAT_CRC								2

#define CH_STATUS_LEN							3
/*********************************************/
/***********JSON_TSOIP_STATUS_VALUE***************/
//dim
#define TSOIP_STATUS_S								0
//dim2
#define TSOIP_STATUS_LINK							0
#define TSOIP_STATUS_TX_BITRATE					1
#define TSOIP_STATUS_RX_BITRATE					2

#define TSOIP_STATUS_LEN							3
/*********************************************/
/*************JSON_INPUT_TREE_VALUE*************/
//dim1
#define INPUT_TREE_INDEX							0
#define INPUT_TREE_LOCK							1
#define INPUT_TREE_PAT_CRC							2
#define INPUT_TREE_PROS_NUM						3
#define INPUT_TREE_EMMPIDS_NUM					4
#define INPUT_TREE_IPS_NUM							5

#define INPUT_TREE_LEN								6


//dim2 IP_PARAS


//dim3 PROGRAM_PARAS
#define INPUT_TREE_PROS_NAME						0
#define INPUT_TREE_PROS_PROVIDER_NAME			1
#define INPUT_TREE_PROS_PN							2
#define INPUT_TREE_PROS_PMT						3
#define INPUT_TREE_PROS_PCR						4
#define INPUT_TREE_PROS_FCA_MODE					5
#define INPUT_TREE_PROS_SER_TYPE					6
#define INPUT_TREE_PROS_ESPIDS_NUM				7
#define INPUT_TREE_PROS_ECMPIDS_NUM				8

#define INPUT_TREE_PROS_LEN						9

//dim4 ES_PIDS	
#define INPUT_TREE_PROS_ESPIDS_PID				0
#define INPUT_TREE_PROS_ESPIDS_TYPE				1

#define INPUT_TREE_PROS_ESPIDS_LEN				2

/******************************************/
/*************JSON_MUX_TREE_VALUE*************/
//dim1
#define MUX_TREE_INDEX								0
#define MUX_TREE_SET_NUM							1
#define MUX_TREE_PROS_NUM							2
#define MUX_TREE_PID_PASS_NUM						3
#define MUX_TREE_NIT_SECTION_NUM					4

#define MUX_TREE_LEN								5
//dim2 programs
#define MUX_TREE_PROS_SOR_INDEX					0
#define MUX_TREE_PROS_OLD_INFOS_NUM				1
#define MUX_TREE_PROS_MUX_INFOS_NUM				2
#define MUX_TREE_PROS_MUX_PIDS_NUM				3
#define MUX_TREE_PROS_CAS_NUM					4

#define MUX_TREE_PROS_LEN							5
//dim2 mux set
#define MUX_TREE_SET_ENA							0
#define MUX_TREE_SET_BITRATE						1
#define MUX_TREE_SET_AUTO							2
#define MUX_TREE_SET_SDT							3
#define MUX_TREE_SET_NIT							4
#define MUX_TREE_SET_EIT							5
#define MUX_TREE_SET_TSID							6
#define MUX_TREE_SET_OID							7
#define MUX_TREE_SET_IPOUT_NUM					8

#define MUX_TREE_SET_LEN							9

//dim2 pid pass
#define MUX_TREE_PID_PASS_CH						0
#define MUX_TREE_PID_PASS_ORI						1
#define MUX_TREE_PID_PASS_MUX						2

#define MUX_TREE_PID_PASS_LEN						3


//dim2 nit


//dim3 old infos
#define MUX_TREE_OLD_INFOS_PN						0
#define MUX_TREE_OLD_INFOS_PMT					1
#define MUX_TREE_OLD_INFOS_PCR					2
#define MUX_TREE_OLD_INFOS_PRO_INDEX				3

#define MUX_TREE_OLD_INFOS_LEN					4

//dim3 mux infos
#define MUX_TREE_MUX_INFOS_PN						0
#define MUX_TREE_MUX_INFOS_PMT					1
#define MUX_TREE_MUX_INFOS_PCR					2
#define MUX_TREE_MUX_INFOS_S_TYPE				3
#define MUX_TREE_MUX_INFOS_PRO_NAME				4
#define MUX_TREE_MUX_INFOS_PROVIDER_NAME		5

#define MUX_TREE_MUX_INFOS_LEN					6
//dim3 mux pids

//dim3 CAS
#define MUX_TREE_PROS_CAS_TYPE					0
#define MUX_TREE_PROS_CAS_ITEM_NUM				1

#define MUX_TREE_PROS_CAS_LEN						2

//dim4 CAS_TYPE&INDEX
#define MUX_TREE_PROS_CAS_CLEAR					0
#define MUX_TREE_PROS_CAS_BISS1					1
#define MUX_TREE_PROS_CAS_BISSE					2
#define MUX_TREE_PROS_CAS_SIM						3

#define MUX_TREE_PROS_CAS_ITEM_BISS1					0
#define MUX_TREE_PROS_CAS_ITEM_BISSE					1
#define MUX_TREE_PROS_CAS_ITEM_SIM					2

#define MUX_TREE_PROS_CAS_ITEM_LEN					3

//dim5 CAS-BISS_1
#define MUX_TREE_PROS_CAS_BISS1_ENC_SES						0

#define MUX_TREE_PROS_CAS_BISS1_LEN							1
//dim5 CAS-BISS_E
#define MUX_TREE_PROS_CAS_BISSE_ENC_SES						0
#define MUX_TREE_PROS_CAS_BISSE_INJ_ID						1

#define MUX_TREE_PROS_CAS_BISSE_LEN							2
//dim5 Simlcrypt
#define MUX_TREE_PROS_CAS_SIM_ENA							0
#define MUX_TREE_PROS_CAS_SIM_AC_INDEX						1
#define MUX_TREE_PROS_CAS_SIM_ECM_PID						2

#define MUX_TREE_PROS_CAS_SIM_LEN							3

//dim5 mux pid	
#define MUX_TREE_PROS_MUX_PIDS_ORI				0
#define MUX_TREE_PROS_MUX_PIDS_TYPE				1
#define MUX_TREE_PROS_MUX_PIDS_MUX				2
#define MUX_TREE_PROS_MUX_PIDS_CA_ENA			3

#define MUX_TREE_PROS_MUX_PIDS_LEN				4

//dim3 outip set
#define MUX_TREE_SET_IPOUT_PORT_ENA				0
#define MUX_TREE_SET_IPOUT_UDP_O_RTP				1
#define MUX_TREE_SET_IPOUT_IPS_NUM				2

#define MUX_TREE_SET_IPOUT_LEN					3

//dim2 nit section set
#define MUX_TREE_NIT_SECTION_TYPE					0
#define MUX_TREE_NIT_SECTION_DATA				1
#define MUX_TREE_NIT_SECTION_LEN					2

/******************************************/
//SYS
/*************JSON_SYS_PAPA_VALUE*************/
//dim1
#define SYS_NAME									0
#define SYS_PORT									1
#define SYS_ASI										2
#define MUX											3

#define SYS_LEN										1			
/******************************************/
/*************JSON_MAN_NET_VALUE*************/
//dim1
#define MAN_NET_IP									0
#define MAN_NET_MAC								1
#define MAN_NET_MASK								2
#define MAN_NET_GATEWAY							3
#define MAN_NET_TRAP								4

#define MAN_NET_LEN								5
/******************************************/
/*************JSON_TSOIP_NET_VALUE*************/
//dim1
#define TSOIP_PORT									0
#define TSOIP_MODE									1
#define TSOIP_IGMP									2
#define TSOIP_IP										3
#define TSOIP_MAC									4
#define TSOIP_MASK									5
#define TSOIP_GATEWAY								6


#define TSOIP_LEN									7
/******************************************/
/*************JSON_CLK_SET_VALUE*************/
//dim1
#define YEAR											0
#define MONTH										1
#define DAYS											2
#define HOUR											3
#define MINUTE											4

#define CLK_LEN										5
/******************************************/
/*************JSON_CARD_VALUE*************/
//dim1
#define CARD_1								0
#define CARD_2								1
#define CARD_3								2
#define CARD_4								3
#define CARD_5								4
#define CARD_6								5
#define CARD_7								6

#define CARD_LEN								7							
/******************************************/

//MISC
/*************JSON_VERSION_VALUE*************/
//dim1
#define FRIMWARE									0
#define KERNEL										1
#define APP											2
#define FPGA											3
#define NIOS											4
#define CA											5
#define MODULATION									6

#define VERSION_LEN									7
/******************************************/

/*************JSON_IPOUT_PARA_VALUE*************/
//dim1
#define PACKETS_PER									0
#define TTL											1
#define TOS											2

#define IPOUT_PARA_LEN								3
/******************************************/
/*************JSON_CMD_VALUE*************/
//dim1
#define CMD_TYPE									0

#define CMD_LEN										1

/*######CMD VALUE######*/
#define REBOOT										0
#define DEFAULT_PARA								1
#define UPGRADE										2
#define RESET_UPGRADE_FLAG						3

/*###################*/
/******************************************/
//MUX_SETTING
/******************************************/
#define	WEB_SETTING_INPUT_P_SHOW				0
#define	WEB_SETTING_INPUT_PORT					1
#define	WEB_SETTING_VERSION						2
#define	WEB_SETTING_CHAR_INSTR					3
#define	WEB_SETTING_CHAR_OUTSTR					4

#define	WEB_SETTING_LEN							5
/******************************************/
//STATUS
/*************JSON_LOG_MSG_VALUE*************/
//dim1
#define LOG_MSG_START_SN										0
#define LOG_MSG_LAST_SN										1
#define LOG_MSG_SUM_SN 										2
#define LOG_MSG_CON_SN										3
#define LOG_MSG_CON_INFOS										4

#define LOG_MSG_LEN										5

//dim2
#define LOG_MSG_CON_INFO_INDEX										0
#define LOG_MSG_CON_INFO_STR									1

#define LOG_MSG_CON_INFO_LEN										2
/******************************************/

/*************JSON_CA_SERVER_VALUE*************/
//dim1
#define CA_SERVER_NO											0
#define CA_SERVER_NAME											1
#define CA_SERVER_ENABLE										2
#define CA_SERVER_CA_SYS_ID									3
#define CA_SERVER_CA_SUB_ID									4
#define CA_SERVER_ECMG_IP										5
#define CA_SERVER_EMMG_IP										6
#define CA_SERVER_ECMG_PORT									7
#define CA_SERVER_EMMG_TCP_PORT								8
#define CA_SERVER_EMMG_UDP_PORT								9
#define CA_SERVER_EMMG_PROTOCOL								10
#define CA_SERVER_EMMG_BANDWIDTH							11
#define CA_SERVER_EMM_PID										12
#define CA_SERVER_EMM_PRIVATE_DATA							13

#define CA_SERVER_PROTOCOL_VERSION							14
#define CA_SERVER_DEFAULT_CP_DURATION						15

#define CA_SERVER_LEN											16

	
/******************************************/
/*************JSON_CA_ACCESS_CRIERIA_VALUE*************/
//dim1
#define AC_CA_SERVER_NO										0
#define AC_LIST_NUM												1

#define CA_AC_LEN												2
//dim2
#define AC_INDEX												0
#define AC_NAME													1
#define AC_DATA													2
#define AC_ECM_DESC											3

#define AC_ITEM_LEN												4
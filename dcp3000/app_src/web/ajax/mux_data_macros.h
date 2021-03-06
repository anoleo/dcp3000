//----SYSTEM_PRODUCT_NAME_DATA_TYPE			0x010001
//product name 20B char[20]
#define SYSTEM_PRODUCT_NAME 0

#define SYSTEM_PRODUCT_NAME_LEN 1

//----SYSTEM_FIRMWARE_VER_DATA_TYPE			0x010002
//firmware version 4B int
//kernel version 4B int
//mcu version 4B int
//fpga version 4B int
//nios version 4B int
//web version 4B int
//modulation version 4B int
#define SYSTEM_FIRMWARE_VER_FIRMWARE 		0
#define SYSTEM_FIRMWARE_VER_KERNEL  			1
#define SYSTEM_FIRMWARE_VER_MCU  				2
#define SYSTEM_FIRMWARE_VER_FPGA 			3
#define SYSTEM_FIRMWARE_VER_NIOS  			4
#define SYSTEM_FIRMWARE_VER_WEB  				5
#define SYSTEM_FIRMWARE_VER_MODULATION 		6

//#define SYSTEM_FIRMWARE_VER_MODULATION 		7

//----SYSTEM_DEVICE_NAME_DATA_TYPE			0x010003
#define SYSTEM_DEVICE_NAME					0

#define SYSTEM_DEVICE_NAME_LEN				1

//----SYSTEM_DEVICE_SN_DATA_TYPE				0x010004
#define SYSTEM_DEVICE_SN						0

#define SYSTEM_DEVICE_SN_LEN					1

//----SYSTEM_SYSTEM_TIME_DATA_TYPE			0x010005
#define SYSTEM_SYSTEM_TIME_YEAR				0
#define SYSTEM_SYSTEM_TIME_MOON				1
#define SYSTEM_SYSTEM_TIME_DAY				2
#define SYSTEM_SYSTEM_TIME_HOUR				3
#define SYSTEM_SYSTEM_TIME_MINUTE			4
#define SYSTEM_SYSTEM_TIME_SECOND			5

#define SYSTEM_SYSTEM_TIME_LEN				6

//----SYSTEM_DEVICE_TEMPERATURE_DATA_TYPE		0x010006
#define SYSTEM_DEVICE_TEMPERATURE						0

#define SYSTEM_DEVICE_TEMPERATURE_LEN					1

//----SYSTEM_WEB_LOGIN_DATA_TYPE				0x010007
#define SYSTEM_WEB_LOGIN_USERNAME 			0
#define SYSTEM_WEB_LOGIN_PASSWORD			1

#define SYSTEM_WEB_LOGIN_LEN					2

//----SYSTEM_DEVICE_IO_QTY_DATA_TYPE			0x010008
#define SYSTEM_DEVICE_IO_QTY_TOTAL_TSIN		0
#define SYSTEM_DEVICE_IO_QTY_TSIP_PORT		1
#define SYSTEM_DEVICE_IO_QTY_IP_IN			2
#define SYSTEM_DEVICE_IO_QTY_IP_OUT			3
#define SYSTEM_DEVICE_IO_QTY_ASI_IN			4
#define SYSTEM_DEVICE_IO_QTY_ASI_OUT			5
#define SYSTEM_DEVICE_IO_QTY_MUX				6

#define SYSTEM_DEVICE_IO_QTY_LEN		7

//----SYSTEM_ENCRYPT_AUTH_DATA_TYPE			0x010009
#define SYSTEM_ENCRYPT_AUTH_ROM				0
#define SYSTEM_ENCRYPT_AUTH_LICENSE			1
#define SYSTEM_ENCRYPT_AUTH_AUTH_CODE		2
#define SYSTEM_ENCRYPT_AUTH_RESULT			3

#define SYSTEM_ENCRYPT_AUTH_LEN				4

//----SYSTEM_EVENTLOG_DATA_TYPE				0x01000a
#define SYSTEM_EVENTLOG_ENVENT_INDEX			0
#define SYSTEM_EVENTLOG_DATATIME				0
#define SYSTEM_EVENTLOG_ENVENT_MSG			0

//----SYSTEM_FACTORY_RESET_DATA_TYPE				0x01000b

//----WEB_GLOBAL_PARAMS_DATA_TYPE				0x0100f1


//----MNG_NETWORK_DATA_TYPE			0x020001
//----TSOIP_NETWORK_DATA_TYPE			0x030001
//----TSOIP_PROTOCOL_PARAM_DATA_TYPE			0x030002
//----TSOIP_NETWORK_DATA_TYPE			0x030001
//----TSOIP_LINK_STATUS_DATA_TYPE			0x030003
//----TSOIP_IN_PORT_BITRATE_DATA_TYPE  0x030004
//----TSOIP_OUT_PORT_BITRATE_DATA_TYPE  0x030005

//----TSOIP_IN_CHANNEL_DATA_TYPE		0x040001
//----TSOIP_OUT_CHANNEL_DATA_TYPE		0x050001

//----ASI_IN_DATA_TYPE				0x060001
//----ASI_OUT_DATA_TYPE				0x070001

//----MODULATION_GLOBLE_PARAM_DATA_TYPE			0x080001
//dim 1
#define MGP_GROUP_START_FREQ_NUM 			0
	
#define MGP_SYMBOLRATE							1	//kbaud/s
	
#define MGP_QAM_CONSTEL						2	//6 : 64qam, 8 : 256qam
#define MGP_ANNEX_ABC							3	//0 : annex A, 1 : annex B, 2 : annex C, 3 : annex A 6M 
#define MGP_ROLLOFF								4	//0 : 0.12, 1 : 0.13, 2 : 0.15, 3 : 0.18
#define MGP_BANDWIDTH							5	//6-8M
	
#define MGP_ICW									6		//Interleaver Control Word(annex B)
/*
[0:CW-0 I128 J1][1:CW-1 I128 J1][2:CW-2 I128 J2][3:CW-3 I64 J2]
[4:CW-4 I128 J3][5:CW-5 I32 J4][6:CW-6 I128 J4][7:CW-7 I16 J8]
[8:CW-8 I128 J5][9:CW-9 I8 J16][10:CW-A I128 J6][11:CW-B RESERVED]
[12:CW-C I128 J7][13:CW-D RESERVED][14:CW-E I128 J8][15:CW-F RESERVED]
*/
#define MGP_SPEC_INV							7	//Spectrum Inversion
#define MGP_MAIN_SW							8		//0 turn off, 1 turn on

#define MGP_LEN									9
//dim 2
#define MGP_GROUP_START_FREQ1				0
#define MGP_GROUP_START_FREQ2				1
#define MGP_GROUP_START_FREQ3				2
#define MGP_GROUP_START_FREQ4				3
#define MGP_GROUP_START_FREQ_LEN				4

//----MODULATION_QAM_CHANNEL_DATA_TYPE			0x080002
//dim1
#define MQC_CHAN_NUM							0
#define MQC_FREQ_IDX							1//#
#define MQC_RF_SW								2//#
#define MQC_MODULATE_SW						3//#

#define MQC_FREQUENCE							4//kHz   #
#define MQC_SYMBOLRATE							5	//kbaud/s  #


#define MQC_QAM_CONSTEL						6	//6 : 64qam, 8 : 256qam 
#define MQC_ANNEX_ABC							7	//0 : annex A, 1 : annex B, 2 : annex C, 3 : annex A 6M 
#define MQC_ROLL_OFF							8		//0 : 0.12, 1 : 0.13, 2 : 0.15, 3 : 0.18
#define MQC_BANDWIDTH							9	//6-8M 
	
#define MQC_ICW									10
#define MQC_SPEC_INV							11	//?
#define MQC_TS_SRC								12//***
	
#define MQC_LEN									13

//----MODULATION_RF_LEVEL_DATA_TYPE			0x080003
#define MRL_LEVEL								0
#define MRL_LEN									1
//----MODULATION_RF_BACKUP_DATA_TYPE			0x080004
#define MRB_ENABLE_FLAG						0
#define MRB_OUT_MODE							1

#define MRB_LEN									2
//----MODULATION_EXECUTE_DATA_TYPE			0x080009

//----TSIN_BITERATE_DATA_TYPE			0x090001
//----TSIN_STATUS_DATA_TYPE			0x090002
//----TSIN_PROGRAM_INFO_DATA_TYPE		0x090003
//dim1
#define TSIN_PROGRAM_INFO_INDEX							0
#define TSIN_PROGRAM_INFO_PROS_NUM						1
#define TSIN_PROGRAM_INFO_EMMPIDS_NUM					2

#define TSIN_PROGRAM_INFO_LEN								3

//dim3 PROGRAM_PARAS
#define TSIN_PROGRAM_INFO_PROS_NAME						0
#define TSIN_PROGRAM_INFO_PROS_PROVIDER_NAME			1
#define TSIN_PROGRAM_INFO_PROS_PN						2
#define TSIN_PROGRAM_INFO_PROS_PMT						3
#define TSIN_PROGRAM_INFO_PROS_PCR						4
#define TSIN_PROGRAM_INFO_PROS_FCA_MODE					5
#define TSIN_PROGRAM_INFO_PROS_SER_TYPE					6
#define TSIN_PROGRAM_INFO_PROS_ESPIDS_NUM				7
#define TSIN_PROGRAM_INFO_PROS_ECMPIDS_NUM				8

#define TSIN_PROGRAM_INFO_PROS_LEN						9

//dim4 ES_PIDS	
#define TSIN_PROGRAM_INFO_PROS_ESPIDS_PID				0
#define TSIN_PROGRAM_INFO_PROS_ESPIDS_TYPE				1

#define TSIN_PROGRAM_INFO_PROS_ESPIDS_LEN				2

//----TSIN_PAT_CRC_DATA_TYPE			0x090004

//----CAPTURE_CONTROL_DATA_TYPE		0x090005
//dim1
#define CAP_CON_GLOBAL_CONTROL_FLAG						0
#define CAP_CON_SET_NUM									1

#define CAP_CON_LEN											2
//dim2
#define CAP_CON_SET_CH										0
#define CAP_CON_SET_CONTROL_FLAG							1

#define CAP_CON_SET_LEN									2


//----MUX_PARAMETER_DATA_TYPE			0x0a0001
//----MUX_BITERATE_DATA_TYPE			0x0a0002
//----MUX_PN_MAPPING_DATA_TYPE		0x0a0003
//----MUX_PASS_PID_DATA_TYPE			0x0a0004
//----MUX_PROGRAM_INFO_DATA_TYPE		0x0a0005
//----MUX_NIT_SECTION_DATA_TYPE		0x0a0006   

//----MUX_EXECUTE_DATA_TYPE			0x0a0009

//----CAS_CP_DURATION_DATA_TYPE			0x0b0001
//----CAS_AC_LIST_DATA_TYPE			0x0b0002
//----CAS_CA_SERVER_DATA_TYPE			0x0b0003
//----CAS_SCRAMBLE_PROGRAM_CHOSED_DATA_TYPE			0x0b0004
//----CAS_ECMG_STATUS_DATA_TYPE			0x0b0005
#define CAS_ECMG_STATUS_CHANNEL							0
#define CAS_ECMG_STATUS_CONNECTED						1
#define CAS_ECMG_STATUS_PROTOCOL_VERSION				2
#define CAS_ECMG_STATUS_IP_PORT							3
#define CAS_ECMG_STATUS_IP_ADDR							4
#define CAS_ECMG_STATUS_MESSAGES_RECEIVED				5
#define CAS_ECMG_STATUS_PROTOCOL_ERRORS					6
#define CAS_ECMG_STATUS_PARAMETER_ERRORS				7
#define CAS_ECMG_STATUS_INTERNAL_ERRORS					8
#define CAS_ECMG_STATUS_CHANNEL_TESTS					9
#define CAS_ECMG_STATUS_CHANNEL_ERRORS					10
#define CAS_ECMG_STATUS_STREAM_TESTS					11
#define CAS_ECMG_STATUS_STREAM_ERRORS					12
#define CAS_ECMG_STATUS_CONNECT_TIMEOUT_ERRORS		13
#define CAS_ECMG_STATUS_CW_PROVISION_TIMEOUT_ERRORS	14
#define CAS_ECMG_STATUS_CW_PROVISION_COUNTS			15

#define CAS_ECMG_STATUS_LEN								16

//----CAS_EMMG_STATUS_DATA_TYPE			0x0b0006
#define CAS_EMMG_STATUS_CHANNEL							0
#define CAS_EMMG_STATUS_CONNECTED						1
#define CAS_EMMG_STATUS_IP_PORT							2
#define CAS_EMMG_STATUS_IP_ADDR							3
#define CAS_EMMG_STATUS_MESSAGES_RECEIVED				4
#define CAS_EMMG_STATUS_PROTOCOL_ERRORS					5
#define CAS_EMMG_STATUS_PARAMETER_ERRORS				6
#define CAS_EMMG_STATUS_INTERNAL_ERRORS					7
#define CAS_EMMG_STATUS_CHANNEL_TESTS					8
#define CAS_EMMG_STATUS_CHANNEL_ERRORS					9
#define CAS_EMMG_STATUS_STREAM_TESTS					10
#define CAS_EMMG_STATUS_STREAM_ERRORS					11
#define CAS_EMMG_STATUS_TCP_DATA_PROVISIONS			12
#define CAS_EMMG_STATUS_UDP_DATA_PROVISIONS			13

#define CAS_EMMG_STATUS_LEN								14


//----CAS_EXECUTE_DATA_TYPE			0x0b0009


//----UPGRADE_DATA_TYPE	0xfe0001



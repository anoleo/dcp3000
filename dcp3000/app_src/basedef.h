#ifndef ___BASE_DEF_H___
#define ___BASE_DEF_H___

#define SZ_1				0x00000001
#define SZ_2				0x00000002
#define SZ_4				0x00000004
#define SZ_8				0x00000008
#define SZ_16				0x00000010
#define SZ_32				0x00000020
#define SZ_64				0x00000040
#define SZ_128				0x00000080
#define SZ_256				0x00000100
#define SZ_512				0x00000200

#define SZ_1K				0x00000400
#define SZ_2K				0x00000800
#define SZ_4K				0x00001000
#define SZ_8K				0x00002000
#define SZ_16K				0x00004000
#define SZ_32K				0x00008000
#define SZ_64K				0x00010000
#define SZ_128K				0x00020000
#define SZ_256K				0x00040000
#define SZ_512K				0x00080000

#define SZ_1M				0x00100000
#define SZ_2M				0x00200000
#define SZ_4M				0x00400000
#define SZ_8M				0x00800000
#define SZ_16M				0x01000000
#define SZ_32M				0x02000000
#define SZ_64M				0x04000000
#define SZ_128M				0x08000000
#define SZ_256M				0x10000000
#define SZ_512M				0x20000000

#define SZ_1G				0x40000000
#define SZ_2G				0x80000000


#define SEC_1MIN	60
#define SEC_1HOUR	(3600)
#define SEC_1DAY	(3600*24)

#define INPUT_FLAG	0
#define OUTPUT_FLAG	1

#define READ_FLAG	0
#define WRITE_FLAG	1
#define RDWR_FLAG	2

#define MUTEX_UNLOCK	0
#define MUTEX_LOCKED	1

#define LOCK		1
#define UNLOCK	0

#define EXEC_OK		1
#define	EXEC_NULL	0
#define EXEC_ERR	-1
#define EXEC_TIMEOUT	-2


#define NO_EXIST		0
#define EXIST			1

#define VALID			1
#define INVALID			0

#define ENABLE			1
#define DISABLE			0

#define ON				1
#define OFF				0

#define NO				0
#define YES				1


#define STOP_FLAG		0
#define RUNNING_FLAG	1
#define PAUSE_FLAG		2

#endif

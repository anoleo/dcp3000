#ifndef ___MYINCLUDE_H___
#define ___MYINCLUDE_H___

#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>

#include <sys/mman.h>

#include <sched.h>



#include "basedef.h"
#include "system.h"

#include "parameter.h"

#include "gpio.h"
#include "fpga.h"
#include "fpga_reg.h"

#include "lm73.h"

#include "flash/mtd.h"
#include "flash/flash.h"
#include "flash/fram.h"

#include "utils/arraylist.h"
#include "utils/cirArrList.h"
#include "utils/cirqueue.h"
#include "utils/dataflag.h"
#include "utils/duList.h"
#include "utils/hashlib.h"
#include "utils/hashmap.h"
#include "utils/hashtable.h"
#include "utils/list.h"
#include "utils/node.h"
#include "utils/properties.h"
#include "utils/queue.h"
#include "utils/sArrList.h"
#include "utils/sim_encrypt.h"
#include "utils/stack.h"
#include "utils/system.h"
#include "utils/thread_pool.h"
#include "utils/utils.h"

#include "ipcmsg/ipcmsg.h"
#include "ipcmsg/ipcmsg_query_req.h"

#include "sqlite3/app_sql.h"
//#include "sqlite3/mysql.h"



#include "transfer/transceiver.h"
#include "transfer/spi.h"


#include "event/eventlog.h"
#include "event/board_event.h"

#include "tsoip/tsoip.h"
#include "tsoip/tsoip_backup.h"

#include "upgrade.h"

#include "ntp/ntpclient.h"

//struct timeval begintime, endtime, sssstime;




#define INTEGER_PARAM		0x1
#define FLOAT_PARAM		0x2
#define TEXT_PARAM		0x3
#define BLOB_PARAM		0x4

#define NETWORK_PARAM		0x5



#endif//___MYINCLUDE_H___

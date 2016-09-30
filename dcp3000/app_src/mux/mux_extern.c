
#include <sys/types.h>

#include "mux.h"
//#include "../sqlite3/mux_sql.h"



#define	REG_FPGA_PHY_LINK			(0x07*(FPGA_BIT_WIDTH/8))

u_int read_transceiver_mux_channel_byterate(int channel);
u_int read_tsin_channel_byterate(int in_chan)
{
	u_int byterate;


	byterate = read_transceiver_mux_channel_byterate(in_chan);
	//printf("read %d ts in  byterate %d \n", in_chan, byterate);

	return byterate;
}

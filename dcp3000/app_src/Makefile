#USED_HOST
# 0  arm
# 1  x86

USE_CMDLINE = 0
USE_FRAM = 1
USED_HOST = 0

ifeq ($(USED_HOST) , 1)
#USE_CMDLINE = 0
USE_FRAM = 0
CFLAGS +=  -DDEBUG_IN_X86
else
CROSS_PREFIX = arm-linux-gnueabihf-

CC = $(CROSS_PREFIX)gcc
STRIP = $(CROSS_PREFIX)strip

CFLAGS = -march=armv7 -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a8 

ifeq ($(USED_HOST) , 0)
CFLAGS +=  -DRUN_IN_ARM
CFLAGS +=  -DUSE_NOR_FLASH
CFLAGS +=  -DUSE_TEMP_SENSOR
else
USE_FRAM = 0
endif

endif


CFLAGS +=  -D_GNU_SOURCE -std=c99 -g 
#CFLAGS +=  -I/usr/include/libxml2

ifeq ($(USE_CMDLINE), 1)
CFLAGS +=  -DUSE_CMD_LINE 
endif

ifeq ($(USE_FRAM) , 0)
CFLAGS += -DSAVE_IN_DB
endif



all: app
#all: upgrade
#all: debug

SO_LIBS = 


include libso.mk




#SSLOBJS = $(OPENSSLOBJS)

#SSLLIB = $(OPENSSL)





TRANSFERLIBOBJS = transfer/spi.c transfer/spi_xyf.c   transfer/transceiver.c 

WEBOBJS	= web/webserver.c 
WEBOBJS	+= web/ajax/cJSON.c web/ajax/charset_ch.c  web/ajax/mux_pro_data.c

MUXOBJS	 =  mux/descriptor.c mux/parse.c mux/table.c 	mux/capture.c mux/mux.c mux/webmux.c  mux/mux_extern.c mux/mux_sql.c


EXTERNLIB =    -lxml2 -L. -lfpga -lrtc -lgpio -lsqlite -llm73 -lutils 

BASEOBJS = parameter.c board_info.c   
ifeq ($(USED_HOST) , 0)

EXTERNLIB += -lmtd -lflash -lfram
endif
BASEOBJS +=$(TRANSFERLIBOBJS) sqlite3/sys_sql.c 

APPOBJS = system.c  tsoip/tsoip.c tsoip/tsoip_backup.c $(BASEOBJS) scan_mac.c  ipcmsgsrv.c
APPOBJS += front_panel/lcd_menu.c front_panel/front_panel.c 
APPOBJS += event/eventlog.c event/board_event.c  event/trapmsg.c


DEBUGOBJS = debugmain.c shmmsg_client.c  system.c tsoip/tsoip.c tsoip/tsoip_backup.c  $(BASEOBJS) 
DEBUGOBJS += event/eventlog.c event/board_event.c  event/trapmsg.c 
DEBUGOBJS += debug/debug_chip.c debug/debug_tsoip.c debug/debug_other.c
#DEBUGOBJS +=debug/debug_mux.c 


ifeq ($(USE_CMDLINE), 1)
EXTERNLIB += -lcmd
endif

EXTERNLIB += -lipcmsg -lntp
DEBUGLIB = $(EXTERNLIB) -lcmd

WEBSRVOBJS = webmain.c system.c upgrade.c tsoip/tsoip.c tsoip/tsoip_backup.c shmmsg_client.c
WEBSRVOBJS += $(WEBOBJS) $(BASEOBJS) $(MUXOBJS)  event/eventlog.c event/board_event.c event/trapmsg.c

WEBSRVEXTERNLIB =  -L. -lfpga -lrtc -lsqlite -lutils -lmtd -lflash -lfram -lipcmsg -lntp -lappweb
WEBSRVEXTERNLIB += -llm73 -lgpio -lxml2


APP_EXEC = dcp3000.bin 
UPGRADE_EXEC = upgrade.bin
DEBUG_EXEC = debug_tools.bin
WATCHDOG_EXEC = watchd.bin
WEBSERVER_EXEC = httpd.bin

EXEC = $(APP_EXEC)
EXEC += $(UPGRADE_EXEC)
EXEC  += $(DEBUG_EXEC)
EXEC  += $(WATCHDOG_EXEC)
EXEC  += $(WEBSERVER_EXEC)

app:$(APP_EXEC) 

upgrade:$(UPGRADE_EXEC) 
debug:$(DEBUG_EXEC)
wdg:$(WATCHDOG_EXEC)
web:$(WEBSERVER_EXEC)
lib:libso

libso:$(SO_LIBS)

$(DEBUG_EXEC): $(DEBUGOBJS) $(SO_LIBS)
	$(CC) $(CFLAGS) $(DEBUGOBJS) -DDEBUG_TOOLS_MODE -DUSE_CMD_LINE -DIPC_CLIENT  -lpthread -lrt -lm -ldl $(DEBUGLIB)  -o $@
	$(STRIP) $@

$(UPGRADE_EXEC):upgrade.c  transfer/spi.c transfer/spi_xyf.c event/eventlog.c fpga.c flash/mtd.c flash/flash.c $(IPCMSGLIBOBJS) $(UTILSLIBOBJS)
	$(CC) $(CFLAGS) $^ -DINDEPENDENT_UPGRADE -DSPI_MAIN_PROCESS -lpthread -L. -lrt  -o $@
	$(STRIP) $@

	
$(APP_EXEC): main.c $(APPOBJS) $(SO_LIBS)
	$(CC) $(CFLAGS) main.c $(APPOBJS) -DSPI_MAIN_PROCESS   -lpthread  -lrt -lm -ldl  $(EXTERNLIB)  -o $@

	
$(WEBSERVER_EXEC):  $(WEBSRVOBJS) $(SO_LIBS)
	$(CC) $(CFLAGS)  $(WEBSRVOBJS) -DSUBROUTINE -DIPC_CLIENT -lpthread  -lrt -lm -ldl  $(WEBSRVEXTERNLIB)  -o $@

$(WATCHDOG_EXEC):watchdog.c $(FPGA_LIB) $(IPCMSG_LIB) 
	$(CC) $(CFLAGS) watchdog.c -lpthread  -lrt -lm -ldl -L. -lfpga -lipcmsg -o $@

strip: $(EXEC) 
	$(STRIP) $^


clean:
	-rm -f *.elf *.gdb *.o 


distclean:
	-rm -f $(EXEC)
libclean:
	-rm -f  $(SO_LIBS)

.PHONY:all clean strip app upgrade debug distclean libso lib



SO_LIBS += 

FPGA_LIB = libfpga.so
$(FPGA_LIB):fpga.c fpga.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

FLASH_LIB = libflash.so
$(FLASH_LIB):flash/flash.c flash/flash.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

MTD_LIB = libmtd.so
$(MTD_LIB):flash/mtd.c flash/mtd.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

FRAM_LIB = libfram.so
$(FRAM_LIB):flash/fram.c flash/fram.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

GPIO_LIB = libgpio.so
$(GPIO_LIB):gpio.c gpio.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

RTC_LIB = librtc.so
$(RTC_LIB):rtc.c rtc.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

LM73_LIB = liblm73.so
$(LM73_LIB):lm73.c lm73.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^



UTILSLIBOBJS = utils/system.c utils/utility.c utils/thread_pool.c utils/dataflag.c utils/node.c utils/list.c utils/duList.c \
		utils/queue.c utils/stack.c utils/cirqueue.c  utils/arraylist.c utils/sArrList.c utils/cirArrList.c utils/properties.c \
		utils/hashlib.c utils/hashtable.c utils/sim_encrypt.c utils/convert_code.c

UTILS_LIB = libutils.so
$(UTILS_LIB):$(UTILSLIBOBJS) 
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

READLINEOBJS = cmdline/readline/readline.c cmdline/readline/funmap.c cmdline/readline/keymaps.c \
	   cmdline/readline/vi_mode.c cmdline/readline/parens.c cmdline/readline/rltty.c \
	   cmdline/readline/complete.c cmdline/readline/bind.c cmdline/readline/isearch.c \
	   cmdline/readline/display.c cmdline/readline/signals.c  cmdline/readline/util.c cmdline/readline/kill.c \
	   cmdline/readline/undo.c cmdline/readline/macro.c cmdline/readline/input.c \
	   cmdline/readline/callback.c cmdline/readline/terminal.c cmdline/readline/xmalloc.c \
	   cmdline/readline/history.c cmdline/readline/histsearch.c cmdline/readline/histexpand.c \
	   cmdline/readline/histfile.c cmdline/readline/nls.c cmdline/readline/search.c \
	   cmdline/readline/shell.c cmdline/readline/tilde.c cmdline/readline/savestring.c \
	   cmdline/readline/text.c cmdline/readline/misc.c cmdline/readline/compat.c \
	   cmdline/readline/colors.c cmdline/readline/parse-colors.c \
	   cmdline/readline/mbutil.c cmdline/readline/xfree.c
	  # cmdline/readline/emacs_keymap.c cmdline/readline/vi_keymap.c 

TERMCAPOBJS	=  cmdline/termcap/termcap.c cmdline/termcap/tparam.c

CMDLINEOBJS = $(READLINEOBJS) $(TERMCAPOBJS) cmdline/cmdline.c

CMD_LIB = libcmd.so
$(CMD_LIB):$(CMDLINEOBJS)  utils/dataflag.c utils/node.c utils/list.c utils/queue.c utils/hashlib.c
	$(CC) $(CFLAGS) -DHAVE_CONFIG_H -I./cmdline/ -fPIC -shared -o $@ $^
	

NTPOBJS = ntp/ntpclient.c ntp/phaselock.c 

NTP_LIB = libntp.so
$(NTP_LIB):$(NTPOBJS)  ntp/ntpclient.h
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^



IPCMSGLIBOBJS = ipcmsg/system_v_shmem.c ipcmsg/posix_shmem.c  ipcmsg/shmem_obj.c \
		 ipcmsg/shmem_msg.c  ipcmsg/ipcmsg.c ipcmsg/ipcmsg_query_req.c 

IPCMSG_LIB = libipcmsg.so
$(IPCMSG_LIB):$(IPCMSGLIBOBJS)  utils/thread_pool.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

SQLITELIBOBJS = sqlite3/sqlite3.c sqlite3/app_sql.c 
#SQLITELIBOBJS += sqlite3/mysql.c
SQLITE_LIB = libsqlite.so 
$(SQLITE_LIB):$(SQLITELIBOBJS)  
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^



SO_LIBS += $(FPGA_LIB) $(RTC_LIB) $(GPIO_LIB)  $(LM73_LIB)
SO_LIBS += $(MTD_LIB) $(FLASH_LIB) $(FRAM_LIB) 
SO_LIBS += $(SQLITE_LIB)
SO_LIBS += $(UTILS_LIB)
SO_LIBS += $(CMD_LIB)
SO_LIBS += $(IPCMSG_LIB)
SO_LIBS += $(NTP_LIB)


include appweb.mk
SO_LIBS += $(APPWEB_LIB) 


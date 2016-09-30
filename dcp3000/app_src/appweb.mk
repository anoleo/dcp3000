#
#   appweb-linux-static.mk -- Makefile to  Embedthis Appweb for linux
#



APPWEB_SRC_DIR                 ?= ./appweb

ME_COM_COMPILER       ?= 1





CFLAGS                += -g -w
DFLAGS                += -DME_DEBUG=0 -DME_COM_COMPILER=$(ME_COM_COMPILER) 

IFLAGS                += 
LDFLAGS               += 

LIBS                  += -lrt -ldl -lpthread -lm
#LIBS                  += -L/usr/local/ssl/lib -lssl -lcrypto

#EMDFLAGS	?= -DMBEDTLS_USER_CONFIG_FILE=\"embedtls.h\"



#
#   appweb.o
#
DEPS_13 += $(APPWEB_SRC_DIR)/appweb.h
#DEPS_13 += $(APPWEB_SRC_DIR)/server.c

$(APPWEB_SRC_DIR)/appweb.o: \
    $(APPWEB_SRC_DIR)/appweb.c $(DEPS_13)
	$(CC) -c -o $(APPWEB_SRC_DIR)/appweb.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/appweb.c


#
#   cgiHandler.o
#
DEPS_16 += $(APPWEB_SRC_DIR)/appweb.h

$(APPWEB_SRC_DIR)/cgiHandler.o: \
    $(APPWEB_SRC_DIR)/cgiHandler.c $(DEPS_16)
	$(CC) -c -o $(APPWEB_SRC_DIR)/cgiHandler.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/cgiHandler.c

#
#   cgiProgram.o
#

$(APPWEB_SRC_DIR)/cgiProgram.o: \
    $(APPWEB_SRC_DIR)/cgiProgram.c $(DEPS_17)
	$(CC) -c -o $(APPWEB_SRC_DIR)/cgiProgram.o $(LDFLAGS) $(DFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/cgiProgram.c

#
#   config.o
#
DEPS_18 += $(APPWEB_SRC_DIR)/appweb.h
DEPS_18 += $(APPWEB_SRC_DIR)/pcre.h

$(APPWEB_SRC_DIR)/config.o: \
    $(APPWEB_SRC_DIR)/config.c $(DEPS_18)
	$(CC) -c -o $(APPWEB_SRC_DIR)/config.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/config.c

#
#   convenience.o
#
DEPS_19 += $(APPWEB_SRC_DIR)/appweb.h

$(APPWEB_SRC_DIR)/convenience.o: \
    $(APPWEB_SRC_DIR)/convenience.c $(DEPS_19)
	$(CC) -c -o $(APPWEB_SRC_DIR)/convenience.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/convenience.c



#
#   espHandler.o
#
DEPS_22 += $(APPWEB_SRC_DIR)/appweb.h
DEPS_22 += $(APPWEB_SRC_DIR)/esp.h

$(APPWEB_SRC_DIR)/espHandler.o: \
    $(APPWEB_SRC_DIR)/espHandler.c $(DEPS_22)
	$(CC) -c -o $(APPWEB_SRC_DIR)/espHandler.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/espHandler.c

#
#   espLib.o
#
DEPS_23 += $(APPWEB_SRC_DIR)/esp.h
DEPS_23 += $(APPWEB_SRC_DIR)/pcre.h
DEPS_23 += $(APPWEB_SRC_DIR)/http.h

$(APPWEB_SRC_DIR)/espLib.o: \
    $(APPWEB_SRC_DIR)/espLib.c $(DEPS_23)
	$(CC) -c -o $(APPWEB_SRC_DIR)/espLib.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/espLib.c

#
#   http.h
#

$(APPWEB_SRC_DIR)/http.h: $(DEPS_24)

#
#   http.o
#
DEPS_25 += $(APPWEB_SRC_DIR)/http.h

$(APPWEB_SRC_DIR)/http.o: \
    $(APPWEB_SRC_DIR)/http.c $(DEPS_25)
	$(CC) -c -o $(APPWEB_SRC_DIR)/http.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/http.c

#
#   httpLib.o
#
DEPS_26 += $(APPWEB_SRC_DIR)/http.h
DEPS_26 += $(APPWEB_SRC_DIR)/pcre.h

$(APPWEB_SRC_DIR)/httpLib.o: \
    $(APPWEB_SRC_DIR)/httpLib.c $(DEPS_26)
	$(CC) -c -o $(APPWEB_SRC_DIR)/httpLib.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/httpLib.c


#
#   mbedtls.o
#
DEPS_29 += $(APPWEB_SRC_DIR)/mbedtls.h

$(APPWEB_SRC_DIR)/mbedtls.o: \
    $(APPWEB_SRC_DIR)/mbedtls.c $(DEPS_29)
	$(CC) -c -o $(APPWEB_SRC_DIR)/mbedtls.o $(LDFLAGS) $(DFLAGS) $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/mbedtls.c

#
#   mpr-mbedtls.o
#
DEPS_30 += $(APPWEB_SRC_DIR)/mpr.h

$(APPWEB_SRC_DIR)/mpr-mbedtls.o: \
    $(APPWEB_SRC_DIR)/mpr-mbedtls.c $(DEPS_30)
	$(CC) -c -o $(APPWEB_SRC_DIR)/mpr-mbedtls.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/mpr-mbedtls.c



#
#   mpr-version.o
#
DEPS_33 += $(APPWEB_SRC_DIR)/mpr-version.h
DEPS_33 += $(APPWEB_SRC_DIR)/pcre.h

$(APPWEB_SRC_DIR)/mpr-version.o: \
    $(APPWEB_SRC_DIR)/mpr-version.c $(DEPS_33)
	$(CC) -c -o $(APPWEB_SRC_DIR)/mpr-version.o $(LDFLAGS) $(DFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/mpr-version.c


#
#   mprLib.o
#
DEPS_35 += $(APPWEB_SRC_DIR)/mpr.h

$(APPWEB_SRC_DIR)/mprLib.o: \
    $(APPWEB_SRC_DIR)/mprLib.c $(DEPS_35)
	$(CC) -c -o $(APPWEB_SRC_DIR)/mprLib.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/mprLib.c


#
#   pcre.o
#
DEPS_37 += $(APPWEB_SRC_DIR)/me.h
DEPS_37 += $(APPWEB_SRC_DIR)/pcre.h

$(APPWEB_SRC_DIR)/pcre.o: \
    $(APPWEB_SRC_DIR)/pcre.c $(DEPS_37)
	$(CC) -c -o $(APPWEB_SRC_DIR)/pcre.o $(LDFLAGS) $(DFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/pcre.c

#
#   rom.o
#
DEPS_38 += $(APPWEB_SRC_DIR)/mpr.h

$(APPWEB_SRC_DIR)/rom.o: \
    $(APPWEB_SRC_DIR)/rom.c $(DEPS_38)
	$(CC) -c -o $(APPWEB_SRC_DIR)/rom.o $(LDFLAGS) $(DFLAGS) -D_FILE_OFFSET_BITS=64 $(EMDFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/rom.c

#
#   mpr-openssl.o
#
DEPS_39 += $(APPWEB_SRC_DIR)/mpr.h

$(APPWEB_SRC_DIR)/mpr-openssl.o: \
    $(APPWEB_SRC_DIR)/mpr-openssl.c $(DEPS_39)
	$(CC) -c -o $(APPWEB_SRC_DIR)/mpr-openssl.o -I/usr/local/ssl/include $(LIBS) $(LDFLAGS) $(DFLAGS) $(IFLAGS) $(APPWEB_SRC_DIR)/mpr-openssl.c 



#APPWEB_OBJ += $(APPWEB_SRC_DIR)/mbedtls.c
#APPWEB_OBJ += $(APPWEB_SRC_DIR)/mpr-mbedtls.c

APPWEB_OBJ += $(APPWEB_SRC_DIR)/mprLib.c

APPWEB_OBJ += $(APPWEB_SRC_DIR)/pcre.c

APPWEB_OBJ += $(APPWEB_SRC_DIR)/httpLib.c

APPWEB_OBJ += $(APPWEB_SRC_DIR)/mpr-version.c



APPWEB_OBJ += $(APPWEB_SRC_DIR)/config.c
APPWEB_OBJ += $(APPWEB_SRC_DIR)/convenience.c
#APPWEB_OBJ += $(APPWEB_SRC_DIR)/rom.c
#APPWEB_OBJ += $(APPWEB_SRC_DIR)/espLib.c
#APPWEB_OBJ += $(APPWEB_SRC_DIR)/cgiHandler.c
#APPWEB_OBJ += $(APPWEB_SRC_DIR)/espHandler.c


APPWEB_OBJ += $(APPWEB_SRC_DIR)/appweb.c

APPWEB_LIB = libappweb.so
$(APPWEB_LIB): $(APPWEB_OBJ)
	$(CC) $(CFLAGS) $(DFLAGS) -fPIC -shared -o $@ $^
	


#appweb.bin: $(APPWEB_OBJ) web/webserver.o
#	$(CC) -o $@ $(LDFLAGS)  $^ $(LIBS) 


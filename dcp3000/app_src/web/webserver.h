
#ifndef _WEB_SERVER_H_
#define _WEB_SERVER_H_

#include "../appweb/appweb.h"


void stop_webserver(void);

void start_webserver(char *configFile);


int set_web_user(HttpConn *conn, char *user, char *passwd);

int debug_web(int argc, char *argv[]);

#endif


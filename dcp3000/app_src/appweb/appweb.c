/**
    appweb.c  -- AppWeb main program

    Copyright (c) All Rights Reserved. See copyright notice at the bottom of the file.

 */


#define MAXTHREADS		4

/********************************* Includes ***********************************/

#include    "appweb.h"



/********************************** Locals ************************************/

static char *appweb_username = NULL;
static char *appweb_password = NULL;
static int maxThreads = MAXTHREADS;
static int debugMode = 0;
static char *logSpec = NULL; // "stdout:2";
static char *traceSpec = NULL; // "stdout:2";

void appweb_account(char *username, char *password)
{
	appweb_username = username;
	appweb_password = password;
}

void appweb_maxThreads(int threadqty)
{
	maxThreads = threadqty;
}


void appweb_debug(int mode)
{
	debugMode = mode;
}

void appweb_loglevel(char *loglevel)
{
	logSpec = loglevel;
}

void appweb_tracelevel(char *tracelevel)
{
	traceSpec = tracelevel;
}

char *makeAuthPassword(char *cipher, char *username, char *password, char *realm)
{
	char        *encodedPassword;

	if (smatch(cipher, "md5")) {
		encodedPassword = mprGetMD5(sfmt("%s:%s:%s", username, realm, password));
	} else {
		/* This uses the more secure blowfish cipher */
		encodedPassword = mprMakePassword(sfmt("%s:%s:%s", username, realm, password), 16, 128);
	}

	return encodedPassword;
}


int authpass(char *authFile, char *username, char *password, char *roles)
{

    MprBuf      *buf;
    HttpRoute   *route;
    HttpAuth    *auth;
    char        *encodedPassword, *realm = "", *cipher;

	cipher = "blowfish";	// Use "md5" or "blowfish"
	//realm = "example.com";

    route = httpGetDefaultRoute(NULL);
    auth = route->auth;


        encodedPassword = makeAuthPassword(cipher, username, password, realm);


	printf("authpass : %s %s %s %s  cipher[%s] %s %s \n", authFile, username, password, roles, cipher, realm, encodedPassword);
	
        httpRemoveUser(auth, username);
        if (httpAddUser(auth, username, encodedPassword, roles) == 0) {
		return (-8);
        }
		
	if (authFile) {
		if (maWriteAuthFile(auth, authFile) < 0) {
			return (-9);
		}
	} else {
		printf("%s\n", encodedPassword);
	}

    return 0;
}



/*********************************** Code *************************************/

 int appweb(char *configFile)
{
	Mpr	*mpr;
	int status;
	int maxThreads = maxThreads;


	
	   
    if ((mpr = mprCreate(0, NULL, MPR_USER_EVENTS_THREAD)) == 0) {
        printf("Cannot create runtime\n");
        return -1;
    }
    if (httpCreate(HTTP_CLIENT_SIDE | HTTP_SERVER_SIDE) == 0) {
        printf("Cannot create the HTTP services\n");
        return -2;
    }
	if(debugMode){
		mprSetDebugMode(1);
	}

	//int             traceLevel;             /**< Current request trace level */
 	//int             startLevel;             /**< Start endpoint trace level */
 	//httpSetInfoLevel(0);

	if(maxThreads>0){
      		mprSetMaxWorkers(maxThreads);
	}

	
    if (logSpec) {
        mprStartLogging(logSpec, MPR_LOG_CONFIG | MPR_LOG_CMDLINE);
    }
	
    if (traceSpec) {
        httpStartTracing(traceSpec);
    }
	
    if (mprStart() < 0) {
        printf("error appweb Cannot start MPR\n");
        mprDestroy();
        return MPR_ERR_CANT_INITIALIZE;
    }

    if (maLoadModules() < 0) {
        return MPR_ERR_CANT_INITIALIZE;
    }
	
    if (maParseConfig(configFile) < 0) {
        printf("Cannot parse the config file %s\n", configFile);
        return -8;
    }

	authpass(NULL, appweb_username, appweb_password, "manage");
 

#if ME_STATIC && ME_COM_ESP
    /*
        Invoke ESP initializers here
     */
	appweb_espAction_defines(httpGetDefaultRoute(NULL));

#endif

	appweb_httpAction_defines();

	if (httpStartEndpoints() < 0) {
		printf("error appweb Cannot listen on HTTP endpoints, exiting.\n");
		return (-9);
	}

	
	//printf("MPR->name : %s, appPath : %s, appDir : %s \n", MPR->name, MPR->appPath, MPR->appDir);

	mprServiceEvents(-1, 0);

	
	return status;
}



/*
    @copy   default

    Copyright (c) Embedthis Software. All Rights Reserved.

    This software is distributed under commercial and open source licenses.
    You may use the Embedthis Open Source license or you may acquire a
    commercial license from Embedthis Software. You agree to be fully bound
    by the terms of either license. Consult the LICENSE.md distributed with
    this software for full details and other copyrights.

    Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */

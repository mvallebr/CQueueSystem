#ifndef __ARS_IF_GINSC_ARSMAIN_H__
#define __ARS_IF_GINSC_ARSMAIN_H__

#include <libcsoap/soap-client.h>
#include <config_mgr.h>
#include <logmgr.h>
#include <libxml/xpath.h>

typedef struct _arsmain {
	int running; /*Indicates if the program is running*/
	xmlNodePtr body;
	configmgr cfg;
	SoapEnv *env, *res;	
} arsmain;

arsmain *arsmain_open(int argc, char *argv[]);
void arsmain_close(arsmain *am);
void arsmain_take_signal(int signal);
int    arsmain_become_daemon(arsmain *am);

#endif /* __ARS_IF_GINSC_ARSMAIN_H__ */
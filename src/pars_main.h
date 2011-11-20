#ifndef __ARS_IF_GINSC_ARSMAIN_H__
#define __ARS_IF_GINSC_ARSMAIN_H__

#include <libcsoap/soap-client.h>
#include <config_mgr.h>
#include <logmgr.h>
#include <notifier.h>
#include <libxml/xpath.h>
#include <primitive_list.h>

typedef struct _arsmain {	
	configmgr *cfg;
	SoapEnv *env, *res;	
	notifier *n;
} arsmain;

arsmain *arsmain_open(int argc, char *argv[]);
void arsmain_close(arsmain *am);
void arsmain_take_signal(int signal);
int    arsmain_become_daemon(arsmain *am);
int arsmain_process_ginsc_queue(arsmain *am);

#endif /* __ARS_IF_GINSC_ARSMAIN_H__ */
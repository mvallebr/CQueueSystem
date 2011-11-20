#ifndef __DDR_GINSC_F1_PROCESSOR_H__
#define __DDR_GINSC_F1_PROCESSOR_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <notification.h>

#include <ars.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <config_mgr.h>

#define PBUFFERSIZE 10240

typedef struct _processor {
	char buffer[PBUFFERSIZE];
	notification notif;
} processor;

processor *processor_init(void);
int processor_destroy(processor *p);
int processor_run(processor *p, char *newbuffer);

#endif /* __DDR_GINSC_F1_PROCESSOR_H__*/

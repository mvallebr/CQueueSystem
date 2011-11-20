#ifndef __DDR_GINSC_F1_NOTIFIER_H__
#define __DDR_GINSC_F1_NOTIFIER_H__

#include <nt.h>
#include <nterrno.h>
#include <ntsextrn.h>

#include <log.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*#include <SocketIF.h>*/
#include <socket_mgr.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>

typedef struct _notifier {
   int fd; /*socket file descriptor*/
   NTStatusList status;
   NTPortAddr port;
	socketmgr *main_socket;
} notifier;

notifier *notifier_init(void);
int notifier_connect(notifier *n);
int notifier_disconnect(notifier *n);
int notifier_receiveData(notifier *n);
int notifier_waitConnection(notifier *n);
void notifier_printStatusList(notifier *n, NTStatusList status);

#endif /* __DDR_GINSC_F1_NOTIFIER_H__*/

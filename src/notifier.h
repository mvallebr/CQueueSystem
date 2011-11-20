#ifndef __ARS_IF_GINSC_NOTIFIER_H__
#define __ARS_IF_GINSC_NOTIFIER_H__

#include <nt.h>
#include <nterrno.h>
#include <ntsextrn.h>

#include <logmgr.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*#include <SocketIF.h>*/
#include <socket_mgr.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>

#define NAMESIZE 1024
#define MAX_SERVERS 15 /* Maximum number of servers to connect*/

typedef struct _notifServer {
	char host[NAMESIZE];
	char user[NAMESIZE];
	char password[NAMESIZE];
	int connected;
} notifServer;

typedef struct _notifServerList {
	notifServer server[MAX_SERVERS];
	int num_servers;
} notifServerList;

typedef struct _notifier {
	int fd; /*socket file descriptor*/
   	NTStatusList status;
   	NTPortAddr port;
   	socketmgr *main_socket;
   	notifServerList server_list;
} notifier;

notifier *notifier_init(void);
int notifier_connect(notifier *n);
int notifier_disconnect(notifier *n);
int notifier_receiveData(notifier *n);
int notifier_waitConnection(notifier *n);
void notifier_printStatusList(notifier *n, NTStatusList status);

#endif /* __ARS_IF_GINSC_NOTIFIER_H__*/

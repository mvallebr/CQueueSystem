#ifndef __DDR_GINSC_F1_SOCKET_MGR_H__
#define __DDR_GINSC_F1_SOCKET_MGR_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_PENDING 10

/*Socket manager, uses TCP sockets*/
typedef struct _socketmgr {
	int fd;
	int port;   /*port used by socket*/
	int client; /*0 = server, 1 = client*/
	
	struct sockaddr_in sin;
	struct hostent *hp;   
} socketmgr;

socketmgr *socketmgr_init(void);
void socketmgr_destroy(socketmgr *s);
int socketmgr_bindPort(socketmgr *s, int port);
int socketmgr_listenPort(socketmgr *s, int port);
int socketmgr_waitConnection(socketmgr *s);
int socketmgr_connect(socketmgr *s, char *host, int port);

#endif /*__DDR_GINSC_F1_SOCKET_MGR_H__*/
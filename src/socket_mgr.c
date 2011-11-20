#include <socket_mgr.h>
#include <stdlib.h>
#include <logmgr.h>

/*Usado para compatibilidade com unix*/
#ifndef _SYS_SOCKET_H
#define socklen_t int
#endif

socketmgr *socketmgr_init(void) {
	socketmgr *s;	
	int fd;
	s = (socketmgr *) malloc(sizeof(socketmgr));
	s->client = -1;
	s->port = 0;

	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!fd) {
		s->fd = 0;
		return s;
	}
	printf("SOCKET: socket descriptior: %d\n\n", fd);
	s->fd = fd;
	return s;
}

void socketmgr_destroy(socketmgr *s) {
	
	free(s);
}

int socketmgr_bindPort(socketmgr *s, int port) {
	int len;
	s->client = 0;
	if (s->fd==0) {
		fprintf(stderr, "SOCKET: Error: socket not connected!\n");
		return 0;
	}
	/*if (s->fd==0) {
		fprintf(stderr, "Error: socket can listen at port 0!!\n");
		return 0;
	}*/
	
	/*Preenche estrutura de endereço*/
	memset((void *)&(s->sin), 0, sizeof(s->sin));
   s->sin.sin_family = AF_INET;
   s->sin.sin_addr.s_addr = INADDR_ANY;
	s->sin.sin_port = htons(port);
	len = sizeof(s->sin);

	/*Bind the address*/
	if (bind(s->fd, (struct sockaddr *)&(s->sin), sizeof(s->sin)) < 0) {
		fprintf(stderr, "SOCKET: socket error: bind - %s\n", strerror(errno));
		return 0;
   }
	
	
	return 1;
}

int socketmgr_listenPort(socketmgr *s, int port) {
	int len;	
	s->client = 0;
	if (s->fd==0) {
		fprintf(stderr, "SOCKET: Error: socket not connected!\n");
		return 0;
	}	
	
	len = sizeof(s->sin);
	
		
	/*Length of the listen queue*/
	if (listen(s->fd, MAX_PENDING) < 0) {
		fprintf(stderr, "SOCKET: socket error: listen - %s\n", strerror(errno));
		return 0;
   }
	printf("SOCKET: porta = %d \n\n", s->sin.sin_port);
	if (getsockname (s->fd, (struct sockaddr *) &(s->sin), (socklen_t *) (&len))!=0) {
		fprintf (stderr, "SOCKET: getsockname, errno %d \n", errno);
	} else {
		s->port = ntohs(s->sin.sin_port);
	}
	
	return 1;
}
int socketmgr_waitConnection(socketmgr *s) {
	int len;
	int new_fd;	
	
	len = sizeof(s->sin);
	
	s->client = 0;
	if (s->fd==0) {
		fprintf(stderr, "SOCKET: Error: socket not connected!\n");
		return 0;
	}
	
	printf("SOCKET: Waiting for connections on port %d ...\n\n", s->port);
	if ((new_fd = accept(s->fd, (struct sockaddr *)&(s->sin), (socklen_t *)  (socklen_t *) (&len))) < 0) {
		fprintf(stderr, "SOCKET: socket error: accept - %s\n", strerror(errno));
		return 0;
   }
	printf("Connection accepted by socket %d, created socket %d to exchange data\n", s->fd, new_fd);
	return new_fd;
}

int socketmgr_connect(socketmgr *s, char *host, int port){
	s->client = 1;
	if (s->fd==0) {
		fprintf(stderr, "Error: socket not connected!\n");
		return 0;
	}
	/*Host*/
	if ((s->hp = gethostbyname(host)) == NULL) {
   	fprintf(stderr, "socket connect error: unknown host '%s'\n", host);
   	return 0;
   }   
	/*Inicializa estrutura de endereço*/
   memset((void *)&(s->sin), 0, sizeof(s->sin));
   s->sin.sin_family = AF_INET;
   memcpy((void *)&(s->sin.sin_addr), (s->hp->h_addr), (s->hp->h_length));
   s->sin.sin_port = htons(port);
	
	if (connect(s->fd, (struct sockaddr *)&(s->sin), sizeof(s->sin)) < 0) {
   	perror("connect");
   	return 0;
   }
	return 1;
}




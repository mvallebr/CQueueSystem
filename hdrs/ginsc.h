#ifndef __DDR_GINSC_F1_GINSC_H__
#define __DDR_GINSC_F1_GINSC_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ar.h>
#include <notification.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <socket_mgr.h>

/*interface odbc com ginsc*/
typedef struct _ginsc {
	int tipo_abertura;
	int fd;
	socketmgr *gsocket; 
} ginsc;

ginsc *ginsc_init(void);
int ginsc_destroy(ginsc *g);
int ginsc_connect(ginsc *g);
int ginsc_disconnect(ginsc *g);
int ginsc_sendData(ginsc *g, ARFieldValueList *valores, notification n);
char *ginsc_timestampToDate(long timestamp);
char * ginsc_timeToDate( long timestamp );
int ginsc_socketSendData(ginsc *g, char valores[11][50], int numCampos,  char *timeStamp);
int ginsc_run_queue(ginsc *g);

#endif /*__DDR_GINSC_F1_GINSC_H__*/

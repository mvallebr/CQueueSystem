#ifndef __ARS_IF_GINSC_GINSC_H__
#define __ARS_IF_GINSC_GINSC_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ar.h>
#include <notification.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <socket_mgr.h>
#include <primitive.h>

/*interface odbc com ginsc*/
typedef struct _ginsc {
	char url[10240];
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
int ginsc_enqueueData(ginsc *g, primitive *p, const char *queue_folder);
int ginsc_processQueue(ginsc *g, const char *pginsc_output_folder, const char *pginsc_error_folder, 
	const char *ars_output_folder);
int ginsc_processARSQueue(ginsc *g);

#endif /*__ARS_IF_GINSC_GINSC_H__*/

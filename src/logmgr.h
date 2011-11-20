#ifndef __ARS_IF_GINSC_LOGMGR_H__
#define __ARS_IF_GINSC_LOGMGR_H__

#include <stdio.h>

typedef struct _logmgr {	
	FILE *fd;                       /*The log file descriptor */
	char filename[10240];  /*The log filename*/
	int write_pid;                 /* Indicates if the PID of the process should be printed before log messages */
	int write_to_stderr;       /*Indicates if the output should go also to the standard error output */	
} logmgr; 

logmgr logmgr_getglobal(void);
/* if you pass NULL to the log argument of those methods, the global_log is used */
int logmgr_open(logmgr *l);
int logmgr_write(logmgr *l, const char *msg);
int logmgr_writef(logmgr *l, const char *msg, ...);
int logmgr_close(logmgr *l);


#endif /*__ARS_IF_GINSC_LOGMGR_H__*/

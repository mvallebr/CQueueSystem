#include <logmgr.h>
#include <config_mgr.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
/*Usado para compatibilidade com unix*/
/*#ifndef _SYS_SOCKET_H*/
/*#include <sys/varargs.h>*/
/*#endif*/

logmgr global_log;

logmgr logmgr_getglobal(void) {
	return global_log;
}

int logmgr_open(logmgr *l) {
	configmgr *cfg;	
	if (l==NULL) l = &global_log;
	l->fd = NULL;
	strcpy(l->filename, "");
	l->write_pid=1;
	l->write_to_stderr=1;
	
	cfg=configmgr_getglobal();
	if (!configmgr_getfirst_string(cfg, l->filename, "//config/general_log_filename")) {
		fprintf(stderr, "LOG: Can't read log filename from config file!\n");
		return 0;
	}
	l->fd = fopen(l->filename, "a"); /* Open file for Append */
	if  (l->fd==NULL) {
		fprintf(stderr, "LOG: COULD NOT OPEN LOG FILE!!\n");
		return 0;
	}
	return 1;
}

int logmgr_write(logmgr *l, const char *msg) {
	char newmsg[10240];
	if (l==NULL) l = &global_log;
	/* Write PID */
	if (l->write_pid)
		sprintf(newmsg, "PID %d >>>$ %s\n", getpid(), msg);
	else
		sprintf(newmsg, "%s\n", msg);
	/* Write to stderr*/
	if (l->write_to_stderr) {
		fprintf(stderr, newmsg);
		fflush(stderr);
	}
	/* Write to log file*/
	fprintf(l->fd, newmsg);
	fflush(l->fd);
	
	return 1;
}

int logmgr_writef(logmgr *l, const char *msg, ...) {
	va_list ap;
	char buffer[10240];
	
	if (l==NULL) l = &global_log;	
  
 	va_start(ap, msg);
	vsprintf(buffer, msg, ap);
	va_end(ap);

  return logmgr_write(l, buffer);
}

int logmgr_close(logmgr *l) {
	if (l==NULL) l = &global_log;
	if (!l->fd) {
		fprintf(stderr, "LOG FILE NOT OPENED!\n");
		return 0;
	}
	
	fclose(l->fd);
	l->fd = NULL;
	return 1;
}

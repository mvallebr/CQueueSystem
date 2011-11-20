#ifndef __ARS_IF_GINSC_PROCESSOR_H__
#define __ARS_IF_GINSC_PROCESSOR_H__

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
	char pginsc_output_folder[1024], pginsc_error_folder[1024];
	char ars_output_folder[1024], ars_error_folder[1024];
} processor;

processor *processor_init(void);
int processor_destroy(processor *p);
int processor_run(processor *p, char *newbuffer);

/* It is like a static function, can be called without an instance 
 Verify if primitive should be moved to destpath folder, because other primitives of same id exists in that folder */
int processor_move_to_dest_if_needed(const char *primitivefile, const char *sourcepath, const char *destpath);
int processor_move_to_dest(const char *primitivefile, const char *sourcepath, const char *destpath);
/* Verify if a primitive starting with entry exists on folder */
int processor_entryid_exists(const char *entryid, int entryidlen, const char *folder);
int processor_enqueueData(primitive *p, const char *queue_folder);
int processor_lock_exists(const char *folder);
int processor_wait_for_no_lock(const char *folder);
int processor_write_lock(const char *folder);
int processor_delete_lock(const char *folder);
int processor_get_lock_filename(char *filename, const char *folder);

#endif /* __ARS_IF_GINSC_PROCESSOR_H__*/

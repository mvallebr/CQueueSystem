#ifndef __ARS_IF_GINSC_NOTIFICATION_H__
#define __ARS_IF_GINSC_NOTIFICATION_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <ar.h>
#include <arstruct.h>
#include <arextern.h>

#include <nt.h>
#include <nterrno.h>
#include <ntsextrn.h>
#include <primitive.h>

#define HOSTNAME  "telesp.hi.inet"
#define USER      "GINSC"
#define PASSWORD  "GINSC"

#define DEFAULT_SERVICE_NAME "ArsDdrNotifBridge"

#define BUFFERSIZE 10240
#define RECV_TIMEOUT 1000 /*in miliseconds*/

enum NOTIFICATION_TYPES  {
	NOTIFICATION_CREATE=10,
	NOTIFICATION_MODIFY=11
};

typedef struct _notification {
	long timestamp;
	int code;
	AREntryIdList idlist;
	char EntryID[AR_MAX_ENTRYID_SIZE+1];
	char Schema[AR_MAX_NAME_SIZE+1];
	char servername[AR_MAX_SERVER_SIZE+1];
} notification;

char *notification_strtok(notification *notif, char *ptr, const char delimiter);
int notification_parse(notification *notif, char *buffer);
notification *notification_new_from_primitive(primitive *p);

#endif /* __ARS_IF_GINSC_NOTIFICATION_H__*/
